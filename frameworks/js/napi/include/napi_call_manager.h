/*
 * Copyright (C) 2021 Huawei Device Co., Ltd.
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
    ~NapiCallManager() = default;

    static napi_value DeclareCallBasisInterface(napi_env env, napi_value exports);
    static napi_value DeclareCallConferenceInterface(napi_env env, napi_value exports);
    static napi_value DeclareCallSupplementInterface(napi_env env, napi_value exports);
    static napi_value DeclareCallExtendInterface(napi_env env, napi_value exports);
    static napi_value DeclareCallMultimediaInterface(napi_env env, napi_value exports);
    static napi_value DeclareCallImsInterface(napi_env env, napi_value exports);
    static napi_value DeclareCallMediaEnum(napi_env env, napi_value exports);
    static napi_value DeclareCallDialEnum(napi_env env, napi_value exports);
    static napi_value DeclareCallStateEnum(napi_env env, napi_value exports);
    static napi_value DeclareCallEventEnum(napi_env env, napi_value exports);
    static napi_value DeclareCallRestrictionEnum(napi_env env, napi_value exports);
    static napi_value DeclareCallWaitingEnum(napi_env env, napi_value exports);
    static napi_value DeclareCallTransferEnum(napi_env env, napi_value exports);
    // Enumeration type extension
    static napi_value DeclareAudioDeviceEnum(napi_env env, napi_value exports);
    static napi_value DeclareVideoStateTypeEnum(napi_env env, napi_value exports);
    static napi_value DeclareImsCallModeEnum(napi_env env, napi_value exports);
    static napi_value DeclareDialSceneEnum(napi_env env, napi_value exports);
    static napi_value DeclareCallTypeEnum(napi_env env, napi_value exports);
    static napi_value DeclareDialTypeEnum(napi_env env, napi_value exports);
    static napi_value DeclareTelCallStateEnum(napi_env env, napi_value exports);
    static napi_value DeclareConferenceStateEnum(napi_env env, napi_value exports);
    static napi_value DeclareCallStateToAppEnum(napi_env env, napi_value exports);
    static napi_value DeclareCallEventEnumEx(napi_env env, napi_value exports);
    static napi_value DeclareRestrictionTypeEnum(napi_env env, napi_value exports);
    static napi_value DeclareRestrictionModeEnum(napi_env env, napi_value exports);
    static napi_value DeclareRestrictionStatusEnum(napi_env env, napi_value exports);
    static napi_value DeclareCallWaitingEnumEx(napi_env env, napi_value exports);
    static napi_value DeclareTransferStatusEnum(napi_env env, napi_value exports);
    static napi_value DeclareTransferTypeEnum(napi_env env, napi_value exports);
    static napi_value DeclareTransferSettingTypeEnum(napi_env env, napi_value exports);
    static napi_value RegisterCallManagerFunc(napi_env env, napi_value exports);
    static napi_value DialCall(napi_env env, napi_callback_info info);
    static napi_value AnswerCall(napi_env env, napi_callback_info info);
    static napi_value RejectCall(napi_env env, napi_callback_info info);
    static napi_value HangUpCall(napi_env env, napi_callback_info info);
    static napi_value HoldCall(napi_env env, napi_callback_info info);
    static napi_value UnHoldCall(napi_env env, napi_callback_info info);
    static napi_value SwitchCall(napi_env env, napi_callback_info info);
    static napi_value CombineConference(napi_env env, napi_callback_info info);
    static napi_value SeparateConference(napi_env env, napi_callback_info info);
    static napi_value GetMainCallId(napi_env env, napi_callback_info info);
    static napi_value GetSubCallIdList(napi_env env, napi_callback_info info);
    static napi_value GetCallIdListForConference(napi_env env, napi_callback_info info);
    static napi_value GetCallWaiting(napi_env env, napi_callback_info info);
    static napi_value SetCallWaiting(napi_env env, napi_callback_info info);
    static napi_value GetCallRestriction(napi_env env, napi_callback_info info);
    static napi_value SetCallRestriction(napi_env env, napi_callback_info info);
    static napi_value GetCallTransferInfo(napi_env env, napi_callback_info info);
    static napi_value SetCallTransferInfo(napi_env env, napi_callback_info info);
    static napi_value StartDTMF(napi_env env, napi_callback_info info);
    static napi_value StopDTMF(napi_env env, napi_callback_info info);
    static napi_value GetCallState(napi_env env, napi_callback_info info);
    static napi_value IsRinging(napi_env env, napi_callback_info info);
    static napi_value HasCall(napi_env env, napi_callback_info info);
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
    static napi_value SetCallPreferenceMode(napi_env env, napi_callback_info info);
    static napi_value EnableImsSwitch(napi_env env, napi_callback_info info);
    static napi_value DisableImsSwitch(napi_env env, napi_callback_info info);
    static napi_value IsImsSwitchEnabled(napi_env env, napi_callback_info info);
    static napi_value StartRTT(napi_env env, napi_callback_info info);
    static napi_value StopRTT(napi_env env, napi_callback_info info);
    static napi_value JoinConference(napi_env env, napi_callback_info info);
    static napi_value UpdateImsCallMode(napi_env env, napi_callback_info info);
    static napi_value EnableLteEnhanceMode(napi_env env, napi_callback_info info);
    static napi_value DisableLteEnhanceMode(napi_env env, napi_callback_info info);
    static napi_value IsLteEnhanceModeEnabled(napi_env env, napi_callback_info info);
    static napi_value ReportOttCallDetailsInfo(napi_env env, napi_callback_info info);
    static napi_value ReportOttCallEventInfo(napi_env env, napi_callback_info info);

private:
    static void RegisterCallBack();
    static void NativeCallBack(napi_env env, napi_status status, void *data);
    static void NativeDialCallBack(napi_env env, napi_status status, void *data);
    static void NativeVoidCallBack(napi_env env, napi_status status, void *data);
    static void NativePropertyCallBack(napi_env env, napi_status status, void *data);
    static void NativeBoolCallBack(napi_env env, napi_status status, void *data);
    static void NativeFormatNumberCallBack(napi_env env, napi_status status, void *data);
    static void NativeListCallBack(napi_env env, napi_status status, void *data);
    static void GetDialInfo(napi_env env, napi_value objValue, DialAsyncContext &asyncContext);
    static void GetSmsInfo(napi_env env, napi_value objValue, RejectAsyncContext &asyncContext);
    static int32_t GetRestrictionInfo(napi_env env, napi_value objValue, CallRestrictionAsyncContext &asyncContext);
    static int32_t GetTransferInfo(napi_env env, napi_value objValue, CallTransferAsyncContext &asyncContext);
    static void NativeDialCall(napi_env env, void *data);
    static void NativeAnswerCall(napi_env env, void *data);
    static void NativeRejectCall(napi_env env, void *data);
    static void NativeHangUpCall(napi_env env, void *data);
    static void NativeHoldCall(napi_env env, void *data);
    static void NativeUnHoldCall(napi_env env, void *data);
    static void NativeSwitchCall(napi_env env, void *data);
    static void NativeCombineConference(napi_env env, void *data);
    static void NativeSeparateConference(napi_env env, void *data);
    static void NativeGetMainCallId(napi_env env, void *data);
    static void NativeGetSubCallIdList(napi_env env, void *data);
    static void NativeGetCallIdListForConference(napi_env env, void *data);
    static void NativeGetCallWaiting(napi_env env, void *data);
    static void NativeSetCallWaiting(napi_env env, void *data);
    static void NativeGetCallRestriction(napi_env env, void *data);
    static void NativeSetCallRestriction(napi_env env, void *data);
    static void NativeGetTransferNumber(napi_env env, void *data);
    static void NativeSetTransferNumber(napi_env env, void *data);
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
    static void NativeSetCallPreferenceMode(napi_env env, void *data);
    static void NativeIsImsSwitchEnabled(napi_env env, void *data);
    static void NativeEnableImsSwitch(napi_env env, void *data);
    static void NativeDisableImsSwitch(napi_env env, void *data);
    static void NativeStartRTT(napi_env env, void *data);
    static void NativeStopRTT(napi_env env, void *data);
    static void NativeJoinConference(napi_env env, void *data);
    static void NativeUpdateImsCallMode(napi_env env, void *data);
    static void NativeEnableLteEnhanceMode(napi_env env, void *data);
    static void NativeDisableLteEnhanceMode(napi_env env, void *data);
    static void NativeIsLteEnhanceModeEnabled(napi_env env, void *data);
    static void NativeReportOttCallDetailsInfo(napi_env env, void *data);
    static void NativeReportOttCallEventInfo(napi_env env, void *data);
    static napi_value HandleAsyncWork(napi_env env, AsyncContext *context, std::string workName,
        napi_async_execute_callback execute, napi_async_complete_callback complete);

private:
    static bool registerStatus_;
};
} // namespace Telephony
} // namespace OHOS

#endif // NAPI_CALL_MANAGER_H
