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

#include "audio_scene_processor.h"

#include "dialing_state.h"
#include "alerting_state.h"
#include "incoming_state.h"
#include "cs_call_state.h"
#include "holding_state.h"
#include "ims_call_state.h"
#include "inactive_state.h"
#include "audio_control_manager.h"
#include "ffrt.h"

#include "telephony_log_wrapper.h"

namespace OHOS {
namespace Telephony {
namespace {
    ffrt::queue reportAudioStateChangeQueue { "report_audio_state_change" };
}
AudioSceneProcessor::AudioSceneProcessor()
    : currentState_(nullptr)
{}

AudioSceneProcessor::~AudioSceneProcessor() {}

int32_t AudioSceneProcessor::Init()
{
    memberFuncMap_[AudioEvent::SWITCH_DIALING_STATE] = &AudioSceneProcessor::SwitchDialing;
    memberFuncMap_[AudioEvent::SWITCH_ALERTING_STATE] = &AudioSceneProcessor::SwitchAlerting;
    memberFuncMap_[AudioEvent::SWITCH_INCOMING_STATE] = &AudioSceneProcessor::SwitchIncoming;
    memberFuncMap_[AudioEvent::SWITCH_CS_CALL_STATE] = &AudioSceneProcessor::SwitchCS;
    memberFuncMap_[AudioEvent::SWITCH_IMS_CALL_STATE] = &AudioSceneProcessor::SwitchIMS;
    memberFuncMap_[AudioEvent::SWITCH_HOLDING_STATE] = &AudioSceneProcessor::SwitchHolding;
    memberFuncMap_[AudioEvent::SWITCH_AUDIO_INACTIVE_STATE] = &AudioSceneProcessor::SwitchInactive;
    currentState_ = std::make_unique<InActiveState>();
    if (currentState_ == nullptr) {
        TELEPHONY_LOGE("current call state nullptr");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    return TELEPHONY_SUCCESS;
}

void AudioSceneProcessor::ProcessEventInner(AudioEvent event)
{
    if (currentState_ == nullptr) {
        TELEPHONY_LOGE("current call state nullptr");
        return;
    }
    switch (event) {
        case AudioEvent::SWITCH_DIALING_STATE:
        case AudioEvent::SWITCH_ALERTING_STATE:
        case AudioEvent::SWITCH_INCOMING_STATE:
        case AudioEvent::SWITCH_CS_CALL_STATE:
        case AudioEvent::SWITCH_IMS_CALL_STATE:
        case AudioEvent::SWITCH_HOLDING_STATE:
        case AudioEvent::SWITCH_AUDIO_INACTIVE_STATE:
            SwitchState(event);
            break;
        case AudioEvent::NEW_ACTIVE_CS_CALL:
        case AudioEvent::NEW_ACTIVE_IMS_CALL:
        case AudioEvent::NEW_DIALING_CALL:
        case AudioEvent::NEW_ALERTING_CALL:
        case AudioEvent::NEW_INCOMING_CALL:
        case AudioEvent::NO_MORE_ACTIVE_CALL:
        case AudioEvent::NO_MORE_DIALING_CALL:
        case AudioEvent::NO_MORE_ALERTING_CALL:
        case AudioEvent::NO_MORE_INCOMING_CALL:
        case AudioEvent::NO_MORE_HOLDING_CALL:
            currentState_->ProcessEvent(event);
            break;
        default:
            break;
    }
}

bool AudioSceneProcessor::ProcessEvent(AudioEvent event)
{
    reportAudioStateChangeQueue.submit([=]() { ProcessEventInner(event); });
    return true;
}

bool AudioSceneProcessor::SwitchState(AudioEvent event)
{
    auto itFunc = memberFuncMap_.find(event);
    if (itFunc != memberFuncMap_.end() && itFunc->second != nullptr) {
        auto memberFunc = itFunc->second;
        return (this->*memberFunc)();
    }
    return false;
}

bool AudioSceneProcessor::SwitchState(CallStateType stateType)
{
    bool result = false;
    std::lock_guard<std::mutex> lock(mutex_);
    switch (stateType) {
        case CallStateType::DIALING_STATE:
            result = SwitchDialing();
            break;
        case CallStateType::ALERTING_STATE:
            result = SwitchAlerting();
            break;
        case CallStateType::INCOMING_STATE:
            result = SwitchIncoming();
            break;
        case CallStateType::CS_CALL_STATE:
            result = SwitchCS();
            break;
        case CallStateType::IMS_CALL_STATE:
            result = SwitchIMS();
            break;
        case CallStateType::HOLDING_STATE:
            result = SwitchHolding();
            break;
        case CallStateType::INACTIVE_STATE:
            result = SwitchInactive();
            break;
        default:
            break;
    }
    TELEPHONY_LOGI("switch call state lock release");
    return result;
}

bool AudioSceneProcessor::SwitchDialing()
{
    currentState_ = std::make_unique<DialingState>();
    if (currentState_ == nullptr) {
        TELEPHONY_LOGE("make_unique DialingState failed");
        return false;
    }
    if (!DelayedSingleton<AudioControlManager>::GetInstance()->PlaySoundtone()) {
        TELEPHONY_LOGE("PlaySoundtone fail");
    }
    
    if (!DelayedSingleton<AudioDeviceManager>::GetInstance()->ProcessEvent(AudioEvent::AUDIO_ACTIVATED)) {
        TELEPHONY_LOGE("ProcessEvent AUDIO_ACTIVATED failed");
    }
    TELEPHONY_LOGI("current call state : dialing state");
    return true;
}

bool AudioSceneProcessor::SwitchAlerting()
{
    currentState_ = std::make_unique<AlertingState>();
    if (currentState_ == nullptr) {
        TELEPHONY_LOGE("make_unique AlertingState failed");
        return false;
    }
    // play ringback tone while alerting state
    DelayedSingleton<AudioControlManager>::GetInstance()->PlayRingback();
    TELEPHONY_LOGI("current call state : alerting state");
    return true;
}

bool AudioSceneProcessor::SwitchIncoming()
{
    currentState_ = std::make_unique<IncomingState>();
    if (currentState_ == nullptr) {
        TELEPHONY_LOGE("make_unique IncomingState failed");
        return false;
    }
    DelayedSingleton<AudioControlManager>::GetInstance()->StopRingtone();
    // play ringtone while incoming state
    DelayedSingleton<AudioControlManager>::GetInstance()->PlayRingtone();
    DelayedSingleton<AudioDeviceManager>::GetInstance()->ProcessEvent(AudioEvent::AUDIO_RINGING);
    TELEPHONY_LOGI("current call state : incoming state");
    return true;
}

bool AudioSceneProcessor::SwitchCS()
{
    currentState_ = std::make_unique<CSCallState>();
    if (currentState_ == nullptr) {
        TELEPHONY_LOGE("make_unique CSCallState failed");
        return false;
    }
    TELEPHONY_LOGI("current call state : cs call state");
    return true;
}

bool AudioSceneProcessor::SwitchIMS()
{
    currentState_ = std::make_unique<IMSCallState>();
    if (currentState_ == nullptr) {
        TELEPHONY_LOGE("make_unique IMSCallState failed");
        return false;
    }
    TELEPHONY_LOGI("current call state : ims call state");
    return true;
}

bool AudioSceneProcessor::SwitchHolding()
{
    currentState_ = std::make_unique<HoldingState>();
    if (currentState_ == nullptr) {
        TELEPHONY_LOGE("make_unique HoldingState failed");
        return false;
    }
    TELEPHONY_LOGI("current call state : holding state");
    return true;
}

bool AudioSceneProcessor::SwitchInactive()
{
    DelayedSingleton<AudioDeviceManager>::GetInstance()->ProcessEvent(AudioEvent::AUDIO_DEACTIVATED);
    currentState_ = std::make_unique<InActiveState>();
    if (currentState_ == nullptr) {
        TELEPHONY_LOGE("make_unique InActiveState failed");
        return false;
    }
    TELEPHONY_LOGI("current call state : inactive state");
    return true;
}

bool AudioSceneProcessor::SwitchOTT()
{
    return true;
}
} // namespace Telephony
} // namespace OHOS