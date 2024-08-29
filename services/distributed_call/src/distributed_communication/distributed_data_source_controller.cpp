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

#include "distributed_data_source_controller.h"
#include "telephony_log_wrapper.h"
#include "call_object_manager.h"
#include "transmission_manager.h"

namespace OHOS {
namespace Telephony {
void DistributedDataSourceController::OnDeviceOnline(const std::string &devId, const std::string &devName,
    AudioDeviceType devType)
{
    if (session_ != nullptr) {
        return;
    }
    auto transMgr = DelayedSingleton<TransmissionManager>::GetInstance();
    if (transMgr == nullptr) {
        TELEPHONY_LOGE("get transmission manager fail");
        return;
    }
    session_ = transMgr->CreateServerSession(shared_from_this());
    if (session_ != nullptr) {
        session_->Create(SESSION_NAME);
    }
}

void DistributedDataSourceController::OnDeviceOffline(const std::string &devId, const std::string &devName,
    AudioDeviceType devType)
{
    if (session_ != nullptr) {
        session_->Destroy();
        session_.reset();
        session_ = nullptr;
    }
}

void DistributedDataSourceController::OnCallCreated(const sptr<CallBase> &call, const std::string &devId)
{
    SaveMtLocalData(call);
    SendMtLocalDataRsp();
}

void DistributedDataSourceController::OnCallDestroyed()
{
    if (CallObjectManager::HasCallExist()) {
        return;
    }
    std::lock_guard<ffrt::mutex> lock(mutex_);
    localMtInfo_.clear();
    queryMtInfo_.clear();
}

void DistributedDataSourceController::OnConnected()
{
    SendMtLocalDataRsp();
}

void DistributedDataSourceController::HandleRecvMsg(int32_t msgType, const cJSON *msg)
{
    if (msg == nullptr) {
        return;
    }
    switch (msgType) {
        case static_cast<int32_t>(DistributedMsgType::MT_DATA_REQ):
            HandleMtDataQueryMsg(msg);
            SendMtLocalDataRsp();
            break;
        case static_cast<int32_t>(DistributedMsgType::CURRENT_DATA_REQ):
            HandleCurrentDataQueryMsg(msg);
            break;
        default:
            break;
    }
}

void DistributedDataSourceController::SaveMtLocalData(const std::string &num, DistributedDataType type,
    const std::string &data)
{
    std::lock_guard<ffrt::mutex> lock(mutex_);
    auto iter = localMtInfo_.find(num);
    if (iter == localMtInfo_.end()) {
        std::map<uint32_t, std::string> info;
        info[static_cast<uint32_t>(type)] = data;
        localMtInfo_[num] = info;
    } else {
        (iter->second)[static_cast<uint32_t>(type)] = data;
    }
}

void DistributedDataSourceController::SaveMtLocalData(const sptr<CallBase> &call)
{
    if (call == nullptr) {
        return;
    }
    if (call->GetCallDirection() != CallDirection::CALL_DIRECTION_IN) {
        return;
    }
    std::string num = call->GetAccountNumber();
    std::string name(call->GetCallerInfo().name);
    std::string location = call->GetNumberLocation();
    if (num.empty()) {
        TELEPHONY_LOGE("invalid phone num");
        return;
    }
    if (!name.empty()) {
        SaveMtLocalData(num, DistributedDataType::NAME, name);
    }
    if (!location.empty() && location != "default") {
        SaveMtLocalData(num, DistributedDataType::LOCATION, location);
    }
}

void DistributedDataSourceController::HandleMtDataQueryMsg(const cJSON *msg)
{
    int32_t type = static_cast<int32_t>(DistributedDataType::MAX);
    if (!GetInt32Value(msg, DISTRIBUTED_ITEM_TYPE, type)) {
        return;
    }
    std::string num = "";
    if (!GetStringValue(msg, DISTRIBUTED_ITEM_NUM, num)) {
        return;
    }
    if (num.empty()) {
        TELEPHONY_LOGE("invalid phone num");
        return;
    }

    std::lock_guard<ffrt::mutex> lock(mutex_);
    auto iter = queryMtInfo_.find(num);
    if (iter == queryMtInfo_.end()) {
        queryMtInfo_[num] = DISTRIBUTED_DATA_TYPE_OFFSET_BASE << static_cast<uint32_t>(type);
    } else {
        queryMtInfo_[num] |= DISTRIBUTED_DATA_TYPE_OFFSET_BASE << static_cast<uint32_t>(type);
    }
}

std::string DistributedDataSourceController::CreateMtDataRspMsg(DistributedMsgType msgType, uint32_t itemType,
    const std::string &num, const std::string &name, const std::string &value)
{
    std::string data = "";
    cJSON *rspMsg = cJSON_CreateObject();
    if (rspMsg == nullptr) {
        TELEPHONY_LOGE("create json msg fail");
        return data;
    }
    do {
        if (cJSON_AddNumberToObject(rspMsg, DISTRIBUTED_MSG_TYPE, static_cast<int32_t>(msgType)) == nullptr) {
            TELEPHONY_LOGE("add msg type fail");
            break;
        }
        if (cJSON_AddNumberToObject(rspMsg, DISTRIBUTED_ITEM_TYPE, static_cast<int32_t>(itemType)) == nullptr) {
            TELEPHONY_LOGE("add item type fail");
            break;
        }
        if (cJSON_AddStringToObject(rspMsg, DISTRIBUTED_ITEM_NUM, num.c_str()) == nullptr) {
            TELEPHONY_LOGE("add num fail");
            break;
        }
        if (cJSON_AddStringToObject(rspMsg, name.c_str(), value.c_str()) == nullptr) {
            TELEPHONY_LOGE("add value fail");
            break;
        }
        char *jsonData = cJSON_PrintUnformatted(rspMsg);
        if (jsonData != nullptr) {
            data = jsonData;
            free(jsonData);
            jsonData = nullptr;
        }
    } while (false);
    cJSON_Delete(rspMsg);
    return data;
}

void DistributedDataSourceController::SendMtLocalDataRsp()
{
    if (session_ == nullptr || !session_->IsReady()) {
        TELEPHONY_LOGI("session not ready");
        return;
    }
    std::lock_guard<ffrt::mutex> lock(mutex_);
    for (auto queryIter = queryMtInfo_.begin(); queryIter != queryMtInfo_.end(); queryIter++) {
        auto localIter = localMtInfo_.find(queryIter->first);
        if (localIter == localMtInfo_.end()) {
            continue; // local info not contain queried number
        }
        uint32_t queryInfo = queryIter->second;
        for (auto type = static_cast<uint32_t>(DistributedDataType::NAME);
             type < static_cast<uint32_t>(DistributedDataType::MAX); type++) {
            if (((queryInfo >> type) & DISTRIBUTED_DATA_TYPE_OFFSET_BASE) != DISTRIBUTED_DATA_TYPE_OFFSET_BASE) {
                continue;
            }
            auto typeIter = localIter->second.find(type);
            if (typeIter == localIter->second.end()) {
                continue; // local info not contain queried data type
            }
            TELEPHONY_LOGI("send response data, type %{public}d", type);
            std::string data;
            if (type == static_cast<uint32_t>(DistributedDataType::NAME)) {
                data = CreateMtDataRspMsg(DistributedMsgType::MT_DATA_RSP, type, queryIter->first,
                    DISTRIBUTED_ITEM_NAME, typeIter->second);
            } else if (type == static_cast<uint32_t>(DistributedDataType::LOCATION)) {
                data = CreateMtDataRspMsg(DistributedMsgType::MT_DATA_RSP, type, queryIter->first,
                    DISTRIBUTED_ITEM_LOCATION, typeIter->second);
            }
            if (data.empty()) {
                continue;
            }
            session_->SendMsg(data.c_str(), static_cast<uint32_t>(data.length()));
            queryIter->second &= ~(DISTRIBUTED_DATA_TYPE_OFFSET_BASE << type);
        }
    }
}

std::string DistributedDataSourceController::CreateCurrentDataRspMsg(const std::string &num, bool isMuted,
    int32_t direction)
{
    std::string data = "";
    cJSON *currentRspMsg = cJSON_CreateObject();
    if (currentRspMsg == nullptr) {
        TELEPHONY_LOGE("create json msg fail");
        return data;
    }
    do {
        if (cJSON_AddNumberToObject(currentRspMsg, DISTRIBUTED_MSG_TYPE,
            static_cast<int32_t>(DistributedMsgType::CURRENT_DATA_RSP)) == nullptr) {
            TELEPHONY_LOGE("add msg type fail");
            break;
        }
        if (cJSON_AddBoolToObject(currentRspMsg, DISTRIBUTED_ITEM_MUTE, isMuted) == nullptr) {
            TELEPHONY_LOGE("add mute value fail");
            break;
        }
        if (cJSON_AddStringToObject(currentRspMsg, DISTRIBUTED_ITEM_NUM, num.c_str()) == nullptr) {
            TELEPHONY_LOGE("add num fail");
            break;
        }
        if (cJSON_AddNumberToObject(currentRspMsg, DISTRIBUTED_ITEM_DIRECTION, direction) == nullptr) {
            TELEPHONY_LOGE("add direction fail");
            break;
        }
        char *jsonData = cJSON_PrintUnformatted(currentRspMsg);
        if (jsonData != nullptr) {
            data = jsonData;
            free(jsonData);
            jsonData = nullptr;
        }
    } while (false);
    cJSON_Delete(currentRspMsg);
    return data;
}

void DistributedDataSourceController::HandleCurrentDataQueryMsg(const cJSON *msg)
{
    std::string num = "";
    if (!GetStringValue(msg, DISTRIBUTED_ITEM_NUM, num)) {
        return;
    }
    auto call = CallObjectManager::GetOneCallObject(num);
    if (call == nullptr) {
        TELEPHONY_LOGE("not find distributed call");
        return;
    }
    auto data = CreateCurrentDataRspMsg(num, call->IsMuted(), static_cast<int32_t>(call->GetCallDirection()));
    if (data.empty()) {
        return;
    }
    if (session_ != nullptr) {
        session_->SendMsg(data.c_str(), static_cast<uint32_t>(data.length()));
    }
}

} // namespace Telephony
} // namespace OHOS
