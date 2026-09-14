# 高风险知识：高危入口、高危配置、高危序列、变更风险矩阵

← 返回 [`AGENTS.md`](../../AGENTS.md)

---

## 高危入口（改动这些位置，事故等级最高）

按事故严重度排序。改动前先确认是否真的必须改。

| 等级 | 入口 | 位置 | 事故形态 |
|---|---|---|---|
| **S** | 紧急呼叫路径 | `CallPolicy` 中一切 `isEcc` 相关分支、`CallRequestProcess::EccDialPolicy()`、`HandleEccCallForDsda()` | 紧急电话拨不出。任何新增的通用校验若未显式豁免 ECC，都会命中 |
| **S** | IPC 接口码枚举 | `interfaces/innerkits/*_ipc_interface_code.h` | 枚举值即 ABI。修改已有值会让新旧部件静默错分发，调 A 执行 B |
| **S** | 上行状态机的 Handle 函数 | `call_status_manager.cpp` 的 `*Handle()` | 阻塞或异常会让整条上行链路停摆，全部通话功能不可用 |
| **A** | 鉴权入口 | `call_manager_service.cpp` 各 API 开头的 `CheckPermission` | 漏判=越权；多判=正常功能失败（如拒接必须带 `SEND_MESSAGES` 才拦） |
| **A** | 音频路由决策 | `AudioControlManager::UpdateDeviceType()` / `SetAudioDevice()` | 通话无声、声音走错设备。用户感知最强 |
| **A** | 全局通话对象表 | `CallObjectManager` 的增删与查找 | 通话对象丢失或错配，界面与实际不一致 |
| **A** | 多路通话仲裁 | `CallRequestProcess::HandleCallWaitingNum*()`、`HandleDsdaIncomingCall()`、`CallPolicy::CheckCallLimit()` | 第二路来电被误拒、通话被误挂 |
| **A** | 观察者广播 | `CallStateListener` 的锁与遍历 | 自死锁 → 全部通话状态停摆 |
| **B** | 通话记录写入 | `call_records_manager.cpp` / `call_data_base_helper.cpp` | 通话记录丢失或重复 |
| **B** | 来电拦截 | `call_incoming_filter_manager.cpp`、`spam_call/`、`antifraud/` | 误拦正常来电 |
| **B** | 蓝牙 HFP 状态同步 | `bluetooth_call_connection.cpp`、`bluetooth_call_policy.cpp` | 车机/耳机显示与实际不一致 |
| **B** | 通话 UI 拉起 | `call_connect_ability.cpp` | 来电界面不弹出 |

---

## 高危配置项

### `callmanager.gni` 的 feature 宏

改动 `declare_args()` 的默认值会影响**所有未显式配置的产品**。新增宏必须同时登记到 `bundle.json` 的 `component.features`。

| 宏 | 默认 | 影响面 |
|---|---|---|
| `call_manager_feature_support_dsoftbus` | `true` | **整个 `services/distributed_call/` 与 `services/interoperable_call/` 目录**是否编入，外加 `device_manager` / `dsoftbus` 外部依赖。关掉会少编 16 个源文件 |
| `call_manager_feature_support_hearing_aid` | `true` | 蓝牙助听器设备类型（`DEVICE_BLUETOOTH_HEARING_AID`）是否参与音频路由 |
| `call_manager_call_transfer` | `true` | 通话转移能力，额外编入 `transfer_control_callback_proxy.cpp`，并打开 `CallStatusManager` / `CallObjectManager` 内多处 `#ifdef` 分支 |
| `call_manager_feature_support_rtt` | `false` | 实时文本。打开后额外编入 `rtt_call/` 与 `rtt_call_listener.cpp`，并注册一个新的状态订阅者 |
| `call_manager_feature_hfp_async_enable` | `false` | HFP 异步化，改变蓝牙状态同步时序 |
| `call_manager_feature_not_support_multicall` | `false` | 打开后**单路通话**语义，接听新来电会挂断旧通话（`HangUpOtherCall`） |
| `call_manager_sos_no_ringback_tone` | `false` | 紧急呼叫不放回铃音 |
| `call_manager_watch_call_blocking` | `false` | 手表侧通话拦截，会动态加载外部 watch 服务库 |
| `call_manager_thermal_protection` | `false` | 高温时主动挂断通话 |

### 由外部部件存在性推导的宏

