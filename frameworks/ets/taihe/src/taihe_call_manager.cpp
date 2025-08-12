/*
 * Copyright (C) 2021-2022 Huawei Device Co., Ltd.
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

#include "taihe_call_manager.h"
#include "taihe_call_manager_callback.h"
#include "telephony_errors.h"
#include "telephony_log_wrapper.h"
#include "singleton.h"
#include "system_ability_definition.h"

namespace OHOS {
namespace Telephony {

TaiheCallManager &TaiheCallManager::GetInstance()
{
    static TaiheCallManager instance;
    return instance;
}

TaiheCallManager::TaiheCallManager()
    : registerStatus_(TELEPHONY_ERR_UNINIT)
{
}

TaiheCallManager::~TaiheCallManager()
{
}

void TaiheCallManager::RegisterCallBack()
{
    if (registerStatus_ == TELEPHONY_SUCCESS) {
        TELEPHONY_LOGW("you have already registered TaiheCallManager RegisterCallBack!");
        return;
    }
    std::unique_ptr<TaiheCallManagerCallback> callbackPtr = std::make_unique<TaiheCallManagerCallback>();
    if (callbackPtr == nullptr) {
        TELEPHONY_LOGE("make_unique TaiheCallManagerCallback failed!");
        return;
    }
    DelayedSingleton<CallManagerClient>::GetInstance()->Init(TELEPHONY_CALL_MANAGER_SYS_ABILITY_ID);
    registerStatus_ = DelayedSingleton<CallManagerClient>::GetInstance()->RegisterCallBack(std::move(callbackPtr));
    if (registerStatus_ != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("Taihe RegisterCallBack failed!");
        return;
    }
}

} // namespace Telephony
} // namespace OHOS