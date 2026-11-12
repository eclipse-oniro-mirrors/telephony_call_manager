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

#include "callmanagerservicestubfour_auto_fuzzer.h"

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
constexpr int32_t IMS_CONFIG_ITEM_NUM = 16;
constexpr int32_t FEATURE_TYPE_NUM = 8;
constexpr int32_t IMS_CALL_MODE_NUM = 8;
constexpr int32_t OTT_EVENT_ID_NUM = 2;
constexpr int32_t OTT_DETAILS_NUM = 8;
constexpr uint32_t IMS_API_TARGET_COUNT = 16;

static void DoOnSetCallPreferenceMode(FuzzedDataProvider &provider)
{
    int32_t slotId = provider.ConsumeIntegral<int32_t>() % SLOT_NUM;
    int32_t mode = provider.ConsumeIntegral<int32_t>();
    MessageParcel messageParcel;
    messageParcel.WriteInt32(slotId);
    messageParcel.WriteInt32(mode);
    MessageParcel reply;
    DelayedSingleton<CallManagerService>::GetInstance()->OnSetCallPreferenceMode(messageParcel, reply);
}

static void DoOnGetImsConfig(FuzzedDataProvider &provider)
{
    int32_t slotId = provider.ConsumeIntegral<int32_t>() % SLOT_NUM;
    int32_t item = provider.ConsumeBool() ? (provider.ConsumeBool() ? ITEM_VIDEO_QUALITY : ITEM_IMS_SWITCH_STATUS)
        : (provider.ConsumeIntegral<int32_t>() % IMS_CONFIG_ITEM_NUM);
    MessageParcel messageParcel;
    messageParcel.WriteInt32(slotId);
    messageParcel.WriteInt32(item);
    MessageParcel reply;
    DelayedSingleton<CallManagerService>::GetInstance()->OnGetImsConfig(messageParcel, reply);
}

static void DoOnSetImsConfig(FuzzedDataProvider &provider)
{
    int32_t slotId = provider.ConsumeIntegral<int32_t>() % SLOT_NUM;
    int32_t item = provider.ConsumeBool() ? (provider.ConsumeBool() ? ITEM_VIDEO_QUALITY : ITEM_IMS_SWITCH_STATUS)
        : (provider.ConsumeIntegral<int32_t>() % IMS_CONFIG_ITEM_NUM);
    std::string value = provider.ConsumeRandomLengthString();
    MessageParcel messageParcel;
    messageParcel.WriteInt32(slotId);
    messageParcel.WriteInt32(item);
    messageParcel.WriteString16(Str8ToStr16(value));
    MessageParcel reply;
    DelayedSingleton<CallManagerService>::GetInstance()->OnSetImsConfig(messageParcel, reply);
}

static void DoOnGetImsFeatureValue(FuzzedDataProvider &provider)
{
    int32_t slotId = provider.ConsumeIntegral<int32_t>() % SLOT_NUM;
    int32_t type = provider.ConsumeIntegral<int32_t>() % FEATURE_TYPE_NUM;
    MessageParcel messageParcel;
    messageParcel.WriteInt32(slotId);
    messageParcel.WriteInt32(type);
    MessageParcel reply;
    DelayedSingleton<CallManagerService>::GetInstance()->OnGetImsFeatureValue(messageParcel, reply);
}

static void DoOnSetImsFeatureValue(FuzzedDataProvider &provider)
{
    int32_t slotId = provider.ConsumeIntegral<int32_t>() % SLOT_NUM;
    int32_t type = provider.ConsumeIntegral<int32_t>() % FEATURE_TYPE_NUM;
    int32_t value = provider.ConsumeIntegral<int32_t>();
    MessageParcel messageParcel;
    messageParcel.WriteInt32(slotId);
    messageParcel.WriteInt32(type);
    messageParcel.WriteInt32(value);
    MessageParcel reply;
    DelayedSingleton<CallManagerService>::GetInstance()->OnSetImsFeatureValue(messageParcel, reply);
}

static void DoOnEnableVoLte(FuzzedDataProvider &provider)
{
    int32_t slotId = provider.ConsumeIntegral<int32_t>() % SLOT_NUM;
    MessageParcel messageParcel;
    messageParcel.WriteInt32(slotId);
    MessageParcel reply;
    DelayedSingleton<CallManagerService>::GetInstance()->OnEnableVoLte(messageParcel, reply);
}

