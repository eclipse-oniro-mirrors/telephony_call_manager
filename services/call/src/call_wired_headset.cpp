/*
 * Copyright (C) 2024 Huawei Device Co., Ltd.
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

#include "call_object_manager.h"
#include "call_wired_headset.h"

#include "input_manager.h"
#include "audio_device_manager.h"
#include "audio_control_manager.h"
#include "call_manager_base.h"
#include "system_ability_definition.h"
#include "call_control_manager.h"
#include "audio_proxy.h"
#include "call_manager_base.h"
#include "ims_conference.h"

namespace OHOS {
namespace Telephony {

using WiredHeadSetCallback = std::function<void(const std::shared_ptr<OHOS::MMI::KeyEvent>)>;

constexpr int32_t INVALID_VALUE = -1;
constexpr int32_t FINAL_KEY_DOWN_DURATION_TWO = 2000;
CallWiredHeadSet::CallWiredHeadSet()
{
    isProcessed_ = false;
    downFirstTime_ = 0;
    subscribeIdForPressedUp_ = INVALID_VALUE;
    subscribeIdForPressedDown_ = INVALID_VALUE;
}

CallWiredHeadSet::~CallWiredHeadSet()
{
    DeInit();
}

bool CallWiredHeadSet::Init()
{
    bool pressedUp = RegistKeyMutePressedUp();
    bool pressedDown = RegistKeyMutePressedDown();
    if (pressedUp && pressedDown) {
        TELEPHONY_LOGI("CallWiredHeadSet Registed KeyMute Pressed callback succeed");
        return true;
    } else {
        TELEPHONY_LOGE("CallWiredHeadSet Registed KeyMute Pressed callback failed,"
            "pressedUp is %{public}s, pressedDown is %{public}s",
            pressedUp ? "true" : "false", pressedDown ? "true" : "false");
        return false;
    }
}

void CallWiredHeadSet::DeInit()
{
    UnregistKeyMutePressedUp();
    UnregistKeyMutePressedDown();
    TELEPHONY_LOGI("CallWiredHeadSet UnRegisted KeyMute Pressed callback succeed");
}

std::shared_ptr<MMI::KeyOption> CallWiredHeadSet::InitOption(
    const std::set<int32_t> &preKeys, int32_t finalKey, bool isFinalKeyDown, int32_t duration)
{
    std::shared_ptr<MMI::KeyOption> keyOption = std::make_shared<MMI::KeyOption>();
    keyOption->SetFinalKeyDown(isFinalKeyDown);
    keyOption->SetFinalKey(finalKey);
    keyOption->SetPreKeys(preKeys);
    keyOption->SetFinalKeyDownDuration(duration);
    return keyOption;
}

bool CallWiredHeadSet::RegistKeyMutePressedUp()
{
    WiredHeadSetCallback pressedFunc = [this](const std::shared_ptr<OHOS::MMI::KeyEvent> event) {
        this->DealKeyMutePressedUp(event);
    };

    std::set<int32_t> preKeys;
    std::shared_ptr<MMI::KeyOption> keyOption = InitOption(preKeys, MMI::KeyEvent::KEYCODE_HEADSETHOOK, false, 0);
    keyOption->SetPriority(MMI::SubscriberPriority::PRIORITY_100);
    subscribeIdForPressedUp_ = MMI::InputManager::GetInstance()->SubscribeKeyEvent(keyOption, pressedFunc);
    TELEPHONY_LOGI("subscribeIdForPressedUp_: %{public}d", subscribeIdForPressedUp_);
    return subscribeIdForPressedUp_ < 0 ? false : true;
}

bool CallWiredHeadSet::RegistKeyMutePressedDown()
{
    WiredHeadSetCallback pressedFunc = [this](const std::shared_ptr<OHOS::MMI::KeyEvent> event) {
        this->DealKeyMutePressedDown(event);
    };

    std::set<int32_t> preKeys;
    std::shared_ptr<MMI::KeyOption> keyOption = InitOption(preKeys, MMI::KeyEvent::KEYCODE_HEADSETHOOK, true, 0);
    keyOption->SetPriority(MMI::SubscriberPriority::PRIORITY_100);
    subscribeIdForPressedDown_ = MMI::InputManager::GetInstance()->SubscribeKeyEvent(keyOption, pressedFunc);
    TELEPHONY_LOGI("subscribeIdForPressedDown_: %{public}d", subscribeIdForPressedDown_);
    return subscribeIdForPressedDown_ < 0 ? false : true;
}

void CallWiredHeadSet::UnregistKeyMutePressedUp()
{
    if (subscribeIdForPressedUp_ > 0) {
        MMI::InputManager::GetInstance()->UnsubscribeKeyEvent(subscribeIdForPressedUp_);
    }
}

void CallWiredHeadSet::UnregistKeyMutePressedDown()
{
    if (subscribeIdForPressedDown_ > 0) {
        MMI::InputManager::GetInstance()->UnsubscribeKeyEvent(subscribeIdForPressedDown_);
    }
}

// wired headset mute key pressed callback method
void CallWiredHeadSet::DealKeyMutePressedUp(std::shared_ptr<MMI::KeyEvent> event)
{
    TELEPHONY_LOGI("received KeyMute Pressed Up event");
    if (DelayedSingleton<AudioDeviceManager>::GetInstance()->IsWiredHeadsetConnected() == true) {
        time_t currentTime = GetCurrentTimeMS();
        if ((currentTime - downFirstTime_) < FINAL_KEY_DOWN_DURATION_TWO) {
            DealKeyMuteShortPressed();
        } else if (!isProcessed_) {
            DealKeyMuteLongPressed();
        }
    }
    downFirstTime_ = 0;
    isProcessed_ = false;
}

void CallWiredHeadSet::DealKeyMutePressedDown(std::shared_ptr<MMI::KeyEvent> event)
{
    TELEPHONY_LOGI("received KeyMute Pressed Down event");
    if (isProcessed_) {
        return;
    }
    if (DelayedSingleton<AudioDeviceManager>::GetInstance()->IsWiredHeadsetConnected() == true) {
        time_t currentTime = GetCurrentTimeMS();
        if (downFirstTime_ == 0) {
            downFirstTime_ = GetCurrentTimeMS();
            return;
        }
        if (!isProcessed_ && (currentTime - downFirstTime_ >= FINAL_KEY_DOWN_DURATION_TWO)) {
            DealKeyMuteLongPressed();
            isProcessed_ = true;
        }
    }
}

// wired headset mute key short pressed callback method
void CallWiredHeadSet::DealKeyMuteShortPressed()
{
    sptr<CallBase> ringingCall = CallObjectManager::GetOneCallObject(CallRunningState::CALL_RUNNING_STATE_RINGING);
    if (ringingCall == nullptr) {
        sptr<CallBase> holdCall = CallObjectManager::GetOneCallObject(CallRunningState::CALL_RUNNING_STATE_HOLD);
        sptr<CallBase> activeCall = CallObjectManager::GetOneCallObject(CallRunningState::CALL_RUNNING_STATE_ACTIVE);
        if (activeCall != nullptr && holdCall != nullptr) {
            TELEPHONY_LOGI("DealKeyMuteShortPressed UnHoldCall callid(%{public}d)", holdCall->GetCallID());
            DelayedSingleton<CallControlManager>::GetInstance()->UnHoldCall(holdCall->GetCallID());
            return;
        }
        sptr<CallBase> call = CallObjectManager::GetAudioLiveCall();
        if (call != nullptr) {
            bool isMuted = DelayedSingleton<AudioProxy>::GetInstance()->IsMicrophoneMute();
            TELEPHONY_LOGI("DealKeyMuteShortPressed SetMuted isMuted((%{public}d))", (!isMuted));
            DelayedSingleton<CallControlManager>::GetInstance()->SetMuted(!isMuted);
        }
    } else {
        TELEPHONY_LOGI("DealKeyMuteShortPressed AnswerCall callid(%{public}d)", ringingCall->GetCallID());
        VideoStateType videoState = ringingCall->GetVideoStateType();
        if (videoState != VideoStateType::TYPE_VOICE && videoState != VideoStateType::TYPE_VIDEO) {
            TELEPHONY_LOGI("DealKeyMuteShortPressed get original call type");
            videoState = static_cast<VideoStateType>(ringingCall->GetOriginalCallType());
        }
        DelayedSingleton<CallControlManager>::GetInstance()->AnswerCall(
            ringingCall->GetCallID(), static_cast<int32_t>(videoState));
    }
}

// wired headset mute key long pressed callback method
void CallWiredHeadSet::DealKeyMuteLongPressed()
{
    sptr<CallBase> ringingCall = CallObjectManager::GetOneCallObject(CallRunningState::CALL_RUNNING_STATE_RINGING);
    if (ringingCall != nullptr) {
        TELEPHONY_LOGI("DealKeyMuteLongPressed RejectCall callid(%{public}d)", ringingCall->GetCallID());
        ringingCall->RejectCall();
    } else {
        sptr<CallBase> foregroundCall = CallObjectManager::GetForegroundCall();
        if (foregroundCall != nullptr) {
            TelConferenceState confState = foregroundCall->GetTelConferenceState();
            if (confState != TelConferenceState::TEL_CONFERENCE_IDLE) {
                int32_t conferenceId = DelayedSingleton<ImsConference>::GetInstance()->GetMainCall();
                TELEPHONY_LOGI("DealKeyMuteLongPressed HangUpCall conferenceId(%{public}d)", conferenceId);
                DelayedSingleton<CallControlManager>::GetInstance()->HangUpCall(conferenceId);
            } else {
                TELEPHONY_LOGI("DealKeyMuteLongPressed HangUpCall callid(%{public}d)", foregroundCall->GetCallID());
                foregroundCall->HangUpCall();
            }
        }
    }
}

time_t CallWiredHeadSet::GetCurrentTimeMS()
{
    std::chrono::time_point<std::chrono::system_clock, std::chrono::milliseconds> tpMicro
        = std::chrono::time_point_cast<std::chrono::milliseconds>(std::chrono::system_clock::now());
    return tpMicro.time_since_epoch().count();
}
} // namespace Telephony
} // namespace OHOS