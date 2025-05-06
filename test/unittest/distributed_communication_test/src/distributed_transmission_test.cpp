/*
 * Copyright (C) 2024-2024 Huawei Device Co., Ltd.
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
#include "transmission_manager.h"
#include "server_session.h"
#include "client_session.h"

namespace OHOS {
namespace Telephony {
using namespace testing::ext;

class SessionCallbackTest : public ISessionCallback {
public:
    void OnConnected() override {}
    void OnReceiveMsg(const char* data, uint32_t dataLen) override {}
};

class DistributedTransmissionTest : public testing::Test {
public:
    static void SetUpTestCase() {}
    static void TearDownTestCase() {}
    virtual void SetUp() {}
    virtual void TearDown() {}
};

/**
 * @tc.number   Telephony_DistributedTransmissionTest_001
 * @tc.name     test transmission manager
 * @tc.desc     Function test
 */
HWTEST_F(DistributedTransmissionTest, Telephony_DistributedTransmissionTest_001, Function | MediumTest | Level1)
{
    std::shared_ptr<ISessionCallback> callback = std::make_shared<SessionCallbackTest>();
    auto session = DelayedSingleton<TransmissionManager>::GetInstance()->CreateServerSession(callback);
    EXPECT_NE(session, nullptr);
    DelayedSingleton<TransmissionManager>::GetInstance()->OnBind(0);
    DelayedSingleton<TransmissionManager>::GetInstance()->OnReceiveMsg(0, nullptr, 0);
    DelayedSingleton<TransmissionManager>::GetInstance()->OnShutdown(0);
    session.reset();
    EXPECT_EQ(DelayedSingleton<TransmissionManager>::GetInstance()->session_.lock(), nullptr);
}

/**
 * @tc.number   Telephony_DistributedTransmissionTest_002
 * @tc.name     test client session
 * @tc.desc     Function test
 */
HWTEST_F(DistributedTransmissionTest, Telephony_DistributedTransmissionTest_002, Function | MediumTest | Level1)
{
    std::shared_ptr<ISessionCallback> callback = std::make_shared<SessionCallbackTest>();
    auto session = DelayedSingleton<TransmissionManager>::GetInstance()->CreateClientSession(callback);
    ASSERT_NE(session, nullptr);
    session->socket_ = 0;
    session->Connect("12345", SESSION_NAME, SESSION_NAME, 4194304);
    session->socket_ = INVALID_SOCKET_ID;
    session->Connect("", SESSION_NAME, SESSION_NAME, 4194304);
    session->OnSessionBind(0);
    session->Disconnect();
    session->OnSessionShutdown(0);
    EXPECT_EQ(session->socket_, INVALID_SOCKET_ID);

    std::string peerDevice = "";
    std::string localName = "localName";
    std::string peerName = "peerName";
    auto clientSession = std::make_shared<ClientSession>(callback);
    clientSession->clientSocket_ = INVALID_SOCKET_ID + 1;
    clientSession->Connect("", SESSION_NAME, SESSION_NAME, 4194304); // already connect
    clientSession->socket_ = INVALID_SOCKET_ID + 1;
    clientSession->Disconnect();
    clientSession->socket_ = 1;
    clientSession->clientSocket_ = 1;
    clientSession->OnSessionShutdown(1);
    EXPECT_EQ(clientSession->CreateSocket(peerDevice, localName, peerName), INVALID_SOCKET_ID);
}

/**
 * @tc.number   Telephony_DistributedTransmissionTest_003
 * @tc.name     test server session
 * @tc.desc     Function test
 */
HWTEST_F(DistributedTransmissionTest, Telephony_DistributedTransmissionTest_003, Function | MediumTest | Level1)
{
    std::shared_ptr<ISessionCallback> callback = std::make_shared<SessionCallbackTest>();
    auto session = std::make_shared<ServerSession>(callback);
    ASSERT_NE(session, nullptr);
    session->Create("", 4194304);
    session->socket_ = 0;
    session->Create(SESSION_NAME, 4194304);
    session->socket_ = INVALID_SOCKET_ID;
    session->Create(SESSION_NAME, 4194304);
    session->OnSessionBind(66);
    session->OnSessionShutdown(66);
    session->OnSessionShutdown(session->serverSocket_);
    session->serverSocket_ = 0;

    session->serverSocket_ = INVALID_SOCKET_ID + 1;
    EXPECT_NO_THROW(session->Create("123", 4194304));
    session->serverSocket_ = INVALID_SOCKET_ID;
    EXPECT_NO_THROW(session->Destroy());
    session->serverSocket_ = INVALID_SOCKET_ID + 1;
    EXPECT_NO_THROW(session->Destroy());
    EXPECT_EQ(session->serverSocket_, INVALID_SOCKET_ID);
}

/**
 * @tc.number   Telephony_DistributedTransmissionTest_004
 * @tc.name     test session adapter
 * @tc.desc     Function test
 */
HWTEST_F(DistributedTransmissionTest, Telephony_DistributedTransmissionTest_004, Function | MediumTest | Level1)
{
    std::shared_ptr<ISessionCallback> callback = std::make_shared<SessionCallbackTest>();
    auto session = DelayedSingleton<TransmissionManager>::GetInstance()->CreateServerSession(callback);
    ASSERT_NE(session, nullptr);
    session->socket_ = 0;
    session->SendMsg(nullptr, 0);
    session->socket_ = INVALID_SOCKET_ID;
    session->SendMsg(nullptr, 0);
    session->OnReceiveMsg(0, nullptr, 0);
    session->socket_ = 0;
    session->OnReceiveMsg(0, nullptr, 0);
    std::string name = "test";
    std::string network = "network";
    std::string pkgName = "pkgName";
    PeerSocketInfo info = {
        .name = const_cast<char *>(name.c_str()),
        .networkId = const_cast<char *>(network.c_str()),
        .pkgName = const_cast<char *>(pkgName.c_str()),
        .dataType = DATA_TYPE_RAW_STREAM_ENCRYPED
    };
    SessionAdapter::OnBind(0, info);
    SessionAdapter::OnShutdown(0, static_cast<ShutdownReason>(0));
    SessionAdapter::OnBytes(0, nullptr, 0);
    SessionAdapter::OnError(0, 0);
    session->socket_ = 0;
    EXPECT_TRUE(session->IsReady());
}

} // namespace Telephony
} // namespace OHOS
