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
#include "call_manager_service.h"
#include "system_ability_definition.h"

using namespace OHOS::Telephony;
namespace OHOS {
static bool g_isInited = false;
constexpr int32_t SLOT_NUM = 2;
constexpr int32_t ACTIVE_NUM = 2;
constexpr int32_t VEDIO_STATE_NUM = 2;
constexpr int32_t DIAL_SCENE_NUM = 3;
constexpr int32_t DIAL_TYPE_NUM = 3;
constexpr int32_t CALL_TYPE_NUM = 3;

bool IsServiceInited()
{
    if (!g_isInited) {
        DelayedSingleton<CallManagerService>::GetInstance()->OnStart();
        if (DelayedSingleton<CallManagerService>::GetInstance()->GetServiceRunningState() ==
            static_cast<int32_t>(CallManagerService::ServiceRunningState::STATE_RUNNING)) {
            g_isInited = true;
        }
    }
    return g_isInited;
}

bool HasCall(const uint8_t *data, size_t size)
{
    if (!IsServiceInited()) {
        return TELEPHONY_ERROR;
    }
    MessageParcel dataParcel;
    dataParcel.WriteBuffer(data, size);
    dataParcel.RewindRead(0);
    MessageParcel reply;
    return DelayedSingleton<CallManagerService>::GetInstance()->OnHasCall(dataParcel, reply);
}

int32_t GetCallState(const uint8_t *data, size_t size)
{
    if (!IsServiceInited()) {
        return TELEPHONY_ERROR;
    }
    MessageParcel dataParcel;
    dataParcel.WriteBuffer(data, size);
    dataParcel.RewindRead(0);
    MessageParcel reply;
    return DelayedSingleton<CallManagerService>::GetInstance()->OnGetCallState(dataParcel, reply);
}

int32_t GetCallWaiting(const uint8_t *data, size_t size)
{
    if (!IsServiceInited()) {
        return TELEPHONY_ERROR;
    }
    int32_t slotId = static_cast<int32_t>(size % SLOT_NUM);
    MessageParcel dataParcel;
    dataParcel.WriteInt32(slotId);
    size_t dataSize = size - sizeof(int32_t);
    dataParcel.WriteBuffer(data + sizeof(int32_t), dataSize);
    dataParcel.RewindRead(0);
    MessageParcel reply;
    return DelayedSingleton<CallManagerService>::GetInstance()->OnGetCallWaiting(dataParcel, reply);
}

bool IsRinging(const uint8_t *data, size_t size)
{
    if (!IsServiceInited()) {
        return false;
    }
    MessageParcel dataParcel;
    dataParcel.WriteBuffer(data, size);
    dataParcel.RewindRead(0);
    MessageParcel reply;
    return DelayedSingleton<CallManagerService>::GetInstance()->OnIsRinging(dataParcel, reply);
}

bool IsInEmergencyCall(const uint8_t *data, size_t size)
{
    if (!IsServiceInited()) {
        return false;
    }
    MessageParcel dataParcel;
    dataParcel.WriteBuffer(data, size);
    dataParcel.RewindRead(0);
    MessageParcel reply;
    return DelayedSingleton<CallManagerService>::GetInstance()->OnIsInEmergencyCall(dataParcel, reply);
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
    MessageParcel dataParcel;
    dataParcel.WriteString16(numberU16);
    dataParcel.WriteInt32(accountId);
    dataParcel.WriteInt32(videoState);
    dataParcel.WriteInt32(dialScene);
    dataParcel.WriteInt32(dialType);
    dataParcel.WriteInt32(callType);
    dataParcel.RewindRead(0);

    MessageParcel reply;
    return DelayedSingleton<CallManagerService>::GetInstance()->OnDialCall(dataParcel, reply);
}

void RemoveMissedIncomingCallNotification(const uint8_t *data, size_t size)
{
    if (!IsServiceInited()) {
        return;
    }
    MessageParcel dataMessageParcel;
    dataMessageParcel.WriteBuffer(data, size);
    dataMessageParcel.RewindRead(0);
    MessageParcel reply;
    DelayedSingleton<CallManagerService>::GetInstance()->OnRemoveMissedIncomingCallNotification(
        dataMessageParcel, reply);
}

int32_t SetCallWaiting(const uint8_t *data, size_t size)
{
    if (!IsServiceInited()) {
        return TELEPHONY_ERROR;
    }
    int32_t slotId = static_cast<int32_t>(size % SLOT_NUM);
    int32_t activate = static_cast<int32_t>(size % ACTIVE_NUM);
    MessageParcel dataParcel;
    dataParcel.WriteInt32(slotId);
    dataParcel.WriteBool(activate);
    dataParcel.RewindRead(0);
    MessageParcel reply;
    return DelayedSingleton<CallManagerService>::GetInstance()->OnSetCallWaiting(dataParcel, reply);
}

int32_t InputDialerSpecialCode(const uint8_t *data, size_t size)
{
    if (!IsServiceInited()) {
        return TELEPHONY_ERROR;
    }
    std::string specialCode(reinterpret_cast<const char *>(data), size);
    MessageParcel dataParcel;
    dataParcel.WriteString(specialCode);
    dataParcel.RewindRead(0);
    MessageParcel reply;
    return DelayedSingleton<CallManagerService>::GetInstance()->OnInputDialerSpecialCode(dataParcel, reply);
}

void DoSomethingInterestingWithMyAPI(const uint8_t *data, size_t size)
{
    if (data == nullptr || size == 0) {
        return;
    }

    HasCall(data, size);
    GetCallState(data, size);
    GetCallWaiting(data, size);
    IsRinging(data, size);
    IsInEmergencyCall(data, size);
    DialCall(data, size);
    RemoveMissedIncomingCallNotification(data, size);
    SetCallWaiting(data, size);
    InputDialerSpecialCode(data, size);
}
}  // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    OHOS::AddCallTokenFuzzer token;
    /* Run your code on data */
    OHOS::DoSomethingInterestingWithMyAPI(data, size);
    return 0;
}
