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

#include "call_manager_special_mock.h"
#include <thread>
#include <chrono>
#include "call_manager_service_proxy.h"
#include "call_manager_errors.h"
#include "message_option.h"
#include "message_parcel.h"
#include "call_manager_proxy.h"
#include "call_status_callback_proxy.h"
#include "voip_call_manager_proxy.h"
#include "call_status_callback.h"
#include "cellular_call_proxy.h"

namespace OHOS::Telephony {
using namespace testing::ext;
class SpecialBranch3Test : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void SpecialBranch3Test::SetUpTestCase()
{
}

void SpecialBranch3Test::TearDownTestCase()
{
}

void SpecialBranch3Test::SetUp()
{
}

void SpecialBranch3Test::TearDown()
{
    sleep(1);
}

/**
 * @tc.number   Telephony_CallManagerServiceProxy_001
 * @tc.name     test branch
 * @tc.desc     Function test
 */
HWTEST_F(SpecialBranch3Test, Telephony_CallManagerServiceProxy_001, TestSize.Level1)
{
    sptr<IRemoteObject> impl;
    CallManagerServiceProxy proxy(impl);
    EXPECT_EQ(proxy.ObserverOnCallDetailsChange(), TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL);
    EXPECT_EQ(proxy.MakeCall(""), TELEPHONY_ERR_ARGUMENT_INVALID);
    EXPECT_EQ(proxy.MakeCall("12345"), TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL);
    EXPECT_EQ(proxy.AnswerCall(0, 0), TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL);
    EXPECT_EQ(proxy.HoldCall(0), TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL);
    EXPECT_EQ(proxy.UnHoldCall(0), TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL);
    EXPECT_EQ(proxy.SwitchCall(0), TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL);
    bool enabled = false;
    EXPECT_EQ(proxy.IsNewCallAllowed(enabled), TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL);
    EXPECT_EQ(proxy.SetMuted(false), TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL);
    AudioDevice device;
    EXPECT_EQ(proxy.SetAudioDevice(device), TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL);
    EXPECT_EQ(proxy.IsRinging(enabled), TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL);
    EXPECT_EQ(proxy.StartDtmf(0, '0'), TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL);
    EXPECT_EQ(proxy.StopDtmf(0), TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL);
    EXPECT_EQ(proxy.PostDialProceed(0, false), TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL);
    EXPECT_EQ(proxy.GetCallWaiting(0), TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL);
    EXPECT_EQ(proxy.SetCallWaiting(0, false), TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL);
    EXPECT_EQ(proxy.PostDialProceed(0, false), TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL);
    EXPECT_EQ(proxy.GetCallRestriction(0, CallRestrictionType::RESTRICTION_TYPE_ALL_INCOMING),
        TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL);
    CallRestrictionInfo info;
    EXPECT_EQ(proxy.SetCallRestriction(0, info), TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL);
    const char *oldPass = nullptr;
    const char *newPass = nullptr;
    EXPECT_EQ(proxy.SetCallRestrictionPassword(0, CallRestrictionType::RESTRICTION_TYPE_ALL_INCOMING,
        oldPass, newPass), TELEPHONY_ERR_ARGUMENT_INVALID);
    oldPass = "str";
    EXPECT_EQ(proxy.SetCallRestrictionPassword(0, CallRestrictionType::RESTRICTION_TYPE_ALL_INCOMING,
        oldPass, newPass), TELEPHONY_ERR_ARGUMENT_INVALID);
    newPass = "str";
    EXPECT_EQ(proxy.SetCallRestrictionPassword(0, CallRestrictionType::RESTRICTION_TYPE_ALL_INCOMING,
        oldPass, newPass), TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL);
    oldPass = nullptr;
    EXPECT_EQ(proxy.SetCallRestrictionPassword(0, CallRestrictionType::RESTRICTION_TYPE_ALL_INCOMING,
        oldPass, newPass), TELEPHONY_ERR_ARGUMENT_INVALID);
    EXPECT_EQ(proxy.GetCallTransferInfo(0, CallTransferType::TRANSFER_TYPE_UNCONDITIONAL),
        TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL);
    CallTransferInfo transferInfo;
    EXPECT_EQ(proxy.SetCallTransferInfo(0, transferInfo), TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL);
    EXPECT_EQ(proxy.CanSetCallTransferTime(0, enabled), TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL);
}

/**
 * @tc.number   Telephony_CallManagerServiceProxy_003
 * @tc.name     test branch
 * @tc.desc     Function test
 */
HWTEST_F(SpecialBranch3Test, Telephony_CallManagerServiceProxy_003, TestSize.Level1)
{
    sptr<IRemoteObject> impl;
    CallManagerServiceProxy proxy(impl);
    EXPECT_EQ(proxy.CloseUnFinishedUssd(0), TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL);
    std::string code = "";
    EXPECT_EQ(proxy.InputDialerSpecialCode(code), TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL);
    EXPECT_EQ(proxy.RemoveMissedIncomingCallNotification(), TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL);
    EXPECT_EQ(proxy.SetVoIPCallState(0), TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL);
    int32_t callId = 0;
    int32_t state = 0;
    std::string phoneNumber = "123456";
    EXPECT_EQ(proxy.SetVoIPCallInfo(callId, state, phoneNumber), TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL);
    EXPECT_EQ(proxy.CancelCallUpgrade(0), TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL);
    EXPECT_EQ(proxy.RequestCameraCapabilities(0), TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL);
    EXPECT_EQ(proxy.SendCallUiEvent(0, code), TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL);
    EXPECT_EQ(proxy.RegisterBluetoothCallManagerCallbackPtr(code), nullptr);
    EXPECT_EQ(proxy.SendUssdResponse(0, code), TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL);
    std::vector<std::string> dialingList;
    std::vector<std::string> incomingList;
    EXPECT_EQ(proxy.SetCallPolicyInfo(false, dialingList, false, incomingList), TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL);
}

/**
 * @tc.number   Telephony_CallStatusCallbackProxy_001
 * @tc.name     test branch
 * @tc.desc     Function test
 */
HWTEST_F(SpecialBranch3Test, Telephony_CallStatusCallbackProxy_001, TestSize.Level1)
{
    auto callStatusCallbackProxy = std::make_shared<CallStatusCallbackProxy>(nullptr);
    CallModeReportInfo response;
    EXPECT_EQ(callStatusCallbackProxy->ReceiveUpdateCallMediaModeRequest(response),
        TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL);
    EXPECT_EQ(callStatusCallbackProxy->ReceiveUpdateCallMediaModeResponse(response),
        TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL);
    std::string str = "";
    const VoipCallEventInfo info;
    EXPECT_EQ(callStatusCallbackProxy->ReportPostDialChar(str), TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL);
    EXPECT_EQ(callStatusCallbackProxy->ReportPostDialDelay(str), TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL);
    EXPECT_EQ(callStatusCallbackProxy->UpdateVoipEventInfo(info), TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL);
    CallSessionReportInfo options;
    EXPECT_EQ(callStatusCallbackProxy->HandleCallSessionEventChanged(options), TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL);
    PeerDimensionsReportInfo peerInfo;
    EXPECT_EQ(callStatusCallbackProxy->HandlePeerDimensionsChanged(peerInfo), TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL);
    int64_t result = 0;
    EXPECT_EQ(callStatusCallbackProxy->HandleCallDataUsageChanged(result), TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL);
    CameraCapabilitiesReportInfo cameraInfo;
    EXPECT_EQ(callStatusCallbackProxy->HandleCameraCapabilitiesChanged(cameraInfo),
        TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL);
}

/**
 * @tc.number   Telephony_callularCallProxy_001
 * @tc.name     test branch
 * @tc.desc     Function test
 */
HWTEST_F(SpecialBranch3Test, Telephony_callularCallProxy_001, TestSize.Level1)
{
    sptr<IRemoteObject> impl;
    int32_t slotId = 0;
    CellularCallInfo callInfo;
    auto cellularCallProxy = std::make_shared<CellularCallProxy>(impl);
    EXPECT_NE(cellularCallProxy->Dial(callInfo), TELEPHONY_SUCCESS);
    EXPECT_NE(cellularCallProxy->Reject(callInfo), TELEPHONY_SUCCESS);
    EXPECT_NE(cellularCallProxy->Answer(callInfo), TELEPHONY_SUCCESS);
    EXPECT_NE(cellularCallProxy->HoldCall(callInfo), TELEPHONY_SUCCESS);
    EXPECT_NE(cellularCallProxy->UnHoldCall(callInfo), TELEPHONY_SUCCESS);
    EXPECT_NE(cellularCallProxy->SwitchCall(callInfo), TELEPHONY_SUCCESS);
    EXPECT_NE(cellularCallProxy->CombineConference(callInfo), TELEPHONY_SUCCESS);
    EXPECT_NE(cellularCallProxy->SeparateConference(callInfo), TELEPHONY_SUCCESS);
    EXPECT_NE(cellularCallProxy->KickOutFromConference(callInfo), TELEPHONY_SUCCESS);
    EXPECT_NE(cellularCallProxy->HangUpAllConnection(), TELEPHONY_SUCCESS);
    EXPECT_NE(cellularCallProxy->StopDtmf(callInfo), TELEPHONY_SUCCESS);
    EXPECT_NE(cellularCallProxy->StopRtt(slotId), TELEPHONY_SUCCESS);
    EXPECT_NE(cellularCallProxy->GetDomainPreferenceMode(slotId), TELEPHONY_SUCCESS);
    std::string value = "";
    EXPECT_NE(cellularCallProxy->SetImsConfig(slotId, ImsConfigItem::ITEM_VIDEO_QUALITY, value), TELEPHONY_SUCCESS);
}

/**
 * @tc.number   Telephony_callularCallProxy_002
 * @tc.name     test branch
 * @tc.desc     Function test
 */
HWTEST_F(SpecialBranch3Test, Telephony_callularCallProxy_002, TestSize.Level1)
{
    sptr<IRemoteObject> impl;
    int32_t slotId = 0;
    CellularCallInfo callInfo;
    CallTransferInfo ctInfo;
    auto Proxy = std::make_shared<CellularCallProxy>(impl);
    std::vector<std::string> numberList;
    char cDtmfCode = '1';
    bool proceed = false;
    bool result = false;
    bool enabled = false;
    int32_t index = 1;
    std::string cameraId = "";
    std::string surfaceId = "123";
    std::string msg = "";
    sptr<Surface> surface;
    std::string path = "";
    std::vector<EmergencyCall> eccVec;
    std::vector<CellularCallInfo> infos;
    std::string number = "0123456789";
    EXPECT_NE(Proxy->HangUp(callInfo, CallSupplementType::TYPE_DEFAULT), TELEPHONY_SUCCESS);
    EXPECT_NE(Proxy->InviteToConference(slotId, numberList), TELEPHONY_SUCCESS);
    EXPECT_NE(Proxy->InviteToConference(slotId, numberList), TELEPHONY_SUCCESS);
    EXPECT_NE(Proxy->SendUpdateCallMediaModeRequest(callInfo, ImsCallMode::CALL_MODE_AUDIO_ONLY), TELEPHONY_SUCCESS);
    EXPECT_NE(Proxy->SendUpdateCallMediaModeResponse(callInfo, ImsCallMode::CALL_MODE_AUDIO_ONLY), TELEPHONY_SUCCESS);
    EXPECT_NE(Proxy->StartDtmf(cDtmfCode, callInfo), TELEPHONY_SUCCESS);
    EXPECT_NE(Proxy->PostDialProceed(callInfo, 0), TELEPHONY_SUCCESS);
    EXPECT_NE(Proxy->SendDtmf(cDtmfCode, callInfo), TELEPHONY_SUCCESS);
    EXPECT_NE(Proxy->StartRtt(slotId, msg), TELEPHONY_SUCCESS);
    EXPECT_NE(Proxy->SetCallTransferInfo(slotId, ctInfo), TELEPHONY_SUCCESS);
    EXPECT_NE(Proxy->CanSetCallTransferTime(slotId, result), TELEPHONY_SUCCESS);
    EXPECT_NE(Proxy->GetVideoCallWaiting(slotId, enabled), TELEPHONY_SUCCESS);
    EXPECT_NE(Proxy->SetDomainPreferenceMode(slotId, -1), TELEPHONY_SUCCESS);
    EXPECT_NE(Proxy->GetCarrierVtConfig(slotId, enabled), TELEPHONY_SUCCESS);
    EXPECT_NE(Proxy->SetImsConfig(slotId, ImsConfigItem::ITEM_VIDEO_QUALITY, 1), TELEPHONY_SUCCESS);
    EXPECT_NE(Proxy->ControlCamera(slotId, index, cameraId), TELEPHONY_SUCCESS);
    EXPECT_NE(Proxy->SetPreviewWindow(slotId, index, surfaceId, surface), TELEPHONY_SUCCESS);
    EXPECT_NE(Proxy->SetDisplayWindow(slotId, index, surfaceId, surface), TELEPHONY_SUCCESS);
    EXPECT_NE(Proxy->SetCameraZoom(1.0), TELEPHONY_SUCCESS);
    EXPECT_NE(Proxy->SetPausePicture(slotId, index, path), TELEPHONY_SUCCESS);
    EXPECT_NE(Proxy->SetDeviceDirection(slotId, index, 0), TELEPHONY_SUCCESS);
    EXPECT_NE(Proxy->SetMute(slotId, 0), TELEPHONY_SUCCESS);
    EXPECT_NE(Proxy->SetEmergencyCallList(slotId, eccVec), TELEPHONY_SUCCESS);
    EXPECT_NE(Proxy->ClearAllCalls(infos), TELEPHONY_SUCCESS);
    EXPECT_NE(Proxy->CancelCallUpgrade(slotId, index), TELEPHONY_SUCCESS);
    EXPECT_NE(Proxy->RequestCameraCapabilities(slotId, index), TELEPHONY_SUCCESS);
    EXPECT_FALSE(Proxy->IsMmiCode(slotId, number));
}
} // namespace OHOS::Telephony
