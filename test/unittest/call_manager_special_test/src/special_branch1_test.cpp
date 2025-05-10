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
#include "call_manager_connect.h"
#include "call_voice_assistant_manager.h"
#include "spam_call_adapter.h"
#include "bluetooth_call_client.h"
#include "bluetooth_call_manager.h"
#include "bluetooth_call_service.h"
#include "bluetooth_connection.h"
#include "iremote_broker.h"
#include "call_ability_connect_callback.h"
#include "call_ability_report_proxy.h"
#include "call_connect_ability.h"
#include "call_control_manager.h"
#include "call_manager_client.h"
#include "call_manager_hisysevent.h"
#include "call_number_utils.h"
#include "call_policy.h"
#include "call_records_manager.h"
#include "call_request_event_handler_helper.h"
#include "call_request_handler.h"
#include "call_request_process.h"
#include "call_setting_manager.h"
#include "call_state_report_proxy.h"
#include "call_status_manager.h"
#include "cellular_call_connection.h"
#include "common_event_manager.h"
#include "common_event_support.h"
#include "cs_call.h"
#include "cs_conference.h"
#include "distributed_call_manager.h"
#include "gtest/gtest.h"
#include "i_voip_call_manager_service.h"
#include "ims_call.h"
#include "ims_conference.h"
#include "incoming_call_notification.h"
#include "missed_call_notification.h"
#include "ott_call.h"
#include "ott_conference.h"
#include "reject_call_sms.h"
#include "report_call_info_handler.h"
#include "satellite_call.h"
#include "surface_utils.h"
#include "telephony_errors.h"
#include "telephony_hisysevent.h"
#include "telephony_log_wrapper.h"
#include "video_call_state.h"
#include "video_control_manager.h"
#include "voip_call_manager_proxy.h"
#include "voip_call.h"
#include "accesstoken_kit.h"
#include "token_setproc.h"
#include "nativetoken_kit.h"
#include "number_identity_data_base_helper.h"
#include "call_ability_callback_death_recipient.h"
#include "app_state_observer.h"
#include "call_ability_callback_proxy.h"
#include "super_privacy_manager_client.h"
#include "call_status_callback.h"
#include "satellite_call_control.h"
#include "proximity_sensor.h"
#include "status_bar.h"
#include "wired_headset.h"
#include "call_status_policy.h"
#include "call_superprivacy_control_manager.h"
#include "bluetooth_hfp_ag.h"
#include "call_manager_service.h"
#include "telephony_types.h"
#include "telephony_permission.h"
#include "voip_call_connection.h"
#include "number_identity_service.h"
#include "bluetooth_call.h"
#include "bluetooth_call_connection.h"
#include "bluetooth_call_state.h"
#include "call_broadcast_subscriber.h"
#include "call_incoming_filter_manager.h"
#include "call_object_manager.h"
#include "call_wired_headset.h"
#include "fold_status_manager.h"
#include "audio_control_manager.h"
#include "call_state_processor.h"

