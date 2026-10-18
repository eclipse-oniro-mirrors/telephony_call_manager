# 代码地图

← 返回 [`AGENTS.md`](../../AGENTS.md)

---

## 顶层目录职责

```text
telephony_call_manager/
├── AGENTS.md            知识库入口
├── docs/agents/         知识库分层文档
├── BUILD.gn             主库 tel_call_manager 的构建定义
├── callmanager.gni      【全仓构建真源】源文件表 / include 表 / 外部依赖 / feature 宏
├── bundle.json          部件声明：SA 依赖、syscap、features、构建分组
├── sa_profile/          SA 4005 启动配置（进程 telecom）
├── interfaces/
│   ├── innerkits/       部件间 C++ 接口与类型定义（本仓的"公共词汇表"）
│   └── kits/js/         对应用的 .d.ts 声明
├── frameworks/
│   ├── native/          客户端 SDK + 所有 IPC proxy
│   ├── js/napi/         NAPI 胶水（ArkTS 1.1）
│   ├── ets/taihe/       taihe 胶水（ArkTS 1.2）
│   └── cj/              仓颉 FFI 胶水
├── services/            服务进程内的全部实现（见本文「services/ 模块职责表」）
├── utils/               通用工具：号码格式化、DataShare、弹窗、配置、传感器
└── test/
    ├── unittest/        gtest（白盒 zero 系列 + 黑盒 gtest 系列）
    ├── fuzztest/        20 个目录 / 18 个已挂载的构建目标（见 build-and-test.md「测试套的选择」）
    └── mock/            测试替身
```

### `interfaces/innerkits/` 是必读的词汇表

改任何业务前先扫一遍这两个文件，本仓的全部状态语义都在这里：

| 文件 | 内容 |
|---|---|
| `call_manager_base.h` | **核心枚举**：`CallType`、`TelCallState`、`CallRunningState`、`TelConferenceState`、`VideoStateType`、`AudioDeviceType`、`DialScene`、`DialType`、`CallDirection`、`CallStateToApp`、`CallAbilityEventId`、`MarkType`、`AntiFraudState` |
| `call_manager_inner_type.h` | 结构体与 IMS 相关枚举：`CallDetailInfo`、`CallDetailsInfo`、`DialParaInfo`、`ImsCallMode`、`ImsConfigItem` |
| `call_manager_info.h` | 对外信息结构：`CallAttributeInfo`、`CallTransferInfo`、`CallRestrictionInfo` |
| `call_manager_errors.h` | 全部错误码 |
| `call_manager_disconnected_details.h` | 挂断原因码（`DisconnectedReason` 与 `RilDisconnectedReason` 两套，注意区分） |
| `*_ipc_interface_code.h` | **IPC ABI**，枚举值只能追加 |

---

## 两个必须分清的状态枚举

本仓有两套通话状态，混用是高频错误来源。

| | `TelCallState` | `CallRunningState` |
|---|---|---|
| 语义 | **协议栈视角**的通话状态 | **本仓视角**的运行阶段 |
| 取值 | `UNKNOWN(-1)`、`ACTIVE(0)`、`HOLDING`、`DIALING`、`ALERTING`、`INCOMING`、`WAITING`、`DISCONNECTED`、`DISCONNECTING`、`IDLE`、`ANSWERED` | `CREATE(0)`、`CONNECTING`、`DIALING`、`RINGING`、`ACTIVE`、`HOLD`、`ENDED`、`ENDING` |
| 读取 | `call->GetTelCallState()` | `call->GetCallRunningState()` |
| 谁在写 | `CallStatusManager::UpdateCallState()`，由协议栈上报驱动 | 由 `TelCallState` 派生 |
| 典型用途 | 状态机分支、上报应用 | 查询「当前有没有正在响铃的通话」之类的聚合判断 |

> `CALL_STATUS_ANSWERED` 是本仓**自造的中间态**，协议栈不产生它。用户按下接听后先进 `ANSWERED`，协议栈确认后才到 `ACTIVE`。音频侧的分支判断依赖这个中间态区分「用户已接听但通路未建立」。

`GetOneCallObject(CallRunningState)` 与 `GetOneCallObject(int32_t callId)` 是重载，传错类型会静默取到别的通话——写代码时显式构造枚举，别依赖隐式转换。

