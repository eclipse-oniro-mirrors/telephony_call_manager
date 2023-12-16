/*
 * Copyright (C) 2023 Huawei Device Co., Ltd.
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

#include "voip_call.h"

#include "call_control_manager.h"
#include "call_manager_errors.h"
#include "call_object_manager.h"
#include "pixel_map.h"
#include "telephony_log_wrapper.h"
#include "voip_call_connection.h"

namespace OHOS {
namespace Telephony {
VoIPCall::VoIPCall(DialParaInfo &info) : CarrierCall(info)
{
    callId_ = info.callId;
    voipCallId_ = info.voipCallInfo.voipCallId;
    userName_ = info.voipCallInfo.userName;
    pixelMap_ = info.voipCallInfo.pixelMap;
    abilityName_ = info.voipCallInfo.abilityName;
    extensionId_ = info.voipCallInfo.extensionId;
    voipBundleName_ = info.voipCallInfo.voipBundleName;
}

VoIPCall::~VoIPCall() {}

int32_t VoIPCall::DialingProcess()
{
    return TELEPHONY_SUCCESS;
}

int32_t VoIPCall::AnswerCall(int32_t videoState)
{
    TELEPHONY_LOGI("VoIPCall::AnswerCall");
    VoipCallEvents voipcallInfo;
    int32_t ret = TELEPHONY_ERROR;
    ret = PackVoipCallInfo(voipcallInfo);
    if (ret != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGW("PackVoipCallInfo failed!");
    }
    voipcallInfo.voipCallEvent = VoipCallEvent::VOIP_CALL_EVENT_ANSWER_VOICE;
    if (videoState == 1) {
        TELEPHONY_LOGI("VoIPCall::AnswerCall VOIP VIDEO");
        voipcallInfo.voipCallEvent = VoipCallEvent::VOIP_CALL_EVENT_ANSWER_VIDEO;
    }
    voipcallInfo.bundleName = voipBundleName_;
    DelayedSingleton<VoipCallConnection>::GetInstance()->ReportVoipCallEventChange(voipcallInfo);
    if (ret != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("answer call failed!");
        return CALL_ERR_ANSWER_FAILED;
    }
    return TELEPHONY_SUCCESS;
}

int32_t VoIPCall::PackVoipCallInfo(VoipCallEvents &voipcallInfo)
{
    voipcallInfo.voipCallId = voipCallId_;
    voipcallInfo.bundleName = voipBundleName_;
    return TELEPHONY_SUCCESS;
}

int32_t VoIPCall::RejectCall()
{
    TELEPHONY_LOGI("VoIPCall::RejectCall enter");
    VoipCallEvents voipcallInfo;
    int32_t ret = TELEPHONY_ERROR;
    ret = PackVoipCallInfo(voipcallInfo);
    if (ret != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGW("PackVoipCallInfo failed!");
    }
    voipcallInfo.voipCallEvent = VoipCallEvent::VOIP_CALL_EVENT_REJECT;
    voipcallInfo.bundleName = voipBundleName_;
    DelayedSingleton<VoipCallConnection>::GetInstance()->ReportVoipCallEventChange(voipcallInfo);
    if (ret != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("reject call failed!");
        return CALL_ERR_ANSWER_FAILED;
    }
    return TELEPHONY_SUCCESS;
}

int32_t VoIPCall::HangUpCall()
{
    TELEPHONY_LOGI("VoIPCall::HangUpCall enter");
    VoipCallEvents voipcallInfo;
    int32_t ret = TELEPHONY_ERROR;
    ret = PackVoipCallInfo(voipcallInfo);
    if (ret != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGW("PackVoipCallInfo failed!");
    }
    voipcallInfo.voipCallEvent = VoipCallEvent::VOIP_CALL_EVENT_HANGUP;
    voipcallInfo.bundleName = voipBundleName_;
    voipcallInfo.errorReason = ErrorReason::USER_ANSWER_CELLULAR_FIRST;
    DelayedSingleton<VoipCallConnection>::GetInstance()->ReportVoipCallEventChange(voipcallInfo);
    if (ret != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("hangup call failed!");
        return CALL_ERR_ANSWER_FAILED;
    }
    return TELEPHONY_SUCCESS;
}

int32_t VoIPCall::HoldCall()
{
    return TELEPHONY_SUCCESS;
}

int32_t VoIPCall::UnHoldCall()
{
    return TELEPHONY_SUCCESS;
}

int32_t VoIPCall::SwitchCall()
{
    return TELEPHONY_SUCCESS;
}

int32_t VoIPCall::SetMute(int32_t mute, int32_t slotId)
{
    return TELEPHONY_SUCCESS;
}

void VoIPCall::GetCallAttributeInfo(CallAttributeInfo &info)
{
    GetCallAttributeCarrierInfo(info);
    info.callId = callId_;
    info.voipCallInfo.voipCallId = voipCallId_;
    info.voipCallInfo.userName = userName_;
    info.voipCallInfo.pixelMap = pixelMap_;
    info.voipCallInfo.extensionId = extensionId_;
    info.voipCallInfo.abilityName = abilityName_;
    info.voipCallInfo.voipBundleName = voipBundleName_;
    return;
}

int32_t VoIPCall::CombineConference()
{
    return TELEPHONY_SUCCESS;
}

void VoIPCall::HandleCombineConferenceFailEvent()
{
    return;
}

int32_t VoIPCall::SeparateConference()
{
    return TELEPHONY_SUCCESS;
}

int32_t VoIPCall::KickOutFromConference()
{
    return TELEPHONY_SUCCESS;
}

int32_t VoIPCall::CanCombineConference()
{
    return TELEPHONY_SUCCESS;
}

int32_t VoIPCall::CanSeparateConference()
{
    return TELEPHONY_SUCCESS;
}

int32_t VoIPCall::CanKickOutFromConference()
{
    return TELEPHONY_SUCCESS;
}

int32_t VoIPCall::GetMainCallId(int32_t &mainCallId)
{
    return TELEPHONY_SUCCESS;
}

int32_t VoIPCall::GetSubCallIdList(std::vector<std::u16string> &callIdList)
{
    return TELEPHONY_SUCCESS;
}

int32_t VoIPCall::GetCallIdListForConference(std::vector<std::u16string> &callIdList)
{
    return TELEPHONY_SUCCESS;
}

int32_t VoIPCall::IsSupportConferenceable()
{
    return TELEPHONY_SUCCESS;
}

int32_t VoIPCall::LaunchConference()
{
    return TELEPHONY_SUCCESS;
}

int32_t VoIPCall::ExitConference()
{
    return TELEPHONY_SUCCESS;
}

int32_t VoIPCall::HoldConference()
{
    return TELEPHONY_SUCCESS;
}

} // namespace Telephony
} // namespace OHOS
