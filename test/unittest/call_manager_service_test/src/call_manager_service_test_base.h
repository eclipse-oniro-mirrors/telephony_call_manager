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

#ifndef CALL_MANAGER_SERVICE_TEST_BASE_H
#define CALL_MANAGER_SERVICE_TEST_BASE_H

#define private public
#define protected public

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "call_manager_service.h"
#include "call_manager_connect.h"
#include "call_control_manager.h"
#include "call_status_manager.h"
#include "call_object_manager.h"
#include "cs_call.h"
#include "ims_call.h"
#include "telephony_errors.h"
#include "telephony_log_wrapper.h"
#include "accesstoken_kit.h"
#include "token_setproc.h"
#include "nativetoken_kit.h"

namespace OHOS {
namespace Telephony {
using namespace testing::ext;

class CallManagerServiceTest : public testing::Test {
public:
    static void SetUpTestCase()
    {
        constexpr int permissionNum = 7;
        const char *perms[permissionNum] = {
            "ohos.permission.PLACE_CALL",
            "ohos.permission.ANSWER_CALL",
            "ohos.permission.SET_TELEPHONY_STATE",
            "ohos.permission.GET_TELEPHONY_STATE",
            "ohos.permission.READ_CALL_LOG",
            "ohos.permission.WRITE_CALL_LOG",
            "ohos.permission.GET_CALL_TRANSFER_INFO"
        };
        NativeTokenInfoParams infoInstance = {
            .dcapsNum = 0,
            .permsNum = permissionNum,
            .aclsNum = 0,
            .dcaps = nullptr,
            .perms = perms,
            .acls = nullptr,
            .processName = "CallManagerServiceTest",
            .aplStr = "system_basic",
        };
        uint64_t tokenId = GetAccessTokenId(&infoInstance);
        SetSelfTokenID(tokenId);
        Security::AccessToken::AccessTokenKit::ReloadNativeTokenInfo();
    }

    static void TearDownTestCase() {}

    void SetUp()
    {
        service_ = std::make_shared<CallManagerService>();
    }

    void TearDown()
    {
        service_ = nullptr;
    }

    void SetCallControlManagerNull()
    {
        service_->callControlManagerPtr_ = nullptr;
    }

    void SetCallStatusManagerNull()
    {
        service_->callStatusManagerPtr_ = nullptr;
    }

    void SetBluetoothCallCallbackNull()
    {
        service_->bluetoothCallCallbackPtr_ = nullptr;
    }

    std::shared_ptr<CallManagerService> service_;
};

} // namespace Telephony
} // namespace OHOS

#endif // CALL_MANAGER_SERVICE_TEST_BASE_H
