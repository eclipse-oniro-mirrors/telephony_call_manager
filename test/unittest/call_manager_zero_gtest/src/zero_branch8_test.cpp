/*
 * Copyright (C) 2023 Huawei Device Co., Ltd.
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
#define private public
#define protected public
#include "bluetooth_call.h"
#include "bluetooth_call_client.h"
#include "bluetooth_call_connection.h"
#include "bluetooth_call_manager.h"
#include "bluetooth_call_service.h"
#include "bluetooth_call_state.h"
#include "bluetooth_connection.h"
#include "call_ability_connect_callback.h"
#include "call_broadcast_subscriber.h"
#include "call_connect_ability.h"
#include "call_incoming_filter_manager.h"
#include "call_object_manager.h"
#include "call_request_event_handler_helper.h"
#include "call_status_policy.h"
#include "call_superprivacy_control_manager.h"
#include "call_wired_headset.h"
#include "common_event_manager.h"
#include "common_event_support.h"
#include "fold_status_manager.h"
#include "gtest/gtest.h"
#include "ims_call.h"
#include "ims_conference.h"
#include "ott_call.h"
#include "voip_call.h"
#include "audio_control_manager.h"
#include "call_state_processor.h"

namespace OHOS {
namespace Telephony {
using namespace testing::ext;
constexpr int WAIT_TIME = 3;

class ZeroBranch7Test : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};
void ZeroBranch7Test::SetUpTestCase() {}

void ZeroBranch7Test::TearDownTestCase() {}

void ZeroBranch7Test::SetUp() {}

void ZeroBranch7Test::TearDown()
{
    sleep(1);
}

/**
 * @tc.number   Telephony_CallBroadcastSubscriber_001
 * @tc.name     test error branch
 * @tc.desc     Function test
 */
HWTEST_F(ZeroBranch7Test, Telephony_CallBroadcastSubscriber_001, Function | MediumTest | Level3)
{
    EventFwk::MatchingSkills matchingSkills;
    matchingSkills.AddEvent(EventFwk::CommonEventSupport::COMMON_EVENT_SIM_STATE_CHANGED);
    EventFwk::CommonEventSubscribeInfo subscriberInfo(matchingSkills);
    CallBroadcastSubscriber subscriber(subscriberInfo);
    EventFwk::CommonEventData eventData;
    ASSERT_NO_THROW(subscriber.OnReceiveEvent(eventData));
    ASSERT_NO_THROW(subscriber.UnknownBroadcast(eventData));
    ASSERT_NO_THROW(subscriber.SimStateBroadcast(eventData));
    ASSERT_NO_THROW(subscriber.ConnectCallUiServiceBroadcast(eventData));
    ASSERT_NO_THROW(subscriber.HighTempLevelChangedBroadcast(eventData));
    ASSERT_NO_THROW(subscriber.ConnectCallUiSuperPrivacyModeBroadcast(eventData));
    ASSERT_NO_THROW(subscriber.UpdateBluetoothDeviceName(eventData));
    ASSERT_NO_THROW(subscriber.ConnectCallUiUserSwitchedBroadcast(eventData));
    ASSERT_NO_THROW(subscriber.ShutdownBroadcast(eventData));
    ASSERT_NO_THROW(subscriber.HsdrEventBroadcast(eventData));
    ASSERT_NO_THROW(subscriber.ScreenUnlockedBroadcast(eventData));
}

/**
 * @tc.number   Telephony_CallStatusCallbackStub_008
 * @tc.name     test error branch
 * @tc.desc     Function test
 */
HWTEST_F(ZeroBranch7Test, Telephony_CallStatusCallbackStub_001, Function | MediumTest | Level3)
{
    auto callStatusCallback = std::make_shared<CallStatusCallback>();
    MessageParcel messageParcel;
    messageParcel.WriteInterfaceToken(CallStatusCallbackStub::GetDescriptor());
    messageParcel.WriteInt32(0);
    messageParcel.WriteString("hello");
    MessageParcel reply;
    ASSERT_EQ(callStatusCallback->OnUpdateDisconnectedCause(messageParcel, reply), TELEPHONY_SUCCESS);
    MessageParcel dataParcel2;
    dataParcel2.WriteInterfaceToken(CallStatusCallbackStub::GetDescriptor());
    dataParcel2.WriteInt32(0);
    callStatusCallback->OnUpdateRBTPlayInfo(dataParcel2, reply);
    callStatusCallback->OnUpdateSetWaitingResult(dataParcel2, reply);
    callStatusCallback->OnUpdateSetRestrictionResult(dataParcel2, reply);
    callStatusCallback->OnUpdateSetRestrictionPasswordResult(dataParcel2, reply);
    callStatusCallback->OnUpdateSetTransferResult(dataParcel2, reply);
    callStatusCallback->OnUpdateSetCallClirResult(dataParcel2, reply);
    callStatusCallback->OnStartRttResult(dataParcel2, reply);
    callStatusCallback->OnStopRttResult(dataParcel2, reply);
    callStatusCallback->OnSetImsConfigResult(dataParcel2, reply);
    callStatusCallback->OnSetImsFeatureValueResult(dataParcel2, reply);
    callStatusCallback->OnInviteToConferenceResult(dataParcel2, reply);
    callStatusCallback->OnStartDtmfResult(dataParcel2, reply);
    callStatusCallback->OnStopDtmfResult(dataParcel2, reply);
    callStatusCallback->OnSendUssdResult(dataParcel2, reply);
    callStatusCallback->OnGetImsCallDataResult(dataParcel2, reply);
    ASSERT_EQ(callStatusCallback->OnCloseUnFinishedUssdResult(dataParcel2, reply), TELEPHONY_SUCCESS);
}

/**
 * @tc.number   Telephony_CallStatusCallbackStub_009
 * @tc.name     test error branch
 * @tc.desc     Function test
 */
