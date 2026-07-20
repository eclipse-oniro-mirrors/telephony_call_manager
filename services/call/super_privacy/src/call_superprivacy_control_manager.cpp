/*
 * Copyright (C) 2021-2022 Huawei Device Co., Ltd.
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

#include "call_superprivacy_control_manager.h"

#include "syspara/parameters.h"
#include "audio_device_manager.h"
#include "call_ability_report_proxy.h"
#include "call_control_manager.h"
#include "call_manager_hisysevent.h"
#include "call_number_utils.h"
#include "display_manager_lite.h"
#include "display_info.h"
#include "super_privacy_manager_client.h"

namespace OHOS {
namespace Telephony {
const char SUPER_PRIVACY_POLICY_PARAM_KEYS[] = "persist.super_privacy_policy.sensors";
const int32_t SOURCE_CALL = 2;

void CallSuperPrivacyControlManager::RegisterSuperPrivacyPolicy()
{
    int32_t ret = WatchParameter(SUPER_PRIVACY_POLICY_PARAM_KEYS, ParamChangeCallback, nullptr);
    if (ret != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGI("RegisterPrivacyPolicy result:%{public}d", ret);
    }
}

void CallSuperPrivacyControlManager::ParamChangeCallback(const char *key, const char *value, void *context)
{
    SuperPrivacyModeChangeEvent();
    if (key == nullptr || value == nullptr) {
        return;
    }
    size_t expectedLen = strlen(SUPER_PRIVACY_POLICY_PARAM_KEYS);
    size_t actualLen = strnlen(key, expectedLen + 1);
    if (expectedLen != actualLen || strncmp(key, SUPER_PRIVACY_POLICY_PARAM_KEYS, expectedLen) != 0) {
        return;
    }
    std::string valueStr(value);
    TELEPHONY_LOGI("ParamChangeCallback valueStr:%{public}s", valueStr.c_str());
    int32_t valueNum;
    bool isTransSucc = StrToInt(valueStr, valueNum);
    auto callSuperPrivacyControlMgr = DelayedSingleton<CallSuperPrivacyControlManager>::GetInstance();
    if (isTransSucc && (callSuperPrivacyControlMgr != nullptr)) {
        SuperPrivacyPolicyInfo policyInfo;
        callSuperPrivacyControlMgr->ParsePolicyFromParam(policyInfo, valueNum);
        callSuperPrivacyControlMgr->CloseCallAccordingPolicy(policyInfo);
        if (policyInfo.superPrivacyMode != SuperPrivacyMode::OFF) {
            callSuperPrivacyControlMgr->SetSuperPrivacyChanged(false);
        }
    }
}

void CallSuperPrivacyControlManager::ParsePolicyFromParam(SuperPrivacyPolicyInfo &policyInfo, int32_t valueNum)
{
    // 解析格式：低到高为 总开关(4bit)，摄像头(4bit)，麦克风(4bit)，位置(4bit)
    int32_t privacyMode = (valueNum & 0xF);
    if (privacyMode > 2) {
        TELEPHONY_LOGE("illegal superprivacy mode");
        return;
    }
    policyInfo.superPrivacyMode = static_cast<SuperPrivacyMode>(privacyMode);
    const size_t POLICY_SIZE = 3;
    for (size_t i = 0; i < POLICY_SIZE; ++i) {
        int32_t sensorState = ((valueNum >> (4 * (i +1))) & 0xF);
        if (sensorState > 2) {
            TELEPHONY_LOGE("illegal sensor state");
            return;
        }
        policyInfo.superPrivacyPolicies[i].sensorState = static_cast<PrivacySensorState>(sensorState);
    }
}

void CallSuperPrivacyControlManager::CloseCallAccordingPolicy(const SuperPrivacyPolicyInfo &policyInfo)
{
    if (!CanCallWithSuperPrivacyPolicy(policyInfo)) {
        CloseAllCall();
    }
}

void CallSuperPrivacyControlManager::SuperPrivacyModeChangeEvent()
{
    CallEventInfo eventInfo;
    (void)memset_s(&eventInfo, sizeof(CallEventInfo), 0, sizeof(CallEventInfo));
    auto callSuperPrivacyControlManager = DelayedSingleton<CallSuperPrivacyControlManager>::GetInstance();
    if (callSuperPrivacyControlManager == nullptr) {
        return;
    }
    bool isSuperPrivacyMode = !callSuperPrivacyControlManager->CanCallWithSuperPrivacyPolicy();
    TELEPHONY_LOGI("SuperPrivacyMode:%{public}d", isSuperPrivacyMode);
    if (isSuperPrivacyMode) {
        eventInfo.eventId = CallAbilityEventId::EVENT_IS_SUPER_PRIVACY_MODE_ON;
    } else {
        eventInfo.eventId = CallAbilityEventId::EVENT_IS_SUPER_PRIVACY_MODE_OFF;
    }
    DelayedSingleton<CallAbilityReportProxy>::GetInstance()->CallEventUpdated(eventInfo);
}

void CallSuperPrivacyControlManager::CloseAllCall()
{
    std::vector<CallAttributeInfo> infos = CallObjectManager::GetAllCallInfoList(false);
    for (auto &info : infos) {
        if (!info.isEcc && !info.isEccContact) {
            TELEPHONY_LOGE("OnSuperPrivacyModeChanged callState:%{public}d", info.callState);
            if (info.callState == TelCallState::CALL_STATUS_INCOMING ||
                info.callState == TelCallState::CALL_STATUS_WAITING) {
                DelayedSingleton<CallControlManager>::GetInstance()->RejectCall(info.callId, false,
                    u"superPrivacyModeOn");
            } else {
                DelayedSingleton<CallControlManager>::GetInstance()->HangUpCall(info.callId);
            }
        }
    }
}

void CallSuperPrivacyControlManager::RecordChangedPrivacyPolicy()
{
    SuperPrivacyPolicyInfo currentPolicy;
    GetCurrentPrivacyPolicy(currentPolicy);
    SetOldSuperPrivacyMode(currentPolicy.superPrivacyMode);
}

void CallSuperPrivacyControlManager::CloseCallSuperPrivacyMode(std::u16string &phoneNumber, int32_t &accountId,
    int32_t &videoState, int32_t &dialType, int32_t &dialScene, int32_t &callType)
{
    int32_t setResult = CloseSuperPrivacyMode();
    if (setResult == SUPER_PRIVACY_MODE_REQUEST_SUCCESS) {
        AppExecFwk::PacMap dialInfo;
        dialInfo.PutIntValue("accountId", accountId);
        dialInfo.PutIntValue("videoState", videoState);
        dialInfo.PutIntValue("dialType", dialType);
        dialInfo.PutIntValue("dialScene", dialScene);
        dialInfo.PutIntValue("callType", callType);
        int32_t ret = DelayedSingleton<CallControlManager>::GetInstance()->DialCall(phoneNumber, dialInfo);
        if (ret != TELEPHONY_SUCCESS) {
            RestorePrivacyPolicy();
            return;
        }
        CallManagerHisysevent::HiWriteBehaviorEventPhoneUE(
            CALL_DIAL_CLOSE_SUPER_PRIVACY, PNAMEID_KEY, KEY_CALL_MANAGER, PVERSIONID_KEY, "");
    }
}

int32_t CallSuperPrivacyControlManager::CloseSuperPrivacyMode()
{
    SuperPrivacyPolicyInfo policyInfo;
    GetCurrentPrivacyPolicy(policyInfo);
    SetSuperPrivacyChanged(true);
    SetOldSuperPrivacyMode(policyInfo.superPrivacyMode);
    policyInfo.superPrivacyMode = SuperPrivacyMode::OFF;
    return SetSuperPrivacyPolicy(policyInfo, SOURCE_CALL);
}

void CallSuperPrivacyControlManager::CloseAnswerSuperPrivacyMode(int32_t callId, int32_t videoState)
{
    int32_t setResult = CloseSuperPrivacyMode();
    if (setResult == SUPER_PRIVACY_MODE_REQUEST_SUCCESS) {
        DelayedSingleton<CallControlManager>::GetInstance()->AnswerCall(callId, videoState);
        SuperPrivacyModeChangeEvent();
        DelayedSingleton<AudioDeviceManager>::GetInstance()->UpdateCurrentAudioDevice();
        CallManagerHisysevent::HiWriteBehaviorEventPhoneUE(
            CALL_DIAL_CLOSE_SUPER_PRIVACY, PNAMEID_KEY, KEY_CALL_MANAGER, PVERSIONID_KEY, "");
    }
}

void CallSuperPrivacyControlManager::RestorePrivacyPolicy()
{
    if (!IsSuperPrivacyChanged()) {
        return;
    }
    
    SuperPrivacyPolicyInfo policyInfo;
    GetCurrentPrivacyPolicy(policyInfo);
    policyInfo.superPrivacyMode = GetOldSuperPrivacyMode();
    SetSuperPrivacyPolicy(policyInfo, SOURCE_CALL);
    SetSuperPrivacyChanged(false);
    TELEPHONY_LOGI("RestorePrivacyPolicy");
}

void CallSuperPrivacyControlManager::GetCurrentPrivacyPolicy(SuperPrivacyPolicyInfo &policyInfo)
{
    int32_t privacyMode = system::GetIntParameter(SUPER_PRIVACY_POLICY_PARAM_KEYS, -1);
    if (privacyMode == -1) {
        return;
    }
    TELEPHONY_LOGI("GetCurrentSuperPrivacyPolicy :%{public}d", privacyMode);
    ParsePolicyFromParam(policyInfo, privacyMode);
}

int32_t CallSuperPrivacyControlManager::SetSuperPrivacyPolicy(const SuperPrivacyPolicyInfo &policyInfo, int32_t source)
{
    return SuperPrivacyManagerClient::GetInstance().SetSuperPrivacyPolicies(policyInfo, source);
}

bool CallSuperPrivacyControlManager::CanCallWithSuperPrivacyPolicy()
{
    SuperPrivacyPolicyInfo policyInfo;
    GetCurrentPrivacyPolicy(policyInfo);
    return CanCallWithSuperPrivacyPolicy(policyInfo);
}

bool CallSuperPrivacyControlManager::CanCallWithSuperPrivacyPolicy(const SuperPrivacyPolicyInfo &policyInfo)
{
    if (policyInfo.superPrivacyMode == SuperPrivacyMode::OFF) {
        return true;
    }

    Rosen::FoldStatus foldStatus = Rosen::DisplayManagerLite::GetInstance().GetFoldStatus();
    if (policyInfo.superPrivacyMode == SuperPrivacyMode::ON_WHEN_FOLDED && foldStatus != Rosen::FoldStatus::FOLDED) {
        return true;
    }

    bool isMicDisabled = (policyInfo.superPrivacyPolicies[static_cast<int32_t>(PrivacySensorType::MICROPHONE)]
        .sensorState == PrivacySensorState::DISABLED);
    bool isCamDisabled = (policyInfo.superPrivacyPolicies[static_cast<int32_t>(PrivacySensorType::CAMERA)]
        .sensorState == PrivacySensorState::DISABLED);
    if (isMicDisabled ||  isCamDisabled) {
        return false;
    }
    return true;
}

bool CallSuperPrivacyControlManager::IsSuperPrivacyChanged()
{
    std::shared_lock<ffrt::shared_mutex> lock(superPrivacyModeMutex_);
    return isSuperPrivacyModeChanged_;
}

void CallSuperPrivacyControlManager::SetSuperPrivacyChanged(bool flag)
{
    std::unique_lock<ffrt::shared_mutex> lock(superPrivacyModeMutex_);
    isSuperPrivacyModeChanged_ = flag;
}

SuperPrivacyMode CallSuperPrivacyControlManager::GetOldSuperPrivacyMode()
{
    std::shared_lock<ffrt::shared_mutex> lock(superPrivacyModeMutex_);
    return oldSuperPrivacyMode_;
}

void CallSuperPrivacyControlManager::SetOldSuperPrivacyMode(SuperPrivacyMode mode)
{
    std::unique_lock<ffrt::shared_mutex> lock(superPrivacyModeMutex_);
    oldSuperPrivacyMode_ = mode;
}
} // namespace Telephony
} // namespace OHOS