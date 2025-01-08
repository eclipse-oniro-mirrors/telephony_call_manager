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
 
#ifndef ANTIFRAUD_SERVICE_H
#define ANTIFRAUD_SERVICE_H
 
#include "call_status_manager.h"
#include "anti_fraud_service_client.h"
#include "anti_fraud_service_client_type.h"
#include "anti_fraud_detect_res_listener.h"
#include "singleton.h"
#include "datashare_helper.h"
 
namespace OHOS {
namespace Telephony {
const std::string ANTIFRAUD_SWITCH = "spamshield_call_live_detection";
const std::string USER_IMPROPLAN_SWITCH = "spamshield_call_live_report";
const std::string USER_SETTINGSDATA_URI =
    "datashare:///com.ohos.settingsdata/entry/settingsdata/USER_SETTINGSDATA_100?Proxy=true";
 
enum class AntiFraudState {
    /**
     * Indicates the default antifraud state
     */
    ANTIFRAUD_STATE_DEFAULT = 0,
    /**
     * Indicates the antifraud is started
     */
    ANTIFRAUD_STATE_STARTED,
    /**
     * Indicates the call is fraud risk call
     */
    ANTIFRAUD_STATE_RISK,
    /**
     * Indicates the antifraud is finished and the call is not fraud call
     */
    ANTIFRAUD_STATE_FINISHED,
};
 
class AntiFraudService {
    DECLARE_DELAYED_SINGLETON(AntiFraudService)
 
public:
    void SetCallStatusManager(std::shared_ptr<CallStatusManager> callStatusManager);
    bool IsSwitchOn(const std::string switchName);
    bool IsAntiFraudSwitchOn();
    bool IsUserImprovementPlanSwitchOn();
    void InitParams();
    void RecordDetectResult(const OHOS::AntiFraudService::AntiFraudResult &antiFraudResult);
    void InitAntiFraudService();
    std::shared_ptr<DataShare::DataShareHelper> CreateDataShareHelper(
        int32_t systemAbilityId, const char *uri);
 
private:
    class AntiFraudDetectResListenerImpl : public OHOS::AntiFraudService::AntiFraudDetectResListener {
        void HandleAntiFraudDetectRes(const OHOS::AntiFraudService::AntiFraudResult &antiFraudResult) override;
    };
 
private:
    bool isResultFraud_ = false;
    int fraudDetectErr_ = 0;
    int fraudDetectVersion_ = 0;
    int fraudDetectType_ = 0;
    float fraudDetectProb_ = 0.0;
    std::string fraudDetectText_ = "";
 
    int32_t antiFraudState_ = 0;
 
private:
    std::shared_ptr<CallStatusManager> callStatusManagerPtr_ = nullptr;
};
}
}
#endif // ANTIFRAUD_SERVICE_H