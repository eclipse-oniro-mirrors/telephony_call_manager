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

int32_t OnRemoteRequest(const uint8_t *data, size_t size)
{
    if (!ServiceInited()) {
        return TELEPHONY_ERROR;
    }
    MessageParcel dataMessageParcel;
    if (!dataMessageParcel.WriteInterfaceToken(CallAbilityCallbackStub::GetDescriptor())) {
        return TELEPHONY_ERROR;
    }
    dataMessageParcel.RewindRead(0);
    uint32_t code = static_cast<uint32_t>(size);
    MessageParcel reply;
    MessageOption option;
    return callAbilityCallbackPtr_->OnRemoteRequest(code, dataMessageParcel, reply, option);
}

int32_t UpdateCallStateInfo(const uint8_t *data, size_t size)
{
    if (!ServiceInited()) {
        return TELEPHONY_ERROR;
    }
    CallAttributeInfo info;
    std::string msg(reinterpret_cast<const char *>(data), size);
    int32_t accountLength = msg.length() > kMaxNumberLen ? kMaxNumberLen : msg.length();
    int32_t bundleLength = msg.length() > kMaxBundleNameLen ? kMaxBundleNameLen : msg.length();
    memcpy_s(info.accountNumber, kMaxNumberLen, msg.c_str(), accountLength);
    memcpy_s(info.bundleName, kMaxBundleNameLen, msg.c_str(), bundleLength);
    info.accountId = static_cast<int32_t>(size % ACCOUNT_ID_NUM);
    info.startTime = static_cast<uint32_t>(size);
    info.callId = static_cast<int32_t>(size % CALL_ID_NUM);
    info.callBeginTime = static_cast<time_t>(size);
    info.callEndTime = static_cast<time_t>(size);
    info.ringBeginTime = static_cast<time_t>(size);
    info.ringEndTime = static_cast<time_t>(size);
    int32_t length = sizeof(CallAttributeInfo);
    MessageParcel dataMessageParcel;
    dataMessageParcel.WriteInt32(length);
    dataMessageParcel.WriteRawData((const void *)&info, length);
    dataMessageParcel.RewindRead(0);
    MessageParcel reply;
    MessageOption option;
    return callAbilityCallbackPtr_->OnUpdateCallStateInfo(dataMessageParcel, reply);
}

int32_t UpdateCallEvent(const uint8_t *data, size_t size)
{
    if (!ServiceInited()) {
        return TELEPHONY_ERROR;
    }
    CallEventInfo info;
    std::string msg(reinterpret_cast<const char *>(data), size);
    int32_t phoneLength = msg.length() > kMaxNumberLen ? kMaxNumberLen : msg.length();
    int32_t bundleLength = msg.length() > kMaxBundleNameLen ? kMaxBundleNameLen : msg.length();
    memcpy_s(info.phoneNum, kMaxNumberLen, msg.c_str(), phoneLength);
    memcpy_s(info.bundleName, kMaxBundleNameLen, msg.c_str(), bundleLength);
    int32_t length = sizeof(CallEventInfo);
    MessageParcel dataMessageParcel;
    dataMessageParcel.WriteInt32(length);
    dataMessageParcel.WriteRawData((const void *)&info, length);
    dataMessageParcel.RewindRead(0);
    MessageParcel reply;
    MessageOption option;
    return callAbilityCallbackPtr_->OnUpdateCallEvent(dataMessageParcel, reply);
}

int32_t UpdateCallDisconnectedCause(const uint8_t *data, size_t size)
{
    if (!ServiceInited()) {
        return TELEPHONY_ERROR;
    }
    int32_t reason = static_cast<uint32_t>(size);
    std::string message(reinterpret_cast<const char *>(data), size);
    MessageParcel dataMessageParcel;
    dataMessageParcel.WriteInt32(reason);
    dataMessageParcel.WriteString(message);
    dataMessageParcel.RewindRead(0);
    MessageParcel reply;
    MessageOption option;
    return callAbilityCallbackPtr_->OnUpdateCallDisconnectedCause(dataMessageParcel, reply);
}

int32_t UpdateAysncResults(const uint8_t *data, size_t size)
{
    if (!ServiceInited()) {
        return TELEPHONY_ERROR;
    }
    int32_t reportId = static_cast<uint32_t>(size % REPORT_ID_NUM);
    int32_t resultId = static_cast<uint32_t>(size % RESULT_ID_NUM);
    MessageParcel dataMessageParcel;
    dataMessageParcel.WriteInt32(reportId);
    dataMessageParcel.WriteInt32(resultId);
    dataMessageParcel.RewindRead(0);
    MessageParcel reply;
    MessageOption option;
    return callAbilityCallbackPtr_->OnUpdateAysncResults(dataMessageParcel, reply);
}

