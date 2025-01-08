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
 
#ifndef I_ANTI_FRAUD_DETECT_RES_LISTENER_H
#define I_ANTI_FRAUD_DETECT_RES_LISTENER_H
 
#include "iremote_broker.h"
#include "iremote_object.h"
#include "anti_fraud_service_client_type.h"
#include "anti_fraud_detect_res_listener_type.h"
 
namespace OHOS::AntiFraudService {
class IAntiFraudDetectResListener : public IRemoteBroker {
public:
    DECLARE_INTERFACE_DESCRIPTOR(u"OHOS.AntiFraudService.IAntiFraudDetectResListener");
 
    virtual void HandleAntiFraudDetectRes(const AntiFraudInnerResult &antiFraudInnerResult) = 0;
 
    enum class Message : uint32_t {
        HANDLE_ANTI_FRAUD_DETECT_RES,
    };
};
}
 
#endif