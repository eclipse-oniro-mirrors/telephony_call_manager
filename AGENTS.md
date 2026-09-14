# AGENTS.md — telephony_call_manager 代码仓领域知识库入口

> 本文件是**唯一入口**。所有其他知识文档从这里路由进入，位于 `docs/agents/`。
> 面向对象：需要在本仓完成需求/修复/重构的 AI Agent。
> 使用方式：先读「三十秒定位：两条主链路」确定方向 → 用「知识路由表」选中要读的文档 → 修改前核对「红线（禁止事项）」 → 完成前执行「最小验证闭环（DoD）」。

> **本知识库描述的是 master 主干的状态。** 文档里写的「不变量」是**主干应当成立的约束**，不是「当前代码一定已经满足」。
> 在旧分支、发布分支或回退版本上工作时，某条不变量有时尚未落地——**这往往就是待修复的缺陷**。
> 因此：读到一条关键不变量后，**先在当前代码里搜一遍它的落点**（置位点、清位点、判断点）。
> 找不到落点时，正确结论是「这里缺了一处」，而不是「文档写错了」。两者的区分方法：
> 缺口与需求现象一致时，它通常就是根因；与现象无关时，记为技术债并在报告里单独列出，不要在本次改动中一并修改。

---

## 这个仓是什么

OpenHarmony 电话子系统的**通话管理**部件（part_name `call_manager`，subsystem `telephony`）。

- 运行形态：系统服务 SA **4005**，进程名 **telecom**，产物 `libtel_call_manager.z.so`，`run-on-create=true`（`sa_profile/4005.json`）。
- 职责：管理 CS / IMS / OTT / VoIP / 卫星 / 蓝牙 六类通话的生命周期，申请与释放音视频资源，仲裁多路通话冲突。
- 上游依赖：`cellular_call`（协议栈下行/上行）、`core_service`（卡与网络）、`audio_framework`、`bluetooth`、`ability_runtime`、`dsoftbus`。
- 下游消费者：通话 UI（CallAbility extension）、应用（`@ohos.telephony.call`）、蓝牙 HFP 设备、分布式对端。

**一句话心智模型**：本仓是一个**双向管道 + 一份全局通话对象表 + 一组订阅者**。
下行把应用请求翻译成协议栈命令，上行把协议栈状态翻译成对象状态变更并广播给订阅者。多数缺陷出在「上行状态变更后，某个订阅者的内部状态没有同步更新」。

---

## 三十秒定位：两条主链路

动手前先判断需求落在**下行**还是**上行**。方向判断错误是本仓最常见的返工原因。

### 下行链路（应用 → 协议栈）

```text
应用 JS  @ohos.telephony.call
  └─ frameworks/js/napi/src/napi_call_manager.cpp        NAPI 胶水
  └─ frameworks/ets/taihe/                                ArkTS 1.2 胶水
  └─ frameworks/cj/                                       仓颉 FFI 胶水
       └─ frameworks/native/src/call_manager_client.cpp    客户端 SDK
            └─ call_manager_proxy.cpp → IPC ─────────────┐
                                                          ▼
services/call_manager_service/src/call_manager_service_stub.cpp   反序列化 + 分发（memberFuncMap_）
     └─ call_manager_service.cpp                                   【鉴权唯一收口】
          └─ services/call/src/call_control_manager.cpp            业务入口
               └─ call_policy.cpp                                  【前置校验唯一收口】
                    └─ call_request_handler.cpp                    转异步（ffrt）
                         └─ call_request_process.cpp               组装协议栈参数
                              └─ services/telephony_interaction/src/cellular_call_connection.cpp
                                   └─ IPC → cellular_call
```

### 上行链路（协议栈 → 应用）

