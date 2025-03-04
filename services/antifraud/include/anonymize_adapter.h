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
 
#ifndef ANONYMIZE_ADAPTER_H
#define ANONYMIZE_ADAPTER_H

#include <securec.h>
#include "antifraud_service.h"
#include "dia_config_c.h"
#include "dia_interface_c.h"
#include "dia_config_info.h"
#include "dia_error_code.h"

namespace OHOS {
namespace Telephony {
typedef int (*PfnInitConfig)(void **config);
typedef int (*PfnSetRule)(void *config, const DIA_String *key, const DIA_Rule_C *value);
typedef int (*PfnCreateAnonymize)(void *config, void **anonymize);
typedef int (*PfnIdentifyAnonymize)(void *anonymize, const DIA_String *input, const DIA_String **output);
typedef int (*PfnReleaseConfig)(void **config);
typedef int (*PfnReleaseAonoymize)(void **anonymize);
typedef int (*PfnReleaseOutputData)(DIA_String **output);
class AnonymizeAdapter {
    DECLARE_DELAYED_SINGLETON(AnonymizeAdapter)

public:
    void *GetLibAnonymize();
    void ReleaseLibAnonymize();
    int InitConfig(void **config);
    int SetRule(void **config, const DIA_String *key, const DIA_Rule_C *value);
    int CreateAnonymize(void *config, void **anonymize);
    int IdentifyAnonymize(void *anonymize, const DIA_String *input, DIA_String **output);
    int ReleaseConfig(void **config);
    int ReleaseAnonymize(void **anonymize);
    int ReleaseOutputData(DIA_String **output);

private:
    void *libAnonymize_ = nullptr;
};
}
}

#endif /* ANONYMIZE_ADAPTER_H */
  