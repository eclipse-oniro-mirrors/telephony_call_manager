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
 
#include "antifraud_adapter.h"
 
#include "anti_fraud_service_client.h"
#include "telephony_log_wrapper.h"
 
namespace OHOS {
namespace Telephony {
AntiFraudAdapter::AntiFraudAdapter()
{}
 
AntiFraudAdapter::~AntiFraudAdapter()
{}
 
void* AntiFraudAdapter::GetLibAntiFraud()
{
    if (libAntiFraud_ != nullptr) {
        return libAntiFraud_;
    }
 
    libAntiFraud_ = dlopen("libanti_fraud_service_client.so", RTLD_LAZY);
    if (libAntiFraud_ == nullptr) {
        TELEPHONY_LOGE("gLibAntiFraud_ is null");
    }
 
    return libAntiFraud_;
}
 
void AntiFraudAdapter::ReleaseAntiFraud()
{
    TELEPHONY_LOGI("ReleaseAntiFraud");
    if (libAntiFraud_ != nullptr) {
        dlclose(libAntiFraud_);
        libAntiFraud_ = nullptr;
    }
}
 
int32_t AntiFraudAdapter::DetectAntiFraud(
    const std::shared_ptr<OHOS::AntiFraudService::AntiFraudDetectResListener> &listener)
{
    libAntiFraud_ = GetLibAntiFraud();
    if (libAntiFraud_ == nullptr) {
        return -1;
    }
 
    PfnAntiFraudVoiceDetect func =
        reinterpret_cast<PfnAntiFraudVoiceDetect>(dlsym(libAntiFraud_, "AntiFraudVoiceDetect"));
    if (func == nullptr) {
        TELEPHONY_LOGE("func is NULL");
        return -1;
    }
 
    return func(listener);
}

int32_t AntiFraudAdapter::StopAntiFraud()
{
    libAntiFraud_ = GetLibAntiFraud();
    if (libAntiFraud_ == nullptr) {
        return -1;
    }

    PfnStopAntiFraudVoiceDetect func =
        reinterpret_cast<PfnStopAntiFraudVoiceDetect>(dlsym(libAntiFraud_, "StopAntiFraudVoiceDetect"));
    if (func == nullptr) {
        TELEPHONY_LOGE("func is NULL");
        return -1;
    }

    return func();
}
}
}