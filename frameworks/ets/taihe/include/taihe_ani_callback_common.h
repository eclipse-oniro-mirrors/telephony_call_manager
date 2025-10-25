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

#ifndef TAIHE_ANI_CALLBACK_COMMON_H
#define TAIHE_ANI_CALLBACK_COMMON_H

#include <thread>
#include "ani.h"

namespace OHOS {
namespace Telephony {

class AniCallbackInfo {
public:
    AniCallbackInfo() {}
    ~AniCallbackInfo();

    bool init(uintptr_t opq);
    void AttachThread();
    void DetachThread();

    ani_env* env_ = nullptr;
    ani_vm* vm_ = nullptr;
    ani_object funObject_ = nullptr;
    ani_ref funRef_ = nullptr;
    ani_env* envT_ = nullptr;
    ani_object promise_ = nullptr;
    ani_resolver deferred_ = nullptr;
    bool attach_ = false;
    std::thread::id threadId_ = std::this_thread::get_id();
};

class AniCommonUtils {
public:
    static ani_status GetAniEnv(ani_vm* vm, ani_env** env);
    static void ExecAsyncCallbackPromise(ani_env *env, ani_resolver deferred, ani_ref data, ani_ref businessError);
    static ani_status ExecAsyncCallBack(ani_env *env, ani_object businessError, ani_object param,
        ani_object callbackFunc);
    static ani_status CreateBusinessError(ani_env *env, ani_ref &aniError, int32_t code,
        const std::string &message);
};
}
}
#endif