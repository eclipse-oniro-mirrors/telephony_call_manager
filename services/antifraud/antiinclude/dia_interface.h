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
#ifndef DIA_INTERFACE_C_H
#define DIA_INTERFACE_C_H
#include "dia_config_c.h"

#ifdef __cplusplus
extern "C" {
#endif

__attribute__((visibility("default"))) int DIA_CreateAnonymize(void *config, void** anonymize); // 创建隐私助理对象
// 释放隐私助理对象
__attribute__((visibility("default"))) int DIA_IdentifyAnonymize(void *anonymize, const DIA_String *input,
                                                                 DIA_string **output);
__attribute__((visibility("default"))) void DIA_ReleaseAnonymize(void** anonymize); // 对敏感信息进行识别和脱敏
__attribute__((visibility("default"))) void DIA_ReleaseOutputData(DIA_String** output); // 释放识别和脱敏后的数据

#ifdef __cplusplus
}
#endif
#endif /* DIA_INTERFACE_C_H */