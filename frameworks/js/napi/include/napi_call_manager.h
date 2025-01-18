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

#ifndef NAPI_CALL_MANAGER_H
#define NAPI_CALL_MANAGER_H

#include "pac_map.h"
#include "string_ex.h"

#include "napi_call_manager_callback.h"
#include "napi_call_manager_types.h"

namespace OHOS {
namespace Telephony {
constexpr int32_t SLOT_ID_INVALID = 202;
constexpr const char *KEY_CONST_TELEPHONY_READ_SET_VOIP_CALL_INFO =
    "const.telephony.read_set_voip_call_info";
#define GET_PARAMS(env, info, num) \
    size_t argc = num;             \
    napi_value argv[num] = {0};    \
    napi_value thisVar = nullptr;  \
    void *data;                    \
    napi_get_cb_info(env, info, &argc, argv, &thisVar, &data)

/**
 * NapiCallManager is responsible for NAPI initialization and JavaScript data parsing.
 */
class NapiCallManager {
public:
    NapiCallManager();
    ~NapiCallManager();

    static napi_value DeclareCallBasisInterface(napi_env env, napi_value exports);
    static napi_value DeclareCallConferenceInterface(napi_env env, napi_value exports);
    static napi_value DeclareCallSupplementInterface(napi_env env, napi_value exports);
    static napi_value DeclareCallExtendInterface(napi_env env, napi_value exports);
    static napi_value DeclareCallMultimediaInterface(napi_env env, napi_value exports);
    static napi_value DeclareCallImsInterface(napi_env env, napi_value exports);
    // Enumeration type extension
    static napi_value DeclareVoNRStateEnum(napi_env env, napi_value exports);
    static napi_value DeclareAudioDeviceEnum(napi_env env, napi_value exports);
    static napi_value DeclareVideoStateTypeEnum(napi_env env, napi_value exports);
    static napi_value DeclareImsCallModeEnum(napi_env env, napi_value exports);
    static napi_value DeclareDeviceDirectionEnum(napi_env env, napi_value exports);
    static napi_value DeclareVideoRequestResultEnum(napi_env env, napi_value exports);
    static napi_value DeclareDialSceneEnum(napi_env env, napi_value exports);
    static napi_value DeclareCallTypeEnum(napi_env env, napi_value exports);
    static napi_value DeclareDialTypeEnum(napi_env env, napi_value exports);
    static napi_value DeclareTelCallStateEnum(napi_env env, napi_value exports);
    static napi_value DeclareConferenceStateEnum(napi_env env, napi_value exports);
    static napi_value DeclareCallStateToAppEnum(napi_env env, napi_value exports);
    static napi_value DeclareCallEventEnumEx(napi_env env, napi_value exports);
    static napi_value DeclareCallSessionEventEnumEx(napi_env env, napi_value exports);
    static napi_value DeclareRestrictionTypeEnum(napi_env env, napi_value exports);
    static napi_value DeclareRestrictionModeEnum(napi_env env, napi_value exports);
    static napi_value DeclareRestrictionStatusEnum(napi_env env, napi_value exports);
    static napi_value DeclareCallWaitingEnumEx(napi_env env, napi_value exports);
    static napi_value DeclareTransferStatusEnum(napi_env env, napi_value exports);
    static napi_value DeclareTransferTypeEnum(napi_env env, napi_value exports);
    static napi_value DeclareTransferSettingTypeEnum(napi_env env, napi_value exports);
    static napi_value DeclareMmiCodeResultEnum(napi_env env, napi_value exports);
    static napi_value DeclareDisconnectedReasonEnum(napi_env env, napi_value exports);
    static napi_value DeclareMarkTypeEnum(napi_env env, napi_value exports);
    static napi_value RegisterCallManagerFunc(napi_env env, napi_value exports);
    static napi_value Dial(napi_env env, napi_callback_info info);
    static napi_value DialCall(napi_env env, napi_callback_info info);
    static napi_value MakeCall(napi_env env, napi_callback_info info);
    static napi_value AnswerCall(napi_env env, napi_callback_info info);
    static napi_value AnswerVideoCall(napi_env env, napi_callback_info info);
    static napi_value RejectCall(napi_env env, napi_callback_info info);
    static napi_value HangUpCall(napi_env env, napi_callback_info info);
    static napi_value HoldCall(napi_env env, napi_callback_info info);
    static napi_value UnHoldCall(napi_env env, napi_callback_info info);
    static napi_value SwitchCall(napi_env env, napi_callback_info info);
    static napi_value CombineConference(napi_env env, napi_callback_info info);
    static napi_value SeparateConference(napi_env env, napi_callback_info info);
    static napi_value KickOutFromConference(napi_env env, napi_callback_info info);
    static napi_value GetMainCallId(napi_env env, napi_callback_info info);
    static napi_value GetSubCallIdList(napi_env env, napi_callback_info info);
    static napi_value GetCallIdListForConference(napi_env env, napi_callback_info info);
    static napi_value GetCallWaiting(napi_env env, napi_callback_info info);
    static napi_value SetCallWaiting(napi_env env, napi_callback_info info);
    static napi_value GetCallRestriction(napi_env env, napi_callback_info info);
    static napi_value SetCallRestriction(napi_env env, napi_callback_info info);
    static napi_value SetCallRestrictionPassword(napi_env env, napi_callback_info info);
    static napi_value GetCallTransferInfo(napi_env env, napi_callback_info info);
    static napi_value SetCallTransferInfo(napi_env env, napi_callback_info info);
    static napi_value CanSetCallTransferTime(napi_env env, napi_callback_info info);
    static napi_value StartDTMF(napi_env env, napi_callback_info info);
    static napi_value StopDTMF(napi_env env, napi_callback_info info);
    static napi_value PostDialProceed(napi_env env, napi_callback_info info);
    static napi_value GetCallState(napi_env env, napi_callback_info info);
    static napi_value GetCallStateSync(napi_env env, napi_callback_info info);
    static napi_value IsRinging(napi_env env, napi_callback_info info);
    static napi_value HasCall(napi_env env, napi_callback_info info);
    static napi_value HasCallSync(napi_env env, napi_callback_info info);
    static napi_value IsNewCallAllowed(napi_env env, napi_callback_info info);
    static napi_value IsInEmergencyCall(napi_env env, napi_callback_info info);
    static napi_value IsEmergencyPhoneNumber(napi_env env, napi_callback_info info);
    static napi_value FormatPhoneNumber(napi_env env, napi_callback_info info);
    static napi_value FormatPhoneNumberToE164(napi_env env, napi_callback_info info);
    static napi_value ObserverOn(napi_env env, napi_callback_info info);
    static napi_value ObserverOff(napi_env env, napi_callback_info info);
    static napi_value SetMuted(napi_env env, napi_callback_info info);
    static napi_value CancelMuted(napi_env env, napi_callback_info info);
    static napi_value MuteRinger(napi_env env, napi_callback_info info);
    static napi_value SetAudioDevice(napi_env env, napi_callback_info info);
    static napi_value ControlCamera(napi_env env, napi_callback_info info);
    static napi_value SetPreviewWindow(napi_env env, napi_callback_info info);
    static napi_value SetDisplayWindow(napi_env env, napi_callback_info info);
    static napi_value SetCameraZoom(napi_env env, napi_callback_info info);
    static napi_value SetPausePicture(napi_env env, napi_callback_info info);
    static napi_value SetDeviceDirection(napi_env env, napi_callback_info info);
    static napi_value RequestCameraCapabilities(napi_env env, napi_callback_info info);
    static napi_value SetCallPreferenceMode(napi_env env, napi_callback_info info);
    static napi_value EnableImsSwitch(napi_env env, napi_callback_info info);
    static napi_value DisableImsSwitch(napi_env env, napi_callback_info info);
    static napi_value IsImsSwitchEnabled(napi_env env, napi_callback_info info);
    static napi_value IsImsSwitchEnabledSync(napi_env env, napi_callback_info info);
    static napi_value SetVoNRState(napi_env env, napi_callback_info info);
    static napi_value GetVoNRState(napi_env env, napi_callback_info info);
    static napi_value StartRTT(napi_env env, napi_callback_info info);
    static napi_value StopRTT(napi_env env, napi_callback_info info);
    static napi_value JoinConference(napi_env env, napi_callback_info info);
    static napi_value UpdateImsCallMode(napi_env env, napi_callback_info info);
    static napi_value ReportOttCallDetailsInfo(napi_env env, napi_callback_info info);
    static napi_value ReportOttCallEventInfo(napi_env env, napi_callback_info info);
    static napi_value HasVoiceCapability(napi_env env, napi_callback_info info);
    static napi_value CloseUnFinishedUssd(napi_env env, napi_callback_info info);
    static napi_value InputDialerSpecialCode(napi_env env, napi_callback_info info);
    static napi_value RemoveMissedIncomingCallNotification(napi_env env, napi_callback_info info);
    static napi_value SetVoIPCallState(napi_env env, napi_callback_info info);
    static napi_value SetVoIPCallInfo(napi_env env, napi_callback_info info);
    static napi_value CancelCallUpgrade(napi_env env, napi_callback_info info);
    static napi_value SendCallUiEvent(napi_env env, napi_callback_info info);

private:
    static void RegisterCallBack();
    static void NativeCallBack(napi_env env, napi_status status, void *data);
    static void NativeDialCallBack(napi_env env, napi_status status, void *data);
    static void NativeVoidCallBack(napi_env env, napi_status status, void *data);
    static void NativeVoidCallBackWithErrorCode(napi_env env, napi_status status, void *data);
    static void NativePropertyCallBack(napi_env env, napi_status status, void *data);
    static void NativeGetMainCallIdCallBack(napi_env env, napi_status status, void *data);
    static void NativeIsImsSwitchEnabledCallBack(napi_env env, napi_status status, void *data);
    static void NativeGetVoNRStateCallBack(napi_env env, napi_status status, void *data);
    static void NativeBoolCallBack(napi_env env, napi_status status, void *data);
    static void NativeBoolCallBackWithErrorCode(napi_env env, napi_status status, void *data);
    static void NativeIsEmergencyPhoneNumberCallBack(napi_env env, napi_status status, void *data);
    static void NativeFormatNumberCallBack(napi_env env, napi_status status, void *data);
    static void NativeListCallBack(napi_env env, napi_status status, void *data);
    static void NativeOffCallBack(napi_env env, napi_status status, void *data);
    static void GetDialInfo(napi_env env, napi_value objValue, DialAsyncContext &asyncContext);
    static void GetSmsInfo(napi_env env, napi_value objValue, RejectAsyncContext &asyncContext);
    static int32_t GetRestrictionInfo(napi_env env, napi_value objValue, CallRestrictionAsyncContext &asyncContext);
    static int32_t GetTransferInfo(napi_env env, napi_value objValue, CallTransferAsyncContext &asyncContext);
    static void NativeDial(napi_env env, void *data);
    static void NativeDialCall(napi_env env, void *data);
    static void NativeMakeCall(napi_env env, void *data);
    static void NativeAnswerCall(napi_env env, void *data);
    static void NativeRejectCall(napi_env env, void *data);
    static void NativeHangUpCall(napi_env env, void *data);
    static void NativeHoldCall(napi_env env, void *data);
    static void NativeUnHoldCall(napi_env env, void *data);
    static void NativeSwitchCall(napi_env env, void *data);
    static void NativeCombineConference(napi_env env, void *data);
    static void NativeSeparateConference(napi_env env, void *data);
    static void NativeKickOutFromConference(napi_env env, void *data);
    static void NativeGetMainCallId(napi_env env, void *data);
    static void NativeGetSubCallIdList(napi_env env, void *data);
    static void NativeGetCallIdListForConference(napi_env env, void *data);
    static void NativeGetCallWaiting(napi_env env, void *data);
    static void NativeSetCallWaiting(napi_env env, void *data);
    static void NativeGetCallRestriction(napi_env env, void *data);
    static void NativeSetCallRestriction(napi_env env, void *data);
    static void NativeSetCallRestrictionPassword(napi_env env, void *data);
    static void NativeGetTransferNumber(napi_env env, void *data);
    static void NativeSetTransferNumber(napi_env env, void *data);
    static void NativeCanSetCallTransferTime(napi_env env, void *data);
    static void NativeStartDTMF(napi_env env, void *data);
    static void NativeStopDTMF(napi_env env, void *data);
    static void NativeGetCallState(napi_env env, void *data);
    static void NativeIsRinging(napi_env env, void *data);
    static void NativeHasCall(napi_env env, void *data);
    static void NativeIsNewCallAllowed(napi_env env, void *data);
    static void NativeIsInEmergencyCall(napi_env env, void *data);
    static void NativeIsEmergencyPhoneNumber(napi_env env, void *data);
    static void NativeFormatPhoneNumber(napi_env env, void *data);
    static void NativeFormatPhoneNumberToE164(napi_env env, void *data);
    static void NativeSetMuted(napi_env env, void *data);
    static void NativeCancelMuted(napi_env env, void *data);
    static void NativeMuteRinger(napi_env env, void *data);
    static void NativeSetAudioDevice(napi_env env, void *data);
    static void NativeControlCamera(napi_env env, void *data);
    static void NativeSetPreviewWindow(napi_env env, void *data);
    static void NativeSetDisplayWindow(napi_env env, void *data);
    static void NativeSetCameraZoom(napi_env env, void *data);
    static void NativeSetPausePicture(napi_env env, void *data);
    static void NativeSetDeviceDirection(napi_env env, void *data);
    static void NativeRequestCameraCapabilities(napi_env env, void *data);
    static void NativeSetCallPreferenceMode(napi_env env, void *data);
    static void NativeIsImsSwitchEnabled(napi_env env, void *data);
    static void NativeEnableImsSwitch(napi_env env, void *data);
    static void NativeDisableImsSwitch(napi_env env, void *data);
    static void NativeSetVoNRState(napi_env env, void *data);
    static void NativeGetVoNRState(napi_env env, void *data);
    static void NativeStartRTT(napi_env env, void *data);
    static void NativeStopRTT(napi_env env, void *data);
    static void NativeJoinConference(napi_env env, void *data);
    static void NativeUpdateImsCallMode(napi_env env, void *data);
    static void NativeCloseUnFinishedUssd(napi_env env, void *data);
    static void NativeInputDialerSpecialCode(napi_env env, void *data);
    static void NativeRemoveMissedIncomingCallNotification(napi_env env, void *data);
    static void NativeSetVoIPCallState(napi_env env, void *data);
    static void NativeSetVoIPCallInfo(napi_env env, void *data);
    static void NativeCancelCallUpgrade(napi_env env, void *data);
    static void NativeSendCallUiEvent(napi_env env, void *data);
    static bool MatchEmptyParameter(napi_env env, const napi_value parameters[], const size_t parameterCount);
    static bool MatchOneOptionalNumberParameter(
        napi_env env, const napi_value parameters[], const size_t parameterCount);
    static bool MatchAnswerCallParameter(
        napi_env env, const napi_value parameters[], const size_t parameterCount);
    static bool MatchTwoNumberAndStringParameters(napi_env env, const napi_value parameters[],
        const size_t parameterCount);
    static bool MatchOneNumberParameter(napi_env env, const napi_value parameters[], const size_t parameterCount);
    static bool MatchOneStringParameter(napi_env env, const napi_value parameters[], const size_t parameterCount);
    static bool MatchObserverOffParameter(napi_env env, const napi_value parameters[], const size_t parameterCount);
    static bool MatchTwoNumberParameters(napi_env env, const napi_value parameters[], const size_t parameterCount);
    static bool MatchNumberAndObjectParameters(
        napi_env env, const napi_value parameters[], const size_t parameterCount);
    static bool MatchNumberAndBoolParameters(napi_env env, const napi_value parameters[], const size_t parameterCount);
    static bool MatchNumberAndStringParameters(
        napi_env env, const napi_value parameters[], const size_t parameterCount);
    static bool MatchAudioDeviceParameters(napi_env env, const napi_value parameters[], const size_t parameterCount);
    static bool MatchRejectCallParameters(napi_env env, const napi_value parameters[], const size_t parameterCount);
    static bool MatchRejectCallFirstIllegalParameters(napi_env env, const napi_value parameters[]);
    static bool MatchRejectCallSecondIllegalParameters(napi_env env, const napi_value parameters[]);
    static bool MatchRejectCallTwoIllegalParameters(napi_env env, const napi_value parameters[]);
    static void NativeReportOttCallDetailsInfo(napi_env env, void *data);
    static void NativeReportOttCallEventInfo(napi_env env, void *data);
    static void NativePostDialProceed(napi_env env, void *data);
    static napi_value HandleAsyncWork(napi_env env, AsyncContext *context, std::string workName,
        napi_async_execute_callback execute, napi_async_complete_callback complete);
    static void RegisterNapiCallFuncCallback(std::string tmpStr, EventCallback stateCallback);
    static void UnRegisterCallbackWithListenerType(std::string tmpStr);
    static bool MatchStringAndVariableObjectParameters(
        napi_env env, const napi_value parameters[], const size_t parameterCount);
    static bool MatchTwoStringParameter(napi_env env, const napi_value parameters[], const size_t parameterCount);
    static bool MatchCallRestrictionPasswordParameter(
        napi_env env, const napi_value parameters[], const size_t parameterCount);
    static void HandleRejectCall(napi_env env, RejectAsyncContext &asyncContext,
        const napi_value parameters[], const size_t parameterCount);
    static bool NeedSetCallInfoThirdParyApp();
private:
    static int32_t registerStatus_;
};
} // namespace Telephony
} // namespace OHOS

#endif // NAPI_CALL_MANAGER_H
