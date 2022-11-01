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

#include "message_option.h"
#include "message_parcel.h"

#include "call_manager_errors.h"

namespace OHOS {
namespace Telephony {
CallManagerServiceProxy::CallManagerServiceProxy(const sptr<IRemoteObject> &impl)
    : IRemoteProxy<ICallManagerService>(impl)
{}

int32_t CallManagerServiceProxy::RegisterCallBack(const sptr<ICallAbilityCallback> &callback)
{
    MessageOption option;
    MessageParcel dataParcel;
    MessageParcel replyParcel;
    if (!dataParcel.WriteInterfaceToken(CallManagerServiceProxy::GetDescriptor())) {
        TELEPHONY_LOGE("write descriptor fail");
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    dataParcel.WriteRemoteObject(callback->AsObject().GetRefPtr());
    auto remote = Remote();
    if (remote == nullptr) {
        TELEPHONY_LOGE("function Remote() return nullptr!");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    int32_t error = remote->SendRequest(INTERFACE_REGISTER_CALLBACK, dataParcel, replyParcel, option);
    if (error != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("Function RegisterCallBack! errCode:%{public}d", error);
        return error;
    }
    return replyParcel.ReadInt32();
}

int32_t CallManagerServiceProxy::UnRegisterCallBack()
{
    MessageOption option;
    MessageParcel dataParcel;
    MessageParcel replyParcel;
    if (!dataParcel.WriteInterfaceToken(CallManagerServiceProxy::GetDescriptor())) {
        TELEPHONY_LOGE("write descriptor fail");
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    auto remote = Remote();
    if (remote == nullptr) {
        TELEPHONY_LOGE("function Remote() return nullptr!");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    int32_t error = remote->SendRequest(INTERFACE_UNREGISTER_CALLBACK, dataParcel, replyParcel, option);
    if (error != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("Function RegisterCallBack! errCode:%{public}d", error);
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    return replyParcel.ReadInt32();
}

int32_t CallManagerServiceProxy::DialCall(std::u16string number, AppExecFwk::PacMap &extras)
{
    MessageOption option;
    MessageParcel dataParcel;
    MessageParcel replyParcel;
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
    auto remote = Remote();
    if (remote == nullptr) {
        TELEPHONY_LOGE("function Remote() return nullptr!");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    int32_t error = remote->SendRequest(INTERFACE_DIAL_CALL, dataParcel, replyParcel, option);
    if (error != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("function DialCall call failed! errCode:%{public}d", error);
        return error;
    }
    return replyParcel.ReadInt32();
}

int32_t CallManagerServiceProxy::AnswerCall(int32_t callId, int32_t videoState)
{
    MessageOption option;
    MessageParcel dataParcel;
    MessageParcel replyParcel;
    if (!dataParcel.WriteInterfaceToken(CallManagerServiceProxy::GetDescriptor())) {
        TELEPHONY_LOGE("write descriptor fail");
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    dataParcel.WriteInt32(callId);
    dataParcel.WriteInt32(videoState);
    auto remote = Remote();
    if (remote == nullptr) {
        TELEPHONY_LOGE("function Remote() return nullptr!");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    int32_t error = remote->SendRequest(INTERFACE_ANSWER_CALL, dataParcel, replyParcel, option);
    if (error != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("function AnswerCall call failed! errCode:%{public}d", error);
        return error;
    }
    return replyParcel.ReadInt32();
}

int32_t CallManagerServiceProxy::RejectCall(int32_t callId, bool rejectWithMessage, std::u16string textMessage)
{
    MessageOption option;
    MessageParcel dataParcel;
    MessageParcel replyParcel;
    if (!dataParcel.WriteInterfaceToken(CallManagerServiceProxy::GetDescriptor())) {
        TELEPHONY_LOGE("write descriptor fail");
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    dataParcel.WriteInt32(callId);
    dataParcel.WriteBool(rejectWithMessage);
    dataParcel.WriteString16(textMessage);
    auto remote = Remote();
    if (remote == nullptr) {
        TELEPHONY_LOGE("function Remote() return nullptr!");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    int32_t error = remote->SendRequest(INTERFACE_REJECT_CALL, dataParcel, replyParcel, option);
    if (error != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("function RejectCall call failed! errCode:%{public}d", error);
        return error;
    }
    return replyParcel.ReadInt32();
}

int32_t CallManagerServiceProxy::HangUpCall(int32_t callId)
{
    MessageOption option;
    MessageParcel dataParcel;
    MessageParcel replyParcel;
    if (!dataParcel.WriteInterfaceToken(CallManagerServiceProxy::GetDescriptor())) {
        TELEPHONY_LOGE("write descriptor fail");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    dataParcel.WriteInt32(callId);
    auto remote = Remote();
    if (remote == nullptr) {
        TELEPHONY_LOGE("function Remote() return nullptr!");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    int32_t error = remote->SendRequest(INTERFACE_DISCONNECT_CALL, dataParcel, replyParcel, option);
    if (error != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("function HangUpCall call failed! errCode:%{public}d", error);
        return error;
    }
    return replyParcel.ReadInt32();
}

int32_t CallManagerServiceProxy::GetCallState()
{
    MessageOption option;
    MessageParcel dataParcel;
    MessageParcel replyParcel;
    if (!dataParcel.WriteInterfaceToken(CallManagerServiceProxy::GetDescriptor())) {
        TELEPHONY_LOGE("write descriptor fail");
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    auto remote = Remote();
    if (remote == nullptr) {
        TELEPHONY_LOGE("function Remote() return nullptr!");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    int32_t error = remote->SendRequest(INTERFACE_GET_CALL_STATE, dataParcel, replyParcel, option);
    if (error != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("function GetCallState! errCode:%{public}d", error);
        return error;
    }
    return replyParcel.ReadInt32();
}

int32_t CallManagerServiceProxy::HoldCall(int32_t callId)
{
    MessageOption option;
    MessageParcel dataParcel;
    MessageParcel replyParcel;
    if (!dataParcel.WriteInterfaceToken(CallManagerServiceProxy::GetDescriptor())) {
        TELEPHONY_LOGE("write descriptor fail");
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    dataParcel.WriteInt32(callId);
    auto remote = Remote();
    if (remote == nullptr) {
        TELEPHONY_LOGE("function Remote() return nullptr!");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    int32_t error = remote->SendRequest(INTERFACE_HOLD_CALL, dataParcel, replyParcel, option);
    if (error != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("Function HoldCall call failed! errCode:%{public}d", error);
        return error;
    }
    return replyParcel.ReadInt32();
}

int32_t CallManagerServiceProxy::UnHoldCall(int32_t callId)
{
    MessageOption option;
    MessageParcel dataParcel;
    MessageParcel replyParcel;
    if (!dataParcel.WriteInterfaceToken(CallManagerServiceProxy::GetDescriptor())) {
        TELEPHONY_LOGE("write descriptor fail");
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    dataParcel.WriteInt32(callId);
    auto remote = Remote();
    if (remote == nullptr) {
        TELEPHONY_LOGE("function Remote() return nullptr!");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    int32_t error = remote->SendRequest(INTERFACE_UNHOLD_CALL, dataParcel, replyParcel, option);
    if (error != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("Function UnHoldCall call failed! errCode:%{public}d", error);
        return error;
    }
    return replyParcel.ReadInt32();
}

int32_t CallManagerServiceProxy::SwitchCall(int32_t callId)
{
    MessageOption option;
    MessageParcel dataParcel;
    MessageParcel replyParcel;
    if (!dataParcel.WriteInterfaceToken(CallManagerServiceProxy::GetDescriptor())) {
        TELEPHONY_LOGE("write descriptor fail");
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    dataParcel.WriteInt32(callId);
    auto remote = Remote();
    if (remote == nullptr) {
        TELEPHONY_LOGE("function Remote() return nullptr!");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    int32_t error = remote->SendRequest(INTERFACE_SWAP_CALL, dataParcel, replyParcel, option);
    if (error != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("Function UnHoldCall call failed! errCode:%{public}d", error);
        return error;
    }
    return replyParcel.ReadInt32();
}

bool CallManagerServiceProxy::HasCall()
{
    MessageOption option;
    MessageParcel dataParcel;
    MessageParcel replyParcel;
    if (!dataParcel.WriteInterfaceToken(CallManagerServiceProxy::GetDescriptor())) {
        TELEPHONY_LOGE("write descriptor fail");
        return false;
    }
    auto remote = Remote();
    if (remote == nullptr) {
        TELEPHONY_LOGE("function Remote() return nullptr!");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    int32_t error = remote->SendRequest(INTERFACE_HAS_CALL, dataParcel, replyParcel, option);
    if (error != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("Function SetAudioDevice! errCode:%{public}d", error);
        return false;
    }
    return replyParcel.ReadBool();
}

bool CallManagerServiceProxy::IsNewCallAllowed()
{
    MessageOption option;
    MessageParcel dataParcel;
    MessageParcel replyParcel;
    if (!dataParcel.WriteInterfaceToken(CallManagerServiceProxy::GetDescriptor())) {
        TELEPHONY_LOGE("write descriptor fail");
        return false;
    }
    auto remote = Remote();
    if (remote == nullptr) {
        TELEPHONY_LOGE("function Remote() return nullptr!");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    int32_t error = remote->SendRequest(INTERFACE_IS_NEW_CALL_ALLOWED, dataParcel, replyParcel, option);
    if (error != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("Function SetAudioDevice! errCode:%{public}d", error);
        return false;
    }
    return replyParcel.ReadBool();
}

int32_t CallManagerServiceProxy::SetMuted(bool isMute)
{
    MessageOption option;
    MessageParcel dataParcel;
    MessageParcel replyParcel;
    if (!dataParcel.WriteInterfaceToken(CallManagerServiceProxy::GetDescriptor())) {
        TELEPHONY_LOGE("write descriptor fail");
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    dataParcel.WriteBool(isMute);
    auto remote = Remote();
    if (remote == nullptr) {
        TELEPHONY_LOGE("function Remote() return nullptr!");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    int32_t error = remote->SendRequest(INTERFACE_SET_MUTE, dataParcel, replyParcel, option);
    if (error != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("function SetMute failed! errCode:%{public}d", error);
        return error;
    }
    return replyParcel.ReadInt32();
}

int32_t CallManagerServiceProxy::MuteRinger()
{
    MessageOption option;
    MessageParcel dataParcel;
    MessageParcel replyParcel;
    if (!dataParcel.WriteInterfaceToken(CallManagerServiceProxy::GetDescriptor())) {
        TELEPHONY_LOGE("write descriptor fail");
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    auto remote = Remote();
    if (remote == nullptr) {
        TELEPHONY_LOGE("function Remote() return nullptr!");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    int32_t error = remote->SendRequest(INTERFACE_MUTE_RINGER, dataParcel, replyParcel, option);
    if (error != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("function MuteRinger failed! errCode:%{public}d", error);
        return error;
    }
    return replyParcel.ReadInt32();
}

int32_t CallManagerServiceProxy::SetAudioDevice(AudioDevice deviceType, const std::string &bluetoothAddress)
{
    MessageOption option;
    MessageParcel dataParcel;
    MessageParcel replyParcel;
    if (!dataParcel.WriteInterfaceToken(CallManagerServiceProxy::GetDescriptor())) {
        TELEPHONY_LOGE("write descriptor fail");
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    dataParcel.WriteInt32(static_cast<int32_t>(deviceType));
    dataParcel.WriteString(bluetoothAddress);
    auto remote = Remote();
    if (remote == nullptr) {
        TELEPHONY_LOGE("function Remote() return nullptr!");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    int32_t error = remote->SendRequest(INTERFACE_SET_AUDIO_DEVICE, dataParcel, replyParcel, option);
    if (error != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("function SetAudioDevice failed! errCode:%{public}d", error);
        return error;
    }
    return replyParcel.ReadInt32();
}

bool CallManagerServiceProxy::IsRinging()
{
    MessageOption option;
    MessageParcel dataParcel;
    MessageParcel replyParcel;
    if (!dataParcel.WriteInterfaceToken(CallManagerServiceProxy::GetDescriptor())) {
        TELEPHONY_LOGE("write descriptor fail");
        return false;
    }
    auto remote = Remote();
    if (remote == nullptr) {
        TELEPHONY_LOGE("function Remote() return nullptr!");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    int32_t error = remote->SendRequest(INTERFACE_IS_RINGING, dataParcel, replyParcel, option);
    if (error != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("function IsRinging errCode:%{public}d", error);
        return false;
    }
    return replyParcel.ReadBool();
}

bool CallManagerServiceProxy::IsInEmergencyCall()
{
    MessageOption option;
    MessageParcel dataParcel;
    MessageParcel replyParcel;
    if (!dataParcel.WriteInterfaceToken(CallManagerServiceProxy::GetDescriptor())) {
        TELEPHONY_LOGE("write descriptor fail");
        return false;
    }
    auto remote = Remote();
    if (remote == nullptr) {
        TELEPHONY_LOGE("function Remote() return nullptr!");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    int32_t error = remote->SendRequest(INTERFACE_IS_EMERGENCY_CALL, dataParcel, replyParcel, option);
    if (error != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("function IsInEmergencyCall errCode:%{public}d", error);
        return false;
    }
    return replyParcel.ReadBool();
}

int32_t CallManagerServiceProxy::StartDtmf(int32_t callId, char str)
{
    MessageOption option;
    MessageParcel dataParcel;
    MessageParcel replyParcel;
    if (!dataParcel.WriteInterfaceToken(CallManagerServiceProxy::GetDescriptor())) {
        TELEPHONY_LOGE("write descriptor fail");
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    auto remote = Remote();
    if (remote == nullptr) {
        TELEPHONY_LOGE("function Remote() return nullptr!");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    dataParcel.WriteInt32(callId);
    dataParcel.WriteInt8(str);
    int32_t error = remote->SendRequest(CallManagerSurfaceCode::INTERFACE_START_DTMF, dataParcel, replyParcel, option);
    if (error != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("Function StartDtmf! errCode:%{public}d", error);
        return error;
    }
    return replyParcel.ReadInt32();
}

int32_t CallManagerServiceProxy::StopDtmf(int32_t callId)
{
    MessageOption option;
    MessageParcel dataParcel;
    MessageParcel replyParcel;
    if (!dataParcel.WriteInterfaceToken(CallManagerServiceProxy::GetDescriptor())) {
        TELEPHONY_LOGE("write descriptor fail");
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    auto remote = Remote();
    if (remote == nullptr) {
        TELEPHONY_LOGE("function Remote() return nullptr!");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    dataParcel.WriteInt32(callId);
    int32_t error = remote->SendRequest(CallManagerSurfaceCode::INTERFACE_STOP_DTMF, dataParcel, replyParcel, option);
    if (error != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("Function StopDtmf! errCode:%{public}d", error);
        return error;
    }
    return replyParcel.ReadInt32();
}

int32_t CallManagerServiceProxy::GetCallWaiting(int32_t slotId)
{
    MessageOption option;
    MessageParcel dataParcel;
    MessageParcel replyParcel;
    if (!dataParcel.WriteInterfaceToken(CallManagerServiceProxy::GetDescriptor())) {
        TELEPHONY_LOGE("write descriptor fail");
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    auto remote = Remote();
    if (remote == nullptr) {
        TELEPHONY_LOGE("function Remote() return nullptr!");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    dataParcel.WriteInt32(slotId);
    int32_t error =
        remote->SendRequest(CallManagerSurfaceCode::INTERFACE_GET_CALL_WAITING, dataParcel, replyParcel, option);
    if (error != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("Function GetCallWaiting! errCode:%{public}d", error);
        return error;
    }
    return replyParcel.ReadInt32();
}

int32_t CallManagerServiceProxy::SetCallWaiting(int32_t slotId, bool activate)
{
    MessageOption option;
    MessageParcel dataParcel;
    MessageParcel replyParcel;
    if (!dataParcel.WriteInterfaceToken(CallManagerServiceProxy::GetDescriptor())) {
        TELEPHONY_LOGE("write descriptor fail");
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    auto remote = Remote();
    if (remote == nullptr) {
        TELEPHONY_LOGE("function Remote() return nullptr!");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    dataParcel.WriteInt32(slotId);
    dataParcel.WriteBool(activate);
    int32_t error =
        remote->SendRequest(CallManagerSurfaceCode::INTERFACE_SET_CALL_WAITING, dataParcel, replyParcel, option);
    if (error != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("Function SetCallWaiting! errCode:%{public}d", error);
        return error;
    }
    return replyParcel.ReadInt32();
}

int32_t CallManagerServiceProxy::GetCallRestriction(int32_t slotId, CallRestrictionType type)
{
    MessageOption option;
    MessageParcel dataParcel;
    MessageParcel replyParcel;
    if (!dataParcel.WriteInterfaceToken(CallManagerServiceProxy::GetDescriptor())) {
        TELEPHONY_LOGE("write descriptor fail");
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    auto remote = Remote();
    if (remote == nullptr) {
        TELEPHONY_LOGE("function Remote() return nullptr!");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    dataParcel.WriteInt32(slotId);
    dataParcel.WriteInt32(static_cast<int32_t>(type));
    int32_t error =
        remote->SendRequest(CallManagerSurfaceCode::INTERFACE_GET_CALL_RESTRICTION, dataParcel, replyParcel, option);
    if (error != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("Function GetCallRestriction! errCode:%{public}d", error);
        return error;
    }
    return replyParcel.ReadInt32();
}

int32_t CallManagerServiceProxy::SetCallRestriction(int32_t slotId, CallRestrictionInfo &info)
{
    MessageOption option;
    MessageParcel dataParcel;
    MessageParcel replyParcel;
    if (!dataParcel.WriteInterfaceToken(CallManagerServiceProxy::GetDescriptor())) {
        TELEPHONY_LOGE("write descriptor fail");
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    auto remote = Remote();
    if (remote == nullptr) {
        TELEPHONY_LOGE("function Remote() return nullptr!");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    dataParcel.WriteInt32(slotId);
    dataParcel.WriteRawData((const void *)&info, sizeof(CallRestrictionInfo));
    int32_t error =
        remote->SendRequest(CallManagerSurfaceCode::INTERFACE_SET_CALL_RESTRICTION, dataParcel, replyParcel, option);
    if (error != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("Function SetCallRestriction! errCode:%{public}d", error);
        return error;
    }
    return replyParcel.ReadInt32();
}

int32_t CallManagerServiceProxy::GetCallTransferInfo(int32_t slotId, CallTransferType type)
{
    MessageOption option;
    MessageParcel dataParcel;
    MessageParcel replyParcel;
    if (!dataParcel.WriteInterfaceToken(CallManagerServiceProxy::GetDescriptor())) {
        TELEPHONY_LOGE("write descriptor fail");
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    auto remote = Remote();
    if (remote == nullptr) {
        TELEPHONY_LOGE("function Remote() return nullptr!");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    dataParcel.WriteInt32(slotId);
    dataParcel.WriteInt32(static_cast<int32_t>(type));
    int32_t error =
        remote->SendRequest(CallManagerSurfaceCode::INTERFACE_GET_CALL_TRANSFER, dataParcel, replyParcel, option);
    if (error != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("Function GetCallTransfer! errCode:%{public}d", error);
        return error;
    }
    return replyParcel.ReadInt32();
}

int32_t CallManagerServiceProxy::SetCallTransferInfo(int32_t slotId, CallTransferInfo &info)
{
    MessageOption option;
    MessageParcel dataParcel;
    MessageParcel replyParcel;
    if (!dataParcel.WriteInterfaceToken(CallManagerServiceProxy::GetDescriptor())) {
        TELEPHONY_LOGE("write descriptor fail");
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    auto remote = Remote();
    if (remote == nullptr) {
        TELEPHONY_LOGE("function Remote() return nullptr!");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    dataParcel.WriteInt32(slotId);
    dataParcel.WriteRawData((const void *)&info, sizeof(CallTransferInfo));
    int32_t error =
        remote->SendRequest(CallManagerSurfaceCode::INTERFACE_SET_CALL_TRANSFER, dataParcel, replyParcel, option);
    if (error != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("Function SetCallTransfer! errCode:%{public}d", error);
        return error;
    }
    return replyParcel.ReadInt32();
}

int32_t CallManagerServiceProxy::SetCallPreferenceMode(int32_t slotId, int32_t mode)
{
    MessageOption option;
    MessageParcel dataParcel;
    MessageParcel replyParcel;
    if (!dataParcel.WriteInterfaceToken(CallManagerServiceProxy::GetDescriptor())) {
        TELEPHONY_LOGE("write descriptor fail");
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    auto remote = Remote();
    if (remote == nullptr) {
        TELEPHONY_LOGE("function Remote() return nullptr!");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    dataParcel.WriteInt32(slotId);
    dataParcel.WriteInt32(mode);
    int32_t error =
        remote->SendRequest(CallManagerSurfaceCode::INTERFACE_SETCALL_PREFERENCEMODE, dataParcel, replyParcel, option);
    if (error != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("Function SetCallPreferenceMode! errCode:%{public}d", error);
        return error;
    }
    return replyParcel.ReadInt32();
}

int32_t CallManagerServiceProxy::StartRtt(int32_t callId, std::u16string &msg)
{
    MessageOption option;
    MessageParcel dataParcel;
    MessageParcel replyParcel;
    if (!dataParcel.WriteInterfaceToken(CallManagerServiceProxy::GetDescriptor())) {
        TELEPHONY_LOGE("write descriptor fail");
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    if (msg.empty()) {
        TELEPHONY_LOGE("msg is empty");
        return TELEPHONY_ERR_ARGUMENT_INVALID;
    }
    auto remote = Remote();
    if (remote == nullptr) {
        TELEPHONY_LOGE("function Remote() return nullptr!");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    dataParcel.WriteInt32(callId);
    dataParcel.WriteString16(msg);
    int32_t error = remote->SendRequest(CallManagerSurfaceCode::INTERFACE_START_RTT, dataParcel, replyParcel, option);
    if (error != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("Function StartRtt errCode:%{public}d", error);
        return error;
    }
    return replyParcel.ReadInt32();
}

int32_t CallManagerServiceProxy::StopRtt(int32_t callId)
{
    MessageOption option;
    MessageParcel dataParcel;
    MessageParcel replyParcel;
    if (!dataParcel.WriteInterfaceToken(CallManagerServiceProxy::GetDescriptor())) {
        TELEPHONY_LOGE("write descriptor fail");
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    auto remote = Remote();
    if (remote == nullptr) {
        TELEPHONY_LOGE("function Remote() return nullptr!");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    dataParcel.WriteInt32(callId);
    int32_t error = remote->SendRequest(CallManagerSurfaceCode::INTERFACE_STOP_RTT, dataParcel, replyParcel, option);
    if (error != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("Function StopRtt errCode:%{public}d", error);
        return error;
    }
    return replyParcel.ReadInt32();
}

int32_t CallManagerServiceProxy::CombineConference(int32_t mainCallId)
{
    MessageOption option;
    MessageParcel dataParcel;
    MessageParcel replyParcel;
    if (!dataParcel.WriteInterfaceToken(CallManagerServiceProxy::GetDescriptor())) {
        TELEPHONY_LOGE("write descriptor fail");
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    auto remote = Remote();
    if (remote == nullptr) {
        TELEPHONY_LOGE("function Remote() return nullptr!");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    dataParcel.WriteInt32(mainCallId);
    int32_t error = remote->SendRequest(INTERFACE_COMBINE_CONFERENCE, dataParcel, replyParcel, option);
    if (error != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("Function CombineConference failed! errCode:%{public}d", error);
        return error;
    }
    return replyParcel.ReadInt32();
}

int32_t CallManagerServiceProxy::SeparateConference(int32_t callId)
{
    MessageOption option;
    MessageParcel dataParcel;
    MessageParcel replyParcel;
    if (!dataParcel.WriteInterfaceToken(CallManagerServiceProxy::GetDescriptor())) {
        TELEPHONY_LOGE("write descriptor fail");
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    dataParcel.WriteInt32(callId);
    auto remote = Remote();
    if (remote == nullptr) {
        TELEPHONY_LOGE("function Remote() return nullptr!");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    int32_t error = remote->SendRequest(INTERFACE_SEPARATE_CONFERENCE, dataParcel, replyParcel, option);
    if (error != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("Function SeparateConference call failed! errCode:%{public}d", error);
        return error;
    }
    return replyParcel.ReadInt32();
}

int32_t CallManagerServiceProxy::ControlCamera(std::u16string cameraId)
{
    MessageOption option;
    MessageParcel dataParcel;
    MessageParcel replyParcel;
    if (!dataParcel.WriteInterfaceToken(CallManagerServiceProxy::GetDescriptor())) {
        TELEPHONY_LOGE("write descriptor fail");
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    dataParcel.WriteString16(cameraId);
    auto remote = Remote();
    if (remote == nullptr) {
        TELEPHONY_LOGE("function Remote() return nullptr!");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    int32_t error = remote->SendRequest(INTERFACE_CTRL_CAMERA, dataParcel, replyParcel, option);
    if (error != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("Function CtrlCamera call failed! errCode:%{public}d", error);
        return error;
    }
    return replyParcel.ReadInt32();
}

int32_t CallManagerServiceProxy::SetPreviewWindow(VideoWindow &window)
{
    MessageOption option;
    MessageParcel dataParcel;
    MessageParcel replyParcel;
    if (!dataParcel.WriteInterfaceToken(CallManagerServiceProxy::GetDescriptor())) {
        TELEPHONY_LOGE("write descriptor fail");
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    dataParcel.WriteRawData((const void *)&window, sizeof(VideoWindow));
    auto remote = Remote();
    if (remote == nullptr) {
        TELEPHONY_LOGE("function Remote() return nullptr!");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    int32_t error = remote->SendRequest(INTERFACE_SET_PREVIEW_WINDOW, dataParcel, replyParcel, option);
    if (error != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("Function SetPreviewWindow call failed! errCode:%{public}d", error);
        return error;
    }
    return replyParcel.ReadInt32();
}

int32_t CallManagerServiceProxy::SetDisplayWindow(VideoWindow &window)
{
    MessageOption option;
    MessageParcel dataParcel;
    MessageParcel replyParcel;
    if (!dataParcel.WriteInterfaceToken(CallManagerServiceProxy::GetDescriptor())) {
        TELEPHONY_LOGE("write descriptor fail");
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    dataParcel.WriteRawData((const void *)&window, sizeof(VideoWindow));
    auto remote = Remote();
    if (remote == nullptr) {
        TELEPHONY_LOGE("function Remote() return nullptr!");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    int32_t error = remote->SendRequest(INTERFACE_SET_DISPLAY_WINDOW, dataParcel, replyParcel, option);
    if (error != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("Function SetDisplayWindow call failed! errCode:%{public}d", error);
        return error;
    }
    return replyParcel.ReadInt32();
}

int32_t CallManagerServiceProxy::SetCameraZoom(float zoomRatio)
{
    MessageOption option;
    MessageParcel dataParcel;
    MessageParcel replyParcel;
    if (!dataParcel.WriteInterfaceToken(CallManagerServiceProxy::GetDescriptor())) {
        TELEPHONY_LOGE("write descriptor fail");
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    dataParcel.WriteFloat(zoomRatio);
    auto remote = Remote();
    if (remote == nullptr) {
        TELEPHONY_LOGE("function Remote() return nullptr!");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    int32_t error = remote->SendRequest(INTERFACE_SET_CAMERA_ZOOM, dataParcel, replyParcel, option);
    if (error != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("Function SetCameraZoom call failed! errCode:%{public}d", error);
        return error;
    }
    return replyParcel.ReadInt32();
}

int32_t CallManagerServiceProxy::SetPausePicture(std::u16string path)
{
    MessageOption option;
    MessageParcel dataParcel;
    MessageParcel replyParcel;
    if (!dataParcel.WriteInterfaceToken(CallManagerServiceProxy::GetDescriptor())) {
        TELEPHONY_LOGE("write descriptor fail");
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    if (path.empty()) {
        TELEPHONY_LOGE("path is empty");
        return TELEPHONY_ERR_ARGUMENT_INVALID;
    }
    dataParcel.WriteString16(path);
    auto remote = Remote();
    if (remote == nullptr) {
        TELEPHONY_LOGE("function Remote() return nullptr!");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    int32_t error = remote->SendRequest(INTERFACE_SET_PAUSE_IMAGE, dataParcel, replyParcel, option);
    if (error != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("Function SetPausePicture call failed! errCode:%{public}d", error);
        return error;
    }
    return replyParcel.ReadInt32();
}

int32_t CallManagerServiceProxy::SetDeviceDirection(int32_t rotation)
{
    MessageOption option;
    MessageParcel dataParcel;
    MessageParcel replyParcel;
    if (!dataParcel.WriteInterfaceToken(CallManagerServiceProxy::GetDescriptor())) {
        TELEPHONY_LOGE("write descriptor fail");
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    dataParcel.WriteInt32(rotation);
    auto remote = Remote();
    if (remote == nullptr) {
        TELEPHONY_LOGE("function Remote() return nullptr!");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    int32_t error = remote->SendRequest(INTERFACE_SET_DEVICE_DIRECTION, dataParcel, replyParcel, option);
    if (error != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("Function SetDeviceDirection call failed! errCode:%{public}d", error);
        return error;
    }
    return replyParcel.ReadInt32();
}

bool CallManagerServiceProxy::IsEmergencyPhoneNumber(std::u16string &number, int32_t slotId, int32_t &errorCode)
{
    MessageOption option;
    MessageParcel dataParcel;
    MessageParcel replyParcel;
    if (!dataParcel.WriteInterfaceToken(CallManagerServiceProxy::GetDescriptor())) {
        TELEPHONY_LOGE("write descriptor fail");
        return false;
    }
    if (number.empty()) {
        TELEPHONY_LOGE("number is empty");
        return false;
    }
    dataParcel.WriteString16(number);
    dataParcel.WriteInt32(slotId);
    auto remote = Remote();
    if (remote == nullptr) {
        TELEPHONY_LOGE("function Remote() return nullptr!");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    int32_t error = remote->SendRequest(INTERFACE_IS_EMERGENCY_NUMBER, dataParcel, replyParcel, option);
    if (error != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("Function IsEmergencyPhoneNumber call failed! errCode:%{public}d", error);
        return false;
    }
    bool result = replyParcel.ReadBool();
    errorCode = replyParcel.ReadInt32();
    return result;
}

int32_t CallManagerServiceProxy::FormatPhoneNumber(
    std::u16string &number, std::u16string &countryCode, std::u16string &formatNumber)
{
    MessageOption option;
    MessageParcel dataParcel;
    MessageParcel replyParcel;
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
    auto remote = Remote();
    if (remote == nullptr) {
        TELEPHONY_LOGE("function Remote() return nullptr!");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    int32_t error = remote->SendRequest(INTERFACE_IS_FORMAT_NUMBER, dataParcel, replyParcel, option);
    if (error != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("Function FormatPhoneNumber call failed! errCode:%{public}d", error);
        return error;
    }
    formatNumber = replyParcel.ReadString16();
    return replyParcel.ReadInt32();
}

int32_t CallManagerServiceProxy::FormatPhoneNumberToE164(
    std::u16string &number, std::u16string &countryCode, std::u16string &formatNumber)
{
    MessageOption option;
    MessageParcel dataParcel;
    MessageParcel replyParcel;
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
    auto remote = Remote();
    if (remote == nullptr) {
        TELEPHONY_LOGE("function Remote() return nullptr!");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    int32_t error = remote->SendRequest(INTERFACE_IS_FORMAT_NUMBER_E164, dataParcel, replyParcel, option);
    if (error != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("Function FormatPhoneNumberToE164 call failed! errCode:%{public}d", error);
        return error;
    }
    formatNumber = replyParcel.ReadString16();
    return replyParcel.ReadInt32();
}

int32_t CallManagerServiceProxy::GetMainCallId(int32_t callId)
{
    MessageOption option;
    MessageParcel dataParcel;
    MessageParcel replyParcel;
    if (!dataParcel.WriteInterfaceToken(CallManagerServiceProxy::GetDescriptor())) {
        TELEPHONY_LOGE("write descriptor fail");
        return TELEPHONY_ERROR;
    }
    dataParcel.WriteInt32(callId);
    auto remote = Remote();
    if (remote == nullptr) {
        TELEPHONY_LOGE("function Remote() return nullptr!");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    int32_t error = remote->SendRequest(INTERFACE_GET_MAINID, dataParcel, replyParcel, option);
    if (error != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("Function StartConference call failed! errCode:%{public}d", error);
        return error;
    }
    return replyParcel.ReadInt32();
}

std::vector<std::u16string> CallManagerServiceProxy::GetSubCallIdList(int32_t callId)
{
    std::vector<std::u16string> list;
    MessageOption option;
    MessageParcel dataParcel;
    MessageParcel replyParcel;
    if (!dataParcel.WriteInterfaceToken(CallManagerServiceProxy::GetDescriptor())) {
        TELEPHONY_LOGE("write descriptor fail");
        return list;
    }
    dataParcel.WriteInt32(callId);
    auto remote = Remote();
    if (remote == nullptr) {
        TELEPHONY_LOGE("function Remote() return nullptr!");
        return list;
    }
    int32_t error = remote->SendRequest(INTERFACE_GET_SUBCALL_LIST_ID, dataParcel, replyParcel, option);
    if (error != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("Function GetSubCallIdList call failed! errCode:%{public}d", error);
        return list;
    }
    replyParcel.ReadString16Vector(&list);
    return list;
}

std::vector<std::u16string> CallManagerServiceProxy::GetCallIdListForConference(int32_t callId)
{
    std::vector<std::u16string> list;
    MessageOption option;
    MessageParcel dataParcel;
    MessageParcel replyParcel;
    if (!dataParcel.WriteInterfaceToken(CallManagerServiceProxy::GetDescriptor())) {
        TELEPHONY_LOGE("write descriptor fail");
        return list;
    }
    dataParcel.WriteInt32(callId);
    auto remote = Remote();
    if (remote == nullptr) {
        TELEPHONY_LOGE("function Remote() return nullptr!");
        return list;
    }
    int32_t error = remote->SendRequest(INTERFACE_GET_CALL_LIST_ID_FOR_CONFERENCE, dataParcel, replyParcel, option);
    if (error != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("Function GetCallIdListForConference call failed! errCode:%{public}d", error);
        return list;
    }
    replyParcel.ReadString16Vector(&list);
    return list;
}

int32_t CallManagerServiceProxy::GetImsConfig(int32_t slotId, ImsConfigItem item)
{
    MessageOption option;
    MessageParcel dataParcel;
    MessageParcel replyParcel;
    if (!dataParcel.WriteInterfaceToken(CallManagerServiceProxy::GetDescriptor())) {
        TELEPHONY_LOGE("write descriptor fail");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    dataParcel.WriteInt32(slotId);
    dataParcel.WriteInt32(item);
    auto remote = Remote();
    if (remote == nullptr) {
        TELEPHONY_LOGE("function Remote() return nullptr!");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    int32_t error = remote->SendRequest(INTERFACE_GET_IMS_CONFIG, dataParcel, replyParcel, option);
    if (error != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("function GetImsConfig failed! errCode:%{public}d", error);
        return error;
    }
    return replyParcel.ReadInt32();
}

int32_t CallManagerServiceProxy::SetImsConfig(int32_t slotId, ImsConfigItem item, std::u16string &value)
{
    MessageOption option;
    MessageParcel dataParcel;
    MessageParcel replyParcel;
    if (!dataParcel.WriteInterfaceToken(CallManagerServiceProxy::GetDescriptor())) {
        TELEPHONY_LOGE("write descriptor fail");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    dataParcel.WriteInt32(slotId);
    dataParcel.WriteInt32(item);
    dataParcel.WriteString16(value);
    auto remote = Remote();
    if (remote == nullptr) {
        TELEPHONY_LOGE("function Remote() return nullptr!");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    int32_t error = remote->SendRequest(INTERFACE_SET_IMS_CONFIG, dataParcel, replyParcel, option);
    if (error != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("function SetImsConfig failed! errCode:%{public}d", error);
        return error;
    }
    return replyParcel.ReadInt32();
}

int32_t CallManagerServiceProxy::GetImsFeatureValue(int32_t slotId, FeatureType type)
{
    MessageOption option;
    MessageParcel dataParcel;
    MessageParcel replyParcel;
    if (!dataParcel.WriteInterfaceToken(CallManagerServiceProxy::GetDescriptor())) {
        TELEPHONY_LOGE("write descriptor fail");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    dataParcel.WriteInt32(slotId);
    dataParcel.WriteInt32(type);
    auto remote = Remote();
    if (remote == nullptr) {
        TELEPHONY_LOGE("function Remote() return nullptr!");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    int32_t error = remote->SendRequest(INTERFACE_GET_IMS_FEATURE_VALUE, dataParcel, replyParcel, option);
    if (error != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("function GetImsFeatureValue failed! errCode:%{public}d", error);
        return error;
    }
    return replyParcel.ReadInt32();
}

int32_t CallManagerServiceProxy::SetImsFeatureValue(int32_t slotId, FeatureType type, int32_t value)
{
    MessageOption option;
    MessageParcel dataParcel;
    MessageParcel replyParcel;
    if (!dataParcel.WriteInterfaceToken(CallManagerServiceProxy::GetDescriptor())) {
        TELEPHONY_LOGE("write descriptor fail");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    dataParcel.WriteInt32(slotId);
    dataParcel.WriteInt32(type);
    dataParcel.WriteInt32(value);
    auto remote = Remote();
    if (remote == nullptr) {
        TELEPHONY_LOGE("function Remote() return nullptr!");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    int32_t error = remote->SendRequest(INTERFACE_SET_IMS_FEATURE_VALUE, dataParcel, replyParcel, option);
    if (error != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("function SetImsFeatureValue failed! errCode:%{public}d", error);
        return error;
    }
    return replyParcel.ReadInt32();
}

int32_t CallManagerServiceProxy::UpdateImsCallMode(int32_t callId, ImsCallMode mode)
{
    MessageOption option;
    MessageParcel dataParcel;
    MessageParcel replyParcel;
    if (!dataParcel.WriteInterfaceToken(CallManagerServiceProxy::GetDescriptor())) {
        TELEPHONY_LOGE("write descriptor fail");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    dataParcel.WriteInt32(callId);
    dataParcel.WriteUint32(mode);
    auto remote = Remote();
    if (remote == nullptr) {
        TELEPHONY_LOGE("function Remote() return nullptr!");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    int32_t error = remote->SendRequest(INTERFACE_UPDATE_CALL_MEDIA_MODE, dataParcel, replyParcel, option);
    if (error != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("function UpdateImsCallMode failed! errCode:%{public}d", error);
        return error;
    }
    return replyParcel.ReadInt32();
}

int32_t CallManagerServiceProxy::EnableImsSwitch(int32_t slotId)
{
    MessageOption option;
    MessageParcel dataParcel;
    MessageParcel replyParcel;
    if (!dataParcel.WriteInterfaceToken(CallManagerServiceProxy::GetDescriptor())) {
        TELEPHONY_LOGE("write descriptor fail");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    dataParcel.WriteInt32(slotId);
    auto remote = Remote();
    if (remote == nullptr) {
        TELEPHONY_LOGE("function Remote() return nullptr!");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    int32_t error = remote->SendRequest(INTERFACE_ENABLE_VOLTE, dataParcel, replyParcel, option);
    if (error != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("function EnableImsSwitch failed! errCode:%{public}d", error);
        return error;
    }
    return replyParcel.ReadInt32();
}

int32_t CallManagerServiceProxy::DisableImsSwitch(int32_t slotId)
{
    MessageOption option;
    MessageParcel dataParcel;
    MessageParcel replyParcel;
    if (!dataParcel.WriteInterfaceToken(CallManagerServiceProxy::GetDescriptor())) {
        TELEPHONY_LOGE("write descriptor fail");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    dataParcel.WriteInt32(slotId);
    auto remote = Remote();
    if (remote == nullptr) {
        TELEPHONY_LOGE("function Remote() return nullptr!");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    int32_t error = remote->SendRequest(INTERFACE_DISABLE_VOLTE, dataParcel, replyParcel, option);
    if (error != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("function DisableImsSwitch failed! errCode:%{public}d", error);
        return error;
    }
    return replyParcel.ReadInt32();
}

int32_t CallManagerServiceProxy::IsImsSwitchEnabled(int32_t slotId)
{
    MessageParcel dataParcel;
    MessageParcel replyParcel;
    MessageOption option;
    if (!dataParcel.WriteInterfaceToken(CallManagerServiceProxy::GetDescriptor())) {
        TELEPHONY_LOGE("write descriptor fail");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    dataParcel.WriteInt32(slotId);
    auto remote = Remote();
    if (remote == nullptr) {
        TELEPHONY_LOGE("function Remote() return nullptr!");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    int32_t error = remote->SendRequest(INTERFACE_IS_VOLTE_ENABLED, dataParcel, replyParcel, option);
    if (error != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("function IsImsSwitchEnabled failed! errCode:%{public}d", error);
        return error;
    }
    return replyParcel.ReadInt32();
}

int32_t CallManagerServiceProxy::JoinConference(int32_t callId, std::vector<std::u16string> &numberList)
{
    MessageOption option;
    MessageParcel dataParcel;
    MessageParcel replyParcel;
    if (!dataParcel.WriteInterfaceToken(CallManagerServiceProxy::GetDescriptor())) {
        TELEPHONY_LOGE("write descriptor fail");
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    dataParcel.WriteInt32(callId);
    dataParcel.WriteString16Vector(numberList);
    auto remote = Remote();
    if (remote == nullptr) {
        TELEPHONY_LOGE("function Remote() return nullptr!");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    int32_t error = remote->SendRequest(INTERFACE_JOIN_CONFERENCE, dataParcel, replyParcel, option);
    if (error != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("function JoinConference failed! errCode:%{public}d", error);
        return error;
    }
    return replyParcel.ReadInt32();
}

int32_t CallManagerServiceProxy::ReportOttCallDetailsInfo(std::vector<OttCallDetailsInfo> &ottVec)
{
    MessageOption option;
    MessageParcel dataParcel;
    MessageParcel replyParcel;
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
    auto remote = Remote();
    if (remote == nullptr) {
        TELEPHONY_LOGE("function Remote() return nullptr!");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    int32_t error = remote->SendRequest(INTERFACE_REPORT_OTT_CALL_DETAIL_INFO, dataParcel, replyParcel, option);
    if (error != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("function ReportOttCallDetailsInfo failed! errCode:%{public}d", error);
        return error;
    }
    return replyParcel.ReadInt32();
}

int32_t CallManagerServiceProxy::ReportOttCallEventInfo(OttCallEventInfo &eventInfo)
{
    MessageOption option;
    MessageParcel dataParcel;
    MessageParcel replyParcel;
    if (!dataParcel.WriteInterfaceToken(CallManagerServiceProxy::GetDescriptor())) {
        TELEPHONY_LOGE("write descriptor fail");
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    dataParcel.WriteRawData((const void *)&eventInfo, sizeof(OttCallEventInfo));
    auto remote = Remote();
    if (remote == nullptr) {
        TELEPHONY_LOGE("function Remote() return nullptr!");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    int32_t error = remote->SendRequest(INTERFACE_REPORT_OTT_CALL_EVENT_INFO, dataParcel, replyParcel, option);
    if (error != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("function ReportOttCallDetailsInfo failed! errCode:%{public}d", error);
        return error;
    }
    return replyParcel.ReadInt32();
}

sptr<IRemoteObject> CallManagerServiceProxy::GetProxyObjectPtr(CallManagerProxyType proxyType)
{
    MessageOption option;
    MessageParcel dataParcel;
    MessageParcel replyParcel;
    if (!dataParcel.WriteInterfaceToken(CallManagerServiceProxy::GetDescriptor())) {
        TELEPHONY_LOGE("write descriptor fail");
        return nullptr;
    }
    dataParcel.WriteInt32(static_cast<int32_t>(proxyType));
    auto remote = Remote();
    if (remote == nullptr) {
        TELEPHONY_LOGE("function Remote() return nullptr!");
        return nullptr;
    }
    int32_t error = remote->SendRequest(INTERFACE_GET_PROXY_OBJECT_PTR, dataParcel, replyParcel, option);
    if (error != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("function GetProxyObjectPtr failed! errCode:%{public}d", error);
        return nullptr;
    }
    return replyParcel.ReadRemoteObject();
}
} // namespace Telephony
} // namespace OHOS