static void DoOnDisableVoLte(FuzzedDataProvider &provider)
{
    int32_t slotId = provider.ConsumeIntegral<int32_t>() % SLOT_NUM;
    MessageParcel messageParcel;
    messageParcel.WriteInt32(slotId);
    MessageParcel reply;
    DelayedSingleton<CallManagerService>::GetInstance()->OnDisableVoLte(messageParcel, reply);
}

static void DoOnIsVoLteEnabled(FuzzedDataProvider &provider)
{
    int32_t slotId = provider.ConsumeIntegral<int32_t>() % SLOT_NUM;
    MessageParcel messageParcel;
    messageParcel.WriteInt32(slotId);
    MessageParcel reply;
    DelayedSingleton<CallManagerService>::GetInstance()->OnIsVoLteEnabled(messageParcel, reply);
}

static void DoOnGetVoNRState(FuzzedDataProvider &provider)
{
    int32_t slotId = provider.ConsumeIntegral<int32_t>() % SLOT_NUM;
    MessageParcel messageParcel;
    messageParcel.WriteInt32(slotId);
    MessageParcel reply;
    DelayedSingleton<CallManagerService>::GetInstance()->OnGetVoNRState(messageParcel, reply);
}

static void DoOnUpdateCallMediaMode(FuzzedDataProvider &provider)
{
    int32_t callId = provider.ConsumeIntegral<int32_t>();
    uint32_t mode = provider.ConsumeIntegral<uint32_t>() % IMS_CALL_MODE_NUM;
    MessageParcel messageParcel;
    messageParcel.WriteInt32(callId);
    messageParcel.WriteUint32(mode);
    MessageParcel reply;
    DelayedSingleton<CallManagerService>::GetInstance()->OnUpdateCallMediaMode(messageParcel, reply);
}

static void DoOnSetVoIPCallInfo(FuzzedDataProvider &provider)
{
    int32_t callId = provider.ConsumeIntegral<int32_t>();
    int32_t state = provider.ConsumeIntegral<int32_t>();
    std::string phoneNumber = provider.ConsumeRandomLengthString();
    MessageParcel messageParcel;
    messageParcel.WriteInt32(callId);
    messageParcel.WriteInt32(state);
    messageParcel.WriteString(phoneNumber);
    MessageParcel reply;
    DelayedSingleton<CallManagerService>::GetInstance()->OnSetVoIPCallInfo(messageParcel, reply);
}

static void DoOnReportOttCallDetailsInfo(FuzzedDataProvider &provider)
{
    int32_t vecCnt = provider.ConsumeIntegral<int32_t>() % OTT_DETAILS_NUM;
    MessageParcel messageParcel;
    messageParcel.WriteInt32(vecCnt);
    for (int32_t i = 0; i < vecCnt; i++) {
        OttCallDetailsInfo info;
        if (memset_s(&info, sizeof(OttCallDetailsInfo), 0, sizeof(OttCallDetailsInfo)) != EOK) {
            return;
        }
        std::string phoneNum = provider.ConsumeRandomLengthString();
        std::string bundleName = provider.ConsumeRandomLengthString();
        size_t numLen = phoneNum.length() > static_cast<size_t>(kMaxNumberLen)
            ? static_cast<size_t>(kMaxNumberLen) : phoneNum.length();
        if (memcpy_s(info.phoneNum, kMaxNumberLen + 1, phoneNum.c_str(), numLen) != EOK) {
            return;
        }
        size_t nameLen = bundleName.length() > static_cast<size_t>(kMaxBundleNameLen)
            ? static_cast<size_t>(kMaxBundleNameLen) : bundleName.length();
        if (memcpy_s(info.bundleName, kMaxBundleNameLen + 1, bundleName.c_str(), nameLen) != EOK) {
            return;
        }
        info.videoState = static_cast<VideoStateType>(provider.ConsumeIntegral<int32_t>());
        info.callState = static_cast<TelCallState>(provider.ConsumeIntegral<int32_t>());
        messageParcel.WriteRawData((const void *)&info, sizeof(OttCallDetailsInfo));
    }
    MessageParcel reply;
    DelayedSingleton<CallManagerService>::GetInstance()->OnReportOttCallDetailsInfo(messageParcel, reply);
}

