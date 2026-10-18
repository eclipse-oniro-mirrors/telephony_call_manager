# 失效模式与根因模式

← 返回 [`AGENTS.md`](../../AGENTS.md)

> 排查 bug 时从这里开始。本仓的问题高度集中在少数几种模式上，先按这里的清单排除，比通读代码快得多。

---

## 最高频失效模式：粘滞标志位（sticky latch）

**这是本仓最高频的根因，排查任何「时序相关 / 第二次才复现 / 换个顺序就好了」的问题都先查这一条。**

### 现象特征

满足以下任一条，即可优先怀疑粘滞标志位：

- 第一通电话正常，**第二通电话开始不正常**。
- 走「常规顺序」正常，**插入一个中间状态后**（来电→保持→再来电、彩振→接听、蓝牙连接→断开→再连接）就不正常。
- 现象是「功能停在上一个阶段该有的样子」，比如通话已接通但还在外放、挂断后铃声继续、耳机连上了但声音走听筒。
- 重启服务后恢复正常。

### 为什么本仓特别容易中招

`services/` 下有约 **90 个** `isXxx_` 形态的布尔成员，其中多数挂在 `DelayedSingleton` 单例上——**生命周期是进程级，而它们描述的却是单通电话或单个阶段的状态**。

密度最高的几处：

| 位置 | 标志位 |
|---|---|
| `audio_control_manager.h` | `isLocalRingbackNeeded_`、`isCrsVibrating_`、`isCrsStartSoundTone_`、`isVideoRingVibrating_`、`isSetAudioDeviceByUser_`、`isScoTemporarilyDisabled_`、`isPlayForNoRing_`、`isIncomingConflict_` |
| `audio_device_manager.h` | `isBtScoDevEnable_`、`isDCallDevEnable_`、`isWiredHeadsetDevEnable_`、`isSpeakerDevEnable_`、`isEarpieceDevEnable_`、`isEarpieceAvailable_`、`isSpeakerAvailable_`、`isWiredHeadsetConnected_`、`isBtScoConnected_`、`isDCallDevConnected_`、`isAudioActivated_` |
| `audio_player.h` / `ring.h` | `isStop_`、`isRingStop_`、`isToneStop_`、`isSoundStop_`、`isMutedRing_`、`isRingStopped_` … |
| `call_object_manager.h` | `isFirstDialCallAdded_`、`needWaitHold_` |
| `call_base.h` | `isSpeakerphoneOn_`、`isMuted_`、`isAnswered_`、`isAnsweredByPhone_`、`isApCauseReported_` |
| `bluetooth_call_connection.h` | `isHfpConnected_`、`isBtCallScoConnected_` |
| `distributed_call_manager.h` | `isCallActived_`、`isSwitching_` |
| `spam_call_adapter.h` | `isBlock_`、`isQueryComplete_`、`isRefreshMarkInfo_` |

除此之外还有**非布尔的隐式锁存**，同样算这一类：

- `AudioDeviceManager::callAudioMode_`（配合 `IsSpeakerMode()` / `SetSpeakerDeactive()` 读写）
- `AudioControlManager::initCrsDeviceType_`（彩振前的原始设备）
- `AudioControlManager::frontCall_`（前台通话指针）
- `CallStatusManager::callDetailsInfo_[SLOT_NUM]` / `priorVideoState_[SLOT_NUM]`

### 排查清单

对怀疑的标志位，逐条核对：

1. **置位路径有几条？清位路径有几条？** 置位 3 处、清位 1 处，基本就是 bug。
2. **清位是否覆盖异常退出？** 只在「正常挂断」清位，不覆盖「拨号失败 / 被拒接 / 协议栈异常断开 / 服务被拉起时已有通话」。
3. **最后一路通话结束时是否归零？** 单例标志位必须在「不再有任何通话」时回到初值。`AudioControlManager::CallStateUpdated()` 末尾的 `if (totalCalls_.empty())` 就是这类统一清位的位置。
4. **阶段性强制值是否在离开阶段时撤销？** 典型：彩振/视频/来电阶段强制切扬声器，接听后必须撤销；否则下游 `IsSpeakerMode()` 读到陈旧的 `true`，把用户路由到听筒或扬声器而不是已连接的耳机。
5. **跨线程读写是否 atomic？** 见本文「并发与重入」。

### 修复模板

```text
置位点：进入阶段时置位，并在同一函数里想清楚"谁负责清"
清位点：① 阶段正常结束 ② 阶段异常结束 ③ 最后一路通话结束（统一清位）
```

三个清位点缺一不可。只补①是最常见的"修了但换个路径又复现"。

---

## 队列阻塞与状态停摆

### 现象