这类宏**不在 `declare_args()` 里**，而是由 `global_parts_info` 推导。它们在不同产品上自动开关，是「同样代码换个产品就不对」的常见原因：

| 宏 | 条件 |
|---|---|
| `ABILITY_BLUETOOTH_SUPPORT` | `communication_bluetooth` 存在 |
| `ABILITY_SMS_SUPPORT` | `telephony_sms_mms` 存在 |
| `ABILITY_CELLULAR_SUPPORT` | `telephony_cellular_call` 存在 |
| `ABILITY_POWER_SUPPORT` | `powermgr_power_manager` 存在 |
| `SUPPORT_VIBRATOR` | `sensors_miscdevice` 存在 |
| `CELLULAR_DATA_SUPPORT` | `telephony_cellular_data` 存在 |
| `HICOLLIE_ENABLE` | `hiviewdfx_hicollie` 存在 |
| `TELEPHONY_CUST_SUPPORT` | `telephony_telephony_enhanced` 存在 |
| `OHOS_BUILD_ENABLE_TELEPHONY_CUST` | `telephony_telephony_cust` 存在 |
| `OHOS_SUBSCRIBE_MOTION_ENABLE` | `msdp_motion` 存在 |
| `OHOS_SUBSCRIBE_USER_STATUS_ENABLE` | `msdp_user_status_awareness` 存在 |
| `ABILITY_SCREENLOCKMGR_SUPPORT` | `theme_screenlock_mgr` 存在 |
| `SUPPORT_MUTE_BY_DATABASE` | `SUPPORT_DSOFTBUS` 且 `target_platform == "watch"` |
| `CALL_MANAGER_AUTO_START_OPTIMIZE` | `device_name == "rk3568"`。**不是仅测试目标。** 它在三处 `BUILD.gn` 被定义：`frameworks/native/BUILD.gn`（`tel_call_manager_api`，即对外发布的客户端 SDK，属 `bundle.json` 的 `inner_kits`）、`test/unittest/BUILD.gn`、`test/unittest/call_manager_zero_gtest/BUILD.gn`。它真正 gate 的是**生产代码**：`frameworks/native/src/call_manager_proxy.cpp` 与 `include/call_manager_proxy.h` 中共 9 处 `#ifdef`。改动该宏内的代码，影响所有链接本 SDK 的进程，不能按"只影响测试"评估影响范围 |

> **规则：`#ifdef` 内声明的成员，其全部引用点必须在同一宏内。** 改动带宏的代码后，至少确认一次「这个宏关闭时能否编译通过」。

### 运行期配置项

| 配置 | 读取位置 | 风险 |
|---|---|---|
| `const.telephony.read_set_voip_call_info` | `CallControlManager` | 控制 VoIP 通话信息是否可设置 |
| OOBE 开机向导状态 `deviceProvisioned_` | `CallStatusManager::GetDevProvisioned()` | 未完成向导时来电行为不同 |
| 佩戴状态 `wearStatus_` | `WearStatusObserver` | 手表未佩戴时走 `PlayForNoRing` |
| 勿扰模式 / 专注模式 | `IsNeedSilentInDoNotDisturbMode()` / `IsFocusModeOpen()` | 静音来电 |
| 超级隐私模式 | `services/call/super_privacy/` | 禁拨禁接 |
| EDM 管控名单 | `EdmCallPolicy`（`SetEdmPolicy` / `IsDialingEnable` / `IsIncomingEnable`） | 企业设备拨号允许列表 |
| 铃声模式 `AudioRingerMode` | `AudioProxy::GetRingerMode()` | NORMAL/VIBRATE/SILENT 三种模式各有独立分支 |
| SA 4005 启动方式 | `sa_profile/4005.json` vs `4005_auto_start_optimize.json` | `run-on-create` true/false，影响首次通话时延 |

---

## 高危事件序列

以下序列是历史缺陷密集区。改动相关模块时**必须**把对应序列逐条验证。