```text
cellular_call ──IPC──▶ services/telephony_interaction/src/call_status_callback_stub.cpp
     └─ call_status_callback.cpp
          └─ report_call_info_handler.cpp        【投递到 ffrt 队列 report_call_info_queue】
               └─ services/call/src/call_status_manager.cpp        上行状态机
                    ├─ IncomingHandle / DialingHandle / AlertHandle /
                    │  ActiveHandle / HoldingHandle / WaitingHandle /
                    │  DisconnectingHandle / DisconnectedHandle
                    └─ UpdateCallState()
                         └─ services/call/src/call_state_listener.cpp   一对多广播
                              ├─ services/audio/          音频路由与铃声
                              ├─ services/bluetooth/      HFP 状态同步
                              ├─ services/call_report/    上报应用与通话 UI
                              ├─ services/call/call_state_observer/  通话记录/通知/传感器
                              ├─ services/distributed_call/  跨端迁移
                              ├─ services/video/          视频窗口
                              └─ …（见 docs/agents/code-map.md 订阅者全表）
```

> **关键边界**：`CallStateListener` 的广播是本仓订阅者最多的分发点。任何一个订阅者在回调里抛错、阻塞或破坏自身的内部标志位，都会表现为「另一个模块的功能坏了」。定位跨模块问题从这里切入。

---

## 知识路由表

### 任务触发路由（按任务类型）

| 任务 | 先读 | 再改 |
|---|---|---|
| 新增/修改对外 JS/ArkTS API | `docs/agents/code-map.md`「对外接口分层（新增 API 时的落点）」 | `interfaces/kits/js/@ohos.telephony.call.d.ts` → `frameworks/js/napi/` → `frameworks/ets/taihe/idl` |
| 新增 innerkit IPC 接口 | `docs/agents/boundaries.md`「新增 IPC 接口的六步清单」 | 六处同步修改，顺序见该节，漏改任一处会导致运行期失败 |
| 拨号/接听/挂断/保持/切换的**策略**变更 | `docs/agents/boundaries.md`「下行链路的调用顺序约束」 | `services/call/src/call_policy.cpp` |
| 拨号/接听/挂断的**流程**变更 | `docs/agents/boundaries.md`「下行链路的调用顺序约束」 | `call_control_manager.cpp` + `call_request_process.cpp` |
| 来电流程、状态迁移（上行） | `docs/agents/boundaries.md`「上行状态机约束（`CallStatusManager`）」 | `services/call/src/call_status_manager.cpp` |
| **多路通话仲裁 / DSDA**（注意：横跨两个文件） | `docs/agents/boundaries.md`「多路通话仲裁横跨两个文件」 | **决策**在 `call_request_process.cpp`（`HandleCallWaitingNum*`、`HoldOrDisconnectedCall`、`HandleDsdaIncomingCall`）；**上行侧的自动接听/自动解保持**在 `call_status_manager.cpp`（`AutoAnswer`、`AutoHandleForDsda`、`AutoUnHoldForDsda`） |
| 音频**路由/设备选择** | `docs/agents/boundaries.md`「音频子系统边界」 | `services/audio/src/audio_control_manager.cpp` + `audio_device_manager.cpp` |
| 铃声/彩振/提示音/振动 | `docs/agents/code-map.md`「音频子系统（`services/audio/`）」 | `services/audio/src/{ring,sound,tone,audio_player}.cpp` |
| 音频场景切换 | `docs/agents/boundaries.md`「音频场景状态机」 | `audio_scene_processor.cpp` + `audio_state/*.cpp` |
| 蓝牙通话 / HFP | `docs/agents/high-risk.md`「高危入口（改动这些位置，事故等级最高）」+「高危事件序列」 | `services/bluetooth/` + `telephony_interaction/src/bluetooth_call_connection.cpp` |
| 分布式 / 跨端通话 | `docs/agents/code-map.md`「分布式（`services/distributed_call/` 与 `services/interoperable_call/`）」 | `services/distributed_call/` + `services/interoperable_call/` |
| 骚扰拦截 / 号码标记 / 防诈 | `docs/agents/code-map.md`「号码识别与安全」 | `services/spam_call/`、`services/antifraud/`、`call_incoming_filter_manager.cpp` |
| 通话记录 / 未接来电通知 | `docs/agents/code-map.md`「状态观察者（`services/call/call_state_observer/`）」 | `services/call/call_state_observer/` |
| 补充业务（呼转/呼等/呼限/IMS 开关） | `docs/agents/code-map.md`「补充业务（`services/call_setting/`）」 | `services/call_setting/` |
| 新增编译开关 / feature 宏 | `docs/agents/high-risk.md`「高危配置项」 | `callmanager.gni` + `bundle.json` 的 `features` |
| 新增源文件 | `docs/agents/build-and-test.md`「`callmanager.gni` 是全仓构建真源」 | `callmanager.gni` 的 `call_manager_sources` |
| 排查线上问题 / 看日志 | `docs/agents/observability.md` | — |
| 写回归用例 | `docs/agents/fix-and-verify.md`「验证与回归」 | `test/unittest/call_manager_zero_gtest/` |
| 评估改动风险 / 灰度 | `docs/agents/high-risk.md`「变更风险矩阵」 | — |

