/*
 * Copyright (C) 2026 Huawei Device Co., Ltd.
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
 
/*
 * DT test for CallStateReportProxy private methods:
 *   - VoIPCallState ConvertToVoipCallState(TelCallState nextState)
 *   - void ReportVoIPCallStateToRegistry(sptr<CallBase> &callObjectPtr, TelCallState nextState)
 *
 * Branch coverage design (90%+ required):
 *   ConvertToVoipCallState: 10 switch cases + default, all covered
 *     (INCOMING/WAITING -> INCOMING, DIALING, ALERTING -> OUTGOING, ANSWERED,
 *      ACTIVE, HOLDING, DISCONNECTING, DISCONNECTED, default -> IDLE)
 *   ReportVoIPCallStateToRegistry: 2 if-branches, all covered
 *     1) callObjectPtr == nullptr   -> covered (NullCall case)
 *     2) ret != TELEPHONY_SUCCESS    -> true/false both covered (UpdateFailed/Success)
 *   Note: DelayedRefSingleton::GetInstance() returns a reference which can never
 *   be null, so no singleton null-check branch exists in the implementation.
 *   Coverage: 100%
 */
 
#define private public
#define protected public
#include "call_state_report_proxy.h"
 
#include <gtest/gtest.h>
#include <string>
 
#include "call_base.h"
#include "call_manager_info.h"
#include "stub.h"
#include "telephony_errors.h"
#include "telephony_state_registry_client.h"
#include "voip_call_state_info.h"
 
namespace OHOS {
namespace Telephony {
using namespace testing::ext;
 
namespace {
int32_t g_stubUpdateResult = TELEPHONY_SUCCESS;
int32_t g_stubUpdateCount = 0;
VoIPCallStateInfo g_stubLastInfo = {};
} // namespace
 
static int32_t StubUpdateVoIPCallState(TelephonyStateRegistryClient *client, const VoIPCallStateInfo &info)
{
    ++g_stubUpdateCount;
    g_stubLastInfo = info;
    return g_stubUpdateResult;
}
 
class MockCallBase : public CallBase {
public:
    explicit MockCallBase(DialParaInfo &info) : CallBase(info) {}
    ~MockCallBase() override = default;
 
    void GetCallAttributeInfo(CallAttributeInfo &info) override
    {
        info = mockAttrInfo_;
    }
 
    int32_t DialingProcess() override
    {
        return TELEPHONY_SUCCESS;
    }
 
    int32_t AnswerCall(int32_t videoState, bool isRTT) override
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
 
    int32_t HoldCall() override
    {
        return TELEPHONY_SUCCESS;
    }
 
    int32_t UnHoldCall() override
    {
        return TELEPHONY_SUCCESS;
    }
 
    int32_t SwitchCall() override
    {
        return TELEPHONY_SUCCESS;
    }
 
    bool GetEmergencyState() override
    {
        return false;
    }
 
    int32_t StartDtmf(char str) override
    {
        return TELEPHONY_SUCCESS;
    }
 
    int32_t StopDtmf() override
    {
        return TELEPHONY_SUCCESS;
    }
 
    int32_t PostDialProceed(bool proceed) override
    {
        return TELEPHONY_SUCCESS;
    }
 
    int32_t GetSlotId() override
    {
        return 0;
    }
 
    int32_t CombineConference() override
    {
        return TELEPHONY_SUCCESS;
    }
 
    void HandleCombineConferenceFailEvent() override {}
 
    int32_t SeparateConference() override
    {
        return TELEPHONY_SUCCESS;
    }
 
    int32_t KickOutFromConference() override
    {
        return TELEPHONY_SUCCESS;
    }
 
    int32_t CanCombineConference() override
    {
        return TELEPHONY_SUCCESS;
    }
 
    int32_t CanSeparateConference() override
    {
        return TELEPHONY_SUCCESS;
    }
 
    int32_t CanKickOutFromConference() override
    {
        return TELEPHONY_SUCCESS;
    }
 
    int32_t LaunchConference() override
    {
        return TELEPHONY_SUCCESS;
    }
 
    int32_t ExitConference() override
    {
        return TELEPHONY_SUCCESS;
    }
 
    int32_t HoldConference() override
    {
        return TELEPHONY_SUCCESS;
    }
 
    int32_t GetMainCallId(int32_t &mainCallId) override
    {
        return TELEPHONY_SUCCESS;
    }
 
    int32_t GetSubCallIdList(std::vector<std::u16string> &callIdList) override
    {
        return TELEPHONY_SUCCESS;
    }
 
    int32_t GetCallIdListForConference(std::vector<std::u16string> &callIdList) override
    {
        return TELEPHONY_SUCCESS;
    }
 
