# 边界知识：模块依赖规则、调用链边界、状态流转约束

← 返回 [`AGENTS.md`](../../AGENTS.md)

> 这是**动手改代码前必读**的文档。本仓常见的返工原因是越过了这里的某一条边界。

---

## 分层依赖规则

允许的依赖方向是**单向向下**，禁止反向与跨层：

```text
frameworks/js|ets|cj   ──▶ frameworks/native ──▶ (IPC) ──▶ services/call_manager_service
                                                                      │
                                                                      ▼
                                                              services/call  ◀── 其他 services/* 订阅
                                                                      │
                                                                      ▼
                                                        services/telephony_interaction ──▶ (IPC) 外部部件
```

| 规则 | 说明 |
|---|---|
| `interfaces/innerkits/` 不依赖任何实现 | 它是纯类型与枚举，被所有层引用。**往这里加实现代码是错的。** |
| `frameworks/native/` 只做序列化 | 不做业务判断、不做鉴权。客户端侧的参数校验只允许做「明显非法」拦截（空指针、长度越界）。 |
| `services/call_manager_service/` 不做业务 | Stub 只反序列化+分发；Service 只鉴权+转发给 `CallControlManager`。 |
| `services/call/` 是业务中心 | 只有它能操作全局通话对象表。 |
| 其他 `services/*` 是**订阅者** | 通过 `CallStateListenerBase` 被动接收状态，**不得反向直接改通话对象状态**。需要影响主流程时，调 `CallControlManager` 的公开方法。 |
| `services/telephony_interaction/` 是**唯一对外出入口** | 对 cellular_call / core_service / voip 的调用只能从这里发出。其他模块直连外部部件是错的。 |
| `utils/` 不依赖 `services/` | 单向被引用。 |

**违反示例（真实高频）**：在 `services/audio/` 里直接调 `CellularCallConnection` 去挂断通话。正确做法是调 `CallControlManager::HangUpCall()`，让它走策略与异步队列。

---

## 新增 IPC 接口的六步清单

顺序执行，**漏任何一步都不会编译报错，而是运行期静默失败**（未注册的 code 走 default 分支返回错误码）。

| # | 文件 | 动作 | 漏掉的后果 |
|---|---|---|---|
| 1 | `interfaces/innerkits/call_manager_service_ipc_interface_code.h` | 在**枚举末尾追加**新 code | — |
| 2 | `frameworks/native/include/i_call_manager_service.h` | 加纯虚函数声明 | 编译报错（唯一会报错的一步） |
| 3 | `frameworks/native/src/call_manager_service_proxy.cpp` + `.h` | 实现 proxy：写 `WriteInterfaceToken` → 写参数 → `SendRequest(code)` → 读 reply | 客户端调不通 |
| 4 | `services/call_manager_service/src/call_manager_service_stub.cpp` + `.h` | ① 实现 `OnXxx(data, reply)`；② **在某个 `InitXxxRequest()` 里注册到 `memberFuncMap_`** | **注册漏了 → 运行期静默失败** |
| 5 | `services/call_manager_service/src/call_manager_service.cpp` + `.h` | 实现业务入口，**开头做权限校验** | 越权漏洞 |
| 6 | `frameworks/native/src/call_manager_proxy.cpp` + `call_manager_client.cpp` | 暴露给应用侧 SDK | 应用调不到 |

**第 4 步的注册必须与序列化严格对称**：proxy 写入顺序与 stub 读取顺序、类型、个数完全一致。`MessageParcel` 不做类型校验，写 `int32` 读 `int64` 会读出垃圾且不报错。

**鉴权收口在第 5 步**，权限常量见 `AGENTS.md`「词汇触发路由（读到这些词就查这里）」与下表：

| 动作 | 需要的权限 |
|---|---|
| 拨号 | `ohos.permission.PLACE_CALL` |
| 接听/拒接/挂断 | `ohos.permission.ANSWER_CALL` 或 `SET_TELEPHONY_STATE` |
| **拒接并发送短消息** | 在上述基础上**追加** `ohos.permission.SEND_MESSAGES` |
| 查询通话状态 | `ohos.permission.GET_TELEPHONY_STATE` |
| 设置类（呼转/呼限/IMS 开关） | `ohos.permission.SET_TELEPHONY_STATE` |
| 查询呼转信息 | `SET_TELEPHONY_STATE` 或 `GET_TELEPHONY_STATE` 或 `GET_CALL_TRANSFER_INFO` |
| 通话记录读写 | `READ_CALL_LOG` / `WRITE_CALL_LOG` |
| 跨设备通话管控 | `ohos.permission.MANAGE_CALL_FOR_DEVICES` |

