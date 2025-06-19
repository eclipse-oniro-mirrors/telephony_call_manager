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
#include "telephony_cust_wrapper.h"

#include "message_option.h"
#include "message_parcel.h"

#include "call_control_manager.h"

#ifdef HICOLLIE_ENABLE
#include "xcollie/xcollie.h"
#include "xcollie/xcollie_define.h"
#define XCOLLIE_TIMEOUT_SECONDS 30
#endif

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
    InitBluetoothOperationRequest();
    memberFuncMap_[INTERFACE_GET_PROXY_OBJECT_PTR] =
        [this](MessageParcel &data, MessageParcel &reply) { return OnGetProxyObjectPtr(data, reply); };
}

CallManagerServiceStub::~CallManagerServiceStub()
{
    memberFuncMap_.clear();
}

void CallManagerServiceStub::InitCallBasicRequest()
{
    memberFuncMap_[static_cast<int32_t>(CallManagerInterfaceCode::INTERFACE_REGISTER_CALLBACK)] =
        [this](MessageParcel &data, MessageParcel &reply) { return OnRegisterCallBack(data, reply); };
    memberFuncMap_[static_cast<int32_t>(CallManagerInterfaceCode::INTERFACE_UNREGISTER_CALLBACK)] =
        [this](MessageParcel &data, MessageParcel &reply) { return OnUnRegisterCallBack(data, reply); };
    memberFuncMap_[static_cast<int32_t>(CallManagerInterfaceCode::INTERFACE_DIAL_CALL)] =
        [this](MessageParcel &data, MessageParcel &reply) { return OnDialCall(data, reply); };
    memberFuncMap_[static_cast<int32_t>(CallManagerInterfaceCode::INTERFACE_MAKE_CALL)] =
        [this](MessageParcel &data, MessageParcel &reply) { return OnMakeCall(data, reply); };
    memberFuncMap_[static_cast<int32_t>(CallManagerInterfaceCode::INTERFACE_ANSWER_CALL)] =
        [this](MessageParcel &data, MessageParcel &reply) { return OnAcceptCall(data, reply); };
    memberFuncMap_[static_cast<int32_t>(CallManagerInterfaceCode::INTERFACE_REJECT_CALL)] =
        [this](MessageParcel &data, MessageParcel &reply) { return OnRejectCall(data, reply); };
    memberFuncMap_[static_cast<int32_t>(CallManagerInterfaceCode::INTERFACE_HOLD_CALL)] =
        [this](MessageParcel &data, MessageParcel &reply) { return OnHoldCall(data, reply); };
    memberFuncMap_[static_cast<int32_t>(CallManagerInterfaceCode::INTERFACE_UNHOLD_CALL)] =
        [this](MessageParcel &data, MessageParcel &reply) { return OnUnHoldCall(data, reply); };
    memberFuncMap_[static_cast<int32_t>(CallManagerInterfaceCode::INTERFACE_DISCONNECT_CALL)] =
        [this](MessageParcel &data, MessageParcel &reply) { return OnHangUpCall(data, reply); };
    memberFuncMap_[static_cast<int32_t>(CallManagerInterfaceCode::INTERFACE_GET_CALL_STATE)] =
        [this](MessageParcel &data, MessageParcel &reply) { return OnGetCallState(data, reply); };
    memberFuncMap_[static_cast<int32_t>(CallManagerInterfaceCode::INTERFACE_SWAP_CALL)] =
        [this](MessageParcel &data, MessageParcel &reply) { return OnSwitchCall(data, reply); };
    memberFuncMap_[static_cast<int32_t>(CallManagerInterfaceCode::INTERFACE_INPUT_DIALER_SPECIAL_CODE)] =
        [this](MessageParcel &data, MessageParcel &reply) { return OnInputDialerSpecialCode(data, reply); };
    memberFuncMap_[static_cast<int32_t>(CallManagerInterfaceCode::INTERFACE_SEND_CALLUI_EVENT)] =
        [this](MessageParcel &data, MessageParcel &reply) { return OnSendCallUiEvent(data, reply); };
}