static void DoOnReportOttCallEventInfo(FuzzedDataProvider &provider)
{
    std::string bundleName = provider.ConsumeRandomLengthString();
    OttCallEventInfo eventInfo;
    if (memset_s(&eventInfo, sizeof(OttCallEventInfo), 0, sizeof(OttCallEventInfo)) != EOK) {
        return;
    }
    eventInfo.ottCallEventId =
        static_cast<OttCallEventId>(provider.ConsumeIntegral<int32_t>() % OTT_EVENT_ID_NUM);
    size_t nameLen = bundleName.length() > static_cast<size_t>(kMaxBundleNameLen)
        ? static_cast<size_t>(kMaxBundleNameLen) : bundleName.length();
    if (memcpy_s(eventInfo.bundleName, kMaxBundleNameLen + 1, bundleName.c_str(), nameLen) != EOK) {
        return;
    }
    MessageParcel messageParcel;
    messageParcel.WriteRawData((const void *)&eventInfo, sizeof(OttCallEventInfo));
    MessageParcel reply;
    DelayedSingleton<CallManagerService>::GetInstance()->OnReportOttCallEventInfo(messageParcel, reply);
}

#ifdef SUPPORT_RTT_CALL
static void DoOnSendRttMessage(FuzzedDataProvider &provider)
{
    int32_t callId = provider.ConsumeIntegral<int32_t>();
    std::string rttMessage = provider.ConsumeRandomLengthString();
    MessageParcel messageParcel;
    messageParcel.WriteInt32(callId);
    messageParcel.WriteString(rttMessage);
    MessageParcel reply;
    DelayedSingleton<CallManagerService>::GetInstance()->OnSendRttMessage(messageParcel, reply);
}

static void DoOnSetRttCapability(FuzzedDataProvider &provider)
{
    int32_t slotId = provider.ConsumeIntegral<int32_t>() % SLOT_NUM;
    bool isEnable = provider.ConsumeBool();
    MessageParcel messageParcel;
    messageParcel.WriteInt32(slotId);
    messageParcel.WriteBool(isEnable);
    MessageParcel reply;
    DelayedSingleton<CallManagerService>::GetInstance()->OnSetRttCapability(messageParcel, reply);
}

static void DoOnUpdateImsRttCallMode(FuzzedDataProvider &provider)
{
    int32_t callId = provider.ConsumeIntegral<int32_t>();
    int32_t mode = provider.ConsumeIntegral<int32_t>();
    MessageParcel messageParcel;
    messageParcel.WriteInt32(callId);
    messageParcel.WriteInt32(mode);
    MessageParcel reply;
    DelayedSingleton<CallManagerService>::GetInstance()->OnUpdateImsRttCallMode(messageParcel, reply);
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
#ifdef SUPPORT_RTT_CALL
    uint32_t switchValue = provider.ConsumeIntegralInRange<uint32_t>(0, IMS_API_TARGET_COUNT - 1);
#else
    uint32_t switchValue = provider.ConsumeIntegralInRange<uint32_t>(0, IMS_API_TARGET_COUNT - 4);
#endif
    switch (switchValue) {
        case 0:
            DoOnSetCallPreferenceMode(provider);
            break;
        case 1:
            DoOnGetImsConfig(provider);
            break;
        case 2:
            DoOnSetImsConfig(provider);
            break;
        case 3:
            DoOnGetImsFeatureValue(provider);
            break;
        case 4:
            DoOnSetImsFeatureValue(provider);
            break;
        case 5:
            DoOnEnableVoLte(provider);
            break;
        case 6:
            DoOnDisableVoLte(provider);
            break;
        case 7:
            DoOnIsVoLteEnabled(provider);
            break;
        case 8:
            DoOnGetVoNRState(provider);
            break;
        case 9:
            DoOnUpdateCallMediaMode(provider);
            break;
        case 10:
            DoOnSetVoIPCallInfo(provider);
            break;
        case 11:
            DoOnReportOttCallDetailsInfo(provider);
            break;
        case 12:
            DoOnReportOttCallEventInfo(provider);
            break;
#ifdef SUPPORT_RTT_CALL
        case 13:
            DoOnSendRttMessage(provider);
            break;
        case 14:
            DoOnSetRttCapability(provider);
            break;
        case 15:
            DoOnUpdateImsRttCallMode(provider);
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
