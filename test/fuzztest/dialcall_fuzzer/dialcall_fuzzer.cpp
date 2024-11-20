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

#include "dialcall_fuzzer.h"

#include <cstddef>
#include <cstdint>
#define private public
#include "addcalltoken_fuzzer.h"
#include "call_ability_callback.h"

using namespace OHOS::Telephony;
namespace OHOS {
constexpr int32_t SLOT_NUM = 2;
constexpr int32_t ACTIVE_NUM = 2;
constexpr int32_t VEDIO_STATE_NUM = 2;
constexpr int32_t DIAL_SCENE_NUM = 3;
constexpr int32_t DIAL_TYPE_NUM = 3;
constexpr int32_t CALL_TYPE_NUM = 3;
constexpr int32_t CALL_ID_NUM = 10;

void OnRegisterVoipCallManagerCallback(const uint8_t *data, size_t size)
{
    if (!IsServiceInited()) {
        return;
    }
    MessageParcel messageParcel;
    std::unique_ptr<CallAbilityCallback> callbackWrap = std::make_unique<CallAbilityCallback>();
    if (callbackWrap == nullptr) {
        return;
    }
    messageParcel.WriteBuffer(data, size);
    messageParcel.RewindRead(0);
    MessageParcel reply;
    DelayedSingleton<CallManagerService>::GetInstance()->OnRegisterVoipCallManagerCallback(messageParcel, reply);
}

void OnRemoteRequest(const uint8_t *data, size_t size)
{
    if (!IsServiceInited()) {
        return;
    }

    MessageParcel messageParcel;
    if (!messageParcel.WriteInterfaceToken(CallManagerServiceStub::GetDescriptor())) {
        return;
    }
    messageParcel.RewindRead(0);
    uint32_t code = static_cast<uint32_t>(*data);
    MessageParcel reply;
    MessageOption option;
    DelayedSingleton<CallManagerService>::GetInstance()->OnRemoteRequest(code, messageParcel, reply, option);
}

int32_t OnRegisterCallBack(const uint8_t *data, size_t size)
{
    if (!IsServiceInited()) {
        return TELEPHONY_ERROR;
    }
    MessageParcel messageParcel;
    std::unique_ptr<CallAbilityCallback> callbackWrap = std::make_unique<CallAbilityCallback>();
    if (callbackWrap == nullptr) {
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    messageParcel.WriteRemoteObject(callbackWrap.release()->AsObject().GetRefPtr());
    messageParcel.WriteBuffer(data, size);
    messageParcel.RewindRead(0);
    MessageParcel reply;
    return DelayedSingleton<CallManagerService>::GetInstance()->OnRegisterCallBack(messageParcel, reply);
}

bool HasCall(const uint8_t *data, size_t size)
{
    if (!IsServiceInited()) {
        return TELEPHONY_ERROR;
    }
    MessageParcel messageParcel;
    messageParcel.WriteBuffer(data, size);
    messageParcel.RewindRead(0);
    MessageParcel reply;
    return DelayedSingleton<CallManagerService>::GetInstance()->OnHasCall(messageParcel, reply);
}

int32_t GetCallState(const uint8_t *data, size_t size)
{
    if (!IsServiceInited()) {
        return TELEPHONY_ERROR;
    }
    MessageParcel messageParcel;
    messageParcel.WriteBuffer(data, size);
    messageParcel.RewindRead(0);
    MessageParcel reply;
    return DelayedSingleton<CallManagerService>::GetInstance()->OnGetCallState(messageParcel, reply);
}

int32_t GetCallWaiting(const uint8_t *data, size_t size)
{
    if (!IsServiceInited()) {
        return TELEPHONY_ERROR;
    }
    int32_t slotId = static_cast<int32_t>(size % SLOT_NUM);
    MessageParcel messageParcel;
    messageParcel.WriteInt32(slotId);
    messageParcel.WriteBuffer(data, size);
    messageParcel.RewindRead(0);
    MessageParcel reply;
    return DelayedSingleton<CallManagerService>::GetInstance()->OnGetCallWaiting(messageParcel, reply);
}

bool IsRinging(const uint8_t *data, size_t size)
{
    if (!IsServiceInited()) {
        return false;
    }
    MessageParcel messageParcel;
    messageParcel.WriteBuffer(data, size);
    messageParcel.RewindRead(0);
    MessageParcel reply;
    return DelayedSingleton<CallManagerService>::GetInstance()->OnIsRinging(messageParcel, reply);
}

bool IsInEmergencyCall(const uint8_t *data, size_t size)
{
    if (!IsServiceInited()) {
        return false;
    }
    MessageParcel messageParcel;
    messageParcel.WriteBuffer(data, size);
    messageParcel.RewindRead(0);
    MessageParcel reply;
    return DelayedSingleton<CallManagerService>::GetInstance()->OnIsInEmergencyCall(messageParcel, reply);
}

int32_t DialCall(const uint8_t *data, size_t size)
{
    if (!IsServiceInited()) {
        return TELEPHONY_ERROR;
    }
    std::string number(reinterpret_cast<const char *>(data), size);
    auto numberU16 = Str8ToStr16(number);
    int32_t accountId = static_cast<int32_t>(size % SLOT_NUM);
    int32_t videoState = static_cast<int32_t>(size % VEDIO_STATE_NUM);
    int32_t dialScene = static_cast<int32_t>(size % DIAL_SCENE_NUM);
    int32_t dialType = static_cast<int32_t>(size % DIAL_TYPE_NUM);
    int32_t callType = static_cast<int32_t>(size % CALL_TYPE_NUM);
    MessageParcel messageParcel;
    messageParcel.WriteString16(numberU16);
    messageParcel.WriteInt32(accountId);
    messageParcel.WriteInt32(videoState);
    messageParcel.WriteInt32(dialScene);
    messageParcel.WriteInt32(dialType);
    messageParcel.WriteInt32(callType);
    messageParcel.RewindRead(0);

    MessageParcel reply;
    return DelayedSingleton<CallManagerService>::GetInstance()->OnDialCall(messageParcel, reply);
}

void RemoveMissedIncomingCallNotification(const uint8_t *data, size_t size)
{
    if (!IsServiceInited()) {
        return;
    }
    MessageParcel messageParcel;
    messageParcel.WriteBuffer(data, size);
    messageParcel.RewindRead(0);
    MessageParcel reply;
    DelayedSingleton<CallManagerService>::GetInstance()->OnRemoveMissedIncomingCallNotification(
        messageParcel, reply);
}

int32_t SetCallWaiting(const uint8_t *data, size_t size)
{
    if (!IsServiceInited()) {
        return TELEPHONY_ERROR;
    }
    int32_t slotId = static_cast<int32_t>(*data % SLOT_NUM);
    int32_t activate = static_cast<int32_t>(*data % ACTIVE_NUM);
    MessageParcel messageParcel;
    messageParcel.WriteInt32(slotId);
    messageParcel.WriteBool(activate);
    messageParcel.RewindRead(0);
    MessageParcel reply;
    return DelayedSingleton<CallManagerService>::GetInstance()->OnSetCallWaiting(messageParcel, reply);
}

int32_t SetCallRestriction(const uint8_t *data, size_t size)
{
    if (!IsServiceInited()) {
        return TELEPHONY_ERROR;
    }
    CallRestrictionInfo info;
    int32_t slotId = static_cast<int32_t>(size % SLOT_NUM);
    std::string msg(reinterpret_cast<const char *>(data), size);
    int32_t length = msg.length() > kMaxNumberLen ? kMaxNumberLen : msg.length();
    memcpy_s(info.password, kMaxNumberLen, msg.c_str(), length);
    MessageParcel messageParcel;
    messageParcel.WriteInt32(slotId);
    messageParcel.WriteRawData((const void *)&info, sizeof(CallRestrictionInfo));
    messageParcel.RewindRead(0);
    MessageParcel reply;
    return DelayedSingleton<CallManagerService>::GetInstance()->OnSetCallRestriction(messageParcel, reply);
}

int32_t JoinConference(const uint8_t *data, size_t size)
{
    if (!IsServiceInited()) {
        return TELEPHONY_ERROR;
    }
    int32_t callId = static_cast<int32_t>(*data % CALL_ID_NUM);
    std::vector<std::u16string> numberList { u"0000000000" };
    MessageParcel messageParcel;
    messageParcel.WriteInt32(callId);
    messageParcel.WriteString16Vector(numberList);
    messageParcel.RewindRead(0);
    MessageParcel reply;
    return DelayedSingleton<CallManagerService>::GetInstance()->OnJoinConference(messageParcel, reply);
}

int32_t StartRtt(const uint8_t *data, size_t size)
{
    if (!IsServiceInited()) {
        return TELEPHONY_ERROR;
    }

    int32_t callId = static_cast<int32_t>(size % CALL_ID_NUM);
    std::string msg(reinterpret_cast<const char *>(data), size);
    MessageParcel messageParcel;
    messageParcel.WriteInt32(callId);
    messageParcel.WriteString(msg);
    messageParcel.RewindRead(0);
    MessageParcel reply;
    return DelayedSingleton<CallManagerService>::GetInstance()->OnStartRtt(messageParcel, reply);
}

int32_t InputDialerSpecialCode(const uint8_t *data, size_t size)
{
    if (!IsServiceInited()) {
        return TELEPHONY_ERROR;
    }
    std::string specialCode(reinterpret_cast<const char *>(data), size);
    MessageParcel messageParcel;
    messageParcel.WriteString(specialCode);
    messageParcel.RewindRead(0);
    MessageParcel reply;
    return DelayedSingleton<CallManagerService>::GetInstance()->OnInputDialerSpecialCode(messageParcel, reply);
}

int32_t CancelCallUpgrade(const uint8_t *data, size_t size)
{
    if (!IsServiceInited()) {
        return TELEPHONY_ERROR;
    }
    MessageParcel messageParcel;
    MessageParcel replyParcel;
    int32_t callId = static_cast<int32_t>(*data % CALL_ID_NUM);
    messageParcel.WriteInt32(callId);
    messageParcel.RewindRead(0);
    return DelayedSingleton<CallManagerService>::GetInstance()->OnCancelCallUpgrade(messageParcel, replyParcel);
}

int32_t RequestCameraCapabilities(const uint8_t *data, size_t size)
{
    if (!IsServiceInited()) {
        return TELEPHONY_ERROR;
    }
    MessageParcel messageParcel;
    MessageParcel replyParcel;
    int32_t callId = static_cast<int32_t>(*data % CALL_ID_NUM);
    messageParcel.WriteInt32(callId);
    messageParcel.RewindRead(0);
    return DelayedSingleton<CallManagerService>::GetInstance()->OnRequestCameraCapabilities(messageParcel, replyParcel);
}

void DoSomethingInterestingWithMyAPI(const uint8_t *data, size_t size)
{
    if (data == nullptr || size == 0) {
        return;
    }
    OnRegisterVoipCallManagerCallback(data, size);
    OnRemoteRequest(data, size);
    OnRegisterCallBack(data, size);
    HasCall(data, size);
    GetCallState(data, size);
    GetCallWaiting(data, size);
    IsRinging(data, size);
    IsInEmergencyCall(data, size);
    DialCall(data, size);
    RemoveMissedIncomingCallNotification(data, size);
    SetCallWaiting(data, size);
    SetCallRestriction(data, size);
    JoinConference(data, size);
    StartRtt(data, size);
    InputDialerSpecialCode(data, size);
    CancelCallUpgrade(data, size);
    RequestCameraCapabilities(data, size);
}
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    OHOS::AddCallTokenFuzzer token;
    /* Run your code on data */
    OHOS::DoSomethingInterestingWithMyAPI(data, size);
    return 0;
}
