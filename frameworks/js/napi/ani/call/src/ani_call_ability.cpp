/*
* Copyright (C) 2025 Huawei Device Co., Ltd.
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

#include <ani.h>
#include <array>
#include <iostream>
#include "call_manager_client.h"
#include "telephony_log_wrapper.h"

namespace OHOS {
namespace Telephony {
static bool hasVoiceCapability([[maybe_unused]] ani_env *env)
{
    bool status = DelayedSingleton<CallManagerClient>::GetInstance()->HasVoiceCapability();
    return status;
}

static int32_t makeCallExecute([[maybe_unused]]ani_env* env, [[maybe_unused]]ani_object obj,
    ani_object callback)
{
    ani_boolean isUndefined;
    env->Reference_IsUndefined(callback, &isUndefined);
    if (isUndefined) {
        isUndefined = true;
    }
    std::string phoneNumber;
    int32_t status = DelayedSingleton<CallManagerClient>::GetInstance()->MakeCall(phoneNumber);
    return 0;
}

ANI_EXPORT ani_status ANI_Constructor(ani_vm* vm, uint32_t* result)
{
    ani_env* env;
    ani_status status = ANI_ERROR;
    if (ANI_OK != vm->GetEnv(ANI_VERSION_1, &env)) {
        return ANI_ERROR;
    }

    const char *spaceName = "L@ohos.call.d.ets/CallAbility;";
    ani_namespace spc;
    status = env->FindNamespace(spaceName, &spc);
    if (ANI_OK != status) {
        return ANI_INVALID_ARGS;
    }

    std::array methods = {
        ani_native_function { "hasVoiceCapability",
            ":I", reinterpret_cast<void*>(hasVoiceCapability) },
        ani_native_function { "makeCallExecute",
            ":I", reinterpret_cast<void*>(makeCallExecute) },
    };
    status = env->Namespace_BindNativeFunctions(spc, methods.data(), methods.size());
    if (ANI_OK != status) {
        return ANI_INVALID_TYPE;
    };

    *result = ANI_VERSION_1;
    return ANI_OK;
}
}
}