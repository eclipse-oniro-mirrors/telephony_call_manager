/*
 * Copyright (C) 2024-2024 Huawei Device Co., Ltd.
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

#include "telephony_cust_wrapper.h"

#include <dlfcn.h>
#include "telephony_log_wrapper.h"

namespace OHOS {
namespace Telephony {
namespace {
const std::string TELEPHONY_CUST_WRAPPER_PATH = "libtelephony_cust_api.z.so";
} // namespace

TelephonyCustWrapper::TelephonyCustWrapper() {}
TelephonyCustWrapper::~TelephonyCustWrapper()
{
    TELEPHONY_LOGI("TelephonyCustWrapper::~TelephonyCustWrapper() start");
    if (telephonyCustWrapperHandle_ != nullptr) {
        dlclose(telephonyCustWrapperHandle_);
        telephonyCustWrapperHandle_ = nullptr;
    }
}

void TelephonyCustWrapper::InitTelephonyCustWrapper()
{
    TELEPHONY_LOGI("TelephonyCustWrapper::InitTelephonyCustWrapper() start");
    telephonyCustWrapperHandle_ = dlopen(TELEPHONY_CUST_WRAPPER_PATH.c_str(), RTLD_NOW);
    if (telephonyCustWrapperHandle_ == nullptr) {
        TELEPHONY_LOGE("libtelephony_cust_api.z.so was not loaded, error: %{public}s", dlerror());
        return;
    }
    isHideViolenceOrEmcNumbersInCallLog_ = (IS_HIDE_VIOLENCE_OR_EMC_NUMBERS_IN_CALLLOG)dlsym(
        telephonyCustWrapperHandle_, "IsHideViolenceOrEmcNumbersInCallLog");
    if (isHideViolenceOrEmcNumbersInCallLog_ == nullptr) {
        TELEPHONY_LOGE("telephony cust wrapper symbol faild, error: %{public}s", dlerror());
        return;
    }
    TELEPHONY_LOGI("telephony cust wrapper init success!");
}
} // namespace Telephony
} // namespace OHOS
