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
 
#include "ffrt.h"
#include <queue>
#include "call_status_manager.h"
#include "anti_fraud_service_client_type.h"
#include "anti_fraud_start_detect_res_listener.h"
#include "singleton.h"
#include "datashare_helper.h"

namespace OHOS {
namespace Telephony {
inline const std::string ANTIFRAUD_SWITCH_VOICE = "spamshield_call_live_detection";
inline const std::string ANTIFRAUD_SWITCH_VIDEO = "spamshield_live_stream_image_detection";
inline const std::string USER_IMPROPLAN_SWITCH = "spamshield_call_live_report";
inline const std::string ANTIFRAUD_CONTACTS_ENABLED_VOICE = "SPAMSHIELD_IS_CONTACTS_VOICE_DETECT_ENABLED";
inline const std::string ANTIFRAUD_CONTACTS_ENABLED_VIDEO = "SPAMSHIELD_IS_CONTACTS_LIVE_DETECT_ENABLED";
inline const std::string USER_SETTINGSDATA_URI =
    "datashare:///com.ohos.settingsdata/entry/settingsdata/USER_SETTINGSDATA_SECURE_100?Proxy=true";
constexpr int32_t SELECT_RULE_LENGTH = 11;

class AntiFraudService {
    DECLARE_DELAYED_SINGLETON(AntiFraudService)

public:
    void SetCallStatusManager(std::shared_ptr<CallStatusManager> callStatusManager);
    bool IsSwitchOn(const std::string switchName);
    bool IsAntiFraudSwitchOn(const VideoStateType videoStateType);
    bool IsAntiFraudContactsEnabled(const VideoStateType videoStateType);
    bool IsUserImprovementPlanSwitchOn();
    void RecordDetectResult(const OHOS::AntiFraudService::StartDetectionResult &antiFraudResult,
        std::string resultPhoneNum, int32_t slotId, int32_t index);
    int32_t StartAntiFraudService(const std::string &phoneNum, int32_t slotId, int32_t index,
        const OHOS::AntiFraudService::AfsDetectType &detectType);
    int32_t StopAntiFraudService(int32_t slotId, int32_t index);
    int32_t StopAntiFraudDetectByType(const OHOS::AntiFraudService::AfsDetectType &detectType);
    std::shared_ptr<DataShare::DataShareHelper> CreateDataShareHelper(int32_t systemAbilityId, const char *uri);
    void AddRuleToConfig(const std::string rulesName, void *config);
    int AnonymizeText();
    void UpdateVideoState(VideoStateType priorVideoState, VideoStateType nextVideoState);

private:
    int32_t GetStoppedSlotId();
    int32_t GetStoppedIndex();
    void SetStoppedSlotId(int32_t slotId);
    void SetStoppedIndex(int32_t index);
    int32_t CheckAntiFraudService(const OHOS::AntiFraudService::AfsDetectType &detectType);

private:
    class AntiFraudStartDetectResListenerImpl : public OHOS::AntiFraudService::AntiFraudStartDetectResListener {
    public:
        AntiFraudStartDetectResListenerImpl(std::string phoneNum, int slotId, int index)
            :phoneNum_(phoneNum), slotId_(slotId), index_(index) {}
        void HandleAntiFraudStartDetectRes(
            const OHOS::AntiFraudService::StartDetectionResult &detectionResult) override;

    private:
        std::string phoneNum_;
        int32_t slotId_;
        int32_t index_;
    };

private:
    void InitParams();
    std::string fraudDetectText_ = "";

    int32_t antiFraudState_ = 0;
    int32_t stoppedSlotId_ = -1;
    int32_t stoppedIndex_ = -1;
    OHOS::AntiFraudService::AntiFraudResultExt antiFraudResultExt_ {};

    ffrt::mutex mutex_;
    ffrt::mutex fraudMutex_;
 
private:
    std::shared_ptr<CallStatusManager> callStatusManagerPtr_ = nullptr;
};
}
}
#endif // ANTIFRAUD_SERVICE_H
