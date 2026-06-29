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

#define private public
#define protected public
#include "call_manager_service_stub.h"

#include <gtest/gtest.h>
#include <cstring>
#include <string>

#include "call_manager_errors.h"
#include "call_manager_service_ipc_interface_code.h"
#include "message_parcel.h"
#include "message_option.h"

namespace OHOS {
namespace Telephony {
using namespace testing;
using namespace testing::ext;

constexpr int32_t INVALID_CODE = 9999;
constexpr int32_t VALID_CALL_ID = 1;
constexpr int32_t VALID_SLOT_ID = 0;

class CallManagerServiceStubTest : public testing::Test {
public:
    static void SetUpTestCase() {}
    static void TearDownTestCase() {}
    void SetUp() {}
    void TearDown() {}
};

class CallManagerServiceStubMock : public CallManagerServiceStub {
public:
    CallManagerServiceStubMock() = default;
    ~CallManagerServiceStubMock() = default;

    int32_t RegisterCallBack(const sptr<ICallAbilityCallback> &callback) override
    {
        return TELEPHONY_SUCCESS;
    }
    int32_t UnRegisterCallBack() override
    {
        return TELEPHONY_SUCCESS;
    }
    int32_t ObserverOnCallDetailsChange() override
    {
        return TELEPHONY_SUCCESS;
    }
    int32_t DialCall(std::u16string number, AppExecFwk::PacMap &extras) override
    {
        return TELEPHONY_SUCCESS;
    }
    int32_t MakeCall(std::string number) override
    {
        return TELEPHONY_SUCCESS;
    }
    int32_t AnswerCall(int32_t callId, int32_t videoState, bool isRTT) override
    {
        return TELEPHONY_SUCCESS;
    }
    int32_t RejectCall(int32_t callId, bool rejectWithMessage, std::u16string textMessage) override
    {
        return TELEPHONY_SUCCESS;
    }
    int32_t HangUpCall(int32_t callId) override
    {
        return TELEPHONY_SUCCESS;
    }
    int32_t GetCallState() override
    {
        return TELEPHONY_SUCCESS;
    }
    int32_t HoldCall(int32_t callId) override
    {
        return TELEPHONY_SUCCESS;
    }
    int32_t UnHoldCall(int32_t callId) override
    {
        return TELEPHONY_SUCCESS;
    }
    int32_t SwitchCall(int32_t callId) override
    {
        return TELEPHONY_SUCCESS;
    }
    bool HasCall(const bool isInCludeVoipCall) override
    {
        return true;
    }
    int32_t IsNewCallAllowed(bool &enabled) override
    {
        enabled = true;
        return TELEPHONY_SUCCESS;
    }
    int32_t SetMuted(bool isMute) override
    {
        return TELEPHONY_SUCCESS;
    }
    int32_t MuteRinger() override
    {
        return TELEPHONY_SUCCESS;
    }
    int32_t SetAudioDevice(const AudioDevice &audioDevice) override
    {
        return TELEPHONY_SUCCESS;
    }
    int32_t IsRinging(bool &enabled) override
    {
        enabled = false;
        return TELEPHONY_SUCCESS;
    }
    int32_t IsInEmergencyCall(bool &enabled) override
    {
        enabled = false;
        return TELEPHONY_SUCCESS;
    }
    int32_t StartDtmf(int32_t callId, char str) override
    {
        return TELEPHONY_SUCCESS;
    }
    int32_t StopDtmf(int32_t callId) override
    {
        return TELEPHONY_SUCCESS;
    }
    int32_t PostDialProceed(int32_t callId, bool proceed) override
    {
        return TELEPHONY_SUCCESS;
    }
    int32_t GetCallWaiting(int32_t slotId) override
    {
        return TELEPHONY_SUCCESS;
    }
    int32_t SetCallWaiting(int32_t slotId, bool activate) override
    {
        return TELEPHONY_SUCCESS;
    }
    int32_t GetCallRestriction(int32_t slotId, CallRestrictionType type) override
    {
        return TELEPHONY_SUCCESS;
    }
    int32_t SetCallRestriction(int32_t slotId, CallRestrictionInfo &info) override
    {
        return TELEPHONY_SUCCESS;
    }
    int32_t SetCallRestrictionPassword(
        int32_t slotId, CallRestrictionType fac, const char *oldPassword, const char *newPassword) override
    {
        return TELEPHONY_SUCCESS;
    }
    int32_t GetCallTransferInfo(int32_t slotId, CallTransferType type) override
    {
        return TELEPHONY_SUCCESS;
    }
    int32_t SetCallTransferInfo(int32_t slotId, CallTransferInfo &info) override
    {
        return TELEPHONY_SUCCESS;
    }
    int32_t CanSetCallTransferTime(int32_t slotId, bool &result) override
    {
        result = true;
        return TELEPHONY_SUCCESS;
    }
    int32_t CombineConference(int32_t mainCallId) override
    {
        return TELEPHONY_SUCCESS;
    }
    int32_t SeparateConference(int32_t callId) override
    {
        return TELEPHONY_SUCCESS;
    }
    int32_t KickOutFromConference(int32_t callId) override
    {
        return TELEPHONY_SUCCESS;
    }
    int32_t IsEmergencyPhoneNumber(std::u16string &number, int32_t slotId, bool &enabled) override
    {
        enabled = false;
        return TELEPHONY_SUCCESS;
    }
    int32_t FormatPhoneNumber(
        std::u16string &number, std::u16string &countryCode, std::u16string &formatNumber) override
    {
        return TELEPHONY_SUCCESS;
    }
    int32_t FormatPhoneNumberToE164(
        std::u16string &number, std::u16string &countryCode, std::u16string &formatNumber) override
    {
        return TELEPHONY_SUCCESS;
    }
    int32_t GetMainCallId(int32_t callId, int32_t &mainCallId) override
    {
        mainCallId = 1;
        return TELEPHONY_SUCCESS;
    }
    int32_t GetSubCallIdList(int32_t callId, std::vector<std::u16string> &callIdList) override
    {
        return TELEPHONY_SUCCESS;
    }
    int32_t GetCallIdListForConference(int32_t callId, std::vector<std::u16string> &callIdList) override
    {
        return TELEPHONY_SUCCESS;
    }
    int32_t ControlCamera(int32_t callId, std::u16string &cameraId) override
    {
        return TELEPHONY_SUCCESS;
    }
    int32_t SetPreviewWindow(int32_t callId, std::string &surfaceId, sptr<Surface> surface) override
    {
        return TELEPHONY_SUCCESS;
    }
    int32_t SetDisplayWindow(int32_t callId, std::string &surfaceId, sptr<Surface> surface) override
    {
        return TELEPHONY_SUCCESS;
    }
    int32_t SetCameraZoom(float zoomRatio) override
    {
        return TELEPHONY_SUCCESS;
    }
    int32_t SetPausePicture(int32_t callId, std::u16string &path) override
    {
        return TELEPHONY_SUCCESS;
    }
    int32_t SetDeviceDirection(int32_t callId, int32_t rotation) override
    {
        return TELEPHONY_SUCCESS;
    }
    int32_t SetCallPreferenceMode(int32_t slotId, int32_t mode) override
    {
        return TELEPHONY_SUCCESS;
    }
    int32_t GetImsConfig(int32_t slotId, ImsConfigItem item) override
    {
        return TELEPHONY_SUCCESS;
    }
    int32_t SetImsConfig(int32_t slotId, ImsConfigItem item, std::u16string &value) override
    {
        return TELEPHONY_SUCCESS;
    }
    int32_t GetImsFeatureValue(int32_t slotId, FeatureType type) override
    {
        return TELEPHONY_SUCCESS;
    }
    int32_t SetImsFeatureValue(int32_t slotId, FeatureType type, int32_t value) override
    {
        return TELEPHONY_SUCCESS;
    }
    int32_t UpdateImsCallMode(int32_t callId, ImsCallMode mode) override
    {
        return TELEPHONY_SUCCESS;
    }
    int32_t EnableImsSwitch(int32_t slotId) override
    {
        return TELEPHONY_SUCCESS;
    }
    int32_t DisableImsSwitch(int32_t slotId) override
    {
        return TELEPHONY_SUCCESS;
    }
    int32_t IsImsSwitchEnabled(int32_t slotId, bool &enabled) override
    {
        enabled = false;
        return TELEPHONY_SUCCESS;
    }
    int32_t SetVoNRState(int32_t slotId, int32_t state) override
    {
        return TELEPHONY_SUCCESS;
    }
    int32_t GetVoNRState(int32_t slotId, int32_t &state) override
    {
        state = 0;
        return TELEPHONY_SUCCESS;
    }
    int32_t JoinConference(int32_t callId, std::vector<std::u16string> &numberList) override
    {
        return TELEPHONY_SUCCESS;
    }
    int32_t ReportOttCallDetailsInfo(std::vector<OttCallDetailsInfo> &ottVec) override
    {
        return TELEPHONY_SUCCESS;
    }
    int32_t ReportOttCallEventInfo(OttCallEventInfo &eventInfo) override
    {
        return TELEPHONY_SUCCESS;
    }
    int32_t CloseUnFinishedUssd(int32_t slotId) override
    {
        return TELEPHONY_SUCCESS;
    }
    int32_t InputDialerSpecialCode(const std::string &specialCode) override
    {
        return TELEPHONY_SUCCESS;
    }
    int32_t RemoveMissedIncomingCallNotification() override
    {
        return TELEPHONY_SUCCESS;
    }
    int32_t SetVoIPCallState(int32_t state) override
    {
        return TELEPHONY_SUCCESS;
    }
    int32_t GetVoIPCallState(int32_t &state) override
    {
        state = 0;
        return TELEPHONY_SUCCESS;
    }
    int32_t SetVoIPCallInfo(const int32_t callId, const int32_t state, const std::string phoneNumber) override
    {
        return TELEPHONY_SUCCESS;
    }
    int32_t GetVoIPCallInfo(int32_t &callId, int32_t &state, std::string &phoneNumber) override
    {
        callId = 0;
        state = 0;
        phoneNumber = "";
        return TELEPHONY_SUCCESS;
    }
    sptr<IRemoteObject> GetProxyObjectPtr(CallManagerProxyType proxyType) override
    {
        return nullptr;
    }
    int32_t ReportAudioDeviceInfo() override
    {
        return TELEPHONY_SUCCESS;
    }
    int32_t CancelCallUpgrade(int32_t callId) override
    {
        return TELEPHONY_SUCCESS;
    }
    int32_t RequestCameraCapabilities(int32_t callId) override
    {
        return TELEPHONY_SUCCESS;
    }
    int32_t RegisterVoipCallManagerCallback() override
    {
        return TELEPHONY_SUCCESS;
    }
    int32_t UnRegisterVoipCallManagerCallback() override
    {
        return TELEPHONY_SUCCESS;
    }
    int32_t SendCallUiEvent(int32_t callId, std::string &eventName) override
    {
        return TELEPHONY_SUCCESS;
    }
    int32_t PreloadCallUi(bool enable) override
    {
        return TELEPHONY_SUCCESS;
    }
    sptr<ICallStatusCallback> RegisterBluetoothCallManagerCallbackPtr(std::string &macAddress) override
    {
        return nullptr;
    }
    int32_t SendUssdResponse(int32_t slotId, const std::string &content) override
    {
        return TELEPHONY_SUCCESS;
    }
    int32_t SetCallPolicyInfo(bool isDialingTrustlist, const std::vector<std::string> &dialingList,
        bool isIncomingTrustlist, const std::vector<std::string> &incomingList) override
    {
        return TELEPHONY_SUCCESS;
    }
    bool EndCall() override
    {
        return true;
    }
    bool HasDistributedCommunicationCapability() override
    {
        return false;
    }
    int32_t NotifyVoIPAudioStreamStart(int32_t uid) override
    {
        return TELEPHONY_SUCCESS;
    }
    int32_t SetCallAudioMode(int32_t mode, int32_t scenarios) override
    {
        return TELEPHONY_SUCCESS;
    }
    int32_t AnswerCall() override
    {
        return TELEPHONY_SUCCESS;
    }
    int32_t RejectCall() override
    {
        return TELEPHONY_SUCCESS;
    }
    int32_t HangUpCall() override
    {
        return TELEPHONY_SUCCESS;
    }
    int32_t GetCallTransferInfo(const std::string number, CallTransferType type) override
    {
        return TELEPHONY_SUCCESS;
    }
};

HWTEST_F(CallManagerServiceStubTest, CallManagerServiceStub_OnRemoteRequest_0100, TestSize.Level0)
{
    sptr<CallManagerServiceStubMock> stub = new CallManagerServiceStubMock();
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    data.WriteInterfaceToken(u"invalid_descriptor");

    int32_t result = stub->OnRemoteRequest(
        static_cast<uint32_t>(CallManagerInterfaceCode::INTERFACE_REGISTER_CALLBACK), data, reply, option);
    EXPECT_EQ(result, TELEPHONY_ERR_DESCRIPTOR_MISMATCH);
}

HWTEST_F(CallManagerServiceStubTest, CallManagerServiceStub_OnRemoteRequest_0200, TestSize.Level0)
{
    sptr<CallManagerServiceStubMock> stub = new CallManagerServiceStubMock();
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    data.WriteInterfaceToken(CallManagerServiceStub::GetDescriptor());

    int32_t result = stub->OnRemoteRequest(INVALID_CODE, data, reply, option);
    EXPECT_NE(result, TELEPHONY_SUCCESS);
}

HWTEST_F(CallManagerServiceStubTest, CallManagerServiceStub_OnRemoteRequest_0300, TestSize.Level0)
{
    sptr<CallManagerServiceStubMock> stub = new CallManagerServiceStubMock();
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    data.WriteInterfaceToken(CallManagerServiceStub::GetDescriptor());

    int32_t result = stub->OnRemoteRequest(
        static_cast<uint32_t>(CallManagerInterfaceCode::INTERFACE_REGISTER_CALLBACK), data, reply, option);
    EXPECT_NE(result, TELEPHONY_SUCCESS);
}

HWTEST_F(CallManagerServiceStubTest, CallManagerServiceStub_OnRegisterCallBack_0100, TestSize.Level1)
{
    sptr<CallManagerServiceStubMock> stub = new CallManagerServiceStubMock();
    MessageParcel data;
    MessageParcel reply;

    data.WriteRemoteObject(nullptr);

    int32_t result = stub->OnRegisterCallBack(data, reply);
    EXPECT_EQ(result, TELEPHONY_ERR_FAIL);
}

HWTEST_F(CallManagerServiceStubTest, CallManagerServiceStub_OnDialCall_0100, TestSize.Level1)
{
    sptr<CallManagerServiceStubMock> stub = new CallManagerServiceStubMock();
    MessageParcel data;
    MessageParcel reply;

    std::u16string longNumber(ACCOUNT_NUMBER_MAX_LENGTH + 1, u'1');
    data.WriteString16(longNumber);
    data.WriteInt32(0);
    data.WriteInt32(0);
    data.WriteInt32(0);
    data.WriteInt32(0);
    data.WriteInt32(0);
    data.WriteBool(false);
    data.WriteString("");
    data.WriteString("");
    data.WriteBool(false);

    int32_t result = stub->OnDialCall(data, reply);
    EXPECT_EQ(result, CALL_ERR_NUMBER_OUT_OF_RANGE);
}

HWTEST_F(CallManagerServiceStubTest, CallManagerServiceStub_OnDialCall_0200, TestSize.Level1)
{
    sptr<CallManagerServiceStubMock> stub = new CallManagerServiceStubMock();
    MessageParcel data;
    MessageParcel reply;

    std::u16string shortNumber(u"10086");
    data.WriteString16(shortNumber);
    data.WriteInt32(0);
    data.WriteInt32(0);
    data.WriteInt32(0);
    data.WriteInt32(0);
    data.WriteInt32(0);
    data.WriteBool(false);
    data.WriteString("");
    data.WriteString("");
    data.WriteBool(false);

    int32_t result = stub->OnDialCall(data, reply);
    EXPECT_EQ(result, TELEPHONY_SUCCESS);
}

HWTEST_F(CallManagerServiceStubTest, CallManagerServiceStub_OnMakeCall_0100, TestSize.Level1)
{
    sptr<CallManagerServiceStubMock> stub = new CallManagerServiceStubMock();
    MessageParcel data;
    MessageParcel reply;

    std::string longNumber(ACCOUNT_NUMBER_MAX_LENGTH + 1, '1');
    data.WriteString(longNumber);

    int32_t result = stub->OnMakeCall(data, reply);
    EXPECT_EQ(result, CALL_ERR_NUMBER_OUT_OF_RANGE);
}

HWTEST_F(CallManagerServiceStubTest, CallManagerServiceStub_OnMakeCall_0200, TestSize.Level1)
{
    sptr<CallManagerServiceStubMock> stub = new CallManagerServiceStubMock();
    MessageParcel data;
    MessageParcel reply;

    std::string shortNumber("10086");
    data.WriteString(shortNumber);

    int32_t result = stub->OnMakeCall(data, reply);
    EXPECT_EQ(result, TELEPHONY_SUCCESS);
}

HWTEST_F(CallManagerServiceStubTest, CallManagerServiceStub_OnAcceptCall_0100, TestSize.Level1)
{
    sptr<CallManagerServiceStubMock> stub = new CallManagerServiceStubMock();
    MessageParcel data;
    MessageParcel reply;

    data.WriteInt32(VALID_CALL_ID);
    data.WriteInt32(0);
    data.WriteBool(false);

    int32_t result = stub->OnAcceptCall(data, reply);
    EXPECT_EQ(result, TELEPHONY_SUCCESS);
}

HWTEST_F(CallManagerServiceStubTest, CallManagerServiceStub_OnRejectCall_0100, TestSize.Level1)
{
    sptr<CallManagerServiceStubMock> stub = new CallManagerServiceStubMock();
    MessageParcel data;
    MessageParcel reply;

    data.WriteInt32(VALID_CALL_ID);
    data.WriteBool(false);
    data.WriteString16(u"");

    int32_t result = stub->OnRejectCall(data, reply);
    EXPECT_EQ(result, TELEPHONY_SUCCESS);
}

HWTEST_F(CallManagerServiceStubTest, CallManagerServiceStub_OnHangUpCall_0100, TestSize.Level1)
{
    sptr<CallManagerServiceStubMock> stub = new CallManagerServiceStubMock();
    MessageParcel data;
    MessageParcel reply;

    data.WriteInt32(VALID_CALL_ID);

    int32_t result = stub->OnHangUpCall(data, reply);
    EXPECT_EQ(result, TELEPHONY_SUCCESS);
}

HWTEST_F(CallManagerServiceStubTest, CallManagerServiceStub_OnGetCallState_0100, TestSize.Level1)
{
    sptr<CallManagerServiceStubMock> stub = new CallManagerServiceStubMock();
    MessageParcel data;
    MessageParcel reply;

    int32_t result = stub->OnGetCallState(data, reply);
    EXPECT_EQ(result, TELEPHONY_SUCCESS);
}

HWTEST_F(CallManagerServiceStubTest, CallManagerServiceStub_OnHoldCall_0100, TestSize.Level1)
{
    sptr<CallManagerServiceStubMock> stub = new CallManagerServiceStubMock();
    MessageParcel data;
    MessageParcel reply;

    data.WriteInt32(VALID_CALL_ID);

    int32_t result = stub->OnHoldCall(data, reply);
    EXPECT_EQ(result, TELEPHONY_SUCCESS);
}

HWTEST_F(CallManagerServiceStubTest, CallManagerServiceStub_OnUnHoldCall_0100, TestSize.Level1)
{
    sptr<CallManagerServiceStubMock> stub = new CallManagerServiceStubMock();
    MessageParcel data;
    MessageParcel reply;

    data.WriteInt32(VALID_CALL_ID);

    int32_t result = stub->OnUnHoldCall(data, reply);
    EXPECT_EQ(result, TELEPHONY_SUCCESS);
}

HWTEST_F(CallManagerServiceStubTest, CallManagerServiceStub_OnSwitchCall_0100, TestSize.Level1)
{
    sptr<CallManagerServiceStubMock> stub = new CallManagerServiceStubMock();
    MessageParcel data;
    MessageParcel reply;

    data.WriteInt32(VALID_CALL_ID);

    int32_t result = stub->OnSwitchCall(data, reply);
    EXPECT_EQ(result, TELEPHONY_SUCCESS);
}

HWTEST_F(CallManagerServiceStubTest, CallManagerServiceStub_OnHasCall_0100, TestSize.Level1)
{
    sptr<CallManagerServiceStubMock> stub = new CallManagerServiceStubMock();
    MessageParcel data;
    MessageParcel reply;

    data.WriteBool(true);

    int32_t result = stub->OnHasCall(data, reply);
    EXPECT_EQ(result, TELEPHONY_SUCCESS);
}

HWTEST_F(CallManagerServiceStubTest, CallManagerServiceStub_OnIsNewCallAllowed_0100, TestSize.Level1)
{
    sptr<CallManagerServiceStubMock> stub = new CallManagerServiceStubMock();
    MessageParcel data;
    MessageParcel reply;

    int32_t result = stub->OnIsNewCallAllowed(data, reply);
    EXPECT_EQ(result, TELEPHONY_SUCCESS);
}

HWTEST_F(CallManagerServiceStubTest, CallManagerServiceStub_OnSetMute_0100, TestSize.Level1)
{
    sptr<CallManagerServiceStubMock> stub = new CallManagerServiceStubMock();
    MessageParcel data;
    MessageParcel reply;

    data.WriteBool(true);

    int32_t result = stub->OnSetMute(data, reply);
    EXPECT_EQ(result, TELEPHONY_SUCCESS);
}

HWTEST_F(CallManagerServiceStubTest, CallManagerServiceStub_OnMuteRinger_0100, TestSize.Level1)
{
    sptr<CallManagerServiceStubMock> stub = new CallManagerServiceStubMock();
    MessageParcel data;
    MessageParcel reply;

    int32_t result = stub->OnMuteRinger(data, reply);
    EXPECT_EQ(result, TELEPHONY_SUCCESS);
}

HWTEST_F(CallManagerServiceStubTest, CallManagerServiceStub_OnSetAudioDevice_0100, TestSize.Level1)
{
    sptr<CallManagerServiceStubMock> stub = new CallManagerServiceStubMock();
    MessageParcel data;
    MessageParcel reply;

    AudioDevice audioDevice;
    data.WriteRawData(&audioDevice, sizeof(audioDevice));

    int32_t result = stub->OnSetAudioDevice(data, reply);
    EXPECT_EQ(result, TELEPHONY_SUCCESS);
}

HWTEST_F(CallManagerServiceStubTest, CallManagerServiceStub_OnSetAudioDevice_0200, TestSize.Level1)
{
    sptr<CallManagerServiceStubMock> stub = new CallManagerServiceStubMock();
    MessageParcel data;
    MessageParcel reply;

    int32_t result = stub->OnSetAudioDevice(data, reply);
    EXPECT_EQ(result, TELEPHONY_ERR_LOCAL_PTR_NULL);
}

HWTEST_F(CallManagerServiceStubTest, CallManagerServiceStub_OnIsRinging_0100, TestSize.Level1)
{
    sptr<CallManagerServiceStubMock> stub = new CallManagerServiceStubMock();
    MessageParcel data;
    MessageParcel reply;

    int32_t result = stub->OnIsRinging(data, reply);
    EXPECT_EQ(result, TELEPHONY_SUCCESS);
}

HWTEST_F(CallManagerServiceStubTest, CallManagerServiceStub_OnIsInEmergencyCall_0100, TestSize.Level1)
{
    sptr<CallManagerServiceStubMock> stub = new CallManagerServiceStubMock();
    MessageParcel data;
    MessageParcel reply;

    int32_t result = stub->OnIsInEmergencyCall(data, reply);
    EXPECT_EQ(result, TELEPHONY_SUCCESS);
}

HWTEST_F(CallManagerServiceStubTest, CallManagerServiceStub_OnStartDtmf_0100, TestSize.Level1)
{
    sptr<CallManagerServiceStubMock> stub = new CallManagerServiceStubMock();
    MessageParcel data;
    MessageParcel reply;

    data.WriteInt32(VALID_CALL_ID);
    data.WriteInt8('1');

    int32_t result = stub->OnStartDtmf(data, reply);
    EXPECT_EQ(result, TELEPHONY_SUCCESS);
}

HWTEST_F(CallManagerServiceStubTest, CallManagerServiceStub_OnStopDtmf_0100, TestSize.Level1)
{
    sptr<CallManagerServiceStubMock> stub = new CallManagerServiceStubMock();
    MessageParcel data;
    MessageParcel reply;

    data.WriteInt32(VALID_CALL_ID);

    int32_t result = stub->OnStopDtmf(data, reply);
    EXPECT_EQ(result, TELEPHONY_SUCCESS);
}

HWTEST_F(CallManagerServiceStubTest, CallManagerServiceStub_OnGetCallWaiting_0100, TestSize.Level1)
{
    sptr<CallManagerServiceStubMock> stub = new CallManagerServiceStubMock();
    MessageParcel data;
    MessageParcel reply;

    data.WriteInt32(VALID_SLOT_ID);

    int32_t result = stub->OnGetCallWaiting(data, reply);
    EXPECT_EQ(result, TELEPHONY_SUCCESS);
}

HWTEST_F(CallManagerServiceStubTest, CallManagerServiceStub_OnSetCallWaiting_0100, TestSize.Level1)
{
    sptr<CallManagerServiceStubMock> stub = new CallManagerServiceStubMock();
    MessageParcel data;
    MessageParcel reply;

    data.WriteInt32(VALID_SLOT_ID);
    data.WriteBool(true);

    int32_t result = stub->OnSetCallWaiting(data, reply);
    EXPECT_EQ(result, TELEPHONY_SUCCESS);
}

HWTEST_F(CallManagerServiceStubTest, CallManagerServiceStub_OnCombineConference_0100, TestSize.Level1)
{
    sptr<CallManagerServiceStubMock> stub = new CallManagerServiceStubMock();
    MessageParcel data;
    MessageParcel reply;

    data.WriteInt32(VALID_CALL_ID);

    int32_t result = stub->OnCombineConference(data, reply);
    EXPECT_EQ(result, TELEPHONY_SUCCESS);
}

HWTEST_F(CallManagerServiceStubTest, CallManagerServiceStub_OnSeparateConference_0100, TestSize.Level1)
{
    sptr<CallManagerServiceStubMock> stub = new CallManagerServiceStubMock();
    MessageParcel data;
    MessageParcel reply;

    data.WriteInt32(VALID_CALL_ID);

    int32_t result = stub->OnSeparateConference(data, reply);
    EXPECT_EQ(result, TELEPHONY_SUCCESS);
}

HWTEST_F(CallManagerServiceStubTest, CallManagerServiceStub_OnKickOutFromConference_0100, TestSize.Level1)
{
    sptr<CallManagerServiceStubMock> stub = new CallManagerServiceStubMock();
    MessageParcel data;
    MessageParcel reply;

    data.WriteInt32(VALID_CALL_ID);

    int32_t result = stub->OnKickOutFromConference(data, reply);
    EXPECT_EQ(result, TELEPHONY_SUCCESS);
}

HWTEST_F(CallManagerServiceStubTest, CallManagerServiceStub_OnJoinConference_0100, TestSize.Level1)
{
    sptr<CallManagerServiceStubMock> stub = new CallManagerServiceStubMock();
    MessageParcel data;
    MessageParcel reply;

    data.WriteInt32(VALID_CALL_ID);
    std::vector<std::u16string> numberList;
    numberList.push_back(u"10086");
    data.WriteString16Vector(numberList);

    int32_t result = stub->OnJoinConference(data, reply);
    EXPECT_NE(result, TELEPHONY_SUCCESS);
}

HWTEST_F(CallManagerServiceStubTest, CallManagerServiceStub_OnJoinConference_0200, TestSize.Level1)
{
    sptr<CallManagerServiceStubMock> stub = new CallManagerServiceStubMock();
    MessageParcel data;
    MessageParcel reply;

    data.WriteInt32(VALID_CALL_ID);

    int32_t result = stub->OnJoinConference(data, reply);
    EXPECT_EQ(result, TELEPHONY_ERR_WRITE_DATA_FAIL);
}

HWTEST_F(CallManagerServiceStubTest, CallManagerServiceStub_OnIsEmergencyPhoneNumber_0100, TestSize.Level1)
{
    sptr<CallManagerServiceStubMock> stub = new CallManagerServiceStubMock();
    MessageParcel data;
    MessageParcel reply;

    data.WriteString16(u"110");
    data.WriteInt32(VALID_SLOT_ID);

    int32_t result = stub->OnIsEmergencyPhoneNumber(data, reply);
    EXPECT_EQ(result, TELEPHONY_SUCCESS);
}

HWTEST_F(CallManagerServiceStubTest, CallManagerServiceStub_OnFormatPhoneNumber_0100, TestSize.Level1)
{
    sptr<CallManagerServiceStubMock> stub = new CallManagerServiceStubMock();
    MessageParcel data;
    MessageParcel reply;

    data.WriteString16(u"10086");
    data.WriteString16(u"CN");

    int32_t result = stub->OnFormatPhoneNumber(data, reply);
    EXPECT_EQ(result, TELEPHONY_SUCCESS);
}

HWTEST_F(CallManagerServiceStubTest, CallManagerServiceStub_OnFormatPhoneNumberToE164_0100, TestSize.Level1)
{
    sptr<CallManagerServiceStubMock> stub = new CallManagerServiceStubMock();
    MessageParcel data;
    MessageParcel reply;

    data.WriteString16(u"10086");
    data.WriteString16(u"CN");

    int32_t result = stub->OnFormatPhoneNumberToE164(data, reply);
    EXPECT_EQ(result, TELEPHONY_SUCCESS);
}

HWTEST_F(CallManagerServiceStubTest, CallManagerServiceStub_OnGetMainCallId_0100, TestSize.Level1)
{
    sptr<CallManagerServiceStubMock> stub = new CallManagerServiceStubMock();
    MessageParcel data;
    MessageParcel reply;

    data.WriteInt32(VALID_CALL_ID);

    int32_t result = stub->OnGetMainCallId(data, reply);
    EXPECT_EQ(result, TELEPHONY_SUCCESS);
}

HWTEST_F(CallManagerServiceStubTest, CallManagerServiceStub_OnGetSubCallIdList_0100, TestSize.Level1)
{
    sptr<CallManagerServiceStubMock> stub = new CallManagerServiceStubMock();
    MessageParcel data;
    MessageParcel reply;

    data.WriteInt32(VALID_CALL_ID);

    int32_t result = stub->OnGetSubCallIdList(data, reply);
    EXPECT_EQ(result, TELEPHONY_SUCCESS);
}

HWTEST_F(CallManagerServiceStubTest, CallManagerServiceStub_OnGetCallIdListForConference_0100, TestSize.Level1)
{
    sptr<CallManagerServiceStubMock> stub = new CallManagerServiceStubMock();
    MessageParcel data;
    MessageParcel reply;

    data.WriteInt32(VALID_CALL_ID);

    int32_t result = stub->OnGetCallIdListForConference(data, reply);
    EXPECT_EQ(result, TELEPHONY_SUCCESS);
}

HWTEST_F(CallManagerServiceStubTest, CallManagerServiceStub_OnEnableVoLte_0100, TestSize.Level1)
{
    sptr<CallManagerServiceStubMock> stub = new CallManagerServiceStubMock();
    MessageParcel data;
    MessageParcel reply;

    data.WriteInt32(VALID_SLOT_ID);

    int32_t result = stub->OnEnableVoLte(data, reply);
    EXPECT_EQ(result, TELEPHONY_SUCCESS);
}

HWTEST_F(CallManagerServiceStubTest, CallManagerServiceStub_OnDisableVoLte_0100, TestSize.Level1)
{
    sptr<CallManagerServiceStubMock> stub = new CallManagerServiceStubMock();
    MessageParcel data;
    MessageParcel reply;

    data.WriteInt32(VALID_SLOT_ID);

    int32_t result = stub->OnDisableVoLte(data, reply);
    EXPECT_EQ(result, TELEPHONY_SUCCESS);
}

HWTEST_F(CallManagerServiceStubTest, CallManagerServiceStub_OnIsVoLteEnabled_0100, TestSize.Level1)
{
    sptr<CallManagerServiceStubMock> stub = new CallManagerServiceStubMock();
    MessageParcel data;
    MessageParcel reply;

    data.WriteInt32(VALID_SLOT_ID);

    int32_t result = stub->OnIsVoLteEnabled(data, reply);
    EXPECT_EQ(result, TELEPHONY_SUCCESS);
}

HWTEST_F(CallManagerServiceStubTest, CallManagerServiceStub_OnRegisterVoipCallManagerCallback_0100, TestSize.Level1)
{
    sptr<CallManagerServiceStubMock> stub = new CallManagerServiceStubMock();
    MessageParcel data;
    MessageParcel reply;

    int32_t result = stub->OnRegisterVoipCallManagerCallback(data, reply);
    EXPECT_EQ(result, TELEPHONY_SUCCESS);
}

HWTEST_F(CallManagerServiceStubTest, CallManagerServiceStub_OnUnRegisterVoipCallManagerCallback_0100, TestSize.Level1)
{
    sptr<CallManagerServiceStubMock> stub = new CallManagerServiceStubMock();
    MessageParcel data;
    MessageParcel reply;

    int32_t result = stub->OnUnRegisterVoipCallManagerCallback(data, reply);
    EXPECT_EQ(result, TELEPHONY_SUCCESS);
}

HWTEST_F(CallManagerServiceStubTest, CallManagerServiceStub_OnEndCall_0100, TestSize.Level1)
{
    sptr<CallManagerServiceStubMock> stub = new CallManagerServiceStubMock();
    MessageParcel data;
    MessageParcel reply;

    int32_t result = stub->OnEndCall(data, reply);
    EXPECT_EQ(result, TELEPHONY_SUCCESS);
}

HWTEST_F(CallManagerServiceStubTest, CallManagerServiceStub_OnHasDistributedCommunicationCapability_0100,
        TestSize.Level1)
{
    sptr<CallManagerServiceStubMock> stub = new CallManagerServiceStubMock();
    MessageParcel data;
    MessageParcel reply;

    int32_t result = stub->OnHasDistributedCommunicationCapability(data, reply);
    EXPECT_EQ(result, TELEPHONY_SUCCESS);
}

HWTEST_F(CallManagerServiceStubTest, CallManagerServiceStub_SetTimer_0100, TestSize.Level1)
{
    sptr<CallManagerServiceStubMock> stub = new CallManagerServiceStubMock();

    int32_t result = stub->SetTimer(
        static_cast<uint32_t>(CallManagerInterfaceCode::INTERFACE_REGISTER_CALLBACK));
    EXPECT_NE(result, -1);
}

HWTEST_F(CallManagerServiceStubTest, CallManagerServiceStub_SetTimer_0200, TestSize.Level1)
{
    sptr<CallManagerServiceStubMock> stub = new CallManagerServiceStubMock();

    int32_t result = stub->SetTimer(INVALID_CODE);
    EXPECT_EQ(result, -1);
}

HWTEST_F(CallManagerServiceStubTest, CallManagerServiceStub_CancelTimer_0100, TestSize.Level1)
{
    sptr<CallManagerServiceStubMock> stub = new CallManagerServiceStubMock();

    stub->CancelTimer(-1);
}

HWTEST_F(CallManagerServiceStubTest, CallManagerServiceStub_OnReportOttCallDetailsInfo_0100, TestSize.Level1)
{
    sptr<CallManagerServiceStubMock> stub = new CallManagerServiceStubMock();
    MessageParcel data;
    MessageParcel reply;

    data.WriteInt32(0);

    int32_t result = stub->OnReportOttCallDetailsInfo(data, reply);
    EXPECT_EQ(result, TELEPHONY_ERR_ARGUMENT_INVALID);
}

HWTEST_F(CallManagerServiceStubTest, CallManagerServiceStub_OnReportOttCallDetailsInfo_0200, TestSize.Level1)
{
    sptr<CallManagerServiceStubMock> stub = new CallManagerServiceStubMock();
    MessageParcel data;
    MessageParcel reply;

    data.WriteInt32(6);

    int32_t result = stub->OnReportOttCallDetailsInfo(data, reply);
    EXPECT_EQ(result, TELEPHONY_ERR_ARGUMENT_INVALID);
}

HWTEST_F(CallManagerServiceStubTest, CallManagerServiceStub_OnReportOttCallDetailsInfo_0300, TestSize.Level1)
{
    sptr<CallManagerServiceStubMock> stub = new CallManagerServiceStubMock();
    MessageParcel data;
    MessageParcel reply;

    data.WriteInt32(1);
    OttCallDetailsInfo info;
    memset(&info, 0, sizeof(info));
    data.WriteRawData(&info, sizeof(info));

    int32_t result = stub->OnReportOttCallDetailsInfo(data, reply);
    EXPECT_EQ(result, TELEPHONY_SUCCESS);
}

HWTEST_F(CallManagerServiceStubTest, CallManagerServiceStub_OnReportOttCallEventInfo_0100, TestSize.Level1)
{
    sptr<CallManagerServiceStubMock> stub = new CallManagerServiceStubMock();
    MessageParcel data;
    MessageParcel reply;

    OttCallEventInfo info;
    memset(&info, 0, sizeof(info));
    data.WriteRawData(&info, sizeof(info));

    int32_t result = stub->OnReportOttCallEventInfo(data, reply);
    EXPECT_EQ(result, TELEPHONY_SUCCESS);
}

HWTEST_F(CallManagerServiceStubTest, CallManagerServiceStub_OnReportOttCallEventInfo_0200, TestSize.Level1)
{
    sptr<CallManagerServiceStubMock> stub = new CallManagerServiceStubMock();
    MessageParcel data;
    MessageParcel reply;

    int32_t result = stub->OnReportOttCallEventInfo(data, reply);
    EXPECT_EQ(result, TELEPHONY_ERR_FAIL);
}

HWTEST_F(CallManagerServiceStubTest, CallManagerServiceStub_OnRegisterBluetoothCallManagerCallbackPtr_0100,
        TestSize.Level1)
{
    sptr<CallManagerServiceStubMock> stub = new CallManagerServiceStubMock();
    MessageParcel data;
    MessageParcel reply;

    data.WriteString("test_mac");

    int32_t result = stub->OnRegisterBluetoothCallManagerCallbackPtr(data, reply);
    EXPECT_EQ(result, TELEPHONY_ERR_FAIL);
}

HWTEST_F(CallManagerServiceStubTest, CallManagerServiceStub_OnAcceptCallNoParam_0100, TestSize.Level1)
{
    sptr<CallManagerServiceStubMock> stub = new CallManagerServiceStubMock();
    MessageParcel data;
    MessageParcel reply;

    int32_t result = stub->OnAcceptCallNoParam(data, reply);
    EXPECT_EQ(result, TELEPHONY_SUCCESS);
}

HWTEST_F(CallManagerServiceStubTest, CallManagerServiceStub_OnRejectCallNoParam_0100, TestSize.Level1)
{
    sptr<CallManagerServiceStubMock> stub = new CallManagerServiceStubMock();
    MessageParcel data;
    MessageParcel reply;

    int32_t result = stub->OnRejectCallNoParam(data, reply);
    EXPECT_EQ(result, TELEPHONY_SUCCESS);
}

HWTEST_F(CallManagerServiceStubTest, CallManagerServiceStub_OnHangUpCallNoParam_0100, TestSize.Level1)
{
    sptr<CallManagerServiceStubMock> stub = new CallManagerServiceStubMock();
    MessageParcel data;
    MessageParcel reply;

    int32_t result = stub->OnHangUpCallNoParam(data, reply);
    EXPECT_EQ(result, TELEPHONY_SUCCESS);
}

HWTEST_F(CallManagerServiceStubTest, CallManagerServiceStub_OnSetCallAudioMode_0100, TestSize.Level1)
{
    sptr<CallManagerServiceStubMock> stub = new CallManagerServiceStubMock();
    MessageParcel data;
    MessageParcel reply;

    data.WriteInt32(0);
    data.WriteInt32(0);

    int32_t result = stub->OnSetCallAudioMode(data, reply);
    EXPECT_EQ(result, TELEPHONY_SUCCESS);
}

HWTEST_F(CallManagerServiceStubTest, CallManagerServiceStub_OnNotifyVoIPAudioStreamStart_0100, TestSize.Level1)
{
    sptr<CallManagerServiceStubMock> stub = new CallManagerServiceStubMock();
    MessageParcel data;
    MessageParcel reply;

    data.WriteInt32(1000);

    int32_t result = stub->OnNotifyVoIPAudioStreamStart(data, reply);
    EXPECT_EQ(result, TELEPHONY_SUCCESS);
}

HWTEST_F(CallManagerServiceStubTest, CallManagerServiceStub_OnGetTransferNumberByNumber_0100, TestSize.Level1)
{
    sptr<CallManagerServiceStubMock> stub = new CallManagerServiceStubMock();
    MessageParcel data;
    MessageParcel reply;

    data.WriteString("10086");
    data.WriteInt32(0);

    int32_t result = stub->OnGetTransferNumberByNumber(data, reply);
    EXPECT_EQ(result, TELEPHONY_SUCCESS);
}

HWTEST_F(CallManagerServiceStubTest, CallManagerServiceStub_OnSendUssdResponse_0100, TestSize.Level1)
{
    sptr<CallManagerServiceStubMock> stub = new CallManagerServiceStubMock();
    MessageParcel data;
    MessageParcel reply;

    data.WriteInt32(VALID_SLOT_ID);
    data.WriteString("test");

    int32_t result = stub->OnSendUssdResponse(data, reply);
    EXPECT_EQ(result, TELEPHONY_SUCCESS);
}

HWTEST_F(CallManagerServiceStubTest, CallManagerServiceStub_OnSetCallPolicyInfo_0100, TestSize.Level1)
{
    sptr<CallManagerServiceStubMock> stub = new CallManagerServiceStubMock();
    MessageParcel data;
    MessageParcel reply;

    data.WriteBool(true);
    std::vector<std::string> dialingList;
    dialingList.push_back("10086");
    data.WriteStringVector(dialingList);
    data.WriteBool(true);
    std::vector<std::string> incomingList;
    incomingList.push_back("10086");
    data.WriteStringVector(incomingList);

    int32_t result = stub->OnSetCallPolicyInfo(data, reply);
    EXPECT_EQ(result, TELEPHONY_SUCCESS);
}

HWTEST_F(CallManagerServiceStubTest, CallManagerServiceStub_OnSendCallUiEvent_0100, TestSize.Level1)
{
    sptr<CallManagerServiceStubMock> stub = new CallManagerServiceStubMock();
    MessageParcel data;
    MessageParcel reply;

    data.WriteInt32(VALID_CALL_ID);
    data.WriteString("test_event");

    int32_t result = stub->OnSendCallUiEvent(data, reply);
    EXPECT_EQ(result, TELEPHONY_SUCCESS);
}

HWTEST_F(CallManagerServiceStubTest, CallManagerServiceStub_OnPreloadCallUi_0100, TestSize.Level1)
{
    sptr<CallManagerServiceStubMock> stub = new CallManagerServiceStubMock();
    MessageParcel data;
    MessageParcel reply;

    data.WriteBool(true);

    int32_t result = stub->OnPreloadCallUi(data, reply);
    EXPECT_EQ(result, TELEPHONY_SUCCESS);
}

HWTEST_F(CallManagerServiceStubTest, CallManagerServiceStub_OnCancelCallUpgrade_0100, TestSize.Level1)
{
    sptr<CallManagerServiceStubMock> stub = new CallManagerServiceStubMock();
    MessageParcel data;
    MessageParcel reply;

    data.WriteInt32(VALID_CALL_ID);

    int32_t result = stub->OnCancelCallUpgrade(data, reply);
    EXPECT_EQ(result, TELEPHONY_SUCCESS);
}

HWTEST_F(CallManagerServiceStubTest, CallManagerServiceStub_OnRequestCameraCapabilities_0100, TestSize.Level1)
{
    sptr<CallManagerServiceStubMock> stub = new CallManagerServiceStubMock();
    MessageParcel data;
    MessageParcel reply;

    data.WriteInt32(VALID_CALL_ID);

    int32_t result = stub->OnRequestCameraCapabilities(data, reply);
    EXPECT_EQ(result, TELEPHONY_SUCCESS);
}

HWTEST_F(CallManagerServiceStubTest, CallManagerServiceStub_OnRemoveMissedIncomingCallNotification_0100,
        TestSize.Level1)
{
    sptr<CallManagerServiceStubMock> stub = new CallManagerServiceStubMock();
    MessageParcel data;
    MessageParcel reply;

    int32_t result = stub->OnRemoveMissedIncomingCallNotification(data, reply);
    EXPECT_EQ(result, TELEPHONY_SUCCESS);
}

HWTEST_F(CallManagerServiceStubTest, CallManagerServiceStub_OnInputDialerSpecialCode_0100, TestSize.Level1)
{
    sptr<CallManagerServiceStubMock> stub = new CallManagerServiceStubMock();
    MessageParcel data;
    MessageParcel reply;

    data.WriteString("*#06#");

    int32_t result = stub->OnInputDialerSpecialCode(data, reply);
    EXPECT_EQ(result, TELEPHONY_SUCCESS);
}

HWTEST_F(CallManagerServiceStubTest, CallManagerServiceStub_OnCloseUnFinishedUssd_0100, TestSize.Level1)
{
    sptr<CallManagerServiceStubMock> stub = new CallManagerServiceStubMock();
    MessageParcel data;
    MessageParcel reply;

    data.WriteInt32(VALID_SLOT_ID);

    int32_t result = stub->OnCloseUnFinishedUssd(data, reply);
    EXPECT_EQ(result, TELEPHONY_SUCCESS);
}

HWTEST_F(CallManagerServiceStubTest, CallManagerServiceStub_OnGetProxyObjectPtr_0100, TestSize.Level1)
{
    sptr<CallManagerServiceStubMock> stub = new CallManagerServiceStubMock();
    MessageParcel data;
    MessageParcel reply;

    data.WriteInt32(0);

    int32_t result = stub->OnGetProxyObjectPtr(data, reply);
    EXPECT_NE(result, TELEPHONY_SUCCESS);
}

HWTEST_F(CallManagerServiceStubTest, CallManagerServiceStub_OnReportAudioDeviceInfo_0100, TestSize.Level1)
{
    sptr<CallManagerServiceStubMock> stub = new CallManagerServiceStubMock();
    MessageParcel data;
    MessageParcel reply;

    int32_t result = stub->OnReportAudioDeviceInfo(data, reply);
    EXPECT_EQ(result, TELEPHONY_SUCCESS);
}

HWTEST_F(CallManagerServiceStubTest, CallManagerServiceStub_OnSetVoIPCallState_0100, TestSize.Level1)
{
    sptr<CallManagerServiceStubMock> stub = new CallManagerServiceStubMock();
    MessageParcel data;
    MessageParcel reply;

    data.WriteInt32(0);

    int32_t result = stub->OnSetVoIPCallState(data, reply);
    EXPECT_EQ(result, TELEPHONY_SUCCESS);
}

HWTEST_F(CallManagerServiceStubTest, CallManagerServiceStub_OnGetVoIPCallState_0100, TestSize.Level1)
{
    sptr<CallManagerServiceStubMock> stub = new CallManagerServiceStubMock();
    MessageParcel data;
    MessageParcel reply;

    int32_t result = stub->OnGetVoIPCallState(data, reply);
    EXPECT_EQ(result, TELEPHONY_SUCCESS);
}

HWTEST_F(CallManagerServiceStubTest, CallManagerServiceStub_OnSetVoIPCallInfo_0100, TestSize.Level1)
{
    sptr<CallManagerServiceStubMock> stub = new CallManagerServiceStubMock();
    MessageParcel data;
    MessageParcel reply;

    data.WriteInt32(VALID_CALL_ID);
    data.WriteInt32(0);
    data.WriteString("10086");

    int32_t result = stub->OnSetVoIPCallInfo(data, reply);
    EXPECT_EQ(result, TELEPHONY_SUCCESS);
}

HWTEST_F(CallManagerServiceStubTest, CallManagerServiceStub_OnGetVoIPCallInfo_0100, TestSize.Level1)
{
    sptr<CallManagerServiceStubMock> stub = new CallManagerServiceStubMock();
    MessageParcel data;
    MessageParcel reply;

    int32_t result = stub->OnGetVoIPCallInfo(data, reply);
    EXPECT_EQ(result, TELEPHONY_SUCCESS);
}

HWTEST_F(CallManagerServiceStubTest, CallManagerServiceStub_OnSetCallPreferenceMode_0100, TestSize.Level1)
{
    sptr<CallManagerServiceStubMock> stub = new CallManagerServiceStubMock();
    MessageParcel data;
    MessageParcel reply;

    data.WriteInt32(VALID_SLOT_ID);
    data.WriteInt32(0);

    int32_t result = stub->OnSetCallPreferenceMode(data, reply);
    EXPECT_EQ(result, TELEPHONY_SUCCESS);
}

HWTEST_F(CallManagerServiceStubTest, CallManagerServiceStub_OnControlCamera_0100, TestSize.Level1)
{
    sptr<CallManagerServiceStubMock> stub = new CallManagerServiceStubMock();
    MessageParcel data;
    MessageParcel reply;

    data.WriteInt32(VALID_CALL_ID);
    data.WriteString16(u"camera0");

    int32_t result = stub->OnControlCamera(data, reply);
    EXPECT_EQ(result, TELEPHONY_SUCCESS);
}

HWTEST_F(CallManagerServiceStubTest, CallManagerServiceStub_OnSetPreviewWindow_0100, TestSize.Level1)
{
    sptr<CallManagerServiceStubMock> stub = new CallManagerServiceStubMock();
    MessageParcel data;
    MessageParcel reply;

    data.WriteInt32(VALID_CALL_ID);
    data.WriteString("surface0");
    data.WriteRemoteObject(nullptr);

    int32_t result = stub->OnSetPreviewWindow(data, reply);
    EXPECT_EQ(result, TELEPHONY_SUCCESS);
}

HWTEST_F(CallManagerServiceStubTest, CallManagerServiceStub_OnSetDisplayWindow_0100, TestSize.Level1)
{
    sptr<CallManagerServiceStubMock> stub = new CallManagerServiceStubMock();
    MessageParcel data;
    MessageParcel reply;

    data.WriteInt32(VALID_CALL_ID);
    data.WriteString("surface0");
    data.WriteRemoteObject(nullptr);

    int32_t result = stub->OnSetDisplayWindow(data, reply);
    EXPECT_EQ(result, TELEPHONY_SUCCESS);
}

HWTEST_F(CallManagerServiceStubTest, CallManagerServiceStub_OnSetCameraZoom_0100, TestSize.Level1)
{
    sptr<CallManagerServiceStubMock> stub = new CallManagerServiceStubMock();
    MessageParcel data;
    MessageParcel reply;

    data.WriteFloat(1.0f);

    int32_t result = stub->OnSetCameraZoom(data, reply);
    EXPECT_EQ(result, TELEPHONY_SUCCESS);
}

HWTEST_F(CallManagerServiceStubTest, CallManagerServiceStub_OnSetPausePicture_0100, TestSize.Level1)
{
    sptr<CallManagerServiceStubMock> stub = new CallManagerServiceStubMock();
    MessageParcel data;
    MessageParcel reply;

    data.WriteInt32(VALID_CALL_ID);
    data.WriteString16(u"/path/to/picture");

    int32_t result = stub->OnSetPausePicture(data, reply);
    EXPECT_EQ(result, TELEPHONY_SUCCESS);
}

HWTEST_F(CallManagerServiceStubTest, CallManagerServiceStub_OnSetDeviceDirection_0100, TestSize.Level1)
{
    sptr<CallManagerServiceStubMock> stub = new CallManagerServiceStubMock();
    MessageParcel data;
    MessageParcel reply;

    data.WriteInt32(VALID_CALL_ID);
    data.WriteInt32(0);

    int32_t result = stub->OnSetDeviceDirection(data, reply);
    EXPECT_EQ(result, TELEPHONY_SUCCESS);
}

HWTEST_F(CallManagerServiceStubTest, CallManagerServiceStub_OnGetImsConfig_0100, TestSize.Level1)
{
    sptr<CallManagerServiceStubMock> stub = new CallManagerServiceStubMock();
    MessageParcel data;
    MessageParcel reply;

    data.WriteInt32(VALID_SLOT_ID);
    data.WriteInt32(0);

    int32_t result = stub->OnGetImsConfig(data, reply);
    EXPECT_EQ(result, TELEPHONY_SUCCESS);
}

HWTEST_F(CallManagerServiceStubTest, CallManagerServiceStub_OnSetImsConfig_0100, TestSize.Level1)
{
    sptr<CallManagerServiceStubMock> stub = new CallManagerServiceStubMock();
    MessageParcel data;
    MessageParcel reply;

    data.WriteInt32(VALID_SLOT_ID);
    data.WriteInt32(0);
    data.WriteString16(u"value");

    int32_t result = stub->OnSetImsConfig(data, reply);
    EXPECT_EQ(result, TELEPHONY_SUCCESS);
}

HWTEST_F(CallManagerServiceStubTest, CallManagerServiceStub_OnGetImsFeatureValue_0100, TestSize.Level1)
{
    sptr<CallManagerServiceStubMock> stub = new CallManagerServiceStubMock();
    MessageParcel data;
    MessageParcel reply;

    data.WriteInt32(VALID_SLOT_ID);
    data.WriteInt32(0);

    int32_t result = stub->OnGetImsFeatureValue(data, reply);
    EXPECT_EQ(result, TELEPHONY_SUCCESS);
}

HWTEST_F(CallManagerServiceStubTest, CallManagerServiceStub_OnSetImsFeatureValue_0100, TestSize.Level1)
{
    sptr<CallManagerServiceStubMock> stub = new CallManagerServiceStubMock();
    MessageParcel data;
    MessageParcel reply;

    data.WriteInt32(VALID_SLOT_ID);
    data.WriteInt32(0);
    data.WriteInt32(0);

    int32_t result = stub->OnSetImsFeatureValue(data, reply);
    EXPECT_EQ(result, TELEPHONY_SUCCESS);
}

HWTEST_F(CallManagerServiceStubTest, CallManagerServiceStub_OnUpdateCallMediaMode_0100, TestSize.Level1)
{
    sptr<CallManagerServiceStubMock> stub = new CallManagerServiceStubMock();
    MessageParcel data;
    MessageParcel reply;

    data.WriteInt32(VALID_CALL_ID);
    data.WriteUint32(0);

    int32_t result = stub->OnUpdateCallMediaMode(data, reply);
    EXPECT_EQ(result, TELEPHONY_SUCCESS);
}

HWTEST_F(CallManagerServiceStubTest, CallManagerServiceStub_OnSetVoNRState_0100, TestSize.Level1)
{
    sptr<CallManagerServiceStubMock> stub = new CallManagerServiceStubMock();
    MessageParcel data;
    MessageParcel reply;

    data.WriteInt32(VALID_SLOT_ID);
    data.WriteInt32(0);

    int32_t result = stub->OnSetVoNRState(data, reply);
    EXPECT_EQ(result, TELEPHONY_SUCCESS);
}

HWTEST_F(CallManagerServiceStubTest, CallManagerServiceStub_OnGetVoNRState_0100, TestSize.Level1)
{
    sptr<CallManagerServiceStubMock> stub = new CallManagerServiceStubMock();
    MessageParcel data;
    MessageParcel reply;

    data.WriteInt32(VALID_SLOT_ID);

    int32_t result = stub->OnGetVoNRState(data, reply);
    EXPECT_EQ(result, TELEPHONY_SUCCESS);
}

HWTEST_F(CallManagerServiceStubTest, CallManagerServiceStub_OnGetTransferNumber_0100, TestSize.Level1)
{
    sptr<CallManagerServiceStubMock> stub = new CallManagerServiceStubMock();
    MessageParcel data;
    MessageParcel reply;

    data.WriteInt32(VALID_SLOT_ID);
    data.WriteInt32(0);

    int32_t result = stub->OnGetTransferNumber(data, reply);
    EXPECT_EQ(result, TELEPHONY_SUCCESS);
}

HWTEST_F(CallManagerServiceStubTest, CallManagerServiceStub_OnSetTransferNumber_0100, TestSize.Level1)
{
    sptr<CallManagerServiceStubMock> stub = new CallManagerServiceStubMock();
    MessageParcel data;
    MessageParcel reply;

    data.WriteInt32(VALID_SLOT_ID);
    CallTransferInfo info;
    data.WriteRawData(&info, sizeof(info));

    int32_t result = stub->OnSetTransferNumber(data, reply);
    EXPECT_EQ(result, TELEPHONY_SUCCESS);
}

HWTEST_F(CallManagerServiceStubTest, CallManagerServiceStub_OnSetTransferNumber_0200, TestSize.Level1)
{
    sptr<CallManagerServiceStubMock> stub = new CallManagerServiceStubMock();
    MessageParcel data;
    MessageParcel reply;

    data.WriteInt32(VALID_SLOT_ID);

    int32_t result = stub->OnSetTransferNumber(data, reply);
    EXPECT_EQ(result, TELEPHONY_ERR_LOCAL_PTR_NULL);
}

HWTEST_F(CallManagerServiceStubTest, CallManagerServiceStub_OnCanSetCallTransferTime_0100, TestSize.Level1)
{
    sptr<CallManagerServiceStubMock> stub = new CallManagerServiceStubMock();
    MessageParcel data;
    MessageParcel reply;

    data.WriteInt32(VALID_SLOT_ID);
    data.WriteBool(true);

    int32_t result = stub->OnCanSetCallTransferTime(data, reply);
    EXPECT_EQ(result, TELEPHONY_SUCCESS);
}

HWTEST_F(CallManagerServiceStubTest, CallManagerServiceStub_OnGetCallRestriction_0100, TestSize.Level1)
{
    sptr<CallManagerServiceStubMock> stub = new CallManagerServiceStubMock();
    MessageParcel data;
    MessageParcel reply;

    data.WriteInt32(VALID_SLOT_ID);
    data.WriteInt32(0);

    int32_t result = stub->OnGetCallRestriction(data, reply);
    EXPECT_EQ(result, TELEPHONY_SUCCESS);
}

HWTEST_F(CallManagerServiceStubTest, CallManagerServiceStub_OnSetCallRestriction_0100, TestSize.Level1)
{
    sptr<CallManagerServiceStubMock> stub = new CallManagerServiceStubMock();
    MessageParcel data;
    MessageParcel reply;

    data.WriteInt32(VALID_SLOT_ID);
    CallRestrictionInfo info;
    data.WriteRawData(&info, sizeof(info));

    int32_t result = stub->OnSetCallRestriction(data, reply);
    EXPECT_EQ(result, TELEPHONY_SUCCESS);
}

HWTEST_F(CallManagerServiceStubTest, CallManagerServiceStub_OnSetCallRestriction_0200, TestSize.Level1)
{
    sptr<CallManagerServiceStubMock> stub = new CallManagerServiceStubMock();
    MessageParcel data;
    MessageParcel reply;

    data.WriteInt32(VALID_SLOT_ID);

    int32_t result = stub->OnSetCallRestriction(data, reply);
    EXPECT_EQ(result, TELEPHONY_ERR_LOCAL_PTR_NULL);
}

HWTEST_F(CallManagerServiceStubTest, CallManagerServiceStub_OnSetCallRestrictionPassword_0100, TestSize.Level1)
{
    sptr<CallManagerServiceStubMock> stub = new CallManagerServiceStubMock();
    MessageParcel data;
    MessageParcel reply;

    data.WriteInt32(VALID_SLOT_ID);
    data.WriteInt32(0);
    data.WriteCString("old_password");
    data.WriteCString("new_password");

    int32_t result = stub->OnSetCallRestrictionPassword(data, reply);
    EXPECT_EQ(result, TELEPHONY_SUCCESS);
}

HWTEST_F(CallManagerServiceStubTest, CallManagerServiceStub_OnPostDialProceed_0100, TestSize.Level1)
{
    sptr<CallManagerServiceStubMock> stub = new CallManagerServiceStubMock();
    MessageParcel data;
    MessageParcel reply;

    data.WriteInt32(VALID_CALL_ID);
    data.WriteBool(true);

    int32_t result = stub->OnPostDialProceed(data, reply);
    EXPECT_EQ(result, TELEPHONY_SUCCESS);
}

HWTEST_F(CallManagerServiceStubTest, CallManagerServiceStub_OnObserverOnCallDetailsChange_0100, TestSize.Level1)
{
    sptr<CallManagerServiceStubMock> stub = new CallManagerServiceStubMock();
    MessageParcel data;
    MessageParcel reply;

    int32_t result = stub->OnObserverOnCallDetailsChange(data, reply);
    EXPECT_EQ(result, TELEPHONY_SUCCESS);
}

HWTEST_F(CallManagerServiceStubTest, CallManagerServiceStub_OnUnRegisterCallBack_0100, TestSize.Level1)
{
    sptr<CallManagerServiceStubMock> stub = new CallManagerServiceStubMock();
    MessageParcel data;
    MessageParcel reply;

    int32_t result = stub->OnUnRegisterCallBack(data, reply);
    EXPECT_EQ(result, TELEPHONY_SUCCESS);
}
} // namespace Telephony
} // namespace OHOS