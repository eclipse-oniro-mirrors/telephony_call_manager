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

#include "call_manager_service_stub.h"

#include <string_ex.h>

#include "call_manager_errors.h"
#include "telephony_log_wrapper.h"

#include "message_option.h"
#include "message_parcel.h"

#include "call_control_manager.h"

namespace OHOS {
namespace Telephony {
const int32_t MAX_CALLS_NUM = 5;

CallManagerServiceStub::CallManagerServiceStub()
{
    InitCallBasicRequest();
    InitCallUtilsRequest();
    InitCallConferenceRequest();
    InitCallDtmfRequest();
    InitCallSupplementRequest();
    initCallConferenceExRequest();
    InitCallMultimediaRequest();
    InitImsServiceRequest();
    InitOttServiceRequest();
    InitVoipOperationRequest();
    memberFuncMap_[INTERFACE_GET_PROXY_OBJECT_PTR] = &CallManagerServiceStub::OnGetProxyObjectPtr;
}

CallManagerServiceStub::~CallManagerServiceStub()
{
    memberFuncMap_.clear();
}

void CallManagerServiceStub::InitCallBasicRequest()
{
    memberFuncMap_[static_cast<int32_t>(CallManagerInterfaceCode::INTERFACE_REGISTER_CALLBACK)] =
        &CallManagerServiceStub::OnRegisterCallBack;
    memberFuncMap_[static_cast<int32_t>(CallManagerInterfaceCode::INTERFACE_UNREGISTER_CALLBACK)] =
        &CallManagerServiceStub::OnUnRegisterCallBack;
    memberFuncMap_[static_cast<int32_t>(CallManagerInterfaceCode::INTERFACE_DIAL_CALL)] =
        &CallManagerServiceStub::OnDialCall;
    memberFuncMap_[static_cast<int32_t>(CallManagerInterfaceCode::INTERFACE_ANSWER_CALL)] =
        &CallManagerServiceStub::OnAcceptCall;
    memberFuncMap_[static_cast<int32_t>(CallManagerInterfaceCode::INTERFACE_REJECT_CALL)] =
        &CallManagerServiceStub::OnRejectCall;
    memberFuncMap_[static_cast<int32_t>(CallManagerInterfaceCode::INTERFACE_HOLD_CALL)] =
        &CallManagerServiceStub::OnHoldCall;
    memberFuncMap_[static_cast<int32_t>(CallManagerInterfaceCode::INTERFACE_UNHOLD_CALL)] =
        &CallManagerServiceStub::OnUnHoldCall;
    memberFuncMap_[static_cast<int32_t>(CallManagerInterfaceCode::INTERFACE_DISCONNECT_CALL)] =
        &CallManagerServiceStub::OnHangUpCall;
    memberFuncMap_[static_cast<int32_t>(CallManagerInterfaceCode::INTERFACE_GET_CALL_STATE)] =
        &CallManagerServiceStub::OnGetCallState;
    memberFuncMap_[static_cast<int32_t>(CallManagerInterfaceCode::INTERFACE_SWAP_CALL)] =
        &CallManagerServiceStub::OnSwitchCall;
    memberFuncMap_[static_cast<int32_t>(CallManagerInterfaceCode::INTERFACE_INPUT_DIALER_SPECIAL_CODE)] =
        &CallManagerServiceStub::OnInputDialerSpecialCode;
}

void CallManagerServiceStub::InitCallUtilsRequest()
{
    memberFuncMap_[static_cast<int32_t>(CallManagerInterfaceCode::INTERFACE_HAS_CALL)] =
        &CallManagerServiceStub::OnHasCall;
    memberFuncMap_[static_cast<int32_t>(CallManagerInterfaceCode::INTERFACE_IS_NEW_CALL_ALLOWED)] =
        &CallManagerServiceStub::OnIsNewCallAllowed;
    memberFuncMap_[static_cast<int32_t>(CallManagerInterfaceCode::INTERFACE_IS_RINGING)] =
        &CallManagerServiceStub::OnIsRinging;
    memberFuncMap_[static_cast<int32_t>(CallManagerInterfaceCode::INTERFACE_IS_EMERGENCY_CALL)] =
        &CallManagerServiceStub::OnIsInEmergencyCall;
    memberFuncMap_[static_cast<int32_t>(CallManagerInterfaceCode::INTERFACE_IS_EMERGENCY_NUMBER)] =
        &CallManagerServiceStub::OnIsEmergencyPhoneNumber;
    memberFuncMap_[static_cast<int32_t>(CallManagerInterfaceCode::INTERFACE_IS_FORMAT_NUMBER)] =
        &CallManagerServiceStub::OnFormatPhoneNumber;
    memberFuncMap_[static_cast<int32_t>(CallManagerInterfaceCode::INTERFACE_IS_FORMAT_NUMBER_E164)] =
        &CallManagerServiceStub::OnFormatPhoneNumberToE164;
    memberFuncMap_[static_cast<int32_t>(CallManagerInterfaceCode::INTERFACE_CANCEL_MISSED_INCOMING_CALL_NOTIFICATION)] =
        &CallManagerServiceStub::OnRemoveMissedIncomingCallNotification;
    memberFuncMap_[static_cast<int32_t>(CallManagerInterfaceCode::INTERFACE_OBSERVER_ON_CALL_DETAILS_CHANGE)] =
        &CallManagerServiceStub::OnObserverOnCallDetailsChange;
}

void CallManagerServiceStub::InitCallConferenceRequest()
{
    memberFuncMap_[static_cast<int32_t>(CallManagerInterfaceCode::INTERFACE_COMBINE_CONFERENCE)] =
        &CallManagerServiceStub::OnCombineConference;
    memberFuncMap_[static_cast<int32_t>(CallManagerInterfaceCode::INTERFACE_SEPARATE_CONFERENCE)] =
        &CallManagerServiceStub::OnSeparateConference;
    memberFuncMap_[static_cast<int32_t>(CallManagerInterfaceCode::INTERFACE_JOIN_CONFERENCE)] =
        &CallManagerServiceStub::OnJoinConference;
    memberFuncMap_[static_cast<int32_t>(CallManagerInterfaceCode::INTERFACE_KICK_OUT_CONFERENCE)] =
        &CallManagerServiceStub::OnKickOutFromConference;
}

void CallManagerServiceStub::InitCallDtmfRequest()
{
    memberFuncMap_[static_cast<int32_t>(CallManagerInterfaceCode::INTERFACE_START_DTMF)] =
        &CallManagerServiceStub::OnStartDtmf;
    memberFuncMap_[static_cast<int32_t>(CallManagerInterfaceCode::INTERFACE_STOP_DTMF)] =
        &CallManagerServiceStub::OnStopDtmf;
    memberFuncMap_[static_cast<int32_t>(CallManagerInterfaceCode::INTERFACE_POST_DIAL_PROCEED)] =
        &CallManagerServiceStub::OnPostDialProceed;
}

void CallManagerServiceStub::InitCallSupplementRequest()
{
    memberFuncMap_[static_cast<int32_t>(CallManagerInterfaceCode::INTERFACE_GET_CALL_WAITING)] =
        &CallManagerServiceStub::OnGetCallWaiting;
    memberFuncMap_[static_cast<int32_t>(CallManagerInterfaceCode::INTERFACE_SET_CALL_WAITING)] =
        &CallManagerServiceStub::OnSetCallWaiting;
    memberFuncMap_[static_cast<int32_t>(CallManagerInterfaceCode::INTERFACE_GET_CALL_RESTRICTION)] =
        &CallManagerServiceStub::OnGetCallRestriction;
    memberFuncMap_[static_cast<int32_t>(CallManagerInterfaceCode::INTERFACE_SET_CALL_RESTRICTION)] =
        &CallManagerServiceStub::OnSetCallRestriction;
    memberFuncMap_[static_cast<int32_t>(CallManagerInterfaceCode::INTERFACE_SET_CALL_RESTRICTION_PASSWORD)] =
        &CallManagerServiceStub::OnSetCallRestrictionPassword;
    memberFuncMap_[static_cast<int32_t>(CallManagerInterfaceCode::INTERFACE_GET_CALL_TRANSFER)] =
        &CallManagerServiceStub::OnGetTransferNumber;
    memberFuncMap_[static_cast<int32_t>(CallManagerInterfaceCode::INTERFACE_SET_CALL_TRANSFER)] =
        &CallManagerServiceStub::OnSetTransferNumber;
    memberFuncMap_[static_cast<int32_t>(CallManagerInterfaceCode::INTERFACE_CAN_SET_CALL_TRANSFER_TIME)] =
        &CallManagerServiceStub::OnCanSetCallTransferTime;
    memberFuncMap_[static_cast<int32_t>(CallManagerInterfaceCode::INTERFACE_CLOSE_UNFINISHED_USSD)] =
        &CallManagerServiceStub::OnCloseUnFinishedUssd;
}

void CallManagerServiceStub::initCallConferenceExRequest()
{
    memberFuncMap_[static_cast<int32_t>(CallManagerInterfaceCode::INTERFACE_GET_MAINID)] =
        &CallManagerServiceStub::OnGetMainCallId;
    memberFuncMap_[static_cast<int32_t>(CallManagerInterfaceCode::INTERFACE_GET_SUBCALL_LIST_ID)] =
        &CallManagerServiceStub::OnGetSubCallIdList;
    memberFuncMap_[static_cast<int32_t>(CallManagerInterfaceCode::INTERFACE_GET_CALL_LIST_ID_FOR_CONFERENCE)] =
        &CallManagerServiceStub::OnGetCallIdListForConference;
}

void CallManagerServiceStub::InitCallMultimediaRequest()
{
    memberFuncMap_[static_cast<int32_t>(CallManagerInterfaceCode::INTERFACE_SET_MUTE)] =
        &CallManagerServiceStub::OnSetMute;
    memberFuncMap_[static_cast<int32_t>(CallManagerInterfaceCode::INTERFACE_MUTE_RINGER)] =
        &CallManagerServiceStub::OnMuteRinger;
    memberFuncMap_[static_cast<int32_t>(CallManagerInterfaceCode::INTERFACE_SET_AUDIO_DEVICE)] =
        &CallManagerServiceStub::OnSetAudioDevice;
    memberFuncMap_[static_cast<int32_t>(CallManagerInterfaceCode::INTERFACE_CTRL_CAMERA)] =
        &CallManagerServiceStub::OnControlCamera;
    memberFuncMap_[static_cast<int32_t>(CallManagerInterfaceCode::INTERFACE_SET_PREVIEW_WINDOW)] =
        &CallManagerServiceStub::OnSetPreviewWindow;
    memberFuncMap_[static_cast<int32_t>(CallManagerInterfaceCode::INTERFACE_SET_DISPLAY_WINDOW)] =
        &CallManagerServiceStub::OnSetDisplayWindow;
    memberFuncMap_[static_cast<int32_t>(CallManagerInterfaceCode::INTERFACE_SET_CAMERA_ZOOM)] =
        &CallManagerServiceStub::OnSetCameraZoom;
    memberFuncMap_[static_cast<int32_t>(CallManagerInterfaceCode::INTERFACE_SET_PAUSE_IMAGE)] =
        &CallManagerServiceStub::OnSetPausePicture;
    memberFuncMap_[static_cast<int32_t>(CallManagerInterfaceCode::INTERFACE_SET_DEVICE_DIRECTION)] =
        &CallManagerServiceStub::OnSetDeviceDirection;
    memberFuncMap_[static_cast<int32_t>(CallManagerInterfaceCode::INTERFACE_UPDATE_CALL_MEDIA_MODE)] =
        &CallManagerServiceStub::OnUpdateCallMediaMode;
    memberFuncMap_[static_cast<int32_t>(CallManagerInterfaceCode::INTERFACE_REPORT_AUDIO_DEVICE_INFO)] =
        &CallManagerServiceStub::OnReportAudioDeviceInfo;
    memberFuncMap_[static_cast<int32_t>(CallManagerInterfaceCode::INTERFACE_CANCEL_CALL_UPGRADE)] =
        &CallManagerServiceStub::OnCancelCallUpgrade;
    memberFuncMap_[static_cast<int32_t>(CallManagerInterfaceCode::INTERFACE_REQUEST_CAMERA_CAPABILITIES)] =
        &CallManagerServiceStub::OnRequestCameraCapabilities;
}

void CallManagerServiceStub::InitImsServiceRequest()
{
    memberFuncMap_[static_cast<int32_t>(CallManagerInterfaceCode::INTERFACE_SETCALL_PREFERENCEMODE)] =
        &CallManagerServiceStub::OnSetCallPreferenceMode;
    memberFuncMap_[static_cast<int32_t>(CallManagerInterfaceCode::INTERFACE_GET_IMS_CONFIG)] =
        &CallManagerServiceStub::OnGetImsConfig;
    memberFuncMap_[static_cast<int32_t>(CallManagerInterfaceCode::INTERFACE_SET_IMS_CONFIG)] =
        &CallManagerServiceStub::OnSetImsConfig;
    memberFuncMap_[static_cast<int32_t>(CallManagerInterfaceCode::INTERFACE_GET_IMS_FEATURE_VALUE)] =
        &CallManagerServiceStub::OnGetImsFeatureValue;
    memberFuncMap_[static_cast<int32_t>(CallManagerInterfaceCode::INTERFACE_SET_IMS_FEATURE_VALUE)] =
        &CallManagerServiceStub::OnSetImsFeatureValue;
    memberFuncMap_[static_cast<int32_t>(CallManagerInterfaceCode::INTERFACE_ENABLE_VOLTE)] =
        &CallManagerServiceStub::OnEnableVoLte;
    memberFuncMap_[static_cast<int32_t>(CallManagerInterfaceCode::INTERFACE_DISABLE_VOLTE)] =
        &CallManagerServiceStub::OnDisableVoLte;
    memberFuncMap_[static_cast<int32_t>(CallManagerInterfaceCode::INTERFACE_IS_VOLTE_ENABLED)] =
        &CallManagerServiceStub::OnIsVoLteEnabled;
    memberFuncMap_[static_cast<int32_t>(CallManagerInterfaceCode::INTERFACE_SET_VONR_STATE)] =
        &CallManagerServiceStub::OnSetVoNRState;
    memberFuncMap_[static_cast<int32_t>(CallManagerInterfaceCode::INTERFACE_GET_VONR_STATE)] =
        &CallManagerServiceStub::OnGetVoNRState;
    memberFuncMap_[static_cast<int32_t>(CallManagerInterfaceCode::INTERFACE_START_RTT)] =
        &CallManagerServiceStub::OnStartRtt;
    memberFuncMap_[static_cast<int32_t>(CallManagerInterfaceCode::INTERFACE_STOP_RTT)] =
        &CallManagerServiceStub::OnStopRtt;
    memberFuncMap_[static_cast<int32_t>(CallManagerInterfaceCode::INTERFACE_SET_VOIP_CALL_STATE)] =
        &CallManagerServiceStub::OnSetVoIPCallState;
    memberFuncMap_[static_cast<int32_t>(CallManagerInterfaceCode::INTERFACE_GET_VOIP_CALL_STATE)] =
        &CallManagerServiceStub::OnGetVoIPCallState;
}

void CallManagerServiceStub::InitOttServiceRequest()
{
    memberFuncMap_[static_cast<int32_t>(CallManagerInterfaceCode::INTERFACE_REPORT_OTT_CALL_DETAIL_INFO)] =
        &CallManagerServiceStub::OnReportOttCallDetailsInfo;
    memberFuncMap_[static_cast<int32_t>(CallManagerInterfaceCode::INTERFACE_REPORT_OTT_CALL_EVENT_INFO)] =
        &CallManagerServiceStub::OnReportOttCallEventInfo;
}

void CallManagerServiceStub::InitVoipOperationRequest()
{
    memberFuncMap_[static_cast<int32_t>(CallManagerInterfaceCode::INTERFACE_VOIP_REGISTER_CALLBACK)] =
        &CallManagerServiceStub::OnRegisterVoipCallManagerCallback;
    memberFuncMap_[static_cast<int32_t>(CallManagerInterfaceCode::INTERFACE_VOIP_UNREGISTER_CALLBACK)] =
        &CallManagerServiceStub::OnUnRegisterVoipCallManagerCallback;
}

int32_t CallManagerServiceStub::OnRegisterVoipCallManagerCallback(MessageParcel &data, MessageParcel &reply)
{
    int32_t result = RegisterVoipCallManagerCallback();
    TELEPHONY_LOGI("OnRegisterVoipCallManagerCallback result:%{public}d", result);
    if (!reply.WriteInt32(result)) {
        TELEPHONY_LOGE("OnRegisterVoipCallManagerCallback write reply failed.");
        return TELEPHONY_ERR_WRITE_REPLY_FAIL;
    }

    return TELEPHONY_SUCCESS;
}

int32_t CallManagerServiceStub::OnUnRegisterVoipCallManagerCallback(MessageParcel &data, MessageParcel &reply)
{
    int32_t result = TELEPHONY_ERR_FAIL;
    result = UnRegisterVoipCallManagerCallback();
    if (!reply.WriteInt32(result)) {
        TELEPHONY_LOGE("fail to write parcel");
        return TELEPHONY_ERR_WRITE_REPLY_FAIL;
    }
    return result;
}

int32_t CallManagerServiceStub::OnRemoteRequest(
    uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option)
{
    std::u16string myDescriptor = CallManagerServiceStub::GetDescriptor();
    std::u16string remoteDescriptor = data.ReadInterfaceToken();
    if (myDescriptor != remoteDescriptor) {
        TELEPHONY_LOGE("descriptor checked fail !");
        return TELEPHONY_ERR_DESCRIPTOR_MISMATCH;
    }
    TELEPHONY_LOGD("OnReceived, cmd = %{public}u", code);
    auto itFunc = memberFuncMap_.find(code);
    if (itFunc != memberFuncMap_.end()) {
        auto memberFunc = itFunc->second;
        if (memberFunc != nullptr) {
            return (this->*memberFunc)(data, reply);
        }
    }
    return IPCObjectStub::OnRemoteRequest(code, data, reply, option);
}

int32_t CallManagerServiceStub::OnRegisterCallBack(MessageParcel &data, MessageParcel &reply)
{
    int32_t result = TELEPHONY_ERR_FAIL;
    sptr<IRemoteObject> remote = data.ReadRemoteObject();
    if (remote == nullptr) {
        TELEPHONY_LOGE("callback ptr is nullptr.");
        reply.WriteInt32(result);
        return result;
    }
    sptr<ICallAbilityCallback> callback = iface_cast<ICallAbilityCallback>(remote);
    result = RegisterCallBack(callback);
    reply.WriteInt32(result);
    return result;
}

int32_t CallManagerServiceStub::OnUnRegisterCallBack(MessageParcel &data, MessageParcel &reply)
{
    int32_t result = TELEPHONY_ERR_FAIL;
    result = UnRegisterCallBack();
    if (!reply.WriteInt32(result)) {
        TELEPHONY_LOGE("fail to write parcel");
        return TELEPHONY_ERR_WRITE_REPLY_FAIL;
    }
    return result;
}

int32_t CallManagerServiceStub::OnObserverOnCallDetailsChange(MessageParcel &data, MessageParcel &reply)
{
    int32_t result = TELEPHONY_ERR_FAIL;
    result = ObserverOnCallDetailsChange();
    if (!reply.WriteInt32(result)) {
        TELEPHONY_LOGE("fail to write parcel");
        return TELEPHONY_ERR_WRITE_REPLY_FAIL;
    }
    return result;
}

int32_t CallManagerServiceStub::OnDialCall(MessageParcel &data, MessageParcel &reply)
{
    int32_t result = TELEPHONY_ERR_FAIL;
    AppExecFwk::PacMap dialInfo;
    std::u16string callNumber = data.ReadString16();
    dialInfo.PutIntValue("accountId", data.ReadInt32());
    dialInfo.PutIntValue("videoState", data.ReadInt32());
    dialInfo.PutIntValue("dialScene", data.ReadInt32());
    dialInfo.PutIntValue("dialType", data.ReadInt32());
    dialInfo.PutIntValue("callType", data.ReadInt32());
    dialInfo.PutStringValue("bundleName", data.ReadString());
    if (callNumber.length() > ACCOUNT_NUMBER_MAX_LENGTH) {
        TELEPHONY_LOGE("the account number length exceeds the limit");
        return CALL_ERR_NUMBER_OUT_OF_RANGE;
    }
    result = DialCall(callNumber, dialInfo);
    TELEPHONY_LOGI("result:%{public}d", result);
    if (!reply.WriteInt32(result)) {
        TELEPHONY_LOGE("fail to write parcel");
        return TELEPHONY_ERR_WRITE_REPLY_FAIL;
    }
    return result;
}

int32_t CallManagerServiceStub::OnAcceptCall(MessageParcel &data, MessageParcel &reply)
{
    int32_t callId = data.ReadInt32();
    int32_t videoState = data.ReadInt32();
    int32_t result = AnswerCall(callId, videoState);
    TELEPHONY_LOGI("result:%{public}d", result);
    if (!reply.WriteInt32(result)) {
        TELEPHONY_LOGE("fail to write parcel");
        return TELEPHONY_ERR_WRITE_REPLY_FAIL;
    }
    return result;
}

int32_t CallManagerServiceStub::OnRejectCall(MessageParcel &data, MessageParcel &reply)
{
    int32_t callId = data.ReadInt32();
    bool isSendSms = data.ReadBool();
    std::u16string content = data.ReadString16();
    int32_t result = RejectCall(callId, isSendSms, content);
    TELEPHONY_LOGI("result:%{public}d", result);
    if (!reply.WriteInt32(result)) {
        TELEPHONY_LOGE("fail to write parcel");
        return TELEPHONY_ERR_WRITE_REPLY_FAIL;
    }
    return result;
}

int32_t CallManagerServiceStub::OnHangUpCall(MessageParcel &data, MessageParcel &reply)
{
    int32_t callId = data.ReadInt32();
    int32_t result = HangUpCall(callId);
    TELEPHONY_LOGI("result:%{public}d", result);
    if (!reply.WriteInt32(result)) {
        TELEPHONY_LOGE("fail to write parcel");
        return TELEPHONY_ERR_WRITE_REPLY_FAIL;
    }
    return result;
}

int32_t CallManagerServiceStub::OnGetCallState(MessageParcel &data, MessageParcel &reply)
{
    int32_t result = GetCallState();
    TELEPHONY_LOGI("result:%{public}d", result);
    if (!reply.WriteInt32(result)) {
        TELEPHONY_LOGE("fail to write parcel");
        return TELEPHONY_ERR_WRITE_REPLY_FAIL;
    }
    return TELEPHONY_SUCCESS;
}

int32_t CallManagerServiceStub::OnHoldCall(MessageParcel &data, MessageParcel &reply)
{
    int32_t callId = data.ReadInt32();
    int32_t result = HoldCall(callId);
    TELEPHONY_LOGI("result:%{public}d", result);
    if (!reply.WriteInt32(result)) {
        TELEPHONY_LOGE("fail to write parcel");
        return TELEPHONY_ERR_WRITE_REPLY_FAIL;
    }
    return result;
}

int32_t CallManagerServiceStub::OnUnHoldCall(MessageParcel &data, MessageParcel &reply)
{
    int32_t callId = data.ReadInt32();
    int32_t result = UnHoldCall(callId);
    TELEPHONY_LOGI("result:%{public}d", result);
    if (!reply.WriteInt32(result)) {
        TELEPHONY_LOGE("fail to write parcel");
        return TELEPHONY_ERR_WRITE_REPLY_FAIL;
    }
    return result;
}

int32_t CallManagerServiceStub::OnSwitchCall(MessageParcel &data, MessageParcel &reply)
{
    int32_t callId = data.ReadInt32();
    int32_t result = SwitchCall(callId);
    TELEPHONY_LOGI("result:%{public}d", result);
    if (!reply.WriteInt32(result)) {
        TELEPHONY_LOGE("fail to write parcel");
        return TELEPHONY_ERR_WRITE_REPLY_FAIL;
    }
    return result;
}

int32_t CallManagerServiceStub::OnHasCall(MessageParcel &data, MessageParcel &reply)
{
    bool result = HasCall();
    TELEPHONY_LOGI("result:%{public}d", result);
    if (!reply.WriteBool(result)) {
        TELEPHONY_LOGE("fail to write parcel");
        return TELEPHONY_ERR_WRITE_REPLY_FAIL;
    }
    return TELEPHONY_SUCCESS;
}

int32_t CallManagerServiceStub::OnIsNewCallAllowed(MessageParcel &data, MessageParcel &reply)
{
    bool enabled = false;
    int32_t result = IsNewCallAllowed(enabled);
    TELEPHONY_LOGI("OnIsNewCallAllowed result:%{public}d enabled:%{public}d", result, enabled);
    if (!reply.WriteInt32(result)) {
        TELEPHONY_LOGE("OnIsNewCallAllowed write reply failed.");
        return TELEPHONY_ERR_WRITE_REPLY_FAIL;
    }
    if (result != TELEPHONY_ERR_SUCCESS) {
        return result;
    }
    if (!reply.WriteBool(enabled)) {
        TELEPHONY_LOGE("OnIsNewCallAllowed fail to write parcel");
        return TELEPHONY_ERR_WRITE_REPLY_FAIL;
    }
    return result;
}

int32_t CallManagerServiceStub::OnSetMute(MessageParcel &data, MessageParcel &reply)
{
    bool isMute = data.ReadBool();
    int32_t result = SetMuted(isMute);
    TELEPHONY_LOGI("result:%{public}d", result);
    if (!reply.WriteInt32(result)) {
        TELEPHONY_LOGE("fail to write parcel");
        return TELEPHONY_ERR_WRITE_REPLY_FAIL;
    }
    return TELEPHONY_SUCCESS;
}

int32_t CallManagerServiceStub::OnMuteRinger(MessageParcel &data, MessageParcel &reply)
{
    int32_t result = MuteRinger();
    TELEPHONY_LOGI("result:%{public}d", result);
    if (!reply.WriteInt32(result)) {
        TELEPHONY_LOGE("fail to write parcel");
        return TELEPHONY_ERR_WRITE_REPLY_FAIL;
    }
    return TELEPHONY_SUCCESS;
}

int32_t CallManagerServiceStub::OnSetAudioDevice(MessageParcel &data, MessageParcel &reply)
{
    AudioDevice *audioDevice = static_cast<AudioDevice *>(const_cast<void *>(data.ReadRawData(sizeof(AudioDevice))));
    if (audioDevice == nullptr) {
        TELEPHONY_LOGE("audioDevice is nullptr");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    int32_t result = SetAudioDevice(*audioDevice);
    TELEPHONY_LOGI("result:%{public}d", result);
    if (!reply.WriteInt32(result)) {
        TELEPHONY_LOGE("fail to write parcel");
        return TELEPHONY_ERR_WRITE_REPLY_FAIL;
    }
    return TELEPHONY_SUCCESS;
}

int32_t CallManagerServiceStub::OnIsRinging(MessageParcel &data, MessageParcel &reply)
{
    bool enabled = false;
    int32_t result = IsRinging(enabled);
    TELEPHONY_LOGI("result:%{public}d enabled:%{public}d", result, enabled);
    if (!reply.WriteInt32(result)) {
        TELEPHONY_LOGE("OnIsRinging write reply failed.");
        return TELEPHONY_ERR_WRITE_REPLY_FAIL;
    }
    if (result != TELEPHONY_ERR_SUCCESS) {
        return result;
    }
    if (!reply.WriteBool(enabled)) {
        TELEPHONY_LOGE("OnIsRinging fail to write parcel");
        return TELEPHONY_ERR_WRITE_REPLY_FAIL;
    }
    return result;
}

int32_t CallManagerServiceStub::OnIsInEmergencyCall(MessageParcel &data, MessageParcel &reply)
{
    bool enabled = false;
    int32_t result = IsInEmergencyCall(enabled);
    TELEPHONY_LOGI("result:%{public}d enabled:%{public}d", result, enabled);
    if (!reply.WriteInt32(result)) {
        TELEPHONY_LOGE("OnIsInEmergencyCall write reply failed.");
        return TELEPHONY_ERR_WRITE_REPLY_FAIL;
    }
    if (result != TELEPHONY_ERR_SUCCESS) {
        return result;
    }
    if (!reply.WriteBool(enabled)) {
        TELEPHONY_LOGE("OnIsInEmergencyCall fail to write parcel");
        return TELEPHONY_ERR_WRITE_REPLY_FAIL;
    }
    return result;
}

int32_t CallManagerServiceStub::OnStartDtmf(MessageParcel &data, MessageParcel &reply)
{
    int32_t result = TELEPHONY_ERR_FAIL;
    int32_t callId = data.ReadInt32();
    char str = static_cast<char>(data.ReadInt8());
    result = StartDtmf(callId, str);
    TELEPHONY_LOGI("result:%{public}d", result);
    if (!reply.WriteInt32(result)) {
        TELEPHONY_LOGE("fail to write parcel");
        return TELEPHONY_ERR_WRITE_REPLY_FAIL;
    }
    return TELEPHONY_SUCCESS;
}

int32_t CallManagerServiceStub::OnStopDtmf(MessageParcel &data, MessageParcel &reply)
{
    int32_t result = TELEPHONY_ERR_FAIL;
    int32_t callId = data.ReadInt32();
    result = StopDtmf(callId);
    TELEPHONY_LOGI("result:%{public}d", result);
    if (!reply.WriteInt32(result)) {
        TELEPHONY_LOGE("fail to write parcel");
        return TELEPHONY_ERR_WRITE_REPLY_FAIL;
    }
    return TELEPHONY_SUCCESS;
}

int32_t CallManagerServiceStub::OnPostDialProceed(MessageParcel &data, MessageParcel &reply)
{
    int32_t result = TELEPHONY_ERR_FAIL;
    int32_t callId = data.ReadInt32();
    bool proceed = data.ReadBool();

    result = PostDialProceed(callId, proceed);
    TELEPHONY_LOGI("result:%{public}d", result);
    if (!reply.WriteInt32(result)) {
        TELEPHONY_LOGE("fail to write parcel");
        return TELEPHONY_ERR_WRITE_REPLY_FAIL;
    }
    return TELEPHONY_SUCCESS;
}

int32_t CallManagerServiceStub::OnGetCallWaiting(MessageParcel &data, MessageParcel &reply)
{
    int32_t result = TELEPHONY_ERR_FAIL;
    int32_t slotId = data.ReadInt32();
    result = GetCallWaiting(slotId);
    TELEPHONY_LOGI("result:%{public}d", result);
    if (!reply.WriteInt32(result)) {
        TELEPHONY_LOGE("fail to write parcel");
        return TELEPHONY_ERR_WRITE_REPLY_FAIL;
    }
    return TELEPHONY_SUCCESS;
}

int32_t CallManagerServiceStub::OnSetCallWaiting(MessageParcel &data, MessageParcel &reply)
{
    int32_t result = TELEPHONY_ERR_FAIL;
    int32_t slotId = data.ReadInt32();
    bool activate = data.ReadBool();
    result = SetCallWaiting(slotId, activate);
    TELEPHONY_LOGI("result:%{public}d", result);
    if (!reply.WriteInt32(result)) {
        TELEPHONY_LOGE("fail to write parcel");
        return TELEPHONY_ERR_WRITE_REPLY_FAIL;
    }
    return TELEPHONY_SUCCESS;
}

int32_t CallManagerServiceStub::OnGetCallRestriction(MessageParcel &data, MessageParcel &reply)
{
    int32_t result = TELEPHONY_ERR_FAIL;
    int32_t slotId = data.ReadInt32();
    CallRestrictionType type = static_cast<CallRestrictionType>(data.ReadInt32());
    result = GetCallRestriction(slotId, type);
    TELEPHONY_LOGI("result:%{public}d", result);
    if (!reply.WriteInt32(result)) {
        TELEPHONY_LOGE("fail to write parcel");
        return TELEPHONY_ERR_WRITE_REPLY_FAIL;
    }
    return TELEPHONY_SUCCESS;
}

int32_t CallManagerServiceStub::OnSetCallRestriction(MessageParcel &data, MessageParcel &reply)
{
    int32_t result = TELEPHONY_ERR_FAIL;
    if (!data.ContainFileDescriptors()) {
        TELEPHONY_LOGW("sent raw data is less than 32k");
    }
    int32_t slotId = data.ReadInt32();
    CallRestrictionInfo *pCRInfo = (CallRestrictionInfo *)data.ReadRawData(sizeof(CallRestrictionInfo));
    if (pCRInfo == nullptr) {
        TELEPHONY_LOGE("data error");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    result = SetCallRestriction(slotId, *pCRInfo);
    TELEPHONY_LOGI("result:%{public}d", result);
    if (!reply.WriteInt32(result)) {
        TELEPHONY_LOGE("fail to write parcel");
        return TELEPHONY_ERR_WRITE_REPLY_FAIL;
    }
    return TELEPHONY_SUCCESS;
}

int32_t CallManagerServiceStub::OnSetCallRestrictionPassword(MessageParcel &data, MessageParcel &reply)
{
    int32_t result = TELEPHONY_ERR_FAIL;
    if (!data.ContainFileDescriptors()) {
        TELEPHONY_LOGW("sent raw data is less than 32k");
    }
    int32_t slotId = data.ReadInt32();
    CallRestrictionType fac = static_cast<CallRestrictionType>(data.ReadInt32());
    auto oldPassword = data.ReadCString();
    auto newPassword = data.ReadCString();
    result = SetCallRestrictionPassword(slotId, fac, oldPassword, newPassword);
    TELEPHONY_LOGI("result:%{public}d", result);
    if (!reply.WriteInt32(result)) {
        TELEPHONY_LOGE("fail to write parcel");
        return TELEPHONY_ERR_WRITE_REPLY_FAIL;
    }
    return TELEPHONY_SUCCESS;
}

int32_t CallManagerServiceStub::OnGetTransferNumber(MessageParcel &data, MessageParcel &reply)
{
    int32_t result = TELEPHONY_ERR_FAIL;
    int32_t slotId = data.ReadInt32();
    CallTransferType type = static_cast<CallTransferType>(data.ReadInt32());
    result = GetCallTransferInfo(slotId, type);
    TELEPHONY_LOGI("result:%{public}d", result);
    if (!reply.WriteInt32(result)) {
        TELEPHONY_LOGE("fail to write parcel");
        return TELEPHONY_ERR_WRITE_REPLY_FAIL;
    }
    return TELEPHONY_SUCCESS;
}

int32_t CallManagerServiceStub::OnSetTransferNumber(MessageParcel &data, MessageParcel &reply)
{
    int32_t result = TELEPHONY_ERR_FAIL;
    if (!data.ContainFileDescriptors()) {
        TELEPHONY_LOGW("sent raw data is less than 32k");
    }
    int32_t slotId = data.ReadInt32();
    CallTransferInfo *pCTInfo = (CallTransferInfo *)data.ReadRawData(sizeof(CallTransferInfo));
    if (pCTInfo == nullptr) {
        TELEPHONY_LOGE("data error");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    result = SetCallTransferInfo(slotId, *pCTInfo);
    TELEPHONY_LOGI("result:%{public}d", result);
    if (!reply.WriteInt32(result)) {
        TELEPHONY_LOGE("fail to write parcel");
        return TELEPHONY_ERR_WRITE_REPLY_FAIL;
    }
    return TELEPHONY_SUCCESS;
}

int32_t CallManagerServiceStub::OnCanSetCallTransferTime(MessageParcel &data, MessageParcel &reply)
{
    int32_t ret = TELEPHONY_ERR_FAIL;
    if (!data.ContainFileDescriptors()) {
        TELEPHONY_LOGW("sent raw data is less than 32k");
    }
    int32_t slotId = data.ReadInt32();
    bool result = data.ReadBool();
    ret = CanSetCallTransferTime(slotId, result);
    if (!reply.WriteBool(result)) {
        TELEPHONY_LOGE("[slot%{public}d] fail to write parcel with bool", slotId);
        return TELEPHONY_ERR_WRITE_REPLY_FAIL;
    }
    if (!reply.WriteInt32(ret)) {
        TELEPHONY_LOGE("[slot%{public}d] fail to write parcel", slotId);
        return TELEPHONY_ERR_WRITE_REPLY_FAIL;
    }
    return TELEPHONY_SUCCESS;
}

int32_t CallManagerServiceStub::OnCombineConference(MessageParcel &data, MessageParcel &reply)
{
    int32_t mainCallId = data.ReadInt32();
    int32_t result = CombineConference(mainCallId);
    TELEPHONY_LOGI("result:%{public}d", result);
    if (!reply.WriteInt32(result)) {
        TELEPHONY_LOGE("fail to write parcel");
        return TELEPHONY_ERR_WRITE_REPLY_FAIL;
    }
    return TELEPHONY_SUCCESS;
}

int32_t CallManagerServiceStub::OnSeparateConference(MessageParcel &data, MessageParcel &reply)
{
    int32_t callId = data.ReadInt32();
    int32_t result = SeparateConference(callId);
    TELEPHONY_LOGI("result:%{public}d", result);
    if (!reply.WriteInt32(result)) {
        TELEPHONY_LOGE("fail to write parcel");
        return TELEPHONY_ERR_WRITE_REPLY_FAIL;
    }
    return TELEPHONY_SUCCESS;
}

int32_t CallManagerServiceStub::OnJoinConference(MessageParcel &data, MessageParcel &reply)
{
    int32_t result = TELEPHONY_ERR_FAIL;
    int32_t callId = data.ReadInt32();
    std::vector<std::u16string> numberList;
    if (!data.ReadString16Vector(&numberList)) {
        TELEPHONY_LOGE("read conference participants numberList failed");
        return TELEPHONY_ERR_WRITE_DATA_FAIL;
    }
    result = JoinConference(callId, numberList);
    if (!reply.WriteInt32(result)) {
        TELEPHONY_LOGE("JoinConference fail to write parcel");
        return TELEPHONY_ERR_WRITE_REPLY_FAIL;
    }
    return TELEPHONY_SUCCESS;
}

int32_t CallManagerServiceStub::OnKickOutFromConference(MessageParcel &data, MessageParcel &reply)
{
    int32_t callId = data.ReadInt32();
    int32_t result = KickOutFromConference(callId);
    TELEPHONY_LOGI("result:%{public}d", result);
    if (!reply.WriteInt32(result)) {
        TELEPHONY_LOGE("fail to write parcel");
        return TELEPHONY_ERR_WRITE_REPLY_FAIL;
    }
    return TELEPHONY_SUCCESS;
}

int32_t CallManagerServiceStub::OnSetCallPreferenceMode(MessageParcel &data, MessageParcel &reply)
{
    int32_t result = TELEPHONY_ERR_FAIL;
    int32_t slotId = data.ReadInt32();
    int32_t mode = data.ReadInt32();
    result = SetCallPreferenceMode(slotId, mode);
    if (!reply.WriteInt32(result)) {
        TELEPHONY_LOGE("SetCallPreferenceMode fail to write parcel");
        return TELEPHONY_ERR_WRITE_REPLY_FAIL;
    }
    return TELEPHONY_SUCCESS;
}

int32_t CallManagerServiceStub::OnControlCamera(MessageParcel &data, MessageParcel &reply)
{
    int32_t result = TELEPHONY_ERR_FAIL;
    int32_t callId = data.ReadInt32();
    std::u16string cameraId = data.ReadString16();
    result = ControlCamera(callId, cameraId);
    TELEPHONY_LOGI("result:%{public}d", result);
    if (!reply.WriteInt32(result)) {
        TELEPHONY_LOGE("ControlCamera fail to write parcel");
        return TELEPHONY_ERR_WRITE_REPLY_FAIL;
    }
    return TELEPHONY_SUCCESS;
}

int32_t CallManagerServiceStub::OnSetPreviewWindow(MessageParcel &data, MessageParcel &reply)
{
    int32_t result = TELEPHONY_ERR_FAIL;
    int32_t callId = data.ReadInt32();
    std::string surfaceId = data.ReadString();
    sptr<Surface> surface = nullptr;
    sptr<IRemoteObject> object = data.ReadRemoteObject();
    if (object != nullptr) {
        sptr<IBufferProducer> producer = iface_cast<IBufferProducer>(object);
        surface = Surface::CreateSurfaceAsProducer(producer);
    }
    TELEPHONY_LOGI("surfaceId:%{public}s", surfaceId.c_str());
    result = SetPreviewWindow(callId, surfaceId, surface);
    if (!reply.WriteInt32(result)) {
        TELEPHONY_LOGE("SetPreviewWindow fail to write parcel");
        return TELEPHONY_ERR_WRITE_REPLY_FAIL;
    }
    return TELEPHONY_SUCCESS;
}

int32_t CallManagerServiceStub::OnSetDisplayWindow(MessageParcel &data, MessageParcel &reply)
{
    int32_t result = TELEPHONY_ERR_FAIL;
    int32_t callId = data.ReadInt32();
    std::string surfaceId = data.ReadString();
    sptr<Surface> surface = nullptr;
    sptr<IRemoteObject> object = data.ReadRemoteObject();
    if (object != nullptr) {
        sptr<IBufferProducer> producer = iface_cast<IBufferProducer>(object);
        surface = Surface::CreateSurfaceAsProducer(producer);
    }
    TELEPHONY_LOGI("surfaceId:%{public}s", surfaceId.c_str());
    result = SetDisplayWindow(callId, surfaceId, surface);
    if (!reply.WriteInt32(result)) {
        TELEPHONY_LOGE("SetDisplayWindow fail to write parcel");
        return TELEPHONY_ERR_WRITE_REPLY_FAIL;
    }
    return TELEPHONY_SUCCESS;
}

int32_t CallManagerServiceStub::OnSetCameraZoom(MessageParcel &data, MessageParcel &reply)
{
    int32_t result = TELEPHONY_ERR_FAIL;
    float zoom = data.ReadFloat();
    result = SetCameraZoom(zoom);
    TELEPHONY_LOGI("result:%{public}d", result);
    if (!reply.WriteInt32(result)) {
        TELEPHONY_LOGE("SetCameraZoom fail to write parcel");
        return TELEPHONY_ERR_WRITE_REPLY_FAIL;
    }
    return TELEPHONY_SUCCESS;
}

int32_t CallManagerServiceStub::OnSetPausePicture(MessageParcel &data, MessageParcel &reply)
{
    int32_t result = TELEPHONY_ERR_FAIL;
    int32_t callId = data.ReadInt32();
    std::u16string path = data.ReadString16();
    result = SetPausePicture(callId, path);
    TELEPHONY_LOGI("result:%{public}d", result);
    if (!reply.WriteInt32(result)) {
        TELEPHONY_LOGE("SetPausePicture fail to write parcel");
        return TELEPHONY_ERR_WRITE_REPLY_FAIL;
    }
    return TELEPHONY_SUCCESS;
}

int32_t CallManagerServiceStub::OnSetDeviceDirection(MessageParcel &data, MessageParcel &reply)
{
    int32_t result = TELEPHONY_ERR_FAIL;
    int32_t callId = data.ReadInt32();
    int32_t rotation = data.ReadInt32();
    result = SetDeviceDirection(callId, rotation);
    TELEPHONY_LOGI("result:%{public}d", result);
    if (!reply.WriteInt32(result)) {
        TELEPHONY_LOGE("SetDeviceDirection fail to write parcel");
        return TELEPHONY_ERR_WRITE_REPLY_FAIL;
    }
    return TELEPHONY_SUCCESS;
}

int32_t CallManagerServiceStub::OnIsEmergencyPhoneNumber(MessageParcel &data, MessageParcel &reply)
{
    std::u16string callNumber = data.ReadString16();
    int32_t slotId = data.ReadInt32();
    bool enabled = false;
    int32_t result = IsEmergencyPhoneNumber(callNumber, slotId, enabled);
    TELEPHONY_LOGI("OnIsEmergencyPhoneNumber result:%{public}d enabled:%{public}d", result, enabled);
    if (!reply.WriteInt32(result)) {
        TELEPHONY_LOGE("OnIsEmergencyPhoneNumber write reply failed.");
        return TELEPHONY_ERR_WRITE_REPLY_FAIL;
    }
    if (result != TELEPHONY_ERR_SUCCESS) {
        return result;
    }
    if (!reply.WriteBool(enabled)) {
        TELEPHONY_LOGE("OnIsEmergencyPhoneNumber fail to write parcel");
        return TELEPHONY_ERR_WRITE_REPLY_FAIL;
    }
    return TELEPHONY_SUCCESS;
}

int32_t CallManagerServiceStub::OnFormatPhoneNumber(MessageParcel &data, MessageParcel &reply)
{
    std::u16string callNumber = data.ReadString16();
    std::u16string countryCode = data.ReadString16();
    std::u16string formatNumber;
    int32_t result = FormatPhoneNumber(callNumber, countryCode, formatNumber);
    TELEPHONY_LOGI("result:%{public}d", result);
    if (!reply.WriteInt32(result)) {
        TELEPHONY_LOGE("OnFormatPhoneNumber write reply failed.");
        return TELEPHONY_ERR_WRITE_REPLY_FAIL;
    }
    if (result != TELEPHONY_ERR_SUCCESS) {
        return result;
    }
    if (!reply.WriteString16(formatNumber)) {
        TELEPHONY_LOGE("OnFormatPhoneNumber fail to write parcel");
        return TELEPHONY_ERR_WRITE_REPLY_FAIL;
    }
    return TELEPHONY_SUCCESS;
}

int32_t CallManagerServiceStub::OnFormatPhoneNumberToE164(MessageParcel &data, MessageParcel &reply)
{
    std::u16string callNumber = data.ReadString16();
    std::u16string countryCode = data.ReadString16();
    std::u16string formatNumber;
    int32_t result = FormatPhoneNumberToE164(callNumber, countryCode, formatNumber);
    TELEPHONY_LOGD("result:%{public}d", result);
    if (!reply.WriteInt32(result)) {
        TELEPHONY_LOGE("OnFormatPhoneNumberToE164 write reply failed.");
        return TELEPHONY_ERR_WRITE_REPLY_FAIL;
    }
    if (result != TELEPHONY_ERR_SUCCESS) {
        return result;
    }
    if (!reply.WriteString16(formatNumber)) {
        TELEPHONY_LOGE("OnFormatPhoneNumberToE164 fail to write parcel");
        return TELEPHONY_ERR_WRITE_REPLY_FAIL;
    }
    return TELEPHONY_SUCCESS;
}

int32_t CallManagerServiceStub::OnGetMainCallId(MessageParcel &data, MessageParcel &reply)
{
    int32_t callId = data.ReadInt32();
    int32_t mainCallId = 0;
    int32_t result = GetMainCallId(callId, mainCallId);
    TELEPHONY_LOGI("result:%{public}d mainCallId:%{public}d", result, mainCallId);
    if (!reply.WriteInt32(result)) {
        TELEPHONY_LOGE("fail to write parcel");
        return TELEPHONY_ERR_WRITE_REPLY_FAIL;
    }
    if (result != TELEPHONY_ERR_SUCCESS) {
        return result;
    }
    if (!reply.WriteInt32(mainCallId)) {
        TELEPHONY_LOGE("OnIsNewCallAllowed fail to write parcel");
        return TELEPHONY_ERR_WRITE_REPLY_FAIL;
    }
    return TELEPHONY_SUCCESS;
}

int32_t CallManagerServiceStub::OnGetSubCallIdList(MessageParcel &data, MessageParcel &reply)
{
    int32_t callId = data.ReadInt32();
    std::vector<std::u16string> callIdList;
    int32_t result = GetSubCallIdList(callId, callIdList);
    if (!reply.WriteInt32(result)) {
        TELEPHONY_LOGE("fail to write parcel");
        return TELEPHONY_ERR_WRITE_REPLY_FAIL;
    }
    if (result != TELEPHONY_ERR_SUCCESS) {
        return result;
    }
    if (!reply.WriteString16Vector(callIdList)) {
        TELEPHONY_LOGE("OnIsNewCallAllowed fail to write parcel");
        return TELEPHONY_ERR_WRITE_REPLY_FAIL;
    }
    return result;
}

int32_t CallManagerServiceStub::OnGetCallIdListForConference(MessageParcel &data, MessageParcel &reply)
{
    int32_t callId = data.ReadInt32();
    std::vector<std::u16string> callIdList;
    int32_t result = GetCallIdListForConference(callId, callIdList);
    if (!reply.WriteInt32(result)) {
        TELEPHONY_LOGE("fail to write parcel");
        return TELEPHONY_ERR_WRITE_REPLY_FAIL;
    }
    if (result != TELEPHONY_ERR_SUCCESS) {
        return result;
    }
    if (!reply.WriteString16Vector(callIdList)) {
        TELEPHONY_LOGE("OnIsNewCallAllowed fail to write parcel");
        return TELEPHONY_ERR_WRITE_REPLY_FAIL;
    }
    return result;
}

int32_t CallManagerServiceStub::OnGetImsConfig(MessageParcel &data, MessageParcel &reply)
{
    int32_t result = TELEPHONY_ERR_FAIL;
    int32_t slotId = data.ReadInt32();
    int32_t item = data.ReadInt32();
    result = GetImsConfig(slotId, static_cast<ImsConfigItem>(item));
    if (!reply.WriteInt32(result)) {
        TELEPHONY_LOGE("GetImsConfig fail to write parcel");
        return TELEPHONY_ERR_WRITE_REPLY_FAIL;
    }
    return TELEPHONY_SUCCESS;
}

int32_t CallManagerServiceStub::OnSetImsConfig(MessageParcel &data, MessageParcel &reply)
{
    int32_t result = TELEPHONY_ERR_FAIL;
    int32_t slotId = data.ReadInt32();
    int32_t item = data.ReadInt32();
    std::u16string value = data.ReadString16();
    result = SetImsConfig(slotId, static_cast<ImsConfigItem>(item), value);
    if (!reply.WriteInt32(result)) {
        TELEPHONY_LOGE("SetImsConfig fail to write parcel");
        return TELEPHONY_ERR_WRITE_REPLY_FAIL;
    }
    return TELEPHONY_SUCCESS;
}

int32_t CallManagerServiceStub::OnGetImsFeatureValue(MessageParcel &data, MessageParcel &reply)
{
    int32_t result = TELEPHONY_ERR_FAIL;
    int32_t slotId = data.ReadInt32();
    FeatureType type = static_cast<FeatureType>(data.ReadInt32());
    result = GetImsFeatureValue(slotId, type);
    if (!reply.WriteInt32(result)) {
        TELEPHONY_LOGE("GetImsFeatureValue fail to write parcel");
        return TELEPHONY_ERR_WRITE_REPLY_FAIL;
    }
    return TELEPHONY_SUCCESS;
}

int32_t CallManagerServiceStub::OnSetImsFeatureValue(MessageParcel &data, MessageParcel &reply)
{
    int32_t result = TELEPHONY_ERR_FAIL;
    int32_t slotId = data.ReadInt32();
    FeatureType type = static_cast<FeatureType>(data.ReadInt32());
    int32_t value = data.ReadInt32();
    result = SetImsFeatureValue(slotId, type, value);
    if (!reply.WriteInt32(result)) {
        TELEPHONY_LOGE("SetImsFeatureValue fail to write parcel");
        return TELEPHONY_ERR_WRITE_REPLY_FAIL;
    }
    return TELEPHONY_SUCCESS;
}

int32_t CallManagerServiceStub::OnUpdateCallMediaMode(MessageParcel &data, MessageParcel &reply)
{
    int32_t result = TELEPHONY_ERR_FAIL;
    if (!data.ContainFileDescriptors()) {
        TELEPHONY_LOGW("sent raw data is less than 32k");
    }
    int32_t callId = data.ReadInt32();
    ImsCallMode mode = static_cast<ImsCallMode>(data.ReadUint32());
    result = UpdateImsCallMode(callId, mode);
    if (!reply.WriteInt32(result)) {
        TELEPHONY_LOGE("OnUpdateCallMediaMode fail to write parcel");
        return TELEPHONY_ERR_WRITE_REPLY_FAIL;
    }
    return TELEPHONY_SUCCESS;
}

int32_t CallManagerServiceStub::OnEnableVoLte(MessageParcel &data, MessageParcel &reply)
{
    int32_t result = TELEPHONY_ERR_FAIL;
    int32_t slotId = data.ReadInt32();
    result = EnableImsSwitch(slotId);
    if (!reply.WriteInt32(result)) {
        TELEPHONY_LOGE("EnableImsSwitch fail to write parcel");
        return TELEPHONY_ERR_WRITE_REPLY_FAIL;
    }
    return TELEPHONY_SUCCESS;
}

int32_t CallManagerServiceStub::OnDisableVoLte(MessageParcel &data, MessageParcel &reply)
{
    int32_t result = TELEPHONY_ERR_FAIL;
    int32_t slotId = data.ReadInt32();
    result = DisableImsSwitch(slotId);
    if (!reply.WriteInt32(result)) {
        TELEPHONY_LOGE("DisableImsSwitch fail to write parcel");
        return TELEPHONY_ERR_WRITE_REPLY_FAIL;
    }
    return TELEPHONY_SUCCESS;
}

int32_t CallManagerServiceStub::OnIsVoLteEnabled(MessageParcel &data, MessageParcel &reply)
{
    int32_t result = TELEPHONY_ERR_FAIL;
    int32_t slotId = data.ReadInt32();
    bool enabled;
    result = IsImsSwitchEnabled(slotId, enabled);
    if (!reply.WriteBool(enabled)) {
        TELEPHONY_LOGE("IsImsSwitchEnabled fail to write parcel");
        return TELEPHONY_ERR_WRITE_REPLY_FAIL;
    }
    if (!reply.WriteInt32(result)) {
        TELEPHONY_LOGE("IsImsSwitchEnabled fail to write parcel");
        return TELEPHONY_ERR_WRITE_REPLY_FAIL;
    }
    return TELEPHONY_SUCCESS;
}

int32_t CallManagerServiceStub::OnSetVoNRState(MessageParcel &data, MessageParcel &reply)
{
    int32_t result = TELEPHONY_ERR_FAIL;
    int32_t slotId = data.ReadInt32();
    int32_t state = data.ReadInt32();
    result = SetVoNRState(slotId, state);
    if (!reply.WriteInt32(result)) {
        TELEPHONY_LOGE("SetVoNRState fail to write parcel");
        return TELEPHONY_ERR_WRITE_REPLY_FAIL;
    }
    return TELEPHONY_SUCCESS;
}

int32_t CallManagerServiceStub::OnGetVoNRState(MessageParcel &data, MessageParcel &reply)
{
    int32_t result = TELEPHONY_ERR_FAIL;
    int32_t slotId = data.ReadInt32();
    int32_t state;
    result = GetVoNRState(slotId, state);
    if (!reply.WriteInt32(state)) {
        TELEPHONY_LOGE("GetVoNRState fail to write parcel");
        return TELEPHONY_ERR_WRITE_REPLY_FAIL;
    }
    if (!reply.WriteInt32(result)) {
        TELEPHONY_LOGE("GetVoNRState fail to write parcel");
        return TELEPHONY_ERR_WRITE_REPLY_FAIL;
    }
    return TELEPHONY_SUCCESS;
}

int32_t CallManagerServiceStub::OnStartRtt(MessageParcel &data, MessageParcel &reply)
{
    int32_t result = TELEPHONY_ERR_FAIL;
    int32_t callId = data.ReadInt32();
    std::u16string msg = data.ReadString16();
    result = StartRtt(callId, msg);
    if (!reply.WriteInt32(result)) {
        TELEPHONY_LOGE("fail to write parcel");
        return TELEPHONY_ERR_WRITE_REPLY_FAIL;
    }
    return TELEPHONY_SUCCESS;
}

int32_t CallManagerServiceStub::OnStopRtt(MessageParcel &data, MessageParcel &reply)
{
    int32_t result = TELEPHONY_ERR_FAIL;
    int32_t callId = data.ReadInt32();
    result = StopRtt(callId);
    if (!reply.WriteInt32(result)) {
        TELEPHONY_LOGE("fail to write parcel");
        return TELEPHONY_ERR_WRITE_REPLY_FAIL;
    }
    return TELEPHONY_SUCCESS;
}

int32_t CallManagerServiceStub::OnReportOttCallDetailsInfo(MessageParcel &data, MessageParcel &reply)
{
    int32_t result = TELEPHONY_ERR_FAIL;
    if (!data.ContainFileDescriptors()) {
        TELEPHONY_LOGW("sent raw data is less than 32k");
    }
    int32_t vecCnt = data.ReadInt32();
    if (vecCnt <= 0 || vecCnt > MAX_CALLS_NUM) {
        TELEPHONY_LOGE("vector is empty");
        return TELEPHONY_ERR_ARGUMENT_INVALID;
    }
    std::vector<OttCallDetailsInfo> ottVec;
    OttCallDetailsInfo *infoPtr = nullptr;
    for (int32_t i = 0; i < vecCnt; i++) {
        if ((infoPtr = (OttCallDetailsInfo *)data.ReadRawData(sizeof(OttCallDetailsInfo))) == nullptr) {
            TELEPHONY_LOGE("reading raw data failed");
            return TELEPHONY_ERR_LOCAL_PTR_NULL;
        }
        ottVec.push_back(*infoPtr);
    }
    result = ReportOttCallDetailsInfo(ottVec);
    if (!reply.WriteInt32(result)) {
        TELEPHONY_LOGE("ReportOttCallDetailsInfo fail to write parcel");
        return TELEPHONY_ERR_WRITE_REPLY_FAIL;
    }
    return TELEPHONY_SUCCESS;
}

int32_t CallManagerServiceStub::OnReportOttCallEventInfo(MessageParcel &data, MessageParcel &reply)
{
    int32_t result = TELEPHONY_ERR_FAIL;
    if (!data.ContainFileDescriptors()) {
        TELEPHONY_LOGW("sent raw data is less than 32k");
    }
    OttCallEventInfo *pEventInfo = (OttCallEventInfo *)data.ReadRawData(sizeof(OttCallEventInfo));
    if (pEventInfo == nullptr) {
        TELEPHONY_LOGE("data error");
        return result;
    }
    result = ReportOttCallEventInfo(*pEventInfo);
    if (!reply.WriteInt32(result)) {
        TELEPHONY_LOGE("ReportOttCallDetailsInfo fail to write parcel");
        return TELEPHONY_ERR_WRITE_REPLY_FAIL;
    }
    return TELEPHONY_SUCCESS;
}

int32_t CallManagerServiceStub::OnCloseUnFinishedUssd(MessageParcel &data, MessageParcel &reply)
{
    int32_t result = TELEPHONY_ERR_FAIL;
    int32_t slotId = data.ReadInt32();
    result = CloseUnFinishedUssd(slotId);
    TELEPHONY_LOGI("result:%{public}d", result);
    if (!reply.WriteInt32(result)) {
        TELEPHONY_LOGE("fail to write parcel");
        return TELEPHONY_ERR_WRITE_REPLY_FAIL;
    }
    return TELEPHONY_SUCCESS;
}

int32_t CallManagerServiceStub::OnInputDialerSpecialCode(MessageParcel &data, MessageParcel &reply)
{
    int32_t result = TELEPHONY_ERR_FAIL;
    std::string specialCode = data.ReadString();
    result = InputDialerSpecialCode(specialCode);
    TELEPHONY_LOGI("result:%{public}d", result);
    if (!reply.WriteInt32(result)) {
        TELEPHONY_LOGE("fail to write parcel");
        return TELEPHONY_ERR_WRITE_REPLY_FAIL;
    }
    return TELEPHONY_SUCCESS;
}

int32_t CallManagerServiceStub::OnRemoveMissedIncomingCallNotification(MessageParcel &data, MessageParcel &reply)
{
    int32_t result = RemoveMissedIncomingCallNotification();
    if (!reply.WriteInt32(result)) {
        TELEPHONY_LOGE("OnRemoveMissedIncomingCallNotification fail to write parcel");
        return TELEPHONY_ERR_WRITE_REPLY_FAIL;
    }
    return TELEPHONY_SUCCESS;
}

int32_t CallManagerServiceStub::OnSetVoIPCallState(MessageParcel &data, MessageParcel &reply)
{
    int32_t result = TELEPHONY_ERR_FAIL;
    int32_t state = data.ReadInt32();
    result = SetVoIPCallState(state);
    if (!reply.WriteInt32(result)) {
        TELEPHONY_LOGE("SetVoIPCallState fail to write parcel");
        return TELEPHONY_ERR_WRITE_REPLY_FAIL;
    }
    return TELEPHONY_SUCCESS;
}

int32_t CallManagerServiceStub::OnGetVoIPCallState(MessageParcel &data, MessageParcel &reply)
{
    int32_t result = TELEPHONY_ERR_FAIL;
    int32_t state;
    result = GetVoIPCallState(state);
    if (!reply.WriteInt32(state)) {
        TELEPHONY_LOGE("GetVoIPCallState fail to write parcel");
        return TELEPHONY_ERR_WRITE_REPLY_FAIL;
    }
    if (!reply.WriteInt32(result)) {
        TELEPHONY_LOGE("GetVoIPCallState fail to write parcel");
        return TELEPHONY_ERR_WRITE_REPLY_FAIL;
    }
    return TELEPHONY_SUCCESS;
}

int32_t CallManagerServiceStub::OnGetProxyObjectPtr(MessageParcel &data, MessageParcel &reply)
{
    CallManagerProxyType proxyType = static_cast<CallManagerProxyType>(data.ReadInt32());
    sptr<IRemoteObject> objectPtr = GetProxyObjectPtr(proxyType);
    if (!reply.WriteRemoteObject(objectPtr)) {
        TELEPHONY_LOGE("OnGetProxyObjectPtr fail to write parcel");
        return TELEPHONY_ERR_WRITE_REPLY_FAIL;
    }
    return TELEPHONY_SUCCESS;
}

int32_t CallManagerServiceStub::OnReportAudioDeviceInfo(MessageParcel &data, MessageParcel &reply)
{
    int32_t result = ReportAudioDeviceInfo();
    TELEPHONY_LOGI("OnReportAudioDeviceInfo result:%{public}d", result);
    if (!reply.WriteInt32(result)) {
        TELEPHONY_LOGE("OnReportAudioDeviceInfo write reply failed.");
        return TELEPHONY_ERR_WRITE_REPLY_FAIL;
    }

    return TELEPHONY_SUCCESS;
}

int32_t CallManagerServiceStub::OnCancelCallUpgrade(MessageParcel &data, MessageParcel &reply)
{
    int32_t result = TELEPHONY_ERR_FAIL;
    int32_t callId = data.ReadInt32();
    result = CancelCallUpgrade(callId);
    if (!reply.WriteInt32(result)) {
        TELEPHONY_LOGE("fail to write parcel");
        return TELEPHONY_ERR_WRITE_REPLY_FAIL;
    }
    return TELEPHONY_SUCCESS;
}

int32_t CallManagerServiceStub::OnRequestCameraCapabilities(MessageParcel &data, MessageParcel &reply)
{
    int32_t result = TELEPHONY_ERR_FAIL;
    int32_t callId = data.ReadInt32();
    result = RequestCameraCapabilities(callId);
    if (!reply.WriteInt32(result)) {
        TELEPHONY_LOGE("fail to write parcel");
        return TELEPHONY_ERR_WRITE_REPLY_FAIL;
    }
    return TELEPHONY_SUCCESS;
}
} // namespace Telephony
} // namespace OHOS
