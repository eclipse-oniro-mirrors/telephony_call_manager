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
 
#include "call_manager_service_test_base.h"
 
#include "core_service_client.h"
#include "dialog_ability_connection.h"
#include "distributed_communication_manager.h"
#include "iremote_object.h"
#include "mock_i_core_service.h"
#include "sim_state_type.h"
#include "telephony_permission.h"
 
namespace OHOS {
namespace Telephony {
namespace {
using namespace Security::AccessToken;
 
constexpr int32_t TEST_SLOT_ID = 0;
 
HapInfoParams directCallSystemInfoParams = {
    .bundleName = "tel_call_manager_service_gtest",
    .userID = 1,
    .instIndex = 0,
    .appIDDesc = "test",
    .isSystemApp = true,
};
 
PermissionDef directCallNoPlaceCallDef = {
    .permissionName = "ohos.permission.ANSWER_CALL",
    .bundleName = "tel_call_manager_service_gtest",
    .grantMode = 1,
    .label = "label",
    .labelId = 1,
    .descriptionId = 1,
    .availableLevel = APL_SYSTEM_BASIC,
};
 
PermissionStateFull directCallNoPlaceCallState = {
    .grantFlags = {2},
    .grantStatus = {PermissionState::PERMISSION_GRANTED},
    .isGeneral = true,
    .permissionName = "ohos.permission.ANSWER_CALL",
    .resDeviceID = {"local"},
};
 
HapPolicyParams directCallNoPlaceCallPolicyParams = {
    .apl = APL_SYSTEM_BASIC,
    .domain = "test.domain",
    .permList = {directCallNoPlaceCallDef},
    .permStateList = {directCallNoPlaceCallState},
};
 
class DirectCallSystemAppNoPlaceCallToken {
public:
    DirectCallSystemAppNoPlaceCallToken()
    {
        currentID_ = GetSelfTokenID();
        AccessTokenIDEx tokenIdEx =
            AccessTokenKit::AllocHapToken(directCallSystemInfoParams, directCallNoPlaceCallPolicyParams);
        accessID_ = tokenIdEx.tokenIdExStruct.tokenID;
        SetSelfTokenID(tokenIdEx.tokenIDEx);
    }
    ~DirectCallSystemAppNoPlaceCallToken()
    {
        AccessTokenKit::DeleteToken(accessID_);
        SetSelfTokenID(currentID_);
    }
 
private:
    AccessTokenID currentID_ = 0;
    AccessTokenID accessID_ = 0;
};
 
void SetCoreServiceMock(const std::vector<IccAccountInfo> &accountInfoList, int32_t defaultVoiceSlotId,
    int32_t getListRet = TELEPHONY_SUCCESS)
{
    sptr<MockICoreService> mockCoreService = new MockICoreService();
    ON_CALL(*mockCoreService, GetActiveSimAccountInfoList(testing::_))
        .WillByDefault(testing::DoAll(testing::SetArgReferee<0>(accountInfoList), testing::Return(getListRet)));
    ON_CALL(*mockCoreService, GetDefaultVoiceSlotId()).WillByDefault(testing::Return(defaultVoiceSlotId));
    CoreServiceClient::GetInstance().proxy_ = mockCoreService;
}
 
void ResetCoreServiceMock()
{
    CoreServiceClient::GetInstance().proxy_ = nullptr;
}
 
std::vector<IccAccountInfo> MakeIccAccountInfoList(int32_t count)
{
    std::vector<IccAccountInfo> accountInfoList;
    for (int32_t i = 0; i < count; ++i) {
        IccAccountInfo info;
        info.slotIndex = i;
        info.simId = i + 1;
        accountInfoList.push_back(info);
    }
    return accountInfoList;
}
 
class DirectCallMockRemoteObject : public IRemoteObject {
public:
    explicit DirectCallMockRemoteObject(int32_t sendResult = 0) : IRemoteObject(u"DirectCallMockRemoteObject"),
        sendResult_(sendResult) {}
 
