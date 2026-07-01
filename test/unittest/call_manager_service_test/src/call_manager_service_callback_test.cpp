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
 * @tc.number   CallManagerService_RegisterCallBack_0100
 * @tc.name     test RegisterCallBack
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerServiceTest, CallManagerService_RegisterCallBack_0100, TestSize.Level1)
{
    sptr<ICallAbilityCallback> callback = nullptr;
    int32_t ret = service_->RegisterCallBack(callback);
    EXPECT_NE(ret, TELEPHONY_ERR_PERMISSION_ERR);
}

/**
 * @tc.number   CallManagerService_UnRegisterCallBack_0100
 * @tc.name     test UnRegisterCallBack
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerServiceTest, CallManagerService_UnRegisterCallBack_0100, TestSize.Level1)
{
    int32_t ret = service_->UnRegisterCallBack();
    EXPECT_NE(ret, TELEPHONY_ERR_PERMISSION_ERR);
}

/**
 * @tc.number   CallManagerService_RegisterVoipCallManagerCallback_0100
 * @tc.name     test RegisterVoipCallManagerCallback
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerServiceTest, CallManagerService_RegisterVoipCallManagerCallback_0100, TestSize.Level1)
{
    int32_t ret = service_->RegisterVoipCallManagerCallback();
    EXPECT_EQ(ret, 0);
}

/**
 * @tc.number   CallManagerService_UnRegisterVoipCallManagerCallback_0100
 * @tc.name     test UnRegisterVoipCallManagerCallback
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerServiceTest, CallManagerService_UnRegisterVoipCallManagerCallback_0100, TestSize.Level1)
{
    int32_t ret = service_->UnRegisterVoipCallManagerCallback();
    EXPECT_NE(ret, TELEPHONY_ERR_PERMISSION_ERR);
}

/**
 * @tc.number   CallManagerService_RegisterBluetoothCallManagerCallbackPtr_0100
 * @tc.name     test RegisterBluetoothCallManagerCallbackPtr
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerServiceTest, CallManagerService_RegisterBluetoothCallManagerCallbackPtr_0100, TestSize.Level1)
{
    std::string macAddress = "00:11:22:33:44:55";
    sptr<ICallStatusCallback> ret = service_->RegisterBluetoothCallManagerCallbackPtr(macAddress);
    EXPECT_NE(ret, nullptr);
}

/**
 * @tc.number   CallManagerService_RegisterBluetoothCallManagerCallbackPtr_0200
 * @tc.name     test RegisterBluetoothCallManagerCallbackPtr with empty macAddress
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerServiceTest, CallManagerService_RegisterBluetoothCallManagerCallbackPtr_0200, TestSize.Level1)
{
    std::string macAddress = "";
    sptr<ICallStatusCallback> ret = service_->RegisterBluetoothCallManagerCallbackPtr(macAddress);
    EXPECT_NE(ret, nullptr);
}

/**
 * @tc.number   CallManagerService_ObserverOnCallDetailsChange_0100
 * @tc.name     test ObserverOnCallDetailsChange
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerServiceTest, CallManagerService_ObserverOnCallDetailsChange_0100, TestSize.Level1)
{
    int32_t ret = service_->ObserverOnCallDetailsChange();
    EXPECT_EQ(ret, TELEPHONY_SUCCESS);
}

} // namespace Telephony
} // namespace OHOS
