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

#include "call_manager_service_test_base.h"

namespace OHOS {
namespace Telephony {

/**
 * @tc.number   CallManagerService_SendCallUiEvent_0100
 * @tc.name     test SendCallUiEvent with various events
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerServiceTest, CallManagerService_SendCallUiEvent_0100, TestSize.Level1)
{
    std::string eventName = "EVENT_IS_CELIA_CALL";
    int32_t ret = service_->SendCallUiEvent(1, eventName);
    EXPECT_NE(ret, TELEPHONY_SUCCESS);

    eventName = "EVENT_SPEAKER_OFF";
    ret = service_->SendCallUiEvent(1, eventName);
    EXPECT_NE(ret, TELEPHONY_SUCCESS);

    eventName = "EVENT_SUPPORT_BLUETOOTH_CALL";
    ret = service_->SendCallUiEvent(1, eventName);
    EXPECT_NE(ret, TELEPHONY_SUCCESS);

    eventName = "EVENT_NOT_SUPPORT_BLUETOOTH_CALL";
    ret = service_->SendCallUiEvent(1, eventName);
    EXPECT_EQ(ret, TELEPHONY_SUCCESS);

    eventName = "EVENT_INVALID_VIDEO_FD";
    SetCallControlManagerNull();
    ret = service_->SendCallUiEvent(1, eventName);
    EXPECT_EQ(ret, TELEPHONY_ERR_LOCAL_PTR_NULL);

    eventName = "unknown_event";
    ret = service_->SendCallUiEvent(1, eventName);
    EXPECT_EQ(ret, TELEPHONY_SUCCESS);
}

/**
 * @tc.number   CallManagerService_SendCallUiEvent_0200
 * @tc.name     test SendCallUiEvent with more events
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerServiceTest, CallManagerService_SendCallUiEvent_0200, TestSize.Level1)
{
    std::string eventName = "DISPLAY_SPECIFIED_CALL_PAGE_BY_CALL_ID";
    int32_t ret = service_->SendCallUiEvent(999, eventName);
    EXPECT_EQ(ret, TELEPHONY_ERR_LOCAL_PTR_NULL);

    eventName = "EVENT_BLUETOOTH_SCO_STATE_OFF";
    ret = service_->SendCallUiEvent(1, eventName);
    EXPECT_NE(ret, TELEPHONY_SUCCESS);

    eventName = "EVENT_BLUETOOTH_SCO_STATE_ON";
    ret = service_->SendCallUiEvent(1, eventName);
    EXPECT_NE(ret, TELEPHONY_SUCCESS);

    eventName = "EVENT_CELIA_AUTO_ANSWER_CALL_ON";
    ret = service_->SendCallUiEvent(999, eventName);
    EXPECT_EQ(ret, TELEPHONY_ERR_LOCAL_PTR_NULL);

    eventName = "EVENT_CELIA_AUTO_ANSWER_CALL_OFF";
    ret = service_->SendCallUiEvent(999, eventName);
    EXPECT_EQ(ret, TELEPHONY_ERR_LOCAL_PTR_NULL);

    eventName = "EVENT_VOIP_CALL_SUCCESS";
    ret = service_->SendCallUiEvent(999, eventName);
    EXPECT_EQ(ret, TELEPHONY_SUCCESS);

    eventName = "EVENT_VOIP_CALL_FAILED";
    ret = service_->SendCallUiEvent(999, eventName);
    EXPECT_EQ(ret, TELEPHONY_SUCCESS);
}

/**
 * @tc.number   CallManagerService_HandleVoIPCallEvent_0100
 * @tc.name     test HandleVoIPCallEvent with null call
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerServiceTest, CallManagerService_HandleVoIPCallEvent_0100, TestSize.Level1)
{
    std::string eventName = "EVENT_VOIP_CALL_SUCCESS";
    int32_t ret = service_->HandleVoIPCallEvent(999, eventName);
    EXPECT_EQ(ret, TELEPHONY_ERR_LOCAL_PTR_NULL);

    eventName = "EVENT_VOIP_CALL_FAILED";
    ret = service_->HandleVoIPCallEvent(999, eventName);
    EXPECT_EQ(ret, TELEPHONY_ERR_LOCAL_PTR_NULL);
}

/**
 * @tc.number   CallManagerService_HandleDisplaySpecifiedCallPage_0100
 * @tc.name     test HandleDisplaySpecifiedCallPage with null call
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerServiceTest, CallManagerService_HandleDisplaySpecifiedCallPage_0100, TestSize.Level1)
{
    int32_t ret = service_->HandleDisplaySpecifiedCallPage(999);
    EXPECT_EQ(ret, TELEPHONY_ERR_LOCAL_PTR_NULL);
}

/**
 * @tc.number   CallManagerService_HandleCeliaAutoAnswerCall_0100
 * @tc.name     test HandleCeliaAutoAnswerCall with null call
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerServiceTest, CallManagerService_HandleCeliaAutoAnswerCall_0100, TestSize.Level1)
{
    int32_t ret = service_->HandleCeliaAutoAnswerCall(999, true);
    EXPECT_EQ(ret, TELEPHONY_ERR_LOCAL_PTR_NULL);
}

/**
 * @tc.number   CallManagerService_dealCeliaCallEvent_0100
 * @tc.name     test dealCeliaCallEvent with null call
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerServiceTest, CallManagerService_dealCeliaCallEvent_0100, TestSize.Level1)
{
    int32_t ret = service_->dealCeliaCallEvent(999);
    EXPECT_EQ(ret, TELEPHONY_ERR_LOCAL_PTR_NULL);
}

/**
 * @tc.number   CallManagerService_SendUssdResponse_0100
 * @tc.name     test SendUssdResponse
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerServiceTest, CallManagerService_SendUssdResponse_0100, TestSize.Level1)
{
    int32_t ret = service_->SendUssdResponse(0, "1");
    EXPECT_EQ(ret, TELEPHONY_SUCCESS);
}

/**
 * @tc.number   CallManagerService_SetCallAudioMode_0100
 * @tc.name     test SetCallAudioMode with null callControlManagerPtr_
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerServiceTest, CallManagerService_SetCallAudioMode_0100, TestSize.Level1)
{
    SetCallControlManagerNull();
    int32_t ret = service_->SetCallAudioMode(0, 0);
    EXPECT_EQ(ret, TELEPHONY_ERR_LOCAL_PTR_NULL);
}

/**
 * @tc.number   CallManagerService_CloseUnFinishedUssd_0100
 * @tc.name     test CloseUnFinishedUssd with null callControlManagerPtr_
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerServiceTest, CallManagerService_CloseUnFinishedUssd_0100, TestSize.Level1)
{
    SetCallControlManagerNull();
    int32_t ret = service_->CloseUnFinishedUssd(0);
    EXPECT_EQ(ret, TELEPHONY_ERR_LOCAL_PTR_NULL);
}

/**
 * @tc.number   CallManagerService_ReportOttCallEventInfo_0100
 * @tc.name     test ReportOttCallEventInfo
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerServiceTest, CallManagerService_ReportOttCallEventInfo_0100, TestSize.Level1)
{
    OttCallEventInfo eventInfo;
    int32_t ret = service_->ReportOttCallEventInfo(eventInfo);
    EXPECT_NE(ret, TELEPHONY_ERR_PERMISSION_ERR);
}

} // namespace Telephony
} // namespace OHOS
