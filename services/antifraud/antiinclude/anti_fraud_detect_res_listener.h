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
 
#ifndef ANTI_FRAUD_DETECT_RES_LISTENER_H
#define ANTI_FRAUD_DETECT_RES_LISTENER_H
 
#include <cstdint>
#include "anti_fraud_service_client_type.h"
 
namespace OHOS::AntiFraudService {
class AntiFraudDetectResListener {
public:
    virtual ~AntiFraudDetectResListener() = default;
    virtual void HandleAntiFraudDetectRes(const AntiFraudResult &antiFraudResult) = 0;
};
}
 
#endif