void CallManagerServiceStub::InitCallUtilsRequest()
{
    memberFuncMap_[static_cast<int32_t>(CallManagerInterfaceCode::INTERFACE_HAS_CALL)] =
        [this](MessageParcel &data, MessageParcel &reply) { return OnHasCall(data, reply); };
    memberFuncMap_[static_cast<int32_t>(CallManagerInterfaceCode::INTERFACE_IS_NEW_CALL_ALLOWED)] =
        [this](MessageParcel &data, MessageParcel &reply) { return OnIsNewCallAllowed(data, reply); };
    memberFuncMap_[static_cast<int32_t>(CallManagerInterfaceCode::INTERFACE_IS_RINGING)] =
        [this](MessageParcel &data, MessageParcel &reply) { return OnIsRinging(data, reply); };
    memberFuncMap_[static_cast<int32_t>(CallManagerInterfaceCode::INTERFACE_IS_EMERGENCY_CALL)] =
        [this](MessageParcel &data, MessageParcel &reply) { return OnIsInEmergencyCall(data, reply); };
    memberFuncMap_[static_cast<int32_t>(CallManagerInterfaceCode::INTERFACE_IS_EMERGENCY_NUMBER)] =
        [this](MessageParcel &data, MessageParcel &reply) { return OnIsEmergencyPhoneNumber(data, reply); };
    memberFuncMap_[static_cast<int32_t>(CallManagerInterfaceCode::INTERFACE_IS_FORMAT_NUMBER)] =
        [this](MessageParcel &data, MessageParcel &reply) { return OnFormatPhoneNumber(data, reply); };
    memberFuncMap_[static_cast<int32_t>(CallManagerInterfaceCode::INTERFACE_IS_FORMAT_NUMBER_E164)] =
        [this](MessageParcel &data, MessageParcel &reply) { return OnFormatPhoneNumberToE164(data, reply); };
    memberFuncMap_[static_cast<int32_t>(CallManagerInterfaceCode::INTERFACE_CANCEL_MISSED_INCOMING_CALL_NOTIFICATION)] =
        [this](
            MessageParcel &data, MessageParcel &reply) { return OnRemoveMissedIncomingCallNotification(data, reply); };
    memberFuncMap_[static_cast<int32_t>(CallManagerInterfaceCode::INTERFACE_OBSERVER_ON_CALL_DETAILS_CHANGE)] =
        [this](MessageParcel &data, MessageParcel &reply) { return OnObserverOnCallDetailsChange(data, reply); };
    memberFuncMap_[static_cast<int32_t>(CallManagerInterfaceCode::INTERFACE_SET_CALL_POLICY_INFO)] =
        [this](MessageParcel &data, MessageParcel &reply) { return OnSetCallPolicyInfo(data, reply); };
}

void CallManagerServiceStub::InitCallConferenceRequest()
{
    memberFuncMap_[static_cast<int32_t>(CallManagerInterfaceCode::INTERFACE_COMBINE_CONFERENCE)] =
        [this](MessageParcel &data, MessageParcel &reply) { return OnCombineConference(data, reply); };
    memberFuncMap_[static_cast<int32_t>(CallManagerInterfaceCode::INTERFACE_SEPARATE_CONFERENCE)] =
        [this](MessageParcel &data, MessageParcel &reply) { return OnSeparateConference(data, reply); };
    memberFuncMap_[static_cast<int32_t>(CallManagerInterfaceCode::INTERFACE_JOIN_CONFERENCE)] =
        [this](MessageParcel &data, MessageParcel &reply) { return OnJoinConference(data, reply); };
    memberFuncMap_[static_cast<int32_t>(CallManagerInterfaceCode::INTERFACE_KICK_OUT_CONFERENCE)] =
        [this](MessageParcel &data, MessageParcel &reply) { return OnKickOutFromConference(data, reply); };
}

void CallManagerServiceStub::InitCallDtmfRequest()
{
    memberFuncMap_[static_cast<int32_t>(CallManagerInterfaceCode::INTERFACE_START_DTMF)] =
        [this](MessageParcel &data, MessageParcel &reply) { return OnStartDtmf(data, reply); };
    memberFuncMap_[static_cast<int32_t>(CallManagerInterfaceCode::INTERFACE_STOP_DTMF)] =
        [this](MessageParcel &data, MessageParcel &reply) { return OnStopDtmf(data, reply); };
    memberFuncMap_[static_cast<int32_t>(CallManagerInterfaceCode::INTERFACE_POST_DIAL_PROCEED)] =
        [this](MessageParcel &data, MessageParcel &reply) { return OnPostDialProceed(data, reply); };
}

