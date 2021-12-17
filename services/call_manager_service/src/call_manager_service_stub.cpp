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

#include "call_manager_service_stub.h"

#include <string_ex.h>

#include "call_manager_errors.h"
#include "telephony_log_wrapper.h"

#include "message_option.h"
#include "message_parcel.h"

#include "call_control_manager.h"
#include "cellular_call_types.h"

namespace OHOS {
namespace Telephony {
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
}

CallManagerServiceStub::~CallManagerServiceStub()
{
    memberFuncMap_.clear();
}

void CallManagerServiceStub::InitCallBasicRequest()
{
    memberFuncMap_[(uint32_t)CallManagerSurfaceCode::INTERFACE_REGISTER_CALLBACK] =
        &CallManagerServiceStub::OnRegisterCallBack;
    memberFuncMap_[(uint32_t)CallManagerSurfaceCode::INTERFACE_UNREGISTER_CALLBACK] =
        &CallManagerServiceStub::OnUnRegisterCallBack;
    memberFuncMap_[(uint32_t)CallManagerSurfaceCode::INTERFACE_DIAL_CALL] = &CallManagerServiceStub::OnDialCall;
    memberFuncMap_[(uint32_t)CallManagerSurfaceCode::INTERFACE_ANSWER_CALL] = &CallManagerServiceStub::OnAcceptCall;
    memberFuncMap_[(uint32_t)CallManagerSurfaceCode::INTERFACE_REJECT_CALL] = &CallManagerServiceStub::OnRejectCall;
    memberFuncMap_[(uint32_t)CallManagerSurfaceCode::INTERFACE_HOLD_CALL] = &CallManagerServiceStub::OnHoldCall;
    memberFuncMap_[(uint32_t)CallManagerSurfaceCode::INTERFACE_UNHOLD_CALL] = &CallManagerServiceStub::OnUnHoldCall;
    memberFuncMap_[(uint32_t)CallManagerSurfaceCode::INTERFACE_DISCONNECT_CALL] =
        &CallManagerServiceStub::OnHangUpCall;
    memberFuncMap_[(uint32_t)CallManagerSurfaceCode::INTERFACE_GET_CALL_STATE] =
        &CallManagerServiceStub::OnGetCallState;
    memberFuncMap_[(uint32_t)CallManagerSurfaceCode::INTERFACE_SWAP_CALL] = &CallManagerServiceStub::OnSwitchCall;
}

void CallManagerServiceStub::InitCallUtilsRequest()
{
    memberFuncMap_[(uint32_t)CallManagerSurfaceCode::INTERFACE_HAS_CALL] = &CallManagerServiceStub::OnHasCall;
    memberFuncMap_[(uint32_t)CallManagerSurfaceCode::INTERFACE_IS_NEW_CALL_ALLOWED] =
        &CallManagerServiceStub::OnIsNewCallAllowed;
    memberFuncMap_[(uint32_t)CallManagerSurfaceCode::INTERFACE_IS_RINGING] = &CallManagerServiceStub::OnIsRinging;
    memberFuncMap_[(uint32_t)CallManagerSurfaceCode::INTERFACE_IS_EMERGENCY_CALL] =
        &CallManagerServiceStub::OnIsInEmergencyCall;
    memberFuncMap_[(uint32_t)CallManagerSurfaceCode::INTERFACE_IS_EMERGENCY_NUMBER] =
        &CallManagerServiceStub::OnIsEmergencyPhoneNumber;
    memberFuncMap_[(uint32_t)CallManagerSurfaceCode::INTERFACE_IS_FORMAT_NUMBER] =
        &CallManagerServiceStub::OnFormatPhoneNumber;
    memberFuncMap_[(uint32_t)CallManagerSurfaceCode::INTERFACE_IS_FORMAT_NUMBER_E164] =
        &CallManagerServiceStub::OnFormatPhoneNumberToE164;
}

void CallManagerServiceStub::InitCallConferenceRequest()
{
    memberFuncMap_[(uint32_t)CallManagerSurfaceCode::INTERFACE_COMBINE_CONFERENCE] =
        &CallManagerServiceStub::OnJoinConference;
    memberFuncMap_[(uint32_t)CallManagerSurfaceCode::INTERFACE_SEPARATE_CONFERENCE] =
        &CallManagerServiceStub::OnSeparateConference;
}

void CallManagerServiceStub::InitCallDtmfRequest()
{
    memberFuncMap_[(uint32_t)CallManagerSurfaceCode::INTERFACE_START_DTMF] = &CallManagerServiceStub::OnStartDtmf;
    memberFuncMap_[(uint32_t)CallManagerSurfaceCode::INTERFACE_SEND_DTMF] = &CallManagerServiceStub::OnSendDtmf;
    memberFuncMap_[(uint32_t)CallManagerSurfaceCode::INTERFACE_STOP_DTMF] = &CallManagerServiceStub::OnStopDtmf;
    memberFuncMap_[(uint32_t)CallManagerSurfaceCode::INTERFACE_SEND_DTMF_BUNCH] =
        &CallManagerServiceStub::OnSendDtmfBunch;
}

void CallManagerServiceStub::InitCallSupplementRequest()
{
    memberFuncMap_[(uint32_t)CallManagerSurfaceCode::INTERFACE_GET_CALL_WAITING] =
        &CallManagerServiceStub::OnGetCallWaiting;
    memberFuncMap_[(uint32_t)CallManagerSurfaceCode::INTERFACE_SET_CALL_WAITING] =
        &CallManagerServiceStub::OnSetCallWaiting;
    memberFuncMap_[(uint32_t)CallManagerSurfaceCode::INTERFACE_GET_CALL_RESTRICTION] =
        &CallManagerServiceStub::OnGetCallRestriction;
    memberFuncMap_[(uint32_t)CallManagerSurfaceCode::INTERFACE_SET_CALL_RESTRICTION] =
        &CallManagerServiceStub::OnSetCallRestriction;
    memberFuncMap_[(uint32_t)CallManagerSurfaceCode::INTERFACE_GET_CALL_TRANSFER] =
        &CallManagerServiceStub::OnGetTransferNumber;
    memberFuncMap_[(uint32_t)CallManagerSurfaceCode::INTERFACE_SET_CALL_TRANSFER] =
        &CallManagerServiceStub::OnSetTransferNumber;
}

void CallManagerServiceStub::initCallConferenceExRequest()
{
    memberFuncMap_[(uint32_t)CallManagerSurfaceCode::INTERFACE_GET_MAINID] =
        &CallManagerServiceStub::OnGetMainCallId;
    memberFuncMap_[(uint32_t)CallManagerSurfaceCode::INTERFACE_GET_SUBCALL_LIST_ID] =
        &CallManagerServiceStub::OnGetSubCallIdList;
    memberFuncMap_[(uint32_t)CallManagerSurfaceCode::INTERFACE_GET_CALL_LIST_ID_FOR_CONFERENCE] =
        &CallManagerServiceStub::OnGetCallIdListForConference;
}

void CallManagerServiceStub::InitCallMultimediaRequest()
{
    memberFuncMap_[(uint32_t)CallManagerSurfaceCode::INTERFACE_SET_MUTE] = &CallManagerServiceStub::OnSetMute;
    memberFuncMap_[(uint32_t)CallManagerSurfaceCode::INTERFACE_MUTE_RINGER] = &CallManagerServiceStub::OnMuteRinger;
    memberFuncMap_[(uint32_t)CallManagerSurfaceCode::INTERFACE_SET_AUDIO_DEVICE] =
        &CallManagerServiceStub::OnSetAudioDevice;
    memberFuncMap_[(uint32_t)CallManagerSurfaceCode::INTERFACE_CANCEL_MISSED_CALLS_NOTIFICATION] =
        &CallManagerServiceStub::OnCancelMissedCallsNotification;
    memberFuncMap_[(uint32_t)CallManagerSurfaceCode::INTERFACE_CTRL_CAMERA] =
        &CallManagerServiceStub::OnControlCamera;
    memberFuncMap_[(uint32_t)CallManagerSurfaceCode::INTERFACE_SET_PREVIEW_WINDOW] =
        &CallManagerServiceStub::OnSetPreviewWindow;
    memberFuncMap_[(uint32_t)CallManagerSurfaceCode::INTERFACE_SET_DISPLAY_WINDOW] =
        &CallManagerServiceStub::OnSetDisplayWindow;
    memberFuncMap_[(uint32_t)CallManagerSurfaceCode::INTERFACE_SET_CAMERA_ZOOM] =
        &CallManagerServiceStub::OnSetCameraZoom;
    memberFuncMap_[(uint32_t)CallManagerSurfaceCode::INTERFACE_SET_PAUSE_PICTURE] =
        &CallManagerServiceStub::OnSetPausePicture;
    memberFuncMap_[(uint32_t)CallManagerSurfaceCode::INTERFACE_SET_DEVICE_DIRECTION] =
        &CallManagerServiceStub::OnSetDeviceDirection;
}

void CallManagerServiceStub::InitImsServiceRequest()
{
    memberFuncMap_[(uint32_t)CallManagerSurfaceCode::INTERFACE_SETCALL_PREFERENCEMODE] =
        &CallManagerServiceStub::OnSetCallPreferenceMode;
}

int CallManagerServiceStub::OnRemoteRequest(
    uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option)
{
    std::u16string myDescriptor = CallManagerServiceStub::GetDescriptor();
    std::u16string remoteDescriptor = data.ReadInterfaceToken();
    if (myDescriptor != remoteDescriptor) {
        TELEPHONY_LOGE("descriptor checked fail !");
        return TELEPHONY_ERR_DESCRIPTOR_MISMATCH;
    }
    TELEPHONY_LOGI("OnReceived, cmd = %{public}u", code);
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
    std::u16string bundleName = data.ReadString16();
    std::string strName = Str16ToStr8(bundleName);
    if (CheckBundleName(strName) != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("%{public}s no permission to register callback", strName.c_str());
        result = TELEPHONY_ERR_PERMISSION_ERR;
        reply.WriteInt32(result);
        return result;
    }
    sptr<ICallAbilityCallback> callback = iface_cast<ICallAbilityCallback>(remote);
    result = RegisterCallBack(callback, bundleName);
    reply.WriteInt32(result);
    return result;
}

int32_t CallManagerServiceStub::OnUnRegisterCallBack(MessageParcel &data, MessageParcel &reply)
{
    int32_t result = TELEPHONY_ERR_FAIL;
    std::u16string bundleName = data.ReadString16();
    result = UnRegisterCallBack(bundleName);
    if (!reply.WriteInt32(result)) {
        TELEPHONY_LOGE("fail to write parcel");
        return TELEPHONY_ERR_WRITE_REPLY_FAIL;
    }
    return result;
}

int32_t CallManagerServiceStub::OnDialCall(MessageParcel &data, MessageParcel &reply)
{
    int32_t result = TELEPHONY_ERR_FAIL;
    std::u16string callNumber = data.ReadString16();
    int32_t accountId = data.ReadInt32();
    int32_t videoState = data.ReadInt32();
    int32_t dialScene = data.ReadInt32();
    int32_t dialType = data.ReadInt32();
    int32_t callType = data.ReadInt32();
    AppExecFwk::PacMap dialInfo;
    dialInfo.PutIntValue("accountId", accountId);
    dialInfo.PutIntValue("videoState", videoState);
    dialInfo.PutIntValue("dialScene", dialScene);
    dialInfo.PutIntValue("dialType", dialType);
    dialInfo.PutIntValue("callType", callType);
    if (!data.ContainFileDescriptors()) {
        TELEPHONY_LOGW("sent raw data is less than 32k");
    }
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
    bool result = IsNewCallAllowed();
    TELEPHONY_LOGI("result:%{public}d", result);
    if (!reply.WriteBool(result)) {
        TELEPHONY_LOGE("fail to write parcel");
        return TELEPHONY_ERR_WRITE_REPLY_FAIL;
    }
    return TELEPHONY_SUCCESS;
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
    int32_t deviceType = data.ReadInt32();
    AudioDevice device = AudioDevice::DEVICE_UNKNOWN;
    switch (deviceType) {
        case (int32_t)AudioDevice::DEVICE_EARPIECE:
            device = AudioDevice::DEVICE_EARPIECE;
            break;
        case (int32_t)AudioDevice::DEVICE_SPEAKER:
            device = AudioDevice::DEVICE_SPEAKER;
            break;
        case (int32_t)AudioDevice::DEVICE_WIRED_HEADSET:
            device = AudioDevice::DEVICE_WIRED_HEADSET;
            break;
        case (int32_t)AudioDevice::DEVICE_BLUETOOTH_SCO:
            device = AudioDevice::DEVICE_BLUETOOTH_SCO;
            break;
        default:
            break;
    }
    int32_t result = SetAudioDevice(device);
    TELEPHONY_LOGI("result:%{public}d", result);
    if (!reply.WriteInt32(result)) {
        TELEPHONY_LOGE("fail to write parcel");
        return TELEPHONY_ERR_WRITE_REPLY_FAIL;
    }
    return TELEPHONY_SUCCESS;
}

int32_t CallManagerServiceStub::OnIsRinging(MessageParcel &data, MessageParcel &reply)
{
    bool result = IsRinging();
    TELEPHONY_LOGI("result:%{public}d", result);
    if (!reply.WriteBool(result)) {
        TELEPHONY_LOGE("fail to write parcel");
        return TELEPHONY_ERR_WRITE_REPLY_FAIL;
    }
    return TELEPHONY_SUCCESS;
}

int32_t CallManagerServiceStub::OnIsInEmergencyCall(MessageParcel &data, MessageParcel &reply)
{
    bool result = IsInEmergencyCall();
    TELEPHONY_LOGI("result:%{public}d", result);
    if (!reply.WriteBool(result)) {
        TELEPHONY_LOGE("fail to write parcel");
        return TELEPHONY_ERR_WRITE_REPLY_FAIL;
    }
    return TELEPHONY_SUCCESS;
}

int32_t CallManagerServiceStub::OnStartDtmf(MessageParcel &data, MessageParcel &reply)
{
    int32_t result = TELEPHONY_ERR_FAIL;
    int32_t callId = data.ReadInt32();
    char str = data.ReadInt8();
    result = StartDtmf(callId, str);
    TELEPHONY_LOGI("result:%{public}d", result);
    if (!reply.WriteInt32(result)) {
        TELEPHONY_LOGE("fail to write parcel");
        return TELEPHONY_ERR_WRITE_REPLY_FAIL;
    }
    return TELEPHONY_SUCCESS;
}

int32_t CallManagerServiceStub::OnSendDtmf(MessageParcel &data, MessageParcel &reply)
{
    int32_t result = TELEPHONY_ERR_FAIL;
    int32_t callId = data.ReadInt32();
    char str = data.ReadInt8();
    result = SendDtmf(callId, str);
    TELEPHONY_LOGI("result:%{public}d", result);
    if (!reply.WriteInt32(result)) {
        TELEPHONY_LOGE("fail to write parcel");
        return TELEPHONY_ERR_WRITE_REPLY_FAIL;
    }
    return TELEPHONY_SUCCESS;
}

int32_t CallManagerServiceStub::OnSendDtmfBunch(MessageParcel &data, MessageParcel &reply)
{
    int32_t result = TELEPHONY_ERR_FAIL;
    int32_t callId = data.ReadInt32();
    std::u16string str = data.ReadString16();
    int32_t on = data.ReadInt32();
    int32_t off = data.ReadInt32();
    result = SendBurstDtmf(callId, str, on, off);
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
    int32_t slotId = data.ReadInt32();
    CallRestrictionInfo *pCRInfo = (CallRestrictionInfo *)data.ReadRawData(sizeof(CallRestrictionInfo));
    result = SetCallRestriction(slotId, *pCRInfo);
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
    int32_t slotId = data.ReadInt32();
    CallTransferInfo *pCTInfo = (CallTransferInfo *)data.ReadRawData(sizeof(CallTransferInfo));
    result = SetCallTransferInfo(slotId, *pCTInfo);
    TELEPHONY_LOGI("result:%{public}d", result);
    if (!reply.WriteInt32(result)) {
        TELEPHONY_LOGE("fail to write parcel");
        return TELEPHONY_ERR_WRITE_REPLY_FAIL;
    }
    return TELEPHONY_SUCCESS;
}

int32_t CallManagerServiceStub::OnJoinConference(MessageParcel &data, MessageParcel &reply)
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

int32_t CallManagerServiceStub::OnIsEmergencyPhoneNumber(MessageParcel &data, MessageParcel &reply)
{
    std::u16string callNumber = data.ReadString16();
    int32_t slotId = data.ReadInt32();
    int32_t errorCode = TELEPHONY_ERROR;
    bool result = IsEmergencyPhoneNumber(callNumber, slotId, errorCode);
    TELEPHONY_LOGI("result:%{public}d", result);
    if (!reply.WriteBool(result) || !reply.WriteInt32(errorCode)) {
        TELEPHONY_LOGE("fail to write parcel");
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
    if (!reply.WriteString16(formatNumber) || !reply.WriteInt32(result)) {
        TELEPHONY_LOGE("fail to write parcel");
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
    TELEPHONY_LOGI("result:%{public}d", result);
    if (!reply.WriteString16(formatNumber) || !reply.WriteInt32(result)) {
        TELEPHONY_LOGE("fail to write parcel");
        return TELEPHONY_ERR_WRITE_REPLY_FAIL;
    }
    return TELEPHONY_SUCCESS;
}

int32_t CallManagerServiceStub::OnGetMainCallId(MessageParcel &data, MessageParcel &reply)
{
    int32_t callId = data.ReadInt32();
    int32_t result = GetMainCallId(callId);
    TELEPHONY_LOGI("result:%{public}d", result);
    if (!reply.WriteInt32(result)) {
        TELEPHONY_LOGE("fail to write parcel");
        return TELEPHONY_ERR_WRITE_REPLY_FAIL;
    }
    return TELEPHONY_SUCCESS;
}

int32_t CallManagerServiceStub::OnGetSubCallIdList(MessageParcel &data, MessageParcel &reply)
{
    int32_t callId = data.ReadInt32();
    std::vector<std::u16string> result = GetSubCallIdList(callId);
    if (!reply.WriteString16Vector(result)) {
        TELEPHONY_LOGE("fail to write parcel");
        return TELEPHONY_ERR_WRITE_REPLY_FAIL;
    }
    return TELEPHONY_SUCCESS;
}

int32_t CallManagerServiceStub::OnGetCallIdListForConference(MessageParcel &data, MessageParcel &reply)
{
    int32_t callId = data.ReadInt32();
    std::vector<std::u16string> result = GetCallIdListForConference(callId);
    if (!reply.WriteString16Vector(result)) {
        TELEPHONY_LOGE("fail to write parcel");
        return TELEPHONY_ERR_WRITE_REPLY_FAIL;
    }
    return TELEPHONY_SUCCESS;
}

int32_t CallManagerServiceStub::OnCancelMissedCallsNotification(MessageParcel &data, MessageParcel &reply)
{
    int32_t id = data.ReadInt32();
    int32_t result = CancelMissedCallsNotification(id);
    TELEPHONY_LOGI("result:%{public}d", result);
    if (!reply.WriteInt32(result)) {
        TELEPHONY_LOGE("fail to write parcel");
        return TELEPHONY_ERR_WRITE_REPLY_FAIL;
    }
    return result;
}
int32_t CallManagerServiceStub::OnControlCamera(MessageParcel &data, MessageParcel &reply)
{
    int32_t result = TELEPHONY_ERR_FAIL;
    std::u16string cameraId = data.ReadString16();
    std::u16string callingPackage = data.ReadString16();
    result = ControlCamera(cameraId, callingPackage);
    if (!reply.WriteInt32(result)) {
        TELEPHONY_LOGE("ControlCamera fail to write parcel");
        return TELEPHONY_ERR_WRITE_REPLY_FAIL;
    }
    return TELEPHONY_SUCCESS;
}

int32_t CallManagerServiceStub::OnSetPreviewWindow(MessageParcel &data, MessageParcel &reply)
{
    int32_t result = TELEPHONY_ERR_FAIL;
    VideoWindow *pSurface = (VideoWindow *)data.ReadRawData(sizeof(VideoWindow));
    result = SetPreviewWindow(*pSurface);
    if (!reply.WriteInt32(result)) {
        TELEPHONY_LOGE("SetPreviewWindow fail to write parcel");
        return TELEPHONY_ERR_WRITE_REPLY_FAIL;
    }
    return TELEPHONY_SUCCESS;
}

int32_t CallManagerServiceStub::OnSetDisplayWindow(MessageParcel &data, MessageParcel &reply)
{
    int32_t result = TELEPHONY_ERR_FAIL;
    VideoWindow *pSurface = (VideoWindow *)data.ReadRawData(sizeof(VideoWindow));
    result = SetDisplayWindow(*pSurface);
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
    if (!reply.WriteInt32(result)) {
        TELEPHONY_LOGE("SetCameraZoom fail to write parcel");
        return TELEPHONY_ERR_WRITE_REPLY_FAIL;
    }
    return TELEPHONY_SUCCESS;
}

int32_t CallManagerServiceStub::OnSetPausePicture(MessageParcel &data, MessageParcel &reply)
{
    int32_t result = TELEPHONY_ERR_FAIL;
    std::u16string path = data.ReadString16();
    result = SetPausePicture(path);
    if (!reply.WriteInt32(result)) {
        TELEPHONY_LOGE("SetPausePicture fail to write parcel");
        return TELEPHONY_ERR_WRITE_REPLY_FAIL;
    }
    return TELEPHONY_SUCCESS;
}

int32_t CallManagerServiceStub::OnSetDeviceDirection(MessageParcel &data, MessageParcel &reply)
{
    int32_t result = TELEPHONY_ERR_FAIL;
    int32_t rotation = data.ReadInt32();
    result = SetDeviceDirection(rotation);
    if (!reply.WriteInt32(result)) {
        TELEPHONY_LOGE("SetDeviceDirection fail to write parcel");
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

int32_t CallManagerServiceStub::CheckBundleName(std::string bundleName)
{
    std::string bundleNameList[] = {
        "com.ohos.callui",
        "com.ohos.callManagerTest",
        "com.ohos.callManagerGtest",
        "com.ohos.callmanagercallmedia",
        "com.ohos.callmanager",
        "com.ohos.callmanagerregister",
        "com.ohos.callmanagerreliabilityperformance",
        "com.ohos.callmanagerimscall",
        "com.ohos.callmanagercallcarmera",
        "com.ohos.callmanagerreliability",
        "com.ohos.callmanagerperformance",
        "com.example.callmanager",
        "com.example.telephone_demo",
        "com.ohos.calldemo",
    };
    for (int i = 0; i < end(bundleNameList) - begin(bundleNameList); i++) {
        if (strcmp(bundleName.c_str(), bundleNameList[i].c_str()) == 0) {
            return TELEPHONY_SUCCESS;
        }
    }
    return TELEPHONY_ERROR;
}
} // namespace Telephony
} // namespace OHOS
