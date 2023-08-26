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

#include "formatphonenumber_fuzzer.h"

#include <cstddef>
#include <cstdint>
#define private public
#include "addcalltoken_fuzzer.h"

using namespace OHOS::Telephony;
namespace OHOS {
constexpr int32_t SLOT_NUM = 2;
constexpr int32_t CALLS_NUM = 5;

int32_t GetMainCallId(const uint8_t *data, size_t size)
{
    if (!IsServiceInited()) {
        return TELEPHONY_ERROR;
    }
    MessageParcel dataParcel;
    int32_t callId = static_cast<int32_t>(size);
    dataParcel.WriteInt32(callId);
    size_t mainCallId = static_cast<int32_t>(size);
    dataParcel.WriteInt32(mainCallId);
    dataParcel.WriteBuffer(data, size);
    dataParcel.RewindRead(0);
    MessageParcel reply;
    return DelayedSingleton<CallManagerService>::GetInstance()->OnGetMainCallId(dataParcel, reply);
}

int32_t GetSubCallIdList(const uint8_t *data, size_t size)
{
    if (!IsServiceInited()) {
        return TELEPHONY_ERROR;
    }
    MessageParcel dataParcel;
    int32_t id = static_cast<int32_t>(size);
    dataParcel.WriteInt32(id);
    dataParcel.WriteBuffer(data, size);
    dataParcel.RewindRead(0);
    MessageParcel reply;
    return DelayedSingleton<CallManagerService>::GetInstance()->OnGetSubCallIdList(dataParcel, reply);
}

int32_t GetCallIdListForConference(const uint8_t *data, size_t size)
{
    if (!IsServiceInited()) {
        return TELEPHONY_ERROR;
    }
    MessageParcel dataParcel;
    int32_t callId = static_cast<int32_t>(size);
    dataParcel.WriteInt32(callId);
    dataParcel.WriteBuffer(data, size);
    dataParcel.RewindRead(0);
    MessageParcel reply;
    return DelayedSingleton<CallManagerService>::GetInstance()->OnGetCallIdListForConference(dataParcel, reply);
}

int32_t GetCallRestriction(const uint8_t *data, size_t size)
{
    if (!IsServiceInited()) {
        return TELEPHONY_ERROR;
    }
    int32_t slotId = static_cast<int32_t>(size % SLOT_NUM);
    MessageParcel dataParcel;
    dataParcel.WriteInt32(slotId);
    dataParcel.WriteInt32(static_cast<int32_t>(size));
    dataParcel.WriteBuffer(data, size);
    dataParcel.RewindRead(0);
    MessageParcel reply;
    return DelayedSingleton<CallManagerService>::GetInstance()->OnGetCallRestriction(dataParcel, reply);
}

int32_t ReportOttCallDetailsInfo(const uint8_t *data, size_t size)
{
    if (!IsServiceInited()) {
        return TELEPHONY_ERROR;
    }
    int32_t vecCnt = static_cast<int32_t>(size % CALLS_NUM);
    OttCallDetailsInfo info;
    std::string msg(reinterpret_cast<const char *>(data), size);
    int32_t phoneLength = msg.length() > kMaxNumberLen ? kMaxNumberLen : msg.length();
    int32_t bundleLength = msg.length() > kMaxBundleNameLen ? kMaxBundleNameLen : msg.length();
    memcpy_s(info.phoneNum, kMaxNumberLen, msg.c_str(), phoneLength);
    memcpy_s(info.bundleName, kMaxBundleNameLen, msg.c_str(), bundleLength);
    info.callState = TelCallState::CALL_STATUS_DIALING;
    info.videoState = VideoStateType::TYPE_VOICE;
    MessageParcel dataParcel;
    dataParcel.WriteInt32(vecCnt);
    dataParcel.WriteRawData((const void *)&info, sizeof(OttCallDetailsInfo));
    dataParcel.RewindRead(0);
    MessageParcel reply;
    return DelayedSingleton<CallManagerService>::GetInstance()->OnReportOttCallDetailsInfo(dataParcel, reply);
}

int32_t ReportOttCallEventInfo(const uint8_t *data, size_t size)
{
    if (!IsServiceInited()) {
        return TELEPHONY_ERROR;
    }
    OttCallEventInfo info;
    info.ottCallEventId = OttCallEventId::OTT_CALL_EVENT_FUNCTION_UNSUPPORTED;
    std::string msg(reinterpret_cast<const char *>(data), size);
    int32_t bundleLength = msg.length() > kMaxBundleNameLen ? kMaxBundleNameLen : msg.length();
    memcpy_s(info.bundleName, kMaxBundleNameLen, msg.c_str(), bundleLength);
    MessageParcel dataParcel;
    dataParcel.WriteRawData((const void *)&info, sizeof(OttCallEventInfo));
    dataParcel.RewindRead(0);
    MessageParcel reply;
    return DelayedSingleton<CallManagerService>::GetInstance()->OnReportOttCallEventInfo(dataParcel, reply);
}

int32_t CloseUnFinishedUssd(const uint8_t *data, size_t size)
{
    if (!IsServiceInited()) {
        return TELEPHONY_ERROR;
    }
    int32_t slotId = static_cast<int32_t>(size % SLOT_NUM);
    MessageParcel dataParcel;
    dataParcel.WriteInt32(slotId);
    dataParcel.RewindRead(0);
    MessageParcel reply;
    return DelayedSingleton<CallManagerService>::GetInstance()->OnCloseUnFinishedUssd(dataParcel, reply);
}

int32_t FormatPhoneNumber(const uint8_t *data, size_t size)
{
    if (!IsServiceInited()) {
        return TELEPHONY_ERROR;
    }
    std::string number(reinterpret_cast<const char *>(data), size);
    auto numberU16 = Str8ToStr16(number);
    std::string countryCode(reinterpret_cast<const char *>(data), size);
    auto countryCodeU16 = Str8ToStr16(countryCode);
    MessageParcel dataParcel;
    dataParcel.WriteString16(numberU16);
    dataParcel.WriteString16(countryCodeU16);
    dataParcel.RewindRead(0);
    MessageParcel reply;
    return DelayedSingleton<CallManagerService>::GetInstance()->OnFormatPhoneNumber(dataParcel, reply);
}

void DoSomethingInterestingWithMyAPI(const uint8_t *data, size_t size)
{
    if (data == nullptr || size == 0) {
        return;
    }

    GetMainCallId(data, size);
    GetSubCallIdList(data, size);
    GetCallIdListForConference(data, size);
    GetCallRestriction(data, size);
    ReportOttCallDetailsInfo(data, size);
    ReportOttCallEventInfo(data, size);
    CloseUnFinishedUssd(data, size);
    FormatPhoneNumber(data, size);
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
