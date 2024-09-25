/*
 * Copyright (C) 2024 Huawei Device Co., Ltd.
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

#include "fold_status_manager.h"

#include "telephony_log_wrapper.h"
#include "syspara/parameters.h"
#include "call_dialog.h"

namespace OHOS {
namespace Telephony {
const std::string FOLD_PRODUCT_TYPE_KEY = "const.window.foldscreen.type";
const std::string SMALL_FOLD_PRODUCT_TYPE = "2,0,0,0";

FoldStatusManager::~FoldStatusManager() {};

FoldStatusManager::FoldStatusManager()
{
    TELEPHONY_LOGI("Enter FoldStatusManager");
    mOldFoldStatus = Rosen::FoldStatus::UNKNOWN;
};

void FoldStatusManager::RegisterFoldableListener()
{
    if (!IsSmallFoldDevice()) {
        TELEPHONY_LOGI("Is not small fold device");
        return;
    }
    if (foldStatusListener_ != nullptr) {
        TELEPHONY_LOGI("Foldable listener is already registed");
        return;
    }
    foldStatusListener_ = new (std::nothrow) FoldStatusListener();
    if (foldStatusListener_ == nullptr) {
        TELEPHONY_LOGE("RegisterFoldableListener new listener failed");
        return;
    }
    auto ret = Rosen::DisplayManager::GetInstance().RegisterFoldStatusListener(foldStatusListener_);
    if (ret != Rosen::DMError::DM_OK) {
        TELEPHONY_LOGE("Rosen::DisplayManager::RegisterFoldStatusListener failed");
        foldStatusListener_ = nullptr;
    } else {
        mOldFoldStatus = Rosen::DisplayManager::GetInstance().GetFoldStatus();
        TELEPHONY_LOGI("Rosen::DisplayManager::RegisterFoldStatusListener success");
    }
}

void FoldStatusManager::UnregisterFoldableListener()
{
    if (foldStatusListener_ == nullptr) {
        TELEPHONY_LOGI("UnregisterFoldableListener listener is nullptr");
        return;
    }
    auto ret = Rosen::DisplayManager::GetInstance().UnregisterFoldStatusListener(foldStatusListener_);
    if (ret != Rosen::DMError::DM_OK) {
        TELEPHONY_LOGE("Rosen::DisplayManager::UnregisterFoldStatusListener failed");
    } else {
        mOldFoldStatus = Rosen::FoldStatus::UNKNOWN;
        foldStatusListener_ = nullptr;
        TELEPHONY_LOGI("Rosen::DisplayManager::UnregisterFoldStatusListener success");
    }
}

void FoldStatusManager::FoldStatusListener::OnFoldStatusChanged(Rosen::FoldStatus foldStatus)
{
    auto oldFoldStatus = DelayedSingleton<FoldStatusManager>::GetInstance()->mOldFoldStatus;
    TELEPHONY_LOGI("OnFoldStatusChanged foldStatus is %{public}d, oldFoldStatus is %{public}d",
        static_cast<int32_t>(foldStatus), static_cast<int32_t>(oldFoldStatus));
    if (foldStatus == oldFoldStatus) {
        TELEPHONY_LOGE("Fold status don't change");
        return;
    }
    DelayedSingleton<CallDialog>::GetInstance()->DialogCallingPrivacyModeExtension(foldStatus);
    DelayedSingleton<FoldStatusManager>::GetInstance()->mOldFoldStatus = foldStatus;
}

bool FoldStatusManager::IsSmallFoldDevice()
{
    std::string foldType = system::GetParameter(FOLD_PRODUCT_TYPE_KEY, "0,0,0,0");
    TELEPHONY_LOGI("IsSmallFoldDevice foldType is %{public}s", foldType.c_str());
    return foldType == SMALL_FOLD_PRODUCT_TYPE;
}
} // namespace Telephony
} // namespace OHOS