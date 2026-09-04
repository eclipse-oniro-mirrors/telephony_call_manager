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

#define private public
#define protected public
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "transfer_control_callback.h"
#include "transfer_control_callback_stub.h"
#include "transfer_control_mock.h"
#include "call_manager_errors.h"
#include "telephony_errors.h"
#include "message_parcel.h"
#include "message_option.h"
#include "iremote_object.h"
#include "transfer_control_callback_ipc_interface_code.h"

namespace OHOS {
namespace Telephony {
using namespace testing::ext;

class TransferControlCallbackTest : public testing::Test {
public:
    void SetUp() override;
    void TearDown() override;
    static void SetUpTestCase();
    static void TearDownTestCase();
private:
    std::shared_ptr<TransferControlCallback> callback_;
};

void TransferControlCallbackTest::SetUp()
{
    callback_ = std::make_shared<TransferControlCallback>();
}

void TransferControlCallbackTest::TearDown()
{
    callback_.reset();
}

void TransferControlCallbackTest::SetUpTestCase() {}

void TransferControlCallbackTest::TearDownTestCase() {}

class TransferControlCallbackStubTest : public testing::Test {
public:
    void SetUp() override;
    void TearDown() override;
    static void SetUpTestCase();
    static void TearDownTestCase();
private:
    sptr<TransferControlCallback> stub_;
};

void TransferControlCallbackStubTest::SetUp()
{
    stub_ = new TransferControlCallback();
}

void TransferControlCallbackStubTest::TearDown() {}

void TransferControlCallbackStubTest::SetUpTestCase() {}

void TransferControlCallbackStubTest::TearDownTestCase() {}

/**
 * @tc.number: TransferControlCallback_SetProcessCallback_NullCallback
 * @tc.name: Test SetProcessCallback with null callback
 * @tc.desc: Verify that SetProcessCallback returns -1 when callback is nullptr
 */
HWTEST_F(TransferControlCallbackTest, TransferControlCallback_SetProcessCallback_NullCallback, TestSize.Level1)
{
    int32_t result = callback_->SetProcessCallback(nullptr);
    EXPECT_EQ(result, -1);
}

/**
 * @tc.number: TransferControlCallback_SetProcessCallback_Success
 * @tc.name: Test SetProcessCallback with valid callback
 * @tc.desc: Verify that SetProcessCallback returns 0 and stores the callback when it is valid
 */
HWTEST_F(TransferControlCallbackTest, TransferControlCallback_SetProcessCallback_Success, TestSize.Level1)
{
    auto mockControl = std::make_unique<MockTransferControl>();
    int32_t result = callback_->SetProcessCallback(std::move(mockControl));
    EXPECT_EQ(result, 0);
}

/**
 * @tc.number: TransferControlCallback_OnGetTransferResult_NullPtr
 * @tc.name: Test OnGetTransferResult with null callbackPtr_
 * @tc.desc: Verify that OnGetTransferResult returns -1 when callbackPtr_ is nullptr
 */
HWTEST_F(TransferControlCallbackTest, TransferControlCallback_OnGetTransferResult_NullPtr, TestSize.Level1)
{
    TransferCallInfo info;
    TransferResultInfo resultInfo;
    int32_t result = callback_->OnGetTransferResult(info, resultInfo);
    EXPECT_EQ(result, -1);
}

/**
 * @tc.number: TransferControlCallback_OnGetTransferResult_NeedTransfer
 * @tc.name: Test OnGetTransferResult when IsCallNeedTransfer returns true
 * @tc.desc: Verify that OnGetTransferResult returns 1 when IsCallNeedTransfer returns true
 */
HWTEST_F(TransferControlCallbackTest, TransferControlCallback_OnGetTransferResult_NeedTransfer, TestSize.Level1)
{
    auto mockControl = std::make_unique<MockTransferControl>();
    EXPECT_CALL(*mockControl, IsCallNeedTransfer(testing::_, testing::_)).WillOnce(testing::Return(true));
    callback_->SetProcessCallback(std::move(mockControl));
    TransferCallInfo info;
    TransferResultInfo resultInfo;
    int32_t result = callback_->OnGetTransferResult(info, resultInfo);
    EXPECT_EQ(result, 1);
}

/**
 * @tc.number: TransferControlCallback_OnGetTransferResult_NotNeedTransfer
 * @tc.name: Test OnGetTransferResult when IsCallNeedTransfer returns false
 * @tc.desc: Verify that OnGetTransferResult returns 0 when IsCallNeedTransfer returns false
 */
HWTEST_F(TransferControlCallbackTest, TransferControlCallback_OnGetTransferResult_NotNeedTransfer, TestSize.Level1)
{
    auto mockControl = std::make_unique<MockTransferControl>();
    EXPECT_CALL(*mockControl, IsCallNeedTransfer(testing::_, testing::_)).WillOnce(testing::Return(false));
    callback_->SetProcessCallback(std::move(mockControl));
    TransferCallInfo info;
    TransferResultInfo resultInfo;
    int32_t result = callback_->OnGetTransferResult(info, resultInfo);
    EXPECT_EQ(result, 0);
}

/**
 * @tc.number: TransferControlCallback_OnUpdateTransferCall_NullPtr
 * @tc.name: Test OnUpdateTransferCall with null callbackPtr_
 * @tc.desc: Verify that OnUpdateTransferCall returns -1 when callbackPtr_ is nullptr
 */
HWTEST_F(TransferControlCallbackTest, TransferControlCallback_OnUpdateTransferCall_NullPtr, TestSize.Level1)
{
    TransferCallInfo info;
    int32_t result = callback_->OnUpdateTransferCall(info);
    EXPECT_EQ(result, -1);
}

/**
 * @tc.number: TransferControlCallback_OnUpdateTransferCall_Success
 * @tc.name: Test OnUpdateTransferCall when NotifyCallState returns true
 * @tc.desc: Verify that OnUpdateTransferCall returns 0 when NotifyCallState returns true
 */
HWTEST_F(TransferControlCallbackTest, TransferControlCallback_OnUpdateTransferCall_Success, TestSize.Level1)
{
    auto mockControl = std::make_unique<MockTransferControl>();
    EXPECT_CALL(*mockControl, NotifyCallState(testing::_)).WillOnce(testing::Return(true));
    callback_->SetProcessCallback(std::move(mockControl));
    TransferCallInfo info;
    int32_t result = callback_->OnUpdateTransferCall(info);
    EXPECT_EQ(result, 0);
}

/**
 * @tc.number: TransferControlCallback_OnUpdateTransferCall_Fail
 * @tc.name: Test OnUpdateTransferCall when NotifyCallState returns false
 * @tc.desc: Verify that OnUpdateTransferCall returns -1 when NotifyCallState returns false
 */
HWTEST_F(TransferControlCallbackTest, TransferControlCallback_OnUpdateTransferCall_Fail, TestSize.Level1)
{
    auto mockControl = std::make_unique<MockTransferControl>();
    EXPECT_CALL(*mockControl, NotifyCallState(testing::_)).WillOnce(testing::Return(false));
    callback_->SetProcessCallback(std::move(mockControl));
    TransferCallInfo info;
    int32_t result = callback_->OnUpdateTransferCall(info);
    EXPECT_EQ(result, -1);
}

/**
 * @tc.number: TransferControlCallbackStub_OnRemoteRequest_DescriptorMismatch
 * @tc.name: Test OnRemoteRequest with mismatched descriptor
 * @tc.desc: Verify that OnRemoteRequest returns TELEPHONY_ERR_DESCRIPTOR_MISMATCH when descriptor does not match
 */
HWTEST_F(TransferControlCallbackStubTest, TransferControlCallbackStub_OnRemoteRequest_DescriptorMismatch,
    TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    data.WriteInterfaceToken(u"wrong.descriptor");
    uint32_t code = static_cast<uint32_t>(TransferControlInterfaceCode::IS_TRANSFER_CALL_ALLOW);
    int32_t result = stub_->OnRemoteRequest(code, data, reply, option);
    EXPECT_EQ(result, TELEPHONY_ERR_DESCRIPTOR_MISMATCH);
}

/**
 * @tc.number: TransferControlCallbackStub_OnRemoteRequest_ValidCode
 * @tc.name: Test OnRemoteRequest with valid code and correct descriptor
 * @tc.desc: Verify that OnRemoteRequest dispatches to the correct handler for a valid code
 */
HWTEST_F(TransferControlCallbackStubTest, TransferControlCallbackStub_OnRemoteRequest_ValidCode, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    data.WriteInterfaceToken(TransferControlCallbackStub::GetDescriptor());
    data.WriteString("00:11:22:33:44:55");
    data.WriteUint32(1);
    data.WriteInt32(static_cast<int32_t>(CallDirection::CALL_DIRECTION_UNKNOW));
    data.WriteInt32(static_cast<int32_t>(TelCallState::CALL_STATUS_UNKNOWN));
    data.WriteInt32(static_cast<int32_t>(CallType::TYPE_BLUETOOTH));
    uint32_t code = static_cast<uint32_t>(TransferControlInterfaceCode::IS_TRANSFER_CALL_ALLOW);
    int32_t result = stub_->OnRemoteRequest(code, data, reply, option);
    EXPECT_EQ(result, TELEPHONY_SUCCESS);
}

/**
 * @tc.number: TransferControlCallbackStub_OnRemoteRequest_InvalidCode
 * @tc.name: Test OnRemoteRequest with invalid code
 * @tc.desc: Verify that OnRemoteRequest falls through to IPCObjectStub for an invalid code
 */
HWTEST_F(TransferControlCallbackStubTest, TransferControlCallbackStub_OnRemoteRequest_InvalidCode, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    data.WriteInterfaceToken(TransferControlCallbackStub::GetDescriptor());
    uint32_t code = 999;
    int32_t result = stub_->OnRemoteRequest(code, data, reply, option);
    EXPECT_NE(result, TELEPHONY_SUCCESS);
}

/**
 * @tc.number: TransferControlCallbackStub_OnGetTransferResultInner_Success
 * @tc.name: Test OnGetTransferResultInner normal path
 * @tc.desc: Verify that OnGetTransferResultInner reads data, calls OnGetTransferResult, and writes reply correctly
 */
HWTEST_F(TransferControlCallbackStubTest, TransferControlCallbackStub_OnGetTransferResultInner_Success,
    TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    data.WriteString("00:11:22:33:44:55");
    data.WriteUint32(1);
    data.WriteInt32(static_cast<int32_t>(CallDirection::CALL_DIRECTION_UNKNOW));
    data.WriteInt32(static_cast<int32_t>(TelCallState::CALL_STATUS_UNKNOWN));
    data.WriteInt32(static_cast<int32_t>(CallType::TYPE_BLUETOOTH));
    int32_t result = stub_->OnGetTransferResultInner(data, reply);
    EXPECT_EQ(result, TELEPHONY_SUCCESS);
    int32_t replyResult = reply.ReadInt32();
    EXPECT_EQ(replyResult, -1);
}

/**
 * @tc.number: TransferControlCallbackStub_OnUpdateTransferCallInner_Success
 * @tc.name: Test OnUpdateTransferCallInner normal path
 * @tc.desc: Verify that OnUpdateTransferCallInner reads data, calls OnUpdateTransferCall, and writes reply correctly
 */
HWTEST_F(TransferControlCallbackStubTest, TransferControlCallbackStub_OnUpdateTransferCallInner_Success,
    TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    data.WriteString("00:11:22:33:44:55");
    data.WriteUint32(1);
    data.WriteInt32(static_cast<int32_t>(CallDirection::CALL_DIRECTION_UNKNOW));
    data.WriteInt32(static_cast<int32_t>(TelCallState::CALL_STATUS_UNKNOWN));
    data.WriteInt32(static_cast<int32_t>(CallType::TYPE_BLUETOOTH));
    int32_t result = stub_->OnUpdateTransferCallInner(data, reply);
    EXPECT_EQ(result, TELEPHONY_SUCCESS);
    int32_t replyResult = reply.ReadInt32();
    EXPECT_EQ(replyResult, -1);
}

/**
 * @tc.number: TransferControlCallback_OnDisconnectSco_NullPtr
 * @tc.name: Test OnDisconnectSco with null callbackPtr_
 * @tc.desc: Verify that OnDisconnectSco returns -1 when callbackPtr_ is nullptr
 */
HWTEST_F(TransferControlCallbackTest, TransferControlCallback_OnDisconnectSco_NullPtr, TestSize.Level1)
{
    std::string devMac = "00:11:22:33:44:55";
    int32_t result = callback_->OnDisconnectSco(devMac);
    EXPECT_EQ(result, -1);
}

/**
 * @tc.number: TransferControlCallback_OnDisconnectSco_Success
 * @tc.name: Test OnDisconnectSco when DisconnectSco returns true
 * @tc.desc: Verify that OnDisconnectSco returns 0 when DisconnectSco returns true
 */
HWTEST_F(TransferControlCallbackTest, TransferControlCallback_OnDisconnectSco_Success, TestSize.Level1)
{
    auto mockControl = std::make_unique<MockTransferControl>();
    EXPECT_CALL(*mockControl, DisconnectSco(testing::_)).WillOnce(testing::Return(true));
    callback_->SetProcessCallback(std::move(mockControl));
    std::string devMac = "00:11:22:33:44:55";
    int32_t result = callback_->OnDisconnectSco(devMac);
    EXPECT_EQ(result, 0);
}

/**
 * @tc.number: TransferControlCallback_OnDisconnectSco_Fail
 * @tc.name: Test OnDisconnectSco when DisconnectSco returns false
 * @tc.desc: Verify that OnDisconnectSco returns -1 when DisconnectSco returns false
 */
HWTEST_F(TransferControlCallbackTest, TransferControlCallback_OnDisconnectSco_Fail, TestSize.Level1)
{
    auto mockControl = std::make_unique<MockTransferControl>();
    EXPECT_CALL(*mockControl, DisconnectSco(testing::_)).WillOnce(testing::Return(false));
    callback_->SetProcessCallback(std::move(mockControl));
    std::string devMac = "00:11:22:33:44:55";
    int32_t result = callback_->OnDisconnectSco(devMac);
    EXPECT_EQ(result, -1);
}

/**
 * @tc.number: TransferControlCallbackStub_OnDisconnectScoInner_Success
 * @tc.name: Test OnDisconnectScoInner normal path
 * @tc.desc: Verify that OnDisconnectScoInner reads data, calls OnDisconnectSco, and writes reply correctly
 */
HWTEST_F(TransferControlCallbackStubTest, TransferControlCallbackStub_OnDisconnectScoInner_Success, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    data.WriteString("00:11:22:33:44:55");
    int32_t result = stub_->OnDisconnectScoInner(data, reply);
    EXPECT_EQ(result, TELEPHONY_SUCCESS);
    int32_t replyResult = reply.ReadInt32();
    EXPECT_EQ(replyResult, -1);
}

} // namespace Telephony
} // namespace OHOS
