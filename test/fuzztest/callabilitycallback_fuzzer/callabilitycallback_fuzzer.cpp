/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#include "callabilitycallback_fuzzer.h"

#include <cstddef>
#include <cstdint>
#define private public
#include "addcalltoken_fuzzer.h"
#include "call_ability_callback.h"
#include "call_ability_callback_proxy.h"
#include "call_manager_callback.h"
#include "call_manager_utils.h"
#include "fuzzer/FuzzedDataProvider.h"

using namespace OHOS::Telephony;
namespace OHOS {
constexpr int32_t ACCOUNT_ID_NUM = 10;
constexpr int32_t BOOL_NUM = 2;
constexpr int32_t CALL_ID_NUM = 10;
constexpr int32_t REPORT_ID_NUM = 23;
constexpr int32_t RESULT_ID_NUM = 50;
constexpr int32_t OTT_ID_NUM = 11;
constexpr int32_t VEDIO_STATE_NUM = 2;
constexpr int32_t IMS_CALL_MODE_NUM = 5;
constexpr int32_t CALL_SESSION_EVENT_ID_NUM = 4;
constexpr int32_t VIDEO_REQUEST_RESULT_TYPE_NUM = 102;
sptr<CallAbilityCallback> callAbilityCallbackPtr_ = nullptr;

bool ServiceInited()
{
    if (!IsServiceInited()) {
        return false;
    }
    callAbilityCallbackPtr_ = new (std::nothrow) CallAbilityCallback();
    if (callAbilityCallbackPtr_ == nullptr) {
        return false;
    }
    return true;
}

int32_t OnRemoteRequest(FuzzedDataProvider& provider)
{
    if (!ServiceInited()) {
        return TELEPHONY_ERROR;
    }
    MessageParcel messageParcel;
    if (!messageParcel.WriteInterfaceToken(CallAbilityCallbackStub::GetDescriptor())) {
        return TELEPHONY_ERROR;
    }
    messageParcel.RewindRead(0);
    uint32_t code = provider.ConsumeIntegral<uint32_t>();
    MessageParcel reply;
    MessageOption option;
    return callAbilityCallbackPtr_->OnRemoteRequest(code, messageParcel, reply, option);
}

int32_t UpdateCallStateInfo(FuzzedDataProvider& provider)
{
    if (!ServiceInited()) {
        return TELEPHONY_ERROR;
    }
    CallAttributeInfo info;
    std::string msg = provider.consumeString();
    int32_t accountLength = msg.length() > kMaxNumberLen ? kMaxNumberLen : msg.length();
    int32_t bundleLength = msg.length() > kMaxBundleNameLen ? kMaxBundleNameLen : msg.length();
    memcpy_s(info.accountNumber, kMaxNumberLen, msg.c_str(), accountLength);
    memcpy_s(info.bundleName, kMaxBundleNameLen, msg.c_str(), bundleLength);
    info.accountId = provider.ConsumeIntegral<int32_t>();
    info.startTime = provider.ConsumeIntegral<uint32_t>();
    info.callId = provider.ConsumeIntegral<int32_t>();
    info.callBeginTime = provider.ConsumeIntegral<time_t>();
    info.callEndTime = provider.ConsumeIntegral<time_t>();
    info.ringBeginTime = provider.ConsumeIntegral<time_t>();
    info.ringEndTime = provider.ConsumeIntegral<time_t>();
    MessageParcel messageParcel;
    messageParcel.WriteCString(info.accountNumber);
    messageParcel.WriteCString(info.bundleName);
    messageParcel.WriteBool(info.speakerphoneOn);
    messageParcel.WriteInt32(info.accountId);
    messageParcel.WriteInt32(static_cast<int32_t>(info.videoState));
    messageParcel.WriteInt64(info.startTime);
    messageParcel.WriteBool(info.isEcc);
    messageParcel.WriteInt32(static_cast<int32_t>(info.callType));
    messageParcel.WriteInt32(info.callId);
    messageParcel.WriteInt32(static_cast<int32_t>(info.callState));
    messageParcel.WriteInt32(static_cast<int32_t>(info.conferenceState));
    messageParcel.WriteInt64(info.callBeginTime);
    messageParcel.WriteInt64(info.callEndTime);
    messageParcel.WriteInt64(info.ringBeginTime);
    messageParcel.WriteInt64(info.ringEndTime);
    messageParcel.WriteInt32(static_cast<int32_t>(info.callDirection));
    messageParcel.WriteInt32(static_cast<int32_t>(info.answerType));
    messageParcel.WriteInt32(info.index);
    messageParcel.WriteString(info.voipCallInfo.voipCallId);
    messageParcel.WriteString(info.voipCallInfo.userName);
    messageParcel.WriteString(info.voipCallInfo.abilityName);
    messageParcel.WriteString(info.voipCallInfo.extensionId);
    messageParcel.WriteString(info.voipCallInfo.voipBundleName);
    messageParcel.WriteUInt8Vector(info.voipCallInfo.userProfile);
    messageParcel.RewindRead(0);
    MessageParcel reply;
    MessageOption option;
    return callAbilityCallbackPtr_->OnUpdateCallStateInfo(messageParcel, reply);
}

int32_t UpdateCallEvent(FuzzedDataProvider& provider)
{
    if (!ServiceInited()) {
        return TELEPHONY_ERROR;
    }
    CallEventInfo info;
    std::string msg = provider.consumeString();
    int32_t phoneLength = msg.length() > kMaxNumberLen ? kMaxNumberLen : msg.length();
    int32_t bundleLength = msg.length() > kMaxBundleNameLen ? kMaxBundleNameLen : msg.length();
    memcpy_s(info.phoneNum, kMaxNumberLen, msg.c_str(), phoneLength);
    memcpy_s(info.bundleName, kMaxBundleNameLen, msg.c_str(), bundleLength);
    int32_t length = sizeof(CallEventInfo);
    MessageParcel messageParcel;
    messageParcel.WriteInt32(length);
    messageParcel.WriteRawData((const void *)&info, length);
    messageParcel.RewindRead(0);
    MessageParcel reply;
    MessageOption option;
    return callAbilityCallbackPtr_->OnUpdateCallEvent(messageParcel, reply);
}

int32_t UpdateCallDisconnectedCause(FuzzedDataProvider& provider)
{
    if (!ServiceInited()) {
        return TELEPHONY_ERROR;
    }
    int32_t reason = provider.ConsumeIntegral<int32_t>();
    std::string message = provider.consumeString();
    MessageParcel messageParcel;
    messageParcel.WriteInt32(reason);
    messageParcel.WriteString(message);
    messageParcel.RewindRead(0);
    MessageParcel reply;
    MessageOption option;
    return callAbilityCallbackPtr_->OnUpdateCallDisconnectedCause(messageParcel, reply);
}

int32_t UpdateAysncResults(FuzzedDataProvider& provider)
{
    if (!ServiceInited()) {
        return TELEPHONY_ERROR;
    }
    int32_t reportId = provider.ConsumeIntegral<uint32_t>() % REPORT_ID_NUM;
    int32_t resultId = provider.ConsumeIntegral<uint32_t>() % RESULT_ID_NUM;
    MessageParcel messageParcel;
    messageParcel.WriteInt32(reportId);
    messageParcel.WriteInt32(resultId);
    messageParcel.RewindRead(0);
    MessageParcel reply;
    MessageOption option;
    return callAbilityCallbackPtr_->OnUpdateAysncResults(messageParcel, reply);
}

int32_t UpdateOttCallRequest(FuzzedDataProvider& provider)
{
    if (!ServiceInited()) {
        return TELEPHONY_ERROR;
    }
    int32_t requestId = provider.ConsumeIntegral<uint32_t>() % OTT_ID_NUM;
    int32_t videoState = provider.ConsumeIntegral<uint32_t>() % VEDIO_STATE_NUM;
    std::string phoneNumber = provider.ConsumeRandomLengthString();
    std::string bundleName = provider.ConsumeRandomLengthString();
    MessageParcel messageParcel;
    messageParcel.WriteInt32(requestId);
    messageParcel.WriteInt32(videoState);
    messageParcel.WriteString(phoneNumber);
    messageParcel.WriteString(bundleName);
    messageParcel.RewindRead(0);
    MessageParcel reply;
    MessageOption option;
    return callAbilityCallbackPtr_->OnUpdateOttCallRequest(messageParcel, reply);
}

int32_t UpdateMmiCodeResults(FuzzedDataProvider& provider)
{
    if (!ServiceInited()) {
        return TELEPHONY_ERROR;
    }
    MmiCodeInfo info;
    int32_t length = sizeof(MmiCodeInfo);
    info.result = provider.ConsumeIntegral<uint32_t>();
    std::string msg = provider.consumeString();
    int32_t msgLength = msg.length() > kMaxNumberLen ? kMaxNumberLen : msg.length();
    memcpy_s(info.message, kMaxNumberLen, msg.c_str(), msgLength);
    MessageParcel messageParcel;
    messageParcel.WriteInt32(length);
    messageParcel.WriteRawData((const void *)&info, length);
    messageParcel.RewindRead(0);
    MessageParcel reply;
    MessageOption option;
    return callAbilityCallbackPtr_->OnUpdateMmiCodeResults(messageParcel, reply);
}

int32_t UpdateAudioDeviceChange(FuzzedDataProvider& provider)
{
    if (!ServiceInited()) {
        return TELEPHONY_ERROR;
    }
    MessageParcel messageParcel;
    if (!messageParcel.WriteInterfaceToken(CallAbilityCallbackProxy::GetDescriptor())) {
        TELEPHONY_LOGE("write descriptor fail");
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    AudioDevice device;
    device.deviceType = AudioDeviceType::DEVICE_UNKNOWN;
    std::string msg = provider.consumeString();
    int32_t length = msg.length() > kMaxAddressLen ? kMaxAddressLen : msg.length();
    memcpy_s(device.address, kMaxAddressLen, msg.c_str(), length);
    int32_t dataSize = provider.ConsumeIntegral<uint32_t>();
    messageParcel.WriteInt32(dataSize);
    messageParcel.WriteRawData((const void *)&device, sizeof(AudioDevice));
    messageParcel.WriteRawData((const void *)&device, sizeof(AudioDevice));
    messageParcel.WriteBool(provider.ConsumeIntegral<uint32_t>() % BOOL_NUM);
    messageParcel.RewindRead(0);
    MessageParcel reply;
    MessageOption option;
    return callAbilityCallbackPtr_->OnUpdateAudioDeviceChange(messageParcel, reply);
}

int32_t UpdateImsCallModeChange(FuzzedDataProvider& provider)
{
    if (!ServiceInited()) {
        return TELEPHONY_ERROR;
    }
    MessageParcel messageParcel;
    MessageParcel replyParcel;
    if (!messageParcel.WriteInterfaceToken(CallAbilityCallbackProxy::GetDescriptor())) {
        TELEPHONY_LOGE("write descriptor fail");
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    int32_t length = sizeof(CallMediaModeInfo);
    messageParcel.WriteInt32(length);
    CallMediaModeInfo callMediaModeInfo;
    callMediaModeInfo.callId = provider.ConsumeIntegral<int32_t>();
    callMediaModeInfo.isRequestInfo = provider.ConsumeIntegral<bool>() % BOOL_NUM;
    callMediaModeInfo.result = provider.ConsumeIntegral<VideoRequestResultType>() % VIDEO_REQUEST_RESULT_TYPE_NUM;
    callMediaModeInfo.callMode = provider.ConsumeIntegral<ImsCallMode>() % IMS_CALL_MODE_NUM;
    messageParcel.WriteRawData((const void *)&callMediaModeInfo, length);
    messageParcel.RewindRead(0);
    return callAbilityCallbackPtr_->OnUpdateImsCallModeChange(messageParcel, replyParcel);
}

int32_t UpdateCallSessionEventChange(FuzzedDataProvider& provider)
{
    if (!ServiceInited()) {
        return TELEPHONY_ERROR;
    }
    MessageParcel messageParcel;
    MessageParcel replyParcel;
    if (!messageParcel.WriteInterfaceToken(CallAbilityCallbackProxy::GetDescriptor())) {
        TELEPHONY_LOGE("write descriptor fail");
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    int32_t length = sizeof(CallSessionEvent);
    messageParcel.WriteInt32(length);
    CallSessionEvent callSessionInfo;
    callSessionInfo.callId = provider.ConsumeIntegral<int32_t>();
    callSessionInfo.eventId = provider.ConsumeIntegral<CallSessionEventId>() % CALL_SESSION_EVENT_ID_NUM;
    messageParcel.WriteRawData((const void *)&callSessionInfo, length);
    messageParcel.RewindRead(0);
    return callAbilityCallbackPtr_->OnUpdateCallSessionEventChange(messageParcel, replyParcel);
}

int32_t UpdatePeerDimensionsChange(FuzzedDataProvider& provider)
{
    if (!ServiceInited()) {
        return TELEPHONY_ERROR;
    }
    MessageParcel messageParcel;
    MessageParcel replyParcel;
    if (!messageParcel.WriteInterfaceToken(CallAbilityCallbackProxy::GetDescriptor())) {
        TELEPHONY_LOGE("write descriptor fail");
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    int32_t length = sizeof(PeerDimensionsDetail);
    messageParcel.WriteInt32(length);
    PeerDimensionsDetail peerDimensionsInfo;
    peerDimensionsInfo.callId = provider.ConsumeIntegral<int32_t>();
    peerDimensionsInfo.width = provider.ConsumeIntegral<int32_t>();
    peerDimensionsInfo.height = provider.ConsumeIntegral<int32_t>();
    messageParcel.RewindRead(0);
    return callAbilityCallbackPtr_->OnUpdatePeerDimensionsChange(messageParcel, replyParcel);
}

int32_t UpdateCallDataUsageChange(FuzzedDataProvider& provider)
{
    if (!ServiceInited()) {
        return TELEPHONY_ERROR;
    }
    MessageParcel messageParcel;
    MessageParcel replyParcel;
    if (!messageParcel.WriteInterfaceToken(CallAbilityCallbackProxy::GetDescriptor())) {
        TELEPHONY_LOGE("write descriptor fail");
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    int64_t dataUsage = provider.ConsumeIntegral<int64_t>();
    messageParcel.WriteInt64(dataUsage);
    messageParcel.RewindRead(0);
    return callAbilityCallbackPtr_->OnUpdateCallDataUsageChange(messageParcel, replyParcel);
}

int32_t UpdateCameraCapabilities(FuzzedDataProvider& provider)
{
    if (!ServiceInited()) {
        return TELEPHONY_ERROR;
    }
    MessageParcel messageParcel;
    MessageParcel replyParcel;
    if (!messageParcel.WriteInterfaceToken(CallAbilityCallbackProxy::GetDescriptor())) {
        TELEPHONY_LOGE("write descriptor fail");
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    int32_t length = sizeof(CameraCapabilities);
    messageParcel.WriteInt32(length);
    CameraCapabilities cameraCapabilitiesInfo;
    cameraCapabilitiesInfo.callId = provider.ConsumeIntegral<int32_t>();
    cameraCapabilitiesInfo.width = provider.ConsumeIntegral<int32_t>();
    cameraCapabilitiesInfo.height = provider.ConsumeIntegral<int32_t>();
    messageParcel.RewindRead(0);
    return callAbilityCallbackPtr_->OnUpdateCameraCapabilities(messageParcel, replyParcel);
}

void WriteCallAttributeInfo(FuzzedDataProvider& provider)
{
    MessageParcel messageParcel;
    CallAttributeInfo info;
    info.accountId = provider.ConsumeIntegral<int32_t>();
    info.callId = provider.ConsumeIntegral<int32_t>();
    info.callState = static_cast<TelCallState>(provider.ConsumeIntegral<int32_t>());
    CallManagerUtils::WriteCallAttributeInfo(info, messageParcel);
}

void DoSomethingInterestingWithMyAPI(const uint8_t *data, size_t size)
{
    if (data == nullptr || size == 0) {
        return;
    }

    FuzzedDataProvider provider(data, size);
    OnRemoteRequest(provider);
    UpdateCallStateInfo(provider);
    UpdateCallEvent(provider);
    UpdateCallDisconnectedCause(provider);
    UpdateAysncResults(provider);
    UpdateOttCallRequest(provider);
    UpdateMmiCodeResults(provider);
    UpdateAudioDeviceChange(provider);
    UpdateImsCallModeChange(provider);
    UpdateCallSessionEventChange(provider);
    UpdatePeerDimensionsChange(provider);
    UpdateCallDataUsageChange(provider);
    UpdateCameraCapabilities(provider);
    WriteCallAttributeInfo(provider);
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
