/*
 * Copyright (C) 2024-2024 Huawei Device Co., Ltd.
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

#include "distributed_data_controller.h"
#include "telephony_log_wrapper.h"
#include "call_control_manager.h"

namespace OHOS {
namespace Telephony {
void DistributedDataController::OnReceiveMsg(const char* data, uint32_t dataLen)
{
    if (dataLen > DISTRIBUTED_MAX_RECV_DATA_LEN) {
        TELEPHONY_LOGE("recv invalid distributed data len %{public}d", dataLen);
        return;
    }
    std::string recvData(data, dataLen);
    cJSON *msg = cJSON_Parse(recvData.c_str());
    if (msg == nullptr) {
        TELEPHONY_LOGE("json string invalid");
        return;
    }
    int32_t msgType = static_cast<int32_t>(DistributedMsgType::UNKNOWN);
    if (!GetInt32Value(msg, DISTRIBUTED_MSG_TYPE, msgType)) {
        cJSON_Delete(msg);
        return;
    }
    TELEPHONY_LOGI("recv data, msg type %{public}d", msgType);
    switch (msgType) {
        case static_cast<int32_t>(DistributedMsgType::MUTE):
            HandleMuted(msg);
            break;
        case static_cast<int32_t>(DistributedMsgType::MUTE_RINGER):
            HandleMuteRinger();
            break;
        default:
            HandleRecvMsg(msgType, msg);
            break;
    }
    cJSON_Delete(msg);
}

void DistributedDataController::SetMuted(bool isMute)
{
    if (session_ == nullptr) {
        TELEPHONY_LOGE("session is null");
        return;
    }
    TELEPHONY_LOGI("send muted %{public}d", isMute);
    auto data = CreateMuteMsg(DistributedMsgType::MUTE, isMute);
    if (data.empty()) {
        return;
    }
    session_->SendMsg(data.c_str(), static_cast<uint32_t>(data.length()));
}

void DistributedDataController::MuteRinger()
{
    if (session_ == nullptr) {
        return;
    }
    TELEPHONY_LOGI("send mute ringer");
    auto data = CreateMuteRingerMsg(DistributedMsgType::MUTE_RINGER);
    if (data.empty()) {
        return;
    }
    session_->SendMsg(data.c_str(), static_cast<uint32_t>(data.length()));
}

bool DistributedDataController::GetInt32Value(const cJSON *msg, const std::string &name, int32_t &value)
{
    cJSON *dataJson = cJSON_GetObjectItem(msg, name.c_str());
    if (dataJson == nullptr || !cJSON_IsNumber(dataJson)) {
        TELEPHONY_LOGE("%{public}s not contain or not number", name.c_str());
        return false;
    }
    value = static_cast<int32_t>(dataJson->valueint);
    return true;
}

bool DistributedDataController::GetStringValue(const cJSON *msg, const std::string &name, std::string &value)
{
    cJSON *dataJson = cJSON_GetObjectItem(msg, name.c_str());
    if (dataJson == nullptr || !cJSON_IsString(dataJson)) {
        TELEPHONY_LOGE("%{public}s not contain or not string", name.c_str());
        return false;
    }
    char *data = cJSON_GetStringValue(dataJson);
    if (data == nullptr) {
        TELEPHONY_LOGE("get null %{public}s", name.c_str());
        return false;
    }
    value = data;
    return true;
}

bool DistributedDataController::GetBoolValue(const cJSON *msg, const std::string &name, bool &value)
{
    cJSON *dataJson = cJSON_GetObjectItem(msg, name.c_str());
    if (dataJson == nullptr || !cJSON_IsBool(dataJson)) {
        TELEPHONY_LOGE("%{public}s not contain or not bool", name.c_str());
        return false;
    }
    value = cJSON_IsTrue(dataJson);
    return true;
}

void DistributedDataController::HandleMuted(const cJSON *msg)
{
    bool isMuted = false;
    if (!GetBoolValue(msg, DISTRIBUTED_ITEM_MUTE, isMuted)) {
        return;
    }
    TELEPHONY_LOGI("set muted %{public}d", isMuted);
    auto controlManager = DelayedSingleton<CallControlManager>::GetInstance();
    if (controlManager != nullptr) {
        controlManager->SetMuted(isMuted);
    }
}

std::string DistributedDataController::CreateMuteMsg(DistributedMsgType msgType, bool isMute)
{
    std::string data = "";
    cJSON *muteMsg = cJSON_CreateObject();
    if (muteMsg == nullptr) {
        TELEPHONY_LOGE("create json msg fail");
        return data;
    }
    do {
        if (cJSON_AddNumberToObject(muteMsg, DISTRIBUTED_MSG_TYPE, static_cast<int32_t>(msgType)) == nullptr) {
            TELEPHONY_LOGE("add msg type fail");
            break;
        }
        if (cJSON_AddBoolToObject(muteMsg, DISTRIBUTED_ITEM_MUTE, isMute) == nullptr) {
            TELEPHONY_LOGE("add mute value fail");
            break;
        }
        char *jsonData = cJSON_PrintUnformatted(muteMsg);
        if (jsonData != nullptr) {
            data = jsonData;
            free(jsonData);
            jsonData = nullptr;
        }
    } while (false);
    cJSON_Delete(muteMsg);
    return data;
}

std::string DistributedDataController::CreateMuteRingerMsg(DistributedMsgType msgType)
{
    std::string data = "";
    cJSON *muteRingerMsg = cJSON_CreateObject();
    if (muteRingerMsg == nullptr) {
        TELEPHONY_LOGE("create json msg fail");
        return data;
    }
    do {
        if (cJSON_AddNumberToObject(muteRingerMsg, DISTRIBUTED_MSG_TYPE, static_cast<int32_t>(msgType)) == nullptr) {
            TELEPHONY_LOGE("add msg type fail");
            break;
        }
        char *jsonData = cJSON_PrintUnformatted(muteRingerMsg);
        if (jsonData != nullptr) {
            data = jsonData;
            free(jsonData);
            jsonData = nullptr;
        }
    } while (false);
    cJSON_Delete(muteRingerMsg);
    return data;
}

void DistributedDataController::HandleMuteRinger()
{
    TELEPHONY_LOGI("mute ringer");
    auto controlManager = DelayedSingleton<CallControlManager>::GetInstance();
    if (controlManager != nullptr) {
        controlManager->MuteRinger();
    }
}

} // namespace Telephony
} // namespace OHOS