来电不响铃、挂断后通话界面不退出、通话状态永久停在某一态、后续所有通话都异常——**且服务进程未崩溃**。

### 根因

上行链路是**单 ffrt 队列** `report_call_info_queue`（见 [`boundaries.md`](boundaries.md)「单队列串行」）。`CallStatusManager` 的任一 Handle 函数阻塞，其后所有状态上报全部排队。

常见阻塞源：

| 阻塞源 | 说明 |
|---|---|
| 同步 IPC 等待 | 在 Handle 里同步调用外部部件并等返回 |
| 同步查 DataShare | 查联系人、查设置项（`SettingsDataShareHelper`）在 DataShare 未就绪时会长时间挂起 |
| 等锁 | 等一个被 IPC 线程持有的锁 |
| 订阅者回调里阻塞 | `CallStateListener` 广播在锁内串行执行，任一订阅者慢，整条链路慢（见 [`boundaries.md`](boundaries.md)「订阅者广播的两条硬约束」） |
| 等待通话对象条件变量 | `CallObjectManager::cv_` |

### 排查方法

看日志时间戳：找 `report_call_info_queue` 上相邻两条 `TELEPHONY_LOGI` 的间隔。间隔异常大的那一步就是阻塞点。另见 [`observability.md`](observability.md)「关键日志观测点」。

### 相关：XCollie 超时

`CallManagerServiceStub` 用 `XCOLLIE_TIMEOUT_SECONDS = 30`（`HICOLLIE_ENABLE` 宏内）看护 IPC 处理。若改动让某个 `OnXxx` 超过 30 秒，会触发 XCollie 上报甚至进程处置。**IPC 处理函数里不得做长耗时同步操作**，要么转异步，要么分段。

---

## 并发与重入

### `ffrt::mutex` 非递归

本仓普遍使用 `ffrt::mutex`，**非递归**。同一线程二次加同一把锁立即死锁。

高危模式：

| 模式 | 位置 |
|---|---|
| 持 `listMutex_` 遍历通话列表时调用另一个也取 `listMutex_` 的静态方法 | `CallObjectManager` |
| 订阅者回调里调 `AddOneObserver` / `RemoveOneObserver` | `CallStateListener::mutex_` |
| 持 `audioModeMutex_` 时回调进 `AudioControlManager` 又取回 `audioModeMutex_` | `AudioDeviceManager` |
| 在 `CallControlManager::mutex_` 保护段内调用会回到 `CallControlManager` 的函数 | `CallControlManager` |

写新代码时：**加锁区间内只调用本文件内、已确认不会回调出去的函数**。需要回调外部就先解锁（`lock.unlock()`，本仓已有此写法，如 `audio_device_manager.cpp:366`）。

### 跨线程标志位必须 atomic

本仓至少有四个线程域会碰同一批状态：

| 线程域 | 来源 |
|---|---|
| IPC 线程 | `CallManagerServiceStub::OnRemoteRequest` 的调用者线程 |
| `report_call_info_queue` | 上行状态处理 |
| 各功能 ffrt 队列 | 音频上报、语音助手、反注册 |
| 系统回调线程 | audio_framework / bluetooth / dsoftbus / DataShare observer 的回调 |

**判据：一个标志位如果被上述两个及以上的线程域读写，就必须是 `std::atomic`**（或以锁保护）。本仓已有正例：`isSetAudioDeviceByUser_`、`isConnected_`/`isConnecting_`/`isDisconnecting_`（`CallConnectAbility`）、`isCallActived_`（`DistributedCallManager`）、`isTransferCall_`（`BluetoothCall`）。

反之，`AudioDeviceManager` 的多数静态布尔量仍是裸 `bool`——**它们是潜在的数据竞争点，改动相关代码时需一并评估**。

### 单例初始化顺序

在单例 A 的**构造函数**里调 `DelayedSingleton<B>::GetInstance()`，会产生初始化顺序依赖，且 B 的构造里若又回到 A 会形成递归构造。**依赖其他单例的初始化一律放进 `Init()`**，由 `CallManagerService` 统一编排。

---

## 空指针

### 通话对象会被并发移除

`GetOneCallObject()` 系列**会返回 `nullptr`**：目标通话已被并发执行的 `DisconnectedHandle` 移除时即出现这种情况。

规则：

- 每次调用后立即判空并返回，不要把判空放到几行之后。
- **不要缓存 `sptr<CallBase>` 跨越任何会触发状态变更的调用**。即使 `sptr` 保证对象不被析构，它也会出现已不在全局表里的情况，此时对它的操作不会产生预期效果（"改了一个已经没人看的对象"）。
- git 历史里连续 5 个 commit 都叫 `fix nullptr bug`，说明这是本仓的持续性问题类别，新代码务必保守。

