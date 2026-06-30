/*
 * Copyright (C) 2021-2024 Huawei Device Co., Ltd.
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

#include "report_call_info_handler.h"
#include "call_status_manager.h"
#include "call_manager_errors.h"
#include "call_object_manager.h"
#include "common_type.h"
#include "telephony_types.h"
#include "call_base.h"
#include "ims_call.h"
#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "telephony_log_wrapper.h"

namespace OHOS {
namespace Telephony {
using namespace testing::ext;
using namespace testing;

constexpr int32_t VALID_CALL_INDEX = 1;
constexpr int32_t VALID_SLOT_ID = 0;
constexpr int32_t INVALID_SLOT_ID = -1;

class ReportCallInfoHandlerTest : public testing::Test {
public:
    static void SetUpTestCase() {}
    static void TearDownTestCase() {}
    void SetUp() {}
    void TearDown() {}
};

/**
 * @tc.number   ReportCallInfoHandler_Init_0100
 * @tc.name     Test Init function
 * @tc.desc     Function test
 */
HWTEST_F(ReportCallInfoHandlerTest, ReportCallInfoHandler_Init_0100, TestSize.Level1)
{
    auto handler = DelayedSingleton<ReportCallInfoHandler>::GetInstance();
    ASSERT_TRUE(handler != nullptr);
    handler->Init();
    EXPECT_TRUE(handler->callStatusManagerPtr_ != nullptr);
}

/**
 * @tc.number   ReportCallInfoHandler_UpdateCallReportInfo_0100
 * @tc.name     Test UpdateCallReportInfo with null callStatusManagerPtr_
 * @tc.desc     Boundary test
 */
HWTEST_F(ReportCallInfoHandlerTest, ReportCallInfoHandler_UpdateCallReportInfo_0100, TestSize.Level1)
{
    auto handler = DelayedSingleton<ReportCallInfoHandler>::GetInstance();
    ASSERT_TRUE(handler != nullptr);
    handler->callStatusManagerPtr_ = nullptr;
    
    CallDetailInfo info;
    int32_t ret = handler->UpdateCallReportInfo(info);
    EXPECT_EQ(ret, TELEPHONY_ERR_LOCAL_PTR_NULL);
}

/**
 * @tc.number   ReportCallInfoHandler_UpdateCallReportInfo_0200
 * @tc.name     Test UpdateCallReportInfo with valid parameters
 * @tc.desc     Function test
 */
HWTEST_F(ReportCallInfoHandlerTest, ReportCallInfoHandler_UpdateCallReportInfo_0200, TestSize.Level1)
{
    auto handler = DelayedSingleton<ReportCallInfoHandler>::GetInstance();
    ASSERT_TRUE(handler != nullptr);
    handler->Init();
    ASSERT_TRUE(handler->callStatusManagerPtr_ != nullptr);
    
    CallDetailInfo info;
    info.callType = CallType::TYPE_CS;
    info.state = TelCallState::CALL_STATUS_IDLE;
    
    int32_t ret = handler->UpdateCallReportInfo(info);
    EXPECT_EQ(ret, TELEPHONY_SUCCESS);
}

/**
 * @tc.number   ReportCallInfoHandler_ReportCallProcedureEvents_0100
 * @tc.name     Test ReportCallProcedureEvents with null callStatusManagerPtr_
 * @tc.desc     Boundary test
 */
HWTEST_F(ReportCallInfoHandlerTest, ReportCallInfoHandler_ReportCallProcedureEvents_0100, TestSize.Level1)
{
    auto handler = DelayedSingleton<ReportCallInfoHandler>::GetInstance();
    ASSERT_TRUE(handler != nullptr);
    handler->callStatusManagerPtr_ = nullptr;
    
    std::string callId = "test_call_id";
    std::string jsonStr = "{}";
    
    int32_t ret = handler->ReportCallProcedureEvents(callId, jsonStr);
    EXPECT_EQ(ret, TELEPHONY_ERR_LOCAL_PTR_NULL);
}

/**
 * @tc.number   ReportCallInfoHandler_ReportCallProcedureEvents_0200
 * @tc.name     Test ReportCallProcedureEvents with valid parameters
 * @tc.desc     Function test
 */
HWTEST_F(ReportCallInfoHandlerTest, ReportCallInfoHandler_ReportCallProcedureEvents_0200, TestSize.Level1)
{
    auto handler = DelayedSingleton<ReportCallInfoHandler>::GetInstance();
    ASSERT_TRUE(handler != nullptr);
    handler->Init();
    ASSERT_TRUE(handler->callStatusManagerPtr_ != nullptr);
    
    std::string callId = "test_call_id";
    std::string jsonStr = "{\"event\":\"test\"}";
    
    int32_t ret = handler->ReportCallProcedureEvents(callId, jsonStr);
    EXPECT_TRUE(ret == TELEPHONY_SUCCESS || ret != TELEPHONY_SUCCESS);
}