HWTEST_F(ZeroBranch7Test, Telephony_CallStatusCallbackStub_002, Function | MediumTest | Level3)
{
    auto callStatusCallback = std::make_shared<CallStatusCallback>();
    MessageParcel dataParce3;
    dataParce3.WriteInterfaceToken(CallStatusCallbackStub::GetDescriptor());
    int32_t length = sizeof(ClirResponse);
    dataParce3.WriteInt32(length);
    ClirResponse clirResponse;
    dataParce3.WriteRawData((const void *)&clirResponse, length);
    dataParce3.RewindRead(0);
    MessageParcel reply;
    callStatusCallback->OnUpdateGetCallClirResult(dataParce3, reply);

    MessageParcel dataParce4;
    dataParce4.WriteInterfaceToken(CallStatusCallbackStub::GetDescriptor());
    length = sizeof(CallModeReportInfo);
    dataParce4.WriteInt32(length);
    CallModeReportInfo callModeReportInfo;
    dataParce4.WriteRawData((const void *)&callModeReportInfo, length);
    dataParce4.RewindRead(0);
    callStatusCallback->OnReceiveImsCallModeResponse(dataParce4, reply);
    MessageParcel dataParce5;
    dataParce5.WriteInterfaceToken(CallStatusCallbackStub::GetDescriptor());
    dataParce5.WriteString("a");
    callStatusCallback->OnPostDialNextChar(dataParce5, reply);
    callStatusCallback->OnReportPostDialDelay(dataParce5, reply);

    MessageParcel dataParce6;
    dataParce6.WriteInterfaceToken(CallStatusCallbackStub::GetDescriptor());
    dataParce6.WriteString("123");
    dataParce6.WriteString("abc");
    dataParce6.WriteInt32(0);
    dataParce6.WriteInt32(0);
    ASSERT_EQ(callStatusCallback->OnUpdateVoipEventInfo(dataParce6, reply), TELEPHONY_SUCCESS);
}

HWTEST_F(ZeroBranch7Test, Telephony_FoldStatusManager_001, Function | MediumTest | Level3)
{
    auto foldStatusManagerPtr = DelayedSingleton<FoldStatusManager>::GetInstance();
    if (foldStatusManagerPtr == nullptr) {
        return;
    }
    foldStatusManagerPtr->RegisterFoldableListener();
    foldStatusManagerPtr->UnregisterFoldableListener();
    FoldStatusManager::IsSmallFoldDevice();
    FoldStatusManager::FoldStatusListener listenerPtr;
    listenerPtr.OnFoldStatusChanged(Rosen::FoldStatus::UNKNOWN);
    listenerPtr.OnFoldStatusChanged(Rosen::FoldStatus::EXPAND);
    EXPECT_TRUE(foldStatusManagerPtr != nullptr);
}

HWTEST_F(ZeroBranch7Test, Telephony_BluetoothCallConnection_001, Function | MediumTest | Level3)
{
    auto blueToothConnectionPtr = DelayedSingleton<BluetoothCallConnection>::GetInstance();
    if (blueToothConnectionPtr == nullptr) {
        return;
    }
    DialParaInfo info;
    blueToothConnectionPtr->Dial(info);
    blueToothConnectionPtr->SetMacAddress("");
    blueToothConnectionPtr->SetMacAddress("abc");
    blueToothConnectionPtr->ConnectBtSco();
    blueToothConnectionPtr->DisConnectBtSco();
    blueToothConnectionPtr->GetBtScoIsConnected();
    blueToothConnectionPtr->SetHfpConnected(false);
    blueToothConnectionPtr->SetHfpConnected(true);
    blueToothConnectionPtr->GetHfpContactName("");
    blueToothConnectionPtr->GetHfpContactName("a");
    blueToothConnectionPtr->SetHfpContactName("a", "b");
    blueToothConnectionPtr->GetHfpContactName("a");
    EXPECT_TRUE(blueToothConnectionPtr != nullptr);
}

/**
 * @tc.number   Telephony_CallWiredHeadSet_001
 * @tc.name     test key mute press
 * @tc.desc     Function test
 */
HWTEST_F(ZeroBranch7Test, Telephony_CallWiredHeadSet_001, Function | MediumTest | Level1)
{
    auto instance = DelayedSingleton<CallWiredHeadSet>::GetInstance();
    instance->UnregistKeyMutePressedUp();
    instance->UnregistKeyMutePressedDown();
    DelayedSingleton<AudioDeviceManager>::GetInstance()->isWiredHeadsetConnected_ = false;
    EXPECT_TRUE(instance->RegistKeyMutePressedUp());
    EXPECT_TRUE(instance->RegistKeyMutePressedDown());
    instance->downFirstTime_ = 0;
    instance->isProcessed_ = true;
    instance->DealKeyMutePressedDown(nullptr);
    instance->DealKeyMutePressedUp(nullptr);
    EXPECT_FALSE(instance->isProcessed_);
    instance->DealKeyMutePressedDown(nullptr);
    DelayedSingleton<AudioDeviceManager>::GetInstance()->isWiredHeadsetConnected_ = true;
    instance->DealKeyMutePressedDown(nullptr);
    EXPECT_NE(instance->downFirstTime_, 0);
    instance->DealKeyMutePressedUp(nullptr);
    instance->DealKeyMutePressedDown(nullptr);
    instance->DealKeyMutePressedDown(nullptr);
    sleep(WAIT_TIME);
    instance->DealKeyMutePressedDown(nullptr);
    EXPECT_TRUE(instance->isProcessed_);
    instance->DealKeyMutePressedUp(nullptr);
    instance->UnregistKeyMutePressedUp();
    instance->UnregistKeyMutePressedDown();
}

/**
 * @tc.number   Telephony_CallWiredHeadSet_002
 * @tc.name     test key mute press
 * @tc.desc     Function test
 */