---

## 对外接口分层（新增 API 时的落点）

一个对外 API 从上到下要穿过四层，**四层都要改**：

| 层 | 文件 | 改什么 |
|---|---|---|
| 1. 声明 | `interfaces/kits/js/@ohos.telephony.call.d.ts` | TS 类型声明与 JSDoc（含 `@permission`、`@since`、`@syscap`） |
| 2. 胶水 | `frameworks/js/napi/src/napi_call_manager.cpp` | NAPI 注册与参数转换（ArkTS 1.1） |
| | `frameworks/ets/taihe/idl/` + `src/ohos.telephony.call.impl.cpp` | taihe IDL 与实现（ArkTS 1.2） |
| | `frameworks/cj/src/` | 仓颉 FFI（若该 API 需要） |
| 3. 客户端 | `frameworks/native/src/call_manager_client.cpp` → `call_manager_proxy.cpp` → `call_manager_service_proxy.cpp` | 序列化与 IPC 发送 |
| 4. 服务端 | 见 [`boundaries.md`](boundaries.md)「新增 IPC 接口的六步清单」 | 反序列化、鉴权、业务 |

> `@ohos.telephony.call.d.ts` 是全仓第二大文件（量级见本文「关键文件体量（定位时的取舍参考）」）。新增接口务必按既有 JSDoc 模板补齐 `@permission` / `@throws` / `@syscap` / `@since`，OpenHarmony 的 API 门禁会检查。

---

## `services/` 模块职责表

### 核心：通话生命周期

| 目录 | 关键类 | 职责 |
|---|---|---|
| `services/call_manager_service/` | `CallManagerService`、`CallManagerServiceStub` | SA 生命周期、IPC 分发、**鉴权唯一收口**、XCollie 超时看护 |
| `services/call/src/` | `CallControlManager` | 下行业务入口（单例，继承 `CallPolicy`） |
| | `CallPolicy` | 下行前置校验（继承 `CallObjectManager`） |
| | `CallObjectManager` | **全局通话对象表**（`callObjectPtrList_` + `voipCallObjectList_`），全静态方法 |
| | `CallRequestHandler` / `CallRequestProcess` | 下行异步化与协议栈参数组装 |
| | `CallStatusManager` / `CallStatusPolicy` | **上行状态机**，`call_status_manager.cpp` 是全仓第三大文件（量级见本文「关键文件体量（定位时的取舍参考）」） |
| | `CallStateListener` | 状态变更一对多广播 |
| | `CallBase` 及子类 | 通话对象模型，见本文「`CallBase` 继承树」 |
| | `CallConnectAbility` / `CallAbilityConnectCallback` | 拉起通话 UI extension |
| `services/telephony_interaction/` | `CellularCallConnection` | **下行出口**，调 cellular_call |
| | `CallStatusCallbackStub` / `CallStatusCallback` | **上行入口**，接 cellular_call |
| | `ReportCallInfoHandler` | 上行投递到 ffrt 队列 `report_call_info_queue` |
| | `CoreServiceConnection` / `VoipCallConnection` / `BluetoothCallConnection` | 其他外部服务连接 |

### `CallBase` 继承树

```text
CallBase                                  通话对象基类（callId / number / state / 音频标志）
├── CarrierCall                           运营商通话
│   ├── CSCall        ── CsConference      电路域
│   ├── IMSCall       ── ImsConference     IMS 域（视频、RTT 挂在这里）
│   └── SatelliteCall                      卫星
├── NetCallBase
│   └── OttCall       ── OttConference     第三方 OTT
├── VoIPCall                               应用 VoIP / MeeTime
└── BluetoothCall                          蓝牙侧发起
```

`ConferenceBase` 及其三个子类管理会议成员关系，与 `CallBase` 是**组合而非继承**关系。

### 音频子系统（`services/audio/`）

本仓 bug 密度最高的模块。三个层次务必分清：