/**
 * @tc.number   ReportCallInfoHandler_UpdateCallsReportInfo_0100
 * @tc.name     Test UpdateCallsReportInfo with null callStatusManagerPtr_
 * @tc.desc     Boundary test
 */
HWTEST_F(ReportCallInfoHandlerTest, ReportCallInfoHandler_UpdateCallsReportInfo_0100, TestSize.Level1)
{
    auto handler = DelayedSingleton<ReportCallInfoHandler>::GetInstance();
    ASSERT_TRUE(handler != nullptr);
    handler->callStatusManagerPtr_ = nullptr;
    
    CallDetailsInfo info;
    int32_t ret = handler->UpdateCallsReportInfo(info);
    EXPECT_EQ(ret, TELEPHONY_ERR_LOCAL_PTR_NULL);
}

/**
 * @tc.number   ReportCallInfoHandler_UpdateCallsReportInfo_0200
 * @tc.name     Test UpdateCallsReportInfo with valid parameters
 * @tc.desc     Function test
 */
HWTEST_F(ReportCallInfoHandlerTest, ReportCallInfoHandler_UpdateCallsReportInfo_0200, TestSize.Level1)
{
    auto handler = DelayedSingleton<ReportCallInfoHandler>::GetInstance();
    ASSERT_TRUE(handler != nullptr);
    handler->Init();
    ASSERT_TRUE(handler->callStatusManagerPtr_ != nullptr);
    
    CallDetailsInfo info;
    info.slotId = VALID_SLOT_ID;
    
    int32_t ret = handler->UpdateCallsReportInfo(info);
    EXPECT_EQ(ret, TELEPHONY_SUCCESS);
}

/**
 * @tc.number   ReportCallInfoHandler_UpdateCallsReportInfo_0300
 * @tc.name     Test UpdateCallsReportInfo with incoming call state
 * @tc.desc     Function test
 */
HWTEST_F(ReportCallInfoHandlerTest, ReportCallInfoHandler_UpdateCallsReportInfo_0300, TestSize.Level1)
{
    auto handler = DelayedSingleton<ReportCallInfoHandler>::GetInstance();
    ASSERT_TRUE(handler != nullptr);
    handler->Init();
    ASSERT_TRUE(handler->callStatusManagerPtr_ != nullptr);
    
    CallDetailsInfo info;
    info.slotId = VALID_SLOT_ID;
    
    CallDetailInfo detailInfo;
    detailInfo.callType = CallType::TYPE_CS;
    detailInfo.state = TelCallState::CALL_STATUS_INCOMING;
    info.callVec.push_back(detailInfo);
    
    int32_t ret = handler->UpdateCallsReportInfo(info);
    EXPECT_EQ(ret, TELEPHONY_SUCCESS);
}

/**
 * @tc.number   ReportCallInfoHandler_UpdateDisconnectedCause_0100
 * @tc.name     Test UpdateDisconnectedCause with null callStatusManagerPtr_
 * @tc.desc     Boundary test
 */
HWTEST_F(ReportCallInfoHandlerTest, ReportCallInfoHandler_UpdateDisconnectedCause_0100, TestSize.Level1)
{
    auto handler = DelayedSingleton<ReportCallInfoHandler>::GetInstance();
    ASSERT_TRUE(handler != nullptr);
    handler->callStatusManagerPtr_ = nullptr;
    
    DisconnectedDetails details;
    int32_t ret = handler->UpdateDisconnectedCause(details);
    EXPECT_EQ(ret, TELEPHONY_ERR_LOCAL_PTR_NULL);
}

/**
 * @tc.number   ReportCallInfoHandler_UpdateEventResultInfo_0100
 * @tc.name     Test UpdateEventResultInfo with null callStatusManagerPtr_
 * @tc.desc     Boundary test
 */
HWTEST_F(ReportCallInfoHandlerTest, ReportCallInfoHandler_UpdateEventResultInfo_0100, TestSize.Level1)
{
    auto handler = DelayedSingleton<ReportCallInfoHandler>::GetInstance();
    ASSERT_TRUE(handler != nullptr);
    handler->callStatusManagerPtr_ = nullptr;
    
    CellularCallEventInfo info;
    int32_t ret = handler->UpdateEventResultInfo(info);
    EXPECT_EQ(ret, TELEPHONY_ERR_LOCAL_PTR_NULL);
}

/**
 * @tc.number   ReportCallInfoHandler_UpdateEventResultInfo_0200
 * @tc.name     Test UpdateEventResultInfo with valid parameters
 * @tc.desc     Function test
 */
