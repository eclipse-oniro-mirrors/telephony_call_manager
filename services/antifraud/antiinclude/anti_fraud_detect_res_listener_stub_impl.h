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
 
#ifndef ANTI_FRAUD_DETECT_RES_LISTENER_STUB_IMPL_H
#define ANTI_FRAUD_DETECT_RES_LISTENER_STUB_IMPL_H
 
#include <functional>
 
#include "anti_fraud_detect_res_listener_stub.h"
#include "anti_fraud_detect_res_listener.h"
#include "anti_fraud_service_client_type.h"
#include "anti_fraud_detect_res_listener_type.h"
 
namespace OHOS::AntiFraudService {
using OnDetectResCallbackFunc = std::function<void(const AntiFraudInnerResult &)>;
class AntiFraudDetectResListenerStubImpl : public AntiFraudDetectResListenerStub {
public:
    AntiFraudDetectResListenerStubImpl(const OnDetectResCallbackFunc &callbackFunc);
    ~AntiFraudDetectResListenerStubImpl() override;
    void HandleAntiFraudDetectRes(const AntiFraudInnerResult &antiFraudInnerResult) override;
 
private:
    OnDetectResCallbackFunc callbackFunc_;
    DISALLOW_COPY_AND_MOVE(AntiFraudDetectResListenerStubImpl);
};
}
 
#endif