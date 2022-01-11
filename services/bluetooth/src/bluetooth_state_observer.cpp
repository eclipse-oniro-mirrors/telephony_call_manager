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

#include "bluetooth_state_observer.h"

#include "telephony_log_wrapper.h"

#include "bluetooth_connection.h"
#include "audio_device_manager.h"

namespace OHOS {
namespace Telephony {
std::shared_ptr<BtEventSubscriber> BluetoothStateObserver::btEventSubscriber_ = nullptr;

BtEventSubscriber::BtEventSubscriber(const OHOS::EventFwk::CommonEventSubscribeInfo &subscriberInfo)
    : CommonEventSubscriber(subscriberInfo)
{}

void BtEventSubscriber::OnReceiveEvent(const OHOS::EventFwk::CommonEventData &data)
{
    OHOS::EventFwk::Want want = data.GetWant();
    std::string action = want.GetAction();
    if (action == BluetoothConnection::EVENT_BLUETOOTH_SCO_CONNECTED) {
        TELEPHONY_LOGI("bluetooth connection receive action : EVENT_BLUETOOTH_SCO_CONNECTED");
        BluetoothConnection::SetBtScoState(BtScoState::SCO_STATE_CONNECTED);
        DelayedSingleton<AudioDeviceManager>::GetInstance()->ProcessEvent(AudioEvent::BLUETOOTH_SCO_CONNECTED);
    } else if (action == BluetoothConnection::EVENT_BLUETOOTH_SCO_DISCONNECTED) {
        TELEPHONY_LOGI("bluetooth connection receive action : EVENT_BLUETOOTH_SCO_DISCONNECTED");
        BluetoothConnection::SetBtScoState(BtScoState::SCO_STATE_DISCONNECTED);
        DelayedSingleton<AudioDeviceManager>::GetInstance()->ProcessEvent(AudioEvent::BLUETOOTH_SCO_DISCONNECTED);
    }
}

BluetoothStateObserver::~BluetoothStateObserver()
{
    if (btEventSubscriber_ != nullptr) {
        UnSubscribeBluetoothEvent();
    }
}

bool BluetoothStateObserver::SubscribeBluetoothEvent()
{
    EventFwk::MatchingSkills matchingSkills;
    matchingSkills.AddEvent(BluetoothConnection::EVENT_BLUETOOTH_SCO_CONNECTED);
    matchingSkills.AddEvent(BluetoothConnection::EVENT_BLUETOOTH_SCO_DISCONNECTED);
    EventFwk::CommonEventSubscribeInfo subscriberInfo(matchingSkills);
    btEventSubscriber_ = std::make_shared<BtEventSubscriber>(subscriberInfo);
    if (btEventSubscriber_ == nullptr) {
        TELEPHONY_LOGE("bt connection nullptr");
        return false;
    }
    return EventFwk::CommonEventManager::SubscribeCommonEvent(btEventSubscriber_);
}

bool BluetoothStateObserver::UnSubscribeBluetoothEvent()
{
    if (btEventSubscriber_ == nullptr) {
        TELEPHONY_LOGE("bt connection nullptr");
        return false;
    }
    return EventFwk::CommonEventManager::UnSubscribeCommonEvent(btEventSubscriber_);
}
} // namespace Telephony
} // namespace OHOS