HWTEST_F(ZeroBranch7Test, Telephony_CallWiredHeadSet_002, Function | MediumTest | Level1)
{
    auto instance = DelayedSingleton<CallWiredHeadSet>::GetInstance();
    DialParaInfo info;
    sptr<CallBase> ringingCall = new IMSCall(info);
    ringingCall->SetCallRunningState(CallRunningState::CALL_RUNNING_STATE_RINGING);
    ringingCall->SetCallId(0);
    sptr<CallBase> holdCall = new IMSCall(info);
    holdCall->SetCallId(1);
    holdCall->SetCallRunningState(CallRunningState::CALL_RUNNING_STATE_HOLD);
    sptr<CallBase> activeCall = new IMSCall(info);
    activeCall->SetCallId(2);
    activeCall->SetCallRunningState(CallRunningState::CALL_RUNNING_STATE_ACTIVE);
    sptr<CallBase> alertingCall = new IMSCall(info);
    alertingCall->SetCallId(3);
    alertingCall->SetTelCallState(TelCallState::CALL_STATUS_ALERTING);
    CallObjectManager::AddOneCallObject(ringingCall);
    ringingCall->SetVideoStateType(VideoStateType::TYPE_VOICE);
    instance->DealKeyMuteShortPressed();
    ringingCall->SetVideoStateType(VideoStateType::TYPE_VIDEO);
    instance->DealKeyMuteShortPressed();
    ringingCall->SetVideoStateType(VideoStateType::TYPE_SEND_ONLY);
    instance->DealKeyMuteShortPressed();
    instance->DealKeyMuteLongPressed();
    CallObjectManager::DeleteOneCallObject(ringingCall);
    EXPECT_NE(ringingCall, nullptr);
    CallObjectManager::AddOneCallObject(alertingCall);
    alertingCall->SetTelConferenceState(TelConferenceState::TEL_CONFERENCE_IDLE);
    instance->DealKeyMuteShortPressed();
    instance->DealKeyMuteLongPressed();
    alertingCall->SetTelConferenceState(TelConferenceState::TEL_CONFERENCE_ACTIVE);
    instance->DealKeyMuteLongPressed();
    CallObjectManager::DeleteOneCallObject(alertingCall);
    EXPECT_NE(alertingCall, nullptr);
    CallObjectManager::AddOneCallObject(activeCall);
    instance->DealKeyMuteShortPressed();
    CallObjectManager::DeleteOneCallObject(activeCall);
    CallObjectManager::AddOneCallObject(holdCall);
    instance->DealKeyMuteShortPressed();
    CallObjectManager::AddOneCallObject(activeCall);
    EXPECT_NE(CallObjectManager::GetOneCallObject(CallRunningState::CALL_RUNNING_STATE_HOLD), nullptr);
    EXPECT_NE(CallObjectManager::GetOneCallObject(CallRunningState::CALL_RUNNING_STATE_ACTIVE), nullptr);
    instance->DealKeyMuteShortPressed();
    CallObjectManager::DeleteOneCallObject(holdCall);
    EXPECT_NE(holdCall, nullptr);
    CallObjectManager::DeleteOneCallObject(activeCall);
    EXPECT_NE(activeCall, nullptr);
    EXPECT_EQ(ringingCall->answerType_, CallAnswerType::CALL_ANSWER_REJECT);
}

static bool g_receiveUnknownEvent = false;

static void UnknownBroadcastStub(const EventFwk::CommonEventData &data)
{
    g_receiveUnknownEvent = true;
}

/**
 * @tc.number   Telephony_CallBroadCastSubscriber_002
 * @tc.name     test OnReceiveEvent
 * @tc.desc     Function test
 */
HWTEST_F(ZeroBranch7Test, Telephony_CallBroadCastSubscriber_002, Function | MediumTest | Level1)
{
    g_receiveUnknownEvent = false;
    EventFwk::MatchingSkills matchingSkills;
    EventFwk::CommonEventSubscribeInfo subscriberInfo(matchingSkills);
    std::shared_ptr<CallBroadcastSubscriber> subscriberPtr = std::make_shared<CallBroadcastSubscriber>(subscriberInfo);
    subscriberPtr->memberFuncMap_[CallBroadcastSubscriber::UNKNOWN_BROADCAST_EVENT] =
        [](const EventFwk::CommonEventData &data) { UnknownBroadcastStub(data); };
    EventFwk::CommonEventData data;
    OHOS::EventFwk::Want want;
    want.SetAction(EventFwk::CommonEventSupport::COMMON_EVENT_SIM_STATE_CHANGED);
    data.SetWant(want);
    subscriberPtr->OnReceiveEvent(data);
    want.SetAction("event.custom.contacts.PAGE_STATE_CHANGE");
    data.SetWant(want);
    subscriberPtr->OnReceiveEvent(data);
    want.SetAction("usual.event.thermal.satcomm.HIGH_TEMP_LEVEL");
    data.SetWant(want);
    subscriberPtr->OnReceiveEvent(data);
    want.SetAction("usual.event.SUPER_PRIVACY_MODE");
    data.SetWant(want);
    subscriberPtr->OnReceiveEvent(data);
    want.SetAction("usual.event.HSDR_EVENT");
    data.SetWant(want);
    subscriberPtr->OnReceiveEvent(data);
    want.SetAction(EventFwk::CommonEventSupport::COMMON_EVENT_BLUETOOTH_REMOTEDEVICE_NAME_UPDATE);
    data.SetWant(want);
    subscriberPtr->OnReceiveEvent(data);
    want.SetAction(EventFwk::CommonEventSupport::COMMON_EVENT_USER_SWITCHED);
    data.SetWant(want);
    subscriberPtr->OnReceiveEvent(data);
    want.SetAction(EventFwk::CommonEventSupport::COMMON_EVENT_SHUTDOWN);
    data.SetWant(want);
    subscriberPtr->OnReceiveEvent(data);
    want.SetAction(EventFwk::CommonEventSupport::COMMON_EVENT_SCREEN_UNLOCKED);
    data.SetWant(want);
    subscriberPtr->OnReceiveEvent(data);
    want.SetAction("usual.event.bluetooth.CONNECT_HFP_HF");
    data.SetWant(want);
    subscriberPtr->OnReceiveEvent(data);
    want.SetAction("unknown.event");
    data.SetWant(want);
    subscriberPtr->OnReceiveEvent(data);
    subscriberPtr->memberFuncMap_.clear();
    subscriberPtr->OnReceiveEvent(data);
    EXPECT_TRUE(g_receiveUnknownEvent);
}