### 路径触发路由（按文件路径）

| 文件路径 | 必读知识 |
|---|---|
| `services/audio/**` | `docs/agents/boundaries.md`「音频子系统边界」、`failure-modes.md`「最高频失效模式：粘滞标志位（sticky latch）」 |
| `services/call/src/call_status_manager.cpp` | `boundaries.md`「上行状态机约束（`CallStatusManager`）」（状态机不变量）、`failure-modes.md`「队列阻塞与状态停摆」 |
| `services/call/src/call_policy.cpp` | `boundaries.md`「下行链路的调用顺序约束」（策略必须先于副作用） |
| `services/call_manager_service/**` | `boundaries.md`「新增 IPC 接口的六步清单」（Stub 零业务逻辑 + 鉴权收口） |
| `services/call/src/call_object_manager.cpp` | `boundaries.md`「全局通话对象表约束（`CallObjectManager`）」（全局对象表与锁） |
| `services/distributed_call/**`、`interoperable_call/**` | `high-risk.md`「高危配置项」（这两个目录整体被 `SUPPORT_DSOFTBUS` 包裹） |
| `frameworks/native/**` | `boundaries.md`「新增 IPC 接口的六步清单」（proxy/stub 必须成对改） |
| `interfaces/innerkits/*_ipc_interface_code.h` | `high-risk.md`「高危入口（改动这些位置，事故等级最高）」（**枚举值即 ABI，只能追加**） |
| `callmanager.gni` | `build-and-test.md`「`callmanager.gni` 是全仓构建真源」（改这里会同时影响服务与全部 gtest 目标） |
| `services/call_setting/**` | `code-map.md`「补充业务（`services/call_setting/`）」（异步请求-响应，下行与上行必须成对改） |
| `services/bluetooth/**` | `high-risk.md`「高危入口（改动这些位置，事故等级最高）」（等级 B 条目）+「高危事件序列」（蓝牙连断重连序列） |
| `services/video/**` | `code-map.md`「其他」、`boundaries.md`「音频子系统边界」（视频通话的音频设备优先级与语音不同） |
| `services/spam_call/**`、`services/antifraud/**` | `code-map.md`「号码识别与安全」、`high-risk.md`「高危入口（改动这些位置，事故等级最高）」（误拦正常来电） |
| `services/call/super_privacy/**` | `high-risk.md`「运行期配置项」（隐私模式禁拨禁接） |
| `utils/**` | 无专属约束，但改动影响全仓调用方，先确认调用范围 |
| `test/unittest/**` | `build-and-test.md`「测试套的选择」、`fix-and-verify.md`「验证与回归」 |

### 词汇触发路由（读到这些词就查这里）

本仓专有术语集中在下表。**看到不认识的词先查这张表再读代码**，否则容易误判。