> **隐式约束**：带可选行为的接口，其**可选行为要单独鉴权**。例如 `RejectCall(callId, rejectWithMessage, textMessage)`，只有当 `rejectWithMessage == true` 时才额外要求 `SEND_MESSAGES`——无条件要求会让不发送短消息的正常拒接失败，不要求则构成越权。新增此类接口时按同一模式处理。

---

## 下行链路的调用顺序约束

`CallControlManager::DialCall()` 是所有下行流程的模板，顺序不可调换：

```text
1. NumberLegalityCheck(number)              号码合法性
2. RemoveSeparatorsPhoneNumber(number)      去分隔符，后续一律用清洗后的号码
3. CheckNumberIsEmergency() → isEcc         紧急号码判定，会改写 extras["dialScene"]
4. 卫星/蓝牙等特殊通道判定，改写 extras["callType"]
5. CanDial() → CallPolicy::DialPolicy()     【策略闸门】
6. PackageDialInformation()                 参数落到 dialSrcInfo_
7. CallRequestHandlerPtr_->DialCall()       转异步
```

### 策略必须先于任何副作用

**`CallPolicy::XxxPolicy()` 是唯一的前置校验收口。** 在策略通过之前不得：创建通话对象、改任何标志位、拉起 UI、申请音频焦点、发广播。

原因：策略失败时只有一个 `return ret`，没有回滚逻辑。任何提前产生的副作用都会残留为不一致的状态。

对应关系：

| 下行动作 | 策略函数 |
|---|---|
| `DialCall` | `DialPolicy` |
| `AnswerCall` | `AnswerCallPolicy` |
| `RejectCall` | `RejectCallPolicy` |
| `HangUpCall` | `HangUpPolicy` |
| `HoldCall` / `UnHoldCall` | `HoldCallPolicy` / `UnHoldCallPolicy` |
| `SwitchCall` | `SwitchCallPolicy` |
| 补充业务 | `GetXxxPolicy` / `SetXxxPolicy`（内部都先 `IsValidSlotId`） |

新增下行动作时**必须同时新增对应的 Policy 函数**，哪怕它当前只做 `IsValidSlotId`。

### 紧急呼叫（ECC）是旁路，不是特例分支

`isEcc` 会让流程绕过多项常规校验（多路限制、EDM 管控、隐私模式、飞行模式）。
**在常规路径上加新校验时，必须显式决定 ECC 是否豁免**，并在代码里写清楚。默认继承常规校验会导致「紧急电话拨不出去」这一最高等级事故。

### 拨号参数是单槽位交接

`PackageDialInformation()` 把参数写入 `CallControlManager` 的**成员** `dialSrcInfo_` 与 `extras_`（`mutex_` 保护），异步侧 `CallRequestProcess::DialRequest()` 再通过 `GetDialParaInfo()` 取回。

> **不变量：同一时刻只允许一个「已提交但未消费」的拨号请求。**
> 第二次拨号在第一次被异步消费前到达，会**覆盖**第一次的参数。任何新增的「批量拨号 / 自动重拨 / 拨号前插入耗时操作」都会破坏这个不变量。如果需要并发，必须改成随请求传递参数，而不是复用成员槽位。

---

## 上行状态机约束（`CallStatusManager`）

### 单队列串行

上行全部经 `ReportCallInfoHandler` 投递到 ffrt 队列 `report_call_info_queue`（`ffrt_qos_user_interactive`）。

- 好处：`CallStatusManager` 内部天然串行，不需要为自身状态加锁。
- 代价：**任何一个 Handle 函数阻塞，整条上行链路停摆**。表现为来电不响、挂断后 UI 不退出、状态无法继续迁移。
- 约束：Handle 函数里禁止同步 IPC 等待、禁止 sleep、禁止等锁、禁止同步查 DataShare。需要耗时操作就再投递到别的队列。

其他 ffrt 队列（各自独立，不要互相等待）：

| 队列 | 位置 | 用途 |
|---|---|---|
| `report_call_info_queue` | `report_call_info_handler.cpp` | 上行状态处理 |
| `report_audio_state_change` | `audio_scene_processor.cpp` | 音频场景上报 |
| `call_voice_assistant_manager` | `call_voice_assistant_manager.cpp` | 语音播报 |
| `unregister_callback` | `app_state_observer.cpp` | 应用退出时反注册 |

