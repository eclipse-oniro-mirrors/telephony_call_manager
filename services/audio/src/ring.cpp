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

#include "ring.h"

#include <thread>
#include <chrono>
#include "audio_player.h"
#include "call_base.h"
#include "telephony_log_wrapper.h"
#include "cpp/task_ext.h"
namespace OHOS {
namespace Telephony {
static constexpr int32_t DEFAULT_SIM_SLOT_ID = 0;

#ifdef OHOS_SUBSCRIBE_USER_STATUS_ENABLE
constexpr float START_INTENSITY = 1.0;
constexpr float END_INTENSITY = 100.0;
constexpr int32_t VOL_LEVEL_UNDER_LINE = 3;
constexpr int32_t TIMEOUT_LIMIT = 500; //ms
constexpr int32_t DECREASE_DURATION = 500000; //us
constexpr int32_t INCREASE_DURATION = 5000000; //us
constexpr int32_t ONE_SECOND = 1; //s
constexpr int32_t ONE_SECOND_US = 1000000; //us
constexpr int32_t US_TO_MS = 1000;
constexpr uint32_t FEATURE_COMFORT_REMINDER = 15;
const std::string ADAPTIVE_SWITCH = "ringtone_vibrate_adaptive_switch";
#endif

Ring::Ring()
{
    TELEPHONY_LOGI("Ring Create");
    Init();
}

Ring::~Ring()
{
    TELEPHONY_LOGI("~Ring Destory");
}

void Ring::Init()
{
    TELEPHONY_LOGI("Ring init");
    SystemSoundManager_ = Media::SystemSoundManagerFactory::CreateSystemSoundManager();
    if (SystemSoundManager_ == nullptr) {
        TELEPHONY_LOGE("get systemSoundManager failed");
        return;
    }

    audioPlayer_ = std::make_unique<AudioPlayer>();

    if (RingtonePlayer_ != nullptr) {
        RingtonePlayer_->Stop();
        RingtonePlayer_->Release();
        RingtonePlayer_.reset();
    }
}

int32_t Ring::Play(int32_t slotId, std::string ringtonePath, Media::HapticStartupMode mode)
{
    std::lock_guard<ffrt::mutex> lock(mutex_);
    if (SystemSoundManager_ == nullptr || audioPlayer_ == nullptr) {
        TELEPHONY_LOGE("SystemSoundManager_ or audioPlayer_ is nullptr");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    const std::shared_ptr<AbilityRuntime::Context> context;
    Media::RingtoneType type = slotId == DEFAULT_SIM_SLOT_ID ? Media::RingtoneType::RINGTONE_TYPE_SIM_CARD_0 :
        Media::RingtoneType::RINGTONE_TYPE_SIM_CARD_1;
    TELEPHONY_LOGI("ringtonePath: %{public}s", ringtonePath.c_str());
    RingtonePlayer_ = SystemSoundManager_->GetSpecificRingTonePlayer(context, type, ringtonePath);
    if (RingtonePlayer_ == nullptr) {
        TELEPHONY_LOGE("get RingtonePlayer failed");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    audioPlayer_->RegisterRingCallback(RingtonePlayer_);
    int32_t result = RingtonePlayer_->Configure(defaultVolume_, true);
    if (result != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("configure failed");
    }

#ifdef OHOS_SUBSCRIBE_USER_STATUS_ENABLE
    GetSettingsData();
    PrepareComfortReminder();
#endif

    audioPlayer_->SetStop(PlayerType::TYPE_RING, false);
    return RingtonePlayer_->Start(mode);
}

int32_t Ring::Stop()
{
    std::lock_guard<ffrt::mutex> lock(mutex_);
    if (audioPlayer_ == nullptr) {
        TELEPHONY_LOGE("audioPlayer_ is nullptr");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    int32_t result = TELEPHONY_SUCCESS;
    audioPlayer_->SetStop(PlayerType::TYPE_RING, true);
    if (RingtonePlayer_ == nullptr) {
        TELEPHONY_LOGE("RingtonePlayer_ is nullptr");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    result = RingtonePlayer_->Stop();
    isMutedRing_ = false;
#ifdef OHOS_SUBSCRIBE_USER_STATUS_ENABLE
    std::unique_lock<ffrt::mutex> lockRing(ringStopMutex_);
    isRingStopped_ = true;
    lockRing.unlock();
    ringStopCv_.notify_all();
    UnRegisterObserver();
    UnsubscribeFeature();
    ResetComfortReminder();
#endif
    return result;
}

void Ring::ReleaseRenderer()
{
    if (RingtonePlayer_ == nullptr) {
        TELEPHONY_LOGE("RingtonePlayer_ is nullptr");
        return;
    }
    RingtonePlayer_->Release();
}

int32_t Ring::SetMute()
{
    if (RingtonePlayer_ == nullptr) {
        TELEPHONY_LOGE("RingtonePlayer_ is nullptr");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    isMutedRing_ = true;
    return RingtonePlayer_->Configure(0, true);
}

int32_t Ring::SetRingToneVolume(float volume)
{
    TELEPHONY_LOGI("SetRingToneVolume volume = %{public}f", volume);
    if (isMutedRing_) {
        TELEPHONY_LOGI("ringTone is already muted");
        return TELEPHONY_SUCCESS;
    }
    if (RingtonePlayer_ == nullptr) {
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    if (volume >= 0.0f && volume <= 1.0f) {
        return RingtonePlayer_->Configure(volume, true);
    } else {
        TELEPHONY_LOGE("volume is valid");
        return TELEPHONY_ERR_FAIL;
    }
}

#ifdef OHOS_SUBSCRIBE_USER_STATUS_ENABLE
void Ring::RegisterObserver()
{
    if (ringtoneSettingStatusObserver_ != nullptr) {
        return;
    }
    ringtoneSettingStatusObserver_ = new (std::nothrow) RingtoneSettingStatusObserver();
    if (ringtoneSettingStatusObserver_ == nullptr) {
        TELEPHONY_LOGE("ringtoneSettingStatusObserver is null");
        return;
    }

    OHOS::Uri ringtoneSettingStatusUri(SettingsDataShareHelper::SETTINGS_DATASHARE_URI + "&key=" + ADAPTIVE_SWITCH);
    auto helper = DelayedSingleton<SettingsDataShareHelper>().GetInstance();
    if (!helper->RegisterToDataShare(ringtoneSettingStatusUri, ringtoneSettingStatusObserver_)) {
        TELEPHONY_LOGE("RegisterObserver failed");
    }
}

void Ring::UnRegisterObserver()
{
    if (ringtoneSettingStatusObserver_ == nullptr) {
        return;
    }
    OHOS::Uri ringtoneSettingStatusUri(SettingsDataShareHelper::SETTINGS_DATASHARE_URI + "&key=" +ADAPTIVE_SWITCH);
    auto helper = DelayedSingleton<SettingsDataShareHelper>().GetInstance();
    if (!helper->UnRegisterToDataShare(ringtoneSettingStatusUri, ringtoneSettingStatusObserver_)) {
        TELEPHONY_LOGE("UnRegisterObserver failed");
    }
    ringtoneSettingStatusObserver_ = nullptr;
}

void RingtoneSettingStatusObserver::OnChange()
{
}

void Ring::GetSettingsData()
{
    std::string ringtoneSettingStatus = "";
    auto settingHelper = SettingsDataShareHelper::GetInstance();
    if (settingHelper != nullptr) {
        OHOS::Uri settingUri(SettingsDataShareHelper::SETTINGS_DATASHARE_URI);
        settingHelper->Query(settingUri, ADAPTIVE_SWITCH, ringtoneSettingStatus);
    }
    TELEPHONY_LOGI("ringtone vibrate adaptive: %{public}s", ringtoneSettingStatus.c_str());
    isAdaptiveSwitchOn_ = ringtoneSettingStatus == "true";
}

void Ring::OnComfortReminderDataChanged(int32_t result,
    std::shared_ptr<Msdp::UserStatusAwareness::ComfortReminderData> comfortReminderData)
{
    if (!isAdaptiveSwitchOn_) {
        return;
    }

    int32_t fusionReminderData = comfortReminderData->GetFusionReminderData();
    int32_t eventType = comfortReminderData->GetEventType();
    TELEPHONY_LOGI("fusionReminderData = %{public}d, eventType = %{public}d", fusionReminderData, eventType);

    std::lock_guard<ffrt::mutex> lock(comfortReminderMutex_);
    switch (eventType) {
        case 0: // swing event
            isSwingMsgRecv_ = true;
            isSwing_ = fusionReminderData == 0;
            break;
        case 1: //env event
            isEnvMsgRecv_ = true;
            isQuiet_ = fusionReminderData == 0;
            break;
        default:
            TELEPHONY_LOGE("unknown eventType %{public}d", eventType);
            break;
    }
    if (isEnvMsgRecv_) {
        conditionVar_.notify_one();
        SetRingToneVibrationState();
    }
}

void Ring::SetRingToneVibrationState()
{
    if (RingtonePlayer_ == nullptr) {
        return;
    }
    TELEPHONY_LOGI("oriVolume %{public}d, curVolume %{public}d", oriRingVolLevel_, curRingVolLevel_.load());
    TELEPHONY_LOGI("swing = %{public}d, quiet = %{public}d", isSwing_, isQuiet_);
    TELEPHONY_LOGI("GentleHappend %{public}d, FadeupHappend %{public}d",
        isGentleHappend_.load(), isFadeupHappend_.load());
    int32_t ret;
    if (isSwing_ && !isGentleHappend_) {
        isGentleHappend_ = true;
        ret = RingtonePlayer_->SetRingtoneHapticsFeature(Media::RingtoneHapticsFeature::RINGTONE_GENTLE_HAPTICS);
        TELEPHONY_LOGI("vibration gentle, ret = %{public}d", ret);
        if (curRingVolLevel_ <= VOL_LEVEL_UNDER_LINE) {
            return;
        }
        std::weak_ptr<Ring> weakPtr = weak_from_this();
        ffrt::submit([weakPtr]() {
            std::shared_ptr<Ring> strongPtr = weakPtr.lock();
            if (strongPtr != nullptr) {
                strongPtr->DecreaseVolume();
            }
        });
    } else if (isQuiet_ && !isFadeupHappend_ && !isGentleHappend_) {
        isFadeupHappend_ = true;
        ret = RingtonePlayer_->SetRingtoneHapticsRamp(INCREASE_DURATION / US_TO_MS, START_INTENSITY, END_INTENSITY);
        TELEPHONY_LOGI("vibration fade up, ret = %{public}d", ret);
        if (oriRingVolLevel_ <= VOL_LEVEL_UNDER_LINE) {
            return;
        }
        std::weak_ptr<Ring> weakPtr = weak_from_this();
        ffrt::submit([weakPtr]() {
            std::shared_ptr<Ring> strongPtr = weakPtr.lock();
            if (strongPtr != nullptr) {
                strongPtr->IncreaseVolume();
            }
        });
    }
}

int32_t Ring::RegisterUserStatusDataCallbackFunc()
{
    if (!isAdaptiveSwitchOn_) {
        return TELEPHONY_SUCCESS;
    }
    Msdp::UserStatusAwareness::UserStatusDataCallbackFunc callback = [this] (int32_t result,
        std::shared_ptr<Msdp::UserStatusAwareness::UserStatusData> userStatusData) {
        auto comfortReminderData =
            std::static_pointer_cast<Msdp::UserStatusAwareness::ComfortReminderData>(userStatusData);
            if (comfortReminderData != nullptr) {
                OnComfortReminderDataChanged(result, comfortReminderData);
            }
        };
        int32_t subRet = Msdp::UserStatusAwareness::UserStatusClient::GetInstance().SubscribeCallback(
            FEATURE_COMFORT_REMINDER, callback);
        TELEPHONY_LOGI("Subscribe User Status Data ret: %{public}d", subRet);
        return subRet;
}

int32_t Ring::SubscribeFeature()
{
    if (!isAdaptiveSwitchOn_) {
        return TELEPHONY_SUCCESS;
    }
    const std::vector<OHOS::Msdp::UserStatusAwareness::DeviceInfo> deviceInfoList;
    int32_t ret = Msdp::UserStatusAwareness::UserStatusClient::GetInstance().Subscribe(
        FEATURE_COMFORT_REMINDER, deviceInfoList);
    TELEPHONY_LOGI("ring Subscribe feature, ret = %{public}d", ret);
    return TELEPHONY_SUCCESS;
}

int32_t Ring::UnsubscribeFeature()
{
    if (!isAdaptiveSwitchOn_) {
        return TELEPHONY_SUCCESS;
    }
    int32_t ret = Msdp::UserStatusAwareness::UserStatusClient::GetInstance().Unsubscribe(FEATURE_COMFORT_REMINDER);
    TELEPHONY_LOGI("ring Unsubscribe feature, ret = %{public}d", ret);
    return TELEPHONY_SUCCESS;
}

void Ring::ResetComfortReminder()
{
    isAdaptiveSwitchOn_ = false;
    isSwing_ = false;
    isQuiet_ = false;
    isSwingMsgRecv_ = false;
    isEnvMsgRecv_ = false;
    isGentleHappend_ = false;
    isFadeupHappend_ = false;
}
void Ring::PrepareComfortReminder()
{
    if (!isAdaptiveSwitchOn_) {
        return;
    }
    isRingStopped_ = false;
    auto audioProxy = DelayedSingleton<AudioProxy>::GetInstance();
    oriRingVolLevel_ = audioProxy->GetVolume(AudioStandard::AudioVolumeType::STREAM_RING);
    curRingVolLevel_ = oriRingVolLevel_;
    oriVolumeDb_ = audioProxy->GetSystemRingVolumeInDb(oriRingVolLevel_);
    TELEPHONY_LOGI("oriVolumeDb_:%{public}f", oriVolumeDb_);
    RegisterUserStatusDataCallbackFunc();
    SubscribeFeature();
    std::unique_lock<ffrt::mutex> lock(comfortReminderMutex_);
    if (conditionVar_.wait_for(lock, std::chrono::milliseconds(TIMEOUT_LIMIT), [this] { return isEnvMsgRecv_; })) {
        TELEPHONY_LOGI("reminder occurred");
        if (isQuiet_ && !isSwing_ && oriRingVolLevel_ > VOL_LEVEL_UNDER_LINE && oriVolumeDb_ > 0.0f) {
            float endVolumeDb = audioProxy->GetSystemRingVolumeInDb(VOL_LEVEL_UNDER_LINE);
            TELEPHONY_LOGI("ready to fade up, reset VolumeDb:%{public}f", endVolumeDb);
            SetRingToneVolume(endVolumeDb / oriVolumeDb_);
            curRingVolLevel_ = VOL_LEVEL_UNDER_LINE;
        } else {
            isFadeupHappend_ = true;
        }
    } else {
        isFadeupHappend_ = true;
        TELEPHONY_LOGI("reminder timeout");
    }
}

void Ring::DecreaseVolume()
{
    int totalSteps = curRingVolLevel_ - VOL_LEVEL_UNDER_LINE;
    if (oriVolumeDb_ <= 0.0f || totalSteps == 0) {
        TELEPHONY_LOGE("invalid oriVolumeDb %{public}f or steps %{public}d", oriVolumeDb_, totalSteps);
        return;
    }
    auto audioProxy = DelayedSingleton<AudioProxy>::GetInstance();
    int32_t ringVolume = curRingVolLevel_;
    const int interval = DECREASE_DURATION / totalSteps;
    TELEPHONY_LOGI("DecreaseVolume totalSteps %{public}d, interval %{public}d", totalSteps, interval);
    for (int i = 0; i < totalSteps; ++i) {
        std::unique_lock<ffrt::mutex> lock(ringStopMutex_);
        if (ringStopCv_.wait_for(lock, std::chrono::microseconds(interval),
            [this] {return isRingStopped_; })) {
            TELEPHONY_LOGI("DecreaseVolume interrupt by ring stop");
            return;
            }
            lock.unlock();
        ringVolume--;
        float endVolumeDb = audioProxy->GetSystemRingVolumeInDb(ringVolume);
        SetRingToneVolume(endVolumeDb / oriVolumeDb_);
    }
}

void Ring::IncreaseVolume()
{
    int totalSteps = oriRingVolLevel_ - VOL_LEVEL_UNDER_LINE;
    if (oriVolumeDb_ <= 0.0f || totalSteps == 0) {
        TELEPHONY_LOGE("invalid oriVolumeDb %{public}f or steps %{public}d", oriVolumeDb_, totalSteps);
        return;
    }
    auto audioProxy = DelayedSingleton<AudioProxy>::GetInstance();
    int32_t ringVolume = VOL_LEVEL_UNDER_LINE;
    const int interval = (INCREASE_DURATION - ONE_SECOND_US) / totalSteps;
    TELEPHONY_LOGI("IncreaseVolume totalSteps %{public}d, interval %{public}d", totalSteps, interval);
    std::unique_lock<ffrt::mutex> lock(ringStopMutex_);
    if (ringStopCv_.wait_for(lock, std::chrono::seconds(ONE_SECOND), [this] { return isRingStopped_; })) {
        TELEPHONY_LOGI("IncreaseVolume interrupt by ring stop in 1sec");
        return;
    }
    lock.unlock();
    for (int i = 0; i < totalSteps; ++i) {
        std::unique_lock<ffrt::mutex> innerLock(ringStopMutex_);
        if (ringStopCv_.wait_for(innerLock, std::chrono::microseconds(interval),
            [this] { return isRingStopped_; })) {
            TELEPHONY_LOGI("IncreaseVolume interrupt by ring stop");
            return;
        }
        innerLock.unlock();
        if (isGentleHappend_) {
            TELEPHONY_LOGI("IncreaseVolume interrupt by DecreaseVolume");
            return;
        }
        ringVolume++;
        float endVolumeDb = audioProxy->GetSystemRingVolumeInDb(ringVolume);
        SetRingToneVolume(endVolumeDb / oriVolumeDb_);
        curRingVolLevel_ = ringVolume;
    }
}
#endif //OHOS_SUBSCRIBE_USER_STATUS_ENABLE
} // namespace Telephony
} // namespace OHOS