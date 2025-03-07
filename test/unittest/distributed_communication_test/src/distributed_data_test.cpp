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
#include "cJSON.h"
#include "cs_call.h"
#include "ims_call.h"
#include "call_object_manager.h"
#include "distributed_data_controller.h"
#include "distributed_data_sink_controller.h"
#include "distributed_data_source_controller.h"
#include "transmission_manager.h"

namespace OHOS {
namespace Telephony {
using namespace testing::ext;

class DataSessionCallbackTest : public ISessionCallback {
public:
    void OnConnected() override {}
    void OnReceiveMsg(const char* data, uint32_t dataLen) override {}
};

class DistributedDataTest : public testing::Test {
public:
    static void SetUpTestCase() {}
    static void TearDownTestCase() {}
    virtual void SetUp() {}
    virtual void TearDown() {}
};

/**
 * @tc.number   Telephony_DistributedDataTest_001
 * @tc.name     test data sink controller destroyed call
 * @tc.desc     Function test
 */
HWTEST_F(DistributedDataTest, Telephony_DistributedDataTest_001, Function | MediumTest | Level1)
{
    auto controller = std::make_shared<DistributedDataSinkController>();
    sptr<CallBase> call = nullptr;
    std::string devId = "";
    controller->OnCallCreated(call, devId);
    controller->ProcessCallInfo(call, DistributedDataType::NAME);
    CallObjectManager::callObjectPtrList_.emplace_back(nullptr);
    controller->OnCallDestroyed();
    controller->OnConnected();
    CallObjectManager::callObjectPtrList_.clear();
    std::shared_ptr<ISessionCallback> callback = std::make_shared<DataSessionCallbackTest>();
    controller->session_ = DelayedSingleton<TransmissionManager>::GetInstance()->CreateServerSession(callback);
    controller->OnCallDestroyed();
    EXPECT_TRUE(controller->queryInfo_.empty());
}

/**
 * @tc.number   Telephony_DistributedDataTest_002
 * @tc.name     test data sink controller handle recv msg
 * @tc.desc     Function test
 */
HWTEST_F(DistributedDataTest, Telephony_DistributedDataTest_002, Function | MediumTest | Level1)
{
    auto controller = std::make_shared<DistributedDataSinkController>();
    ASSERT_NO_THROW(controller->HandleRecvMsg(0, nullptr));
    std::string json = "{ \"dataType\": 101 }";
    cJSON *msg = cJSON_Parse(json.c_str());
    ASSERT_NO_THROW(controller->HandleRecvMsg(101, msg));
    ASSERT_NO_THROW(controller->HandleRecvMsg(106, msg));
    ASSERT_NO_THROW(controller->HandleRecvMsg(0, msg));
    cJSON_Delete(msg);
}

/**
 * @tc.number   Telephony_DistributedDataTest_003
 * @tc.name     test data sink controller connect remote
 * @tc.desc     Function test
 */
HWTEST_F(DistributedDataTest, Telephony_DistributedDataTest_003, Function | MediumTest | Level1)
{
    auto controller = std::make_shared<DistributedDataSinkController>();
    std::string devId = "";
    ASSERT_NO_THROW(controller->ConnectRemote(devId));
    ASSERT_NO_THROW(controller->ConnectRemote(devId)); // check session is not null
}

/**
 * @tc.number   Telephony_DistributedDataTest_004
 * @tc.name     test data sink controller check local data
 * @tc.desc     Function test
 */
HWTEST_F(DistributedDataTest, Telephony_DistributedDataTest_004, Function | MediumTest | Level1)
{
    auto controller = std::make_shared<DistributedDataSinkController>();
    sptr<CallBase> call = nullptr;
    ASSERT_NO_THROW(controller->CheckLocalData(call, DistributedDataType::NAME));
    DialParaInfo paraInfo;
    call = (std::make_unique<IMSCall>(paraInfo)).release();
    call->accountNumber_ = "";
    ASSERT_NO_THROW(controller->CheckLocalData(call, DistributedDataType::NAME));
    call->accountNumber_ = "13512345678";
    call->numberLocation_ = "default";
    call->contactInfo_.name = "";
    ASSERT_NO_THROW(controller->CheckLocalData(call, DistributedDataType::NAME));
    ASSERT_NO_THROW(controller->CheckLocalData(call, DistributedDataType::LOCATION));
}

/**
 * @tc.number   Telephony_DistributedDataTest_005
 * @tc.name     test data sink controller send query req
 * @tc.desc     Function test
 */
HWTEST_F(DistributedDataTest, Telephony_DistributedDataTest_005, Function | MediumTest | Level1)
{
    auto controller = std::make_shared<DistributedDataSinkController>();
    std::shared_ptr<ISessionCallback> callback = std::make_shared<DataSessionCallbackTest>();
    controller->session_ = DelayedSingleton<TransmissionManager>::GetInstance()->CreateServerSession(callback);
    ASSERT_NO_THROW(controller->SendDataQueryReq());
    controller->session_->socket_ = 0;
    controller->queryInfo_["13512345678"] = 2;
    ASSERT_NO_THROW(controller->SendDataQueryReq());
    EXPECT_EQ(controller->queryInfo_["13512345678"], 0);
}

/**
 * @tc.number   Telephony_DistributedDataTest_006
 * @tc.name     test data sink controller handle data query response
 * @tc.desc     Function test
 */
HWTEST_F(DistributedDataTest, Telephony_DistributedDataTest_006, Function | MediumTest | Level1)
{
    auto controller = std::make_shared<DistributedDataSinkController>();
    cJSON *msg = cJSON_Parse("{ \"dataType\": 101 }");
    ASSERT_NO_THROW(controller->HandleDataQueryRsp(msg));
    cJSON_Delete(msg);

    msg = cJSON_Parse("{ \"itemType\": 0 }");
    ASSERT_NO_THROW(controller->HandleDataQueryRsp(msg));
    cJSON_Delete(msg);

    msg = cJSON_Parse("{ \"itemType\": 0, \"num\": \"123456\" }");
    ASSERT_NO_THROW(controller->HandleDataQueryRsp(msg));
    DialParaInfo paraInfo;
    sptr<CallBase> call = (std::make_unique<IMSCall>(paraInfo)).release();
    call->accountNumber_ = "123456";
    CallObjectManager::callObjectPtrList_.emplace_back(call);
    ASSERT_NO_THROW(controller->HandleDataQueryRsp(msg));
    cJSON_Delete(msg);

    msg = cJSON_Parse("{ \"itemType\": 1, \"num\": \"123456\" }");
    ASSERT_NO_THROW(controller->HandleDataQueryRsp(msg));
    cJSON_Delete(msg);
    CallObjectManager::callObjectPtrList_.clear();
}

/**
 * @tc.number   Telephony_DistributedDataTest_007
 * @tc.name     test data sink controller update call info
 * @tc.desc     Function test
 */
HWTEST_F(DistributedDataTest, Telephony_DistributedDataTest_007, Function | MediumTest | Level1)
{
    DialParaInfo paraInfo;
    sptr<CallBase> call = (std::make_unique<IMSCall>(paraInfo)).release();
    call->accountNumber_ = "123456";

    auto controller = std::make_shared<DistributedDataSinkController>();
    cJSON *msg = cJSON_Parse("{ \"itemType\": 0, \"num\": \"123456\" }");
    ASSERT_NO_THROW(controller->UpdateCallName(call, msg));
    cJSON_Delete(msg);

    msg = cJSON_Parse("{ \"itemType\": 0, \"num\": \"123456\", \"name\": \"\" }");
    ASSERT_NO_THROW(controller->UpdateCallName(call, msg));
    cJSON_Delete(msg);

    msg = cJSON_Parse("{ \"itemType\": 0, \"num\": \"123456\", \"name\": \"test\" }");
    ASSERT_NO_THROW(controller->UpdateCallName(call, msg));
    cJSON_Delete(msg);

    msg = cJSON_Parse("{ \"itemType\": 0, \"num\": \"123456\" }");
    ASSERT_NO_THROW(controller->UpdateCallLocation(call, msg));
    cJSON_Delete(msg);

    msg = cJSON_Parse("{ \"itemType\": 0, \"num\": \"123456\", \"location\": \"test\" }");
    ASSERT_NO_THROW(controller->UpdateCallLocation(call, msg));
    cJSON_Delete(msg);
}

/**
 * @tc.number   Telephony_DistributedDataTest_008
 * @tc.name     test data sink controller send current data query req
 * @tc.desc     Function test
 */
HWTEST_F(DistributedDataTest, Telephony_DistributedDataTest_008, Function | MediumTest | Level1)
{
    DialParaInfo paraInfo;
    sptr<CallBase> call1 = (std::make_unique<IMSCall>(paraInfo)).release();
    call1->callType_ = CallType::TYPE_OTT;
    sptr<CallBase> call2 = (std::make_unique<IMSCall>(paraInfo)).release();
    call2->callType_ = CallType::TYPE_IMS;
    CallObjectManager::callObjectPtrList_.emplace_back(nullptr);
    CallObjectManager::callObjectPtrList_.emplace_back(call1);
    CallObjectManager::callObjectPtrList_.emplace_back(call2);
    sptr<CallBase> call = new IMSCall(paraInfo);
    call->callType_ = CallType::TYPE_IMS;
    CallObjectManager::callObjectPtrList_.push_back(call);
    auto controller = std::make_shared<DistributedDataSinkController>();
    ASSERT_NO_THROW(controller->SendCurrentDataQueryReq());
    std::shared_ptr<ISessionCallback> callback = std::make_shared<DataSessionCallbackTest>();
    controller->session_ = DelayedSingleton<TransmissionManager>::GetInstance()->CreateServerSession(callback);
    ASSERT_NO_THROW(controller->SendCurrentDataQueryReq());
    CallObjectManager::callObjectPtrList_.clear();
}

/**
 * @tc.number   Telephony_DistributedDataTest_009
 * @tc.name     test data sink controller handle current data response
 * @tc.desc     Function test
 */
HWTEST_F(DistributedDataTest, Telephony_DistributedDataTest_009, Function | MediumTest | Level1)
{
    auto controller = std::make_shared<DistributedDataSinkController>();
    ASSERT_NO_THROW(controller->HandleCurrentDataQueryRsp(nullptr));

    cJSON *msg = cJSON_Parse("{ \"itemType\": 0, \"num\": \"123456\" }");
    ASSERT_NO_THROW(controller->HandleCurrentDataQueryRsp(msg));
    cJSON_Delete(msg);

    msg = cJSON_Parse("{ \"itemType\": 0, \"num\": \"123456\", \"direction\": 3 }");
    ASSERT_NO_THROW(controller->HandleCurrentDataQueryRsp(msg));
    cJSON_Delete(msg);

    DialParaInfo paraInfo;
    sptr<CallBase> call = (std::make_unique<IMSCall>(paraInfo)).release();
    call->accountNumber_ = "123456";
    CallObjectManager::callObjectPtrList_.emplace_back(call);

    msg = cJSON_Parse("{ \"itemType\": 0, \"num\": \"123456\", \"direction\": 0 }");
    ASSERT_NO_THROW(controller->HandleCurrentDataQueryRsp(msg));
    std::string num = "123";
    std::string reqMsg = controller->CreateCurrentDataReqMsg(num);
    EXPECT_FALSE(reqMsg.empty());
    cJSON_Delete(msg);
}

/**
 * @tc.number   Telephony_DistributedDataTest_010
 * @tc.name     test normal function
 * @tc.desc     Function test
 */
HWTEST_F(DistributedDataTest, Telephony_DistributedDataTest_010, Function | MediumTest | Level1)
{
    std::string num = "number1";
    std::string data = "data";
    std::string numLocation = "location_one";
    DistributedDataType type = DistributedDataType::LOCATION;
    std::string devId = "UnitTestDeviceId";
    std::string devName = "UnitTestDeviceName";
    std::string json = "{ \"dataType\": 101 }";
    cJSON *msg = cJSON_Parse(json.c_str());
    DialParaInfo mDialParaInfo;
    sptr<CallBase> csCall = new CSCall(mDialParaInfo);
    AudioDeviceType deviceType = AudioDeviceType::DEVICE_DISTRIBUTED_PHONE;
    auto sourceController = std::make_shared<DistributedDataSourceController>();
    ASSERT_NO_THROW(sourceController->OnDeviceOnline(devId, devName, deviceType));
    ASSERT_NO_THROW(sourceController->OnDeviceOffline(devId, devName, deviceType));
    ASSERT_NO_THROW(sourceController->OnCallDestroyed());
    ASSERT_NO_THROW(sourceController->OnConnected());
    ASSERT_NO_THROW(sourceController->HandleRecvMsg(0, nullptr));
    ASSERT_NO_THROW(sourceController->HandleRecvMsg(static_cast<int32_t>(DistributedMsgType::DATA_REQ), msg));
    ASSERT_NO_THROW(sourceController->HandleRecvMsg(static_cast<int32_t>(DistributedMsgType::CURRENT_DATA_REQ), msg));
    ASSERT_NO_THROW(sourceController->HandleRecvMsg(static_cast<int32_t>(DistributedMsgType::MUTE), msg));
    ASSERT_NO_THROW(sourceController->SaveLocalData(num, type, data));
    ASSERT_NO_THROW(sourceController->SaveLocalData(num, type, data));
    cJSON_Delete(msg);

    type = DistributedDataType::NAME;
    ASSERT_NO_THROW(csCall->SetAccountNumber(""));
    ASSERT_NO_THROW(sourceController->SaveLocalData(csCall, type)); // Account number empty
    ASSERT_NO_THROW(csCall->SetAccountNumber(num));
    ASSERT_NO_THROW(sourceController->SaveLocalData(csCall, type));

    type = DistributedDataType::LOCATION;
    ASSERT_NO_THROW(csCall->SetNumberLocation(numLocation));
    ASSERT_NO_THROW(sourceController->SaveLocalData(csCall, type));
    ASSERT_NO_THROW(sourceController->SendLocalDataRsp());
    ASSERT_NO_THROW(sourceController->OnDeviceOnline(devId, devName, deviceType));
    ASSERT_NO_THROW(sourceController->SendLocalDataRsp());
    std::string ret = sourceController->CreateDataRspMsg(DistributedMsgType::DATA_REQ, 1, num, devName, data);
    ASSERT_FALSE(ret.empty());
}

/**
 * @tc.number   Telephony_DistributedDataTest_011
 * @tc.name     test data msg
 * @tc.desc     Function test
 */
HWTEST_F(DistributedDataTest, Telephony_DistributedDataTest_011, Function | MediumTest | Level1)
{
    bool isMuted = true;
    int32_t direction = 0;
    std::string num = "number_1";
    cJSON *msg = cJSON_Parse("{ \"testKey\": 0 }");
    auto sourceController = std::make_shared<DistributedDataSourceController>();
    ASSERT_NO_THROW(sourceController->HandleDataQueryMsg(msg));
    cJSON_Delete(msg);

    msg = cJSON_Parse("{ \"itemType\": 0 }");
    ASSERT_NO_THROW(sourceController->HandleCurrentDataQueryMsg(msg));
    ASSERT_NO_THROW(sourceController->HandleDataQueryMsg(msg));
    cJSON_Delete(msg);

    msg = cJSON_Parse("{ \"itemType\": 0, \"num\": \"123456\" }");
    ASSERT_NO_THROW(sourceController->HandleCurrentDataQueryMsg(msg));
    ASSERT_NO_THROW(sourceController->HandleDataQueryMsg(msg));
    ASSERT_NO_THROW(sourceController->HandleDataQueryMsg(msg));
    std::string rspMsg = sourceController->CreateCurrentDataRspMsg(num, isMuted, direction);
    ASSERT_FALSE(rspMsg.empty());
    cJSON_Delete(msg);
}

/**
 * @tc.number   Telephony_DistributedDataTest_012
 * @tc.name     test data controller receive msg
 * @tc.desc     Function test
 */
HWTEST_F(DistributedDataTest, Telephony_DistributedDataTest_012, Function | MediumTest | Level1)
{
    auto controller = std::make_shared<DistributedDataSinkController>();
    ASSERT_NO_THROW(controller->OnReceiveMsg(nullptr, DISTRIBUTED_MAX_RECV_DATA_LEN + 1));
    std::string data = "test";
    ASSERT_NO_THROW(controller->OnReceiveMsg(data.c_str(), data.length()));

    data = "{ \"itemType\": 0, \"num\": \"123456\" }";
    ASSERT_NO_THROW(controller->OnReceiveMsg(data.c_str(), data.length()));

    data = "{ \"dataType\": 104, \"itemType\": 0, \"num\": \"123456\" }";
    ASSERT_NO_THROW(controller->OnReceiveMsg(data.c_str(), data.length()));

    data = "{ \"dataType\": 102, \"itemType\": 0, \"num\": \"123456\" }";
    ASSERT_NO_THROW(controller->OnReceiveMsg(data.c_str(), data.length()));

    data = "{ \"dataType\": 0, \"itemType\": 0, \"num\": \"123456\" }";
    ASSERT_NO_THROW(controller->OnReceiveMsg(data.c_str(), data.length()));
}

/**
 * @tc.number   Telephony_DistributedDataTest_013
 * @tc.name     test data controller mute
 * @tc.desc     Function test
 */
HWTEST_F(DistributedDataTest, Telephony_DistributedDataTest_013, Function | MediumTest | Level1)
{
    auto controller = std::make_shared<DistributedDataSinkController>();
    controller->session_ = nullptr;
    ASSERT_NO_THROW(controller->SetMuted(true));
    ASSERT_NO_THROW(controller->MuteRinger());

    std::shared_ptr<ISessionCallback> callback = std::make_shared<DataSessionCallbackTest>();
    controller->session_ = DelayedSingleton<TransmissionManager>::GetInstance()->CreateServerSession(callback);
    ASSERT_NO_THROW(controller->SetMuted(true));
    ASSERT_NO_THROW(controller->MuteRinger());
}

/**
 * @tc.number   Telephony_DistributedDataTest_014
 * @tc.name     test get json value
 * @tc.desc     Function test
 */
HWTEST_F(DistributedDataTest, Telephony_DistributedDataTest_014, Function | MediumTest | Level1)
{
    auto controller = std::make_shared<DistributedDataSinkController>();
    cJSON *msg = cJSON_Parse("{ \"test\": 0 }");
    std::string name = "test";
    std::string stringValue = "";
    int32_t intValue = 0;
    cJSON *dataJson = cJSON_GetObjectItem(msg, name.c_str());
    EXPECT_TRUE(controller->GetInt32Value(msg, name, intValue));
    EXPECT_FALSE(controller->GetStringValue(msg, name, stringValue));
    cJSON_Delete(msg);

    msg = cJSON_Parse("{ \"test\": \"hello\" }");
    EXPECT_FALSE(controller->GetInt32Value(msg, name, intValue));
    EXPECT_TRUE(controller->GetStringValue(msg, name, stringValue));
    bool boolValue = false;
    EXPECT_FALSE(controller->GetBoolValue(msg, name, boolValue));
    cJSON_Delete(msg);

    msg = cJSON_Parse("{ \"test\": true }");
    EXPECT_TRUE(controller->GetBoolValue(msg, name, boolValue));
    cJSON_Delete(msg);
}

/**
 * @tc.number   Telephony_DistributedDataTest_015
 * @tc.name     test handle mute
 * @tc.desc     Function test
 */
HWTEST_F(DistributedDataTest, Telephony_DistributedDataTest_015, Function | MediumTest | Level1)
{
    auto controller = std::make_shared<DistributedDataSinkController>();
    cJSON *msg = cJSON_Parse("{ \"test\": 0 }");
    ASSERT_NO_THROW(controller->HandleMuted(msg));
    cJSON_Delete(msg);

    msg = cJSON_Parse("{ \"mute\": true }");
    ASSERT_NO_THROW(controller->HandleMuted(msg));
    cJSON_Delete(msg);
}

/**
 * @tc.number   Telephony_DistributedDataTest_016
 * @tc.name     test data msg
 * @tc.desc     Function test
 */
HWTEST_F(DistributedDataTest, Telephony_DistributedDataTest_016, Function | MediumTest | Level1)
{
    DialParaInfo mDialParaInfo;
    sptr<CallBase> imsCall = nullptr;
    ContactInfo info;
    info.name = "name";
    auto sourceController = std::make_shared<DistributedDataSourceController>();
    ASSERT_NO_THROW(sourceController->SaveLocalData(imsCall, DistributedDataType::LOCATION));
    imsCall = new IMSCall(mDialParaInfo);
    imsCall->SetCallerInfo(info);
    imsCall->SetAccountNumber("number");
    ASSERT_NO_THROW(sourceController->SaveLocalData(imsCall, DistributedDataType::NAME));
    std::shared_ptr<ISessionCallback> callback = std::make_shared<DataSessionCallbackTest>();
    sourceController->session_ = DelayedSingleton<TransmissionManager>::GetInstance()->CreateServerSession(callback);
    sourceController->session_->socket_ = INVALID_SOCKET_ID + 1; // session is ready
    ASSERT_NO_THROW(sourceController->SendLocalDataRsp()); // both localInfo_ and queryInfo_ empty
    sourceController->queryInfo_["name_1"] = 1;
    sourceController->queryInfo_["name_2"] = 2;
    std::map<uint32_t, std::string> localInfo;
    sourceController->localInfo_["name_1"] = localInfo;
    sourceController->localInfo_["name_2"] = localInfo;
    ASSERT_NO_THROW(sourceController->SendLocalDataRsp()); // localInfo_["name"] is empty
    localInfo[0] = "zero";
    localInfo[1] = "one";
    localInfo[2] = "two";
    sourceController->localInfo_["name_1"] = localInfo;
    sourceController->localInfo_["name_2"] = localInfo;
    ASSERT_NO_THROW(sourceController->SendLocalDataRsp()); // both localInfo_ and queryInfo_ not empty
    sourceController->localInfo_.clear();
    sourceController->queryInfo_.clear();
    cJSON *msg = cJSON_Parse("{ \"num\": \"123\" }");
    imsCall->SetAccountNumber("123");
    CallObjectManager::callObjectPtrList_.push_back(imsCall);
    ASSERT_NO_THROW(sourceController->HandleCurrentDataQueryMsg(msg));
    CallObjectManager::callObjectPtrList_.clear();
}

/**
 * @tc.number   Telephony_DistributedDataTest_017
 * @tc.name     test data msg
 * @tc.desc     Function test
 */
HWTEST_F(DistributedDataTest, Telephony_DistributedDataTest_017, Function | MediumTest | Level1)
{
    DialParaInfo mDialParaInfo;
    AudioDeviceType deviceType = AudioDeviceType::DEVICE_DISTRIBUTED_PHONE;
    std::string devId = "UnitTestDeviceId";
    std::string devName = "UnitTestDeviceName";
    sptr<CallBase> imsCall = new IMSCall(mDialParaInfo);
    imsCall->callType_ = CallType::TYPE_IMS;
    auto sourceController = std::make_shared<DistributedDataSourceController>();
    std::shared_ptr<ISessionCallback> callback = std::make_shared<DataSessionCallbackTest>();
    sourceController->session_ = DelayedSingleton<TransmissionManager>::GetInstance()->CreateServerSession(callback);
    ASSERT_NO_THROW(sourceController->OnDeviceOnline(devId, devName, deviceType));
    sourceController->session_ = nullptr;
    CallObjectManager::callObjectPtrList_.emplace_back(nullptr);
    CallObjectManager::callObjectPtrList_.emplace_back(imsCall);
    ASSERT_NO_THROW(sourceController->OnDeviceOnline(devId, devName, deviceType));
    ASSERT_NO_THROW(sourceController->OnCallCreated(imsCall, devId));
    CallObjectManager::callObjectPtrList_.clear();
    ASSERT_NO_THROW(sourceController->OnCallDestroyed());
    ASSERT_NO_THROW(sourceController->ProcessCallInfo(imsCall, DistributedDataType::NAME));
}

} // namespace Telephony
} // namespace OHOS
