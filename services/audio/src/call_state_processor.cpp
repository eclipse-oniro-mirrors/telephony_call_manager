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

#include "call_state_processor.h"

#include "alerting_state.h"
#include "incoming_state.h"
#include "cs_call_state.h"
#include "holding_state.h"
#include "ims_call_state.h"
#include "inactive_state.h"
#include "audio_control_manager.h"

#include "telephony_log_wrapper.h"

namespace OHOS {
namespace Telephony {
CallStateProcessor::CallStateProcessor() : currentAudioMode_(IN_IDLE), currentCallState_(nullptr) {}

CallStateProcessor::~CallStateProcessor() {}

int32_t CallStateProcessor::Init()
{
    memberFuncMap_[AudioEvent::SWITCH_ALERTING_STATE] = &CallStateProcessor::SwitchAlerting;
    memberFuncMap_[AudioEvent::SWITCH_INCOMING_STATE] = &CallStateProcessor::SwitchIncoming;
    memberFuncMap_[AudioEvent::SWITCH_CS_CALL_STATE] = &CallStateProcessor::SwitchCS;
    memberFuncMap_[AudioEvent::SWITCH_IMS_CALL_STATE] = &CallStateProcessor::SwitchIMS;
    memberFuncMap_[AudioEvent::SWITCH_HOLDING_STATE] = &CallStateProcessor::SwitchHolding;
    memberFuncMap_[AudioEvent::SWITCH_AUDIO_INACTIVE_STATE] = &CallStateProcessor::SwitchInactive;
    currentCallState_ = std::make_unique<InActiveState>();
    if (currentCallState_ == nullptr) {
        TELEPHONY_LOGE("current call state nullptr");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    return TELEPHONY_SUCCESS;
}

bool CallStateProcessor::ProcessEvent(AudioEvent event)
{
    if (currentCallState_ == nullptr) {
        TELEPHONY_LOGE("current call state nullptr");
        return false;
    }
    bool result = false;
    switch (event) {
        case AudioEvent::SWITCH_ALERTING_STATE:
        case AudioEvent::SWITCH_INCOMING_STATE:
        case AudioEvent::SWITCH_CS_CALL_STATE:
        case AudioEvent::SWITCH_IMS_CALL_STATE:
        case AudioEvent::SWITCH_HOLDING_STATE:
        case AudioEvent::SWITCH_AUDIO_INACTIVE_STATE:
            result = SwitchState(event);
            break;
        case AudioEvent::NEW_ACTIVE_CS_CALL:
        case AudioEvent::NEW_ACTIVE_IMS_CALL:
        case AudioEvent::NEW_ALERTING_CALL:
        case AudioEvent::NEW_INCOMING_CALL:
        case AudioEvent::NO_MORE_ACTIVE_CALL:
        case AudioEvent::NO_MORE_ALERTING_CALL:
        case AudioEvent::NO_MORE_INCOMING_CALL:
            result = currentCallState_->ProcessEvent(event);
            break;
        default:
            break;
    }
    return result;
}

bool CallStateProcessor::SwitchState(AudioEvent event)
{
    auto itFunc = memberFuncMap_.find(event);
    if (itFunc != memberFuncMap_.end() && itFunc->second != nullptr) {
        auto memberFunc = itFunc->second;
        return (this->*memberFunc)();
    }
    return false;
}

bool CallStateProcessor::SwitchState(CallStateType stateType)
{
    bool result = false;
    std::lock_guard<std::mutex> lock(mutex_);
    switch (stateType) {
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

bool CallStateProcessor::SwitchAlerting()
{
    if (!ActivateAudioInterrupt()) {
        TELEPHONY_LOGE("activate audio interrupt failed");
        return false;
    }
    if (DelayedSingleton<AudioProxy>::GetInstance()->SetAudioMode(AudioCallState::IN_IDLE)) {
        currentCallState_ = std::make_unique<AlertingState>();
        if (currentCallState_ == nullptr) {
            TELEPHONY_LOGE("make_unique AlertingState failed");
            return false;
        }
        DelayedSingleton<AudioControlManager>::GetInstance()->PlayRingback();
        currentAudioMode_ = AudioCallState::IN_IDLE;
        TELEPHONY_LOGI("current call state : alerting state");
        return true;
    }
    return false;
}

bool CallStateProcessor::SwitchIncoming()
{
    if (!ActivateAudioInterrupt()) {
        TELEPHONY_LOGE("activate audio interrupt failed");
        return false;
    }
    if (DelayedSingleton<AudioProxy>::GetInstance()->SetAudioMode(AudioCallState::RINGTONE)) {
        currentCallState_ = std::make_unique<IncomingState>();
        if (currentCallState_ == nullptr) {
            TELEPHONY_LOGE("make_unique IncomingState failed");
            return false;
        }
        DelayedSingleton<AudioControlManager>::GetInstance()->PlayRingtone(); // play ringtone while incoming state
        currentAudioMode_ = AudioCallState::RINGTONE;
        TELEPHONY_LOGI("current call state : incoming state");
        return true;
    }
    return false;
}

bool CallStateProcessor::SwitchCS()
{
    if (!ActivateAudioInterrupt()) {
        TELEPHONY_LOGE("activate audio interrupt failed");
        return false;
    }
    if (DelayedSingleton<AudioProxy>::GetInstance()->SetAudioMode(AudioCallState::IN_CALL)) {
        currentCallState_ = std::make_unique<CSCallState>();
        if (currentCallState_ == nullptr) {
            TELEPHONY_LOGE("make_unique CSCallState failed");
            return false;
        }
        currentAudioMode_ = AudioCallState::IN_CALL;
        TELEPHONY_LOGI("current call state : cs call state");
        return true;
    }
    return false;
}

bool CallStateProcessor::SwitchIMS()
{
    if (!ActivateAudioInterrupt()) {
        TELEPHONY_LOGE("activate audio interrupt failed");
        return false;
    }
    if (DelayedSingleton<AudioProxy>::GetInstance()->SetAudioMode(AudioCallState::IN_VOIP)) {
        currentCallState_ = std::make_unique<IMSCallState>();
        if (currentCallState_ == nullptr) {
            TELEPHONY_LOGE("make_unique IMSCallState failed");
            return false;
        }
        currentAudioMode_ = AudioCallState::IN_VOIP;
        TELEPHONY_LOGI("current call state : ims call state");
        return true;
    }
    return false;
}

bool CallStateProcessor::SwitchHolding()
{
    if (DelayedSingleton<AudioProxy>::GetInstance()->SetAudioMode(currentAudioMode_)) {
        currentCallState_ = std::make_unique<HoldingState>();
        if (currentCallState_ == nullptr) {
            TELEPHONY_LOGE("make_unique HoldingState failed");
            return false;
        }
        TELEPHONY_LOGI("current call state : holding state");
        return true;
    }
    return false;
}

bool CallStateProcessor::SwitchInactive()
{
    if (!DeactivateAudioInterrupt()) {
        TELEPHONY_LOGE("deactivate audio interrupt failed");
        return false;
    }
    if (DelayedSingleton<AudioProxy>::GetInstance()->SetAudioMode(AudioCallState::IN_IDLE)) {
        currentCallState_ = std::make_unique<InActiveState>();
        if (currentCallState_ == nullptr) {
            TELEPHONY_LOGE("make_unique InActiveState failed");
            return false;
        }
        currentAudioMode_ = AudioCallState::IN_IDLE;
        TELEPHONY_LOGI("current call state : inactive state");
        return true;
    }
    return false;
}

bool CallStateProcessor::SwitchOTT()
{
    if (!ActivateAudioInterrupt()) {
        TELEPHONY_LOGE("activate audio interrupt failed");
        return false;
    }
    return true;
}

bool CallStateProcessor::ActivateAudioInterrupt()
{
    int32_t ret = DelayedSingleton<AudioProxy>::GetInstance()->ActivateAudioInterrupt();
    if (ret == TELEPHONY_SUCCESS) {
        DelayedSingleton<AudioControlManager>::GetInstance()->SetAudioInterruptState(
            AudioInterruptState::INTERRUPT_STATE_ACTIVATED);
        return true;
    }
    return false;
}

bool CallStateProcessor::DeactivateAudioInterrupt()
{
    int32_t ret = DelayedSingleton<AudioProxy>::GetInstance()->DeactivateAudioInterrupt();
    if (ret == TELEPHONY_SUCCESS) {
        DelayedSingleton<AudioControlManager>::GetInstance()->SetAudioInterruptState(
            AudioInterruptState::INTERRUPT_STATE_DEACTIVATED);
        return true;
    }
    return false;
}
} // namespace Telephony
} // namespace OHOS