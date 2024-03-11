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

#include "call_manager_service_proxy.h"

#include "call_manager_errors.h"
#include "message_option.h"
#include "message_parcel.h"

namespace OHOS {
namespace Telephony {
CallManagerServiceProxy::CallManagerServiceProxy(const sptr<IRemoteObject> &impl)
    : IRemoteProxy<ICallManagerService>(impl)
{}

int32_t CallManagerServiceProxy::RegisterCallBack(const sptr<ICallAbilityCallback> &callback)
{
    MessageParcel dataParcel;
    if (!dataParcel.WriteInterfaceToken(CallManagerServiceProxy::GetDescriptor())) {
        TELEPHONY_LOGE("write descriptor fail");
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    dataParcel.WriteRemoteObject(callback->AsObject().GetRefPtr());
    MessageParcel replyParcel;
    int32_t error = SendRequest(INTERFACE_REGISTER_CALLBACK, dataParcel, replyParcel);
    if (error != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("Function RegisterCallBack! errCode:%{public}d", error);
        return error;
    }
    return replyParcel.ReadInt32();
}

int32_t CallManagerServiceProxy::UnRegisterCallBack()
{
    return SendRequest(INTERFACE_UNREGISTER_CALLBACK);
}

int32_t CallManagerServiceProxy::ObserverOnCallDetailsChange()
{
    int32_t error = SendRequest(INTERFACE_OBSERVER_ON_CALL_DETAILS_CHANGE);
    if (error != ERR_NONE) {
        TELEPHONY_LOGE("function ObserverOnCallDetailsChange failed! errCode:%{public}d", error);
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    return TELEPHONY_SUCCESS;
}

int32_t CallManagerServiceProxy::DialCall(std::u16string number, AppExecFwk::PacMap &extras)
{
    MessageParcel dataParcel;
    if (!dataParcel.WriteInterfaceToken(CallManagerServiceProxy::GetDescriptor())) {
        TELEPHONY_LOGE("write descriptor fail");
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    if (number.empty()) {
        TELEPHONY_LOGE("number is empty");
        return TELEPHONY_ERR_ARGUMENT_INVALID;
    }
    dataParcel.WriteString16(number);
    dataParcel.WriteInt32(extras.GetIntValue("accountId"));
    dataParcel.WriteInt32(extras.GetIntValue("videoState"));
    dataParcel.WriteInt32(extras.GetIntValue("dialScene"));
    dataParcel.WriteInt32(extras.GetIntValue("dialType"));
    dataParcel.WriteInt32(extras.GetIntValue("callType"));
    MessageParcel replyParcel;
    int32_t error = SendRequest(INTERFACE_DIAL_CALL, dataParcel, replyParcel);
    if (error != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("function DialCall call failed! errCode:%{public}d", error);
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    return replyParcel.ReadInt32();
}

int32_t CallManagerServiceProxy::AnswerCall(int32_t callId, int32_t videoState)
{
    MessageParcel dataParcel;
    if (!dataParcel.WriteInterfaceToken(CallManagerServiceProxy::GetDescriptor())) {
        TELEPHONY_LOGE("write descriptor fail");
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    dataParcel.WriteInt32(callId);
    dataParcel.WriteInt32(videoState);
    MessageParcel replyParcel;
    int32_t error = SendRequest(INTERFACE_ANSWER_CALL, dataParcel, replyParcel);
    if (error != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("function AnswerCall call failed! errCode:%{public}d", error);
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    return replyParcel.ReadInt32();
}

int32_t CallManagerServiceProxy::RejectCall(int32_t callId, bool rejectWithMessage, std::u16string textMessage)
{
    MessageParcel dataParcel;
    if (!dataParcel.WriteInterfaceToken(CallManagerServiceProxy::GetDescriptor())) {
        TELEPHONY_LOGE("write descriptor fail");
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    dataParcel.WriteInt32(callId);
    dataParcel.WriteBool(rejectWithMessage);
    dataParcel.WriteString16(textMessage);
    MessageParcel replyParcel;
    int32_t error = SendRequest(INTERFACE_REJECT_CALL, dataParcel, replyParcel);
    if (error != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("function RejectCall call failed! errCode:%{public}d", error);
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    return replyParcel.ReadInt32();
}

int32_t CallManagerServiceProxy::HangUpCall(int32_t callId)
{
    MessageParcel dataParcel;
    if (!dataParcel.WriteInterfaceToken(CallManagerServiceProxy::GetDescriptor())) {
        TELEPHONY_LOGE("write descriptor fail");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    dataParcel.WriteInt32(callId);
    MessageParcel replyParcel;
    int32_t error = SendRequest(INTERFACE_DISCONNECT_CALL, dataParcel, replyParcel);
    if (error != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("function HangUpCall call failed! errCode:%{public}d", error);
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    return replyParcel.ReadInt32();
}

int32_t CallManagerServiceProxy::GetCallState()
{
    return SendRequest(INTERFACE_GET_CALL_STATE);
}

int32_t CallManagerServiceProxy::HoldCall(int32_t callId)
{
    MessageParcel dataParcel;
    if (!dataParcel.WriteInterfaceToken(CallManagerServiceProxy::GetDescriptor())) {
        TELEPHONY_LOGE("write descriptor fail");
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    dataParcel.WriteInt32(callId);
    MessageParcel replyParcel;
    int32_t error = SendRequest(INTERFACE_HOLD_CALL, dataParcel, replyParcel);
    if (error != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("Function HoldCall call failed! errCode:%{public}d", error);
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    return replyParcel.ReadInt32();
}

int32_t CallManagerServiceProxy::RegisterVoipCallManagerCallback()
{
    int32_t error = SendRequest(INTERFACE_VOIP_REGISTER_CALLBACK);
    if (error != ERR_NONE) {
        TELEPHONY_LOGE("function RegisterVoipCallManagerCallback failed! errCode:%{public}d", error);
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    return 0;
}

int32_t CallManagerServiceProxy::UnRegisterVoipCallManagerCallback()
{
    return SendRequest(INTERFACE_VOIP_UNREGISTER_CALLBACK);
}

int32_t CallManagerServiceProxy::UnHoldCall(int32_t callId)
{
    MessageParcel dataParcel;
    if (!dataParcel.WriteInterfaceToken(CallManagerServiceProxy::GetDescriptor())) {
        TELEPHONY_LOGE("write descriptor fail");
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    dataParcel.WriteInt32(callId);
    MessageParcel replyParcel;
    int32_t error = SendRequest(INTERFACE_UNHOLD_CALL, dataParcel, replyParcel);
    if (error != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("Function UnHoldCall call failed! errCode:%{public}d", error);
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    return replyParcel.ReadInt32();
}

int32_t CallManagerServiceProxy::SwitchCall(int32_t callId)
{
    MessageParcel dataParcel;
    if (!dataParcel.WriteInterfaceToken(CallManagerServiceProxy::GetDescriptor())) {
        TELEPHONY_LOGE("write descriptor fail");
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    dataParcel.WriteInt32(callId);
    MessageParcel replyParcel;
    int32_t error = SendRequest(INTERFACE_SWAP_CALL, dataParcel, replyParcel);
    if (error != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("Function SwitchCall call failed! errCode:%{public}d", error);
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    return replyParcel.ReadInt32();
}

bool CallManagerServiceProxy::HasCall()
{
    MessageParcel dataParcel;
    if (!dataParcel.WriteInterfaceToken(CallManagerServiceProxy::GetDescriptor())) {
        TELEPHONY_LOGE("write descriptor fail");
        return false;
    }
    MessageParcel replyParcel;
    int32_t error = SendRequest(INTERFACE_HAS_CALL, dataParcel, replyParcel);
    if (error != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("Function HasCall! errCode:%{public}d", error);
        return false;
    }
    return replyParcel.ReadBool();
}

int32_t CallManagerServiceProxy::IsNewCallAllowed(bool &enabled)
{
    MessageParcel dataParcel;
    if (!dataParcel.WriteInterfaceToken(CallManagerServiceProxy::GetDescriptor())) {
        TELEPHONY_LOGE("write descriptor fail");
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    MessageParcel replyParcel;
    int32_t error = SendRequest(INTERFACE_IS_NEW_CALL_ALLOWED, dataParcel, replyParcel);
    if (error != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("Function IsNewCallAllowed! errCode:%{public}d", error);
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    int32_t result = replyParcel.ReadInt32();
    if (result == TELEPHONY_ERR_SUCCESS) {
        enabled = replyParcel.ReadBool();
    }
    return result;
}

int32_t CallManagerServiceProxy::SetMuted(bool isMute)
{
    MessageParcel dataParcel;
    if (!dataParcel.WriteInterfaceToken(CallManagerServiceProxy::GetDescriptor())) {
        TELEPHONY_LOGE("write descriptor fail");
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    dataParcel.WriteBool(isMute);
    MessageParcel replyParcel;
    int32_t error = SendRequest(INTERFACE_SET_MUTE, dataParcel, replyParcel);
    if (error != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("function SetMute failed! errCode:%{public}d", error);
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    return replyParcel.ReadInt32();
}

int32_t CallManagerServiceProxy::MuteRinger()
{
    return SendRequest(INTERFACE_MUTE_RINGER);
}

int32_t CallManagerServiceProxy::SetAudioDevice(const AudioDevice &audioDevice)
{
    MessageParcel dataParcel;
    if (!dataParcel.WriteInterfaceToken(CallManagerServiceProxy::GetDescriptor())) {
        TELEPHONY_LOGE("write descriptor fail");
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    dataParcel.WriteRawData((const void *)&audioDevice, sizeof(AudioDevice));
    MessageParcel replyParcel;
    int32_t error = SendRequest(INTERFACE_SET_AUDIO_DEVICE, dataParcel, replyParcel);
    if (error != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("function SetAudioDevice failed! errCode:%{public}d", error);
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    return replyParcel.ReadInt32();
}

int32_t CallManagerServiceProxy::IsRinging(bool &enabled)
{
    MessageParcel dataParcel;
    if (!dataParcel.WriteInterfaceToken(CallManagerServiceProxy::GetDescriptor())) {
        TELEPHONY_LOGE("write descriptor fail");
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    MessageParcel replyParcel;
    int32_t error = SendRequest(INTERFACE_IS_RINGING, dataParcel, replyParcel);
    if (error != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("function IsRinging errCode:%{public}d", error);
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    int32_t result = replyParcel.ReadInt32();
    if (result == TELEPHONY_ERR_SUCCESS) {
        enabled = replyParcel.ReadBool();
    }
    return result;
}

int32_t CallManagerServiceProxy::IsInEmergencyCall(bool &enabled)
{
    MessageParcel dataParcel;
    if (!dataParcel.WriteInterfaceToken(CallManagerServiceProxy::GetDescriptor())) {
        TELEPHONY_LOGE("write descriptor fail");
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    MessageParcel replyParcel;
    int32_t error = SendRequest(INTERFACE_IS_EMERGENCY_CALL, dataParcel, replyParcel);
    if (error != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("function IsInEmergencyCall errCode:%{public}d", error);
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    int32_t result = replyParcel.ReadInt32();
    if (result == TELEPHONY_ERR_SUCCESS) {
        enabled = replyParcel.ReadBool();
    }
    return result;
}

int32_t CallManagerServiceProxy::StartDtmf(int32_t callId, char str)
{
    MessageParcel dataParcel;
    if (!dataParcel.WriteInterfaceToken(CallManagerServiceProxy::GetDescriptor())) {
        TELEPHONY_LOGE("write descriptor fail");
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    dataParcel.WriteInt32(callId);
    dataParcel.WriteInt8(str);
    MessageParcel replyParcel;
    int32_t error = SendRequest(CallManagerInterfaceCode::INTERFACE_START_DTMF, dataParcel, replyParcel);
    if (error != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("Function StartDtmf! errCode:%{public}d", error);
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    return replyParcel.ReadInt32();
}

int32_t CallManagerServiceProxy::StopDtmf(int32_t callId)
{
    MessageParcel dataParcel;
    if (!dataParcel.WriteInterfaceToken(CallManagerServiceProxy::GetDescriptor())) {
        TELEPHONY_LOGE("write descriptor fail");
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    dataParcel.WriteInt32(callId);
    MessageParcel replyParcel;
    int32_t error = SendRequest(CallManagerInterfaceCode::INTERFACE_STOP_DTMF, dataParcel, replyParcel);
    if (error != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("Function StopDtmf! errCode:%{public}d", error);
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    return replyParcel.ReadInt32();
}

int32_t CallManagerServiceProxy::PostDialProceed(int32_t callId, bool proceed)
{
    MessageParcel dataParcel;
    if (!dataParcel.WriteInterfaceToken(CallManagerServiceProxy::GetDescriptor())) {
        TELEPHONY_LOGE("WriteInterfaceToken fail");
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    dataParcel.WriteInt32(callId);
    dataParcel.WriteBool(proceed);
    MessageParcel replyParcel;
    int32_t error = SendRequest(CallManagerInterfaceCode::INTERFACE_POST_DIAL_PROCEED, dataParcel, replyParcel);
    if (error != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("Function PostDialProceed! errCode:%{public}d", error);
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    return replyParcel.ReadInt32();
}

int32_t CallManagerServiceProxy::GetCallWaiting(int32_t slotId)
{
    MessageParcel dataParcel;
    if (!dataParcel.WriteInterfaceToken(CallManagerServiceProxy::GetDescriptor())) {
        TELEPHONY_LOGE("write descriptor fail");
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    dataParcel.WriteInt32(slotId);
    MessageParcel replyParcel;
    int32_t error = SendRequest(CallManagerInterfaceCode::INTERFACE_GET_CALL_WAITING, dataParcel, replyParcel);
    if (error != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("Function GetCallWaiting! errCode:%{public}d", error);
        return error;
    }
    return replyParcel.ReadInt32();
}

int32_t CallManagerServiceProxy::SetCallWaiting(int32_t slotId, bool activate)
{
    MessageParcel dataParcel;
    if (!dataParcel.WriteInterfaceToken(CallManagerServiceProxy::GetDescriptor())) {
        TELEPHONY_LOGE("write descriptor fail");
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    dataParcel.WriteInt32(slotId);
    dataParcel.WriteBool(activate);
    MessageParcel replyParcel;
    int32_t error = SendRequest(CallManagerInterfaceCode::INTERFACE_SET_CALL_WAITING, dataParcel, replyParcel);
    if (error != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("Function SetCallWaiting! errCode:%{public}d", error);
        return error;
    }
    return replyParcel.ReadInt32();
}

int32_t CallManagerServiceProxy::GetCallRestriction(int32_t slotId, CallRestrictionType type)
{
    MessageParcel dataParcel;
    if (!dataParcel.WriteInterfaceToken(CallManagerServiceProxy::GetDescriptor())) {
        TELEPHONY_LOGE("write descriptor fail");
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    dataParcel.WriteInt32(slotId);
    dataParcel.WriteInt32(static_cast<int32_t>(type));
    MessageParcel replyParcel;
    int32_t error = SendRequest(CallManagerInterfaceCode::INTERFACE_GET_CALL_RESTRICTION, dataParcel, replyParcel);
    if (error != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("Function GetCallRestriction! errCode:%{public}d", error);
        return error;
    }
    return replyParcel.ReadInt32();
}

int32_t CallManagerServiceProxy::SetCallRestriction(int32_t slotId, CallRestrictionInfo &info)
{
    MessageParcel dataParcel;
    if (!dataParcel.WriteInterfaceToken(CallManagerServiceProxy::GetDescriptor())) {
        TELEPHONY_LOGE("write descriptor fail");
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    dataParcel.WriteInt32(slotId);
    dataParcel.WriteRawData((const void *)&info, sizeof(CallRestrictionInfo));
    MessageParcel replyParcel;
    int32_t error = SendRequest(CallManagerInterfaceCode::INTERFACE_SET_CALL_RESTRICTION, dataParcel, replyParcel);
    if (error != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("Function SetCallRestriction! errCode:%{public}d", error);
        return error;
    }
    return replyParcel.ReadInt32();
}

int32_t CallManagerServiceProxy::SetCallRestrictionPassword(
    int32_t slotId, CallRestrictionType fac, const char *oldPassword, const char *newPassword)
{
    MessageParcel dataParcel;
    if (!dataParcel.WriteInterfaceToken(CallManagerServiceProxy::GetDescriptor())) {
        TELEPHONY_LOGE("write descriptor fail");
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    if (oldPassword == nullptr || newPassword == nullptr || oldPassword[0] == '\0' || newPassword[0] == '\0') {
        TELEPHONY_LOGE("oldPassword or newPassword is empty");
        return TELEPHONY_ERR_ARGUMENT_INVALID;
    }
    dataParcel.WriteInt32(slotId);
    dataParcel.WriteInt32(static_cast<int32_t>(fac));
    dataParcel.WriteCString(oldPassword);
    dataParcel.WriteCString(newPassword);
    MessageParcel replyParcel;
    int32_t error =
        SendRequest(CallManagerInterfaceCode::INTERFACE_SET_CALL_RESTRICTION_PASSWORD, dataParcel, replyParcel);
    if (error != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("Function SetCallRestrictionPassword! errCode:%{public}d", error);
        return error;
    }
    return replyParcel.ReadInt32();
}

int32_t CallManagerServiceProxy::GetCallTransferInfo(int32_t slotId, CallTransferType type)
{
    MessageParcel dataParcel;
    if (!dataParcel.WriteInterfaceToken(CallManagerServiceProxy::GetDescriptor())) {
        TELEPHONY_LOGE("write descriptor fail");
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    dataParcel.WriteInt32(slotId);
    dataParcel.WriteInt32(static_cast<int32_t>(type));
    MessageParcel replyParcel;
    int32_t error = SendRequest(CallManagerInterfaceCode::INTERFACE_GET_CALL_TRANSFER, dataParcel, replyParcel);
    if (error != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("Function GetCallTransfer! errCode:%{public}d", error);
        return error;
    }
    return replyParcel.ReadInt32();
}

int32_t CallManagerServiceProxy::SetCallTransferInfo(int32_t slotId, CallTransferInfo &info)
{
    MessageParcel dataParcel;
    if (!dataParcel.WriteInterfaceToken(CallManagerServiceProxy::GetDescriptor())) {
        TELEPHONY_LOGE("write descriptor fail");
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    dataParcel.WriteInt32(slotId);
    dataParcel.WriteRawData((const void *)&info, sizeof(CallTransferInfo));
    MessageParcel replyParcel;
    int32_t error = SendRequest(CallManagerInterfaceCode::INTERFACE_SET_CALL_TRANSFER, dataParcel, replyParcel);
    if (error != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("Function SetCallTransfer! errCode:%{public}d", error);
        return error;
    }
    return replyParcel.ReadInt32();
}

int32_t CallManagerServiceProxy::CanSetCallTransferTime(int32_t slotId, bool &result)
{
    MessageParcel dataParcel;
    if (!dataParcel.WriteInterfaceToken(CallManagerServiceProxy::GetDescriptor())) {
        TELEPHONY_LOGE("[slot%{public}d] write descriptor fail", slotId);
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    if (!dataParcel.WriteInt32(slotId)) {
        TELEPHONY_LOGE("[slot%{public}d] write slotId fail", slotId);
        return TELEPHONY_ERR_WRITE_DATA_FAIL;
    }
    if (!dataParcel.WriteBool(result)) {
        TELEPHONY_LOGE("[slot%{public}d] write result fail", slotId);
        return TELEPHONY_ERR_WRITE_DATA_FAIL;
    }
    MessageParcel replyParcel;
    int32_t error =
        SendRequest(CallManagerInterfaceCode::INTERFACE_CAN_SET_CALL_TRANSFER_TIME, dataParcel, replyParcel);
    if (error == ERR_NONE) {
        result = replyParcel.ReadBool();
        return replyParcel.ReadInt32();
    }
    return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
}

int32_t CallManagerServiceProxy::SetCallPreferenceMode(int32_t slotId, int32_t mode)
{
    MessageParcel dataParcel;
    if (!dataParcel.WriteInterfaceToken(CallManagerServiceProxy::GetDescriptor())) {
        TELEPHONY_LOGE("write descriptor fail");
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    dataParcel.WriteInt32(slotId);
    dataParcel.WriteInt32(mode);
    MessageParcel replyParcel;
    int32_t error = SendRequest(CallManagerInterfaceCode::INTERFACE_SETCALL_PREFERENCEMODE, dataParcel, replyParcel);
    if (error != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("Function SetCallPreferenceMode! errCode:%{public}d", error);
        return error;
    }
    return replyParcel.ReadInt32();
}

int32_t CallManagerServiceProxy::StartRtt(int32_t callId, std::u16string &msg)
{
    MessageParcel dataParcel;
    if (!dataParcel.WriteInterfaceToken(CallManagerServiceProxy::GetDescriptor())) {
        TELEPHONY_LOGE("write descriptor fail");
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    if (msg.empty()) {
        TELEPHONY_LOGE("msg is empty");
        return TELEPHONY_ERR_ARGUMENT_INVALID;
    }
    dataParcel.WriteInt32(callId);
    dataParcel.WriteString16(msg);
    MessageParcel replyParcel;
    int32_t error = SendRequest(CallManagerInterfaceCode::INTERFACE_START_RTT, dataParcel, replyParcel);
    if (error != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("Function StartRtt errCode:%{public}d", error);
        return error;
    }
    return replyParcel.ReadInt32();
}

int32_t CallManagerServiceProxy::StopRtt(int32_t callId)
{
    MessageParcel dataParcel;
    if (!dataParcel.WriteInterfaceToken(CallManagerServiceProxy::GetDescriptor())) {
        TELEPHONY_LOGE("write descriptor fail");
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    dataParcel.WriteInt32(callId);
    MessageParcel replyParcel;
    int32_t error = SendRequest(CallManagerInterfaceCode::INTERFACE_STOP_RTT, dataParcel, replyParcel);
    if (error != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("Function StopRtt errCode:%{public}d", error);
        return error;
    }
    return replyParcel.ReadInt32();
}

int32_t CallManagerServiceProxy::CombineConference(int32_t mainCallId)
{
    MessageParcel dataParcel;
    if (!dataParcel.WriteInterfaceToken(CallManagerServiceProxy::GetDescriptor())) {
        TELEPHONY_LOGE("write descriptor fail");
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    dataParcel.WriteInt32(mainCallId);
    MessageParcel replyParcel;
    int32_t error = SendRequest(INTERFACE_COMBINE_CONFERENCE, dataParcel, replyParcel);
    if (error != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("Function CombineConference failed! errCode:%{public}d", error);
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    return replyParcel.ReadInt32();
}

int32_t CallManagerServiceProxy::SeparateConference(int32_t callId)
{
    MessageParcel dataParcel;
    if (!dataParcel.WriteInterfaceToken(CallManagerServiceProxy::GetDescriptor())) {
        TELEPHONY_LOGE("write descriptor fail");
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    dataParcel.WriteInt32(callId);
    MessageParcel replyParcel;
    int32_t error = SendRequest(INTERFACE_SEPARATE_CONFERENCE, dataParcel, replyParcel);
    if (error != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("Function SeparateConference call failed! errCode:%{public}d", error);
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    return replyParcel.ReadInt32();
}

int32_t CallManagerServiceProxy::KickOutFromConference(int32_t callId)
{
    MessageParcel dataParcel;
    if (!dataParcel.WriteInterfaceToken(CallManagerServiceProxy::GetDescriptor())) {
        TELEPHONY_LOGE("write descriptor fail");
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    dataParcel.WriteInt32(callId);
    MessageParcel replyParcel;
    int32_t error = SendRequest(INTERFACE_KICK_OUT_CONFERENCE, dataParcel, replyParcel);
    if (error != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("Function KickOutFromConference call failed! errCode:%{public}d", error);
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    return replyParcel.ReadInt32();
}

int32_t CallManagerServiceProxy::ControlCamera(int32_t callId, std::u16string &cameraId)
{
    MessageParcel dataParcel;
    if (!dataParcel.WriteInterfaceToken(CallManagerServiceProxy::GetDescriptor())) {
        TELEPHONY_LOGE("write descriptor fail");
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    dataParcel.WriteInt32(callId);
    dataParcel.WriteString16(cameraId);
    MessageParcel replyParcel;
    int32_t error = SendRequest(INTERFACE_CTRL_CAMERA, dataParcel, replyParcel);
    if (error != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("Function CtrlCamera call failed! errCode:%{public}d", error);
        return error;
    }
    return replyParcel.ReadInt32();
}

int32_t CallManagerServiceProxy::SetPreviewWindow(int32_t callId, std::string &surfaceId, sptr<Surface> surface)
{
    MessageParcel dataParcel;
    if (!dataParcel.WriteInterfaceToken(CallManagerServiceProxy::GetDescriptor())) {
        TELEPHONY_LOGE("write descriptor fail");
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    dataParcel.WriteInt32(callId);
    dataParcel.WriteString(surfaceId);
    if (surface != nullptr) {
        sptr<IBufferProducer> producer = surface->GetProducer();
        if (producer != nullptr) {
            dataParcel.WriteRemoteObject(producer->AsObject());
        }
    }
    MessageParcel replyParcel;
    int32_t error = SendRequest(INTERFACE_SET_PREVIEW_WINDOW, dataParcel, replyParcel);
    if (error != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("Function SetPreviewWindow call failed! errCode:%{public}d", error);
        return error;
    }
    return replyParcel.ReadInt32();
}

int32_t CallManagerServiceProxy::SetDisplayWindow(int32_t callId, std::string &surfaceId, sptr<Surface> surface)
{
    MessageParcel dataParcel;
    if (!dataParcel.WriteInterfaceToken(CallManagerServiceProxy::GetDescriptor())) {
        TELEPHONY_LOGE("write descriptor fail");
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    dataParcel.WriteInt32(callId);
    dataParcel.WriteString(surfaceId);
    if (surface != nullptr) {
        sptr<IBufferProducer> producer = surface->GetProducer();
        if (producer != nullptr) {
            dataParcel.WriteRemoteObject(producer->AsObject());
        }
    }
    MessageParcel replyParcel;
    int32_t error = SendRequest(INTERFACE_SET_DISPLAY_WINDOW, dataParcel, replyParcel);
    if (error != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("Function SetDisplayWindow call failed! errCode:%{public}d", error);
        return error;
    }
    return replyParcel.ReadInt32();
}

int32_t CallManagerServiceProxy::SetCameraZoom(float zoomRatio)
{
    MessageParcel dataParcel;
    if (!dataParcel.WriteInterfaceToken(CallManagerServiceProxy::GetDescriptor())) {
        TELEPHONY_LOGE("write descriptor fail");
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    dataParcel.WriteFloat(zoomRatio);
    MessageParcel replyParcel;
    int32_t error = SendRequest(INTERFACE_SET_CAMERA_ZOOM, dataParcel, replyParcel);
    if (error != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("Function SetCameraZoom call failed! errCode:%{public}d", error);
        return error;
    }
    return replyParcel.ReadInt32();
}

int32_t CallManagerServiceProxy::SetPausePicture(int32_t callId, std::u16string &path)
{
    MessageParcel dataParcel;
    if (!dataParcel.WriteInterfaceToken(CallManagerServiceProxy::GetDescriptor())) {
        TELEPHONY_LOGE("write descriptor fail");
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    dataParcel.WriteInt32(callId);
    dataParcel.WriteString16(path);
    MessageParcel replyParcel;
    int32_t error = SendRequest(INTERFACE_SET_PAUSE_IMAGE, dataParcel, replyParcel);
    if (error != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("Function SetPausePicture call failed! errCode:%{public}d", error);
        return error;
    }
    return replyParcel.ReadInt32();
}

int32_t CallManagerServiceProxy::SetDeviceDirection(int32_t callId, int32_t rotation)
{
    MessageParcel dataParcel;
    if (!dataParcel.WriteInterfaceToken(CallManagerServiceProxy::GetDescriptor())) {
        TELEPHONY_LOGE("write descriptor fail");
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    dataParcel.WriteInt32(callId);
    dataParcel.WriteInt32(rotation);
    MessageParcel replyParcel;
    int32_t error = SendRequest(INTERFACE_SET_DEVICE_DIRECTION, dataParcel, replyParcel);
    if (error != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("Function SetDeviceDirection call failed! errCode:%{public}d", error);
        return error;
    }
    return replyParcel.ReadInt32();
}

int32_t CallManagerServiceProxy::IsEmergencyPhoneNumber(std::u16string &number, int32_t slotId, bool &enabled)
{
    MessageParcel dataParcel;
    if (!dataParcel.WriteInterfaceToken(CallManagerServiceProxy::GetDescriptor())) {
        TELEPHONY_LOGE("write descriptor fail");
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    if (number.empty()) {
        TELEPHONY_LOGE("number is empty");
        return TELEPHONY_ERR_ARGUMENT_INVALID;
    }
    dataParcel.WriteString16(number);
    dataParcel.WriteInt32(slotId);
    MessageParcel replyParcel;
    int32_t error = SendRequest(INTERFACE_IS_EMERGENCY_NUMBER, dataParcel, replyParcel);
    if (error != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("Function IsEmergencyPhoneNumber call failed! errCode:%{public}d", error);
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    int32_t result = replyParcel.ReadInt32();
    if (result == TELEPHONY_SUCCESS) {
        enabled = replyParcel.ReadBool();
    }
    return result;
}

int32_t CallManagerServiceProxy::FormatPhoneNumber(
    std::u16string &number, std::u16string &countryCode, std::u16string &formatNumber)
{
    MessageParcel dataParcel;
    if (!dataParcel.WriteInterfaceToken(CallManagerServiceProxy::GetDescriptor())) {
        TELEPHONY_LOGE("write descriptor fail");
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    if (number.empty() || countryCode.empty()) {
        TELEPHONY_LOGE("number or countryCode is empty");
        return TELEPHONY_ERR_ARGUMENT_INVALID;
    }
    dataParcel.WriteString16(number);
    dataParcel.WriteString16(countryCode);
    MessageParcel replyParcel;
    int32_t error = SendRequest(INTERFACE_IS_FORMAT_NUMBER, dataParcel, replyParcel);
    if (error != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("Function FormatPhoneNumber call failed! errCode:%{public}d", error);
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    int32_t result = replyParcel.ReadInt32();
    if (result == TELEPHONY_SUCCESS) {
        formatNumber = replyParcel.ReadString16();
    }
    return result;
}

int32_t CallManagerServiceProxy::FormatPhoneNumberToE164(
    std::u16string &number, std::u16string &countryCode, std::u16string &formatNumber)
{
    MessageParcel dataParcel;
    if (!dataParcel.WriteInterfaceToken(CallManagerServiceProxy::GetDescriptor())) {
        TELEPHONY_LOGE("write descriptor fail");
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    if (number.empty() || countryCode.empty()) {
        TELEPHONY_LOGE("number or countryCode is empty");
        return TELEPHONY_ERR_ARGUMENT_INVALID;
    }
    dataParcel.WriteString16(number);
    dataParcel.WriteString16(countryCode);
    MessageParcel replyParcel;
    int32_t error = SendRequest(INTERFACE_IS_FORMAT_NUMBER_E164, dataParcel, replyParcel);
    if (error != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("Function FormatPhoneNumberToE164 call failed! errCode:%{public}d", error);
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    int32_t result = replyParcel.ReadInt32();
    if (result == TELEPHONY_SUCCESS) {
        formatNumber = replyParcel.ReadString16();
    }
    return result;
}

int32_t CallManagerServiceProxy::GetMainCallId(int32_t callId, int32_t &mainCallId)
{
    MessageParcel dataParcel;
    if (!dataParcel.WriteInterfaceToken(CallManagerServiceProxy::GetDescriptor())) {
        TELEPHONY_LOGE("write descriptor fail");
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    dataParcel.WriteInt32(callId);
    MessageParcel replyParcel;
    int32_t error = SendRequest(INTERFACE_GET_MAINID, dataParcel, replyParcel);
    if (error != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("Function StartConference call failed! errCode:%{public}d", error);
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    int32_t result = replyParcel.ReadInt32();
    if (result == TELEPHONY_ERR_SUCCESS) {
        mainCallId = replyParcel.ReadInt32();
    }
    return result;
}

int32_t CallManagerServiceProxy::GetSubCallIdList(int32_t callId, std::vector<std::u16string> &callIdList)
{
    MessageParcel dataParcel;
    if (!dataParcel.WriteInterfaceToken(CallManagerServiceProxy::GetDescriptor())) {
        TELEPHONY_LOGE("write descriptor fail");
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    dataParcel.WriteInt32(callId);
    MessageParcel replyParcel;
    int32_t error = SendRequest(INTERFACE_GET_SUBCALL_LIST_ID, dataParcel, replyParcel);
    if (error != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("Function GetSubCallIdList call failed! errCode:%{public}d", error);
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    int32_t result = replyParcel.ReadInt32();
    if (result == TELEPHONY_ERR_SUCCESS) {
        replyParcel.ReadString16Vector(&callIdList);
    }
    return result;
}

int32_t CallManagerServiceProxy::GetCallIdListForConference(int32_t callId, std::vector<std::u16string> &callIdList)
{
    MessageParcel dataParcel;
    if (!dataParcel.WriteInterfaceToken(CallManagerServiceProxy::GetDescriptor())) {
        TELEPHONY_LOGE("write descriptor fail");
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    dataParcel.WriteInt32(callId);
    MessageParcel replyParcel;
    int32_t error = SendRequest(INTERFACE_GET_CALL_LIST_ID_FOR_CONFERENCE, dataParcel, replyParcel);
    if (error != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("Function GetCallIdListForConference call failed! errCode:%{public}d", error);
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    int32_t result = replyParcel.ReadInt32();
    if (result == TELEPHONY_ERR_SUCCESS) {
        replyParcel.ReadString16Vector(&callIdList);
    }
    return result;
}

int32_t CallManagerServiceProxy::GetImsConfig(int32_t slotId, ImsConfigItem item)
{
    MessageParcel dataParcel;
    if (!dataParcel.WriteInterfaceToken(CallManagerServiceProxy::GetDescriptor())) {
        TELEPHONY_LOGE("write descriptor fail");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    dataParcel.WriteInt32(slotId);
    dataParcel.WriteInt32(item);
    MessageParcel replyParcel;
    int32_t error = SendRequest(INTERFACE_GET_IMS_CONFIG, dataParcel, replyParcel);
    if (error != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("function GetImsConfig failed! errCode:%{public}d", error);
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    return replyParcel.ReadInt32();
}

int32_t CallManagerServiceProxy::SetImsConfig(int32_t slotId, ImsConfigItem item, std::u16string &value)
{
    MessageParcel dataParcel;
    if (!dataParcel.WriteInterfaceToken(CallManagerServiceProxy::GetDescriptor())) {
        TELEPHONY_LOGE("write descriptor fail");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    dataParcel.WriteInt32(slotId);
    dataParcel.WriteInt32(item);
    dataParcel.WriteString16(value);
    MessageParcel replyParcel;
    int32_t error = SendRequest(INTERFACE_SET_IMS_CONFIG, dataParcel, replyParcel);
    if (error != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("function SetImsConfig failed! errCode:%{public}d", error);
        return error;
    }
    return replyParcel.ReadInt32();
}

int32_t CallManagerServiceProxy::GetImsFeatureValue(int32_t slotId, FeatureType type)
{
    MessageParcel dataParcel;
    if (!dataParcel.WriteInterfaceToken(CallManagerServiceProxy::GetDescriptor())) {
        TELEPHONY_LOGE("write descriptor fail");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    dataParcel.WriteInt32(slotId);
    dataParcel.WriteInt32(type);
    MessageParcel replyParcel;
    int32_t error = SendRequest(INTERFACE_GET_IMS_FEATURE_VALUE, dataParcel, replyParcel);
    if (error != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("function GetImsFeatureValue failed! errCode:%{public}d", error);
        return error;
    }
    return replyParcel.ReadInt32();
}

int32_t CallManagerServiceProxy::SetImsFeatureValue(int32_t slotId, FeatureType type, int32_t value)
{
    MessageParcel dataParcel;
    if (!dataParcel.WriteInterfaceToken(CallManagerServiceProxy::GetDescriptor())) {
        TELEPHONY_LOGE("write descriptor fail");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    dataParcel.WriteInt32(slotId);
    dataParcel.WriteInt32(type);
    dataParcel.WriteInt32(value);
    MessageParcel replyParcel;
    int32_t error = SendRequest(INTERFACE_SET_IMS_FEATURE_VALUE, dataParcel, replyParcel);
    if (error != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("function SetImsFeatureValue failed! errCode:%{public}d", error);
        return error;
    }
    return replyParcel.ReadInt32();
}

int32_t CallManagerServiceProxy::UpdateImsCallMode(int32_t callId, ImsCallMode mode)
{
    MessageParcel dataParcel;
    if (!dataParcel.WriteInterfaceToken(CallManagerServiceProxy::GetDescriptor())) {
        TELEPHONY_LOGE("write descriptor fail");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    dataParcel.WriteInt32(callId);
    dataParcel.WriteUint32(mode);
    MessageParcel replyParcel;
    int32_t error = SendRequest(INTERFACE_UPDATE_CALL_MEDIA_MODE, dataParcel, replyParcel);
    if (error != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("function UpdateImsCallMode failed! errCode:%{public}d", error);
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    return replyParcel.ReadInt32();
}

int32_t CallManagerServiceProxy::EnableImsSwitch(int32_t slotId)
{
    MessageParcel dataParcel;
    if (!dataParcel.WriteInterfaceToken(CallManagerServiceProxy::GetDescriptor())) {
        TELEPHONY_LOGE("write descriptor fail");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    dataParcel.WriteInt32(slotId);
    MessageParcel replyParcel;
    int32_t error = SendRequest(INTERFACE_ENABLE_VOLTE, dataParcel, replyParcel);
    if (error != ERR_NONE) {
        TELEPHONY_LOGE("function EnableImsSwitch failed! errCode:%{public}d", error);
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    return replyParcel.ReadInt32();
}

int32_t CallManagerServiceProxy::DisableImsSwitch(int32_t slotId)
{
    MessageParcel dataParcel;
    if (!dataParcel.WriteInterfaceToken(CallManagerServiceProxy::GetDescriptor())) {
        TELEPHONY_LOGE("write descriptor fail");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    dataParcel.WriteInt32(slotId);
    MessageParcel replyParcel;
    int32_t error = SendRequest(INTERFACE_DISABLE_VOLTE, dataParcel, replyParcel);
    if (error != ERR_NONE) {
        TELEPHONY_LOGE("function DisableImsSwitch failed! errCode:%{public}d", error);
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    return replyParcel.ReadInt32();
}

int32_t CallManagerServiceProxy::IsImsSwitchEnabled(int32_t slotId, bool &enabled)
{
    MessageParcel dataParcel;
    if (!dataParcel.WriteInterfaceToken(CallManagerServiceProxy::GetDescriptor())) {
        TELEPHONY_LOGE("write descriptor fail");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    dataParcel.WriteInt32(slotId);
    MessageParcel replyParcel;
    int32_t error = SendRequest(INTERFACE_IS_VOLTE_ENABLED, dataParcel, replyParcel);
    if (error != ERR_NONE) {
        TELEPHONY_LOGE("function IsImsSwitchEnabled failed! errCode:%{public}d", error);
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    enabled = replyParcel.ReadBool();
    return replyParcel.ReadInt32();
}

int32_t CallManagerServiceProxy::SetVoNRState(int32_t slotId, int32_t state)
{
    MessageParcel dataParcel;
    if (!dataParcel.WriteInterfaceToken(CallManagerServiceProxy::GetDescriptor())) {
        TELEPHONY_LOGE("write descriptor fail");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    dataParcel.WriteInt32(slotId);
    dataParcel.WriteInt32(state);
    MessageParcel replyParcel;
    int32_t error = SendRequest(INTERFACE_SET_VONR_STATE, dataParcel, replyParcel);
    if (error != ERR_NONE) {
        TELEPHONY_LOGE("function SetVoNRState failed! errCode:%{public}d", error);
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    return replyParcel.ReadInt32();
}

int32_t CallManagerServiceProxy::GetVoNRState(int32_t slotId, int32_t &state)
{
    MessageParcel dataParcel;
    if (!dataParcel.WriteInterfaceToken(CallManagerServiceProxy::GetDescriptor())) {
        TELEPHONY_LOGE("write descriptor fail");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    dataParcel.WriteInt32(slotId);
    MessageParcel replyParcel;
    int32_t error = SendRequest(INTERFACE_GET_VONR_STATE, dataParcel, replyParcel);
    if (error != ERR_NONE) {
        TELEPHONY_LOGE("function GetVoNRState failed! errCode:%{public}d", error);
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    state = replyParcel.ReadInt32();
    return replyParcel.ReadInt32();
}

int32_t CallManagerServiceProxy::JoinConference(int32_t callId, std::vector<std::u16string> &numberList)
{
    MessageParcel dataParcel;
    if (!dataParcel.WriteInterfaceToken(CallManagerServiceProxy::GetDescriptor())) {
        TELEPHONY_LOGE("write descriptor fail");
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    dataParcel.WriteInt32(callId);
    dataParcel.WriteString16Vector(numberList);
    MessageParcel replyParcel;
    int32_t error = SendRequest(INTERFACE_JOIN_CONFERENCE, dataParcel, replyParcel);
    if (error != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("function JoinConference failed! errCode:%{public}d", error);
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    return replyParcel.ReadInt32();
}

int32_t CallManagerServiceProxy::ReportOttCallDetailsInfo(std::vector<OttCallDetailsInfo> &ottVec)
{
    MessageParcel dataParcel;
    if (!dataParcel.WriteInterfaceToken(CallManagerServiceProxy::GetDescriptor())) {
        TELEPHONY_LOGE("write descriptor fail");
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    if (ottVec.empty()) {
        TELEPHONY_LOGE("ottVec is empty");
        return TELEPHONY_ERR_ARGUMENT_INVALID;
    }
    dataParcel.WriteInt32(ottVec.size());
    std::vector<OttCallDetailsInfo>::iterator it = ottVec.begin();
    for (; it != ottVec.end(); ++it) {
        dataParcel.WriteRawData((const void *)&(*it), sizeof(OttCallDetailsInfo));
    }
    MessageParcel replyParcel;
    int32_t error = SendRequest(INTERFACE_REPORT_OTT_CALL_DETAIL_INFO, dataParcel, replyParcel);
    if (error != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("function ReportOttCallDetailsInfo failed! errCode:%{public}d", error);
        return error;
    }
    return replyParcel.ReadInt32();
}

int32_t CallManagerServiceProxy::ReportOttCallEventInfo(OttCallEventInfo &eventInfo)
{
    MessageParcel dataParcel;
    if (!dataParcel.WriteInterfaceToken(CallManagerServiceProxy::GetDescriptor())) {
        TELEPHONY_LOGE("write descriptor fail");
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    dataParcel.WriteRawData((const void *)&eventInfo, sizeof(OttCallEventInfo));
    MessageParcel replyParcel;
    int32_t error = SendRequest(INTERFACE_REPORT_OTT_CALL_EVENT_INFO, dataParcel, replyParcel);
    if (error != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("function ReportOttCallDetailsInfo failed! errCode:%{public}d", error);
        return error;
    }
    return replyParcel.ReadInt32();
}

int32_t CallManagerServiceProxy::CloseUnFinishedUssd(int32_t slotId)
{
    MessageParcel dataParcel;
    if (!dataParcel.WriteInterfaceToken(CallManagerServiceProxy::GetDescriptor())) {
        TELEPHONY_LOGE("write descriptor fail");
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    dataParcel.WriteInt32(slotId);
    MessageParcel replyParcel;
    int32_t error = SendRequest(CallManagerInterfaceCode::INTERFACE_CLOSE_UNFINISHED_USSD, dataParcel, replyParcel);
    if (error != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("Function CloseUnFinishedUssd! errCode:%{public}d", error);
        return error;
    }
    return replyParcel.ReadInt32();
}

int32_t CallManagerServiceProxy::InputDialerSpecialCode(const std::string &specialCode)
{
    MessageParcel dataParcel;
    if (!dataParcel.WriteInterfaceToken(CallManagerServiceProxy::GetDescriptor())) {
        TELEPHONY_LOGE("write descriptor fail");
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    dataParcel.WriteString(specialCode);
    MessageParcel replyParcel;
    int32_t error = SendRequest(CallManagerInterfaceCode::INTERFACE_INPUT_DIALER_SPECIAL_CODE, dataParcel, replyParcel);
    if (error != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("Function InputDialerSpecialCode! errCode:%{public}d", error);
        return error;
    }
    return replyParcel.ReadInt32();
}

int32_t CallManagerServiceProxy::RemoveMissedIncomingCallNotification()
{
    MessageParcel dataParcel;
    if (!dataParcel.WriteInterfaceToken(CallManagerServiceProxy::GetDescriptor())) {
        TELEPHONY_LOGE("write descriptor fail");
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    MessageParcel replyParcel;
    int32_t error = SendRequest(
        CallManagerInterfaceCode::INTERFACE_CANCEL_MISSED_INCOMING_CALL_NOTIFICATION, dataParcel, replyParcel);
    if (error != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("Function RemoveMissedIncomingCallNotification! errCode:%{public}d", error);
        return error;
    }
    return replyParcel.ReadInt32();
}

int32_t CallManagerServiceProxy::SetVoIPCallState(int32_t state)
{
    MessageParcel dataParcel;
    if (!dataParcel.WriteInterfaceToken(CallManagerServiceProxy::GetDescriptor())) {
        TELEPHONY_LOGE("write descriptor fail");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    dataParcel.WriteInt32(state);
    MessageParcel replyParcel;
    int32_t error = SendRequest(INTERFACE_SET_VOIP_CALL_STATE, dataParcel, replyParcel);
    if (error != ERR_NONE) {
        TELEPHONY_LOGE("function SetVoIPCallState failed! errCode:%{public}d", error);
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    return replyParcel.ReadInt32();
}

int32_t CallManagerServiceProxy::GetVoIPCallState(int32_t &state)
{
    MessageParcel dataParcel;
    if (!dataParcel.WriteInterfaceToken(CallManagerServiceProxy::GetDescriptor())) {
        TELEPHONY_LOGE("write descriptor fail");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    MessageParcel replyParcel;
    int32_t error = SendRequest(INTERFACE_GET_VOIP_CALL_STATE, dataParcel, replyParcel);
    if (error != ERR_NONE) {
        TELEPHONY_LOGE("function GetVoIPCallState failed! errCode:%{public}d", error);
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    state = replyParcel.ReadInt32();
    return replyParcel.ReadInt32();
}

sptr<IRemoteObject> CallManagerServiceProxy::GetProxyObjectPtr(CallManagerProxyType proxyType)
{
    MessageParcel dataParcel;
    if (!dataParcel.WriteInterfaceToken(CallManagerServiceProxy::GetDescriptor())) {
        TELEPHONY_LOGE("write descriptor fail");
        return nullptr;
    }
    dataParcel.WriteInt32(static_cast<int32_t>(proxyType));
    MessageParcel replyParcel;
    int32_t error = SendRequest(INTERFACE_GET_PROXY_OBJECT_PTR, dataParcel, replyParcel);
    if (error != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("function GetProxyObjectPtr failed! errCode:%{public}d", error);
        return nullptr;
    }
    return replyParcel.ReadRemoteObject();
}

int32_t CallManagerServiceProxy::ReportAudioDeviceInfo()
{
    return SendRequest(INTERFACE_REPORT_AUDIO_DEVICE_INFO);
}

int32_t CallManagerServiceProxy::CancelCallUpgrade(int32_t callId)
{
    MessageParcel dataParcel;
    if (!dataParcel.WriteInterfaceToken(CallManagerServiceProxy::GetDescriptor())) {
        TELEPHONY_LOGE("write descriptor fail");
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    dataParcel.WriteInt32(callId);
    MessageParcel replyParcel;
    int32_t error = SendRequest(CallManagerInterfaceCode::INTERFACE_CANCEL_CALL_UPGRADE, dataParcel, replyParcel);
    if (error != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("Function CloseUnFinishedUssd! errCode:%{public}d", error);
        return error;
    }
    return replyParcel.ReadInt32();
}

int32_t CallManagerServiceProxy::RequestCameraCapabilities(int32_t callId)
{
    MessageParcel dataParcel;
    if (!dataParcel.WriteInterfaceToken(CallManagerServiceProxy::GetDescriptor())) {
        TELEPHONY_LOGE("write descriptor fail");
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    dataParcel.WriteInt32(callId);
    MessageParcel replyParcel;
    int32_t error = SendRequest(
        CallManagerInterfaceCode::INTERFACE_REQUEST_CAMERA_CAPABILITIES, dataParcel, replyParcel);
    if (error != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("Function CloseUnFinishedUssd! errCode:%{public}d", error);
        return error;
    }
    return replyParcel.ReadInt32();
}

int32_t CallManagerServiceProxy::SendRequest(CallManagerInterfaceCode code)
{
    MessageParcel dataParcel;
    if (!dataParcel.WriteInterfaceToken(CallManagerServiceProxy::GetDescriptor())) {
        TELEPHONY_LOGE("write descriptor fail");
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    MessageParcel replyParcel;
    int32_t error = SendRequest(code, dataParcel, replyParcel);
    if (error != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("CallManagerInterfaceCode:%{public}d errCode:%{public}d", static_cast<int32_t>(code), error);
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    return replyParcel.ReadInt32();
}

int32_t CallManagerServiceProxy::SendRequest(
    CallManagerInterfaceCode code, MessageParcel &dataParcel, MessageParcel &replyParcel)
{
    auto remote = Remote();
    if (remote == nullptr) {
        TELEPHONY_LOGE("function Remote() return nullptr!");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    MessageOption option;
    return remote->SendRequest(static_cast<int32_t>(code), dataParcel, replyParcel, option);
}

int32_t CallManagerServiceProxy::SendCallUiEvent(int32_t callId, std:string &eventName)
{
    MessageParcel dataParcel;
    if (!dataParcel.WriteInterfaceToken(CallManagerServiceProxy::GetDescriptor())) {
        TELEPHONY_LOGE("write descriptor fail");
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    dataParcel.WriteInt32(callId);
    dataParcel.WriteString(eventName);
    MessageParcel replyParcel;
    int32_t error = SendRequest(INTERFACE_SEND_CALLUI_EVENT, dataParcel, replyParcel);
    if (error != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("function SendCallUiEvent call failed! errCode:%{public}d", error);
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    return replyParcel.ReadInt32();
}
} // namespace Telephony
} // namespace OHOS
