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
#include "fuzzer/FuzzedDataProvider.h"

using namespace OHOS::Telephony;
namespace OHOS {
constexpr int32_t SLOT_NUM = 2;
constexpr int32_t ACTIVE_NUM = 2;
constexpr int32_t VEDIO_STATE_NUM = 2;
constexpr int32_t DIAL_SCENE_NUM = 3;
constexpr int32_t DIAL_TYPE_NUM = 3;
constexpr int32_t CALL_TYPE_NUM = 3;
constexpr int32_t CALL_ID_NUM = 10;

void OnRegisterVoipCallManagerCallback(FuzzedDataProvider& provider)
{
    if (!IsServiceInited()) {
        return;
    }
    MessageParcel messageParcel;
    std::unique_ptr<CallAbilityCallback> callbackWrap = std::make_unique<CallAbilityCallback>();
    if (callbackWrap == nullptr) {
        return;
    }
    messageParcel.std::vector<uint8_t> testData = provider.ConsumeRemainingBytes<uint8_t>();
    messageParcel.WriteBuffer(static_cast<void*>(testData.data()), testData.size());
    messageParcel.RewindRead(0);
    MessageParcel reply;
    DelayedSingleton<CallManagerService>::GetInstance()->OnRegisterVoipCallManagerCallback(messageParcel, reply);
    std::shared_ptr<FoldStatusManager> foldStatusManager = DelayedSingleton<FoldStatusManager>::GetInstance();
    foldStatusManager->UnregisterFoldableListener();
    foldStatusManager->RegisterFoldableListener();
    foldStatusManager->RegisterFoldableListener();
    foldStatusManager->UnregisterFoldableListener();
    FoldStatusManager::IsSmallFoldDevice();
}

void OnRemoteRequest(FuzzedDataProvider& provider)
{
    if (!IsServiceInited()) {
        return;
    }

    MessageParcel messageParcel;
    if (!messageParcel.WriteInterfaceToken(CallManagerServiceStub::GetDescriptor())) {
        return;
    }
    messageParcel.RewindRead(0);
    uint32_t code = provider.ConsumeIntegral<uint32_t>();
    MessageParcel reply;
    MessageOption option;
    DelayedSingleton<CallManagerService>::GetInstance()->OnRemoteRequest(code, messageParcel, reply, option);
}

int32_t OnRegisterCallBack(FuzzedDataProvider& provider)
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
    messageParcel.std::vector<uint8_t> testData = provider.ConsumeRemainingBytes<uint8_t>();
    messageParcel.WriteBuffer(static_cast<void*>(testData.data()), testData.size());
    messageParcel.RewindRead(0);
    MessageParcel reply;
    return DelayedSingleton<CallManagerService>::GetInstance()->OnRegisterCallBack(messageParcel, reply);
}

bool HasCall(FuzzedDataProvider& provider)
{
    if (!IsServiceInited()) {
        return TELEPHONY_ERROR;
    }
    MessageParcel messageParcel;
    messageParcel.std::vector<uint8_t> testData = provider.ConsumeRemainingBytes<uint8_t>();
    messageParcel.WriteBuffer(static_cast<void*>(testData.data()), testData.size());
    messageParcel.RewindRead(0);
    MessageParcel reply;
    return DelayedSingleton<CallManagerService>::GetInstance()->OnHasCall(messageParcel, reply);
}

int32_t GetCallState(FuzzedDataProvider& provider)
{
    if (!IsServiceInited()) {
        return TELEPHONY_ERROR;
    }
    MessageParcel messageParcel;
    messageParcel.std::vector<uint8_t> testData = provider.ConsumeRemainingBytes<uint8_t>();
    messageParcel.WriteBuffer(static_cast<void*>(testData.data()), testData.size());
    messageParcel.RewindRead(0);
    MessageParcel reply;
    return DelayedSingleton<CallManagerService>::GetInstance()->OnGetCallState(messageParcel, reply);
}

