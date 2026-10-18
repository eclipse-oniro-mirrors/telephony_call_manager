# 编译与测试

← 返回 [`AGENTS.md`](../../AGENTS.md)

---

## 构建定位

本仓是 OpenHarmony 整仓的一个部件，**不能独立构建**，必须在 OpenHarmony 源码树中，落位于：

```text
<OHOS_ROOT>/base/telephony/call_manager        （bundle.json 的 segment.destPath）
```

| 项 | 值 |
|---|---|
| `part_name` | `call_manager` |
| `subsystem_name` | `telephony` |
| 主产物 | `libtel_call_manager.z.so` |
| 主目标 | `//base/telephony/call_manager:tel_call_manager` |

`bundle.json` 的构建分组：

| 分组 | 目标 |
|---|---|
| `fwk_group` | `frameworks/js:call`、`frameworks/ets/taihe:telephony_call_taihe` |
| `service_group` | `:tel_call_manager`、`sa_profile:call_manager_sa_profile`、`services/deps_adapter:tel_cm_deps_adapter`、`services/etc/init:telecom_trust`、`services/etc/init:telecom.cfg` |
| `inner_kits` | `frameworks/native:tel_call_manager_api`、`frameworks/cj:cj_telephony_call_ffi` |
| `test` | `test/unittest:unittest`、`test/fuzztest:fuzztest` |

### 构建命令

```bash
# 在 OHOS 源码根目录执行

# 整个部件
./build.sh --product-name <product> --build-target call_manager --ccache

# 只构建主服务库
./build.sh --product-name <product> \
  --build-target "//base/telephony/call_manager:tel_call_manager"

# 全部单测
./build.sh --product-name <product> \
  --build-target "//base/telephony/call_manager/test/unittest:unittest"

# 单个测试目标（最快的验证路径）
./build.sh --product-name <product> \
  --build-target "//base/telephony/call_manager/test/unittest/call_manager_zero_gtest:tel_call_manager_zero9_gtest"
```

`<product>` 取决于所用的开发板或模拟器，常见 `rk3568`。

> **注意：**
> `device_name == "rk3568"` 会额外定义 `CALL_MANAGER_AUTO_START_OPTIMIZE`。
> 这个宏**同时影响生产代码与测试目标**，不是只影响测试：它在 `frameworks/native/BUILD.gn`（对外发布的客户端 SDK `tel_call_manager_api`）以及两个测试 `BUILD.gn` 中都被定义，并 gate 了 `frameworks/native/src/call_manager_proxy.cpp` 与 `include/call_manager_proxy.h` 中的 9 处 `#ifdef`。
> 因此**在 rk3568 上编译出的客户端 SDK 与其他产品上的不同**。改动该宏内的代码要按生产改动评估，见 [`high-risk.md`](high-risk.md)「由外部部件存在性推导的宏」。

### 编译选项约束

`BUILD.gn` 的 `cflags_cc` 全库生效，改代码时受这些约束：

```text
-fno-exceptions      禁止 try/catch
-fno-rtti            禁止 dynamic_cast / typeid
-Wunused             未使用变量会告警
-Wunreachable-code   不可达代码会告警
-fstack-protector-all -D_FORTIFY_SOURCE=2 -Os -flto
-fvisibility=hidden  默认隐藏符号，导出需显式标注
```

另有 CFI 加固：`cfi = true`、`cfi_cross_dso = true`、`cfi_vcall_icall_only = true`，以及 `branch_protector_ret = "pac_ret"`。**跨 DSO 的间接调用与函数指针转换要小心**，CFI 会在运行期直接终止进程。测试目标用 `test/unittest/cfi_blocklist.txt` 豁免。

---

## `callmanager.gni` 是全仓构建真源

这是最容易出错的一处。四个数组决定了服务库与**全部测试目标**的构成：

| 变量 | 作用 |
|---|---|
| `call_manager_sources` | 全部业务源文件（约 120 个 `.cpp`） |
| `call_manager_include_dirs` | 全部内部 include 目录 |
| `call_manager_external_deps` | 全部外部部件依赖 |
| `call_manager_defines` | 全部 feature 宏 |

