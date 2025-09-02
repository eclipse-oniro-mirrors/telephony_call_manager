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

#ifndef TELEPHONY_AUDIO_RING_H
#define TELEPHONY_AUDIO_RING_H

#include <memory>
#include <atomic>
#include <mutex>
#include <condition_variable>

#include "audio_renderer.h"
#include "ffrt.h"
#include "audio_player.h"
#include "audio_proxy.h"
#include "ringtone_player.h"
#include "system_sound_manager.h"

#ifdef OHOS_SUBSCRIBE_USER_STATUS_ENABLE
#include "data_ability_observer_stub.h"
#include "comfort_reminder_data.h"
#include "settings_datashare_helper.h"
#include "datashare_helper.h"
#include "user_status_client.h"
#endif

namespace OHOS {
namespace Telephony {
enum class RingState {
    RINGING = 0,
    STOPPED,
};

#ifdef OHOS_SUBSCRIBE_USER_STATUS_ENABLE
class RingtoneSettingStatusObserver : public AAFwk::DataAbilityObserverStub {
public:
    RingtoneSettingStatusObserver() = default;
    ~RingtoneSettingStatusObserver() = default;
    void OnChange() override;
};
#endif

static constexpr const char* RING_PLAY_THREAD = "ringPlayThread";

/**
 * @class Ring
 * plays the default or specific ringtone.
 */
class Ring {
public:
    Ring();
    virtual ~Ring();
    void Init();
    int32_t Play(int32_t slotId, std::string ringtonePath, Media::HapticStartupMode mode);
    int32_t Stop();
    void ReleaseRenderer();
    int32_t SetMute();
    int32_t SetRingToneVolume(float volume);
    bool isMutedRing_ = false;

#ifdef OHOS_SUBSCRIBE_USER_STATUS_ENABLE
private:
    void RegisterObserver();
    void UnRegisterObserver();
    void getSettingsData();
    void onComfortReminderDataChanged(int32_t result,
        std::shared_ptr<Msdp::UserStatusAwareness::ComfortReminderData> comfortReminderData);
    int32_t RegisterUserStatusDataCallbackFunc();
    int32_t SubscribeFeature();
    int32_t UnsubscribeFeature();
    void PrepareComfortReminder();
    void SetRingToneVibrationState();
    void DecreaseVolume();
    void IncreaseVolume();
#endif

private:
    ffrt::mutex mutex_;
    AudioPlayer *audioPlayer_ = nullptr;
    std::shared_ptr<Media::SystemSoundManager> SystemSoundManager_ = nullptr;
    std::shared_ptr<Media::RingtonePlayer> RingtonePlayer_ = nullptr;
    int32_t defaultVolume_ = 1;

    #ifdef OHOS_SUBSCRIBE_USER_STATUS_ENABLE
    static constexpr uint32_t FEATURE_COMFORT_REMINDER = 15;
    bool ringtoneVibrationSwitchState_ = true;
    bool swing_ = false;
    bool quiet_ = false;
    bool isSwingMsgRecv_ = false;
    bool isEnvMsgRecv_ = false;
    int32_t oriRingVolLevel_ = 0;
    float oriVolumeDb_ = 0.0f;
    std::mutex comfortReminderMutex_;
    std::condition_variable conditionVar_;
    std::atomic<bool> isGentleHappend_{false};
    std::atomic<bool> isFadeupHappend_{false};
    std::atomic<int32_t> curRingVolLevel_{0};
    sptr<RingtoneSettingStatusObserver> ringtoneSettingStatusObserver_ = nullptr;
#endif
};
} // namespace Telephony
} // namespace OHOS
#endif // TELEPHONY_AUDIO_RING_H