# 运维观测：日志、打点、dump

← 返回 [`AGENTS.md`](../../AGENTS.md)

---

## 日志标识

| 项 | 值 | 定义位置 |
|---|---|---|
| 服务库 log tag | `CallManager` | `BUILD.gn` 的 `defines` |
| 服务库 log domain | `0xD001F10` | `BUILD.gn` |
| 客户端 SDK tag | `CallManagerApi` | `test/unittest/BUILD.gn`、`frameworks/native/BUILD.gn` |
| 单测 tag / domain | `CallManagerGtest` / `0xD000F00` | 各 gtest 的 `BUILD.gn` |
| 进程名 | `telecom` | `sa_profile/4005.json` |
| SA ID | `4005` | 同上 |

抓日志的起点：

```shell
hdc shell hilog -w start                       # 开始落盘
hdc shell hilog | grep -i callmanager          # 实时过滤
hdc shell hilog -D 0xD001F10                   # 按 domain 过滤（只看服务侧）
hdc shell "ps -ef | grep telecom"              # 确认进程在
```

## 日志宏与用量

> 下列用量为快照值，只用于判断**相对比例**（错误日志远多于信息日志），不要当作精确断言引用。
> 统计口径：全仓 `*.cpp` + `*.h`，按出现次数计。

| 宏 | 量级 | 用法 |
|---|---|---|
| `TELEPHONY_LOGE` | ~3300 | 错误。本仓错误日志占比最高，**排查时先只看 E** |
| `TELEPHONY_LOGI` | ~1600 | 关键状态变化。**不要在通话路径高频函数里加**，会导致日志量激增 |
| `TELEPHONY_LOGW` | ~210 | 可恢复异常 |
| `TELEPHONY_LOGD` | ~60 | 调试。默认不输出，新增详细日志用它 |
| `HILOG_COMM_INFO` / `HILOG_COMM_WARN` / `HILOG_COMM_ERROR` | ~13 | 通话公共域日志，主要用在音频设备变更等跨域场景 |

**隐私约束（这一条是可复核的硬事实，不是快照值）**：全仓 `%{private}` 用量为 **0**。
约定是**号码、联系人名、地址等隐私字段根本不进日志**，而不是用 `%{private}` 包起来。

需要定位具体通话时用 `callId` / `slotId` / `index`，不要打号码。

---

## 关键日志观测点

排查问题时按链路顺序看这几组日志，能快速判断断在哪一环。

### 下行链路

| 观测点 | 文件 | 判断 |
|---|---|---|
| `dialInfo slotId is[...]` | `call_control_manager.cpp` `DialCall` | 请求是否到达服务 |
| Dial policy 失败 | `CallPolicy` 各 `XxxPolicy` 的 `TELEPHONY_LOGE` | 是否被策略拦截，拦在哪一条 |
| `DialCall failed!` | `call_control_manager.cpp` | 异步派发是否成功 |
| `CellularCallConnection` 的返回码 | `cellular_call_connection.cpp` | 是否成功下发到协议栈 |

### 上行链路

| 观测点 | 文件 | 判断 |
|---|---|---|
| `CallStatusCallbackStub` 入口 | `call_status_callback_stub.cpp` | 协议栈是否上报了 |
| 各 `*Handle()` 的入口日志 | `call_status_manager.cpp` | 状态机走到哪一步 |
| `control audio ... callId, priorState, nextState` | `audio_control_manager.cpp` | 状态广播是否到达音频模块 |

> **队列阻塞的判定方法**：上行全部在单队列 `report_call_info_queue` 上串行。看**相邻两条上行日志的时间戳间隔**，间隔异常大的那一步就是阻塞点。见 [`failure-modes.md`](failure-modes.md)「队列阻塞与状态停摆」。

### 音频链路

音频问题按这个顺序看，能直接定位到是"决策错"还是"执行错"：

| 观测点 | 输出 |
|---|---|
| `before CRS, initDeviceType = %d` | 彩振前记录的原始设备 |
| `InitAudioDeviceType is %d` | 当前算出的目标设备 |
| `GetCurrentAudioDevice: %d, initDeviceType: %d` | **当前设备 vs 目标设备**，两者不等却没切换，说明被某个标志位挡住了 |
| `set audio device, type: %d` | `SetAudioDevice` 实际执行 |
| `update deviceType = %d, initCrsDeviceType_ %d` | 彩振期间的设备更新 |
| `crs ring tone should be speaker` | 彩振强制外放（**这条出现后必须有对应的撤销**） |
| `answer call set speaker deactive` | 扬声器模式撤销 |
| `process event inner, event: %d` | 音频场景状态机迁移 |

**典型判读**：出现 `GetCurrentAudioDevice: 1, initDeviceType: 7`（当前听筒、目标星闪）但后面没有 `set audio device, type: 7`，说明决策算对了但被拦住了——去查 `isSetAudioDeviceByUser_` 与 `IsSpeakerMode()` 两个标志位。