> **关键机制**：`test/unittest/` 下的 gtest 目标普遍写有 `sources += call_manager_sources` 和 `include_dirs += call_manager_include_dirs`。
> 所以**新增一个 `.cpp` 只需要在 `callmanager.gni` 加一行，服务库和全部单测目标同时生效**；反过来，漏加这一行，新代码不会编入服务库，单测也看不到符号，表现为链接期 undefined reference 或运行期行为完全没变。

### 新增源文件清单

1. 在 `callmanager.gni` 的 `call_manager_sources` 加路径（保持字母序）。
2. 若引入了新的 include 目录，加到 `call_manager_include_dirs`。
3. 若引入了新的外部部件，加到 `call_manager_external_deps`，**同时**加到 `bundle.json` 的 `component.deps.components`。
4. 若该文件只在某个 feature 下编译，放进对应的 `if (call_manager_feature_xxx)` 块里。

### 新增 feature 宏清单

1. `callmanager.gni` 的 `declare_args()` 加变量并给默认值。
2. 同文件底部加 `if (xxx) { call_manager_defines += [ "MACRO_NAME" ] }`，需要额外源文件/include 的也在这个块里加。
3. `bundle.json` 的 `component.features` 加同名条目。
4. **宏关闭时必须能编过** —— 检查宏内声明的成员是否被宏外的代码引用。

> **仓内现状与上述第 3 步不一致，属于已知技术债**：`callmanager.gni` 的 `declare_args()` 目前有 **9** 个 feature 变量，而 `bundle.json` 的 `component.features` 只登记了 **8** 个，缺 `call_manager_call_transfer`（该宏默认 `true`，且确实控制 `transfer_control_callback_proxy.cpp` 的编入与多处 `#ifdef`）。
> 这不是文档写错，是仓库本身漏登记。**新增宏时仍按第 3 步登记齐全**，不要以这处遗漏为先例。若本次改动涉及 `call_manager_call_transfer`，可一并补齐登记。

### 生成代码边界

**本仓不提交任何构建期生成的代码产物**，因此不存在「改了生成物、下次构建被覆盖」这类问题。具体到 taihe（ArkTS 1.2）这一层：

| 路径 | 性质 | 能否手改 |
|---|---|---|
| `frameworks/ets/taihe/idl/ohos.telephony.call.taihe` | **IDL，是接口的源真相** | 改这里，接口签名以它为准 |
| `frameworks/ets/taihe/src/*.cpp`、`include/*.h` | 手写实现与适配层 | 可以手改 |
| taihe 工具链根据 IDL 生成的胶水 | 构建期产物，**不入库** | 不存在于仓内，无需也无法手改 |

> 规则：**要改 ArkTS 1.2 的接口形状，改 `.taihe` IDL；要改行为，改 `src/` 下的手写实现。** 不要试图在生成物里找东西。
> 同理，`interfaces/kits/js/@ohos.telephony.call.d.ts` 是手写的对外声明，不是生成的。

### 第三方依赖与许可证

| 规则 | 说明 |
|---|---|
| 新增 `external_deps` 必须两处同步 | `callmanager.gni` 的 `call_manager_external_deps` **与** `bundle.json` 的 `component.deps.components`，漏一处构建报错或运行期缺符号 |
| 只能依赖 OpenHarmony 部件 | 通过部件名引用（如 `bluetooth:btframework`），**禁止直接 vendor 第三方源码进本仓** |
| 第三方库经 `third_party` 引用 | 现有的 `libphonenumber`、`cJSON`、`protobuf`、`openssl`、`json` 都走部件依赖，不在本仓内维护副本 |
| 新引入第三方库属于**需人工复核**的改动 | 涉及 license 合规审核，不要自行决定；按 [`AGENTS.md`](../../AGENTS.md)「必须停下来找人确认的改动（Ask before）」停下来显式标注 |
| 可选依赖用 `global_parts_info` 包裹 | 见 [`high-risk.md`](high-risk.md)「由外部部件存在性推导的宏」，保证该部件不存在时仍能编过 |

---

## 测试套的选择

