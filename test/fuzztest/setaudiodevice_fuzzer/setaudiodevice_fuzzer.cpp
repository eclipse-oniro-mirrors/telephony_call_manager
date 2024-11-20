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

#include "setaudiodevice_fuzzer.h"

#include <cstddef>
#include <cstdint>
#define private public
#include "addcalltoken_fuzzer.h"
#include "audio_control_manager.h"

using namespace OHOS::Telephony;
namespace OHOS {
constexpr int32_t AUDIO_DEVICE_NUM = 6;

void SetAudioDevice(const uint8_t *data, size_t size)
{
    if (!IsServiceInited()) {
        return;
    }
    std::string address(reinterpret_cast<const char *>(data), size);
    AudioDevice audioDevice;
    if (memset_s(&audioDevice, sizeof(AudioDevice), 0, sizeof(AudioDevice)) != EOK) {
        TELEPHONY_LOGE("memset_s fail");
        return;
    }
    audioDevice.deviceType = static_cast<AudioDeviceType>(size % AUDIO_DEVICE_NUM);
    if (address.length() > kMaxAddressLen) {
        TELEPHONY_LOGE("address is not too long");
        return;
    }
    if (memcpy_s(audioDevice.address, kMaxAddressLen, address.c_str(), address.length()) != EOK) {
        return;
    }
    MessageParcel messageParcel;
    messageParcel.WriteRawData((const void *)&audioDevice, sizeof(AudioDevice));
    messageParcel.RewindRead(0);

    MessageParcel reply;
    DelayedSingleton<CallManagerService>::GetInstance()->OnSetAudioDevice(messageParcel, reply);
}

void DoSomethingInterestingWithMyAPI(const uint8_t *data, size_t size)
{
    if (data == nullptr || size == 0) {
        return;
    }
    DelayedSingleton<AudioProxy>::GetInstance()->SetAudioMicStateChangeCallback();
    DelayedSingleton<AudioProxy>::GetInstance()->SetAudioDeviceChangeCallback();
    DelayedSingleton<AudioProxy>::GetInstance()->SetAudioPreferDeviceChangeCallback();
    SetAudioDevice(data, size);
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