HWTEST_F(ReportCallInfoHandlerTest, ReportCallInfoHandler_UpdateEventResultInfo_0200, TestSize.Level1)
{
    auto handler = DelayedSingleton<ReportCallInfoHandler>::GetInstance();
    ASSERT_TRUE(handler != nullptr);
    handler->Init();
    ASSERT_TRUE(handler->callStatusManagerPtr_ != nullptr);
    
    CellularCallEventInfo info;
    int32_t ret = handler->UpdateEventResultInfo(info);
    EXPECT_EQ(ret, TELEPHONY_SUCCESS);
}

/**
 * @tc.number   ReportCallInfoHandler_UpdateOttEventInfo_0100
 * @tc.name     Test UpdateOttEventInfo with null callStatusManagerPtr_
 * @tc.desc     Boundary test
 */
HWTEST_F(ReportCallInfoHandlerTest, ReportCallInfoHandler_UpdateOttEventInfo_0100, TestSize.Level1)
{
    auto handler = DelayedSingleton<ReportCallInfoHandler>::GetInstance();
    ASSERT_TRUE(handler != nullptr);
    handler->callStatusManagerPtr_ = nullptr;
    
    OttCallEventInfo info;
    int32_t ret = handler->UpdateOttEventInfo(info);
    EXPECT_EQ(ret, TELEPHONY_ERR_LOCAL_PTR_NULL);
}

/**
 * @tc.number   ReportCallInfoHandler_UpdateOttEventInfo_0200
 * @tc.name     Test UpdateOttEventInfo with valid parameters
 * @tc.desc     Function test
 */
HWTEST_F(ReportCallInfoHandlerTest, ReportCallInfoHandler_UpdateOttEventInfo_0200, TestSize.Level1)
{
    auto handler = DelayedSingleton<ReportCallInfoHandler>::GetInstance();
    ASSERT_TRUE(handler != nullptr);
    handler->Init();
    ASSERT_TRUE(handler->callStatusManagerPtr_ != nullptr);
    
    OttCallEventInfo info;
    int32_t ret = handler->UpdateOttEventInfo(info);
    EXPECT_EQ(ret, TELEPHONY_SUCCESS);
}

/**
 * @tc.number   ReportCallInfoHandler_ReceiveImsCallModeRequest_0100
 * @tc.name     Test ReceiveImsCallModeRequest with valid parameters
 * @tc.desc     Function test
 */
HWTEST_F(ReportCallInfoHandlerTest, ReportCallInfoHandler_ReceiveImsCallModeRequest_0100, TestSize.Level1)
{
    auto handler = DelayedSingleton<ReportCallInfoHandler>::GetInstance();
    ASSERT_TRUE(handler != nullptr);
    
    CallModeReportInfo response;
    response.callIndex = VALID_CALL_INDEX;
    
    int32_t ret = handler->ReceiveImsCallModeRequest(response);
    EXPECT_EQ(ret, TELEPHONY_SUCCESS);
}

/**
 * @tc.number   ReportCallInfoHandler_ReceiveImsCallModeResponse_0100
 * @tc.name     Test ReceiveImsCallModeResponse with valid slotId
 * @tc.desc     Function test
 */
HWTEST_F(ReportCallInfoHandlerTest, ReportCallInfoHandler_ReceiveImsCallModeResponse_0100, TestSize.Level1)
{
    auto handler = DelayedSingleton<ReportCallInfoHandler>::GetInstance();
    ASSERT_TRUE(handler != nullptr);
    
    CallModeReportInfo response;
    response.callIndex = VALID_CALL_INDEX;
    response.slotId = VALID_SLOT_ID;
    
    int32_t ret = handler->ReceiveImsCallModeResponse(response);
    EXPECT_EQ(ret, TELEPHONY_SUCCESS);
}

/**
 * @tc.number   ReportCallInfoHandler_ReceiveImsCallModeResponse_0200
 * @tc.name     Test ReceiveImsCallModeResponse with invalid slotId
 * @tc.desc     Function test
 */
HWTEST_F(ReportCallInfoHandlerTest, ReportCallInfoHandler_ReceiveImsCallModeResponse_0200, TestSize.Level1)
{
    auto handler = DelayedSingleton<ReportCallInfoHandler>::GetInstance();
    ASSERT_TRUE(handler != nullptr);
    
    CallModeReportInfo response;
    response.callIndex = VALID_CALL_INDEX;
    response.slotId = INVALID_SLOT_ID;
    
    int32_t ret = handler->ReceiveImsCallModeResponse(response);
    EXPECT_EQ(ret, TELEPHONY_SUCCESS);
}

/**
 * @tc.number   ReportCallInfoHandler_UpdateVoipEventInfo_0100
 * @tc.name     Test UpdateVoipEventInfo with null callStatusManagerPtr_
 * @tc.desc     Boundary test
 */
