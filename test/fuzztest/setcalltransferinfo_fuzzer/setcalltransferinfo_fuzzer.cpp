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
#include "fuzzer/FuzzedDataProvider.h"

using namespace OHOS::Telephony;
namespace OHOS {
constexpr int32_t SLOT_NUM = 2;
constexpr int32_t IMS_CONFIG_ITEM_NUM = 2;
constexpr int32_t FEATURE_TYPE_NUM = 3;

void IsImsSwitchEnabled(FuzzedDataProvider &provider)
{
    if (!IsServiceInited()) {
        return;
    }

    int32_t slotId = provider.ConsumeIntegral<int32_t>() % SLOT_NUM;
    MessageParcel messageParcel;
    messageParcel.WriteInt32(slotId);
    std::vector<uint8_t> testData = provider.ConsumeRemainingBytes<uint8_t>();
    messageParcel.WriteBuffer(static_cast<void*>(testData.data()), testData.size());
    messageParcel.RewindRead(0);
    MessageParcel reply;
    DelayedSingleton<CallManagerService>::GetInstance()->OnIsVoLteEnabled(messageParcel, reply);
}

void GetVoNRState(FuzzedDataProvider &provider)
{
    if (!IsServiceInited()) {
        return;
    }

    MessageParcel messageParcel;
    messageParcel.WriteInt32(provider.ConsumeIntegral<int32_t>() % SLOT_NUM);
    std::vector<uint8_t> testData = provider.ConsumeRemainingBytes<uint8_t>();
    messageParcel.WriteBuffer(static_cast<void*>(testData.data()), testData.size());
    messageParcel.RewindRead(0);
    MessageParcel reply;
    DelayedSingleton<CallManagerService>::GetInstance()->OnGetVoNRState(messageParcel, reply);
}

void GetImsConfig(FuzzedDataProvider &provider)
{
    if (!IsServiceInited()) {
        return;
    }

    int32_t slotId = provider.ConsumeIntegral<int32_t>() % SLOT_NUM;
    int32_t item = provider.ConsumeIntegral<int32_t>() % IMS_CONFIG_ITEM_NUM;
    MessageParcel messageParcel;
    messageParcel.WriteInt32(slotId);
    messageParcel.WriteInt32(static_cast<ImsConfigItem>(item));
    std::vector<uint8_t> testData = provider.ConsumeRemainingBytes<uint8_t>();
    messageParcel.WriteBuffer(static_cast<void*>(testData.data()), testData.size());
    messageParcel.RewindRead(0);
    MessageParcel reply;
    DelayedSingleton<CallManagerService>::GetInstance()->OnGetImsConfig(messageParcel, reply);
}

void GetImsFeatureValue(FuzzedDataProvider &provider)
{
    if (!IsServiceInited()) {
        return;
    }

    int32_t slotId = provider.ConsumeIntegral<int32_t>() % SLOT_NUM;
    int32_t type = provider.ConsumeIntegral<int32_t>() % FEATURE_TYPE_NUM;
    MessageParcel messageParcel;
    messageParcel.WriteInt32(slotId);
    messageParcel.WriteInt32(static_cast<FeatureType>(type));
    std::vector<uint8_t> testData = provider.ConsumeRemainingBytes<uint8_t>();
    messageParcel.WriteBuffer(static_cast<void*>(testData.data()), testData.size());
    messageParcel.RewindRead(0);
    MessageParcel reply;
    DelayedSingleton<CallManagerService>::GetInstance()->OnGetImsFeatureValue(messageParcel, reply);
}

void GetCallTransferInfo(FuzzedDataProvider &provider)
{
    if (!IsServiceInited()) {
        return;
    }

    int32_t slotId = provider.ConsumeIntegral<int32_t>() % SLOT_NUM;
    MessageParcel messageParcel;
    messageParcel.WriteInt32(slotId);
    messageParcel.WriteInt32(provider.ConsumeIntegral<int32_t>());
    std::vector<uint8_t> testData = provider.ConsumeRemainingBytes<uint8_t>();
    messageParcel.WriteBuffer(static_cast<void*>(testData.data()), testData.size());
    messageParcel.RewindRead(0);
    MessageParcel reply;
    DelayedSingleton<CallManagerService>::GetInstance()->OnGetTransferNumber(messageParcel, reply);
}

void SetCallTransferInfo(FuzzedDataProvider &provider)
{
    if (!IsServiceInited()) {
        return;
    }

    int32_t slotId = provider.ConsumeIntegral<int32_t>() % SLOT_NUM;
    MessageParcel messageParcel;
    CallTransferInfo info;
    std::string msg = provider.ConsumeRandomLengthString();
    (void)memcpy_s(info.transferNum, kMaxNumberLen, msg, msg.size());
    info.settingType = CallTransferSettingType::CALL_TRANSFER_ENABLE;
    info.type = CallTransferType::TRANSFER_TYPE_BUSY;
    messageParcel.WriteInt32(slotId);
    messageParcel.WriteRawData((const void *)&info, sizeof(CallTransferInfo));
    messageParcel.RewindRead(0);
    MessageParcel reply;
    DelayedSingleton<CallManagerService>::GetInstance()->OnSetTransferNumber(messageParcel, reply);
}

void DoSomethingInterestingWithMyAPI(const uint8_t *data, size_t size)
{
    if (data == nullptr || size == 0) {
        return;
    }

    FuzzedDataProvider provider(data, size);
    IsImsSwitchEnabled(provider);
    GetVoNRState(provider);
    GetImsConfig(provider);
    GetImsFeatureValue(provider);
    GetCallTransferInfo(provider);
    SetCallTransferInfo(provider);
    DelayedSingleton<CallManagerService>::GetInstance()->OnStop();
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
