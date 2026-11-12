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

#include "callmanagerservicestubtwo_auto_fuzzer.h"

#include <cstddef>
#include <cstdint>
#include <securec.h>
#include <fuzzer/FuzzedDataProvider.h>
#define private public
#include "addcalltoken_fuzzer.h"
#include "call_manager_service_stub.h"

using namespace OHOS::Telephony;
namespace OHOS {
constexpr int32_t SLOT_NUM = 2;
constexpr int32_t POLICY_LIST_NUM = 3;
constexpr int32_t RESTRICTION_TYPE_NUM = 16;
constexpr int32_t RESTRICTION_MODE_NUM = 2;
constexpr int32_t TRANSFER_TYPE_NUM = 8;
constexpr int32_t TRANSFER_SETTING_NUM = 8;
constexpr uint32_t SUPPLEMENT_API_TARGET_COUNT = 17;

static void DoOnIsEmergencyPhoneNumber(FuzzedDataProvider &provider)
{
    std::string number = provider.ConsumeRandomLengthString();
    int32_t slotId = provider.ConsumeIntegral<int32_t>() % SLOT_NUM;
    MessageParcel messageParcel;
    messageParcel.WriteString16(Str8ToStr16(number));
    messageParcel.WriteInt32(slotId);
    MessageParcel reply;
    DelayedSingleton<CallManagerService>::GetInstance()->OnIsEmergencyPhoneNumber(messageParcel, reply);
}

static void DoOnGetHomeLocation(FuzzedDataProvider &provider)
{
    std::string number = provider.ConsumeRandomLengthString();
    MessageParcel messageParcel;
    messageParcel.WriteString(number);
    MessageParcel reply;
    DelayedSingleton<CallManagerService>::GetInstance()->OnGetHomeLocation(messageParcel, reply);
}

static void DoOnFormatPhoneNumber(FuzzedDataProvider &provider)
{
    std::string number = provider.ConsumeRandomLengthString();
    std::string countryCode = provider.ConsumeRandomLengthString();
    MessageParcel messageParcel;
    messageParcel.WriteString16(Str8ToStr16(number));
    messageParcel.WriteString16(Str8ToStr16(countryCode));
    MessageParcel reply;
    DelayedSingleton<CallManagerService>::GetInstance()->OnFormatPhoneNumber(messageParcel, reply);
}

static void DoOnFormatPhoneNumberToE164(FuzzedDataProvider &provider)
{
    std::string number = provider.ConsumeRandomLengthString();
    std::string countryCode = provider.ConsumeRandomLengthString();
    MessageParcel messageParcel;
    messageParcel.WriteString16(Str8ToStr16(number));
    messageParcel.WriteString16(Str8ToStr16(countryCode));
    MessageParcel reply;
    DelayedSingleton<CallManagerService>::GetInstance()->OnFormatPhoneNumberToE164(messageParcel, reply);
}

static void DoOnSetCallPolicyInfo(FuzzedDataProvider &provider)
{
    bool isDialingTrustlist = provider.ConsumeBool();
    std::vector<std::string> dialingList;
    for (int32_t i = 0; i < POLICY_LIST_NUM; i++) {
        dialingList.push_back(provider.ConsumeRandomLengthString());
    }
    bool isIncomingTrustlist = provider.ConsumeBool();
    std::vector<std::string> incomingList;
    for (int32_t i = 0; i < POLICY_LIST_NUM; i++) {
        incomingList.push_back(provider.ConsumeRandomLengthString());
    }
    MessageParcel messageParcel;
    messageParcel.WriteBool(isDialingTrustlist);
    messageParcel.WriteStringVector(dialingList);
    messageParcel.WriteBool(isIncomingTrustlist);
    messageParcel.WriteStringVector(incomingList);
    MessageParcel reply;
    DelayedSingleton<CallManagerService>::GetInstance()->OnSetCallPolicyInfo(messageParcel, reply);
}

static void DoOnGetCallWaiting(FuzzedDataProvider &provider)
{
    int32_t slotId = provider.ConsumeIntegral<int32_t>() % SLOT_NUM;
    MessageParcel messageParcel;
    messageParcel.WriteInt32(slotId);
    MessageParcel reply;
    DelayedSingleton<CallManagerService>::GetInstance()->OnGetCallWaiting(messageParcel, reply);
}

static void DoOnSetCallWaiting(FuzzedDataProvider &provider)
{
    int32_t slotId = provider.ConsumeIntegral<int32_t>() % SLOT_NUM;
    bool activate = provider.ConsumeBool();
    MessageParcel messageParcel;
    messageParcel.WriteInt32(slotId);
    messageParcel.WriteBool(activate);
    MessageParcel reply;
    DelayedSingleton<CallManagerService>::GetInstance()->OnSetCallWaiting(messageParcel, reply);
}

static void DoOnGetCallRestriction(FuzzedDataProvider &provider)
{
    int32_t slotId = provider.ConsumeIntegral<int32_t>() % SLOT_NUM;
    int32_t type = provider.ConsumeIntegral<int32_t>() % RESTRICTION_TYPE_NUM;
    MessageParcel messageParcel;
    messageParcel.WriteInt32(slotId);
    messageParcel.WriteInt32(type);
    MessageParcel reply;
    DelayedSingleton<CallManagerService>::GetInstance()->OnGetCallRestriction(messageParcel, reply);
}

static void DoOnSetCallRestriction(FuzzedDataProvider &provider)
{
    int32_t slotId = provider.ConsumeIntegral<int32_t>() % SLOT_NUM;
    std::string password = provider.ConsumeRandomLengthString();
    CallRestrictionInfo info;
    if (memset_s(&info, sizeof(CallRestrictionInfo), 0, sizeof(CallRestrictionInfo)) != EOK) {
        return;
    }
    size_t pwdLen = password.length() > static_cast<size_t>(kMaxNumberLen)
        ? static_cast<size_t>(kMaxNumberLen) : password.length();
    if (memcpy_s(info.password, kMaxNumberLen + 1, password.c_str(), pwdLen) != EOK) {
        return;
    }
    info.fac = static_cast<CallRestrictionType>(provider.ConsumeIntegral<int32_t>() % RESTRICTION_TYPE_NUM);
    info.mode = static_cast<CallRestrictionMode>(provider.ConsumeIntegral<int32_t>() % RESTRICTION_MODE_NUM);
    MessageParcel messageParcel;
    messageParcel.WriteInt32(slotId);
    messageParcel.WriteRawData((const void *)&info, sizeof(CallRestrictionInfo));
    MessageParcel reply;
    DelayedSingleton<CallManagerService>::GetInstance()->OnSetCallRestriction(messageParcel, reply);
}

static void DoOnGetTransferNumber(FuzzedDataProvider &provider)
{
    int32_t slotId = provider.ConsumeIntegral<int32_t>() % SLOT_NUM;
    int32_t type = provider.ConsumeIntegral<int32_t>() % TRANSFER_TYPE_NUM;
    MessageParcel messageParcel;
    messageParcel.WriteInt32(slotId);
    messageParcel.WriteInt32(type);
    MessageParcel reply;
    DelayedSingleton<CallManagerService>::GetInstance()->OnGetTransferNumber(messageParcel, reply);
}

static void DoOnSetTransferNumber(FuzzedDataProvider &provider)
{
    int32_t slotId = provider.ConsumeIntegral<int32_t>() % SLOT_NUM;
    std::string transferNum = provider.ConsumeRandomLengthString();
    CallTransferInfo info;
    if (memset_s(&info, sizeof(CallTransferInfo), 0, sizeof(CallTransferInfo)) != EOK) {
        return;
    }
    size_t numLen = transferNum.length() > static_cast<size_t>(kMaxNumberLen)
        ? static_cast<size_t>(kMaxNumberLen) : transferNum.length();
    if (memcpy_s(info.transferNum, kMaxNumberLen + 1, transferNum.c_str(), numLen) != EOK) {
        return;
    }
    info.settingType = static_cast<CallTransferSettingType>(
        provider.ConsumeIntegral<int32_t>() % TRANSFER_SETTING_NUM);
    info.type = static_cast<CallTransferType>(provider.ConsumeIntegral<int32_t>() % TRANSFER_TYPE_NUM);
    info.startHour = provider.ConsumeIntegral<int32_t>();
    info.startMinute = provider.ConsumeIntegral<int32_t>();
    info.endHour = provider.ConsumeIntegral<int32_t>();
    info.endMinute = provider.ConsumeIntegral<int32_t>();
    MessageParcel messageParcel;
    messageParcel.WriteInt32(slotId);
    messageParcel.WriteRawData((const void *)&info, sizeof(CallTransferInfo));
    MessageParcel reply;
    DelayedSingleton<CallManagerService>::GetInstance()->OnSetTransferNumber(messageParcel, reply);
}

static void DoOnCloseUnFinishedUssd(FuzzedDataProvider &provider)
{
    int32_t slotId = provider.ConsumeIntegral<int32_t>() % SLOT_NUM;
    MessageParcel messageParcel;
    messageParcel.WriteInt32(slotId);
    MessageParcel reply;
    DelayedSingleton<CallManagerService>::GetInstance()->OnCloseUnFinishedUssd(messageParcel, reply);
}

static void DoOnSendUssdResponse(FuzzedDataProvider &provider)
{
    int32_t slotId = provider.ConsumeIntegral<int32_t>() % SLOT_NUM;
    std::string content = provider.ConsumeRandomLengthString();
    MessageParcel messageParcel;
    messageParcel.WriteInt32(slotId);
    messageParcel.WriteString(content);
    MessageParcel reply;
    DelayedSingleton<CallManagerService>::GetInstance()->OnSendUssdResponse(messageParcel, reply);
}

static void DoOnGetTransferNumberByNumber(FuzzedDataProvider &provider)
{
    std::string number = provider.ConsumeRandomLengthString();
    int32_t type = provider.ConsumeIntegral<int32_t>() % TRANSFER_TYPE_NUM;
    MessageParcel messageParcel;
    messageParcel.WriteString(number);
    messageParcel.WriteInt32(type);
    MessageParcel reply;
    DelayedSingleton<CallManagerService>::GetInstance()->OnGetTransferNumberByNumber(messageParcel, reply);
}

static void DoOnCheckCallRecordingPermission(FuzzedDataProvider &provider)
{
    std::string phoneNumber = provider.ConsumeRandomLengthString();
    std::string recordToken = provider.ConsumeRandomLengthString();
    MessageParcel messageParcel;
    messageParcel.WriteString(phoneNumber);
    messageParcel.WriteString(recordToken);
    MessageParcel reply;
    DelayedSingleton<CallManagerService>::GetInstance()->OnCheckCallRecordingPermission(messageParcel, reply);
}

static void DoOnSetRegMmiCodeCallbackState(FuzzedDataProvider &provider)
{
    bool isReg = provider.ConsumeBool();
    MessageParcel messageParcel;
    messageParcel.WriteBool(isReg);
    MessageParcel reply;
    DelayedSingleton<CallManagerService>::GetInstance()->OnSetRegMmiCodeCallbackState(messageParcel, reply);
}

#ifdef CALL_MANAGER_CALL_TRANSFER
static void DoOnNotifyTransferCallContact(FuzzedDataProvider &provider)
{
    std::string contactName = provider.ConsumeRandomLengthString();
    MessageParcel messageParcel;
    messageParcel.WriteString(contactName);
    MessageParcel reply;
    DelayedSingleton<CallManagerService>::GetInstance()->OnNotifyTransferCallContact(messageParcel, reply);
}
#endif

void DoSomethingInterestingWithMyAPI(const uint8_t *data, size_t size)
{
    if (data == nullptr || size == 0) {
        return;
    }
    if (!IsServiceInited()) {
        return;
    }
    FuzzedDataProvider provider(data, size);
#ifdef CALL_MANAGER_CALL_TRANSFER
    uint32_t switchValue = provider.ConsumeIntegralInRange<uint32_t>(0, SUPPLEMENT_API_TARGET_COUNT - 1);
#else
    uint32_t switchValue = provider.ConsumeIntegralInRange<uint32_t>(0, SUPPLEMENT_API_TARGET_COUNT - 2);
#endif
    switch (switchValue) {
        case 0:
            DoOnIsEmergencyPhoneNumber(provider);
            break;
        case 1:
            DoOnGetHomeLocation(provider);
            break;
        case 2:
            DoOnFormatPhoneNumber(provider);
            break;
        case 3:
            DoOnFormatPhoneNumberToE164(provider);
            break;
        case 4:
            DoOnSetCallPolicyInfo(provider);
            break;
        case 5:
            DoOnGetCallWaiting(provider);
            break;
        case 6:
            DoOnSetCallWaiting(provider);
            break;
        case 7:
            DoOnGetCallRestriction(provider);
            break;
        case 8:
            DoOnSetCallRestriction(provider);
            break;
        case 9:
            DoOnGetTransferNumber(provider);
            break;
        case 10:
            DoOnSetTransferNumber(provider);
            break;
        case 11:
            DoOnCloseUnFinishedUssd(provider);
            break;
        case 12:
            DoOnSendUssdResponse(provider);
            break;
        case 13:
            DoOnGetTransferNumberByNumber(provider);
            break;
        case 14:
            DoOnCheckCallRecordingPermission(provider);
            break;
        case 15:
            DoOnSetRegMmiCodeCallbackState(provider);
            break;
#ifdef CALL_MANAGER_CALL_TRANSFER
        case 16:
            DoOnNotifyTransferCallContact(provider);
            break;
#endif
        default:
            break;
    }
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