| 词 | 含义 | 落点 |
|---|---|---|
| **CRS** / 彩振 | 运营商网络侧下发的回铃视频/音频（Customized Ring Signal）。判定：`call->GetCrsType() == CRS_TYPE`（`CRS_TYPE` 定义在 `audio_control_manager.cpp`，值为 2；**代码里一律用具名常量，搜字面量 `== 2` 查不到**） | `audio_control_manager.cpp` |
| **星闪** / NearLink | 华为短距无线，独立于蓝牙的音频设备类型 `AudioDeviceType::DEVICE_NEARLINK` | `audio_device_manager.cpp` |
| **DSDA / DSDS** | 双卡双通 / 双卡双待。`IsDsdsMode3()`/`IsDsdsMode5()` 区分能力档位 | `call_request_process.cpp` |
| **ECC / SOS** | 紧急呼叫。走独立策略 `EccDialPolicy()`，绕过多路限制、EDM 管控、隐私模式等常规校验 | `call_policy.cpp`、`call_request_process.cpp` |
| **OTT** | 第三方互联网通话（非运营商） | `services/call/src/ott_call.cpp` |
| **VoIP / MeeTime** | 应用侧 VoIP 通话，与运营商通话存在互斥仲裁 | `voip_call.cpp`、`CarrierAndVoipConflictProcess` |
| **MMI code** | `*#…#` 特服码，拨号号码若命中则不建立通话 | `call_request_process.cpp` |
| **FDN** | 固定拨号允许列表（SIM 侧限制） | `IsFdnNumber()` |
| **超级隐私 / Super privacy** | 隐私模式下禁拨/禁接 | `services/call/super_privacy/` |
| **HFP** | 蓝牙免提协议；`SCO` 是其音频链路 | `services/bluetooth/` |
| **OOBE** | 开机向导未完成状态，`deviceProvisioned_` | `call_status_manager.cpp` |
| **Soundtone / Ringtone / Ringback / Waitingtone / CallEndedTone** | **五种不同的音**，切勿混用：本机来电铃声（Ringtone）/ 接听后的提示音与彩振恢复（Soundtone）/ 拨出后的回铃（Ringback）/ 通话中第二路来电提示（Waitingtone）/ 挂断音（CallEndedTone） | `services/audio/` |
| **callui / CallAbility** | 通话界面 extension，由本仓主动拉起 | `call_connect_ability.cpp` |
| **slotId / accountId** | 卡槽号（0/1） | 全仓 |
| **index / phoneIndex** | **协议栈侧通话序号，与 `callId` 不是一回事**，混用会取错对象 | `call_object_manager.cpp` |
| **callId** | 本仓自增的通话对象 ID（`GetNewCallId()`） | `call_object_manager.cpp` |
| **AnsweredByPhone** | 本机接听（相对于分布式对端接听），音频、蓝牙、分布式模块的分支判断以它为条件 | `call_base.h` |
| **reportCallInfoQueue** | 上行状态处理的**单 ffrt 队列**，串行 | `report_call_info_handler.cpp` |

### 动手前必须先自陈（强制）

**在写出第一行代码改动之前，先在回复里用三行写清楚：**

```text
任务类别：<「任务触发路由（按任务类型）」表中的哪一行>
已读文档：<docs/agents/ 下实际读过的文件与小节>
命中约束：<「红线（禁止事项）」中的编号 + 分层文档里找到的相关不变量；确认没有则写"无">
```

这三行是**检查点，不是格式要求**。无法写出时，说明路由尚未完成，不要开始修改。

尤其是「命中约束」一行：如果列不出本次改动涉及的不变量，说明尚未掌握本次改动的影响范围。

---

## 红线（禁止事项）

违反以下任一条的改动不可合入。

1. **禁止在铃声/提示音函数里改音频路由。** 路由决策唯一收口是 `AudioControlManager::UpdateDeviceType()` → `UpdateDeviceForForegroundCall()`；执行唯一收口是 `AudioControlManager::SetAudioDevice()`。在 `ResumeCrsSoundTone()`、`PlayRingtone()` 这类函数里改设备，是本仓最典型的错误修法。
2. **禁止绕过 `CallPolicy` 直接调用 `CallRequestProcess`。** 所有下行动作必须先过策略。
3. **禁止在 `CallManagerServiceStub` 里写业务逻辑。** Stub 只做反序列化与转发，鉴权在 `CallManagerService`，业务在 `CallControlManager`。
4. **禁止在 `CallStatusManager::XxxHandle()` 中做阻塞或耗时操作。** 它们跑在单一 ffrt 队列 `report_call_info_queue` 上，阻塞会让**所有后续通话状态上报**停摆，表现为「来电不响铃 / 挂断后界面不消失」。
5. **禁止新增不判断 `isSetAudioDeviceByUser_` 的 `SetAudioDevice()` 自动调用。** 会覆盖用户手动选择的音频设备。
6. **禁止修改 `CallManagerInterfaceCode` / `CallStatusInterfaceCode` / `CellularCallInterfaceCode` 等已有枚举值。** 枚举值即 IPC ABI，跨部件版本不一致会静默错分发。只能在末尾追加。
7. **禁止使用 `try/catch` 与 `dynamic_cast`。** `BUILD.gn` 全局 `-fno-exceptions -fno-rtti`。
8. **禁止打印号码、联系人名等隐私字段。** 全仓 `%{private}` 用量为 **0**（这一条是全仓口径，可复核）；约定是隐私字段根本不进日志，而不是用 `%{private}` 包起来。定位具体通话请用 `callId` / `slotId` / `index`。
9. **禁止用裸指针持有 `CallBase`。** 统一 `sptr<CallBase>`，且**每次解引用前判空**——通话对象会被并发线程从全局表中移除。
10. **禁止新增 `.cpp` 而不同步加入 `callmanager.gni` 的 `call_manager_sources`。** 该数组同时供服务库与全部 gtest 目标使用。