| 序列 | 为什么危险 |
|---|---|
| 彩振来电 → 接听 | 振铃阶段强制外放，接听后必须撤销；涉及 `initCrsDeviceType_`、扬声器模式、提示音恢复三处状态 |
| 外设先连 → 再来电 → 接听 / 来电中途连外设 → 接听 | 设备优先级与"用户是否手选"两条路径交叉 |
| 通话中 → 第二路来电 → 接听/拒接 | `WaitingHandle` + 自动保持 + 等待提示音，多路仲裁最复杂的一段 |
| 通话保持 → 来电 → 切换 | `HoldCall` / `SwitchCall` / `AutoUnHoldForDsda` 交织 |
| DSDA 双卡分别来电 | `index` 跨卡重复，必须带 `slotId` 查找 |
| 视频通话 ↔ 语音通话 互转 | 音频路由的目标设备优先级在视频/语音下不同（视频优先外放） |
| 拨号中立即挂断 / 拨号失败 | `dialSrcInfo_` 单槽位、`DealFailDial()`、`pendingHangupHandle_` |
| 分布式迁移中挂断 / 对端接听 | `GetAnsweredByPhone()` 为 `false` 的分支覆盖不足 |
| 蓝牙连接 → 断开 → 重连（通话中） | `isHfpConnected_`、`isBtScoConnected_`、SCO 临时禁用标志三处状态 |
| 紧急呼叫（含飞行模式 / 无卡 / DSDA） | 绕过多数校验，分支独立 |
| 通话中 VoIP 来电 / VoIP 通话中运营商来电 | `CarrierAndVoipConflictProcess()`，VoIP 不在主对象表里 |
| SA 被卸载后重拉 | 外部代理全部失效，`SystemAbilityListener` 重建；已有通话状态如何恢复 |
| 最后一路通话结束 | **全部单例标志位的统一清位时机**，漏清就跨通话泄漏 |

---

## 变更风险矩阵

| 改动类型 | 风险 | 灰度手段 | 必须的验证 |
|---|---|---|---|
| 新增 feature 宏包裹的新能力 | 低 | **宏本身即开关**，默认 `false` 合入 | 宏开/宏关各编一次 |
| 修改既有 feature 宏默认值 | 高 | 无法灰度，直接影响全部未显式配置的产品 | 全量回归 + 与产品侧确认 |
| 新增 IPC 接口（末尾追加） | 低 | 旧客户端不调用即不受影响 | [`boundaries.md`](boundaries.md)「新增 IPC 接口的六步清单」 |
| 修改 IPC 已有接口的参数 | **极高** | 无灰度手段 | 必须新增接口而非修改；旧接口保留 |
| 音频路由逻辑调整 | 高 | 无 | `fix-and-verify.md`「回归面判定」中的音频回归项 |
| 状态机新增一条边 | 中 | 无 | 该边 + 反向边 + 相邻状态 |
| 状态机修改既有边的副作用 | 高 | 无 | 全部经过该边的序列（见本文「高危事件序列」） |
| 新增标志位 | 中 | 无 | 置位 + 三条清位路径 |
| 新增订阅者 | 中 | 可用宏包裹 | 确认回调不阻塞、不重入 `AddOneObserver` |
| 新增外部部件依赖 | 中 | 用 `global_parts_info` 条件包裹 | 该部件不存在时能编过能运行 |
| 日志/打点调整 | 低 | 无 | 确认无隐私字段、无高频 info |
| 纯重构（不改行为） | 中 | 无 | 重构前后 zero_gtest 全绿 |

### 本仓可用的灰度开关只有两种

1. **编译期 feature 宏**（`callmanager.gni`）——新能力默认 `false`，按产品逐步打开。这是本仓唯一成熟的灰度路径。
2. **运行期配置项**（DataShare 设置项 / 系统参数）——如 `const.telephony.*`、OOBE 状态、EDM 名单。适合可后台下发的策略类改动。

> **没有基于用户分组的 A/B 灰度能力。** 因此高风险改动（音频路由、状态机既有边、IPC 已有接口）只能靠回归覆盖保证，**要么用新宏包起来，要么就得把本文「高危事件序列」中的相关序列全部验证一遍**。

---

## 改动前自检

动手改任何本文「高危入口（改动这些位置，事故等级最高）」列出的入口之前，先回答：

1. 这个改动能不能放进一个默认关闭的 feature 宏？能就放进去。
2. 它触发的是本文「高危事件序列」中的哪几条？逐条列出。
3. 它涉及哪些标志位？置位与清位路径分别在哪？
4. 它是不是 ECC 路径的上游？如果是，ECC 是否需要豁免？
5. 它改了跨部件的东西吗（IPC 码、结构体、错误码、`.d.ts`）？如果是，是"追加"还是"修改"？只允许追加。
