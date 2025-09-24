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

#ifndef TELEPHONY_AUDIO_SCENE_PROCESSOR_H
#define TELEPHONY_AUDIO_SCENE_PROCESSOR_H

#include "audio_base.h"

#include <map>

#include "singleton.h"
#include "audio_proxy.h"

namespace OHOS {
namespace Telephony {
enum CallStateType {
    INACTIVE_STATE = 0,
    DIALING_STATE,
    ALERTING_STATE,
    INCOMING_STATE,
    CS_CALL_STATE,
    IMS_CALL_STATE,
    HOLDING_STATE,
    UNKNOWN_STATE
};

/**
 * @class AudioSceneProcessor
 * describes the available audio scenes of a call.
 */
class AudioSceneProcessor : public std::enable_shared_from_this<AudioSceneProcessor> {
    DECLARE_DELAYED_SINGLETON(AudioSceneProcessor)
public:
    int32_t Init();
    bool ProcessEvent(AudioEvent event);

private:
    ffrt::mutex mutex_;
    bool SwitchState(AudioEvent event);
    bool SwitchState(CallStateType state);
    bool SwitchCS();
    bool SwitchIMS();
    bool SwitchOTT();
    bool SwitchDialing();
    bool SwitchAlerting();
    bool SwitchIncoming();
    bool SwitchHolding();
    bool SwitchInactive();
    bool ActivateAudioInterrupt(const AudioStandard::AudioStreamType &streamType);
    bool DeactivateAudioInterrupt();
    void ProcessEventInner(AudioEvent event);
    std::unique_ptr<AudioBase> currentState_;
    using AudioSceneProcessorFunc = std::function<bool()>;
    std::map<uint32_t, AudioSceneProcessorFunc> memberFuncMap_;
};
} // namespace Telephony
} // namespace OHOS
#endif // TELEPHONY_AUDIO_SCENE_PROCESSOR_H