int32_t GetCallWaiting(FuzzedDataProvider& provider)
{
    if (!IsServiceInited()) {
        return TELEPHONY_ERROR;
    }
    int32_t slotId = provider.ConsumeIntegral<int32_t>() % SLOT_NUM;
    MessageParcel messageParcel;
    messageParcel.WriteInt32(slotId);
    messageParcel.std::vector<uint8_t> testData = provider.ConsumeRemainingBytes<uint8_t>();
    messageParcel.WriteBuffer(static_cast<void*>(testData.data()), testData.size());
    messageParcel.RewindRead(0);
    MessageParcel reply;
    return DelayedSingleton<CallManagerService>::GetInstance()->OnGetCallWaiting(messageParcel, reply);
}

bool IsRinging(FuzzedDataProvider& provider)
{
    if (!IsServiceInited()) {
        return false;
    }
    MessageParcel messageParcel;
    messageParcel.std::vector<uint8_t> testData = provider.ConsumeRemainingBytes<uint8_t>();
    messageParcel.WriteBuffer(static_cast<void*>(testData.data()), testData.size());
    messageParcel.RewindRead(0);
    MessageParcel reply;
    return DelayedSingleton<CallManagerService>::GetInstance()->OnIsRinging(messageParcel, reply);
}

bool IsInEmergencyCall(FuzzedDataProvider& provider)
{
    if (!IsServiceInited()) {
        return false;
    }
    MessageParcel messageParcel;
    messageParcel.std::vector<uint8_t> testData = provider.ConsumeRemainingBytes<uint8_t>();
    messageParcel.WriteBuffer(static_cast<void*>(testData.data()), testData.size());
    messageParcel.RewindRead(0);
    MessageParcel reply;
    return DelayedSingleton<CallManagerService>::GetInstance()->OnIsInEmergencyCall(messageParcel, reply);
}