    int SendRequest(uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option)
    {
        reply.WriteInt32(0);
        return sendResult_;
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
 
private:
    int32_t sendResult_ = 0;
};
} // namespace
 
/**
 * @tc.number   CallManagerService_MakeDirectCall_0100
 * @tc.name     test MakeDirectCall when getting sim list fails
 * @tc.desc     Failed account info query should dial directly
 */
HWTEST_F(CallManagerServiceTest, CallManagerService_MakeDirectCall_0100, TestSize.Level1)
{
    SetCoreServiceMock({}, -1, TELEPHONY_ERR_FAIL);
    int32_t ret = service_->MakeDirectCall("10086");
    EXPECT_EQ(ret, TELEPHONY_ERR_PERMISSION_ERR);
    ResetCoreServiceMock();
}
 
/**
 * @tc.number   CallManagerService_MakeDirectCall_0200
 * @tc.name     test MakeDirectCall with one sim card
 * @tc.desc     Single sim card should dial directly with the sim slot index
 */
HWTEST_F(CallManagerServiceTest, CallManagerService_MakeDirectCall_0200, TestSize.Level1)
{
    SetCoreServiceMock(MakeIccAccountInfoList(1), -1);
    int32_t ret = service_->MakeDirectCall("10086");
    EXPECT_EQ(ret, TELEPHONY_ERR_PERMISSION_ERR);
    ResetCoreServiceMock();
}
 
/**
 * @tc.number   CallManagerService_MakeDirectCall_0300
 * @tc.name     test MakeDirectCall with multi sim and valid default voice slot
 * @tc.desc     Multi sim with a valid default dial card should dial directly
 */
HWTEST_F(CallManagerServiceTest, CallManagerService_MakeDirectCall_0300, TestSize.Level1)
{
    SetCoreServiceMock(MakeIccAccountInfoList(2), TEST_SLOT_ID);
    int32_t ret = service_->MakeDirectCall("10086");
    EXPECT_EQ(ret, TELEPHONY_ERR_PERMISSION_ERR);
    ResetCoreServiceMock();
}
 
/**
 * @tc.number   CallManagerService_MakeDirectCall_0400
 * @tc.name     test MakeDirectCall with multi sim and no valid default voice slot
 * @tc.desc     Multi sim without a valid default dial card should open the system dialog
 */
HWTEST_F(CallManagerServiceTest, CallManagerService_MakeDirectCall_0400, TestSize.Level1)
{
    SetCoreServiceMock(MakeIccAccountInfoList(2), -1);
    int32_t ret = service_->MakeDirectCall("10086");
    EXPECT_NE(ret, TELEPHONY_ERR_LOCAL_PTR_NULL);
    ResetCoreServiceMock();
}
 
/**
 * @tc.number   CallManagerService_MakeDirectCall_0500
 * @tc.name     test MakeDirectCall with no sim card
 * @tc.desc     No sim card should dial directly using the default voice slot id
 */
HWTEST_F(CallManagerServiceTest, CallManagerService_MakeDirectCall_0500, TestSize.Level1)
{
    SetCoreServiceMock({}, TEST_SLOT_ID);
    int32_t ret = service_->MakeDirectCall("10086");
    EXPECT_EQ(ret, TELEPHONY_ERR_PERMISSION_ERR);
    ResetCoreServiceMock();
}
 
/**
 * @tc.number   CallManagerService_MakeDirectCall_0600
 * @tc.name     test MakeDirectCall with multi sim and invalid default voice slot
 * @tc.desc     Multi sim without a valid default dial card should open the system dialog
 */
HWTEST_F(CallManagerServiceTest, CallManagerService_MakeDirectCall_0600, TestSize.Level1)
{
    SetCoreServiceMock(MakeIccAccountInfoList(3), -1);
    int32_t ret = service_->MakeDirectCall("10086");
    EXPECT_EQ(ret, TELEPHONY_ERR_PERMISSION_ERR);
    ResetCoreServiceMock();
}
 
#ifdef SUPPORT_DSOFTBUS
/**
 * @tc.number   CallManagerService_MakeDirectCall_0700
 * @tc.name     test MakeDirectCall when distributed communication is sink role and connected
 * @tc.desc     Sink role with connected status should fail directly
 */
HWTEST_F(CallManagerServiceTest, CallManagerService_MakeDirectCall_0700, TestSize.Level1)
{
    auto dcMgrInstance = DelayedSingleton<DistributedCommunicationManager>::GetInstance();
    ASSERT_NE(dcMgrInstance, nullptr);
    dcMgrInstance->role_ = DistributedRole::SINK;
    dcMgrInstance->status_ = DistributedStatus::CONNECT;
    int32_t ret = service_->MakeDirectCall("10086");
    EXPECT_EQ(ret, TELEPHONY_ERR_PERMISSION_ERR);
    dcMgrInstance->role_ = DistributedRole::UNKNOWN;
    dcMgrInstance->status_ = DistributedStatus::UNKNOWN;
}
#endif
 
/**
 * @tc.number   CallManagerService_StartContactsDialog_0100
 * @tc.name     test StartContactsDialog directly
 * @tc.desc     MakeDirectStartContactsDialogCallCore should open the dialog ability and return its result
 */
HWTEST_F(CallManagerServiceTest, CallManagerService_StartContactsDialog_0100, TestSize.Level1)
{
    int32_t ret = service_->StartContactsDialog("10086");
    EXPECT_NE(service_->dialogAbilityConnection_, nullptr);
    EXPECT_EQ(ret, TELEPHONY_SUCCESS);
}
 
/**
 * @tc.number   CallManagerService_DialCall_0500
 * @tc.name     test DialCall by system app without PLACE_CALL permission
 * @tc.desc     System application without PLACE_CALL permission should be rejected
 */
HWTEST_F(CallManagerServiceTest, CallManagerService_DialCall_0500, TestSize.Level1)
{
    DirectCallSystemAppNoPlaceCallToken token;
    std::u16string number = Str8ToStr16("10086");
    AppExecFwk::PacMap extras;
    extras.PutIntValue("accountId", TEST_SLOT_ID);
    extras.PutIntValue("callType", 0);
    extras.PutIntValue("videoState", 0);
    int32_t ret = service_->DialCall(number, extras);
    EXPECT_EQ(ret, TELEPHONY_ERR_ILLEGAL_USE_OF_SYSTEM_API);
}
 
/**
 * @tc.number   DialogAbilityConnection_OpenSystemDialog_0100
 * @tc.name     test OpenSystemDialog
 * @tc.desc     Connect extension should fail when ability manager is unavailable
 */
HWTEST_F(CallManagerServiceTest, DialogAbilityConnection_OpenSystemDialog_0100, TestSize.Level1)
{
    sptr<DialogAbilityConnection> dialog = sptr<DialogAbilityConnection>::MakeSptr();
    ASSERT_NE(dialog, nullptr);
    int32_t ret = dialog->OpenSystemDialog("com.ohos.contacts", "DirectDialogExtensionAbility", "10086", "com.test");
    EXPECT_EQ(ret, TELEPHONY_SUCCESS);
}
 
/**
 * @tc.number   DialogAbilityConnection_OnAbilityConnectDone_0100
 * @tc.name     test OnAbilityConnectDone with null remote object
 * @tc.desc     Null remote object should return early and keep context null
 */
HWTEST_F(CallManagerServiceTest, DialogAbilityConnection_OnAbilityConnectDone_0100, TestSize.Level1)
{
    sptr<DialogAbilityConnection> dialog = sptr<DialogAbilityConnection>::MakeSptr();
    ASSERT_NE(dialog, nullptr);
    AppExecFwk::ElementName element;
    dialog->OnAbilityConnectDone(element, nullptr, 0);
    EXPECT_EQ(dialog->dialogContext_, nullptr);
}
 
/**
 * @tc.number   DialogAbilityConnection_OnAbilityConnectDone_0200
 * @tc.name     test OnAbilityConnectDone with valid remote object
 * @tc.desc     Dialog data should be prepared and sent to the remote object
 */
HWTEST_F(CallManagerServiceTest, DialogAbilityConnection_OnAbilityConnectDone_0200, TestSize.Level1)
{
    sptr<DialogAbilityConnection> dialog = sptr<DialogAbilityConnection>::MakeSptr();
    ASSERT_NE(dialog, nullptr);
    dialog->OpenSystemDialog("com.ohos.contacts", "DirectDialogExtensionAbility", "10086", "com.test");
    sptr<IRemoteObject> remoteObject = new DirectCallMockRemoteObject(0);
    AppExecFwk::ElementName element;
    dialog->OnAbilityConnectDone(element, remoteObject, 0);
    EXPECT_NE(dialog->dialogContext_, nullptr);
}
 
/**
 * @tc.number   DialogAbilityConnection_OnAbilityConnectDone_0300
 * @tc.name     test OnAbilityConnectDone when remote SendRequest fails
 * @tc.desc     SendRequest failure should be handled and dialog context still assigned
 */
HWTEST_F(CallManagerServiceTest, DialogAbilityConnection_OnAbilityConnectDone_0300, TestSize.Level1)
{
    sptr<DialogAbilityConnection> dialog = sptr<DialogAbilityConnection>::MakeSptr();
    ASSERT_NE(dialog, nullptr);
    sptr<IRemoteObject> remoteObject = new DirectCallMockRemoteObject(TELEPHONY_ERR_FAIL);
    AppExecFwk::ElementName element;
    dialog->OnAbilityConnectDone(element, remoteObject, 0);
    EXPECT_NE(dialog->dialogContext_, nullptr);
}
 
/**
 * @tc.number   DialogAbilityConnection_OnAbilityDisconnectDone_0100
 * @tc.name     test OnAbilityDisconnectDone
 * @tc.desc     Disconnect done should destroy the dialog connection
 */
HWTEST_F(CallManagerServiceTest, DialogAbilityConnection_OnAbilityDisconnectDone_0100, TestSize.Level1)
{
    sptr<DialogAbilityConnection> dialog = sptr<DialogAbilityConnection>::MakeSptr();
    ASSERT_NE(dialog, nullptr);
    AppExecFwk::ElementName element;
    dialog->OnAbilityDisconnectDone(element, 0);
    EXPECT_EQ(dialog->dialogContext_, nullptr);
}
 
/**
 * @tc.number   DialogAbilityConnection_Destroy_0100
 * @tc.name     test Destroy
 * @tc.desc     Destroy should clear dialog context and disconnect the ability
 */
HWTEST_F(CallManagerServiceTest, DialogAbilityConnection_Destroy_0100, TestSize.Level1)
{
    sptr<DialogAbilityConnection> dialog = sptr<DialogAbilityConnection>::MakeSptr();
    ASSERT_NE(dialog, nullptr);
    int32_t ret = dialog->Destroy();
    EXPECT_NE(ret, TELEPHONY_SUCCESS);
    EXPECT_EQ(dialog->dialogContext_, nullptr);
}
} // namespace Telephony
} // namespace OHOS