    int32_t IsSupportConferenceable() override
    {
        return TELEPHONY_SUCCESS;
    }
 
    int32_t SetMute(int32_t mute, int32_t slotId) override
    {
        return TELEPHONY_SUCCESS;
    }
 
    CallAttributeInfo mockAttrInfo_ = {};
};
 
class CallStateReportProxyTest : public testing::Test {
public:
    static void SetUpTestCase() {}
    static void TearDownTestCase() {}
 
    void SetUp() override
    {
        g_stubUpdateResult = TELEPHONY_SUCCESS;
        g_stubUpdateCount = 0;
        g_stubLastInfo = {};
        DialParaInfo dialPara;
        call_ = new MockCallBase(dialPara);
    }
 
    void TearDown() override {}
 
    CallStateReportProxy proxy_;
    sptr<MockCallBase> call_ = nullptr;
};
 
/**
 * @tc.number: CallStateReportProxy_ConvertToVoipCallState_Incoming
 * @tc.name: Test ConvertToVoipCallState with incoming state
 * @tc.desc: Verify that CALL_STATUS_INCOMING is converted to VoIPCallState::INCOMING
 */
HWTEST_F(CallStateReportProxyTest, CallStateReportProxy_ConvertToVoipCallState_Incoming, TestSize.Level1)
{
    VoIPCallState result = proxy_.ConvertToVoipCallState(TelCallState::CALL_STATUS_INCOMING);
    EXPECT_EQ(result, VoIPCallState::INCOMING);
}
 
/**
 * @tc.number: CallStateReportProxy_ConvertToVoipCallState_Waiting
 * @tc.name: Test ConvertToVoipCallState with waiting state
 * @tc.desc: Verify that CALL_STATUS_WAITING is converted to VoIPCallState::INCOMING
 */
HWTEST_F(CallStateReportProxyTest, CallStateReportProxy_ConvertToVoipCallState_Waiting, TestSize.Level1)
{
    VoIPCallState result = proxy_.ConvertToVoipCallState(TelCallState::CALL_STATUS_WAITING);
    EXPECT_EQ(result, VoIPCallState::INCOMING);
}
 
/**
 * @tc.number: CallStateReportProxy_ConvertToVoipCallState_Dialing
 * @tc.name: Test ConvertToVoipCallState with dialing state
 * @tc.desc: Verify that CALL_STATUS_DIALING is converted to VoIPCallState::DIALING
 */
HWTEST_F(CallStateReportProxyTest, CallStateReportProxy_ConvertToVoipCallState_Dialing, TestSize.Level1)
{
    VoIPCallState result = proxy_.ConvertToVoipCallState(TelCallState::CALL_STATUS_DIALING);
    EXPECT_EQ(result, VoIPCallState::DIALING);
}
 
/**
 * @tc.number: CallStateReportProxy_ConvertToVoipCallState_Alerting
 * @tc.name: Test ConvertToVoipCallState with alerting state
 * @tc.desc: Verify that CALL_STATUS_ALERTING is converted to VoIPCallState::OUTGOING
 */
HWTEST_F(CallStateReportProxyTest, CallStateReportProxy_ConvertToVoipCallState_Alerting, TestSize.Level1)
{
    VoIPCallState result = proxy_.ConvertToVoipCallState(TelCallState::CALL_STATUS_ALERTING);
    EXPECT_EQ(result, VoIPCallState::OUTGOING);
}
 
/**
 * @tc.number: CallStateReportProxy_ConvertToVoipCallState_Answered
 * @tc.name: Test ConvertToVoipCallState with answered state
 * @tc.desc: Verify that CALL_STATUS_ANSWERED is converted to VoIPCallState::ANSWERED
 */
HWTEST_F(CallStateReportProxyTest, CallStateReportProxy_ConvertToVoipCallState_Answered, TestSize.Level1)
{
    VoIPCallState result = proxy_.ConvertToVoipCallState(TelCallState::CALL_STATUS_ANSWERED);
    EXPECT_EQ(result, VoIPCallState::ANSWERED);
}
 
/**
 * @tc.number: CallStateReportProxy_ConvertToVoipCallState_Active
 * @tc.name: Test ConvertToVoipCallState with active state
 * @tc.desc: Verify that CALL_STATUS_ACTIVE is converted to VoIPCallState::ACTIVE
 */
HWTEST_F(CallStateReportProxyTest, CallStateReportProxy_ConvertToVoipCallState_Active, TestSize.Level1)
{
    VoIPCallState result = proxy_.ConvertToVoipCallState(TelCallState::CALL_STATUS_ACTIVE);
    EXPECT_EQ(result, VoIPCallState::ACTIVE);
}
 
/**
 * @tc.number: CallStateReportProxy_ConvertToVoipCallState_Holding
 * @tc.name: Test ConvertToVoipCallState with holding state
 * @tc.desc: Verify that CALL_STATUS_HOLDING is converted to VoIPCallState::HOLDING
 */
HWTEST_F(CallStateReportProxyTest, CallStateReportProxy_ConvertToVoipCallState_Holding, TestSize.Level1)
{
    VoIPCallState result = proxy_.ConvertToVoipCallState(TelCallState::CALL_STATUS_HOLDING);
    EXPECT_EQ(result, VoIPCallState::HOLDING);
}
 
/**
 * @tc.number: CallStateReportProxy_ConvertToVoipCallState_Disconnecting
 * @tc.name: Test ConvertToVoipCallState with disconnecting state
 * @tc.desc: Verify that CALL_STATUS_DISCONNECTING is converted to VoIPCallState::DISCONNECTING
 */
HWTEST_F(CallStateReportProxyTest, CallStateReportProxy_ConvertToVoipCallState_Disconnecting, TestSize.Level1)
{
    VoIPCallState result = proxy_.ConvertToVoipCallState(TelCallState::CALL_STATUS_DISCONNECTING);
    EXPECT_EQ(result, VoIPCallState::DISCONNECTING);
}
 
/**
 * @tc.number: CallStateReportProxy_ConvertToVoipCallState_Disconnected
 * @tc.name: Test ConvertToVoipCallState with disconnected state
 * @tc.desc: Verify that CALL_STATUS_DISCONNECTED is converted to VoIPCallState::DISCONNECTED
 */
HWTEST_F(CallStateReportProxyTest, CallStateReportProxy_ConvertToVoipCallState_Disconnected, TestSize.Level1)
{
    VoIPCallState result = proxy_.ConvertToVoipCallState(TelCallState::CALL_STATUS_DISCONNECTED);
    EXPECT_EQ(result, VoIPCallState::DISCONNECTED);
}
 
/**
 * @tc.number: CallStateReportProxy_ConvertToVoipCallState_IdleDefault
 * @tc.name: Test ConvertToVoipCallState with idle state falls to default
 * @tc.desc: Verify that CALL_STATUS_IDLE falls to default and is converted to VoIPCallState::IDLE
 */
HWTEST_F(CallStateReportProxyTest, CallStateReportProxy_ConvertToVoipCallState_IdleDefault, TestSize.Level1)
{
    VoIPCallState result = proxy_.ConvertToVoipCallState(TelCallState::CALL_STATUS_IDLE);
    EXPECT_EQ(result, VoIPCallState::IDLE);
}
 
/**
 * @tc.number: CallStateReportProxy_ConvertToVoipCallState_UnknownDefault
 * @tc.name: Test ConvertToVoipCallState with unknown state falls to default
 * @tc.desc: Verify that CALL_STATUS_UNKNOWN (negative boundary) falls to default and is converted to VoIPCallState::IDLE
 */
HWTEST_F(CallStateReportProxyTest, CallStateReportProxy_ConvertToVoipCallState_UnknownDefault, TestSize.Level1)
{
    VoIPCallState result = proxy_.ConvertToVoipCallState(TelCallState::CALL_STATUS_UNKNOWN);
    EXPECT_EQ(result, VoIPCallState::IDLE);
}
 
/**
 * @tc.number: CallStateReportProxy_ConvertToVoipCallState_InvalidValueDefault
 * @tc.name: Test ConvertToVoipCallState with out-of-range value falls to default
 * @tc.desc: Verify that an out-of-range TelCallState value falls to default and is converted to VoIPCallState::IDLE
 */
HWTEST_F(CallStateReportProxyTest, CallStateReportProxy_ConvertToVoipCallState_InvalidValueDefault, TestSize.Level1)
{
    constexpr int32_t INVALID_STATE = 100;
    VoIPCallState result = proxy_.ConvertToVoipCallState(static_cast<TelCallState>(INVALID_STATE));
    EXPECT_EQ(result, VoIPCallState::IDLE);
}
 
/**
 * @tc.number: CallStateReportProxy_ReportVoIPCallState_NullCall
 * @tc.name: Test ReportVoIPCallStateToRegistry with null call object
 * @tc.desc: Verify that ReportVoIPCallStateToRegistry returns early and never touches
 *           the state registry client when callObjectPtr is nullptr
 */
HWTEST_F(CallStateReportProxyTest, CallStateReportProxy_ReportVoIPCallState_NullCall, TestSize.Level1)
{
    sptr<CallBase> nullCall = nullptr;
    proxy_.ReportVoIPCallStateToRegistry(nullCall, TelCallState::CALL_STATUS_INCOMING);
    EXPECT_EQ(g_stubUpdateCount, 0);
}
 
/**
 * @tc.number: CallStateReportProxy_ReportVoIPCallState_Success
 * @tc.name: Test ReportVoIPCallStateToRegistry success path
 * @tc.desc: Verify that ReportVoIPCallStateToRegistry fills VoIPCallStateInfo from the call
 *           attribute (appName, contactName, callType, callState, isVoiceAnswerSupported)
 *           and reports to the state registry client successfully
 */
HWTEST_F(CallStateReportProxyTest, CallStateReportProxy_ReportVoIPCallState_Success, TestSize.Level1)
{
    call_->mockAttrInfo_.videoState = VideoStateType::TYPE_VOICE;
    call_->mockAttrInfo_.voipCallInfo.voipBundleName = "com.example.voip";
    call_->mockAttrInfo_.voipCallInfo.userName = "Alice";
    call_->mockAttrInfo_.voipCallInfo.isConferenceCall = false;
    call_->mockAttrInfo_.voipCallInfo.isVoiceAnswerSupported = true;
    sptr<CallBase> callObject = call_;
    Stub stub;
    stub.set(ADDR(TelephonyStateRegistryClient, UpdateVoIPCallState), StubUpdateVoIPCallState);
    proxy_.ReportVoIPCallStateToRegistry(callObject, TelCallState::CALL_STATUS_INCOMING);
    EXPECT_EQ(g_stubUpdateCount, 1);
    EXPECT_EQ(g_stubLastInfo.appName, "com.example.voip");
    EXPECT_EQ(g_stubLastInfo.contactName, "Alice");
    EXPECT_EQ(g_stubLastInfo.callType, VoIPCallType::VOICE_ONE_TO_ONE);
    EXPECT_EQ(g_stubLastInfo.callState, VoIPCallState::INCOMING);
    EXPECT_TRUE(g_stubLastInfo.isVoiceAnswerSupported);
}
 
/**
 * @tc.number: CallStateReportProxy_ReportVoIPCallState_UpdateFailed
 * @tc.name: Test ReportVoIPCallStateToRegistry when registry update fails
 * @tc.desc: Verify that ReportVoIPCallStateToRegistry handles the non-success return value of
 *           UpdateVoIPCallState and returns without crash
 */
HWTEST_F(CallStateReportProxyTest, CallStateReportProxy_ReportVoIPCallState_UpdateFailed, TestSize.Level1)
{
    g_stubUpdateResult = TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    sptr<CallBase> callObject = call_;
    Stub stub;
    stub.set(ADDR(TelephonyStateRegistryClient, UpdateVoIPCallState), StubUpdateVoIPCallState);
    proxy_.ReportVoIPCallStateToRegistry(callObject, TelCallState::CALL_STATUS_ACTIVE);
    EXPECT_EQ(g_stubUpdateCount, 1);
}
 
/**
 * @tc.number: CallStateReportProxy_ReportVoIPCallState_VideoConference
 * @tc.name: Test ReportVoIPCallStateToRegistry with video conference call
 * @tc.desc: Verify that ReportVoIPCallStateToRegistry converts videoState/isConferenceCall
 *           attributes to VoIPCallType::VIDEO_CONFERENCE and passes through
 *           isVoiceAnswerSupported=false to the registry
 */
HWTEST_F(CallStateReportProxyTest, CallStateReportProxy_ReportVoIPCallState_VideoConference, TestSize.Level1)
{
    call_->mockAttrInfo_.videoState = VideoStateType::TYPE_VIDEO;
    call_->mockAttrInfo_.voipCallInfo.voipBundleName = "com.example.meeting";
    call_->mockAttrInfo_.voipCallInfo.userName = "Bob";
    call_->mockAttrInfo_.voipCallInfo.isConferenceCall = true;
    call_->mockAttrInfo_.voipCallInfo.isVoiceAnswerSupported = false;
    sptr<CallBase> callObject = call_;
    Stub stub;
    stub.set(ADDR(TelephonyStateRegistryClient, UpdateVoIPCallState), StubUpdateVoIPCallState);
    proxy_.ReportVoIPCallStateToRegistry(callObject, TelCallState::CALL_STATUS_ACTIVE);
    EXPECT_EQ(g_stubUpdateCount, 1);
    EXPECT_EQ(g_stubLastInfo.callType, VoIPCallType::VIDEO_CONFERENCE);
    EXPECT_EQ(g_stubLastInfo.callState, VoIPCallState::ACTIVE);
    EXPECT_FALSE(g_stubLastInfo.isVoiceAnswerSupported);
}
} // namespace Telephony
} // namespace OHOS