### 状态迁移必须走 `UpdateCallState()`

**状态迁移**不得直接 `call->SetTelCallState()`。`UpdateCallState()` 负责：写状态 → 同步 `CallRunningState` → 触发 `CallStateListener` 广播 → 维护会议态。绕过它会让所有订阅者失联，表现为「协议栈已经通了但界面没反应」。

> **已知的合法例外（不要误判为违规）**：仓内确实存在若干处直接调用 `SetTelCallState()` 的生产代码，集中在 `call_status_manager.cpp`（`IncomingHandle` 等）与 `call_object_manager.cpp`。
> 判据是**这次写入是「建对象时的初始赋值」还是「已存在对象的状态迁移」**：
> - 通话对象刚创建、尚未进入全局对象表、还没有任何订阅者关心它 → 直接 `SetTelCallState()` 设初值是合法的，此时广播没有意义。
> - 对象已在全局表中、已被广播过 → **必须**走 `UpdateCallState()`。
>
> 新增代码时按这个判据自查；看到既有代码里的直接调用，先确认它属于哪一类，不要当作缺陷在本次改动中一并修改。

### 各 Handle 函数的职责边界

| 函数 | 触发 | 只应做 |
|---|---|---|
| `IncomingHandle` | 新来电 | 创建对象、过滤（拦截/防诈/隐私/OOBE/EDM）、进 `INCOMING` |
| `DialingHandle` | 拨出建立 | 绑定 `dialSrcInfo_`、进 `DIALING` |
| `AlertHandle` | 对端振铃 | 进 `ALERTING` |
| `ActiveHandle` | 通话接通 | 进 `ACTIVE`、DSDA 自动保持仲裁 |
| `HoldingHandle` / `WaitingHandle` | 保持/等待 | 进对应态 |
| `DisconnectingHandle` / `DisconnectedHandle` | 挂断 | 进对应态、**清理该通话相关的全部标志位**、摘除对象 |

> **`DisconnectedHandle` 是标志位清理的唯一可靠时机。** 新增任何跨状态标志位时，必须在这里（或 `DisconnectedHandle` 触发的下游）加清位。见 [`failure-modes.md`](failure-modes.md)「最高频失效模式：粘滞标志位（sticky latch）」。

### `index` / `phoneIndex` 与 `callId` 不可互换

协议栈上报里的 `index` 是协议栈侧序号；`callId` 是本仓 `GetNewCallId()` 自增的。查找对象时用对的那组：

- 按协议栈上报找对象：`GetOneCallObjectByIndex()` / `GetOneCallObjectByIndexAndSlotId()` / `GetOneCallObjectByIndexSlotIdAndCallType()`
- 按应用请求找对象：`GetOneCallObject(callId)`

DSDA 场景下**两张卡的 `index` 会重复**，所以跨卡查找必须用带 `slotId` 的重载。只用 `index` 查找是 DSDA 串话类问题的典型根因。

### 多路通话仲裁横跨两个文件

「DSDA / 多路通话仲裁」不是一个模块，而是**两处协作**。只看一处会漏改：

| 位置 | 文件 | 函数 | 职责 |
|---|---|---|---|
| **仲裁决策**（下行侧） | `services/call/src/call_request_process.cpp` | `HoldOrDisconnectedCall`、`HandleCallWaitingNumZero/One/OneNext/Two`、`HandleDsdaIncomingCall`、`IsDialCallForDsda` | 决定「为了接这一路，已有通话是保持还是挂断」，**直接调 `call->HoldCall()` / `call->HangUpCall()`** |
| **自动动作**（上行侧） | `services/call/src/call_status_manager.cpp` | `AutoAnswer`、`AutoHandleForDsda`、`AutoAnswerForVoiceCall`、`AutoAnswerForVideoCall`、`AutoUnHoldForDsda`、`IsCanUnHold` | 协议栈状态回来后，决定「要不要自动接听 / 自动解保持」 |

两者通过 `CallObjectManager` 的静态标志 `needWaitHold_` 串起来，这是一个**跨文件的粘滞标志位**：

| 动作 | 位置 |
|---|---|
| 置位 | `call_request_process.cpp:929`（`IsDialCallForDsda` 中，且仅在保持命令返回成功时） |
| 消费 | `call_request_process.cpp:901`（`HandleStartDial` 据此挂起待拨号请求） |
| 清位 | `call_request_process.cpp` 三处 + `call_status_manager.cpp` 三处（`HoldingHandle` / `DisconnectedHandle` 等） |