int32_t UpdateOttCallRequest(const uint8_t *data, size_t size)
{
    if (!ServiceInited()) {
        return TELEPHONY_ERROR;
    }
    int32_t requestId = static_cast<uint32_t>(size % OTT_ID_NUM);
    int32_t videoState = static_cast<uint32_t>(size % VEDIO_STATE_NUM);
    std::string phoneNumber(reinterpret_cast<const char *>(data), size);
    std::string bundleName(reinterpret_cast<const char *>(data), size);
    MessageParcel dataMessageParcel;
    dataMessageParcel.WriteInt32(requestId);
    dataMessageParcel.WriteInt32(videoState);
    dataMessageParcel.WriteString(phoneNumber);
    dataMessageParcel.WriteString(bundleName);
    dataMessageParcel.RewindRead(0);
    MessageParcel reply;
    MessageOption option;
    return callAbilityCallbackPtr_->OnUpdateOttCallRequest(dataMessageParcel, reply);
}

int32_t UpdateMmiCodeResults(const uint8_t *data, size_t size)
{
    if (!ServiceInited()) {
        return TELEPHONY_ERROR;
    }
    MmiCodeInfo info;
    int32_t length = sizeof(MmiCodeInfo);
    info.result = static_cast<uint32_t>(size);
    std::string msg(reinterpret_cast<const char *>(data), size);
    int32_t msgLength = msg.length() > kMaxNumberLen ? kMaxNumberLen : msg.length();
    memcpy_s(info.message, kMaxNumberLen, msg.c_str(), msgLength);
    MessageParcel dataMessageParcel;
    dataMessageParcel.WriteInt32(length);
    dataMessageParcel.WriteRawData((const void *)&info, length);
    dataMessageParcel.RewindRead(0);
    MessageParcel reply;
    MessageOption option;
    return callAbilityCallbackPtr_->OnUpdateMmiCodeResults(dataMessageParcel, reply);
}

