/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include "distributed_communication_fuzzer.h"

#include <cstddef>
#include <cstdint>
#include "addcalltoken_fuzzer.h"
#include "ims_call.h"
#include "distributed_data_sink_controller.h"
#include "distributed_data_source_controller.h"

using namespace OHOS::Telephony;
namespace OHOS {
void TestCommonController(const std::shared_ptr<DistributedDataController> &controller, const uint8_t *data,
    size_t size)
{
    std::string stringValue(reinterpret_cast<const char *>(data), size);
    int32_t intValue = static_cast<int32_t>(size);
    AudioDeviceType audioDevType = static_cast<AudioDeviceType>(
        size % static_cast<uint32_t>(AudioDeviceType::DEVICE_DISTRIBUTED_PC));
    const char *charValue = reinterpret_cast<const char *>(data);
    uint32_t uintValue = static_cast<uint32_t>(size);
    DialParaInfo dialParaInfo;
    sptr<OHOS::Telephony::CallBase> call = new IMSCall(dialParaInfo);
    DistributedDataType distributedDataType = static_cast<DistributedDataType>(
        size % static_cast<uint32_t>(DistributedDataType::MAX));
    DistributedMsgType msgType = static_cast<DistributedMsgType>(
        size % static_cast<uint32_t>(DistributedMsgType::CURRENT_DATA_RSP));
    controller->OnDistributedAudioDeviceChange(stringValue, stringValue, audioDevType, intValue);
    controller->OnRemoveSystemAbility();
    controller->OnReceiveMsg(charValue, uintValue);
    controller->OnDeviceOnline(stringValue, stringValue, audioDevType);
    controller->OnDeviceOffline(stringValue, stringValue, audioDevType);
    controller->OnCallCreated(call, stringValue);
    controller->OnCallDestroyed();
    controller->ProcessCallInfo(call, distributedDataType);
    controller->OnConnected();
    controller->SetMuted(true);
    controller->MuteRinger();

    controller->HandleRecvMsg(intValue, nullptr);
    std::string json = "{ \"dataType\": 101 }";
    cJSON *msg = cJSON_Parse(json.c_str());
    controller->HandleRecvMsg(intValue, msg);
    int32_t jsonIntValue;
    controller->GetInt32Value(msg, stringValue, jsonIntValue);
    std::string jsonStringValue;
    controller->GetStringValue(msg, stringValue, jsonStringValue);
    bool jsonBoolValue;
    controller->GetBoolValue(msg, stringValue, jsonBoolValue);
    controller->HandleMuted(msg);
    controller->CreateMuteMsg(msgType, true);
    controller->CreateMuteRingerMsg(msgType);
    controller->HandleMuteRinger();
    cJSON_Delete(msg);
}

void TestSinkController(const uint8_t *data, size_t size)
{
    if (!IsServiceInited()) {
        return;
    }
    if (data == nullptr) {
        return;
    }
    auto controller = std::make_shared<DistributedDataSinkController>();
    if (controller == nullptr) {
        return;
    }
    TestCommonController(controller, data, size);

    std::string stringValue(reinterpret_cast<const char *>(data), size);
    int32_t intValue = static_cast<int32_t>(size);
    DistributedDataType distributedDataType = static_cast<DistributedDataType>(
        size % static_cast<uint32_t>(DistributedDataType::MAX));
    DistributedMsgType msgType = static_cast<DistributedMsgType>(
        size % static_cast<uint32_t>(DistributedMsgType::CURRENT_DATA_RSP));
    uint32_t uintValue = static_cast<uint32_t>(size);
    DialParaInfo dialParaInfo;
    sptr<OHOS::Telephony::CallBase> call = new IMSCall(dialParaInfo);
    controller->ConnectRemote(stringValue);
    controller->CheckLocalData(call, distributedDataType);
    controller->CreateDataReqMsg(msgType, uintValue, stringValue);
    controller->SendDataQueryReq();
    controller->ReportCallInfo(call);
    controller->CreateCurrentDataReqMsg(stringValue);

    cJSON *msg = cJSON_Parse("{ \"itemType\": 0, \"num\": \"123456\", \"name\": \"test\", \"location\": \"test\" }");
    controller->HandleDataQueryRsp(msg);
    controller->UpdateCallName(call, msg);
    controller->UpdateCallLocation(call, msg);
    controller->HandleCurrentDataQueryRsp(msg);
    cJSON_Delete(msg);
}

void TestSourceController(const uint8_t *data, size_t size)
{
    if (!IsServiceInited()) {
        return;
    }
    if (data == nullptr) {
        return;
    }
    auto controller = std::make_shared<DistributedDataSourceController>();
    if (controller == nullptr) {
        return;
    }
    TestCommonController(controller, data, size);

    std::string stringValue(reinterpret_cast<const char *>(data), size);
    DistributedDataType distributedDataType = static_cast<DistributedDataType>(
        size % static_cast<uint32_t>(DistributedDataType::MAX));
    DialParaInfo dialParaInfo;
    sptr<OHOS::Telephony::CallBase> call = new IMSCall(dialParaInfo);
    DistributedMsgType msgType = static_cast<DistributedMsgType>(
        size % static_cast<uint32_t>(DistributedMsgType::CURRENT_DATA_RSP));
    uint32_t uintValue = static_cast<uint32_t>(size);
    int32_t intValue = static_cast<int32_t>(size);
    controller->SaveLocalData(stringValue, distributedDataType, stringValue);
    controller->SaveLocalData(call, distributedDataType);
    controller->CreateDataRspMsg(msgType, uintValue, stringValue, stringValue, stringValue);
    controller->SendLocalDataRsp();
    controller->CreateCurrentDataRspMsg(stringValue, true, intValue);

    cJSON *msg = cJSON_Parse("{ \"itemType\": 0, \"num\": \"123456\" }");
    controller->HandleDataQueryMsg(msg);
    controller->HandleCurrentDataQueryMsg((msg));
    cJSON_Delete(msg);
}

void DoSomethingInterestingWithMyAPI(const uint8_t *data, size_t size)
{
    if (data == nullptr || size == 0) {
        return;
    }

    TestSinkController(data, size);
    TestSourceController(data, size);
}
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size)
{
    OHOS::AddCallTokenFuzzer token;
    /* Run your code on data */
    OHOS::DoSomethingInterestingWithMyAPI(data, size);
    return 0;
}