| 层次 | 类/文件 | 职责 | 边界 |
|---|---|---|---|
| 路由决策 | `AudioControlManager` | 决定「当前该用哪个设备」 | 决策入口 `UpdateDeviceType()`；执行出口 `SetAudioDevice()` |
| 设备状态 | `AudioDeviceManager` | 维护设备可用性/连接性/当前设备、`callAudioMode_` | 所有设备布尔量在此，见 [`failure-modes.md`](failure-modes.md)「最高频失效模式：粘滞标志位（sticky latch）」 |
| 场景状态机 | `AudioSceneProcessor` + `audio_state/*.cpp` | 通话音频场景迁移 | 见 [`boundaries.md`](boundaries.md)「音频场景状态机」 |
| 发声 | `Ring` / `Sound` / `Tone` / `AudioPlayer` | 实际播放 | **只播放，不改路由** |
| 外部代理 | `AudioProxy` | 封装 audio_framework 调用 | 唯一允许直连 `AudioSystemManager` 的地方 |

音频设备优先级集中在 `AudioControlManager::GetInitAudioDeviceTypeOfRemote()`：

```text
分布式车机 > 星闪(NearLink) > 蓝牙 SCO > 有线耳机 > 蓝牙助听器 > (无外设)
无外设时按通话类型：视频/卫星/蓝牙通话 → 扬声器；否则 → 听筒 → 扬声器
```

> **新增音频设备类型是横切改动**，至少需要同步：`AudioDeviceType` 枚举、`GetInitAudioDeviceTypeOfRemote()` 优先级、`IsInEarAudioDevice()`、`IsExternalAudioDevice()`、`SetAudioDevice()` 的 switch、`ReportAudioDeviceChange()` 的地址填充分支、`AudioDeviceManager` 的连接性布尔量与其清位路径。漏任何一处都表现为「设备能连上但通话不走它」。

### 补充业务（`services/call_setting/`）

`CallSettingManager` 负责呼叫转移、呼叫等待、呼叫限制、IMS 开关、VoNR 开关。这些是**异步请求-响应**：下行经 `CellularCallConnection` 发出，结果经上行 `HandleEventResultReportInfo()` 回来，再由 `CallAbilityReportProxy` 报给应用。改这块务必两端都改。

### 状态观察者（`services/call/call_state_observer/`）

被动响应通话状态的附属功能，互不依赖：

| 文件 | 功能 |
|---|---|
| `call_records_manager.cpp` / `call_records_handler.cpp` / `call_data_base_helper.cpp` | 通话记录写入 DataShare |
| `missed_call_notification.cpp` | 未接来电通知 |
| `incoming_call_notification.cpp` | 来电通知 |
| `incoming_call_wake_up.cpp` | 来电亮屏 |
| `proximity_sensor.cpp` | 贴耳灭屏 |
| `reject_call_sms.cpp` | 拒接短消息 |
| `wired_headset.cpp` | 有线耳机按键 |
| `status_bar.cpp` | 状态栏图标 |
| `rtt_call_listener.cpp` | RTT（`SUPPORT_RTT_CALL` 宏内） |

### 分布式（`services/distributed_call/` 与 `services/interoperable_call/`）

**整个目录在 `SUPPORT_DSOFTBUS` 宏内**（默认 `true`）。

- `distributed_call/`：通话在设备间迁移（手机↔车机/平板/PC）。`distributed_communication/` 是控制面，`transmission/` 是 dsoftbus 会话封装（client/server session）。
- `interoperable_call/`：跨端通话数据同步（手表等）。`target_platform == "watch"` 时额外编入 `interoperable_settings_handler.cpp` 并打开 `SUPPORT_MUTE_BY_DATABASE`。

### 号码识别与安全

| 目录 | 职责 |
|---|---|
| `services/spam_call/` | 骚扰拦截、号码标记（含 `SpamCallAdapter`、独立 stub/proxy） |
| `services/antifraud/` | 防诈检测，通过 `dia_*` C 接口动态加载外部库（`AnonymizeAdapter` 做脱敏） |
| `services/number_identity_proxy/` | 号码归属地识别 |
| `services/call/super_privacy/` | 超级隐私模式下的拨打/接听管控 |
| `services/call/src/call_incoming_filter_manager.cpp` | 来电过滤总入口 |

### 其他