> **改动这条链路时的约束**：`needWaitHold_` 一旦置位，就必须有一条**必然会到来**的状态迁移把它清掉。
> 如果某条路径不再发出保持命令（例如「某类通话不允许被保持」），就必须同步确认：
> 要么不置位，要么改走一条仍会产生状态迁移（如挂断→`DisconnectedHandle`）的路径。
> 否则待拨号请求会永久挂起，表现为「拨号无反应且无任何错误」。

---

## 音频子系统边界

### 三个收口

| 关注点 | 唯一收口 | 不允许的做法 |
|---|---|---|
| 「现在该用哪个设备」 | `AudioControlManager::UpdateDeviceType()` → `UpdateDeviceForForegroundCall()` | 在别处自行判定并 `SetAudioDevice` |
| 「切到某设备」 | `AudioControlManager::SetAudioDevice(device, isByUser)` | 直接调 `AudioProxy` 或 `AudioSystemManager` |
| 「调 audio_framework」 | `AudioProxy` | 其他文件直连 `AudioStandard::*` |

### 用户意图优先于自动策略

`isSetAudioDeviceByUser_`（`AudioControlManager`）记录「当前设备是不是用户手选的」。

这个标志位有**四个必须同时存在的落点**，是本仓「标志位完整性」的标准范例。改音频路由前先按此表核对一遍当前代码：

| 落点 | 位置（master HEAD） | 缺失后果 |
|---|---|---|
| 声明为 atomic | `audio_control_manager.h` `std::atomic<bool> isSetAudioDeviceByUser_` | 数据竞争：写在 IPC 线程（用户操作），读在状态广播线程 |
| 置位 | `SetAudioDevice(device, isByUser)` 用第二参数写入 | 无法区分用户手选与自动路由 |
| 判断 | `UpdateDeviceForForegroundCall()` 中在比较设备前**提前 return** | 用户手选耳机后被自动逻辑改回听筒 |
| 清位 | `CallStateUpdated()` 末尾 `if (totalCalls_.empty())` 归零 | **跨通话泄漏**：第一通电话的用户选择影响第二通 |

> 这四点缺任何一点都是缺陷，且四种缺陷的现象各不相同。
> 尤其注意「清位」一项：它是[`failure-modes.md`](failure-modes.md)「修复模板」所说的第三条清位路径，即最后一路通话结束时的统一清位。
> 若当前代码中搜不到这一句，那就是缺口本身，不是文档过时——见 [`AGENTS.md`](../../AGENTS.md) 开头的版本说明。

**同类跨线程标志位都应当是 atomic，且都应当具备上表的四个落点。**

### 音频场景状态机

`AudioSceneProcessor` 持有 `currentState_`，状态类在 `services/audio/src/audio_state/`：

```text
InActiveState ──SWITCH_DIALING_STATE──▶ DialingState
              ──SWITCH_INCOMING_STATE─▶ IncomingState
              ──SWITCH_ALERTING_STATE─▶ AlertingState
                                        │
                    SWITCH_CS_CALL_STATE / SWITCH_IMS_CALL_STATE
                                        ▼
                                  CSCallState / IMSCallState / OttCallState
                                        │
                    SWITCH_HOLDING_STATE ▼
                                   HoldingState
                                        │
                 SWITCH_AUDIO_INACTIVE_STATE ▼
                                   InActiveState
```

约束：
- 迁移只能通过 `AudioSceneProcessor::ProcessEvent(AudioEvent)`，不得直接赋值 `currentState_`。
- 每个 `SWITCH_*` 事件在切换前会统一判断 `ShouldStopSoundtone()` 并停音、释放 disconnected 锁。**新增 `SWITCH_*` 事件必须加入 `ProcessEventInner()` 的同一个 case 组**，否则会漏掉停音与锁释放，表现为「挂断后还在响」+ 电源锁泄漏。
- 设备状态机（`audio_state/*_device_state.cpp`）与场景状态机是**两套独立状态机**，分别由 `AudioDeviceManager` 与 `AudioSceneProcessor` 驱动。改动时不要混淆。

### 五种音互不等价

| 音 | 何时 | 相关函数 |
|---|---|---|
| Ringtone | 本机来电铃声 | `PlayRingtone` / `StopRingtone` / `MuteRinger` |
| Soundtone | 接听后的提示音、CRS 恢复 | `PlaySoundtone` / `StopSoundtone` / `ResumeCrsSoundTone` |
| Ringback | 拨出后听到的回铃 | `StopRingback` |
| Waiting tone | 通话中第二路来电的提示 | `PlayWaitingTone` / `StopWaitingTone` |
| CallEnded tone | 挂断音 | `PlayCallEndedTone` |