### 必须停下来找人确认的改动（Ask before）

红线是「不要做」，这一节是「**做之前先停下来，不要自行判断后直接提交**」。碰到以下任一类，在最终回复与 commit message 里显式标注「需人工复核」，说明理由与影响面：

| 触发条件 | 需要人工复核的原因 |
|---|---|
| 修改 `*_ipc_interface_code.h` 中**已有**枚举值或其顺序 | 跨部件 ABI，影响面超出本仓，无法在本仓验证 |
| 修改 `@ohos.telephony.call.d.ts` 中**已发布**接口的签名或语义 | 对外 API 兼容性，影响所有应用 |
| 收紧或放宽任何 `CheckPermission` 逻辑 | 放宽=越权风险，收紧=正常功能失败，两个方向都是安全事件 |
| 改变跨部件结构体（如 `CallAttributeInfo`）的字段顺序或含义 | 与未重编的对端错位 |
| 改动 ECC / 紧急呼叫路径 | 最高等级事故面，见 [`high-risk.md`](docs/agents/high-risk.md)「高危入口（改动这些位置，事故等级最高）」 |
| 修改既有 feature 宏的默认值 | 影响所有未显式配置的产品，且无灰度手段 |
| 新引入第三方依赖 | license 合规审核 |
| 复用已有错误码表达新含义 | 应用的错误分支判断会失效 |

> 判据：**这个改动的后果能不能在本仓内验证？** 不能，就属于本节。

---

## 最小验证闭环（DoD）

任何改动，按顺序完成以下四步才算完成。详细命令见 `docs/agents/build-and-test.md`。

**第 1 步 · 静态自检（无需构建环境，必做）**
- 新增源文件已加入 `callmanager.gni:call_manager_sources`；新增 include 目录已加入 `call_manager_include_dirs`。
- 新增 IPC 接口的六处改动齐全（见 `boundaries.md`「新增 IPC 接口的六步清单」）。
- 新增 feature 宏同时出现在 `callmanager.gni` 的 `declare_args()` 与 `bundle.json` 的 `features`。
- 被 `#ifdef` 包裹的代码，在宏关闭时仍能编译（检查宏外是否引用了宏内声明的成员）。

**第 2 步 · 构建**
```shell
# 全量部件
./build.sh --product-name <product> --build-target call_manager --ccache
# 仅本仓单测
./build.sh --product-name <product> --build-target "//base/telephony/call_manager/test/unittest:unittest"
```

**第 3 步 · 单元测试（首选 zero_gtest 白盒套）**
```shell
# zero 系列把 call_manager_sources 直接编进测试二进制，无需拉起 SA，是最快的验证路径
./build.sh --product-name <product> \
  --build-target "//base/telephony/call_manager/test/unittest/call_manager_zero_gtest:tel_call_manager_zero9_gtest"
hdc shell /data/test/tel_call_manager_zero9_gtest --gtest_filter=<YourCase>*
```

