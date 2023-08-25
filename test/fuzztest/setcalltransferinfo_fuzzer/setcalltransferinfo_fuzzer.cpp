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

#include "setcalltransferinfo_fuzzer.h"

#include <cstddef>
#include <cstdint>
#define private public
#include "addcalltoken_fuzzer.h"
#include "call_manager_service_stub.h"

using namespace OHOS::Telephony;
namespace OHOS {
constexpr int32_t SLOT_NUM = 2;

void IsImsSwitchEnabled(const uint8_t *data, size_t size)
{
    if (!IsServiceInited()) {
        return;
    }

    int32_t slotId = static_cast<int32_t>(size % SLOT_NUM);
    MessageParcel dataMessageParcel;
    dataMessageParcel.WriteInt32(slotId);
    dataMessageParcel.WriteBuffer(data, size);
    dataMessageParcel.RewindRead(0);
    MessageParcel reply;
    DelayedSingleton<CallManagerService>::GetInstance()->OnIsVoLteEnabled(dataMessageParcel, reply);
}

void GetVoNRState(const uint8_t *data, size_t size)
{
    if (!IsServiceInited()) {
        return;
    }

    MessageParcel dataMessageParcel;
    dataMessageParcel.WriteInt32(static_cast<int32_t>(size % SLOT_NUM));
    dataMessageParcel.WriteBuffer(data, size);
    dataMessageParcel.RewindRead(0);
    MessageParcel reply;
    DelayedSingleton<CallManagerService>::GetInstance()->OnGetVoNRState(dataMessageParcel, reply);
}

void GetImsConfig(const uint8_t *data, size_t size)
{
    if (!IsServiceInited()) {
        return;
    }

    int32_t slotId = static_cast<int32_t>(size % SLOT_NUM);
    MessageParcel dataMessageParcel;
    dataMessageParcel.WriteInt32(slotId);
    dataMessageParcel.WriteInt32(static_cast<ImsConfigItem>(size));
    dataMessageParcel.WriteBuffer(data, size);
    dataMessageParcel.RewindRead(0);
    MessageParcel reply;
    DelayedSingleton<CallManagerService>::GetInstance()->OnGetImsConfig(dataMessageParcel, reply);
}

void GetImsFeatureValue(const uint8_t *data, size_t size)
{
    if (!IsServiceInited()) {
        return;
    }

    int32_t slotId = static_cast<int32_t>(size % SLOT_NUM);
    MessageParcel dataMessageParcel;
    dataMessageParcel.WriteInt32(slotId);
    dataMessageParcel.WriteInt32(static_cast<FeatureType>(size));
    dataMessageParcel.WriteBuffer(data, size);
    dataMessageParcel.RewindRead(0);
    MessageParcel reply;
    DelayedSingleton<CallManagerService>::GetInstance()->OnGetImsFeatureValue(dataMessageParcel, reply);
}

void GetCallTransferInfo(const uint8_t *data, size_t size)
{
    if (!IsServiceInited()) {
        return;
    }

    int32_t slotId = static_cast<int32_t>(size % SLOT_NUM);
    MessageParcel dataMessageParcel;
    dataMessageParcel.WriteInt32(slotId);
    dataMessageParcel.WriteInt32(static_cast<int32_t>(size));
    dataMessageParcel.WriteBuffer(data, size);
    dataMessageParcel.RewindRead(0);
    MessageParcel reply;
    DelayedSingleton<CallManagerService>::GetInstance()->OnGetTransferNumber(dataMessageParcel, reply);
}

void SetCallTransferInfo(const uint8_t *data, size_t size)
{
    if (!IsServiceInited()) {
        return;
    }

    int32_t slotId = static_cast<int32_t>(size % SLOT_NUM);
    MessageParcel dataMessageParcel;
    CallTransferInfo info;
    (void)memcpy_s(info.transferNum, kMaxNumberLen, reinterpret_cast<const char *>(data), size);
    info.settingType = CallTransferSettingType::CALL_TRANSFER_ENABLE;
    info.type = CallTransferType::TRANSFER_TYPE_BUSY;
    dataMessageParcel.WriteInt32(slotId);
    dataMessageParcel.WriteRawData((const void *)&info, sizeof(CallTransferInfo));
    dataMessageParcel.RewindRead(0);
    MessageParcel reply;
    DelayedSingleton<CallManagerService>::GetInstance()->OnSetTransferNumber(dataMessageParcel, reply);
}

void DoSomethingInterestingWithMyAPI(const uint8_t *data, size_t size)
{
    if (data == nullptr || size == 0) {
        return;
    }

    IsImsSwitchEnabled(data, size);
    GetVoNRState(data, size);
    GetImsConfig(data, size);
    GetImsFeatureValue(data, size);
    GetCallTransferInfo(data, size);
    SetCallTransferInfo(data, size);
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
