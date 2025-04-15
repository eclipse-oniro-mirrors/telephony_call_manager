/*
 * Copyright (C) 2025-2025 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <gtest/gtest.h>
#include <cstring>
#include "cs_call.h"
#include "interoperable_communication_manager.h"
#include "singleton.h"

namespace OHOS {
namespace Telephony {
using namespace testing::ext;
 
class InteroperableCommunicationManagerTest : public testing::Test {
public:
    static void SetUpTestCase() {}
    static void TearDownTestCase() {}
    virtual void SetUp() {}
    virtual void TearDown() {}
};
 
/**
 * @tc.number   Telephony_InteroperableCommunicationManagerTest_001
 * @tc.name     test normal branch
 * @tc.desc     normal branch test
 */
HWTEST_F(InteroperableCommunicationManagerTest,
         Telephony_InteroperableCommunicationManagerTest_001, Function | MediumTest | Level1)
{
    DisconnectedDetails details;
    sptr<CallBase> call1 = nullptr;
    DialParaInfo mDialParaInfo;
    sptr<CallBase> call2 = new CSCall(mDialParaInfo);
    std::string devId = "UnitTestDeviceId";
    auto dcManager = DelayedSingleton<InteroperableCommunicationManager>::GetInstance();
    TelCallState pState = TelCallState::CALL_STATUS_UNKNOWN;
    TelCallState nState1 = TelCallState::CALL_STATUS_INCOMING;
    TelCallState nState2 = TelCallState::CALL_STATUS_DISCONNECTED;
    TelCallState nState3 = TelCallState::CALL_STATUS_DISCONNECTING;
    DistributedHardware::DmDeviceInfo deviceInfo;
    deviceInfo.deviceTypeId = 0x6D;
 
    ASSERT_NO_THROW(dcManager->CallStateUpdated(call1, pState, nState1));
    ASSERT_NO_THROW(dcManager->CallStateUpdated(call2, pState, nState1));
    ASSERT_NO_THROW(dcManager->SetMuted(true));
    ASSERT_NO_THROW(dcManager->MuteRinger());
    dcManager->OnDeviceOnline(deviceInfo);
    ASSERT_NO_THROW(dcManager->CallStateUpdated(call1, pState, nState1));
    ASSERT_NO_THROW(dcManager->CallStateUpdated(call2, pState, nState1));
    ASSERT_NO_THROW(dcManager->CallStateUpdated(call2, pState, nState2));
    ASSERT_NO_THROW(dcManager->CallStateUpdated(call2, pState, nState3));
    ASSERT_NO_THROW(dcManager->SetMuted(true));
    ASSERT_NO_THROW(dcManager->MuteRinger());
}
 
/**
 * @tc.number   Telephony_InteroperableCommunicationManagerTest_002
 * @tc.name     test device online and offline
 * @tc.desc     Function test
 */
HWTEST_F(InteroperableCommunicationManagerTest,
         Telephony_InteroperableCommunicationManagerTest_002, Function | MediumTest | Level1)
{
    DistributedHardware::DmDeviceInfo deviceInfo;
    strncpy_s(deviceInfo.networkId, DM_MAX_DEVICE_ID_LEN + 1, "NetId", DM_MAX_DEVICE_ID_LEN + 1);
    strncpy_s(deviceInfo.deviceName, DM_MAX_DEVICE_NAME_LEN + 1, "DevName", DM_MAX_DEVICE_NAME_LEN + 1);
    deviceInfo.deviceTypeId = 1;
    auto dcManager = DelayedSingleton<InteroperableCommunicationManager>::GetInstance();
    
    ASSERT_NO_THROW(dcManager->OnDeviceOnline(deviceInfo));
    ASSERT_NO_THROW(dcManager->OnDeviceOffline(deviceInfo));

    deviceInfo.deviceTypeId = 0x0E;
    ASSERT_NO_THROW(dcManager->OnDeviceOnline(deviceInfo));
    ASSERT_NO_THROW(dcManager->OnDeviceOffline(deviceInfo));

    deviceInfo.deviceTypeId = 0x6D;
    ASSERT_NO_THROW(dcManager->OnDeviceOnline(deviceInfo));
    ASSERT_NO_THROW(dcManager->OnDeviceOffline(deviceInfo));

    deviceInfo.deviceTypeId = 0x83;
    ASSERT_NO_THROW(dcManager->OnDeviceOnline(deviceInfo));
    ASSERT_NO_THROW(dcManager->OnDeviceOffline(deviceInfo));
}
} // namespace Telephony
} // namespace OHOS