int32_t DialCall(FuzzedDataProvider& provider)
{
    if (!IsServiceInited()) {
        return TELEPHONY_ERROR;
    }
    std::string number = provider.consumeString();
    auto numberU16 = Str8ToStr16(number);
    int32_t accountId = provider.ConsumeIntegral<int32_t>() % SLOT_NUM;
    int32_t videoState = provider.ConsumeIntegral<int32_t>() % VEDIO_STATE_NUM;
    int32_t dialScene = provider.ConsumeIntegral<int32_t>() % DIAL_SCENE_NUM;
    int32_t dialType = provider.ConsumeIntegral<int32_t>() % DIAL_TYPE_NUM;
    int32_t callType = provider.ConsumeIntegral<int32_t>() % CALL_TYPE_NUM;
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

void RemoveMissedIncomingCallNotification(FuzzedDataProvider& provider)
{
    if (!IsServiceInited()) {
        return;
    }
    MessageParcel messageParcel;
    messageParcel.std::vector<uint8_t> testData = provider.ConsumeRemainingBytes<uint8_t>();
    messageParcel.WriteBuffer(static_cast<void*>(testData.data()), testData.size());
    messageParcel.RewindRead(0);
    MessageParcel reply;
    DelayedSingleton<CallManagerService>::GetInstance()->OnRemoveMissedIncomingCallNotification(
        messageParcel, reply);
}

int32_t SetCallWaiting(FuzzedDataProvider& provider)
{
    if (!IsServiceInited()) {
        return TELEPHONY_ERROR;
    }
    int32_t slotId = provider.ConsumeIntegral<int32_t>() % SLOT_NUM;
    int32_t activate = provider.ConsumeIntegral<int32_t>() % ACTIVE_NUM;
    MessageParcel messageParcel;
    messageParcel.WriteInt32(slotId);
    messageParcel.WriteBool(activate);
    messageParcel.RewindRead(0);
    MessageParcel reply;
    return DelayedSingleton<CallManagerService>::GetInstance()->OnSetCallWaiting(messageParcel, reply);
}

int32_t SetCallRestriction(FuzzedDataProvider& provider)
{
    if (!IsServiceInited()) {
        return TELEPHONY_ERROR;
    }
    CallRestrictionInfo info;
    int32_t slotId = provider.ConsumeIntegral<int32_t>() % SLOT_NUM;
    std::string msg = provider.consumeString();
    int32_t length = msg.length() > kMaxNumberLen ? kMaxNumberLen : msg.length();
    memcpy_s(info.password, kMaxNumberLen, msg.c_str(), length);
    MessageParcel messageParcel;
    messageParcel.WriteInt32(slotId);
    messageParcel.WriteRawData((const void *)&info, sizeof(CallRestrictionInfo));
    messageParcel.RewindRead(0);
    MessageParcel reply;
    return DelayedSingleton<CallManagerService>::GetInstance()->OnSetCallRestriction(messageParcel, reply);
}

int32_t JoinConference(FuzzedDataProvider& provider)
{
    if (!IsServiceInited()) {
        return TELEPHONY_ERROR;
    }
    int32_t callId = provider.ConsumeIntegral<int32_t>() % CALL_ID_NUM;
    std::vector<std::u16string> numberList { u"0000000000" };
    MessageParcel messageParcel;
    messageParcel.WriteInt32(callId);
    messageParcel.WriteString16Vector(numberList);
    messageParcel.RewindRead(0);
    MessageParcel reply;
    return DelayedSingleton<CallManagerService>::GetInstance()->OnJoinConference(messageParcel, reply);
}

#ifdef SUPPORT_RTT_CALL
int32_t StartRtt(FuzzedDataProvider& provider)
{
    if (!IsServiceInited()) {
        return TELEPHONY_ERROR;
    }

    int32_t callId = provider.ConsumeIntegral<int32_t>() % CALL_ID_NUM;
    MessageParcel messageParcel;
    messageParcel.WriteInt32(callId);
    messageParcel.WriteInt32(0);
    messageParcel.RewindRead(0);
    MessageParcel reply;
    return DelayedSingleton<CallManagerService>::GetInstance()->OnUpdateImsRttCallMode(messageParcel, reply);
}
#endif

int32_t InputDialerSpecialCode(FuzzedDataProvider& provider)
{
    if (!IsServiceInited()) {
        return TELEPHONY_ERROR;
    }
    std::string specialCode = provider.consumeString();
    MessageParcel messageParcel;
    messageParcel.WriteString(specialCode);
    messageParcel.RewindRead(0);
    MessageParcel reply;
    return DelayedSingleton<CallManagerService>::GetInstance()->OnInputDialerSpecialCode(messageParcel, reply);
}

int32_t CancelCallUpgrade(FuzzedDataProvider& provider)
{
    if (!IsServiceInited()) {
        return TELEPHONY_ERROR;
    }
    MessageParcel messageParcel;
    MessageParcel replyParcel;
    int32_t callId = provider.ConsumeIntegral<int32_t>() % CALL_ID_NUM;
    messageParcel.WriteInt32(callId);
    messageParcel.RewindRead(0);
    return DelayedSingleton<CallManagerService>::GetInstance()->OnCancelCallUpgrade(messageParcel, replyParcel);
}

int32_t RequestCameraCapabilities(FuzzedDataProvider& provider)
{
    if (!IsServiceInited()) {
        return TELEPHONY_ERROR;
    }
    MessageParcel messageParcel;
    MessageParcel replyParcel;
    int32_t callId = provider.ConsumeIntegral<int32_t>() % CALL_ID_NUM;
    messageParcel.WriteInt32(callId);
    messageParcel.RewindRead(0);
    return DelayedSingleton<CallManagerService>::GetInstance()->OnRequestCameraCapabilities(messageParcel, replyParcel);
}

void DoSomethingInterestingWithMyAPI(const uint8_t *data, size_t size)
{
    if (data == nullptr || size == 0) {
        return;
    }

    FuzzedDataProvider provider(data, size);
    OnRegisterVoipCallManagerCallback(provider);
    OnRemoteRequest(provider);
    OnRegisterCallBack(provider);
    HasCall(provider);
    GetCallState(provider);
    GetCallWaiting(provider);
    IsRinging(provider);
    IsInEmergencyCall(provider);
    DialCall(provider);
    RemoveMissedIncomingCallNotification(provider);
    SetCallWaiting(provider);
    SetCallRestriction(provider);
    JoinConference(provider);
#ifdef SUPPORT_RTT_CALL
    StartRtt(provider);
#endif
    InputDialerSpecialCode(provider);
    CancelCallUpgrade(provider);
    RequestCameraCapabilities(provider);
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