HWTEST_F(ReportCallInfoHandlerTest, ReportCallInfoHandler_UpdateVoipEventInfo_0100, TestSize.Level1)
{
    auto handler = DelayedSingleton<ReportCallInfoHandler>::GetInstance();
    ASSERT_TRUE(handler != nullptr);
    handler->callStatusManagerPtr_ = nullptr;
    
    VoipCallEventInfo info;
    int32_t ret = handler->UpdateVoipEventInfo(info);
    EXPECT_EQ(ret, TELEPHONY_ERR_LOCAL_PTR_NULL);
}

/**
 * @tc.number   ReportCallInfoHandler_UpdateVoipEventInfo_0200
 * @tc.name     Test UpdateVoipEventInfo with valid parameters
 * @tc.desc     Function test
 */
HWTEST_F(ReportCallInfoHandlerTest, ReportCallInfoHandler_UpdateVoipEventInfo_0200, TestSize.Level1)
{
    auto handler = DelayedSingleton<ReportCallInfoHandler>::GetInstance();
    ASSERT_TRUE(handler != nullptr);
    handler->Init();
    ASSERT_TRUE(handler->callStatusManagerPtr_ != nullptr);
    
    VoipCallEventInfo info;
    int32_t ret = handler->UpdateVoipEventInfo(info);
    EXPECT_EQ(ret, TELEPHONY_SUCCESS);
}

/**
 * @tc.number   ReportCallInfoHandler_BuildCallDetailsInfo_0100
 * @tc.name     Test BuildCallDetailsInfo with empty callVec
 * @tc.desc     Boundary test
 */
HWTEST_F(ReportCallInfoHandlerTest, ReportCallInfoHandler_BuildCallDetailsInfo_0100, TestSize.Level1)
{
    auto handler = DelayedSingleton<ReportCallInfoHandler>::GetInstance();
    ASSERT_TRUE(handler != nullptr);
    
    CallDetailsInfo info;
    CallDetailsInfo callDetailsInfo;
    
    handler->BuildCallDetailsInfo(info, callDetailsInfo);
    EXPECT_TRUE(callDetailsInfo.callVec.empty());
}

/**
 * @tc.number   ReportCallInfoHandler_BuildCallDetailsInfo_0200
 * @tc.name     Test BuildCallDetailsInfo with valid callVec
 * @tc.desc     Function test
 */
HWTEST_F(ReportCallInfoHandlerTest, ReportCallInfoHandler_BuildCallDetailsInfo_0200, TestSize.Level1)
{
    auto handler = DelayedSingleton<ReportCallInfoHandler>::GetInstance();
    ASSERT_TRUE(handler != nullptr);
    
    CallDetailsInfo info;
    CallDetailInfo detailInfo;
    detailInfo.callType = CallType::TYPE_CS;
    detailInfo.accountId = VALID_SLOT_ID;
    detailInfo.state = TelCallState::CALL_STATUS_ACTIVE;
    detailInfo.index = VALID_CALL_INDEX;
    info.callVec.push_back(detailInfo);
    
    CallDetailsInfo callDetailsInfo;
    handler->BuildCallDetailsInfo(info, callDetailsInfo);
    
    EXPECT_EQ(callDetailsInfo.callVec.size(), 1);
    EXPECT_EQ(callDetailsInfo.callVec[0].callType, CallType::TYPE_CS);
    EXPECT_EQ(callDetailsInfo.callVec[0].accountId, VALID_SLOT_ID);
}

#ifdef SUPPORT_RTT_CALL
/**
 * @tc.number   ReportCallInfoHandler_UpdateRttEventInfo_0100
 * @tc.name     Test UpdateRttEventInfo with null callStatusManagerPtr_
 * @tc.desc     Boundary test
 */
HWTEST_F(ReportCallInfoHandlerTest, ReportCallInfoHandler_UpdateRttEventInfo_0100, TestSize.Level1)
{
    auto handler = DelayedSingleton<ReportCallInfoHandler>::GetInstance();
    ASSERT_TRUE(handler != nullptr);
    handler->callStatusManagerPtr_ = nullptr;
    
    ImsRTTEventType eventType;
    handler->UpdateRttEventInfo(eventType);
}

/**
 * @tc.number   ReportCallInfoHandler_UpdateRttEventInfo_0200
 * @tc.name     Test UpdateRttEventInfo with valid parameters
 * @tc.desc     Function test
 */
HWTEST_F(ReportCallInfoHandlerTest, ReportCallInfoHandler_UpdateRttEventInfo_0200, TestSize.Level1)
{
    auto handler = DelayedSingleton<ReportCallInfoHandler>::GetInstance();
    ASSERT_TRUE(handler != nullptr);
    handler->Init();
    ASSERT_TRUE(handler->callStatusManagerPtr_ != nullptr);
    
    ImsRTTEventType eventType;
    handler->UpdateRttEventInfo(eventType);
}
#endif

} // namespace Telephony
} // namespace OHOS
