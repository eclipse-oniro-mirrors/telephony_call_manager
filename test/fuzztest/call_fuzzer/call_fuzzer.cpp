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
#include "antifraud_service.h"
#ifdef SUPPORT_DSOFTBUS
#include "interoperable_device_observer.h"
#include "interoperable_communication_manager.h"
#endif
#include "bluetooth_call_connection.h"
#include "bluetooth_call_state.h"
#include "antifraud_adapter.h"
#include "call_manager_service.h"
#include "call_manager_utils.h"
#include "fuzzer/FuzzedDataProvider.h"

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
constexpr int32_t DATA_COUNT = 2;

void CSCallFunc(FuzzedDataProvider& provider)
{
    if (!IsServiceInited()) {
        return;
    }

    DialParaInfo dialParaInfo;
    dialParaInfo.dialType = static_cast<DialType>(provider.ConsumeIntegralInRange<int32_t>(0, DIAL_TYPE));
    dialParaInfo.callType = static_cast<CallType>(provider.ConsumeIntegralInRange<int32_t>(0, CALL_TYPE_NUM));
    dialParaInfo.videoState = static_cast<VideoStateType>(
        provider.ConsumeIntegralInRange<int32_t>(0, VIDIO_TYPE_NUM));
    dialParaInfo.callState = static_cast<TelCallState>(
        provider.ConsumeIntegralInRange<int32_t>(0, TEL_CALL_STATE_NUM));
    sptr<CallBase> callObjectPtr = std::make_unique<CSCall>(dialParaInfo).release();
    int32_t videoState = provider.ConsumeIntegral<int32_t>();
    int32_t mute = provider.ConsumeIntegral<int32_t>();
    int32_t slotId = provider.ConsumeIntegral<int32_t>();
    TelCallState nextState = static_cast<TelCallState>(
        provider.ConsumeIntegralInRange<int32_t>(0, TEL_CALL_STATE_NUM));
    TelConferenceState telConferenceState = static_cast<TelConferenceState>(
        provider.ConsumeIntegralInRange<int32_t>(0, TEL_CONFERENCE_STATE_NUM));
    VideoStateType mediaType = static_cast<VideoStateType>(
        provider.ConsumeIntegralInRange<int32_t>(0, VIDIO_TYPE_NUM));
    PolicyFlag flag = static_cast<PolicyFlag>(provider.ConsumeIntegral<int32_t>());
    bool needAutoAnswer = provider.ConsumeBool();
    bool canUnHoldState = provider.ConsumeBool();

    callObjectPtr->AnswerCall(videoState);
    callObjectPtr->SetMute(mute, slotId);
    callObjectPtr->StartDtmf(provider.ConsumeIntegral<uint8_t>());
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

void DialingProcess(FuzzedDataProvider& provider)
{
    if (!IsServiceInited()) {
        return;
    }

    DialParaInfo paraInfo;
    paraInfo.dialType = static_cast<DialType>(
        provider.ConsumeIntegralInRange<int32_t>(0, DIAL_TYPE));
    paraInfo.callType = static_cast<CallType>(
        provider.ConsumeIntegralInRange<int32_t>(0, CALL_TYPE_NUM));
    paraInfo.videoState = static_cast<VideoStateType>(
        provider.ConsumeIntegralInRange<int32_t>(0, VIDIO_TYPE_NUM));
    paraInfo.callState = static_cast<TelCallState>(
        provider.ConsumeIntegralInRange<int32_t>(0, TEL_CALL_STATE_NUM));
    sptr<CallBase> callObjectPtr = std::make_unique<CSCall>(paraInfo).release();

    callObjectPtr->StartDtmf(provider.ConsumeIntegral<uint8_t>());
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

void GetCallerInfo(FuzzedDataProvider& provider)
{
    if (!IsServiceInited()) {
        return;
    }

    DialParaInfo info;
    info.dialType = static_cast<DialType>(
        provider.ConsumeIntegralInRange<int32_t>(0, DIAL_TYPE));
    info.callType = static_cast<CallType>(
        provider.ConsumeIntegralInRange<int32_t>(0, CALL_TYPE_NUM));
    info.videoState = static_cast<VideoStateType>(
        provider.ConsumeIntegralInRange<int32_t>(0, VIDIO_TYPE_NUM));
    info.callState = static_cast<TelCallState>(
        provider.ConsumeIntegralInRange<int32_t>(0, TEL_CALL_STATE_NUM));
    sptr<CallBase> callObjectPtr = std::make_unique<CSCall>(info).release();
    ContactInfo contactInfo;
    CallRunningState callRunningState = static_cast<CallRunningState>(
        provider.ConsumeIntegralInRange<int32_t>(0, CALL_RUNNING_STATE_NUM));
    bool speakerphoneOn = provider.ConsumeBool();
    std::string phoneNumber = provider.ConsumeRadomLengthString();
    int32_t callId = provider.ConsumeIntegral<int32_t>();
    CallAnswerType answerType = static_cast<CallAnswerType>(
        provider.ConsumeIntegralInRange<int32_t>(0, CALL_ANSWER_TYPE_NUM));
    int64_t startTime = provider.ConsumeIntegral<int64_t>();
    time_t callBeginTime = provider.ConsumeIntegral<time_t>();
    time_t callCreateTime = provider.ConsumeIntegral<time_t>();
    time_t callEndTime = provider.ConsumeIntegral<time_t>();
    time_t ringBeginTime = provider.ConsumeIntegral<time_t>();
    time_t ringEndTime = provider.ConsumeIntegral<time_t>();
    callObjectPtr->GetCallerInfo();
    callObjectPtr->SetCallerInfo(contactInfo);
    callObjectPtr->SetCallRunningState(callRunningState);
    callObjectPtr->SetStartTime(startTime);
    callObjectPtr->SetCallBeginTime(callBeginTime);
    callObjectPtr->SetCallBeginTime(callCreateTime);
    callObjectPtr->SetCallEndTime(callEndTime);
    callObjectPtr->SetRingBeginTime(ringBeginTime);
    callObjectPtr->SetRingEndTime(ringEndTime);
    callObjectPtr->SetAnswerType(answerType);
    callObjectPtr->SetCallId(callId);
    callObjectPtr->IsSpeakerphoneEnabled();
    callObjectPtr->IsCurrentRinging();
    callObjectPtr->GetAccountNumber();
    callObjectPtr->SetSpeakerphoneOn(speakerphoneOn);
    callObjectPtr->IsSpeakerphoneOn();
    callObjectPtr->CheckVoicemailNumber(phoneNumber);
    callObjectPtr->IsAliveState();
}

void IMSCallFunc(FuzzedDataProvider& provider)
{
    if (!IsServiceInited()) {
        return;
    }

    DialParaInfo paraInfo;
    paraInfo.dialType = static_cast<DialType>(
        provider.ConsumeIntegralInRange<int32_t>(0, DIAL_TYPE));
    paraInfo.callType = static_cast<CallType>(
        provider.ConsumeIntegralInRange<int32_t>(0, CALL_TYPE_NUM));
    paraInfo.videoState = static_cast<VideoStateType>(
        provider.ConsumeIntegralInRange<int32_t>(0, VIDIO_TYPE_NUM));
    paraInfo.callState = static_cast<TelCallState>(
        provider.ConsumeIntegralInRange<int32_t>(0, TEL_CALL_STATE_NUM));
    sptr<IMSCall> callObjectPtr = std::make_unique<IMSCall>(paraInfo).release();
    int32_t videoState = provider.ConsumeIntegral<int32_t>() % VIDIO_TYPE_NUM;
    int32_t mute = provider.ConsumeIntegral<int32_t>() % BOOL_NUM;
    int32_t slotId = provider.ConsumeIntegral<int32_t>() % SLOT_NUM;

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
#ifdef SUPPORT_RTT_CALL
    CellularCallInfo callInfo;
    callObjectPtr->UpdateImsRttCallMode(ImsRTTCallMode::LOCAL_REQUEST_UPGRADE);
#endif
    callObjectPtr->SetMute(mute, slotId);
}

void VoIPCallFunc(FuzzedDataProvider& provider)
{
    if (!IsServiceInited()) {
        return;
    }
    CallAttributeInfo info;
    DialParaInfo dialParaInfo;
    int32_t mainCallId = 0;
    VoipCallEventInfo voipcallInfo;
    std::vector<std::u16string> callIdList;
    int32_t videoState = provider.ConsumeIntegral<int32_t>() % VIDIO_TYPE_NUM;
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

void IMSVideoCallFunc(FuzzedDataProvider& provider)
{
    if (!IsServiceInited()) {
        return;
    }

    DialParaInfo paraInfo;
    paraInfo.dialType = static_cast<DialType>(
        provider.ConsumeIntegralInRange<int32_t>(0, DIAL_TYPE));
    paraInfo.callType = static_cast<CallType>(
        provider.ConsumeIntegralInRange<int32_t>(0, CALL_TYPE_NUM));
    paraInfo.videoState = static_cast<VideoStateType>(
        provider.ConsumeIntegralInRange<int32_t>(0, VIDIO_TYPE_NUM));
    paraInfo.callState = static_cast<TelCallState>(
        provider.ConsumeIntegralInRange<int32_t>(0, TEL_CALL_STATE_NUM));
    sptr<IMSCall> callObjectPtr = std::make_unique<IMSCall>(paraInfo).release();
    std::string msg = provider.ConsumeRadomLengthString();
    int32_t callingUid = provider.ConsumeIntegral<int32_t>();
    int32_t callingPid = provider.ConsumeIntegral<int32_t>();
    int32_t rotation = provider.ConsumeIntegral<int32_t>();
    ImsCallMode mode = static_cast<ImsCallMode>(
        provider.ConsumeIntegralInRange<int32_t>(0, IMS_CALL_MODE_NUM));
    CallModeReportInfo callModeReportInfo;
    callModeReportInfo.callIndex = provider.ConsumeIntegral<int32_t>() % CALL_INDEX_MAX_NUM;
    callModeReportInfo.callMode = static_cast<ImsCallMode>(
        provider.ConsumeIntegralInRange<int32_t>(0, IMS_CALL_MODE_NUM));
    callModeReportInfo.result = static_cast<VideoRequestResultType>(
        provider.ConsumeIntegralInRange<int32_t>(0, VIDEO_REQUEST_RESULT_TYPE_NUM));
    callObjectPtr->UpdateImsCallMode(mode);
    callObjectPtr->SendUpdateCallMediaModeRequest(mode);
    callObjectPtr->RecieveUpdateCallMediaModeRequest(callModeReportInfo);
    callObjectPtr->SendUpdateCallMediaModeResponse(mode);
    callObjectPtr->ReceiveUpdateCallMediaModeResponse(callModeReportInfo);
    CallMediaModeInfo callMediaModeInfo;
    callMediaModeInfo.callId = provider.ConsumeIntegral<int32_t>();
    callMediaModeInfo.isRequestInfo = provider.ConsumeBool();
    callMediaModeInfo.result = static_cast<VideoRequestResultType>(
        provider.ConsumeIntegralInRange<int32_t>(0, VIDEO_REQUEST_RESULT_TYPE_NUM));
    callMediaModeInfo.callMode = static_cast<ImsCallMode>(
        provider.ConsumeIntegralInRange<int32_t>(0, IMS_CALL_MODE_NUM));
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

void IMSVideoCallWindowFunc(FuzzedDataProvider& provider)
{
    if (!IsServiceInited()) {
        return;
    }

    DialParaInfo paraInfo;
    paraInfo.dialType = static_cast<DialType>(
        provider.ConsumeIntegralInRange<int32_t>(0, DIAL_TYPE));
    paraInfo.callType = static_cast<CallType>(
        provider.ConsumeIntegralInRange<int32_t>(0, CALL_TYPE_NUM));
    paraInfo.videoState = static_cast<VideoStateType>(
        provider.ConsumeIntegralInRange<int32_t>(0, VIDIO_TYPE_NUM));
    paraInfo.callState = static_cast<TelCallState>(
        provider.ConsumeIntegralInRange<int32_t>(0, TEL_CALL_STATE_NUM));
    sptr<IMSCall> callObjectPtr = std::make_unique<IMSCall>(paraInfo).release();
    std::string msg = provider.ConsumeRadomLengthString();
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

void OttCallFunc(FuzzedDataProvider& provider)
{
    if (!IsServiceInited()) {
        return;
    }

    DialParaInfo paraInfo;
    paraInfo.dialType = static_cast<DialType>(
        provider.ConsumeIntegralInRange<int32_t>(0, DIAL_TYPE));
    paraInfo.callType = static_cast<CallType>(
        provider.ConsumeIntegralInRange<int32_t>(0, CALL_TYPE_NUM));
    paraInfo.videoState = static_cast<VideoStateType>(
        provider.ConsumeIntegralInRange<int32_t>(0, VIDIO_TYPE_NUM));
    paraInfo.callState = static_cast<TelCallState>(
        provider.ConsumeIntegralInRange<int32_t>(0, TEL_CALL_STATE_NUM));
    sptr<OTTCall> callObjectPtr = std::make_unique<OTTCall>(paraInfo).release();
    int32_t videoState = provider.ConsumeIntegral<int32_t>() % VIDIO_TYPE_NUM;
    int32_t mute = provider.ConsumeIntegral<int32_t>() % BOOL_NUM;
    int32_t slotId = provider.ConsumeIntegral<int32_t>() % SLOT_NUM;

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

void OttVideoCallFunc(FuzzedDataProvider& provider)
{
    if (!IsServiceInited()) {
        return;
    }
    DialParaInfo paraInfo;
    paraInfo.dialType = static_cast<DialType>(
        provider.ConsumeIntegralInRange<int32_t>(0, DIAL_TYPE));
    paraInfo.callType = static_cast<CallType>(
        provider.ConsumeIntegralInRange<int32_t>(0, CALL_TYPE_NUM));
    paraInfo.videoState = static_cast<VideoStateType>(
        provider.ConsumeIntegralInRange<int32_t>(0, VIDIO_TYPE_NUM));
    paraInfo.callState = static_cast<TelCallState>(
        provider.ConsumeIntegralInRange<int32_t>(0, TEL_CALL_STATE_NUM));
    sptr<OTTCall> callObjectPtr = std::make_unique<OTTCall>(paraInfo).release();
    std::string msg = provider.ConsumeRadomLengthString();
    int32_t callingUid = provider.ConsumeIntegral<int32_t>();
    int32_t callingPid = provider.ConsumeIntegral<int32_t>();
    int32_t rotation = provider.ConsumeIntegral<int32_t>();
    callObjectPtr->InitVideoCall();
    ImsCallMode mode = static_cast<ImsCallMode>(
        provider.ConsumeIntegralInRange<int32_t>(0, IMS_CALL_MODE_NUM));
    CallModeReportInfo callModeReportInfo;
    callModeReportInfo.callIndex = provider.ConsumeIntegral<int32_t>();
    callModeReportInfo.callMode = static_cast<ImsCallMode>(
        provider.ConsumeIntegralInRange<int32_t>(0, IMS_CALL_MODE_NUM));
    callModeReportInfo.result = static_cast<VideoRequestResultType>(
        provider.ConsumeIntegralInRange<int32_t>(0, VIDEO_REQUEST_RESULT_TYPE_NUM));
    callObjectPtr->UpdateImsCallMode(mode);
    callObjectPtr->SendUpdateCallMediaModeRequest(mode);
    callObjectPtr->RecieveUpdateCallMediaModeRequest(callModeReportInfo);
    callObjectPtr->SendUpdateCallMediaModeResponse(mode);
    callObjectPtr->ReceiveUpdateCallMediaModeResponse(callModeReportInfo);
    CallMediaModeInfo callMediaModeInfo;
    callMediaModeInfo.callId = provider.ConsumeIntegral<int32_t>();
    callMediaModeInfo.isRequestInfo = provider.ConsumeBool();
    callMediaModeInfo.result = static_cast<VideoRequestResultType>(
        provider.ConsumeIntegralInRange<int32_t>(0, VIDEO_REQUEST_RESULT_TYPE_NUM));
    callMediaModeInfo.callMode = static_cast<ImsCallMode>(
        provider.ConsumeIntegralInRange<int32_t>(0, IMS_CALL_MODE_NUM));
    callObjectPtr->ReportImsCallModeInfo(callMediaModeInfo);
    callObjectPtr->ControlCamera(msg, callingUid, callingPid);
    callObjectPtr->SetPausePicture(msg);
    callObjectPtr->SetDeviceDirection(rotation);
    callObjectPtr->CancelCallUpgrade();
    callObjectPtr->RequestCameraCapabilities();
}

void OttVideoCallWindowFunc(FuzzedDataProvider& provider)
{
    if (!IsServiceInited()) {
        return;
    }
    DialParaInfo paraInfo;
    paraInfo.dialType = static_cast<DialType>(
        provider.ConsumeIntegralInRange<int32_t>(0, DIAL_TYPE));
    paraInfo.callType = static_cast<CallType>(
        provider.ConsumeIntegralInRange<int32_t>(0, CALL_TYPE_NUM));
    paraInfo.videoState = static_cast<VideoStateType>(
        provider.ConsumeIntegralInRange<int32_t>(0, VIDIO_TYPE_NUM));
    paraInfo.callState = static_cast<TelCallState>(
        provider.ConsumeIntegralInRange<int32_t>(0, TEL_CALL_STATE_NUM));
    sptr<OTTCall> callObjectPtr = std::make_unique<OTTCall>(paraInfo).release();
    std::string msg = provider.ConsumeRadomLengthString();
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

void SatelliteCallFunc(FuzzedDataProvider& provider)
{
    if (!IsServiceInited()) {
        return;
    }

    DialParaInfo paraInfo;
    paraInfo.dialType = static_cast<DialType>(
        provider.ConsumeIntegralInRange<int32_t>(0, DIAL_TYPE));
    paraInfo.callType = static_cast<CallType>(
        provider.ConsumeIntegralInRange<int32_t>(0, CALL_TYPE_NUM));
    paraInfo.videoState = static_cast<VideoStateType>(
        provider.ConsumeIntegralInRange<int32_t>(0, VIDIO_TYPE_NUM));
    paraInfo.callState = static_cast<TelCallState>(
        provider.ConsumeIntegralInRange<int32_t>(0, TEL_CALL_STATE_NUM));
    sptr<SatelliteCall> callObjectPtr = std::make_unique<SatelliteCall>(paraInfo).release();
    int32_t videoState = provider.ConsumeIntegral<int32_t>() % VIDIO_TYPE_NUM;
    CallAttributeInfo info;

    callObjectPtr->DialingProcess();
    callObjectPtr->AnswerCall(videoState);
    callObjectPtr->RejectCall();
    callObjectPtr->HangUpCall();
    callObjectPtr->GetCallAttributeInfo(info);
}

void AntiFraudServiceFunc(FuzzedDataProvider& provider)
{
    int index = 0;
    auto antiFraudService = DelayedSingleton<Telephony::AntiFraudService>::GetInstance();
    int32_t slotId = provider.ConsumeIntegral<int32_t>();
    int32_t count = provider.ConsumeIntegral<int32_t>();
    antiFraudService->CheckAntiFraudService(provider.ConsumeRandomLengthString(), slotId, count);
    antiFraudService->StartAntiFraudService(provider.ConsumeRandomLengthString(), slotId, count);
    uint8_t size = provider.ConsumeIntegral<uint8_t>()
    char temp[size + 1];
    for (size_t i = 0; i < sisizeze; ++i) {
        temp[i] = static_cast<char>(data[i]);
    }
    temp[size] = '\0';
    antiFraudService->CreateDataShareHelper(slotId, temp);
    antiFraudService->IsSwitchOn(provider.ConsumeRandomLengthString());
    antiFraudService->IsAntiFraudSwitchOn();
    antiFraudService->IsUserImprovementPlanSwitchOn();
    antiFraudService->InitParams();
    antiFraudService->GetStoppedSlotId();
    antiFraudService->GetStoppedIndex();
    antiFraudService->AnonymizeText();
    OHOS::AntiFraudService::AntiFraudResult antiFraudResult;
    antiFraudResult.modelVersion = provider.ConsumeIntegral<int32_t>();
    antiFraudResult.fraudType = provider.ConsumeIntegral<int32_t>();
    antiFraudService->RecordDetectResult(antiFraudResult, provider.ConsumeRandomLengthString(), slotId, count);
    antiFraudService->StopAntiFraudService(slotId, count);
    antiFraudService->SetStoppedSlotId(slotId);
    antiFraudService->SetStoppedIndex(count);
    auto antiFraudAdapter = DelayedSingleton<AntiFraudAdapter>::GetInstance();
    antiFraudAdapter->ReleaseAntiFraud();
    std::string phoneNum = provider.ConsumeRandomLengthString();
    auto listener = std::make_shared<Telephony::AntiFraudService::AntiFraudDetectResListenerImpl>(
        phoneNum, slotId, index);
    listener->HandleAntiFraudDetectRes(antiFraudResult);
}
#ifdef SUPPORT_DSOFTBUS
void InterOperableCommunicationManagerFunc(FuzzedDataProvider& provider)
{
    int index = 0;
    auto communicationManager = DelayedSingleton<InteroperableCommunicationManager>::GetInstance();
    DistributedHardware::DmDeviceInfo deviceInfo;
    deviceInfo.range = provider.ConsumeIntegral<int32_t>();
    deviceInfo.networkType = provider.ConsumeIntegral<int32_t>();
    communicationManager->SetMuted(provider.ConsumeBool());
    communicationManager->OnDeviceOnline(deviceInfo);
    communicationManager->OnDeviceOffline(deviceInfo);
    communicationManager->MuteRinger();
}

void InterOperableDeviceObserverFunc(FuzzedDataProvider& provider)
{
    auto observer = DelayedSingleton<InteroperableDeviceObserver>::GetInstance();
    auto stateCallback = std::make_shared<DmStateCallback>();
    observer->Init();
    size_t length = provider.ConsumeIntegral<uint8_t>() / DATA_COUNT;
    std::string networkId = provide.ConsumeRadomLengthString(length);
    std::string devName = provide.ConsumeRadomLengthString(length);
    observer->OnDeviceOnline(networkId, devName, provider.ConsumeIntegral<uint16_t>());
    observer->OnDeviceOffline(networkId, devName, provider.ConsumeIntegral<uint16_t>());
    DistributedHardware::DmDeviceInfo deviceInfo;
    deviceInfo.range = provider.ConsumeIntegral<int32_t>();
    deviceInfo.networkType = provider.ConsumeIntegral<int32_t>();
    stateCallback->OnDeviceOnline(deviceInfo);
    stateCallback->OnDeviceOffline(deviceInfo);
}
#endif
void BluetoothCallConnectionFunc(FuzzedDataProvider& provider)
{
    int index = 0;
    auto bluetoothConnection = DelayedSingleton<BluetoothCallConnection>::GetInstance();
    DialParaInfo info;
    info.accountId = provider.ConsumeIntegral<int32_t>();
    info.callId = provider.ConsumeIntegral<int32_t>();
    bluetoothConnection->Dial(info);
    bluetoothConnection->GetMacAddress();
    bluetoothConnection->ConnectBtSco();
    bluetoothConnection->DisConnectBtSco();
    bluetoothConnection->GetBtScoIsConnected();
    bluetoothConnection->SetHfpConnected(provider.ConsumeBool());
    bluetoothConnection->GetSupportBtCall();
    bluetoothConnection->SetBtCallScoConnected(provider.ConsumeBool());
    bluetoothConnection->HfpDisConnectedEndBtCall();
    size_t length = provider.ConsumeIntegral<uint8_t>() / DATA_COUNT;
    std::string hfpPhoneNumber = provide.ConsumeRadomLengthString(length);
    std::string hfpContactName = provide.ConsumeRadomLengthString(length);
    bluetoothConnection->SetHfpContactName(hfpPhoneNumber, hfpContactName);
    bluetoothConnection->GetHfpContactName(hfpPhoneNumber);
}

void BluetoothCallStateFunc(FuzzedDataProvider& provider)
{
    auto bluetoothCallState = std::make_shared<BluetoothCallState>();
    Bluetooth::BluetoothRemoteDevice device;
    int32_t state = provider.ConsumeIntegral<int32_t>();
    int32_t cause = provider.ConsumeIntegral<int32_t>();
    bluetoothCallState->OnConnectionStateChanged(device, state, cause);
    bluetoothCallState->OnScoStateChanged(device, state);
}

void CallMangerServiceFunc(FuzzedDataProvider& provider)
{
    auto callManagerService = DelayedSingleton<CallManagerService>::GetInstance();
    int index = 0;
    int32_t systemAbilityId = provider.ConsumeIntegral<int32_t>();
    callManagerService->OnAddSystemAbility(systemAbilityId, provider.ConsumeRandomLengthString());
    std::vector<std::u16string> args;
    std::u16string arg = Str8ToStr16(provider.ConsumeRandomLengthString());
    args.push_back(arg);
    std::int32_t fd = provider.ConsumeIntegral<int32_t>();
    callManagerService->Dump(fd, args);
    callManagerService->GetBindTime();
    callManagerService->GetStartServiceSpent();
    bool enabled = provider.ConsumeBool();
    callManagerService->IsNewCallAllowed(enabled);
    int32_t callId = provider.ConsumeIntegral<int32_t>();
    callManagerService->SwitchCall(callId);
    callManagerService->UnHoldCall(callId);
    callManagerService->StopDtmf(callId);
    callManagerService->EnableImsSwitch(callId);
    callManagerService->GetBundleInfo();
    callManagerService->dealCeliaCallEvent(callId);
    std::string eventName = fdp.ConsumeRandomLengthString();
    callManagerService->HandleVoIPCallEvent(callId, eventName);
    callManagerService->HandleDisplaySpecifiedCallPage(callId);
    callManagerService->HandleCeliaAutoAnswerCall(callId, provider.ConsumeBool());
    callManagerService->SendUssdResponse(callId, fdp.ConsumeRandomLengthString());
    callManagerService->OnStop();
    callManagerService->UnRegisterCallBack();
    callManagerService->UnInit();
    MessageParcel messageParcel;
    CallAttributeInfo info;
    info.accountId = provider.ConsumeIntegral<int32_t>();
    info.callType = CallType::TYPE_VOIP;
    CallManagerUtils::WriteCallAttributeInfo(info, messageParcel);
    CallManagerUtils::IsBundleInstalled(fdp.ConsumeRandomLengthString(), callId);
}

void CallMangerServiceStubFunc(FuzzedDataProvider& provider)
{
    auto callManagerService = DelayedSingleton<CallManagerService>::GetInstance();
    MessageParcel dataParcel;
    MessageParcel reply;
    int index = 0;
    int32_t callId = provider.ConsumeIntegral<int32_t>();
    dataParcel.WriteInt32(callId);
    callManagerService->OnUnHoldCall(dataParcel, reply);
    callManagerService->OnSwitchCall(dataParcel, reply);
    callManagerService->OnStopDtmf(dataParcel, reply);
    callManagerService->OnCombineConference(dataParcel, reply);
    callManagerService->OnEnableVoLte(dataParcel, reply);
    callManagerService->OnSendUssdResponse(dataParcel, reply);
    dataParcel.WriteBool(provider.ConsumeBool());
    callManagerService->OnIsNewCallAllowed(dataParcel, reply);
}

void DoSomethingInterestingWithMyAPI(const uint8_t *data, size_t size)
{
    if (data == nullptr || size == 0) {
        return;
    }

    FuzzedDataProvider provider(data, size);
    CSCallFunc(provider);
    DialingProcess(provider);
    GetCallerInfo(provider);
    IMSCallFunc(provider);
    IMSVideoCallFunc(provider);
    IMSVideoCallWindowFunc(provider);
    OttCallFunc(provider);
    VoIPCallFunc(provider);
    OttVideoCallFunc(provider);
    OttVideoCallWindowFunc(provider);
    SatelliteCallFunc(provider);
    AntiFraudServiceFunc(provider);
#ifdef SUPPORT_DSOFTBUS
    InterOperableCommunicationManagerFunc(provider);
    InterOperableDeviceObserverFunc(provider);
#endif
    BluetoothCallConnectionFunc(provider);
    BluetoothCallStateFunc(provider);
    CallMangerServiceFunc(provider);
    CallMangerServiceStubFunc(provider);
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