int32_t UpdateAudioDeviceChange(const uint8_t *data, size_t size)
{
    if (!ServiceInited()) {
        return TELEPHONY_ERROR;
    }
    MessageParcel dataMessageParcel;
    if (!dataMessageParcel.WriteInterfaceToken(CallAbilityCallbackProxy::GetDescriptor())) {
        TELEPHONY_LOGE("write descriptor fail");
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    AudioDevice device;
    device.deviceType = AudioDeviceType::DEVICE_UNKNOWN;
    std::string msg(reinterpret_cast<const char *>(data), size);
    int32_t length = msg.length() > kMaxAddressLen ? kMaxAddressLen : msg.length();
    memcpy_s(device.address, kMaxAddressLen, msg.c_str(), length);
    int32_t dataSize = static_cast<uint32_t>(size);
    dataMessageParcel.WriteInt32(dataSize);
    dataMessageParcel.WriteRawData((const void *)&device, sizeof(AudioDevice));
    dataMessageParcel.WriteRawData((const void *)&device, sizeof(AudioDevice));
    dataMessageParcel.WriteBool(static_cast<uint32_t>(size % BOOL_NUM));
    dataMessageParcel.RewindRead(0);
    MessageParcel reply;
    MessageOption option;
    return callAbilityCallbackPtr_->OnUpdateAudioDeviceChange(dataMessageParcel, reply);
}

int32_t UpdateImsCallModeChange(const uint8_t *data, size_t size)
{
    if (!ServiceInited()) {
        return TELEPHONY_ERROR;
    }
    MessageParcel dataParcel;
    MessageParcel replyParcel;
    if (!dataParcel.WriteInterfaceToken(CallAbilityCallbackProxy::GetDescriptor())) {
        TELEPHONY_LOGE("write descriptor fail");
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    int32_t length = sizeof(CallMediaModeInfo);
    dataParcel.WriteInt32(length);
    CallMediaModeInfo callMediaModeInfo;
    callMediaModeInfo.callId = static_cast<int32_t>(size);
    callMediaModeInfo.isRequestInfo = static_cast<bool>(size % BOOL_NUM);
    callMediaModeInfo.result = static_cast<VideoRequestResultType>(size % VIDEO_REQUEST_RESULT_TYPE_NUM);
    callMediaModeInfo.callMode = static_cast<ImsCallMode>(size % IMS_CALL_MODE_NUM);
    dataParcel.WriteRawData((const void *)&callMediaModeInfo, length);
    dataParcel.RewindRead(0);
    return callAbilityCallbackPtr_->OnUpdateImsCallModeChange(dataParcel, replyParcel);
}

int32_t UpdateCallSessionEventChange(const uint8_t *data, size_t size)
{
    if (!ServiceInited()) {
        return TELEPHONY_ERROR;
    }
    MessageParcel dataParcel;
    MessageParcel replyParcel;
    if (!dataParcel.WriteInterfaceToken(CallAbilityCallbackProxy::GetDescriptor())) {
        TELEPHONY_LOGE("write descriptor fail");
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    int32_t length = sizeof(CallSessionEvent);
    dataParcel.WriteInt32(length);
    CallSessionEvent callSessionInfo;
    callSessionInfo.callId = static_cast<int32_t>(size);
    callSessionInfo.eventId = static_cast<CallSessionEventId>(size % CALL_SESSION_EVENT_ID_NUM);
    dataParcel.WriteRawData((const void *)&callSessionInfo, length);
    dataParcel.RewindRead(0);
    return callAbilityCallbackPtr_->OnUpdateCallSessionEventChange(dataParcel, replyParcel);
}

int32_t UpdatePeerDimensionsChange(const uint8_t *data, size_t size)
{
    if (!ServiceInited()) {
        return TELEPHONY_ERROR;
    }
    MessageParcel dataParcel;
    MessageParcel replyParcel;
    if (!dataParcel.WriteInterfaceToken(CallAbilityCallbackProxy::GetDescriptor())) {
        TELEPHONY_LOGE("write descriptor fail");
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    int32_t length = sizeof(PeerDimensionsDetail);
    dataParcel.WriteInt32(length);
    PeerDimensionsDetail peerDimensionsInfo;
    peerDimensionsInfo.callId = static_cast<int32_t>(size);
    peerDimensionsInfo.width = static_cast<int32_t>(size);
    peerDimensionsInfo.height = static_cast<int32_t>(size);
    dataParcel.RewindRead(0);
    return callAbilityCallbackPtr_->OnUpdatePeerDimensionsChange(dataParcel, replyParcel);
}

int32_t UpdateCallDataUsageChange(const uint8_t *data, size_t size)
{
    if (!ServiceInited()) {
        return TELEPHONY_ERROR;
    }
    MessageParcel dataParcel;
    MessageParcel replyParcel;
    if (!dataParcel.WriteInterfaceToken(CallAbilityCallbackProxy::GetDescriptor())) {
        TELEPHONY_LOGE("write descriptor fail");
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    int64_t dataUsage = static_cast<int64_t>(size);
    dataParcel.WriteInt64(dataUsage);
    dataParcel.RewindRead(0);
    return callAbilityCallbackPtr_->OnUpdateCallDataUsageChange(dataParcel, replyParcel);
}

int32_t UpdateCameraCapabilities(const uint8_t *data, size_t size)
{
    if (!ServiceInited()) {
        return TELEPHONY_ERROR;
    }
    MessageParcel dataParcel;
    MessageParcel replyParcel;
    if (!dataParcel.WriteInterfaceToken(CallAbilityCallbackProxy::GetDescriptor())) {
        TELEPHONY_LOGE("write descriptor fail");
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    int32_t length = sizeof(CameraCapabilities);
    dataParcel.WriteInt32(length);
    CameraCapabilities cameraCapabilitiesInfo;
    cameraCapabilitiesInfo.callId = static_cast<int32_t>(size);
    cameraCapabilitiesInfo.width = static_cast<int32_t>(size);
    cameraCapabilitiesInfo.height = static_cast<int32_t>(size);
    dataParcel.RewindRead(0);
    return callAbilityCallbackPtr_->OnUpdateCameraCapabilities(dataParcel, replyParcel);
}

void DoSomethingInterestingWithMyAPI(const uint8_t *data, size_t size)
{
    if (data == nullptr || size == 0) {
        return;
    }
    OnRemoteRequest(data, size);
    UpdateCallStateInfo(data, size);
    UpdateCallEvent(data, size);
    UpdateCallDisconnectedCause(data, size);
    UpdateAysncResults(data, size);
    UpdateOttCallRequest(data, size);
    UpdateMmiCodeResults(data, size);
    UpdateAudioDeviceChange(data, size);
    UpdateImsCallModeChange(data, size);
    UpdateCallSessionEventChange(data, size);
    UpdatePeerDimensionsChange(data, size);
    UpdateCallDataUsageChange(data, size);
    UpdateCameraCapabilities(data, size);
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