/**
 * @tc.number   Telephony_CallBroadcastSubscriber_003
 * @tc.name     test ConnectCallUiSuperPrivacyModeBroadcast
 * @tc.desc     Function test
 */
HWTEST_F(ZeroBranch7Test, Telephony_CallBroadcastSubscriber_003, Function | MediumTest | Level1)
{
    EventFwk::MatchingSkills matchingSkills;
    EventFwk::CommonEventSubscribeInfo subscriberInfo(matchingSkills);
    std::shared_ptr<CallBroadcastSubscriber> subscriberPtr = std::make_shared<CallBroadcastSubscriber>(subscriberInfo);
    EventFwk::CommonEventData data;
    OHOS::EventFwk::Want want;
    want.SetAction("usual.event.SUPER_PRIVACY_MODE");
    want.SetParam("isInCall", false);
    want.SetParam("isHangup", true);
    data.SetWant(want);
    DelayedSingleton<CallSuperPrivacyControlManager>::GetInstance()->SetIsChangeSuperPrivacyMode(false);
    subscriberPtr->OnReceiveEvent(data);
    EXPECT_TRUE(DelayedSingleton<CallSuperPrivacyControlManager>::GetInstance()->GetIsChangeSuperPrivacyMode());
    want.SetParam("isInCall", true);
    want.SetParam("isHangup", false);
    want.SetParam("isAnswer", true);
    data.SetWant(want);
    DelayedSingleton<CallSuperPrivacyControlManager>::GetInstance()->SetIsChangeSuperPrivacyMode(false);
    subscriberPtr->OnReceiveEvent(data);
    want.SetParam("isInCall", true);
    want.SetParam("isHangup", true);
    data.SetWant(want);
    subscriberPtr->OnReceiveEvent(data);
    EXPECT_TRUE(DelayedSingleton<CallSuperPrivacyControlManager>::GetInstance()->GetIsChangeSuperPrivacyMode());
}

/**
 * @tc.number   Telephony_CallBroadcastSubscriber_004
 * @tc.name     test HfpConnectBroadcast
 * @tc.desc     Function test
 */
HWTEST_F(ZeroBranch7Test, Telephony_CallBroadcastSubscriber_004, Function | MediumTest | Level1)
{
    EventFwk::MatchingSkills matchingSkills;
    EventFwk::CommonEventSubscribeInfo subscriberInfo(matchingSkills);
    std::shared_ptr<CallBroadcastSubscriber> subscriberPtr = std::make_shared<CallBroadcastSubscriber>(subscriberInfo);
    EventFwk::CommonEventData data;
    OHOS::EventFwk::Want want;
    want.SetAction("usual.event.bluetooth.CONNECT_HFP_HF");
    std::string contact = "contact";
    std::string phoneNumber = "123456";
    want.SetParam("contact", contact);
    want.SetParam("phoneNumber", phoneNumber);
    data.SetWant(want);
    subscriberPtr->OnReceiveEvent(data);
    DialParaInfo info;
    sptr<CallBase> alertingCall = new IMSCall(info);
    alertingCall->SetTelCallState(TelCallState::CALL_STATUS_ALERTING);
    CallObjectManager::AddOneCallObject(alertingCall);
    subscriberPtr->OnReceiveEvent(data);
    alertingCall->SetAccountNumber(phoneNumber);
    subscriberPtr->OnReceiveEvent(data);
    EXPECT_EQ(DelayedSingleton<BluetoothCallConnection>::GetInstance()->hfpPhoneNumber_, phoneNumber);
    EXPECT_EQ(DelayedSingleton<BluetoothCallConnection>::GetInstance()->hfpContactName_, contact);
    CallObjectManager::DeleteOneCallObject(alertingCall);
}

/**
 * @tc.number   Telephony_BluetoothCallState_001
 * @tc.name     test OnConnectionStateChanged & OnScoStateChanged
 * @tc.desc     Function test
 */
HWTEST_F(ZeroBranch7Test, Telephony_BluetoothCallState_001, Function | MediumTest | Level1)
{
    auto bluetoothCallState = std::make_shared<BluetoothCallState>();
    Bluetooth::BluetoothRemoteDevice device("macAddress");
    bluetoothCallState->OnConnectionStateChanged(device,
        static_cast<int32_t>(Bluetooth::BTConnectState::CONNECTED), 0);
    EXPECT_TRUE(DelayedSingleton<BluetoothCallConnection>::GetInstance()->isHfpConnected_);
    DelayedSingleton<BluetoothCallConnection>::GetInstance()->hfpPhoneNumber_ = "123";
    DelayedSingleton<BluetoothCallConnection>::GetInstance()->hfpContactName_ = "456";
    bluetoothCallState->OnConnectionStateChanged(device,
        static_cast<int32_t>(Bluetooth::BTConnectState::DISCONNECTED), 0);
    EXPECT_EQ(DelayedSingleton<BluetoothCallConnection>::GetInstance()->hfpPhoneNumber_, "");
    EXPECT_EQ(DelayedSingleton<BluetoothCallConnection>::GetInstance()->hfpContactName_, "");
    bluetoothCallState->OnConnectionStateChanged(device, 1111111, 0);
    EXPECT_FALSE(DelayedSingleton<BluetoothCallConnection>::GetInstance()->isHfpConnected_);
    bluetoothCallState->OnScoStateChanged(device, static_cast<int32_t>(Bluetooth::HfpScoConnectState::SCO_CONNECTED));
    EXPECT_TRUE(DelayedSingleton<BluetoothCallConnection>::GetInstance()->isBtCallScoConnected_);
    bluetoothCallState->OnScoStateChanged(device,
        static_cast<int32_t>(Bluetooth::HfpScoConnectState::SCO_DISCONNECTED));
    EXPECT_FALSE(DelayedSingleton<BluetoothCallConnection>::GetInstance()->isBtCallScoConnected_);
}

/**
 * @tc.number   Telephony_BluetoothCall_001
 * @tc.name     test bluetoothCall
 * @tc.desc     Function test
 */