改「铃声不响 / 响不停」类问题时先确认是哪一种音，用错函数是常见返工原因。

---

## 全局通话对象表约束（`CallObjectManager`）

- 数据：`callObjectPtrList_`（`std::list<sptr<CallBase>>`）+ `voipCallObjectList_`（`std::map<int32_t, CallAttributeInfo>`），都是 **static**，由 **static `ffrt::mutex listMutex_`** 保护。
- 所有方法都是 `static`，本类无实例状态。`CallPolicy` 与 `CallRequestProcess` 通过继承获得这些方法，**不是**组合关系。
- **`GetOneCallObject()` 会返回 `nullptr`**（对象已被并发移除时）。每一次调用后判空，不要缓存返回值跨越会触发状态变更的调用。
- 遍历列表期间不得调用会再次获取 `listMutex_` 的函数——`ffrt::mutex` 非递归，会自死锁。
- VoIP 通话**不在** `callObjectPtrList_` 里，走独立的 `voipCallObjectList_`。所以「统计当前通话数」的函数普遍带有 `isIncludeVoipCall` 参数，**传错会导致多路仲裁判断错误**。写新代码时显式传值，不要依赖默认参数。

---

## 订阅者广播的两条硬约束

`CallStateListener` 的实现（`services/call/src/call_state_listener.cpp`）决定了：

**约束一：订阅者之间不得有顺序依赖。**
`listenerSet_` 是 `std::set<std::shared_ptr<CallStateListenerBase>>`，遍历顺序是 `shared_ptr` 的**指针地址序**，与 `AddOneObserver` 的注册顺序无关，且跨次启动不稳定。

> 如果改动要求「音频先处理完，上报再发出」，**不能靠调整注册顺序实现**。正确做法是在 `CallStatusManager` 的 Handle 函数里显式排序，或让后者监听前者的完成信号。

**约束二：广播全程持有非递归锁。**
`NewCallCreated` / `CallStateUpdated` / `CallDestroyed` 等都在 `std::lock_guard<ffrt::mutex> lock(mutex_)` 下完成整个 for 循环。因此订阅者回调里：

- **禁止**调用 `AddOneObserver` / `RemoveOneObserver` / `RemoveAllObserver` → 立即自死锁。
- **禁止**阻塞或长耗时 → 拖住其余全部订阅者，且因为上游是单队列，会连带阻塞整条上行链路。
- 需要做耗时事的订阅者，应当在回调里只记录状态并投递到自己的队列。

---

## 隐式约束速查

| 约束 | 说明 |
|---|---|
| 默认参数陷阱 | `GetCallNum(state, isIncludeVoipCall = true)`、`GetForegroundCall(isIncludeVoipCall = true)`、`AnswerCall(callId, videoState, isRTT = false)` 等默认值会改变仲裁结果。新代码显式传参。 |
| `CALL_STATUS_ANSWERED` 是本仓自造态 | 协议栈不产生它；`priorState == INCOMING && nextState == ANSWERED` 是「用户已按接听」的唯一可靠判据。 |
| `GetAnsweredByPhone()` | 区分本机接听与分布式对端接听。音频、蓝牙、分布式三个模块的分支判断以它为条件。新增音频分支时必须考虑它为 `false` 的情况。 |
| 编译宏内的成员 | `#ifdef` 包裹的成员变量，其所有引用点也必须在同一宏内。宏关闭时编译不过是本仓改动的常见低级错误。 |
| 号码清洗时机 | `RemoveSeparatorsPhoneNumber()` 之后的所有逻辑用清洗后的号码；对外上报与写通话记录用**原始号码**。两者不可混用。 |
| 单例初始化顺序 | `DelayedSingleton<T>::GetInstance()` 在首次调用时构造。在另一个单例的构造函数里取第三个单例会产生初始化顺序依赖，`Init()` 里做才安全。 |
| `-fno-exceptions -fno-rtti` | 全库生效。不能 `try/catch`、不能 `dynamic_cast`。类型判别用 `GetCallType()` + `static_cast`。 |
| IPC 枚举即 ABI | `*_ipc_interface_code.h` 的枚举值只能末尾追加。插入或重排会让新旧版本部件之间静默错分发。 |
