/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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
 
#ifndef ANTI_FRAUD_SERVICE_CLIENT_H
#define ANTI_FRAUD_SERVICE_CLIENT_H
 
#include "anti_fraud_service_client_type.h"
#include "anti_fraud_detect_res_listener.h"
#include "anti_fraud_detect_res_listener_type.h"
#include "anti_fraud_detect_res_listener_stub_impl.h"
#include "ianti_fraud_detect_res_listener.h"
#include <vector>
#include <memory>
#include <mutex>
 
#define TAS_API __attribute__ ((visibility ("default")))
 
namespace OHOS::AntiFraudService {
 
class AntiFraudServiceClient {
public:
    static AntiFraudServiceClient& GetInstance();
 
    /**
     * @brief Invoking anti fraud service to perform voice anti fraud detection.
     *
     * This is an asynchronous interface, it returns the test result through the listener callback.
     *
     * @param listener The listener of voice anti-fraud detection result.
     * @return The invoking result.
     */
    ErrCode AntiFraudVoiceDetect(const std::shared_ptr<AntiFraudDetectResListener> &listener);
 
private:
    AntiFraudServiceClient();
    ~AntiFraudServiceClient() = default;
 
    void DispatchHandleAntiFraudDetectRes(const AntiFraudInnerResult &antiFraudInnerResult);
 
    ffrt::mutex listenerMutex_;
    std::shared_ptr<AntiFraudDetectResListener> listener_;
 
    OnDetectResCallbackFunc callbackFunc_;
    sptr<IAntiFraudDetectResListener> remoteCallback_ = nullptr;
 
    DISALLOW_COPY_AND_MOVE(AntiFraudServiceClient);
};
}
#endif