两套单测目标，用途完全不同，**选错测试套会导致需要拉起服务或无法访问私有成员**。

### `call_manager_zero_gtest`（白盒，首选）

| 特性 | 说明 |
|---|---|
| 构成 | `sources += call_manager_sources`，把全部业务代码**直接编进测试二进制** |
| 是否需要 SA | **不需要**，不用拉起 telecom 进程 |
| ffrt | `external_deps -= ["ffrt:libffrt"]` + `core_service:ffrt_mocked`，**队列变同步执行，时序可控** |
| 私有成员 | 测试文件开头 `#define private public` / `#define protected public`，可直接读写 |
| 目标 | `tel_call_manager_zero1_gtest` … `tel_call_manager_zero10_gtest`，另有 `zero_flash_reminder` / `zero_super_privacy` / `zero_transfer_callback` |
| 适用 | **验证分支覆盖、标志位置位清位、状态机迁移** —— 本仓的常规修复应优先使用它 |

配套的白盒套还有：`call_manager_special_test`（4 个目标）、`call_manager_service_test`（10 个目标）、`call_manager_service_stub_test`、`report_call_info_handler_test`、`antifraud_*_test`、`motion_recognition_test`、`call_voice_assistant_test`、`call_earthquake_alarm_test`、`distributed_communication_test`（需 `SUPPORT_DSOFTBUS`）、`rtt_test`（需 `SUPPORT_RTT_CALL`）。

### `call_manager_gtest`（偏黑盒）

| 特性 | 说明 |
|---|---|
| 构成 | 同样编入 `call_manager_sources`，但额外带 `call_manager_connect.cpp` 与 `privacy_test_common.cpp`，走客户端接口 |
| CFI | `cfi = false`（与 zero 套相反） |
| 目标 | `tel_call_manager1_gtest` … `tel_call_manager8_gtest` |
| 适用 | 端到端接口行为、权限校验 |

### 其他

| 套 | 说明 |
|---|---|
| `ui_client:tel_call_manager_ui_test` | 交互式 UI 客户端测试 |
| `test/fuzztest/` | **20 个目录，但只有 18 个挂进了 `group("fuzztest")`**，详见下方注。新增对外 IPC 接口时应考虑补一个 fuzzer |

> **`test/fuzztest/` 的两个例外（数目录会数错，务必看清）**：
> - `common_fuzzer/` **不是** fuzz 目标，它没有自己的 `BUILD.gn`，只提供共用的 `addcalltoken_fuzzer.cpp/.h`（给各 fuzzer 授权用），被其他 fuzzer 引用。
> - `callvoiceassistantmanager_fuzzer/` **是**一个完整有效的 fuzz 目标（自带 `BUILD.gn`，定义了 `ohos_fuzztest("CallVoiceAssistantManagerFuzzTest")`、源文件与 `corpus/`），但**没有被登记进 `test/fuzztest/BUILD.gn` 的 `group("fuzztest")` 的 `deps`**，因此跟着 `fuzztest` 组构建时它根本不会被编译。这是仓内未登记构建的测试目标，属于已知技术债。
>
> 引申规则：**新增 fuzzer 目录后必须同时在 `test/fuzztest/BUILD.gn` 的 `group("fuzztest")` 里加一行 `deps += [ "xxx_fuzzer:fuzztest" ]`**，否则它静默不参与构建，测试覆盖并未实际增加。这与 [`boundaries.md`](boundaries.md)「新增 IPC 接口的六步清单」中「IPC code 漏注册 → 运行期静默失败」是同一类错误。
| `test/mock/` | 测试替身，zero 套通过 `include_dirs` 引入 |

### 运行

```bash
# 推送并执行（测试二进制安装到 /data/test/）
hdc shell /data/test/tel_call_manager_zero9_gtest

# 只跑某条用例
hdc shell /data/test/tel_call_manager_zero9_gtest \
  --gtest_filter=ZeroBranch9Test.Telephony_AudioControlManager_001

# 列出全部用例
hdc shell /data/test/tel_call_manager_zero9_gtest --gtest_list_tests
```

测试产物路径由 `module_out_path = "call_manager/call_manager/<test_module>"` 决定。

