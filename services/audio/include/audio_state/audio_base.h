/*
 * Copyright (C) 2021 Huawei Device Co., Ltd.
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

#ifndef TELEPHONY_AUDIO_BASE_H
#define TELEPHONY_AUDIO_BASE_H

#include <cstdint>
#include <mutex>

namespace OHOS {
namespace Telephony {
enum AudioEvent {
    UNKNOWN_EVENT = 0,
    // audio focus event
    AUDIO_ACTIVATED,
    AUDIO_RINGING,
    AUDIO_DEACTIVATED,
    // call state event
    SWITCH_CS_CALL_STATE,
    SWITCH_IMS_CALL_STATE,
    SWITCH_OTT_CALL_STATE,
    SWITCH_DIALING_STATE,
    SWITCH_ALERTING_STATE,
    SWITCH_INCOMING_STATE,
    SWITCH_HOLDING_STATE,
    SWITCH_AUDIO_INACTIVE_STATE,
    NEW_ACTIVE_CS_CALL,
    NEW_ACTIVE_IMS_CALL,
    NEW_ACTIVE_OTT_CALL,
    NEW_DIALING_CALL,
    NEW_ALERTING_CALL,
    NEW_INCOMING_CALL,
    NO_MORE_ACTIVE_CALL,
    NO_MORE_DIALING_CALL,
    NO_MORE_ALERTING_CALL,
    NO_MORE_INCOMING_CALL,
    NO_MORE_HOLDING_CALL,
    CALL_TYPE_CS_CHANGE_IMS,
    CALL_TYPE_IMS_CHANGE_CS,
    // audio device event
    ENABLE_DEVICE_EARPIECE,
    ENABLE_DEVICE_SPEAKER,
    ENABLE_DEVICE_WIRED_HEADSET,
    ENABLE_DEVICE_BLUETOOTH,
    WIRED_HEADSET_CONNECTED,
    WIRED_HEADSET_DISCONNECTED,
    BLUETOOTH_SCO_CONNECTED,
    BLUETOOTH_SCO_DISCONNECTED,
    INIT_AUDIO_DEVICE
};

class AudioBase {
public:
    AudioBase() {}
    virtual ~AudioBase() {}
    // handle audio events in current state
    virtual bool ProcessEvent(int32_t event) = 0;

protected:
    ffrt::ffrt mutex_;
};
} // namespace Telephony
} // namespace OHOS
#endif // TELEPHONY_AUDIO_BASE_H