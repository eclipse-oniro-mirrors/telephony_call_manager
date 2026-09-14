# 修复策略与验证回归

← 返回 [`AGENTS.md`](../../AGENTS.md)

> 定位到根因之后、动手之前读这一篇。**本仓最大的返工来源不是找不到 bug，而是修在了错误的位置。**

---

## 修复点选择原则

### 在「决策点」修，不在「执行点」修

本仓的分层是「决策 → 执行」。同一个错误结果，在决策点和执行点都能"改出正确现象"，但只有决策点是对的。

| 关注点 | 决策点（正确修复位置） | 执行点（错误修复位置） |
|---|---|---|
| 该用哪个音频设备 | `AudioControlManager::UpdateDeviceType()` → `UpdateDeviceForForegroundCall()` | `ResumeCrsSoundTone()`、`PlayRingtone()`、`AudioProxy` 里任何地方 |
| 这通电话能不能拨 | `CallPolicy::DialPolicy()` | `CallRequestProcess::CarrierDialProcess()` |
| 这通来电要不要拦 | `CallStatusManager::IncomingFilterPolicy()` / `ShouldBlockIncomingCall()` | 各个通知/铃声模块各自跳过 |
| 该不该上报应用 | `CallAbilityReportProxy` | NAPI 层过滤 |
| 音频场景该切到哪 | `AudioSceneProcessor::ProcessEvent()` | 各 `audio_state/*.cpp` 里互相直接跳转 |

**判据**：如果修复方式是在某个具体功能函数里加一个 `if`，去补救上游传下来的错误输入，通常说明修复位置有误。回头找是谁算出了这个错误输入。

### 在「状态的拥有者」修，不在「状态的消费者」修

一个标志位/字段只有一个拥有者（负责置位与清位的模块）。消费者读到了陈旧值时，**修拥有者的清位逻辑**，不要在消费者里加"如果看起来不对就忽略"的补丁。

> 真实案例模式：彩振来电接听后声音走听筒而不是已连接的耳机。
> **错误修法**：在提示音恢复函数 `ResumeCrsSoundTone()` 里放宽设备恢复条件。这是消费者侧补丁，只覆盖了"经过提示音恢复"这一条路径，换个进入路径（普通来电、视频转语音、蓝牙先连后接听）立刻复现。
> **正确修法**：① 在前台通话设备更新的决策点，把上一阶段（彩振振铃强制外放）遗留的扬声器模式显式撤销；② 让自动路由在覆盖设备前尊重用户手选标志位，并在最后一路通话结束时清零该标志位。修的是**两个标志位的清位路径**，即状态拥有者侧。

### 补齐「对称性」，而不是只补当前路径

本仓的正确修复通常是成对的：

| 新增内容 | 必须同时检查 |
|---|---|
| 一处置位 | 正常清位、异常清位、最后一路通话结束时统一清位（见 [`failure-modes.md`](failure-modes.md)「修复模板」） |
| 一个 Acquire / Start / Register / Connect / Open | 对应的 Release / Stop / UnRegister / Disconnect / Close，且异常分支也要走到 |
| 状态机一条边 | 反向边是否也需要同样的副作用 |
| 一个 `#ifdef` 分支 | 宏关闭时能否编译、行为是否合理 |
| 一个新 IPC 接口 | [`boundaries.md`](boundaries.md)「新增 IPC 接口的六步清单」全部完成 |
| 一个新音频设备类型 | `code-map.md`「音频子系统（`services/audio/`）」列出的七处全部修改 |
| proxy 的一次 `Write` | stub 的对应 `Read`，顺序类型个数一致 |

### 优先"不共享"，其次 atomic，最后加锁

并发问题的修复顺序（见 [`failure-modes.md`](failure-modes.md)「并发与重入」）：

1. **改成不共享**：把成员槽位改成随请求传递的参数，可从根本上消除竞争。
2. **改成 `std::atomic`**：适用于独立的布尔/整型标志位。
3. **加锁**：最后选择。本仓 `ffrt::mutex` 非递归，新增锁容易与既有锁形成死锁或重入。加锁前先画出该锁的持有期内会调用到哪些函数。

---

## 错误修法反例清单

以下修法在本仓都出现过，且都会在评审或回归中被驳回。

| 反例 | 为什么错 | 正确做法 |
|---|---|---|
| 在播音函数里 `SetAudioDevice()` | 越过路由决策收口，只覆盖单条路径 | 修 `UpdateDeviceForForegroundCall()` |
| 在订阅者回调里直接改通话对象状态 | 绕过 `UpdateCallState()`，其他订阅者收不到广播 | 调 `CallControlManager` 的公开方法 |
| 靠调整 `AddOneObserver` 的注册顺序来排序 | `listenerSet_` 是 `std::set`，遍历是指针地址序，注册顺序无效 | 在 `CallStatusManager` 里显式排序 |
| 在 Stub 的 `OnXxx` 里写业务判断 | 违反分层，且绕过鉴权收口 | 放到 `CallManagerService` / `CallControlManager` |
| 用 `sleep` / 轮询等待状态就绪 | 上行是单队列，会整条停摆 | 事件驱动或投递到独立队列 |
| 在 Handle 函数里同步查 DataShare | DataShare 未就绪时会长时间阻塞 | 异步查，或先用缓存值 |
| 给「多路通话计数」类函数传默认参数 | `isIncludeVoipCall` 默认 `true`，VoIP 存在时仲裁结果不同 | 显式传值 |
| 只用 `index` 查通话对象 | DSDA 下两卡 `index` 重复 | 用带 `slotId` 的重载 |
| 在常规校验里未豁免 ECC | 紧急电话拨不出，最高等级事故 | 显式判断 `isEcc` 并豁免 |
| 修改 IPC 枚举已有值来"整理顺序" | ABI 断裂，跨版本静默错分发 | 只在末尾追加 |
| `try/catch` 兜异常 | 全库 `-fno-exceptions` | 返回错误码 |
| 加 `TELEPHONY_LOGI` 打印号码 | 隐私合规问题 | 不打印，或只打印长度/状态 |
| 在通话路径高频函数里加 info 日志 | 日志量激增，掩盖真正的问题 | 用 debug 级别，或只在状态变化时输出 |