---

## 用例编写约定

见 [`fix-and-verify.md`](fix-and-verify.md)「用例编写约定」中的完整模板。要点复述：

- 文件头 `#define private public` / `#define protected public`，再 include 被测头文件。
- `namespace OHOS::Telephony` + `using namespace testing::ext`。
- 每条用例前三行注释：`@tc.number`（`Telephony_<被测类>_<序号>`）、`@tc.name`、`@tc.desc`。
- `HWTEST_F(<Suite>, <tc.number>, TestSize.Level0)`。
- **单例跨用例共享**：用例里改过的标志位要在 `TearDown()` 里复原，否则污染同二进制内的后续用例。
- **必须包含实际断言**。既有代码中存在只验证不崩溃的覆盖式用例，新增用例必须断言被修复的那个具体状态。

---

## 无 OHOS 构建环境时的静态自检

多数情况下 Agent 拿不到完整 OHOS 源码树，无法真正编译。此时**必须**完成以下静态自检，它能拦住本仓大部分低级错误：

| # | 检查 | 方法 |
|---|---|---|
| 1 | 新增 `.cpp` 已进 `call_manager_sources` | 搜 `callmanager.gni` |
| 2 | 新增 include 目录已进 `call_manager_include_dirs` | 同上 |
| 3 | 新增外部依赖同时进了 `.gni` 和 `bundle.json` | 两处都搜 |
| 4 | 新增 feature 宏三处齐全（`declare_args` / `defines` 块 / `bundle.json`） | 三处都搜 |
| 5 | `#ifdef` 内声明的成员，其引用点全在同一宏内 | 搜成员名，逐个确认所在宏 |
| 6 | 新增 IPC 接口六步齐全 | 见 [`boundaries.md`](boundaries.md)「新增 IPC 接口的六步清单」 |
| 7 | proxy 写入与 stub 读取顺序/类型/个数一致 | 并排比对两个函数 |
| 8 | 没有 `try` / `catch` / `dynamic_cast` | 全文搜 |
| 9 | 没有在日志里打印号码/联系人 | 检查新增的 `TELEPHONY_LOG*` |
| 10 | 新增标志位的置位与三条清位路径齐全 | 见 [`failure-modes.md`](failure-modes.md)「修复模板」 |
| 11 | 新增 `sptr<CallBase>` 使用点都判空 | 检查每个 `GetOneCallObject` 调用 |
| 12 | 加锁区间内没有会回调到外部的函数 | 检查每个新增 `lock_guard` |
| 13 | **`.d.ts` 签名变更已人工核对** | 见下方说明 |

> **第 13 项展开：本仓没有本地 API 兼容性检查工具。**
> 仓内不存在任何 `api-check` / `api-diff` / 契约校验脚本，`.d.ts` 的 `@permission` / `@throws` / `@syscap` / `@since` 完整性只由 OpenHarmony 远端门禁把关，**本地无法执行，无需查找**。
> 因此改动 `interfaces/kits/js/@ohos.telephony.call.d.ts` 时，这一项只能人工做，且必须做：
> 1. 逐条列出本次新增或修改的每一个签名。
> 2. 对照文件内既有条目的 JSDoc 模板，确认四个标签补齐。
> 3. 确认是**新增**接口而非修改已发布接口的签名或语义——后者属于 [`AGENTS.md`](../../AGENTS.md)「必须停下来找人确认的改动（Ask before）」，必须停下来找人确认。
> 4. 在最终回复里把这份签名清单写出来，让评审者能直接看到改了什么契约。

---

## DoD

完整 DoD 见 [`AGENTS.md`](../../AGENTS.md)「最小验证闭环（DoD）」。最小要求：

- [ ] 本文「无 OHOS 构建环境时的静态自检」中的 13 项静态自检全部通过。
- [ ] 有构建环境时，`call_manager` 目标编译通过，相关 zero_gtest 目标编译通过并运行全绿。
- [ ] 新增/修改至少一条包含实际断言的用例覆盖改动分支。
- [ ] [`fix-and-verify.md`](fix-and-verify.md)「回归面判定」中的回归面已按改动模块逐项确认。