void CallManagerServiceStub::InitCallSupplementRequest()
{
    memberFuncMap_[static_cast<int32_t>(CallManagerInterfaceCode::INTERFACE_GET_CALL_WAITING)] =
        [this](MessageParcel &data, MessageParcel &reply) { return OnGetCallWaiting(data, reply); };
    memberFuncMap_[static_cast<int32_t>(CallManagerInterfaceCode::INTERFACE_SET_CALL_WAITING)] =
        [this](MessageParcel &data, MessageParcel &reply) { return OnSetCallWaiting(data, reply); };
    memberFuncMap_[static_cast<int32_t>(CallManagerInterfaceCode::INTERFACE_GET_CALL_RESTRICTION)] =
        [this](MessageParcel &data, MessageParcel &reply) { return OnGetCallRestriction(data, reply); };
    memberFuncMap_[static_cast<int32_t>(CallManagerInterfaceCode::INTERFACE_SET_CALL_RESTRICTION)] =
        [this](MessageParcel &data, MessageParcel &reply) { return OnSetCallRestriction(data, reply); };
    memberFuncMap_[static_cast<int32_t>(CallManagerInterfaceCode::INTERFACE_SET_CALL_RESTRICTION_PASSWORD)] =
        [this](MessageParcel &data, MessageParcel &reply) { return OnSetCallRestrictionPassword(data, reply); };
    memberFuncMap_[static_cast<int32_t>(CallManagerInterfaceCode::INTERFACE_GET_CALL_TRANSFER)] =
        [this](MessageParcel &data, MessageParcel &reply) { return OnGetTransferNumber(data, reply); };
    memberFuncMap_[static_cast<int32_t>(CallManagerInterfaceCode::INTERFACE_SET_CALL_TRANSFER)] =
        [this](MessageParcel &data, MessageParcel &reply) { return OnSetTransferNumber(data, reply); };
    memberFuncMap_[static_cast<int32_t>(CallManagerInterfaceCode::INTERFACE_CAN_SET_CALL_TRANSFER_TIME)] =
        [this](MessageParcel &data, MessageParcel &reply) { return OnCanSetCallTransferTime(data, reply); };
    memberFuncMap_[static_cast<int32_t>(CallManagerInterfaceCode::INTERFACE_CLOSE_UNFINISHED_USSD)] =
        [this](MessageParcel &data, MessageParcel &reply) { return OnCloseUnFinishedUssd(data, reply); };
    memberFuncMap_[static_cast<int32_t>(CallManagerInterfaceCode::INTERFACE_SEND_USSD_RESPONSE)] =
        [this](MessageParcel &data, MessageParcel &reply) { return OnSendUssdResponse(data, reply); };
}

void CallManagerServiceStub::initCallConferenceExRequest()
{
    memberFuncMap_[static_cast<int32_t>(CallManagerInterfaceCode::INTERFACE_GET_MAINID)] =
        [this](MessageParcel &data, MessageParcel &reply) { return OnGetMainCallId(data, reply); };
    memberFuncMap_[static_cast<int32_t>(CallManagerInterfaceCode::INTERFACE_GET_SUBCALL_LIST_ID)] =
        [this](MessageParcel &data, MessageParcel &reply) { return OnGetSubCallIdList(data, reply); };
    memberFuncMap_[static_cast<int32_t>(CallManagerInterfaceCode::INTERFACE_GET_CALL_LIST_ID_FOR_CONFERENCE)] =
        [this](MessageParcel &data, MessageParcel &reply) { return OnGetCallIdListForConference(data, reply); };
}

