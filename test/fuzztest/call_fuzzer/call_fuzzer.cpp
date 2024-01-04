/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include "call_fuzzer.h"

#include <cstddef>
#include <cstdint>
#define private public
#define protected public
#include "addcalltoken_fuzzer.h"
#include "cs_call.h"
#include "ims_call.h"
#include "ott_call.h"
#include "satellite_call.h"
#include "surface_utils.h"
#include "voip_call.h"

using namespace OHOS::Telephony;
namespace OHOS {
constexpr int32_t SLOT_NUM = 2;
constexpr int32_t BOOL_NUM = 2;
constexpr int32_t DIAL_TYPE = 3;
constexpr int32_t CALL_TYPE_NUM = 4;
constexpr int32_t VIDIO_TYPE_NUM = 2;
constexpr int32_t TEL_CALL_STATE_NUM = 9;
constexpr int32_t TEL_CONFERENCE_STATE_NUM = 4;
constexpr int32_t CALL_RUNNING_STATE_NUM = 8;
constexpr int32_t CALL_ENDED_TYPE_NUM = 4;
constexpr int32_t CALL_ANSWER_TYPE_NUM = 3;
constexpr int32_t INVALID_CALL_ID = -1;
constexpr int32_t IMS_CALL_MODE_NUM = 5;
constexpr int32_t CALL_INDEX_MAX_NUM = 8;
constexpr int32_t VIDEO_REQUEST_RESULT_TYPE_NUM = 102;

void CSCallFunc(const uint8_t *data, size_t size)
{
    if (!IsServiceInited()) {
        return;
    }

    DialParaInfo dialParaInfo;
    dialParaInfo.dialType = static_cast<DialType>(size % DIAL_TYPE);
    dialParaInfo.callType = static_cast<CallType>(size % CALL_TYPE_NUM);
    dialParaInfo.videoState = static_cast<VideoStateType>(size % VIDIO_TYPE_NUM);
    dialParaInfo.callState = static_cast<TelCallState>(size % TEL_CALL_STATE_NUM);
    sptr<CallBase> callObjectPtr = std::make_unique<CSCall>(dialParaInfo).release();
    int32_t videoState = static_cast<int32_t>(size % VIDIO_TYPE_NUM);
    int32_t mute = static_cast<int32_t>(size % BOOL_NUM);
    int32_t slotId = static_cast<int32_t>(size % SLOT_NUM);
    TelCallState nextState = static_cast<TelCallState>(size % TEL_CALL_STATE_NUM);
    TelConferenceState telConferenceState = static_cast<TelConferenceState>(size % TEL_CONFERENCE_STATE_NUM);
    VideoStateType mediaType = static_cast<VideoStateType>(size % VIDIO_TYPE_NUM);
    PolicyFlag flag = static_cast<PolicyFlag>(size);
    bool needAutoAnswer = static_cast<bool>(size);
    bool canUnHoldState = static_cast<bool>(size);

    callObjectPtr->AnswerCall(videoState);
    callObjectPtr->SetMute(mute, slotId);
    callObjectPtr->StartDtmf(static_cast<char>(*data));
    callObjectPtr->StopDtmf();
    callObjectPtr->GetSlotId();
    callObjectPtr->DialCallBase();
    callObjectPtr->IncomingCallBase();
    callObjectPtr->AnswerCallBase();
    callObjectPtr->RejectCallBase();
    callObjectPtr->GetCallID();
    callObjectPtr->GetCallType();
    callObjectPtr->GetCallRunningState();
    callObjectPtr->SetTelCallState(nextState);
    callObjectPtr->GetTelCallState();
    callObjectPtr->SetTelConferenceState(telConferenceState);
    callObjectPtr->GetTelConferenceState();
    callObjectPtr->GetVideoStateType();
    callObjectPtr->SetVideoStateType(mediaType);
    callObjectPtr->SetPolicyFlag(flag);
    callObjectPtr->GetPolicyFlag();
    callObjectPtr->SetAutoAnswerState(needAutoAnswer);
    callObjectPtr->GetAutoAnswerState();
    callObjectPtr->SetCanUnHoldState(canUnHoldState);
    callObjectPtr->GetCanUnHoldState();
}

void DialingProcess(const uint8_t *data, size_t size)
{
    if (!IsServiceInited()) {
        return;
    }

    DialParaInfo paraInfo;
    paraInfo.dialType = static_cast<DialType>(size % DIAL_TYPE);
    paraInfo.callType = static_cast<CallType>(size % CALL_TYPE_NUM);
    paraInfo.videoState = static_cast<VideoStateType>(size % VIDIO_TYPE_NUM);
    paraInfo.callState = static_cast<TelCallState>(size % TEL_CALL_STATE_NUM);
    sptr<CallBase> callObjectPtr = std::make_unique<CSCall>(paraInfo).release();

    callObjectPtr->StartDtmf(static_cast<char>(*data));
    callObjectPtr->RejectCall();
    callObjectPtr->HangUpCall();
    callObjectPtr->HoldCall();
    callObjectPtr->UnHoldCall();
    callObjectPtr->SwitchCall();
    callObjectPtr->DialingProcess();
    callObjectPtr->CombineConference(); // merge calls
    callObjectPtr->SeparateConference();
    callObjectPtr->KickOutFromConference();
    callObjectPtr->CanSeparateConference();
    callObjectPtr->CanCombineConference();
    callObjectPtr->CanKickOutFromConference();
    callObjectPtr->LaunchConference();
    callObjectPtr->ExitConference();
    callObjectPtr->HoldConference();
    int32_t mainCallId = INVALID_CALL_ID;
    callObjectPtr->GetMainCallId(mainCallId);
    std::vector<std::u16string> subCallIdList;
    callObjectPtr->GetSubCallIdList(subCallIdList);
    std::vector<std::u16string> callIdList;
    callObjectPtr->GetCallIdListForConference(callIdList);
    callObjectPtr->IsSupportConferenceable();
    callObjectPtr->GetEmergencyState();
}

void GetCallerInfo(const uint8_t *data, size_t size)
{
    if (!IsServiceInited()) {
        return;
    }

    DialParaInfo info;
    info.dialType = static_cast<DialType>(size % DIAL_TYPE);
    info.callType = static_cast<CallType>(size % CALL_TYPE_NUM);
    info.videoState = static_cast<VideoStateType>(size % VIDIO_TYPE_NUM);
    info.callState = static_cast<TelCallState>(size % TEL_CALL_STATE_NUM);
    sptr<CallBase> callObjectPtr = std::make_unique<CSCall>(info).release();
    ContactInfo contactInfo;
    CallRunningState callRunningState = static_cast<CallRunningState>(size % CALL_RUNNING_STATE_NUM);
    bool speakerphoneOn = static_cast<bool>(size % BOOL_NUM);
    std::string phoneNumber(reinterpret_cast<const char *>(data), size);
    int32_t callId = static_cast<int32_t>(size);
    CallEndedType callEndedType = static_cast<CallEndedType>(size % CALL_ENDED_TYPE_NUM);
    CallAnswerType answerType = static_cast<CallAnswerType>(size % CALL_ANSWER_TYPE_NUM);
    int64_t startTime = static_cast<int64_t>(size);
    time_t callBeginTime = static_cast<time_t>(size);
    time_t callEndTime = static_cast<time_t>(size);
    time_t ringBeginTime = static_cast<time_t>(size);
    time_t ringEndTime = static_cast<time_t>(size);
    callObjectPtr->GetCallerInfo();
    callObjectPtr->SetCallerInfo(contactInfo);
    callObjectPtr->SetCallRunningState(callRunningState);
    callObjectPtr->SetStartTime(startTime);
    callObjectPtr->SetCallBeginTime(callBeginTime);
    callObjectPtr->SetCallEndTime(callEndTime);
    callObjectPtr->SetRingBeginTime(ringBeginTime);
    callObjectPtr->SetRingEndTime(ringEndTime);
    callObjectPtr->SetAnswerType(answerType);
    callObjectPtr->GetCallEndedType();
    callObjectPtr->SetCallEndedType(callEndedType);
    callObjectPtr->SetCallId(callId);
    callObjectPtr->IsSpeakerphoneEnabled();
    callObjectPtr->IsCurrentRinging();
    callObjectPtr->GetAccountNumber();
    callObjectPtr->SetSpeakerphoneOn(speakerphoneOn);
    callObjectPtr->IsSpeakerphoneOn();
    callObjectPtr->CheckVoicemailNumber(phoneNumber);
    callObjectPtr->IsAliveState();
}

void IMSCallFunc(const uint8_t *data, size_t size)
{
    if (!IsServiceInited()) {
        return;
    }

    DialParaInfo paraInfo;
    paraInfo.dialType = static_cast<DialType>(size % DIAL_TYPE);
    paraInfo.callType = static_cast<CallType>(size % CALL_TYPE_NUM);
    paraInfo.videoState = static_cast<VideoStateType>(size % VIDIO_TYPE_NUM);
    paraInfo.callState = static_cast<TelCallState>(size % TEL_CALL_STATE_NUM);
    sptr<IMSCall> callObjectPtr = std::make_unique<IMSCall>(paraInfo).release();
    int32_t videoState = static_cast<int32_t>(size % VIDIO_TYPE_NUM);
    int32_t mute = static_cast<int32_t>(size % BOOL_NUM);
    int32_t slotId = static_cast<int32_t>(size % SLOT_NUM);
    std::string msg(reinterpret_cast<const char *>(data), size);
    std::u16string msgU16 = Str8ToStr16(msg);

    callObjectPtr->InitVideoCall();
    callObjectPtr->DialingProcess();
    callObjectPtr->AnswerCall(videoState);
    callObjectPtr->RejectCall();
    callObjectPtr->HangUpCall();
    callObjectPtr->UnHoldCall();
    callObjectPtr->HoldCall();
    callObjectPtr->SwitchCall();
    callObjectPtr->CombineConference();
    callObjectPtr->SeparateConference();
    callObjectPtr->KickOutFromConference();
    callObjectPtr->CanKickOutFromConference();
    callObjectPtr->CanCombineConference();
    callObjectPtr->CanSeparateConference();
    callObjectPtr->LaunchConference();
    callObjectPtr->ExitConference();
    callObjectPtr->HoldConference();
    int32_t mainCallId = INVALID_CALL_ID;
    callObjectPtr->GetMainCallId(mainCallId);
    std::vector<std::u16string> subCallIdList;
    callObjectPtr->GetSubCallIdList(subCallIdList);
    std::vector<std::u16string> callIdList;
    callObjectPtr->GetCallIdListForConference(callIdList);
    callObjectPtr->IsSupportConferenceable();
    callObjectPtr->StartRtt(msgU16);
    callObjectPtr->StopRtt();
    callObjectPtr->SetMute(mute, slotId);
}

void VoIPCallFunc(const uint8_t *data, size_t size)
{
    if (!IsServiceInited()) {
        return;
    }
    CallAttributeInfo info;
    DialParaInfo dialParaInfo;
    int32_t mainCallId = 0;
    VoipCallEventInfo voipcallInfo;
    std::vector<std::u16string> callIdList;
    int32_t videoState = static_cast<int32_t>(size % VIDIO_TYPE_NUM);
    sptr<VoIPCall> voipCall = std::make_unique<VoIPCall>(dialParaInfo).release();

    voipCall->DialingProcess();
    voipCall->AnswerCall(videoState);
    voipCall->PackVoipCallInfo(voipcallInfo);
    voipCall->RejectCall();
    voipCall->HangUpCall();
    voipCall->HoldCall();
    voipCall->UnHoldCall();
    voipCall->SwitchCall();
    voipCall->SetMute(0, 0);
    voipCall->GetCallAttributeInfo(info);
    voipCall->CombineConference();
    voipCall->HandleCombineConferenceFailEvent();
    voipCall->SeparateConference();
    voipCall->KickOutFromConference();
    voipCall->CanCombineConference();
    voipCall->CanSeparateConference();
    voipCall->CanKickOutFromConference();
    voipCall->GetMainCallId(mainCallId);
    voipCall->GetSubCallIdList(callIdList);
    voipCall->GetCallIdListForConference(callIdList);
    voipCall->IsSupportConferenceable();
    voipCall->LaunchConference();
    voipCall->ExitConference();
    voipCall->HoldConference();
}

void IMSVideoCallFunc(const uint8_t *data, size_t size)
{
    if (!IsServiceInited()) {
        return;
    }

    DialParaInfo paraInfo;
    paraInfo.dialType = static_cast<DialType>(size % DIAL_TYPE);
    paraInfo.callType = static_cast<CallType>(size % CALL_TYPE_NUM);
    paraInfo.videoState = static_cast<VideoStateType>(size % VIDIO_TYPE_NUM);
    paraInfo.callState = static_cast<TelCallState>(size % TEL_CALL_STATE_NUM);
    sptr<IMSCall> callObjectPtr = std::make_unique<IMSCall>(paraInfo).release();
    std::string msg(reinterpret_cast<const char *>(data), size);
    int32_t callingUid = static_cast<int32_t>(size);
    int32_t callingPid = static_cast<int32_t>(size);
    int32_t rotation = static_cast<int32_t>(size);
    ImsCallMode mode = static_cast<ImsCallMode>(size % IMS_CALL_MODE_NUM);
    CallModeReportInfo callModeReportInfo;
    callModeReportInfo.callIndex = static_cast<int32_t>(size % CALL_INDEX_MAX_NUM);
    callModeReportInfo.callMode = static_cast<ImsCallMode>(size % IMS_CALL_MODE_NUM);
    callModeReportInfo.result = static_cast<VideoRequestResultType>(size % VIDEO_REQUEST_RESULT_TYPE_NUM);
    callObjectPtr->UpdateImsCallMode(mode);
    callObjectPtr->SendUpdateCallMediaModeRequest(mode);
    callObjectPtr->RecieveUpdateCallMediaModeRequest(callModeReportInfo);
    callObjectPtr->SendUpdateCallMediaModeResponse(mode);
    callObjectPtr->ReceiveUpdateCallMediaModeResponse(callModeReportInfo);
    CallMediaModeInfo callMediaModeInfo;
    callMediaModeInfo.callId = static_cast<int32_t>(size);
    callMediaModeInfo.isRequestInfo = static_cast<bool>(size % BOOL_NUM);
    callMediaModeInfo.result = static_cast<VideoRequestResultType>(size % VIDEO_REQUEST_RESULT_TYPE_NUM);
    callMediaModeInfo.callMode = static_cast<ImsCallMode>(size % IMS_CALL_MODE_NUM);
    callObjectPtr->ReportImsCallModeInfo(callMediaModeInfo);
    callObjectPtr->SwitchVideoState(mode);
    callObjectPtr->IsSupportVideoCall();
    callObjectPtr->GetCallVideoState(mode);
    callObjectPtr->ControlCamera(msg, callingUid, callingPid);
    callObjectPtr->SetPausePicture(msg);
    callObjectPtr->SetDeviceDirection(rotation);
    callObjectPtr->CancelCallUpgrade();
    callObjectPtr->RequestCameraCapabilities();
}

void IMSVideoCallWindowFunc(const uint8_t *data, size_t size)
{
    if (!IsServiceInited()) {
        return;
    }

    DialParaInfo paraInfo;
    paraInfo.dialType = static_cast<DialType>(size % DIAL_TYPE);
    paraInfo.callType = static_cast<CallType>(size % CALL_TYPE_NUM);
    paraInfo.videoState = static_cast<VideoStateType>(size % VIDIO_TYPE_NUM);
    paraInfo.callState = static_cast<TelCallState>(size % TEL_CALL_STATE_NUM);
    sptr<IMSCall> callObjectPtr = std::make_unique<IMSCall>(paraInfo).release();
    std::string msg(reinterpret_cast<const char *>(data), size);
    int len = static_cast<int>(msg.length());
    std::string subSurfaceId = msg;
    if (len >= 1) {
        subSurfaceId = msg.substr(0, 1);
    }
    if (subSurfaceId.empty() || subSurfaceId[0] < '0' || subSurfaceId[0] > '9') {
        subSurfaceId = "";
        callObjectPtr->SetPreviewWindow(subSurfaceId, nullptr);
        callObjectPtr->SetDisplayWindow(subSurfaceId, nullptr);
    } else {
        uint64_t tmpSurfaceId = std::stoull(subSurfaceId);
        auto surface = SurfaceUtils::GetInstance()->GetSurface(tmpSurfaceId);
        callObjectPtr->SetPreviewWindow(subSurfaceId, surface);
        callObjectPtr->SetDisplayWindow(subSurfaceId, surface);
    }
}

void OttCallFunc(const uint8_t *data, size_t size)
{
    if (!IsServiceInited()) {
        return;
    }

    DialParaInfo paraInfo;
    paraInfo.dialType = static_cast<DialType>(size % DIAL_TYPE);
    paraInfo.callType = static_cast<CallType>(size % CALL_TYPE_NUM);
    paraInfo.videoState = static_cast<VideoStateType>(size % VIDIO_TYPE_NUM);
    paraInfo.callState = static_cast<TelCallState>(size % TEL_CALL_STATE_NUM);
    sptr<OTTCall> callObjectPtr = std::make_unique<OTTCall>(paraInfo).release();
    int32_t videoState = static_cast<int32_t>(size % VIDIO_TYPE_NUM);
    int32_t mute = static_cast<int32_t>(size % BOOL_NUM);
    int32_t slotId = static_cast<int32_t>(size % SLOT_NUM);

    callObjectPtr->DialingProcess();
    callObjectPtr->AnswerCall(videoState);
    callObjectPtr->RejectCall();
    callObjectPtr->HangUpCall();
    callObjectPtr->HoldCall();
    callObjectPtr->UnHoldCall();
    callObjectPtr->SwitchCall();
    callObjectPtr->GetEmergencyState();
    callObjectPtr->StopDtmf();
    callObjectPtr->GetSlotId();
    callObjectPtr->CombineConference();
    callObjectPtr->SeparateConference();
    callObjectPtr->KickOutFromConference();
    callObjectPtr->CanCombineConference();
    callObjectPtr->CanSeparateConference();
    callObjectPtr->CanKickOutFromConference();
    callObjectPtr->LaunchConference();
    callObjectPtr->ExitConference();
    callObjectPtr->HoldConference();
    int32_t mainCallId = INVALID_CALL_ID;
    callObjectPtr->GetMainCallId(mainCallId);
    std::vector<std::u16string> subCallIdList;
    callObjectPtr->GetSubCallIdList(subCallIdList);
    std::vector<std::u16string> callIdList;
    callObjectPtr->GetCallIdListForConference(callIdList);
    callObjectPtr->IsSupportConferenceable();
    callObjectPtr->SetMute(mute, slotId);
}

void OttVideoCallFunc(const uint8_t *data, size_t size)
{
    if (!IsServiceInited()) {
        return;
    }
    DialParaInfo paraInfo;
    paraInfo.dialType = static_cast<DialType>(size % DIAL_TYPE);
    paraInfo.callType = static_cast<CallType>(size % CALL_TYPE_NUM);
    paraInfo.videoState = static_cast<VideoStateType>(size % VIDIO_TYPE_NUM);
    paraInfo.callState = static_cast<TelCallState>(size % TEL_CALL_STATE_NUM);
    sptr<OTTCall> callObjectPtr = std::make_unique<OTTCall>(paraInfo).release();
    std::string msg(reinterpret_cast<const char *>(data), size);
    int32_t callingUid = static_cast<int32_t>(size);
    int32_t callingPid = static_cast<int32_t>(size);
    int32_t rotation = static_cast<int32_t>(size);
    callObjectPtr->InitVideoCall();
    ImsCallMode mode = static_cast<ImsCallMode>(size % IMS_CALL_MODE_NUM);
    CallModeReportInfo callModeReportInfo;
    callModeReportInfo.callIndex = static_cast<int32_t>(size % CALL_INDEX_MAX_NUM);
    callModeReportInfo.callMode = static_cast<ImsCallMode>(size % IMS_CALL_MODE_NUM);
    callModeReportInfo.result = static_cast<VideoRequestResultType>(size % VIDEO_REQUEST_RESULT_TYPE_NUM);
    callObjectPtr->UpdateImsCallMode(mode);
    callObjectPtr->SendUpdateCallMediaModeRequest(mode);
    callObjectPtr->RecieveUpdateCallMediaModeRequest(callModeReportInfo);
    callObjectPtr->SendUpdateCallMediaModeResponse(mode);
    callObjectPtr->ReceiveUpdateCallMediaModeResponse(callModeReportInfo);
    CallMediaModeInfo callMediaModeInfo;
    callMediaModeInfo.callId = static_cast<int32_t>(size);
    callMediaModeInfo.isRequestInfo = static_cast<bool>(size % BOOL_NUM);
    callMediaModeInfo.result = static_cast<VideoRequestResultType>(size % VIDEO_REQUEST_RESULT_TYPE_NUM);
    callMediaModeInfo.callMode = static_cast<ImsCallMode>(size % IMS_CALL_MODE_NUM);
    callObjectPtr->ReportImsCallModeInfo(callMediaModeInfo);
    callObjectPtr->ControlCamera(msg, callingUid, callingPid);
    callObjectPtr->SetPausePicture(msg);
    callObjectPtr->SetDeviceDirection(rotation);
    callObjectPtr->CancelCallUpgrade();
    callObjectPtr->RequestCameraCapabilities();
}

void OttVideoCallWindowFunc(const uint8_t *data, size_t size)
{
    if (!IsServiceInited()) {
        return;
    }
    DialParaInfo paraInfo;
    paraInfo.dialType = static_cast<DialType>(size % DIAL_TYPE);
    paraInfo.callType = static_cast<CallType>(size % CALL_TYPE_NUM);
    paraInfo.videoState = static_cast<VideoStateType>(size % VIDIO_TYPE_NUM);
    paraInfo.callState = static_cast<TelCallState>(size % TEL_CALL_STATE_NUM);
    sptr<OTTCall> callObjectPtr = std::make_unique<OTTCall>(paraInfo).release();
    std::string msg(reinterpret_cast<const char *>(data), size);
    int len = static_cast<int>(msg.length());
    std::string subSurfaceId = msg;
    if (len >= 1) {
        subSurfaceId = msg.substr(0, 1);
    }
    if (subSurfaceId.empty() || subSurfaceId[0] < '0' || subSurfaceId[0] > '9') {
        subSurfaceId = "";
        callObjectPtr->SetPreviewWindow(subSurfaceId, nullptr);
        callObjectPtr->SetDisplayWindow(subSurfaceId, nullptr);
    } else {
        uint64_t tmpSurfaceId = std::stoull(subSurfaceId);
        auto surface = SurfaceUtils::GetInstance()->GetSurface(tmpSurfaceId);
        callObjectPtr->SetPreviewWindow(subSurfaceId, surface);
        callObjectPtr->SetDisplayWindow(subSurfaceId, surface);
    }
}

void SatelliteCallFunc(const uint8_t *data, size_t size)
{
    if (!IsServiceInited()) {
        return;
    }

    DialParaInfo paraInfo;
    paraInfo.dialType = static_cast<DialType>(size % DIAL_TYPE);
    paraInfo.callType = static_cast<CallType>(size % CALL_TYPE_NUM);
    paraInfo.videoState = static_cast<VideoStateType>(size % VIDIO_TYPE_NUM);
    paraInfo.callState = static_cast<TelCallState>(size % TEL_CALL_STATE_NUM);
    sptr<SatelliteCall> callObjectPtr = std::make_unique<SatelliteCall>(paraInfo).release();
    int32_t videoState = static_cast<int32_t>(size % VIDIO_TYPE_NUM);
    CallAttributeInfo info;

    callObjectPtr->DialingProcess();
    callObjectPtr->AnswerCall(videoState);
    callObjectPtr->RejectCall();
    callObjectPtr->HangUpCall();
    callObjectPtr->GetCallAttributeInfo(info);
}

void DoSomethingInterestingWithMyAPI(const uint8_t *data, size_t size)
{
    if (data == nullptr || size == 0) {
        return;
    }

    CSCallFunc(data, size);
    DialingProcess(data, size);
    GetCallerInfo(data, size);
    IMSCallFunc(data, size);
    IMSVideoCallFunc(data, size);
    IMSVideoCallWindowFunc(data, size);
    OttCallFunc(data, size);
    VoIPCallFunc(data, size);
    OttVideoCallFunc(data, size);
    OttVideoCallWindowFunc(data, size);
    SatelliteCallFunc(data, size);
}
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size)
{
    OHOS::AddCallTokenFuzzer token;
    /* Run your code on data */
    OHOS::DoSomethingInterestingWithMyAPI(data, size);
    return 0;
}