### 单例获取结果需判空

`DelayedSingleton<T>::GetInstance()` 在进程退出阶段或内存受限时会返回空。本仓约定是**每次取单例后判空**，代码里随处可见：

```cpp
auto audioDeviceManager = DelayedSingleton<AudioDeviceManager>::GetInstance();
if (audioDeviceManager == nullptr) {
    return;
}
```

新代码保持一致。批量取多个单例时一次性判完再用。

### 外部服务代理需判空

`CellularCallConnection`、`CoreServiceConnection`、蓝牙/音频/dsoftbus 代理在对端 SA 未拉起或已死亡时为空。**不能假设 `Init()` 成功后就一直可用**——SA 会被卸载重拉。`SystemAbilityListener::OnRemoveSystemAbility` 就是为此存在的。

---

## 资源泄漏

| 资源 | 泄漏点 | 后果 |
|---|---|---|
| `PowerMgr::RunningLock` | `AcquireIncomingLock` / `AcquireDisconnectedLock` 有 Acquire 无对应 Release 的分支 | 息屏失效、耗电异常 |
| `ffrt::task_handle` | `disconnectHandle`、`pendingHangupHandle_` 未 cancel 就被覆盖 | 旧任务延迟触发，误挂断后续通话 |
| 音频 renderer / capturer | `audioRenderer_` 未 `Release()` 就置空，或异常分支漏停 | 音频通路占用，后续通话无声 |
| 振动器 | `StartVibrator` 后异常路径漏 `StopVibrator`（`isCrsVibrating_` / `isVideoRingVibrating_`） | 持续振动 |
| Ability 连接 | `CallConnectAbility` 的连接未断开 | 通话 UI 残留 |
| 动态库句柄 | `watchSystemServiceHandler_`、antifraud 的 `dia_*` 句柄未 `dlclose` | 句柄泄漏 |
| DataShare observer | `RegisterObserver` 无对应 `UnRegisterObserver`（如 `WearStatusObserver`、`OOBEStatusObserver`） | 回调堆积 |

> **通用检查**：新增任何 Acquire/Start/Register/Connect/Open，立刻搜索对应的 Release/Stop/UnRegister/Disconnect/Close，并确认**异常分支也走到**。本仓 `-fno-exceptions`，没有 RAII 异常安全机制，只能依靠显式配对。

---

## 兼容性断裂

| 断裂类型 | 触发 | 后果 |
|---|---|---|
| IPC ABI | 修改 `*_ipc_interface_code.h` 已有枚举值 | 新旧部件间静默错分发，调 A 执行 B |
| 序列化不对称 | proxy 写入与 stub 读取的顺序/类型/个数不一致 | 读出垃圾值，不报错 |
| 对外 API | 改 `@ohos.telephony.call.d.ts` 已发布接口的签名或语义 | 应用崩溃；OpenHarmony API 门禁会拦截 |
| 结构体扩字段 | 在 `CallAttributeInfo` 等跨部件结构体中间插字段 | 与未重编的对端错位 |
| feature 宏语义 | 改 `callmanager.gni` 中 `declare_args()` 的默认值 | 影响所有未显式配置的产品 |
| 错误码 | 复用已有 `CallManagerErrorCode` 值表达新含义 | 应用的错误分支判断失效 |

**规则：这六类都只能"追加"，不能"修改"或"重排"。**

---

## 根因模式分类

定位到问题后，先归类，再选修复策略（见 [`fix-and-verify.md`](fix-and-verify.md)）。

### 设计类

状态在错误的层被表达。典型：某模块自己维护了一份本应由 `CallObjectManager` 提供的通话状态副本，副本与主状态不同步。

**识别**：同一事实在两处存储。
**修复方向**：删掉副本，改为每次查询主状态。不要去"同步两份副本"。

### 实现类

单点逻辑写错：条件写反、分支漏 case、用错重载、默认参数取错、用 `index` 当 `callId`。

**识别**：可以指着一行说"这里应该是 X 不是 Y"。
**修复方向**：就地改，补一条覆盖该分支的用例。

### 并发类

见本文「并发与重入」。表现为偶现、难复现、加日志就不复现。

**识别**：现象与时序相关且不稳定复现。
**修复方向**：优先改成不共享（参数传递而非成员槽位），其次 atomic，最后才加锁——加锁在本仓容易引入死锁。

### 状态流转类

状态机漏了一条边，或某条边上漏了副作用（漏停音、漏清标志位、漏广播）。

**识别**：某个特定的状态序列才复现；正常序列没问题。
**修复方向**：补齐该边的完整副作用集合，并检查**对称的反向边**是否也漏了。

### 依赖类

外部部件行为变化、SA 未拉起、宏未打开、能力未编入。

