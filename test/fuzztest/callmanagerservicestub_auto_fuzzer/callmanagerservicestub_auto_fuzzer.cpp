/*
 * Copyright (C) 2026 Huawei Device Co., Ltd.
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

#include "callmanagerservicestub_auto_fuzzer.h"

#include <cstddef>
#include <cstdint>
#include <securec.h>
#include <fuzzer/FuzzedDataProvider.h>
#define private public
#include "addcalltoken_fuzzer.h"
#include "call_manager_service_stub.h"
#include "call_ability_callback.h"

using namespace OHOS::Telephony;
namespace OHOS {
constexpr int32_t SLOT_NUM = 2;
constexpr int32_t VIDEO_STATE_NUM = 2;
constexpr int32_t DIAL_SCENE_NUM = 3;
constexpr int32_t DIAL_TYPE_NUM = 3;
constexpr int32_t CALL_TYPE_NUM = 5;
constexpr int32_t REJECT_TYPE_NUM = 4;

static void DoOnRegisterCallBack(FuzzedDataProvider &provider)
{
    MessageParcel messageParcel;
    if (provider.ConsumeBool()) {
        sptr<CallAbilityCallback> callback = new CallAbilityCallback();
        messageParcel.WriteRemoteObject(callback->AsObject());
    } else {
        messageParcel.WriteRemoteObject(nullptr);
    }
    MessageParcel reply;
    DelayedSingleton<CallManagerService>::GetInstance()->OnRegisterCallBack(messageParcel, reply);
}

static void DoOnDialCall(FuzzedDataProvider &provider)
{
    std::string number = provider.ConsumeRandomLengthString();
    std::u16string numberU16 = Str8ToStr16(number);
    int32_t accountId = provider.ConsumeIntegral<int32_t>() % SLOT_NUM;
    int32_t videoState = provider.ConsumeIntegral<int32_t>() % VIDEO_STATE_NUM;
    int32_t dialScene = provider.ConsumeIntegral<int32_t>() % DIAL_SCENE_NUM;
    int32_t dialType = provider.ConsumeIntegral<int32_t>() % DIAL_TYPE_NUM;
    int32_t callType = provider.ConsumeIntegral<int32_t>() % CALL_TYPE_NUM;
    bool isRtt = provider.ConsumeBool();
    int32_t phoneIndex = provider.ConsumeIntegral<int32_t>() % SLOT_NUM;
    std::string extraParams = provider.ConsumeRandomLengthString();
    std::string bundleName = provider.ConsumeRandomLengthString();
    bool btSlotIdUnknown = provider.ConsumeBool();
    int32_t xCallType = provider.ConsumeIntegral<int32_t>() % CALL_TYPE_NUM;
    MessageParcel messageParcel;
    messageParcel.WriteString16(numberU16);
    messageParcel.WriteInt32(accountId);
    messageParcel.WriteInt32(videoState);
    messageParcel.WriteInt32(dialScene);
    messageParcel.WriteInt32(dialType);
    messageParcel.WriteInt32(callType);
    messageParcel.WriteBool(isRtt);
    messageParcel.WriteInt32(phoneIndex);
    messageParcel.WriteString(extraParams);
    messageParcel.WriteString(bundleName);
    messageParcel.WriteBool(btSlotIdUnknown);
    messageParcel.WriteInt32(xCallType);
    MessageParcel reply;
    DelayedSingleton<CallManagerService>::GetInstance()->OnDialCall(messageParcel, reply);
}

static void DoOnMakeCall(FuzzedDataProvider &provider)
{
    std::string number = provider.ConsumeRandomLengthString();
    MessageParcel messageParcel;
    messageParcel.WriteString(number);
    MessageParcel reply;
    DelayedSingleton<CallManagerService>::GetInstance()->OnMakeCall(messageParcel, reply);
}

static void DoOnMakeDirectCall(FuzzedDataProvider &provider)
{
    std::string number = provider.ConsumeRandomLengthString();
    MessageParcel messageParcel;
    messageParcel.WriteString(number);
    MessageParcel reply;
    DelayedSingleton<CallManagerService>::GetInstance()->OnMakeDirectCall(messageParcel, reply);
}

static void DoOnMakeCallWithToken(FuzzedDataProvider &provider)
{
    std::string number = provider.ConsumeRandomLengthString();
    bool isCustomAccessibility = provider.ConsumeBool();
    MessageParcel messageParcel;
    messageParcel.WriteString(number);
    messageParcel.WriteBool(isCustomAccessibility);
    MessageParcel reply;
    DelayedSingleton<CallManagerService>::GetInstance()->OnMakeCallWithToken(messageParcel, reply);
}

static void DoOnAcceptCall(FuzzedDataProvider &provider)
{
    int32_t callId = provider.ConsumeIntegral<int32_t>();
    int32_t videoState = provider.ConsumeIntegral<int32_t>() % VIDEO_STATE_NUM;
    bool isRtt = provider.ConsumeBool();
    MessageParcel messageParcel;
    messageParcel.WriteInt32(callId);
    messageParcel.WriteInt32(videoState);
    messageParcel.WriteBool(isRtt);
    MessageParcel reply;
    DelayedSingleton<CallManagerService>::GetInstance()->OnAcceptCall(messageParcel, reply);
}

static void DoOnRejectCall(FuzzedDataProvider &provider)
{
    int32_t callId = provider.ConsumeIntegral<int32_t>();
    bool isSendSms = provider.ConsumeBool();
    std::string content = provider.ConsumeRandomLengthString();
    MessageParcel messageParcel;
    messageParcel.WriteInt32(callId);
    messageParcel.WriteBool(isSendSms);
    messageParcel.WriteString16(Str8ToStr16(content));
    MessageParcel reply;
    DelayedSingleton<CallManagerService>::GetInstance()->OnRejectCall(messageParcel, reply);
}

static void DoOnHangUpCall(FuzzedDataProvider &provider)
{
    int32_t callId = provider.ConsumeIntegral<int32_t>();
    MessageParcel messageParcel;
    messageParcel.WriteInt32(callId);
    MessageParcel reply;
    DelayedSingleton<CallManagerService>::GetInstance()->OnHangUpCall(messageParcel, reply);
}

static void DoOnHoldCall(FuzzedDataProvider &provider)
{
    int32_t callId = provider.ConsumeIntegral<int32_t>();
    MessageParcel messageParcel;
    messageParcel.WriteInt32(callId);
    MessageParcel reply;
    DelayedSingleton<CallManagerService>::GetInstance()->OnHoldCall(messageParcel, reply);
}

static void DoOnUnHoldCall(FuzzedDataProvider &provider)
{
    int32_t callId = provider.ConsumeIntegral<int32_t>();
    MessageParcel messageParcel;
    messageParcel.WriteInt32(callId);
    MessageParcel reply;
    DelayedSingleton<CallManagerService>::GetInstance()->OnUnHoldCall(messageParcel, reply);
}

static void DoOnSwitchCall(FuzzedDataProvider &provider)
{
    int32_t callId = provider.ConsumeIntegral<int32_t>();
    MessageParcel messageParcel;
    messageParcel.WriteInt32(callId);
    MessageParcel reply;
    DelayedSingleton<CallManagerService>::GetInstance()->OnSwitchCall(messageParcel, reply);
}

static void DoOnRejectCallWithType(FuzzedDataProvider &provider)
{
    int32_t rejectType = provider.ConsumeIntegral<int32_t>() % REJECT_TYPE_NUM;
    MessageParcel messageParcel;
    messageParcel.WriteInt32(rejectType);
    MessageParcel reply;
    DelayedSingleton<CallManagerService>::GetInstance()->OnRejectCallWithType(messageParcel, reply);
}

static void DoOnHasCall(FuzzedDataProvider &provider)
{
    bool isIncludeVoipCall = provider.ConsumeBool();
    MessageParcel messageParcel;
    messageParcel.WriteBool(isIncludeVoipCall);
    MessageParcel reply;
    DelayedSingleton<CallManagerService>::GetInstance()->OnHasCall(messageParcel, reply);
}

static void DoOnSendCallUiEvent(FuzzedDataProvider &provider)
{
    int32_t callId = provider.ConsumeIntegral<int32_t>();
    std::string eventName = provider.ConsumeRandomLengthString();
    MessageParcel messageParcel;
    messageParcel.WriteInt32(callId);
    messageParcel.WriteString(eventName);
    MessageParcel reply;
    DelayedSingleton<CallManagerService>::GetInstance()->OnSendCallUiEvent(messageParcel, reply);
}

static void DoOnPreloadCallUi(FuzzedDataProvider &provider)
{
    bool enable = provider.ConsumeBool();
    MessageParcel messageParcel;
    messageParcel.WriteBool(enable);
    MessageParcel reply;
    DelayedSingleton<CallManagerService>::GetInstance()->OnPreloadCallUi(messageParcel, reply);
}

static void DoOnNotifyVoIPAudioStreamStart(FuzzedDataProvider &provider)
{
    int32_t uid = provider.ConsumeIntegral<int32_t>();
    MessageParcel messageParcel;
    messageParcel.WriteInt32(uid);
    MessageParcel reply;
    DelayedSingleton<CallManagerService>::GetInstance()->OnNotifyVoIPAudioStreamStart(messageParcel, reply);
}

static void DoOnSetCallAudioMode(FuzzedDataProvider &provider)
{
    int32_t mode = provider.ConsumeIntegral<int32_t>();
    int32_t scenarios = provider.ConsumeIntegral<int32_t>();
    MessageParcel messageParcel;
    messageParcel.WriteInt32(mode);
    messageParcel.WriteInt32(scenarios);
    MessageParcel reply;
    DelayedSingleton<CallManagerService>::GetInstance()->OnSetCallAudioMode(messageParcel, reply);
}

static void DoOnInputDialerSpecialCode(FuzzedDataProvider &provider)
{
    std::string specialCode = provider.ConsumeRandomLengthString();
    MessageParcel messageParcel;
    messageParcel.WriteString(specialCode);
    MessageParcel reply;
    DelayedSingleton<CallManagerService>::GetInstance()->OnInputDialerSpecialCode(messageParcel, reply);
}

using FuzzApiHandler = void (*)(FuzzedDataProvider &);
constexpr FuzzApiHandler BASIC_API_HANDLERS[] = {
    DoOnRegisterCallBack,
    DoOnDialCall,
    DoOnMakeCall,
    DoOnMakeDirectCall,
    DoOnMakeCallWithToken,
    DoOnAcceptCall,
    DoOnRejectCall,
    DoOnHangUpCall,
    DoOnHoldCall,
    DoOnUnHoldCall,
    DoOnSwitchCall,
    DoOnRejectCallWithType,
    DoOnHasCall,
    DoOnSendCallUiEvent,
    DoOnPreloadCallUi,
    DoOnNotifyVoIPAudioStreamStart,
    DoOnSetCallAudioMode,
    DoOnInputDialerSpecialCode,
};
constexpr uint32_t BASIC_API_TARGET_COUNT =
    sizeof(BASIC_API_HANDLERS) / sizeof(BASIC_API_HANDLERS[0]);

void DoSomethingInterestingWithMyAPI(const uint8_t *data, size_t size)
{
    if (data == nullptr || size == 0) {
        return;
    }
    if (!IsServiceInited()) {
        return;
    }
    FuzzedDataProvider provider(data, size);
    uint32_t targetIndex = provider.ConsumeIntegralInRange<uint32_t>(0, BASIC_API_TARGET_COUNT - 1);
    BASIC_API_HANDLERS[targetIndex](provider);
}
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size)
{
    OHOS::AddCallTokenFuzzer token;
    /* Run your code on data */
    OHOS::DoSomethingInterestingWithMyAPI(data, size);
    return 0;
}