> **关于设备操作与破坏性命令（已评估，本仓不涉及，此处说明边界）**：
> 本仓全部验证手段都是**只读或沙箱内**的——`hdc shell` 跑 gtest 二进制、`hidumper -s 4005`、`hilog`、`hisysevent -r`，都不改设备持久状态。
> **本仓的验证流程里不存在、也不需要任何刷机、恢复出厂、`hdc shell rm`、改写 `/system` 分区、重置 SIM 或清空通话记录数据库的操作。**
> 如果准备执行的命令会改变设备持久状态，说明方向有误：停止操作，按本文「必须停下来找人确认的改动（Ask before）」标注需人工确认。
> 唯一一类容易误伤的是通话记录（走 DataShare 落库），调试时不要为了"清理环境"直接删除数据库，改用新的 `callId` 或换测试号码。

**第 3.5 步 · 静态检查**

本仓**没有部件内独立的 lint / codecheck 目标**（`bundle.json` 未定义，`BUILD.gn` 也没有）。

不要去找不存在的 lint 命令。静态检查依赖两件事：编译告警（`-Wunused -Wunreachable-code` 全局开启，**不要留新告警**）与 `build-and-test.md`「无 OHOS 构建环境时的静态自检」中的 13 项自检。仓库级规范检查由 OpenHarmony 门禁的 codecheck 负责，本地无法执行。

**第 4 步 · DoD 判定**
- [ ] 目标行为在**最小复现场景**下被验证（场景写进 commit message）。
- [ ] 新增/修改至少一条 gtest 用例覆盖该分支，且用例名能自解释触发条件。
- [ ] 回归面已确认：改动触及的模块在 `high-risk.md`「变更风险矩阵」中查过，关联场景已人工或用例覆盖。
- [ ] 若改动涉及标志位，已确认**置位路径与清位路径成对**（见 `failure-modes.md`「最高频失效模式：粘滞标志位（sticky latch）」）。
- [ ] 日志中无隐私字段，无高频 `TELEPHONY_LOGI`（通话路径每秒多次调用的函数里输出 info 日志会导致日志量激增）。

**第 5 步 · 最终回复必须包含（强制）**

给用户/评审的最终回复里，以下五项缺一不可。**没做的事要明确说没做，不要省略。**

1. **改了什么**：文件清单 + 每处改动一句话说明为什么改在这里。
2. **本文「动手前必须先自陈（强制）」中的三行自陈**：任务类别 / 已读文档 / 命中约束。
3. **DoD 逐项结果**：第 1～4 步每一条是「已验证 / 未验证」，未验证的写明原因。
4. **验证边界**：没有 OpenHarmony 构建环境时，必须明说「仅完成静态自检，未实际编译、未跑测试」，不要用"应该可以编过"这类措辞含混过去。
5. **遗留风险**：命中本文「必须停下来找人确认的改动（Ask before）」需人工复核的项、发现但未修的技术债、[`high-risk.md`](docs/agents/high-risk.md)「变更风险矩阵」中未覆盖的回归面。

> 如实报告未验证项在本仓尤其重要：通话问题多与时序相关，一条「未验证 X」的说明，比一份看似完整的汇报更有价值。

---

## 分层文档索引

| 文档 | 内容 | 什么时候读 |
|---|---|---|
| [`docs/agents/code-map.md`](docs/agents/code-map.md) | 目录分层、模块职责、关键路径映射、任务→路径定位 | 第一次进入某个模块 |
| [`docs/agents/boundaries.md`](docs/agents/boundaries.md) | 模块依赖规则、调用链边界、状态流转约束、隐式约束 | **动手改代码前必读** |
| [`docs/agents/high-risk.md`](docs/agents/high-risk.md) | 高危入口、高危配置项、高危事件序列、变更风险矩阵 | 改动涉及 IPC/宏/多路通话/蓝牙时 |
| [`docs/agents/failure-modes.md`](docs/agents/failure-modes.md) | 失效模式（粘滞标志位、队列阻塞、空指针、重入、兼容性断裂）与根因模式分类 | 排查 bug 时 |
| [`docs/agents/fix-and-verify.md`](docs/agents/fix-and-verify.md) | 修复点选择原则、错误修法反例、最小复现、回归用例 | 确定根因之后、动手之前 |
| [`docs/agents/observability.md`](docs/agents/observability.md) | 关键日志点、log domain、dump 命令、HiSysEvent 打点 | 定位现场问题 |
| [`docs/agents/build-and-test.md`](docs/agents/build-and-test.md) | 构建目标、测试套分类与选择、测试编写约定 | 验证阶段 |
