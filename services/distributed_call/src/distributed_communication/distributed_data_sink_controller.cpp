/*
 * Copyright (C) 2024 Huawei Device Co., Ltd.
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

#include "distributed_data_sink_controller.h"
#include "call_ability_report_proxy.h"
#include "common_type.h"
#include "telephony_log_wrapper.h"
#include "call_object_manager.h"
#include "transmission_manager.h"

namespace OHOS {
namespace Telephony {
void DistributedDataSinkController::OnCallCreated(const sptr<CallBase> &call, const std::string &devId)
{
    ConnectRemote(devId);
    CheckLocalData(call, DistributedDataType::NAME);
    CheckLocalData(call, DistributedDataType::LOCATION);
    SendDataQueryReq();
}

void DistributedDataSinkController::OnCallDestroyed()
{
    if (CallObjectManager::HasCallExist()) {
        return;
    }
    if (session_ != nullptr) {
        session_->Disconnect();
        session_.reset();
        session_ = nullptr;
    }

    std::lock_guard<ffrt::mutex> lock(mutex_);
    queryInfo_.clear();
}

void DistributedDataSinkController::ProcessCallInfo(const sptr<CallBase> &call, DistributedDataType type)
{
    CheckLocalData(call, type);
    SendDataQueryReq();
}

void DistributedDataSinkController::OnConnected()
{
    SendDataQueryReq();
    SendCurrentDataQueryReq();
}

void DistributedDataSinkController::HandleRecvMsg(int32_t msgType, const cJSON *msg)
{
    if (msg == nullptr) {
        return;
    }
    switch (msgType) {
        case static_cast<int32_t>(DistributedMsgType::DATA_RSP):
            HandleDataQueryRsp(msg);
            break;
        case static_cast<int32_t>(DistributedMsgType::CURRENT_DATA_RSP):
            HandleCurrentDataQueryRsp(msg);
            break;
        default:
            break;
    }
}

void DistributedDataSinkController::ConnectRemote(const std::string &devId)
{
    if (session_ != nullptr) {
        return;
    }
    auto transMgr = DelayedSingleton<TransmissionManager>::GetInstance();
    if (transMgr == nullptr) {
        TELEPHONY_LOGE("get transmission manager fail");
        return;
    }
    session_ = transMgr->CreateClientSession(shared_from_this());
    if (session_ != nullptr) {
        session_->Connect(devId, SESSION_NAME, SESSION_NAME);
    }
}

void DistributedDataSinkController::CheckLocalData(const sptr<CallBase> &call, DistributedDataType type)
{
    if (call == nullptr) {
        return;
    }
    std::string num = call->GetAccountNumber();
    std::string location = call->GetNumberLocation();
    std::string name(call->GetCallerInfo().name);
    if (num.empty()) {
        TELEPHONY_LOGE("invalid phone num");
        return;
    }
    uint32_t queryInfo = 0;
    if (type == DistributedDataType::NAME && name.empty()) {
        queryInfo |= DISTRIBUTED_DATA_TYPE_OFFSET_BASE << static_cast<uint32_t>(DistributedDataType::NAME);
    }
    if (type == DistributedDataType::LOCATION && (location.empty() || location == "default")) {
        queryInfo |= DISTRIBUTED_DATA_TYPE_OFFSET_BASE << static_cast<uint32_t>(DistributedDataType::LOCATION);
    }
    TELEPHONY_LOGI("query distributed data info %{public}d", queryInfo);

    std::lock_guard<ffrt::mutex> lock(mutex_);
    if (queryInfo_.find(num) == queryInfo_.end()) {
        queryInfo_[num] = queryInfo;
    } else {
        queryInfo_[num] |= queryInfo;
    }
}

std::string DistributedDataSinkController::CreateDataReqMsg(DistributedMsgType msgType, uint32_t itemType,
    const std::string &num)
{
    std::string data = "";
    cJSON *reqMsg = cJSON_CreateObject();
    if (reqMsg == nullptr) {
        TELEPHONY_LOGE("create json msg fail");
        return data;
    }
    do {
        if (cJSON_AddNumberToObject(reqMsg, DISTRIBUTED_MSG_TYPE, static_cast<int32_t>(msgType)) == nullptr) {
            TELEPHONY_LOGE("add msg type fail");
            break;
        }
        if (cJSON_AddNumberToObject(reqMsg, DISTRIBUTED_ITEM_TYPE, static_cast<int32_t>(itemType)) == nullptr) {
            TELEPHONY_LOGE("add item type fail");
            break;
        }
        if (cJSON_AddStringToObject(reqMsg, DISTRIBUTED_ITEM_NUM, num.c_str()) == nullptr) {
            TELEPHONY_LOGE("add num fail");
            break;
        }
        char *jsonData = cJSON_PrintUnformatted(reqMsg);
        if (jsonData != nullptr) {
            data = jsonData;
            free(jsonData);
            jsonData = nullptr;
        }
    } while (false);
    cJSON_Delete(reqMsg);
    return data;
}

void DistributedDataSinkController::SendDataQueryReq()
{
    if (session_ == nullptr || !session_->IsReady()) {
        TELEPHONY_LOGI("session not ready");
        return;
    }
    std::lock_guard<ffrt::mutex> lock(mutex_);
    for (auto iter = queryInfo_.begin(); iter != queryInfo_.end(); iter++) {
        uint32_t queryInfo = iter->second;
        for (auto type = static_cast<uint32_t>(DistributedDataType::NAME);
             type < static_cast<uint32_t>(DistributedDataType::MAX); type++) {
            if (((queryInfo >> type) & DISTRIBUTED_DATA_TYPE_OFFSET_BASE) != DISTRIBUTED_DATA_TYPE_OFFSET_BASE) {
                continue;
            }
            TELEPHONY_LOGI("send request data, type %{public}d", type);
            auto data = CreateDataReqMsg(DistributedMsgType::DATA_REQ, type, iter->first);
            if (data.empty()) {
                continue;
            }
            session_->SendMsg(data.c_str(), static_cast<uint32_t>(data.length()));
            iter->second &= ~(DISTRIBUTED_DATA_TYPE_OFFSET_BASE << type);
        }
    }
}

void DistributedDataSinkController::HandleDataQueryRsp(const cJSON *msg)
{
    int32_t type = static_cast<int32_t>(DistributedDataType::MAX);
    if (!GetInt32Value(msg, DISTRIBUTED_ITEM_TYPE, type)) {
        return;
    }
    std::string num = "";
    if (!GetStringValue(msg, DISTRIBUTED_ITEM_NUM, num)) {
        return;
    }
    auto call = CallObjectManager::GetOneCallObject(num);
    if (call == nullptr) {
        TELEPHONY_LOGE("not find distributed call");
        return;
    }
    if (type == static_cast<int32_t>(DistributedDataType::NAME)) {
        UpdateCallName(call, msg);
    } else if (type == static_cast<int32_t>(DistributedDataType::LOCATION)) {
        UpdateCallLocation(call, msg);
    }
}

void DistributedDataSinkController::UpdateCallName(sptr<CallBase> &call, const cJSON *msg)
{
    std::string name = "";
    if (!GetStringValue(msg, DISTRIBUTED_ITEM_NAME, name)) {
        return;
    }
    if (name.empty()) {
        return;
    }
    auto callerInfo = call->GetCallerInfo();
    callerInfo.name = name;
    call->SetCallerInfo(callerInfo);
    ReportCallInfo(call);
}

void DistributedDataSinkController::UpdateCallLocation(sptr<CallBase> &call, const cJSON *msg)
{
    std::string location = "";
    if (!GetStringValue(msg, DISTRIBUTED_ITEM_LOCATION, location)) {
        return;
    }
    if (!location.empty()) {
        call->SetNumberLocation(location);
        ReportCallInfo(call);
    }
}

void DistributedDataSinkController::ReportCallInfo(const sptr<CallBase> &call)
{
    CallAttributeInfo info;
    call->GetCallAttributeBaseInfo(info);
    auto abilityProxy = DelayedSingleton<CallAbilityReportProxy>::GetInstance();
    if (abilityProxy != nullptr) {
        abilityProxy->ReportCallStateInfo(info);
    }
}

std::string DistributedDataSinkController::CreateCurrentDataReqMsg(const std::string &num)
{
    std::string data = "";
    cJSON *currentReqMsg = cJSON_CreateObject();
    if (currentReqMsg == nullptr) {
        TELEPHONY_LOGE("create json msg fail");
        return data;
    }
    do {
        if (cJSON_AddNumberToObject(currentReqMsg, DISTRIBUTED_MSG_TYPE,
            static_cast<int32_t>(DistributedMsgType::CURRENT_DATA_REQ)) == nullptr) {
            TELEPHONY_LOGE("add msg type fail");
            break;
        }
        if (cJSON_AddStringToObject(currentReqMsg, DISTRIBUTED_ITEM_NUM, num.c_str()) == nullptr) {
            TELEPHONY_LOGE("add num fail");
            break;
        }
        char *jsonData = cJSON_PrintUnformatted(currentReqMsg);
        if (jsonData != nullptr) {
            data = jsonData;
            free(jsonData);
            jsonData = nullptr;
        }
    } while (false);
    cJSON_Delete(currentReqMsg);
    return data;
}

void DistributedDataSinkController::SendCurrentDataQueryReq()
{
    if (session_ == nullptr) {
        return;
    }
    auto calls = CallObjectManager::GetAllCallList();
    for (auto &call : calls) {
        if (call == nullptr) {
            continue;
        }
        auto callType = call->GetCallType();
        if (callType != CallType::TYPE_IMS && callType != CallType::TYPE_CS) {
            continue;
        }
        TELEPHONY_LOGI("send current data request");
        auto data = CreateCurrentDataReqMsg(call->GetAccountNumber());
        if (data.empty()) {
            continue;
        }
        session_->SendMsg(data.c_str(), static_cast<uint32_t>(data.length()));
    }
}

void DistributedDataSinkController::HandleCurrentDataQueryRsp(const cJSON *msg)
{
    HandleMuted(msg);
    std::string num = "";
    if (!GetStringValue(msg, DISTRIBUTED_ITEM_NUM, num)) {
        return;
    }
    int32_t direction = static_cast<int32_t>(CallDirection::CALL_DIRECTION_UNKNOW);
    if (!GetInt32Value(msg, DISTRIBUTED_ITEM_DIRECTION, direction)) {
        return;
    }
    TELEPHONY_LOGI("get distributed call direction %{public}d", direction);
    if (direction < static_cast<int32_t>(CallDirection::CALL_DIRECTION_IN) ||
        direction > static_cast<int32_t>(CallDirection::CALL_DIRECTION_OUT)) {
        return;
    }
    auto call = CallObjectManager::GetOneCallObject(num);
    if (call != nullptr) {
        call->SetCallDirection(static_cast<CallDirection>(direction));
    }
}

} // namespace Telephony
} // namespace OHOS