namespace OHOS {
namespace Telephony {
using namespace testing::ext;

class SpecialBranch1Test : public testing::Test {
    public:
        static void SetUpTestCase();
        static void TearDownTestCase();
        void SetUp();
        void TearDown();
};

void SpecialBranch1Test::SetUpTestCase()
{
    const char *perms[1] = {
        "ohos.permission.WRITE_CALL_LOG",
    };
    NativeTokenInfoParams infoInstance = {
        .dcapsNum = 0,
        .permsNum = 1,
        .aclsNum = 0,
        .dcaps = nullptr,
        .perms = perms,
        .acls = nullptr,
        .processName = "SpecialBranch1Test",
        .aplStr = "system_basic",
    };
    uint64_t tokenId = GetAccessTokenId(&infoInstance);
    SetSelfTokenID(tokenId);
    auto result = Security::AccessToken::AccessTokenKit::ReloadNativeTokenInfo();
    EXPECT_EQ(result, Security::AccessToken::RET_SUCCESS);
}

void SpecialBranch1Test::TearDownTestCase()
{
}

void SpecialBranch1Test::SetUp()
{
}

void SpecialBranch1Test::TearDown()
{
    sleep(1);
}

/**
 * @tc.number   Telephony_CallManagerService_001
 * @tc.name     test branch
 * @tc.desc     Function test
 */
HWTEST_F(SpecialBranch1Test, Telephony_CallManagerService_001, TestSize.Level0)
{
    std::shared_ptr<CallManagerService> callManagerService = std::make_shared<CallManagerService>();
    ASSERT_TRUE(callManagerService != nullptr);
    sptr<ICallAbilityCallback> callback;
    int32_t ret = callManagerService->RegisterCallBack(callback);
    EXPECT_NE(ret, 0);
}

/**
 * @tc.number   Telephony_CallManagerService_002
 * @tc.name     test branch
 * @tc.desc     Function test
 */
HWTEST_F(SpecialBranch1Test, Telephony_CallManagerService_002, TestSize.Level0)
{
    std::shared_ptr<CallManagerService> callManagerService = std::make_shared<CallManagerService>();
    ASSERT_TRUE(callManagerService != nullptr);
    int32_t ret = callManagerService->UnRegisterCallBack();
    EXPECT_NE(ret, 0);
}

/**
 * @tc.number   Telephony_CallManagerService_003
 * @tc.name     test branch
 * @tc.desc     Function test
 */
HWTEST_F(SpecialBranch1Test, Telephony_CallManagerService_003, TestSize.Level0)
{
    std::shared_ptr<CallManagerService> callManagerService = std::make_shared<CallManagerService>();
    ASSERT_TRUE(callManagerService != nullptr);
    int32_t ret = callManagerService->ObserverOnCallDetailsChange();
    EXPECT_NE(ret, 0);
}

/**
 * @tc.number   Telephony_CallManagerService_004
 * @tc.name     test branch
 * @tc.desc     Function test
 */
HWTEST_F(SpecialBranch1Test, Telephony_CallManagerService_004, TestSize.Level0)
{
    std::shared_ptr<CallManagerService> callManagerService = std::make_shared<CallManagerService>();
    ASSERT_TRUE(callManagerService != nullptr);
    CallRestrictionInfo info;
    int32_t ret = callManagerService->SetCallRestriction(0, info);
    EXPECT_NE(ret, 0);
}

/**
 * @tc.number   Telephony_CallManagerService_005
 * @tc.name     test branch
 * @tc.desc     Function test
 */
HWTEST_F(SpecialBranch1Test, Telephony_CallManagerService_005, TestSize.Level0)
{
    std::shared_ptr<CallManagerService> callManagerService = std::make_shared<CallManagerService>();
    ASSERT_TRUE(callManagerService != nullptr);
    CallTransferInfo info;
    int32_t ret = callManagerService->SetCallTransferInfo(0, info);
    EXPECT_NE(ret, 0);
}

/**
 * @tc.number   Telephony_CallManagerService_006
 * @tc.name     test branch
 * @tc.desc     Function test
 */
HWTEST_F(SpecialBranch1Test, Telephony_CallManagerService_006, TestSize.Level0)
{
    std::shared_ptr<CallManagerService> callManagerService = std::make_shared<CallManagerService>();
    ASSERT_TRUE(callManagerService != nullptr);
    AudioDevice audioDevice;
    int32_t ret = callManagerService->SetAudioDevice(audioDevice);
    EXPECT_NE(ret, 0);
}

/**
 * @tc.number   Telephony_CallManagerService_007
 * @tc.name     test branch
 * @tc.desc     Function test
 */
HWTEST_F(SpecialBranch1Test, Telephony_CallManagerService_007, TestSize.Level0)
{
    std::shared_ptr<CallManagerService> callManagerService = std::make_shared<CallManagerService>();
    ASSERT_TRUE(callManagerService != nullptr);
    std::string surfaceId;
    int32_t ret = callManagerService->SetPreviewWindow(0, surfaceId, nullptr);
    EXPECT_NE(ret, 0);
}

/**
 * @tc.number   Telephony_CallManagerService_008
 * @tc.name     test branch
 * @tc.desc     Function test
 */
HWTEST_F(SpecialBranch1Test, Telephony_CallManagerService_008, TestSize.Level0)
{
    std::shared_ptr<CallManagerService> callManagerService = std::make_shared<CallManagerService>();
    ASSERT_TRUE(callManagerService != nullptr);
    std::string surfaceId;
    int32_t ret = callManagerService->SetDisplayWindow(0, surfaceId, nullptr);
    EXPECT_NE(ret, 0);
}

/**
 * @tc.number   Telephony_CallManagerService_009
 * @tc.name     test branch
 * @tc.desc     Function test
 */
HWTEST_F(SpecialBranch1Test, Telephony_CallManagerService_009, TestSize.Level0)
{
    std::shared_ptr<CallManagerService> callManagerService = std::make_shared<CallManagerService>();
    ASSERT_TRUE(callManagerService != nullptr);
    std::vector<OttCallDetailsInfo> ottVec;
    int32_t ret = callManagerService->ReportOttCallDetailsInfo(ottVec);
    EXPECT_NE(ret, 0);
}

/**
 * @tc.number   Telephony_CallManagerService_010
 * @tc.name     test branch
 * @tc.desc     Function test
 */
HWTEST_F(SpecialBranch1Test, Telephony_CallManagerService_010, TestSize.Level0)
{
    std::shared_ptr<CallManagerService> callManagerService = std::make_shared<CallManagerService>();
    ASSERT_TRUE(callManagerService != nullptr);
    OttCallEventInfo eventInfo;
    int32_t ret = callManagerService->ReportOttCallEventInfo(eventInfo);
    EXPECT_NE(ret, 0);
}

/**
 * @tc.number   Telephony_SatelliteCall_001
 * @tc.name     test branch
 * @tc.desc     Function test
 */
HWTEST_F(SpecialBranch1Test, Telephony_SatelliteCall_001, TestSize.Level0)
{
    DialParaInfo info;
    std::shared_ptr<SatelliteCall> satelliteCall = std::make_shared<SatelliteCall>(info);
    ASSERT_TRUE(satelliteCall != nullptr);
    satelliteCall->HoldCall();
    satelliteCall->UnHoldCall();
    satelliteCall->SwitchCall();
    satelliteCall->SetMute(0, 0);
    satelliteCall->CombineConference();
    satelliteCall->HandleCombineConferenceFailEvent();
    satelliteCall->SeparateConference();
    satelliteCall->KickOutFromConference();
    satelliteCall->CanCombineConference();
    satelliteCall->CanSeparateConference();
    satelliteCall->CanKickOutFromConference();
    satelliteCall->LaunchConference();
    satelliteCall->ExitConference();
    satelliteCall->HoldConference();
    int32_t callId = 0;
    satelliteCall->GetMainCallId(callId);
    std::vector<std::u16string> callIdList;
    satelliteCall->GetSubCallIdList(callIdList);
    satelliteCall->GetCallIdListForConference(callIdList);
    int32_t ret = satelliteCall->IsSupportConferenceable();
    EXPECT_NE(ret, 0);
}

/**
 * @tc.number   Telephony_CallRequestProcess_001
 * @tc.name     test branch
 * @tc.desc     Function test
 */
HWTEST_F(SpecialBranch1Test, Telephony_CallRequestProcess_001, TestSize.Level0)
{
    std::shared_ptr<CallRequestProcess> callRequestProcess = std::make_shared<CallRequestProcess>();
    ASSERT_TRUE(callRequestProcess != nullptr);
    DialParaInfo info;
    info.dialType = static_cast<DialType>(-1);
    int32_t ret = callRequestProcess->HandleDialRequest(info);
    EXPECT_NE(ret, 0);
}
} // namespace Telephony
} // namespace OHOS