| 目录 | 职责 |
|---|---|
| `services/bluetooth/` | 蓝牙通话服务（独立 SA 接口 `BluetoothCallStub`）、HFP 策略 |
| `services/video/` | 视频通话窗口、摄像头控制 |
| `services/display/` | 折叠屏形态（`FoldStatusManager`） |
| `services/call_report/` | 向应用/通话 UI 上报（`CallAbilityReportProxy`、`CallStateReportProxy`、死亡通知） |
| `services/call_voice_assistant/` | 语音播报来电 |
| `services/satellite_call/` | 卫星通话管控 |
| `services/call_earthquake_alarm/` | 地震预警联动 |
| `services/rtt_call/` | 实时文本（`SUPPORT_RTT_CALL` 宏内，默认关） |
| `services/hisysevent/` | 打点与 dump |
| `services/deps_adapter/` | 依赖适配（独立构建目标） |
| `services/etc/init/` | `telecom.cfg` 与 SELinux trust 配置 |

---

## 状态广播订阅者表

`CallControlManager::CallStateObserve()`（`call_control_manager.cpp:1762`）注册的订阅者：

| 订阅者 | 条件 | 关注 |
|---|---|---|
| `CallAbilityReportProxy` | always | 上报通话 UI 与注册了回调的应用 |
| `CallStateReportProxy` | always | 上报 state_registry（`observer.on('callStateChange')`） |
| `AudioControlManager` | always | 音频路由与铃声 |
| `RejectCallSms` | always | 拒接短消息 |
| `MissedCallNotification` | always | 未接通知 |
| `IncomingCallWakeup` | always | 来电亮屏 |
| `CallRecordsManager` | always | 通话记录 |
| `DistributedCommunicationManager` | `SUPPORT_DSOFTBUS` | 跨端 |
| `InteroperableCommunicationManager` | `SUPPORT_DSOFTBUS` | 跨端 |
| `CallVoiceAssistantManager` | always | 语音播报 |
| `RttCallListener` | `SUPPORT_RTT_CALL` | RTT |

> 另有 4 个类继承 `CallStateListenerBase` 但**未在此注册**：`IncomingCallNotification`、`ProximitySensor`、`StatusBar`、`WiredHeadset`。它们由各自模块单独驱动。若某个 observer 的回调不触发，先确认它是否在这张表里。

**两条硬约束**（见 [`boundaries.md`](boundaries.md)「订阅者广播的两条硬约束」展开）：

1. `listenerSet_` 是 `std::set<std::shared_ptr<...>>`，**遍历顺序是指针地址序，不是注册序，且跨次启动不稳定**。订阅者之间不得有顺序依赖。
2. 广播全程持 `CallStateListener::mutex_`（非递归 `ffrt::mutex`）。订阅者回调里**不得**调用 `AddOneObserver`/`RemoveOneObserver`，会自死锁；也不得阻塞，会拖住全部订阅者。

---

## 关键文件体量（定位时的取舍参考）

> 下表数字为**快照值，仅用于判断相对量级**，不是当前精确行数。代码仓持续演进，不要把它当作可校验的断言引用。
> 统计口径：`(Get-Content $f).Count`，含空行。

| 文件 | 量级 | 说明 |
|---|---|---|
| `frameworks/js/napi/src/napi_call_manager.cpp` | ~5700 | NAPI 胶水，改动多为机械式 |
| `interfaces/kits/js/@ohos.telephony.call.d.ts` | ~5400 | 对外声明 |
| `services/call/src/call_status_manager.cpp` | ~3000 | **上行状态机，改动前务必通读相关 Handle 函数** |
| `frameworks/js/napi/src/napi_call_ability_callback.cpp` | ~2700 | 回调上抛 |
| `services/call/src/call_control_manager.cpp` | ~2600 | 下行入口 |
| `services/call_manager_service/src/call_manager_service.cpp` | ~2200 | 鉴权 + 转发 |
| `frameworks/native/src/call_manager_proxy.cpp` | ~2100 | 客户端 proxy |
| `services/audio/src/audio_control_manager.cpp` | ~1900 | **音频路由，bug 密度最高** |
| `services/call_manager_service/src/call_manager_service_stub.cpp` | ~1700 | IPC 分发表 |
| `services/call/src/call_object_manager.cpp` | ~1200 | 全局对象表 |
| `services/call/src/call_request_process.cpp` | ~1100 | 下行组装 |
| `services/audio/src/audio_device_manager.cpp` | ~1000 | 设备状态 |

大文件不要整读。用 `AGENTS.md`「任务触发路由（按任务类型）」定位到函数，再读该函数与其直接调用者。