void CallManagerServiceStub::InitCallMultimediaRequest()
{
    memberFuncMap_[static_cast<int32_t>(CallManagerInterfaceCode::INTERFACE_SET_MUTE)] =
        [this](MessageParcel &data, MessageParcel &reply) { return OnSetMute(data, reply); };
    memberFuncMap_[static_cast<int32_t>(CallManagerInterfaceCode::INTERFACE_MUTE_RINGER)] =
        [this](MessageParcel &data, MessageParcel &reply) { return OnMuteRinger(data, reply); };
    memberFuncMap_[static_cast<int32_t>(CallManagerInterfaceCode::INTERFACE_SET_AUDIO_DEVICE)] =
        [this](MessageParcel &data, MessageParcel &reply) { return OnSetAudioDevice(data, reply); };
    memberFuncMap_[static_cast<int32_t>(CallManagerInterfaceCode::INTERFACE_CTRL_CAMERA)] =
        [this](MessageParcel &data, MessageParcel &reply) { return OnControlCamera(data, reply); };
    memberFuncMap_[static_cast<int32_t>(CallManagerInterfaceCode::INTERFACE_SET_PREVIEW_WINDOW)] =
        [this](MessageParcel &data, MessageParcel &reply) { return OnSetPreviewWindow(data, reply); };
    memberFuncMap_[static_cast<int32_t>(CallManagerInterfaceCode::INTERFACE_SET_DISPLAY_WINDOW)] =
        [this](MessageParcel &data, MessageParcel &reply) { return OnSetDisplayWindow(data, reply); };
    memberFuncMap_[static_cast<int32_t>(CallManagerInterfaceCode::INTERFACE_SET_CAMERA_ZOOM)] =
        [this](MessageParcel &data, MessageParcel &reply) { return OnSetCameraZoom(data, reply); };
    memberFuncMap_[static_cast<int32_t>(CallManagerInterfaceCode::INTERFACE_SET_PAUSE_IMAGE)] =
        [this](MessageParcel &data, MessageParcel &reply) { return OnSetPausePicture(data, reply); };
    memberFuncMap_[static_cast<int32_t>(CallManagerInterfaceCode::INTERFACE_SET_DEVICE_DIRECTION)] =
        [this](MessageParcel &data, MessageParcel &reply) { return OnSetDeviceDirection(data, reply); };
    memberFuncMap_[static_cast<int32_t>(CallManagerInterfaceCode::INTERFACE_UPDATE_CALL_MEDIA_MODE)] =
        [this](MessageParcel &data, MessageParcel &reply) { return OnUpdateCallMediaMode(data, reply); };
    memberFuncMap_[static_cast<int32_t>(CallManagerInterfaceCode::INTERFACE_REPORT_AUDIO_DEVICE_INFO)] =
        [this](MessageParcel &data, MessageParcel &reply) { return OnReportAudioDeviceInfo(data, reply); };
    memberFuncMap_[static_cast<int32_t>(CallManagerInterfaceCode::INTERFACE_CANCEL_CALL_UPGRADE)] =
        [this](MessageParcel &data, MessageParcel &reply) { return OnCancelCallUpgrade(data, reply); };
    memberFuncMap_[static_cast<int32_t>(CallManagerInterfaceCode::INTERFACE_REQUEST_CAMERA_CAPABILITIES)] =
        [this](MessageParcel &data, MessageParcel &reply) { return OnRequestCameraCapabilities(data, reply); };
}

