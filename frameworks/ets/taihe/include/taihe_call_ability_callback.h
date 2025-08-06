/*
 * Copyright (C) 2021-2022 Huawei Device Co., Ltd.
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

#ifndef TAIHE_CALL_ABILITY_CALLBACK_H
#define TAIHE_CALL_ABILITY_CALLBACK_H

#include <string>
#include <mutex>
#include "call_manager_inner_type.h"

#include "ohos.telephony.call.proj.hpp"
#include "ohos.telephony.call.impl.hpp"

namespace OHOS {
namespace Telephony {

class TaiheCallAbilityCallback {
public:
    static TaiheCallAbilityCallback &GetInstance();

    // 禁止拷贝和赋值
    TaiheCallAbilityCallback(const TaiheCallAbilityCallback &) = delete;
    TaiheCallAbilityCallback &operator=(const TaiheCallAbilityCallback &) = delete;

    // 通话详情变化回调
    int32_t UpdateCallStateInfo(const CallAttributeInfo &info);

    // 通话事件变化回调
    int32_t UpdateCallEventInfo(const CallEventInfo &info);

    // 通话断开原因回调
    int32_t UpdateDisconnectedCause(const DisconnectedDetails &details);

    // MMI码结果回调
    int32_t UpdateMmiCodeResult(const MmiCodeInfo &info);

    // 音频设备变化回调
    int32_t UpdateAudioDeviceInfo(const AudioDeviceInfo &info);

    // 拨号后延迟回调
    int32_t UpdatePostDialDelay(const std::string &str);

    // IMS通话模式变化回调
    int32_t UpdateImsCallModeInfo(const CallMediaModeInfo &info);

    // 通话会话事件回调
    int32_t UpdateCallSessionEvent(const CallSessionEvent &event);

    // 对端尺寸变化回调
    int32_t UpdatePeerDimensions(const PeerDimensionsDetail &detail);

    // 相机能力变化回调
    int32_t UpdateCameraCapabilities(const CameraCapabilities &capabilities);

    ::taihe::optional<taihe::callback<void(::ohos::telephony::call::CallAttributeOptions const&)>> callStateCallback_;
    ::taihe::optional<taihe::callback<void(::ohos::telephony::call::CallEventOptions const&)>> callEventCallback_;;
    ::taihe::optional<taihe::callback<void(
        ::ohos::telephony::call::DisconnectedDetails const&)>> disconnectedCauseCallback_;
    ::taihe::optional<taihe::callback<void(::ohos::telephony::call::MmiCodeResults const&)>> mmiCodeResultCallback_;
    ::taihe::optional<taihe::callback<void(
        ::ohos::telephony::call::AudioDeviceCallbackInfo const&)>> audioDeviceCallback_;
    ::taihe::optional<taihe::callback<void(::taihe::string_view)>> postDialDelayCallback_;
    ::taihe::optional<taihe::callback<void(::ohos::telephony::call::ImsCallModeInfo const&)>> imsCallModeCallback_;
    ::taihe::optional<taihe::callback<void(
        ::ohos::telephony::call::CallSessionEvent const&)>> callSessionEventCallback_;
    ::taihe::optional<taihe::callback<void(
        ::ohos::telephony::call::PeerDimensionsDetail const&)>> peerDimensionsCallback_;
    ::taihe::optional<taihe::callback<void(
        ::ohos::telephony::call::CameraCapabilities const&)>> cameraCapabilitiesCallback_;

private:
    TaiheCallAbilityCallback() = default;
    ~TaiheCallAbilityCallback() = default;

    std::mutex callbackMutex_;
};

} // namespace Telephony
} // namespace OHOS

#endif // TAIHE_CALL_ABILITY_CALLBACK_H