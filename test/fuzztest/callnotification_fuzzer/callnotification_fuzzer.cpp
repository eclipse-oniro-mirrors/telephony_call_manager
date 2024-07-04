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

#include "callnotification_fuzzer.h"

#include <cstddef>
#include <cstdint>
#include <securec.h>
#define private public
#include "addcalltoken_fuzzer.h"
#include "call_data_base_helper.h"
#include "cs_call.h"
#include "incoming_call_notification.h"
#include "incoming_call_wake_up.h"
#include "missed_call_notification.h"
#include "proximity_sensor.h"
#include "reject_call_sms.h"
#include "status_bar.h"
#include "wired_headset.h"

using namespace OHOS::Telephony;
namespace OHOS {
constexpr int32_t CALL_STATE_NUM = 8;
constexpr int32_t BOOL_NUM = 2;

void IncomingCallNotificationFunc(const uint8_t *data, size_t size)
{
    if (!IsServiceInited()) {
        return;
    }

    std::shared_ptr<IncomingCallNotification> notification = std::make_shared<IncomingCallNotification>();
    DialParaInfo paraInfo;
    sptr<CallBase> callObjectPtr = std::make_unique<CSCall>(paraInfo).release();
    TelCallState priorState = static_cast<TelCallState>(size % CALL_STATE_NUM);
    TelCallState nextState = static_cast<TelCallState>(size % CALL_STATE_NUM);
    std::string message(reinterpret_cast<const char *>(data), size);
    DisconnectedDetails details;

    notification->NewCallCreated(callObjectPtr);
    notification->CallStateUpdated(callObjectPtr, priorState, nextState);
    notification->IncomingCallActivated(callObjectPtr);
    notification->IncomingCallHungUp(callObjectPtr, true, message);
    notification->CallDestroyed(details);
    notification->PublishIncomingCallNotification(callObjectPtr);
    notification->CancelIncomingCallNotification();
    notification->IsFullScreen();
}

void IncomingCallWakeupFunc(const uint8_t *data, size_t size)
{
    if (!IsServiceInited()) {
        return;
    }

    std::shared_ptr<IncomingCallWakeup> notification = std::make_shared<IncomingCallWakeup>();
    DialParaInfo paraInfo;
    sptr<CallBase> callObjectPtr = std::make_unique<CSCall>(paraInfo).release();
    TelCallState priorState = static_cast<TelCallState>(size % CALL_STATE_NUM);
    TelCallState nextState = static_cast<TelCallState>(size % CALL_STATE_NUM);
    std::string message(reinterpret_cast<const char *>(data), size);
    DisconnectedDetails details;

    notification->NewCallCreated(callObjectPtr);
    notification->WakeupDevice();
    notification->IsPowerAbilityExist();
    notification->CallDestroyed(details);
    notification->IncomingCallActivated(callObjectPtr);
    notification->IncomingCallHungUp(callObjectPtr, true, message);
    notification->CallStateUpdated(callObjectPtr, priorState, nextState);
}

void ProximitySensorFunc(const uint8_t *data, size_t size)
{
    if (!IsServiceInited()) {
        return;
    }

    std::shared_ptr<ProximitySensor> notification = std::make_shared<ProximitySensor>();
    DialParaInfo paraInfo;
    sptr<CallBase> callObjectPtr = std::make_unique<CSCall>(paraInfo).release();
    TelCallState priorState = static_cast<TelCallState>(size % CALL_STATE_NUM);
    TelCallState nextState = static_cast<TelCallState>(size % CALL_STATE_NUM);
    std::string message(reinterpret_cast<const char *>(data), size);
    DisconnectedDetails details;

    notification->CallDestroyed(details);
    notification->NewCallCreated(callObjectPtr);
    notification->IncomingCallActivated(callObjectPtr);
    notification->IncomingCallHungUp(callObjectPtr, true, message);
    notification->CallStateUpdated(callObjectPtr, priorState, nextState);
}

void StatusBarFunc(const uint8_t *data, size_t size)
{
    if (!IsServiceInited()) {
        return;
    }

    std::shared_ptr<StatusBar> notification = std::make_shared<StatusBar>();
    DialParaInfo paraInfo;
    sptr<CallBase> callObjectPtr = std::make_unique<CSCall>(paraInfo).release();
    TelCallState priorState = static_cast<TelCallState>(size % CALL_STATE_NUM);
    TelCallState nextState = static_cast<TelCallState>(size % CALL_STATE_NUM);
    std::string message(reinterpret_cast<const char *>(data), size);
    DisconnectedDetails details;
    int32_t isDisplayMute = static_cast<int32_t>(size % BOOL_NUM);

    notification->UpdateMuteIcon(isDisplayMute);
    notification->UpdateSpeakerphoneIcon(isDisplayMute);
    notification->NewCallCreated(callObjectPtr);
    notification->CallDestroyed(details);
    notification->IncomingCallActivated(callObjectPtr);
    notification->IncomingCallHungUp(callObjectPtr, true, message);
    notification->CallStateUpdated(callObjectPtr, priorState, nextState);
}

void WiredHeadsetHandlerFunc(const uint8_t *data, size_t size)
{
    if (!IsServiceInited()) {
        return;
    }

    std::shared_ptr<WiredHeadset> notification = std::make_shared<WiredHeadset>();
    DialParaInfo paraInfo;
    sptr<CallBase> callObjectPtr = std::make_unique<CSCall>(paraInfo).release();
    TelCallState priorState = static_cast<TelCallState>(size % CALL_STATE_NUM);
    TelCallState nextState = static_cast<TelCallState>(size % CALL_STATE_NUM);
    std::string message(reinterpret_cast<const char *>(data), size);
    DisconnectedDetails details;

    notification->Init();
    notification->NewCallCreated(callObjectPtr);
    notification->CallDestroyed(details);
    notification->IncomingCallActivated(callObjectPtr);
    notification->IncomingCallHungUp(callObjectPtr, true, message);
    notification->CallStateUpdated(callObjectPtr, priorState, nextState);
}

void CallDataRdbObserverFunc(const uint8_t *data, size_t size)
{
    if (!IsServiceInited()) {
        return;
    }

    std::string message(reinterpret_cast<const char *>(data), size);
    std::shared_ptr<CallDataBaseHelper> callDataBaseHelper = DelayedSingleton<CallDataBaseHelper>::GetInstance();
    DataShare::DataShareValuesBucket values;
    DataShare::DataSharePredicates predicates;
    DialParaInfo paraInfo;
    sptr<CallBase> callObjectPtr = std::make_unique<CSCall>(paraInfo).release();
    ContactInfo contactInfo;
    int32_t length = message.length() > CONTACT_NAME_LEN ? CONTACT_NAME_LEN : message.length();
    if (memcpy_s(contactInfo.name, CONTACT_NAME_LEN, message.c_str(), length) != EOK) {
        return;
    }
    length = message.length() > kMaxNumberLen ? kMaxNumberLen : message.length();
    if (memcpy_s(contactInfo.number, kMaxNumberLen, message.c_str(), length) != EOK) {
        return;
    }
    length = message.length() > FILE_PATH_MAX_LEN ? FILE_PATH_MAX_LEN : message.length();
    if (memcpy_s(contactInfo.ringtonePath, FILE_PATH_MAX_LEN, message.c_str(), length) != EOK) {
        return;
    }

    callDataBaseHelper->UnRegisterObserver();
    callDataBaseHelper->Insert(values);
    callDataBaseHelper->Query(contactInfo, predicates);
    callDataBaseHelper->Delete(predicates);
}

void MissedCallNotificationFunc(const uint8_t *data, size_t size)
{
    if (!IsServiceInited()) {
        return;
    }

    std::shared_ptr<MissedCallNotification> notification = std::make_shared<MissedCallNotification>();
    DialParaInfo paraInfo;
    sptr<CallBase> callObjectPtr = std::make_unique<CSCall>(paraInfo).release();
    TelCallState priorState = static_cast<TelCallState>(size % CALL_STATE_NUM);
    TelCallState nextState = static_cast<TelCallState>(size % CALL_STATE_NUM);
    std::string message(reinterpret_cast<const char *>(data), size);
    DisconnectedDetails details;

    notification->NewCallCreated(callObjectPtr);
    notification->PublishMissedCallEvent(callObjectPtr);
    notification->PublishMissedCallNotification(callObjectPtr);
    notification->CancelMissedCallsNotification(static_cast<int32_t>(size));
    notification->CallDestroyed(details);
    notification->IncomingCallActivated(callObjectPtr);
    notification->IncomingCallHungUp(callObjectPtr, true, message);
    notification->CallStateUpdated(callObjectPtr, priorState, nextState);
}

void RejectCallSmsFunc(const uint8_t *data, size_t size)
{
    if (!IsServiceInited()) {
        return;
    }

    std::shared_ptr<RejectCallSms> notification = std::make_shared<RejectCallSms>();
    DialParaInfo paraInfo;
    int32_t slotId = static_cast<int32_t>(size % 2);
    sptr<CallBase> callObjectPtr = std::make_unique<CSCall>(paraInfo).release();
    TelCallState priorState = static_cast<TelCallState>(size % CALL_STATE_NUM);
    TelCallState nextState = static_cast<TelCallState>(size % CALL_STATE_NUM);
    std::string message(reinterpret_cast<const char *>(data), size);
    std::string desAddr(reinterpret_cast<const char *>(data), size);
    std::u16string desAddrU16 = Str8ToStr16(desAddr);
    std::u16string messageU16 = Str8ToStr16(message);
    DisconnectedDetails details;

    notification->NewCallCreated(callObjectPtr);
    notification->SendMessage(slotId, desAddrU16, messageU16);
    notification->CallDestroyed(details);
    notification->IncomingCallActivated(callObjectPtr);
    notification->IncomingCallHungUp(callObjectPtr, true, message);
    notification->CallStateUpdated(callObjectPtr, priorState, nextState);
}

void DoSomethingInterestingWithMyAPI(const uint8_t *data, size_t size)
{
    if (data == nullptr || size == 0) {
        return;
    }

    IncomingCallNotificationFunc(data, size);
    IncomingCallWakeupFunc(data, size);
    ProximitySensorFunc(data, size);
    StatusBarFunc(data, size);
    WiredHeadsetHandlerFunc(data, size);
    CallDataRdbObserverFunc(data, size);
    MissedCallNotificationFunc(data, size);
    RejectCallSmsFunc(data, size);
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