void CallManagerServiceStub::InitImsServiceRequest()
{
    memberFuncMap_[static_cast<int32_t>(CallManagerInterfaceCode::INTERFACE_SETCALL_PREFERENCEMODE)] =
        [this](MessageParcel &data, MessageParcel &reply) { return OnSetCallPreferenceMode(data, reply); };
    memberFuncMap_[static_cast<int32_t>(CallManagerInterfaceCode::INTERFACE_GET_IMS_CONFIG)] =
        [this](MessageParcel &data, MessageParcel &reply) { return OnGetImsConfig(data, reply); };
    memberFuncMap_[static_cast<int32_t>(CallManagerInterfaceCode::INTERFACE_SET_IMS_CONFIG)] =
        [this](MessageParcel &data, MessageParcel &reply) { return OnSetImsConfig(data, reply); };
    memberFuncMap_[static_cast<int32_t>(CallManagerInterfaceCode::INTERFACE_GET_IMS_FEATURE_VALUE)] =
        [this](MessageParcel &data, MessageParcel &reply) { return OnGetImsFeatureValue(data, reply); };
    memberFuncMap_[static_cast<int32_t>(CallManagerInterfaceCode::INTERFACE_SET_IMS_FEATURE_VALUE)] =
        [this](MessageParcel &data, MessageParcel &reply) { return OnSetImsFeatureValue(data, reply); };
    memberFuncMap_[static_cast<int32_t>(CallManagerInterfaceCode::INTERFACE_ENABLE_VOLTE)] =
        [this](MessageParcel &data, MessageParcel &reply) { return OnEnableVoLte(data, reply); };
    memberFuncMap_[static_cast<int32_t>(CallManagerInterfaceCode::INTERFACE_DISABLE_VOLTE)] =
        [this](MessageParcel &data, MessageParcel &reply) { return OnDisableVoLte(data, reply); };
    memberFuncMap_[static_cast<int32_t>(CallManagerInterfaceCode::INTERFACE_IS_VOLTE_ENABLED)] =
        [this](MessageParcel &data, MessageParcel &reply) { return OnIsVoLteEnabled(data, reply); };
    memberFuncMap_[static_cast<int32_t>(CallManagerInterfaceCode::INTERFACE_SET_VONR_STATE)] =
        [this](MessageParcel &data, MessageParcel &reply) { return OnSetVoNRState(data, reply); };
    memberFuncMap_[static_cast<int32_t>(CallManagerInterfaceCode::INTERFACE_GET_VONR_STATE)] =
        [this](MessageParcel &data, MessageParcel &reply) { return OnGetVoNRState(data, reply); };
    memberFuncMap_[static_cast<int32_t>(CallManagerInterfaceCode::INTERFACE_START_RTT)] =
        [this](MessageParcel &data, MessageParcel &reply) { return OnStartRtt(data, reply); };
    memberFuncMap_[static_cast<int32_t>(CallManagerInterfaceCode::INTERFACE_STOP_RTT)] =
        [this](MessageParcel &data, MessageParcel &reply) { return OnStopRtt(data, reply); };
    memberFuncMap_[static_cast<int32_t>(CallManagerInterfaceCode::INTERFACE_SET_VOIP_CALL_STATE)] =
        [this](MessageParcel &data, MessageParcel &reply) { return OnSetVoIPCallState(data, reply); };
    memberFuncMap_[static_cast<int32_t>(CallManagerInterfaceCode::INTERFACE_GET_VOIP_CALL_STATE)] =
        [this](MessageParcel &data, MessageParcel &reply) { return OnGetVoIPCallState(data, reply); };
    memberFuncMap_[static_cast<int32_t>(CallManagerInterfaceCode::INTERFACE_SET_VOIP_CALL_INFO)] =
        [this](MessageParcel &data, MessageParcel &reply) { return OnSetVoIPCallInfo(data, reply); };
    memberFuncMap_[static_cast<int32_t>(CallManagerInterfaceCode::INTERFACE_GET_VOIP_CALL_INFO)] =
        [this](MessageParcel &data, MessageParcel &reply) { return OnGetVoIPCallInfo(data, reply); };
}

void CallManagerServiceStub::InitOttServiceRequest()
{
    memberFuncMap_[static_cast<int32_t>(CallManagerInterfaceCode::INTERFACE_REPORT_OTT_CALL_DETAIL_INFO)] =
        [this](MessageParcel &data, MessageParcel &reply) { return OnReportOttCallDetailsInfo(data, reply); };
    memberFuncMap_[static_cast<int32_t>(CallManagerInterfaceCode::INTERFACE_REPORT_OTT_CALL_EVENT_INFO)] =
        [this](MessageParcel &data, MessageParcel &reply) { return OnReportOttCallEventInfo(data, reply); };
}

void CallManagerServiceStub::InitVoipOperationRequest()
{
    memberFuncMap_[static_cast<int32_t>(CallManagerInterfaceCode::INTERFACE_VOIP_REGISTER_CALLBACK)] =
        [this](MessageParcel &data, MessageParcel &reply) { return OnRegisterVoipCallManagerCallback(data, reply); };
    memberFuncMap_[static_cast<int32_t>(CallManagerInterfaceCode::INTERFACE_VOIP_UNREGISTER_CALLBACK)] =
        [this](MessageParcel &data, MessageParcel &reply) { return OnUnRegisterVoipCallManagerCallback(data, reply); };
}

void CallManagerServiceStub::InitBluetoothOperationRequest()
{
    memberFuncMap_[static_cast<int32_t>(CallManagerInterfaceCode::INTERFACE_BLUETOOTH_REGISTER_CALLBACKPTR)] =
        [this](MessageParcel &data, MessageParcel &reply) {
            return OnRegisterBluetoothCallManagerCallbackPtr(data, reply);
        };
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
            int32_t idTimer = SetTimer(code);
            int32_t result = memberFunc(data, reply);
            CancelTimer(idTimer);
            return result;
        }
    }
    return IPCObjectStub::OnRemoteRequest(code, data, reply, option);
}