**识别**：同样的代码在 A 产品正常 B 产品异常；或某次外部部件升级后开始出现。
**修复方向**：先确认宏与 `bundle.json` 依赖，再考虑改代码。查 `callmanager.gni` 的 `global_parts_info` 条件块。

### 门控类：门开了，但喂给门的状态从来没被填上

**这是实现「某类通话/某种状态不允许做 X」这类需求时最容易犯的错，且自测极难发现。**

需求形如「只要是 P 类通话，就不允许 X」。直觉做法是在 X 的各个入口加 `if (IsP(call)) return ERR;`。
**但这只完成了一半**：还必须确认 `IsP()` 依赖的那个字段，在**所有产生该属性的路径上都被真正写入了**。

门控失效有三种形态，缺一不可地都要查：

| 形态 | 表现 | 查法 |
|---|---|---|
| ① 判据函数本身写错 | 门永远不触发（或永远触发） | 逐行读 `IsP()` / `HasP()`：条件是否写反、类型转换前是否先校验了类型 |
| ② 字段只在部分路径被写入 | 本端主动发起的场景正常，**网络/对端下发的场景失效** | 搜该字段的**全部写入点**，对照「本端主动」「上行上报」「跨端同步」三类来源是否都覆盖 |
| ③ 字段跨线程读写但非 atomic | 偶现、难复现 | 见本文「跨线程标志位必须 atomic」 |

> **真实案例模式**：需求是「RTT 通话任何情况下都不允许被保持」。
> 只在策略层加 `if (IsRttCall(call)) return ERR;` 是不够的——
> ① `HasRttCall()` 的类型判断条件写反（`== TYPE_IMS` 应为 `!= TYPE_IMS`），导致它跳过了唯一能承载 RTT 的通话类型，并对其余类型做了错误的向下转换；
> ② 承载 RTT 状态的字段只在「本端接听」与「本端发起模式变更」时被写入，**协议栈上行上报里带来的 RTT 状态从未被同步到通话对象上**——于是在多数真实场景中，门控读到的永远是默认值，新加的 `if` 一次也不会触发；
> ③ 该字段被上行队列与 IPC 线程同时读写，需要是 `std::atomic`。
> 完整修复必须同时覆盖这三点。只加门控而不修 ①②，代码看起来完全正确，测试也会因为用例里手工 set 了该字段而通过，但真机上功能完全不生效。

**动手清单**：实现任何「基于属性 P 的门控」需求时，先回答三个问题，再写 `if`。

1. `IsP()` 的实现对不对？（读一遍，别假设）
2. P 这个字段有几个写入点？本端主动、上行上报、跨端同步三类来源是否都覆盖？**上行上报这一路最常被漏掉**——它通常在 `services/telephony_interaction/` 里把上报结构体的字段同步到通话对象上。
3. P 跨线程吗？需要 atomic 吗？

---

## 快速分诊表

| 现象 | 常见根因 | 先看 |
|---|---|---|
| 第二通电话才异常 | 粘滞标志位 | 本文「最高频失效模式：粘滞标志位（sticky latch）」 |
| 声音走错设备 | 粘滞标志位（扬声器模式 / 用户意图）或设备优先级表漏改 | 本文「最高频失效模式：粘滞标志位（sticky latch）」、`code-map.md`「音频子系统（`services/audio/`）」 |
| 状态停滞、UI 不退出 | 上行队列阻塞 | 本文「队列阻塞与状态停摆」 |
| 偶现、加日志不复现 | 并发 | 本文「并发与重入」 |
| 进程崩溃 | 空指针 | 本文「空指针」 |
| 耗电/息屏异常/持续振动 | 资源泄漏 | 本文「资源泄漏」 |
| 换个产品形态就不对 | 编译宏 / 依赖 | 本文「依赖类」、`high-risk.md`「高危配置项」 |
| 调接口没反应也不报错 | IPC code 未注册 或 ABI 错位 | `boundaries.md`「新增 IPC 接口的六步清单」、本文「兼容性断裂」 |
| DSDA 双卡串话 | 用 `index` 未带 `slotId` 查找 | `boundaries.md`「`index` / `phoneIndex` 与 `callId` 不可互换」 |
| 紧急电话拨不出 | 新增校验未豁免 ECC | `boundaries.md`「紧急呼叫（ECC）是旁路，不是特例分支」 |
| 新加的限制/门控在真机上完全不生效 | 门控依赖的字段从未被填充，或判据函数写反 | 本文「门控类：门开了，但喂给门的状态从来没被填上」 |
| 拨号无反应且无任何错误码 | `needWaitHold_` 置位后等不到状态迁移 | `boundaries.md`「多路通话仲裁横跨两个文件」 |
