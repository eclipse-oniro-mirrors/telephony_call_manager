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

#ifndef ANTIFRAUD_ADAPTER_H
#define ANTIFRAUD_ADAPTER_H

#include <securec.h>
#include "antifraud_service.h"
#include "errors.h"

namespace OHOS {
namespace Telephony {
using PfnAntiFraudDetectCheck = OHOS::ErrCode (*)(const OHOS::AntiFraudService::AfsDetectType &detectType);
using PfnAntiFraudStartDetect = OHOS::ErrCode (*)(
    const std::shared_ptr<OHOS::AntiFraudService::AntiFraudStartDetectResListener> &listener,
    const OHOS::AntiFraudService::AfsDetectType &detectType);
using PfnAntiFraudStopDetect = OHOS::ErrCode (*)();
class AntiFraudAdapter {
    DECLARE_DELAYED_SINGLETON(AntiFraudAdapter)

public:
    void *GetLibAntiFraud();
    void ReleaseAntiFraud();
    OHOS::ErrCode AntiFraudDetectCheck(const OHOS::AntiFraudService::AfsDetectType &detectType);
    OHOS::ErrCode AntiFraudStartDetect(
        const std::shared_ptr<OHOS::AntiFraudService::AntiFraudStartDetectResListener> &listener,
        const OHOS::AntiFraudService::AfsDetectType &detectType);
    OHOS::ErrCode AntiFraudStopDetect();
    int32_t StopAntiFraud();
private:
    void *libAntiFraud_ = nullptr;
};
}
}
#endif /* ANTIFRAUD_ADAPTER_H */