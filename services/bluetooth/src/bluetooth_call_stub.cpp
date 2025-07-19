/*
 * Copyright (C) 2021-2023 Huawei Device Co., Ltd.
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

#include "bluetooth_call_stub.h"

#include <string_ex.h>

#include "call_manager_errors.h"
#include "telephony_log_wrapper.h"

#include "message_option.h"
#include "message_parcel.h"
#include "call_manager_utils.h"

#include "call_control_manager.h"

namespace OHOS {
namespace Telephony {
BluetoothCallStub::BluetoothCallStub()
{
    memberFuncMap_[static_cast<uint32_t>(BluetoothCallInterfaceCode::INTERFACE_BT_ANSWER_CALL)] =
        [this](MessageParcel &data, MessageParcel &reply) { return OnAnswerCall(data, reply); };
    memberFuncMap_[static_cast<uint32_t>(BluetoothCallInterfaceCode::INTERFACE_BT_REJECT_CALL)] =
        [this](MessageParcel &data, MessageParcel &reply) { return OnRejectCall(data, reply); };
    memberFuncMap_[static_cast<uint32_t>(BluetoothCallInterfaceCode::INTERFACE_BT_HOLD_CALL)] =
        [this](MessageParcel &data, MessageParcel &reply) { return OnHoldCall(data, reply); };
    memberFuncMap_[static_cast<uint32_t>(BluetoothCallInterfaceCode::INTERFACE_BT_UNHOLD_CALL)] =
        [this](MessageParcel &data, MessageParcel &reply) { return OnUnHoldCall(data, reply); };
    memberFuncMap_[static_cast<uint32_t>(BluetoothCallInterfaceCode::INTERFACE_BT_DISCONNECT_CALL)] =
        [this](MessageParcel &data, MessageParcel &reply) { return OnHangUpCall(data, reply); };
    memberFuncMap_[static_cast<uint32_t>(BluetoothCallInterfaceCode::INTERFACE_BT_GET_CALL_STATE)] =
        [this](MessageParcel &data, MessageParcel &reply) { return OnGetBtCallState(data, reply); };
    memberFuncMap_[static_cast<uint32_t>(BluetoothCallInterfaceCode::INTERFACE_BT_SWAP_CALL)] =
        [this](MessageParcel &data, MessageParcel &reply) { return OnSwitchCall(data, reply); };
    memberFuncMap_[static_cast<uint32_t>(BluetoothCallInterfaceCode::INTERFACE_BT_COMBINE_CONFERENCE)] =
        [this](MessageParcel &data, MessageParcel &reply) { return OnCombineConference(data, reply); };
    memberFuncMap_[static_cast<uint32_t>(BluetoothCallInterfaceCode::INTERFACE_BT_SEPARATE_CONFERENCE)] =
        [this](MessageParcel &data, MessageParcel &reply) { return OnSeparateConference(data, reply); };
    memberFuncMap_[static_cast<uint32_t>(BluetoothCallInterfaceCode::INTERFACE_BT_KICK_OUT_CONFERENCE)] =
        [this](MessageParcel &data, MessageParcel &reply) { return OnKickOutFromConference(data, reply); };
    memberFuncMap_[static_cast<uint32_t>(BluetoothCallInterfaceCode::INTERFACE_BT_START_DTMF)] =
        [this](MessageParcel &data, MessageParcel &reply) { return OnStartDtmf(data, reply); };
    memberFuncMap_[static_cast<uint32_t>(BluetoothCallInterfaceCode::INTERFACE_BT_STOP_DTMF)] =
        [this](MessageParcel &data, MessageParcel &reply) { return OnStopDtmf(data, reply); };
    memberFuncMap_[static_cast<uint32_t>(BluetoothCallInterfaceCode::INTERFACE_BT_GET_CURRENT_CALL_LIST)] =
        [this](MessageParcel &data, MessageParcel &reply) { return OnGetCurrentCallList(data, reply); };
    memberFuncMap_[static_cast<uint32_t>(BluetoothCallInterfaceCode::INTERFACE_BT_ADD_AUDIO_DEVICE)] =
        [this](MessageParcel &data, MessageParcel &reply) { return OnAddAudioDeviceList(data, reply); };
    memberFuncMap_[static_cast<uint32_t>(BluetoothCallInterfaceCode::INTERFACE_BT_REMOVE_AUDIO_DEVICE)] =
        [this](MessageParcel &data, MessageParcel &reply) { return OnRemoveAudioDeviceList(data, reply); };
    memberFuncMap_[static_cast<uint32_t>(BluetoothCallInterfaceCode::INTERFACE_BT_RESET_NEARLINK_AUDIO_DEVICE)] =
        [this](MessageParcel &data, MessageParcel &reply) { return OnResetNearlinkDeviceList(data, reply); };
    memberFuncMap_[static_cast<uint32_t>(BluetoothCallInterfaceCode::INTERFACE_BT_RESET_BT_HEARINGAID_AUDIO_DEVICE)] =
        [this](MessageParcel &data, MessageParcel &reply) { return OnResetBtHearingAidDeviceList(data, reply); };
}

BluetoothCallStub::~BluetoothCallStub()
{
    memberFuncMap_.clear();
}

int32_t BluetoothCallStub::OnRemoteRequest(uint32_t code, MessageParcel &data, MessageParcel &reply,
    MessageOption &option)
{
    std::u16string myDescriptor = BluetoothCallStub::GetDescriptor();
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
            return memberFunc(data, reply);
        }
    }
    return IPCObjectStub::OnRemoteRequest(code, data, reply, option);
}

int32_t BluetoothCallStub::OnAnswerCall(MessageParcel &data, MessageParcel &reply)
{
    int32_t result = AnswerCall();
    TELEPHONY_LOGI("result:%{public}d", result);
    if (!reply.WriteInt32(result)) {
        TELEPHONY_LOGE("fail to write parcel");
        return TELEPHONY_ERR_WRITE_REPLY_FAIL;
    }
    return result;
}

int32_t BluetoothCallStub::OnRejectCall(MessageParcel &data, MessageParcel &reply)
{
    int32_t result = RejectCall();
    TELEPHONY_LOGI("result:%{public}d", result);
    if (!reply.WriteInt32(result)) {
        TELEPHONY_LOGE("fail to write parcel");
        return TELEPHONY_ERR_WRITE_REPLY_FAIL;
    }
    return result;
}

int32_t BluetoothCallStub::OnHangUpCall(MessageParcel &data, MessageParcel &reply)
{
    int32_t result = HangUpCall();
    TELEPHONY_LOGI("result:%{public}d", result);
    if (!reply.WriteInt32(result)) {
        TELEPHONY_LOGE("fail to write parcel");
        return TELEPHONY_ERR_WRITE_REPLY_FAIL;
    }
    return result;
}

int32_t BluetoothCallStub::OnGetBtCallState(MessageParcel &data, MessageParcel &reply)
{
    int32_t result = GetCallState();
    TELEPHONY_LOGI("result:%{public}d", result);
    if (!reply.WriteInt32(result)) {
        TELEPHONY_LOGE("fail to write parcel");
        return TELEPHONY_ERR_WRITE_REPLY_FAIL;
    }
    return TELEPHONY_SUCCESS;
}

int32_t BluetoothCallStub::OnHoldCall(MessageParcel &data, MessageParcel &reply)
{
    int32_t result = HoldCall();
    TELEPHONY_LOGI("result:%{public}d", result);
    if (!reply.WriteInt32(result)) {
        TELEPHONY_LOGE("fail to write parcel");
        return TELEPHONY_ERR_WRITE_REPLY_FAIL;
    }
    return result;
}

int32_t BluetoothCallStub::OnUnHoldCall(MessageParcel &data, MessageParcel &reply)
{
    int32_t result = UnHoldCall();
    TELEPHONY_LOGI("result:%{public}d", result);
    if (!reply.WriteInt32(result)) {
        TELEPHONY_LOGE("fail to write parcel");
        return TELEPHONY_ERR_WRITE_REPLY_FAIL;
    }
    return result;
}

int32_t BluetoothCallStub::OnSwitchCall(MessageParcel &data, MessageParcel &reply)
{
    int32_t result = SwitchCall();
    TELEPHONY_LOGI("result:%{public}d", result);
    if (!reply.WriteInt32(result)) {
        TELEPHONY_LOGE("fail to write parcel");
        return TELEPHONY_ERR_WRITE_REPLY_FAIL;
    }
    return result;
}

int32_t BluetoothCallStub::OnCombineConference(MessageParcel &data, MessageParcel &reply)
{
    int32_t result = CombineConference();
    TELEPHONY_LOGI("result:%{public}d", result);
    if (!reply.WriteInt32(result)) {
        TELEPHONY_LOGE("fail to write parcel");
        return TELEPHONY_ERR_WRITE_REPLY_FAIL;
    }
    return TELEPHONY_SUCCESS;
}

int32_t BluetoothCallStub::OnSeparateConference(MessageParcel &data, MessageParcel &reply)
{
    int32_t result = SeparateConference();
    TELEPHONY_LOGI("result:%{public}d", result);
    if (!reply.WriteInt32(result)) {
        TELEPHONY_LOGE("fail to write parcel");
        return TELEPHONY_ERR_WRITE_REPLY_FAIL;
    }
    return TELEPHONY_SUCCESS;
}

int32_t BluetoothCallStub::OnKickOutFromConference(MessageParcel &data, MessageParcel &reply)
{
    int32_t result = KickOutFromConference();
    TELEPHONY_LOGI("result:%{public}d", result);
    if (!reply.WriteInt32(result)) {
        TELEPHONY_LOGE("fail to write parcel");
        return TELEPHONY_ERR_WRITE_REPLY_FAIL;
    }
    return TELEPHONY_SUCCESS;
}

int32_t BluetoothCallStub::OnStartDtmf(MessageParcel &data, MessageParcel &reply)
{
    int32_t result = TELEPHONY_ERR_FAIL;
    char str = static_cast<char>(data.ReadInt8());
    result = StartDtmf(str);
    TELEPHONY_LOGI("result:%{public}d", result);
    if (!reply.WriteInt32(result)) {
        TELEPHONY_LOGE("fail to write parcel");
        return TELEPHONY_ERR_WRITE_REPLY_FAIL;
    }
    return TELEPHONY_SUCCESS;
}

int32_t BluetoothCallStub::OnStopDtmf(MessageParcel &data, MessageParcel &reply)
{
    int32_t result = TELEPHONY_ERR_FAIL;
    result = StopDtmf();
    TELEPHONY_LOGI("result:%{public}d", result);
    if (!reply.WriteInt32(result)) {
        TELEPHONY_LOGE("fail to write parcel");
        return TELEPHONY_ERR_WRITE_REPLY_FAIL;
    }
    return TELEPHONY_SUCCESS;
}

int32_t BluetoothCallStub::OnGetCurrentCallList(MessageParcel &data, MessageParcel &reply)
{
    int32_t slotId = data.ReadInt32();
    std::vector<CallAttributeInfo> callVec = GetCurrentCallList(slotId);
    reply.WriteInt32(callVec.size());
    std::vector<CallAttributeInfo>::iterator it = callVec.begin();
    for (; it != callVec.end(); ++it) {
        CallManagerUtils::WriteCallAttributeInfo(*it, reply);
    }
    return TELEPHONY_SUCCESS;
}

int32_t BluetoothCallStub::OnAddAudioDeviceList(MessageParcel &data, MessageParcel &reply)
{
    std::string address = data.ReadString();
    int32_t deviceType = data.ReadInt32();
    std::string name = data.ReadString();
    int32_t result = AddAudioDeviceList(address, deviceType, name);
    TELEPHONY_LOGI("result:%{public}d", result);
    if (!reply.WriteInt32(result)) {
        TELEPHONY_LOGE("fail to write parcel");
        return TELEPHONY_ERR_WRITE_REPLY_FAIL;
    }
    return result;
}

int32_t BluetoothCallStub::OnRemoveAudioDeviceList(MessageParcel &data, MessageParcel &reply)
{
    std::string address = data.ReadString();
    int32_t deviceType = data.ReadInt32();
    int32_t result = RemoveAudioDeviceList(address, deviceType);
    TELEPHONY_LOGI("result:%{public}d", result);
    if (!reply.WriteInt32(result)) {
        TELEPHONY_LOGE("fail to write parcel");
        return TELEPHONY_ERR_WRITE_REPLY_FAIL;
    }
    return result;
}

int32_t BluetoothCallStub::OnResetNearlinkDeviceList(MessageParcel &data, MessageParcel &reply)
{
    int32_t result = ResetNearlinkDeviceList();
    TELEPHONY_LOGI("result:%{public}d", result);
    if (!reply.WriteInt32(result)) {
        TELEPHONY_LOGE("fail to write parcel");
        return TELEPHONY_ERR_WRITE_REPLY_FAIL;
    }
    return result;
}

int32_t BluetoothCallStub::OnResetBtHearingAidDeviceList(MessageParcel &data, MessageParcel &reply)
{
    int32_t result = ResetBtHearingAidDeviceList();
    TELEPHONY_LOGI("result:%{public}d", result);
    if (!reply.WriteInt32(result)) {
        TELEPHONY_LOGE("fail to write parcel");
        return TELEPHONY_ERR_WRITE_REPLY_FAIL;
    }
    return result;
}
} // namespace Telephony
} // namespace OHOS
