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

#ifndef DIA_CONFIG_C_H
#define DIA_CONFIG_C_H
#include "dia_config_info.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct DIA_String {
    char *data;
    unsigned dataLength;
} DIA_String;

typedef struct DiaRuleC {
    char *ruleName;     // 需使用上面定义的规则名称
    bool isOpenKeywords;      // 是否开启关键词
} DiaRuleC;

__attribute__((visibility("default"))) int DIA_InitConfig(void **config); // 创建Config对象
__attribute__((visibility("default"))) void DIA_ReleaseConfig(void** config); // 释放Config对象
// 设置int类型参数
__attribute__((visibility("default"))) int DIA_SetInt(void* config, const DIA_String* key, int value);
// 设置double类型参数
__attribute__((visibility("default"))) int DIA_SetDouble(void* config, const DIA_String* key, double value);
// 设置字符串类型参数
__attribute__((visibility("default"))) int DIA_SetString(void *config, const DIA_String *key, const DIA_String *value);
// 设置dia_rule类型参数
__attribute__((visibility("default"))) int DIA_SetRule(void *config, const DIA_String *key, const DIA_Rule_C *value);

#ifdef __cplusplus
}
#endif
#endif // DIA_CONFIG_C_H