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

#include "startdtmf_fuzzer.h"

#include <cstddef>
#include <cstdint>
#define private public
#include "addcalltoken_fuzzer.h"
#include "call_manager_service_stub.h"

using namespace OHOS::Telephony;
namespace OHOS {
constexpr int32_t CALL_ID_NUM = 10;

void StartDtmf(const uint8_t *data, size_t size)
{
    if (!IsServiceInited()) {
        return;
    }

    int32_t callId = size % CALL_ID_NUM;
    char str = *reinterpret_cast<const char *>(data);
    MessageParcel dataMessageParcel;
    dataMessageParcel.WriteInt32(callId);
    dataMessageParcel.WriteInt8(str);
    dataMessageParcel.RewindRead(0);
    MessageParcel reply;
    DelayedSingleton<CallManagerService>::GetInstance()->OnStartDtmf(dataMessageParcel, reply);
}

void StopRtt(const uint8_t *data, size_t size)
{
    if (!IsServiceInited()) {
        return;
    }

    int32_t callId = *data % CALL_ID_NUM;
    MessageParcel dataMessageParcel;
    dataMessageParcel.WriteInt32(callId);
    dataMessageParcel.RewindRead(0);
    MessageParcel reply;
    DelayedSingleton<CallManagerService>::GetInstance()->OnStopRtt(dataMessageParcel, reply);
}

void SetMuted(const uint8_t *data, size_t size)
{
    if (!IsServiceInited()) {
        return;
    }

    bool isMute = false;
    MessageParcel dataMessageParcel;
    dataMessageParcel.WriteBool(isMute);
    dataMessageParcel.RewindRead(0);
    MessageParcel reply;
    DelayedSingleton<CallManagerService>::GetInstance()->OnSetMute(dataMessageParcel, reply);
}

void MuteRinger(const uint8_t *data, size_t size)
{
    if (!IsServiceInited()) {
        return;
    }

    MessageParcel dataMessageParcel;
    dataMessageParcel.WriteBuffer(data, size);
    dataMessageParcel.RewindRead(0);
    MessageParcel reply;
    DelayedSingleton<CallManagerService>::GetInstance()->OnSetMute(dataMessageParcel, reply);
}

void DoSomethingInterestingWithMyAPI(const uint8_t *data, size_t size)
{
    if (data == nullptr || size == 0) {
        return;
    }

    StartDtmf(data, size);
    StopRtt(data, size);
    SetMuted(data, size);
    MuteRinger(data, size);
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