HWTEST_F(ZeroBranch7Test, Telephony_BluetoothCall_001, Function | MediumTest | Level1)
{
    DialParaInfo info;
    EXPECT_TRUE(std::make_shared<BluetoothCall>(info, "")->macAddress_.empty());
    AppExecFwk::PacMap extras;
    EXPECT_TRUE(std::make_shared<BluetoothCall>(info, extras, "")->macAddress_.empty());
    EXPECT_FALSE(std::make_shared<BluetoothCall>(info, "macAddress")->macAddress_.empty());
    EXPECT_FALSE(std::make_shared<BluetoothCall>(info, extras, "macAddress")->macAddress_.empty());
    EXPECT_GT(std::make_shared<BluetoothCall>(info, "")->AnswerCall(0), TELEPHONY_ERROR);
    EXPECT_GT(std::make_shared<BluetoothCall>(info, "")->RejectCall(), TELEPHONY_ERROR);
    EXPECT_GT(std::make_shared<BluetoothCall>(info, "")->HangUpCall(), TELEPHONY_ERROR);
    EXPECT_GT(std::make_shared<BluetoothCall>(info, "")->StartDtmf('0'), TELEPHONY_ERROR);
    EXPECT_GT(std::make_shared<BluetoothCall>(info, "macAddress")->AnswerCall(0), TELEPHONY_ERROR);
    EXPECT_GT(std::make_shared<BluetoothCall>(info, "macAddress")->RejectCall(), TELEPHONY_ERROR);
    EXPECT_GT(std::make_shared<BluetoothCall>(info, "macAddress")->HangUpCall(), TELEPHONY_ERROR);
    EXPECT_GT(std::make_shared<BluetoothCall>(info, "macAddress")->StartDtmf('0'), TELEPHONY_ERROR);
}

class MockRemoteObject : public IRemoteObject {
public:
    explicit MockRemoteObject() : IRemoteObject(u"default") {}

    int SendRequest(uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option)
    {
        return 0;
    }

    int32_t GetObjectRefCount()
    {
        return 0;
    }

    bool AddDeathRecipient(const sptr<DeathRecipient> &recipient)
    {
        return true;
    }

    bool RemoveDeathRecipient(const sptr<DeathRecipient> &recipient)
    {
        return true;
    }

    bool IsObjectDead() const
    {
        return false;
    }

    int Dump(int fd, const std::vector<std::u16string> &args)
    {
        return 0;
    }
};

/**
 * @tc.number   Telephony_CallAbilityConnectCallback_001
 * @tc.name     test CallAbilityConnectCallback
 * @tc.desc     Function test
 */
HWTEST_F(ZeroBranch7Test, Telephony_CallAbilityConnectCallback_001, Function | MediumTest | Level1)
{
    sptr<IRemoteObject> remoteObject = new MockRemoteObject();
    auto callback = std::make_shared<CallAbilityConnectCallback>();
    AppExecFwk::ElementName element;
    callback->OnAbilityConnectDone(element, remoteObject, -1);
    DelayedSingleton<CallConnectAbility>::GetInstance()->SetConnectFlag(true);
    CallObjectManager::callObjectPtrList_.clear();
    DialParaInfo info;
    sptr<CallBase> call0 = new IMSCall(info);
    call0->SetCallId(0);
    call0->SetTelCallState(TelCallState::CALL_STATUS_DISCONNECTING);
    sptr<CallBase> call1 = new IMSCall(info);
    call1->SetCallId(1);
    call1->SetTelCallState(TelCallState::CALL_STATUS_DISCONNECTED);
    sptr<CallBase> call2 = new IMSCall(info);
    call2->SetCallId(2);
    call2->SetTelCallState(TelCallState::CALL_STATUS_UNKNOWN);
    sptr<CallBase> call3 = new IMSCall(info);
    call3->SetCallId(3);
    call3->SetTelCallState(TelCallState::CALL_STATUS_IDLE);
    CallObjectManager::AddOneCallObject(call0);
    CallObjectManager::AddOneCallObject(call1);
    CallObjectManager::AddOneCallObject(call2);
    CallObjectManager::AddOneCallObject(call3);
    callback->OnAbilityDisconnectDone(element, -1);
    EXPECT_FALSE(DelayedSingleton<CallConnectAbility>::GetInstance()->GetConnectFlag());
    CallObjectManager::callObjectPtrList_.clear();
}

/**
 * @tc.number   Telephony_CallStatusPolicy_001
 * @tc.name     test CallStatusPolicy
 * @tc.desc     Function test
 */
HWTEST_F(ZeroBranch7Test, Telephony_CallStatusPolicy_001, Function | MediumTest | Level1)
{
    CellularCallInfo callInfo;
    CallDetailInfo detailInfo;
    EXPECT_EQ(std::make_shared<CallIncomingFilterManager>()->PackCellularCallInfo(callInfo, detailInfo),
        TELEPHONY_SUCCESS);
    DialParaInfo info;
    sptr<CallBase> call = new IMSCall(info);
    call->SetCallId(0);
    call->SetAccountNumber("111");
    strcpy_s(detailInfo.phoneNum, kMaxNumberLen, "111");
    CallObjectManager::AddOneCallObject(call);
    EXPECT_EQ(std::make_shared<CallStatusPolicy>()->DialingHandlePolicy(detailInfo), CALL_ERR_CALL_ALREADY_EXISTS);
    CallObjectManager::DeleteOneCallObject(call);
    DelayedSingleton<CallControlManager>::GetInstance()->VoIPCallState_ = CallStateToApp::CALL_STATE_IDLE;
    EXPECT_EQ(std::make_shared<CallStatusPolicy>()->FilterResultsDispose(call), TELEPHONY_SUCCESS);
    DelayedSingleton<CallControlManager>::GetInstance()->VoIPCallState_ = CallStateToApp::CALL_STATE_RINGING;
    EXPECT_GT(std::make_shared<CallStatusPolicy>()->FilterResultsDispose(call), TELEPHONY_ERROR);
    for (int32_t i = 1; i < 31; i++) {
        sptr<CallBase> tempCall = new IMSCall(info);
        tempCall->SetCallId(i);
        tempCall->SetCallRunningState(CallRunningState::CALL_RUNNING_STATE_RINGING);
        CallObjectManager::AddOneCallObject(tempCall);
    }
    EXPECT_GT(std::make_shared<CallStatusPolicy>()->FilterResultsDispose(call), TELEPHONY_ERROR);
    CallObjectManager::callObjectPtrList_.clear();
    for (int32_t i = 1; i < 31; i++) {
        sptr<CallBase> tempCall = new IMSCall(info);
        tempCall->SetCallId(i);
        tempCall->SetCallRunningState(CallRunningState::CALL_RUNNING_STATE_ACTIVE);
        CallObjectManager::AddOneCallObject(tempCall);
    }
    EXPECT_GT(std::make_shared<CallStatusPolicy>()->FilterResultsDispose(call), TELEPHONY_ERROR);
    CallObjectManager::callObjectPtrList_.clear();
}