---

## dump 命令

服务实现了 `CallManagerDumpHelper`，可在线查看运行状态：

```shell
hdc shell hidumper -s 4005
hdc shell hidumper -s 4005 -a "-call_manager_info"
```

支持的选项（`call_manager_dump_helper.cpp::ShowHelp`）：

| 选项 | 作用 |
|---|---|
| `-call_manager_info` | 全部通话管理信息 |
| `-input_simulate <event>` | 模拟输入事件（`login`/`logout`/`token_invalid`） |
| `-output_simulate <event>` | 模拟输出事件 |
| `-show_log_level` | 查看当前日志级别 |
| `-set_log_level <level>` | **动态调日志级别**，排查时先用这个打开 debug |
| `-perf_dump` | 性能统计 |

`-call_manager_info` 输出包含：服务绑定时间、启动耗时、服务运行状态、各卡槽是否有 SIM、当前通话状态、振铃通话状态。**排查"状态停滞"时第一时间 dump 一次**，确认服务自身认为的状态与界面是否一致。

---

## HiSysEvent 打点

`CallManagerHisysevent`（`services/hisysevent/`）负责埋点，domain 为 `KEY_CALL_MANAGER = "callmanager"`。

### 故障类打点（排查线上问题的主要依据）

| 打点 | 触发 |
|---|---|
| `WriteDialCallFaultEvent` | 拨号失败，带 `slotId` / `callType` / `videoState` / 错误码 / 原因串 |
| `WriteAnswerCallFaultEvent` | 接听失败 |
| `WriteHangUpFaultEvent` | 挂断失败 |
| `WriteIncomingCallFaultEvent` | 来电处理失败 |
| `ReportCallDropChrEvent` | **掉话**，带 `slotId` / `callIndex` / `dropReason` |

### 行为类打点

| 打点 | 触发 |
|---|---|
| `WriteCallStateBehaviorEvent` | 通话状态变更 |
| `WriteIncomingCallBehaviorEvent` | 来电 |
| `WriteIncomingNumIdentityBehaviorEvent` | 号码标记与拦截结果 |
| `WriteVoipCallStatisticalEvent` / `RecordVoipProcedure` | VoIP 通话全流程 |
| `ReportCallProcedureEvents` | 通话流程事件（JSON 串，从 `CallStatusManager::ReportCallProcedureEvents` 进） |

### 隐私模式 UE 打点

`HiWriteBehaviorEventPhoneUE` 配合以下事件名（`call_manager_hisysevent.h`）：

`CALL_DIAL_IN_SUPER_PRIVACY`、`CALL_ANSWER_IN_SUPER_PRIVACY`、`CALL_REJECT_IN_SUPER_PRIVACY`、`CALL_DIAL_CLOSE_SUPER_PRIVACY`、`CALL_INCOMING_REJECT_BY_SYSTEM`

查看打点：

```shell
hdc shell hisysevent -r -d callmanager
```

### 新增打点的约定

- 故障类走 `Write*FaultEvent`，必须带错误码与可读原因串。
- **新增功能若有失败分支，必须补故障打点**，否则线上无法定位。
- 打点参数同样不得包含号码等隐私字段；`VoipProcedure` 系列用 `callId` 作为关联键。

---

## 其他观测手段

| 手段 | 用途 |
|---|---|
| `hitrace` | `BUILD.gn` 依赖 `hitrace:hitrace_meter`、`libhitracechain`。用 `hdc shell hitrace --trace_begin telephony` 抓通话时序 |
| XCollie | `HICOLLIE_ENABLE` 下，`CallManagerServiceStub` 对每个 IPC 处理设 30s 超时看护。日志里出现 XCollie 告警说明某个 `OnXxx` 处理超时 |
| 进程状态 | `hdc shell "ps -ef \| grep telecom"`；进程反复重启说明有崩溃，配合 faultlog |
| faultlog | `hdc shell "ls /data/log/faultlog/faultlogger/"` 取崩溃栈 |

---

## 排查动作顺序（推荐）

1. `hidumper -s 4005 -a "-call_manager_info"` —— 服务自身认为的状态是什么。
2. `hilog -D 0xD001F10` 只看服务侧日志，先过滤 `TELEPHONY_LOGE`。
3. 按本文「关键日志观测点」中的链路顺序确认断点在下行、上行还是订阅者。
4. 若疑似状态停滞，比对上行日志时间戳间隔（本文「上行链路」）。
5. 若是音频问题，直接跳到本文「音频链路」中的判读表。
6. `hisysevent -r -d callmanager` 看有没有对应的 Fault 打点。
7. 定位到模块后，转 [`failure-modes.md`](failure-modes.md)「快速分诊表」。
