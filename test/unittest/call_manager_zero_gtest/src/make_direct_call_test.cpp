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
 
#define PRIVATE public
#define PROTECTED public
#include <gtest/gtest.h>
#include <gmock/gmock.h>
 
#include "call_manager_client.h"
#include "call_manager_errors.h"
#include "call_manager_proxy.h"
#include "call_manager_service_proxy.h"
#include "i_call_manager_service.h"
#include "iremote_object.h"
#include "message_option.h"
#include "message_parcel.h"
#include "telephony_errors.h"
 
namespace OHOS {
namespace Telephony {
namespace {
class MakeDirectCallMockRemoteObject : public IRemoteObject {
public:
    explicit MakeDirectCallMockRemoteObject(int32_t sendResult = 0) : IRemoteObject(u"MakeDirectCallMockRemoteObject"),
        sendResult_(sendResult) {}
 
    int SendRequest(uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option)
    {
        if (sendResult_ != 0) {
            return sendResult_;
        }
        reply.WriteInt32(0);
        return 0;
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
 
using namespace testing::ext;
class MakeDirectCallTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};
 
void MakeDirectCallTest::SetUpTestCase() {}
 
void MakeDirectCallTest::TearDownTestCase() {}
 
void MakeDirectCallTest::SetUp() {}
 
void MakeDirectCallTest::TearDown() {}
 
/**
 * @tc.number   MakeDirectCallTest_ServiceProxy_EmptyNumber
 * @tc.name     CallManagerServiceProxy MakeDirectCall with empty number
 * @tc.desc     Empty number should be rejected with TELEPHONY_ERR_ARGUMENT_INVALID
 */
HWTEST_F(MakeDirectCallTest, MakeDirectCallTest_ServiceProxy_EmptyNumber, TestSize.Level1)
{
    sptr<IRemoteObject> impl;
    CallManagerServiceProxy proxy(impl);
    int32_t result = proxy.MakeDirectCall("");
    EXPECT_EQ(result, TELEPHONY_ERR_ARGUMENT_INVALID);
}
 
/**
 * @tc.number   MakeDirectCallTest_ServiceProxy_SendFail
 * @tc.name     CallManagerServiceProxy MakeDirectCall with null remote
 * @tc.desc     Null remote should cause SendRequest to fail
 */
HWTEST_F(MakeDirectCallTest, MakeDirectCallTest_ServiceProxy_SendFail, TestSize.Level1)
{
    sptr<IRemoteObject> impl;
    CallManagerServiceProxy proxy(impl);
    int32_t result = proxy.MakeDirectCall("10086");
    EXPECT_EQ(result, TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL);
}
 
/**
 * @tc.number   MakeDirectCallTest_ServiceProxy_Success
 * @tc.name     CallManagerServiceProxy MakeDirectCall success path
 * @tc.desc     Valid remote should forward the request and read the result
 */
HWTEST_F(MakeDirectCallTest, MakeDirectCallTest_ServiceProxy_Success, TestSize.Level1)
{
    sptr<IRemoteObject> remoteObject = new MakeDirectCallMockRemoteObject(0);
    CallManagerServiceProxy proxy(remoteObject);
    int32_t result = proxy.MakeDirectCall("10086");
    EXPECT_EQ(result, TELEPHONY_SUCCESS);
}
 
/**
 * @tc.number   MakeDirectCallTest_Proxy_ReconnectFail
 * @tc.name     CallManagerProxy MakeDirectCall when reconnect fails
 * @tc.desc     Reconnect failure should return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL
 */
HWTEST_F(MakeDirectCallTest, MakeDirectCallTest_Proxy_ReconnectFail, TestSize.Level1)
{
    auto proxy = DelayedSingleton<CallManagerProxy>::GetInstance();
    ASSERT_NE(proxy, nullptr);
    auto originalService = proxy->callManagerServicePtr_;
    proxy->callManagerServicePtr_ = nullptr;
    proxy->systemAbilityId_ = 0x7FFFFFFF;
    int32_t result = proxy->MakeDirectCall("10086");
    EXPECT_EQ(result, TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL);
    proxy->callManagerServicePtr_ = originalService;
}
 
/**
 * @tc.number   MakeDirectCallTest_Proxy_ServiceFail
 * @tc.name     CallManagerProxy MakeDirectCall when service returns failure
 * @tc.desc     Failure from the service proxy should be propagated
 */
HWTEST_F(MakeDirectCallTest, MakeDirectCallTest_Proxy_ServiceFail, TestSize.Level1)
{
    auto proxy = DelayedSingleton<CallManagerProxy>::GetInstance();
    ASSERT_NE(proxy, nullptr);
    auto originalService = proxy->callManagerServicePtr_;
    sptr<IRemoteObject> remoteObject = new MakeDirectCallMockRemoteObject(-1);
    proxy->callManagerServicePtr_ = iface_cast<ICallManagerService>(remoteObject);
    int32_t result = proxy->MakeDirectCall("10086");
    EXPECT_NE(result, TELEPHONY_SUCCESS);
    proxy->callManagerServicePtr_ = originalService;
}
 
/**
 * @tc.number   MakeDirectCallTest_Proxy_Success
 * @tc.name     CallManagerProxy MakeDirectCall success path
 * @tc.desc     Successful service proxy response should return TELEPHONY_SUCCESS
 */
HWTEST_F(MakeDirectCallTest, MakeDirectCallTest_Proxy_Success, TestSize.Level1)
{
    auto proxy = DelayedSingleton<CallManagerProxy>::GetInstance();
    ASSERT_NE(proxy, nullptr);
    auto originalService = proxy->callManagerServicePtr_;
    sptr<IRemoteObject> remoteObject = new MakeDirectCallMockRemoteObject(0);
    proxy->callManagerServicePtr_ = iface_cast<ICallManagerService>(remoteObject);
    int32_t result = proxy->MakeDirectCall("10086");
    EXPECT_EQ(result, TELEPHONY_SUCCESS);
    proxy->callManagerServicePtr_ = originalService;
}
 
/**
 * @tc.number   MakeDirectCallTest_Client_Forward
 * @tc.name     CallManagerClient MakeDirectCall forwards to proxy
 * @tc.desc     After Init, MakeDirectCall should forward to the proxy instance
 */
HWTEST_F(MakeDirectCallTest, MakeDirectCallTest_Client_Forward, TestSize.Level1)
{
    auto client = DelayedSingleton<CallManagerClient>::GetInstance();
    ASSERT_NE(client, nullptr);
    client->Init(4005);
    auto proxy = DelayedSingleton<CallManagerProxy>::GetInstance();
    ASSERT_NE(proxy, nullptr);
    auto originalService = proxy->callManagerServicePtr_;
    sptr<IRemoteObject> remoteObject = new MakeDirectCallMockRemoteObject(0);
    proxy->callManagerServicePtr_ = iface_cast<ICallManagerService>(remoteObject);
    int32_t result = client->MakeDirectCall("10086");
    EXPECT_EQ(result, TELEPHONY_SUCCESS);
    proxy->callManagerServicePtr_ = originalService;
}
} // namespace Telephony
} // namespace OHOS