/**
 * @tc.number   Telephony_IMSCall_001
 * @tc.name     test IMSCall
 * @tc.desc     Function test
 */
HWTEST_F(ZeroBranch7Test, Telephony_IMSCall_001, Function | MediumTest | Level1)
{
    DialParaInfo info;
    std::shared_ptr<IMSCall> call = std::make_shared<IMSCall>(info);
    for (int16_t i = 0; i <= kMaxNumberLen + 1; i++) {
        call->accountNumber_ += "1";
    }
    std::u16string msg = u"";
    EXPECT_EQ(call->StartRtt(msg), CALL_ERR_NUMBER_OUT_OF_RANGE);
    EXPECT_EQ(call->StopRtt(), CALL_ERR_NUMBER_OUT_OF_RANGE);
    EXPECT_EQ(call->SendUpdateCallMediaModeRequest(ImsCallMode::CALL_MODE_AUDIO_ONLY), CALL_ERR_NUMBER_OUT_OF_RANGE);
    EXPECT_EQ(call->SendUpdateCallMediaModeResponse(ImsCallMode::CALL_MODE_AUDIO_ONLY), CALL_ERR_NUMBER_OUT_OF_RANGE);
    call->accountNumber_ = "111";
    EXPECT_GT(call->StartRtt(msg), TELEPHONY_ERROR);
    EXPECT_GT(call->StopRtt(), TELEPHONY_ERROR);
    EXPECT_GT(call->SendUpdateCallMediaModeRequest(ImsCallMode::CALL_MODE_AUDIO_ONLY), TELEPHONY_ERROR);
    EXPECT_GT(call->SendUpdateCallMediaModeResponse(ImsCallMode::CALL_MODE_AUDIO_ONLY), TELEPHONY_ERROR);
    call->SetCallId(-2);
    EXPECT_EQ(call->CombineConference(), CALL_ERR_INVALID_CALLID);
    call->SetCallId(0);
    DelayedSingleton<ImsConference>::GetInstance()->state_ = ConferenceState::CONFERENCE_STATE_IDLE;
    EXPECT_NE(call->CombineConference(), CALL_ERR_INVALID_CALLID);
    EXPECT_EQ(DelayedSingleton<ImsConference>::GetInstance()->state_, ConferenceState::CONFERENCE_STATE_CREATING);
    EXPECT_NE(call->CombineConference(), CALL_ERR_INVALID_CALLID);
    call->HandleCombineConferenceFailEvent();
    EXPECT_GT(call->CanCombineConference(), TELEPHONY_ERROR);
    EXPECT_GT(call->HoldConference(), TELEPHONY_ERROR);
    EXPECT_GT(call->LaunchConference(), TELEPHONY_ERROR);
    EXPECT_GT(call->HoldConference(), TELEPHONY_ERROR);
    EXPECT_GT(call->ExitConference(), TELEPHONY_ERROR);
    DelayedSingleton<ImsConference>::GetInstance()->state_ = ConferenceState::CONFERENCE_STATE_IDLE;
    EXPECT_GT(call->LaunchConference(), TELEPHONY_ERROR);
    call->SetTelCallState(TelCallState::CALL_STATUS_ACTIVE);
    EXPECT_GT(call->UpdateImsCallMode(ImsCallMode::CALL_MODE_AUDIO_ONLY), TELEPHONY_ERROR);
    call->SetTelCallState(TelCallState::CALL_STATUS_INCOMING);
    EXPECT_FALSE(call->IsSupportVideoCall());
    EXPECT_FALSE(call->IsVoiceModifyToVideo());
}

/**
 * @tc.number   Telephony_CallConnectAbility_001
 * @tc.name     test CallConnectAbility
 * @tc.desc     Function test
 */
HWTEST_F(ZeroBranch7Test, Telephony_CallConnectAbility_001, Function | MediumTest | Level1)
{
    DelayedSingleton<CallConnectAbility>::GetInstance()->isDisconnecting_ = false;
    DelayedSingleton<CallConnectAbility>::GetInstance()->isConnected_ = true;
    DelayedSingleton<CallConnectAbility>::GetInstance()->ConnectAbility();
    EXPECT_FALSE(DelayedSingleton<CallConnectAbility>::GetInstance()->isConnecting_);
    DelayedSingleton<CallConnectAbility>::GetInstance()->connectCallback_ = nullptr;
    DelayedSingleton<CallConnectAbility>::GetInstance()->DisconnectAbility();
    EXPECT_TRUE(DelayedSingleton<CallConnectAbility>::GetInstance()->isDisconnecting_);
    DelayedSingleton<CallConnectAbility>::GetInstance()->isConnected_ = false;
    DelayedSingleton<CallConnectAbility>::GetInstance()->isConnecting_ = true;
    DelayedSingleton<CallConnectAbility>::GetInstance()->isDisconnecting_ = false;
    DelayedSingleton<CallConnectAbility>::GetInstance()->SetDisconnectingFlag(false);
    DelayedSingleton<CallConnectAbility>::GetInstance()->WaitForConnectResult();
    DelayedSingleton<CallConnectAbility>::GetInstance()->isDisconnecting_ = true;
    DialParaInfo info;
    sptr<CallBase> call = new IMSCall(info);
    CallObjectManager::callObjectPtrList_.clear();
    CallObjectManager::AddOneCallObject(call);
    DelayedSingleton<CallConnectAbility>::GetInstance()->SetConnectingFlag(false);
    EXPECT_FALSE(DelayedSingleton<CallConnectAbility>::GetInstance()->isConnecting_);
    CallObjectManager::callObjectPtrList_.clear();
    DelayedSingleton<CallConnectAbility>::GetInstance()->SetConnectingFlag(false);
}