---

## 验证与回归

### 最小复现场景要写下来

修复的 commit message 必须包含可执行的复现步骤，格式：

```text
前置条件：<设备形态 / 已连接外设 / 卡状态 / 开关状态>
操作序列：1. … 2. … 3. …
预期：…
实际（修复前）：…
```

**本仓的复现场景通常是"序列"而非"单点"**，因为根因多是粘滞标志位。只写「接听彩振来电」不够，必须写清前置的振铃阶段与外设连接时机。

### 首选 zero_gtest 白盒套

`test/unittest/call_manager_zero_gtest/` 是最快的验证路径：

- 它把 `call_manager_sources` **直接编进测试二进制**，不需要拉起 SA 4005。
- 用 `core_service:ffrt_mocked` 替换真实 ffrt（`external_deps -= ["ffrt:libffrt"]`），队列变同步执行，时序可控。
- 测试文件开头 `#define private public` / `#define protected public`，可以直接读写私有成员——**这正是验证标志位清位逻辑的手段**。

对比：`call_manager_gtest/` 系列偏黑盒，通过 `call_manager_connect.cpp` 连接服务，更接近真实环境，但耗时更长、稳定性更差。**验证标志位、分支覆盖用 zero；验证端到端行为用 gtest 系列。**

### 用例编写约定

沿用既有风格（见 `zero_branch9_test.cpp`）：

```cpp
#define private public
#define protected public
#include "audio_control_manager.h"
#include "gtest/gtest.h"

namespace OHOS::Telephony {
using namespace testing::ext;
constexpr int32_t CRS_TYPE = 2;

class ZeroBranch9Test : public testing::Test {
public:
    void SetUp();
    void TearDown();
    static void SetUpTestCase();
    static void TearDownTestCase();
};

/**
 * @tc.number   Telephony_AudioControlManager_0xx
 * @tc.name     test <被测行为>
 * @tc.desc     Function test
 */
HWTEST_F(ZeroBranch9Test, Telephony_AudioControlManager_0xx, TestSize.Level0)
{
    DialParaInfo info;
    info.accountId = 0;
    sptr<CallBase> call = new IMSCall(info);
    call->SetCallType(CallType::TYPE_IMS);
    call->SetCrsType(CRS_TYPE);

    auto audioControl = DelayedSingleton<AudioControlManager>::GetInstance();
    // 构造前置阶段
    audioControl->VideoStateUpdated(call, VideoStateType::TYPE_VOICE, VideoStateType::TYPE_VIDEO);
    // 触发被测迁移
    audioControl->UpdateDeviceType();
    // 断言标志位已被正确清理
    ASSERT_FALSE(audioControl->isSetAudioDeviceByUser_);
}
} // namespace OHOS::Telephony
```

要点：

- 必须有 `@tc.number` / `@tc.name` / `@tc.desc` 三行注释，`@tc.number` 用 `Telephony_<被测类>_<序号>` 格式。
- 用例名要能自解释触发条件，不要叫 `test001`。
- **单例是跨用例共享的**。用例里改过的标志位要在 `TearDown()` 里复原，否则污染同一二进制里的后续用例——这本身就是粘滞标志位问题的一次演练。
- 断言不要只 `ASSERT_TRUE(true)`。既有代码中存在"只验证不崩溃"的覆盖式用例，**新增用例必须断言被修复的具体状态**。

### 回归面判定

改动落在哪个模块，就按下表逐项回归关联场景：

| 改动模块 | 必须回归的场景 |
|---|---|
| `services/audio/` | 听筒/外放/有线耳机/蓝牙/星闪 五种设备 × 来电接听/拨出/视频转语音/通话中切设备；彩振来电；第二通电话 |
| `services/call/call_status_manager` | 来电接听、来电拒接、拨出接通、对端挂断、本端挂断、通话中来电、DSDA 双卡 |
| `services/call/call_policy` | 正常拨号、紧急呼叫、多路上限、飞行模式、隐私模式、EDM 管控 |
| `services/bluetooth/` | HFP 连接/断开、蓝牙侧接听挂断、蓝牙与手机同时操作 |
| `services/distributed_call/` | 跨端迁移中/迁移后挂断、对端接听 |
| IPC 层 | 新旧版本对端互通（若无法实测，至少确认只做了末尾追加） |
| `callmanager.gni` | 宏开与宏关两种配置都能编过 |

### DoD

见 [`AGENTS.md`](../../AGENTS.md)「最小验证闭环（DoD）」。核心四条：

1. 最小复现场景已验证，并写进 commit message。
2. 至少一条包含实际断言的 gtest 用例覆盖该分支。
3. 回归面按本文「回归面判定」查过。
4. 若涉及标志位，置位与三条清位路径已成对确认。
