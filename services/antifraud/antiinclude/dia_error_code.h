/*
 * Copyright (C) 2025-2025 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#ifndef DIA_ERROR_CODE_H
#define DIA_ERROR_CODE_H

namespace OHOS::Security::DIA {

enum DIAErrCode {
    DIA_SUCCESS = 0,
    DIA_FAILURE = 1,

    /* error for common config 1000 - 1100 */
    DIA_ERR_CONFIG_SIPRAM = 1000,       // 配置参数错误
    DIA_ERR_CONFIG_CALLBACK = 1001,     // 回调参数错误
    DIA_ERR_DATA_ILEGEL = 1002,       // 参数不合法
    DIA_ERR_INVLAID_PARAM = 1003,       // 参数无效
    DIA_ERR_PERMISSION_DENIED = 1004,   // 权限不足
    DIA_ERR_MALLOC = 1005,           // 内存分配失败

    /* error for information handle 1100 - 1200 */
    DIA_ERR_IH_ABNORMAL_DATA = 1100,  // 检测数据错误
    DIA_ERR_IH_DETECT = 1101,         // 检测失败
    DIA_ERR_IH_GET_RESULT = 1102,     // 获取结果失败
    DIA_ERR_IH_FAILURE = 1103,        // 识别脱敏错误
    DIA_ERR_IH_ANONYMIZE = 1104,      // 脱敏错误
    DIA_ERR_IH_TEXTIDENTIFY = 1105,    // 文本识别错误
    DIA_ERR_IH_DIFFERENTIAL_PRIVACY = 1106,  // 匿名统计错误
    DIA_ERR_IH_INPUT_SIZE = 1107,  // 输入数据超过1000个错误
    DIA_ERR_IH_INPUT_LEN = 1108,   // 输入数据长度超过4096
    DIA_ERR_IH_DP_ANONYMIZE = 1109,   // 匿名统计脱敏失败
};
}

#endif  // DIA_ERROR_CODE_H