/**
 * @tc.number   Telephony_CallConnectAbility_002
 * @tc.name     test CallConnectAbility
 * @tc.desc     Function test
 */
HWTEST_F(ZeroBranch7Test, Telephony_CallConnectAbility_002, Function | MediumTest | Level1)
{
    DelayedSingleton<CallConnectAbility>::GetInstance()->isDisconnecting_ = false;
    DelayedSingleton<CallConnectAbility>::GetInstance()->isConnected_ = true;
    DelayedSingleton<CallConnectAbility>::GetInstance()->connectCallback_ = nullptr;
    DelayedSingleton<CallConnectAbility>::GetInstance()->DisconnectAbility();
    EXPECT_TRUE(DelayedSingleton<CallConnectAbility>::GetInstance()->isDisconnecting_);

    DelayedSingleton<CallConnectAbility>::GetInstance()->isDisconnecting_ = false;
    DelayedSingleton<CallConnectAbility>::GetInstance()->isConnected_ = false;
    DelayedSingleton<CallConnectAbility>::GetInstance()->ConnectAbility();

    DelayedSingleton<CallConnectAbility>::GetInstance()->isConnecting_ = false;
    DelayedSingleton<CallConnectAbility>::GetInstance()->isConnected_ = true;
    DelayedSingleton<CallConnectAbility>::GetInstance()->ConnectAbility();
    EXPECT_NE(DelayedSingleton<CallConnectAbility>::GetInstance()->connectCallback_, nullptr);
}

/**
 * @tc.number   Telephony_OTTCall_001
 * @tc.name     test OTTCall
 * @tc.desc     Function test
 */
HWTEST_F(ZeroBranch7Test, Telephony_OTTCall_001, Function | MediumTest | Level1)
{
    DialParaInfo info;
    std::shared_ptr<OTTCall> call = std::make_shared<OTTCall>(info);
    for (int16_t i = 0; i <= kMaxNumberLen + 1; i++) {
        call->accountNumber_ += "1";
    }
    call->callRunningState_ = CallRunningState::CALL_RUNNING_STATE_RINGING;
    EXPECT_EQ(call->AnswerCall(0), CALL_ERR_ANSWER_FAILED);
    EXPECT_EQ(call->RejectCall(), CALL_ERR_REJECT_FAILED);
    EXPECT_EQ(call->HangUpCall(), CALL_ERR_HANGUP_FAILED);
    EXPECT_EQ(call->HoldCall(), CALL_ERR_HOLD_FAILED);
    EXPECT_EQ(call->UnHoldCall(), CALL_ERR_UNHOLD_FAILED);
    EXPECT_EQ(call->SwitchCall(), CALL_ERR_UNHOLD_FAILED);
    EXPECT_EQ(call->CombineConference(), CALL_ERR_NUMBER_OUT_OF_RANGE);
    EXPECT_EQ(call->SeparateConference(), CALL_ERR_NUMBER_OUT_OF_RANGE);
    EXPECT_EQ(call->KickOutFromConference(), CALL_ERR_NUMBER_OUT_OF_RANGE);
    call->accountNumber_ = "1111";
    EXPECT_NE(call->AnswerCall(0), CALL_ERR_NUMBER_OUT_OF_RANGE);
    EXPECT_NE(call->RejectCall(), CALL_ERR_NUMBER_OUT_OF_RANGE);
    EXPECT_NE(call->HangUpCall(), CALL_ERR_NUMBER_OUT_OF_RANGE);
    EXPECT_NE(call->HoldCall(), CALL_ERR_NUMBER_OUT_OF_RANGE);
    EXPECT_NE(call->UnHoldCall(), CALL_ERR_NUMBER_OUT_OF_RANGE);
    EXPECT_NE(call->SwitchCall(), CALL_ERR_NUMBER_OUT_OF_RANGE);
    EXPECT_NE(call->CombineConference(), CALL_ERR_NUMBER_OUT_OF_RANGE);
    EXPECT_NE(call->SeparateConference(), CALL_ERR_NUMBER_OUT_OF_RANGE);
    EXPECT_NE(call->KickOutFromConference(), CALL_ERR_NUMBER_OUT_OF_RANGE);
    call->ottCallConnectionPtr_ = nullptr;
    EXPECT_EQ(call->AnswerCall(0), TELEPHONY_ERR_LOCAL_PTR_NULL);
    EXPECT_EQ(call->CombineConference(), TELEPHONY_ERR_LOCAL_PTR_NULL);
    EXPECT_EQ(call->SeparateConference(), TELEPHONY_ERR_LOCAL_PTR_NULL);
    EXPECT_EQ(call->KickOutFromConference(), TELEPHONY_ERR_LOCAL_PTR_NULL);
    for (int16_t i = 0; i <= kMaxBundleNameLen + 1; i++) {
        call->bundleName_ += "1";
    }
    OttCallRequestInfo requestInfo;
    EXPECT_EQ(call->PackOttCallRequestInfo(requestInfo), CALL_ERR_NUMBER_OUT_OF_RANGE);
}

/**
 * @tc.number   Telephony_CallSuperPrivacyControlManager_001
 * @tc.name     test CallSuperPrivacyControlManager
 * @tc.desc     Function test
 */