int32_t CallManagerServiceStub::OnRegisterCallBack(MessageParcel &data, MessageParcel &reply)
{
    TELEPHONY_LOGI("OnRegisterCallBack entry");
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
    dialInfo.PutStringValue("extraParams", data.ReadString());
    dialInfo.PutStringValue("bundleName", data.ReadString());
    if (callNumber.length() > ACCOUNT_NUMBER_MAX_LENGTH) {
        TELEPHONY_LOGE("the account number length exceeds the limit");
        return CALL_ERR_NUMBER_OUT_OF_RANGE;
    }
    if (TELEPHONY_CUST_WRAPPER.isChangeDialNumberToTwEmc_ != nullptr
        && TELEPHONY_CUST_WRAPPER.isChangeDialNumberToTwEmc_(callNumber, dialInfo.GetIntValue("accountId"))) {
        TELEPHONY_LOGI("changed dial num to tw emc");
    }
    result = DialCall(callNumber, dialInfo);
    TELEPHONY_LOGI("result:%{public}d", result);
    if (!reply.WriteInt32(result)) {
        TELEPHONY_LOGE("fail to write parcel");
        return TELEPHONY_ERR_WRITE_REPLY_FAIL;
    }
    return result;
}

int32_t CallManagerServiceStub::OnMakeCall(MessageParcel &data, MessageParcel &reply)
{
    int32_t result = TELEPHONY_ERR_FAIL;
    AppExecFwk::PacMap dialInfo;
    std::string callNumber = data.ReadString();
    if (callNumber.length() > ACCOUNT_NUMBER_MAX_LENGTH) {
        TELEPHONY_LOGE("the account number length exceeds the limit");
        return CALL_ERR_NUMBER_OUT_OF_RANGE;
    }
    result = MakeCall(callNumber);
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
    TELEPHONY_LOGD("result:%{public}d", result);
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
    bool isInCludeVoipCall = data.ReadBool();
    bool result = HasCall(isInCludeVoipCall);
    TELEPHONY_LOGD("result:%{public}d", result);
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
    TELEPHONY_LOGD("result:%{public}d", result);
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

int32_t CallManagerServiceStub::OnSetVoIPCallInfo(MessageParcel &data, MessageParcel &reply)
{
    int32_t result = TELEPHONY_ERR_FAIL;
    int32_t callId = data.ReadInt32();
    int32_t state = data.ReadInt32();
    std::string phoneNumber = data.ReadString();
    result = SetVoIPCallInfo(callId, state, phoneNumber);
    if (!reply.WriteInt32(result)) {
        TELEPHONY_LOGE("SetVoIPCallInfo fail to write parcel");
        return TELEPHONY_ERR_WRITE_REPLY_FAIL;
    }
    return TELEPHONY_SUCCESS;
}

int32_t CallManagerServiceStub::OnGetVoIPCallInfo(MessageParcel &data, MessageParcel &reply)
{
    int32_t result = TELEPHONY_ERR_FAIL;
    int32_t callId;
    int32_t state;
    std::string phoneNumber;
    result = GetVoIPCallInfo(callId, state, phoneNumber);
    if (!reply.WriteInt32(callId)) {
        TELEPHONY_LOGE("GetVoIPCallInfo fail to write parcel");
        return TELEPHONY_ERR_WRITE_REPLY_FAIL;
    }
    if (!reply.WriteInt32(state)) {
        TELEPHONY_LOGE("GetVoIPCallInfo fail to write parcel");
        return TELEPHONY_ERR_WRITE_REPLY_FAIL;
    }
    if (!reply.WriteString(phoneNumber)) {
        TELEPHONY_LOGE("GetVoIPCallInfo fail to write parcel");
        return TELEPHONY_ERR_WRITE_REPLY_FAIL;
    }
    if (!reply.WriteInt32(result)) {
        TELEPHONY_LOGE("GetVoIPCallInfo fail to write parcel");
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

int32_t CallManagerServiceStub::OnSendCallUiEvent(MessageParcel &data, MessageParcel &reply)
{
    int32_t callId = data.ReadInt32();
    std::string eventName = data.ReadString();
    int32_t result = SendCallUiEvent(callId, eventName);
    TELEPHONY_LOGI("result:%{public}d", result);
    if (!reply.WriteInt32(result)) {
        TELEPHONY_LOGE("fail to write parcel");
        return TELEPHONY_ERR_WRITE_REPLY_FAIL;
    }
    return result;
}

int32_t CallManagerServiceStub::SetTimer(uint32_t code)
{
#ifdef HICOLLIE_ENABLE
    int32_t idTimer = HiviewDFX::INVALID_ID;
    std::map<uint32_t, std::string>::iterator itCollieId = collieCodeStringMap_.find(code);
    if (itCollieId != collieCodeStringMap_.end()) {
        std::string collieStr = itCollieId->second;
        std::string collieName = "CallManagerServiceStub: " + collieStr;
        unsigned int flag = HiviewDFX::XCOLLIE_FLAG_NOOP;
        auto TimerCallback = [collieStr](void *) {
            TELEPHONY_LOGE("OnRemoteRequest timeout func: %{public}s",
                collieStr.c_str());
        };
        idTimer = HiviewDFX::XCollie::GetInstance().SetTimer(
            collieName, XCOLLIE_TIMEOUT_SECONDS, TimerCallback, nullptr, flag);
        TELEPHONY_LOGD("SetTimer id: %{public}d, name: %{public}s.", idTimer, collieStr.c_str());
    }
    return idTimer;
#else
    TELEPHONY_LOGD("No HICOLLIE_ENABLE");
    return -1;
#endif
}

void CallManagerServiceStub::CancelTimer(int32_t id)
{
#ifdef HICOLLIE_ENABLE
    if (id == HiviewDFX::INVALID_ID) {
        return;
    }
    TELEPHONY_LOGD("CancelTimer id: %{public}d.", id);
    HiviewDFX::XCollie::GetInstance().CancelTimer(id);
#else
    return;
#endif
}

int32_t CallManagerServiceStub::OnRegisterBluetoothCallManagerCallbackPtr(MessageParcel &data, MessageParcel &reply)
{
    std::string macAddress = data.ReadString();
    sptr<ICallStatusCallback> callback = RegisterBluetoothCallManagerCallbackPtr(macAddress);
    if (callback == nullptr) {
        TELEPHONY_LOGE("OnRegisterBluetoothCallManagerCallbackPtr fail.");
        return TELEPHONY_ERR_FAIL;
    }
    sptr<IRemoteObject> objectPtr = callback->AsObject().GetRefPtr();
    if (objectPtr == nullptr) {
        TELEPHONY_LOGE("RegisterBluetoothCallManagerCallbackPtr faild!");
    }
    if (!reply.WriteRemoteObject(objectPtr)) {
        TELEPHONY_LOGE("OnRegisterBluetoothCallManagerCallbackPtr fail to write parcel");
        return TELEPHONY_ERR_WRITE_REPLY_FAIL;
    }
    return TELEPHONY_SUCCESS;
}

int32_t CallManagerServiceStub::OnSendUssdResponse(MessageParcel &data, MessageParcel &reply)
{
    int32_t result = TELEPHONY_ERR_FAIL;
    int32_t slotId = data.ReadInt32();
    std::string content = data.ReadString();
    result = SendUssdResponse(slotId, content);
    TELEPHONY_LOGI("result:%{public}d", result);
    if (!reply.WriteInt32(result)) {
        TELEPHONY_LOGE("fail to write parcel");
        return TELEPHONY_ERR_WRITE_REPLY_FAIL;
    }
    return result;
}

int32_t CallManagerServiceStub::OnSetCallPolicyInfo(MessageParcel &data, MessageParcel &reply)
{
    int32_t result = TELEPHONY_ERR_FAIL;
    int32_t dialingPolicy = data.ReadInt32();
    std::vector<std::string> dialingList;
    data.ReadStringVector(&dialingList);
    int32_t incomingPolicy = data.ReadInt32();
    std::vector<std::string> incomingList;
    data.ReadStringVector(&incomingList);

    result = SetCallPolicyInfo(dialingPolicy, dialingList, incomingPolicy, incomingList);
    if (!reply.WriteInt32(result)) {
        TELEPHONY_LOGE("SetCallPolicyInfo fail to write parcel");
        return TELEPHONY_ERR_WRITE_REPLY_FAIL;
    }
    return TELEPHONY_SUCCESS;
}
} // namespace Telephony
} // namespace OHOS
