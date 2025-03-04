/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2025-2025. All rights reserved.
*/
#ifdef DIA_INTERFACE_C_H
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