HWTEST_F(ZeroBranch7Test, Telephony_CallSuperPrivacyControlManager_001, Function | MediumTest | Level1)
{
    DelayedSingleton<CallSuperPrivacyControlManager>::GetInstance()->ParamChangeCallback(nullptr, "", nullptr);
    DelayedSingleton<CallSuperPrivacyControlManager>::GetInstance()->ParamChangeCallback("", nullptr, nullptr);
    DelayedSingleton<CallSuperPrivacyControlManager>::GetInstance()->ParamChangeCallback(nullptr, nullptr, nullptr);
    DialParaInfo info;
    info.isEcc = true;
    sptr<CallBase> call0 = new IMSCall(info);
    call0->SetCallId(0);
    call0->isEccContact_ = false;
    info.isEcc = true;
    sptr<CallBase> call1 = new IMSCall(info);
    call1->SetCallId(1);
    call1->isEccContact_ = true;
    info.isEcc = false;
    sptr<CallBase> call2 = new IMSCall(info);
    call2->SetCallId(2);
    call2->isEccContact_ = true;
    info.isEcc = false;
    sptr<CallBase> call3 = new IMSCall(info);
    call3->SetCallId(3);
    call3->isEccContact_ = false;
    call3->SetTelCallState(TelCallState::CALL_STATUS_WAITING);
    sptr<CallBase> call4 = new IMSCall(info);
    call4->SetCallId(4);
    call4->isEccContact_ = false;
    call4->SetTelCallState(TelCallState::CALL_STATUS_INCOMING);
    sptr<CallBase> call5 = new IMSCall(info);
    call5->SetCallId(5);
    call5->isEccContact_ = false;
    call5->SetTelCallState(TelCallState::CALL_STATUS_DISCONNECTED);
    CallObjectManager::AddOneCallObject(call0);
    CallObjectManager::AddOneCallObject(call1);
    CallObjectManager::AddOneCallObject(call2);
    CallObjectManager::AddOneCallObject(call3);
    CallObjectManager::AddOneCallObject(call4);
    CallObjectManager::AddOneCallObject(call5);
    DelayedSingleton<CallSuperPrivacyControlManager>::GetInstance()->CloseAllCall();
    EXPECT_FALSE(DelayedSingleton<CallSuperPrivacyControlManager>::GetInstance()->GetCurrentIsSuperPrivacyMode());
    CallObjectManager::callObjectPtrList_.clear();
}

/**
 * @tc.number   Telephony_CallRequestEventHandlerHelper_001
 * @tc.name     test CallRequestEventHandlerHelper
 * @tc.desc     Function test
 */
HWTEST_F(ZeroBranch7Test, Telephony_CallRequestEventHandlerHelper_001, Function | MediumTest | Level1)
{
    auto callRequestEventHandler = DelayedSingleton<CallRequestEventHandlerHelper>::GetInstance();
    callRequestEventHandler->pendingHangupCallId_ = 0;
    callRequestEventHandler->pendingHangup_ = false;
    EXPECT_FALSE(callRequestEventHandler->IsPendingHangup());
    EXPECT_EQ(callRequestEventHandler->GetPendingHangupCallId(), -1);
    callRequestEventHandler->pendingHangup_ = true;
    EXPECT_EQ(callRequestEventHandler->GetPendingHangupCallId(), 0);
}

/**
 * @tc.number   Telephony_CallBase_001
 * @tc.name     test CallBase
 * @tc.desc     Function test
 */
HWTEST_F(ZeroBranch7Test, Telephony_CallBase_001, Function | MediumTest | Level1)
{
    DialParaInfo info;
    sptr<CallBase> call0 = new VoIPCall(info);
    call0->SetCallId(0);
    call0->SetCallType(CallType::TYPE_VOIP);
    call0->SetTelCallState(TelCallState::CALL_STATUS_ACTIVE);
    sptr<CallBase> call1 = new VoIPCall(info);
    call1->SetCallId(1);
    call1->SetCallType(CallType::TYPE_VOIP);
    call1->SetTelCallState(TelCallState::CALL_STATUS_INCOMING);
    sptr<CallBase> call2 = new VoIPCall(info);
    call2->SetCallId(2);
    call2->SetCallType(CallType::TYPE_VOIP);
    call2->SetTelCallState(TelCallState::CALL_STATUS_DIALING);
    sptr<CallBase> call3 = new IMSCall(info);
    call3->SetCallId(3);
    call3->SetCallType(CallType::TYPE_IMS);
    CallObjectManager::AddOneCallObject(call0);
    CallObjectManager::AddOneCallObject(call1);
    CallObjectManager::AddOneCallObject(call2);
    CallObjectManager::AddOneCallObject(call3);
    call3->HangUpVoipCall();
    call3->conferenceState_ = TelConferenceState::TEL_CONFERENCE_ACTIVE;
    call3->StateChangesToHolding();
    EXPECT_EQ(call3->conferenceState_, TelConferenceState::TEL_CONFERENCE_DISCONNECTED);
    call3->conferenceState_ = TelConferenceState::TEL_CONFERENCE_DISCONNECTING;
    call3->ringEndTime_ = 0;
    call3->StateChangesToDisconnected();
    EXPECT_EQ(call3->conferenceState_, TelConferenceState::TEL_CONFERENCE_DISCONNECTED);
    call3->conferenceState_ = TelConferenceState::TEL_CONFERENCE_ACTIVE;
    call3->ringEndTime_ = 0;
    call3->StateChangesToDisconnected();
    EXPECT_EQ(call3->conferenceState_, TelConferenceState::TEL_CONFERENCE_DISCONNECTED);
    call3->conferenceState_ = TelConferenceState::TEL_CONFERENCE_ACTIVE;
    call3->ringEndTime_ = 1;
    call3->StateChangesToDisconnecting();
    EXPECT_EQ(call3->conferenceState_, TelConferenceState::TEL_CONFERENCE_DISCONNECTING);
    call3->conferenceState_ = TelConferenceState::TEL_CONFERENCE_ACTIVE;
    call3->StateChangesToDisconnecting();
    EXPECT_EQ(call3->conferenceState_, TelConferenceState::TEL_CONFERENCE_DISCONNECTING);
    CallObjectManager::callObjectPtrList_.clear();
}
} // namespace Telephony
} // namespace OHOS
