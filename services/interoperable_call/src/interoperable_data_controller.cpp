/*
 * Copyright (C) 2025-2025 Huawei Device Co., Ltd.
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

#include "interoperable_data_controller.h"
#include "telephony_log_wrapper.h"
#include "call_control_manager.h"

namespace OHOS {
namespace Telephony {
void InteroperableDataController::OnReceiveMsg(const char* data, uint32_t dataLen)
{
    if (dataLen > INTEROPERABLE_MAX_RECV_DATA_LEN) {
        TELEPHONY_LOGE("recv invalid data len %{public}d", dataLen);
        return;
    }
    std::string recvData(data, dataLen);
    cJSON *msg = cJSON_Parse(recvData.c_str());
    if (msg == nullptr) {
        TELEPHONY_LOGE("json string invalid");
        return;
    }
    int32_t msgType = static_cast<int32_t>(InteroperableMsgType::DATA_TYPE_UNKNOWN);
    if (!GetInt32Value(msg, DATA_TYPE, msgType)) {
        cJSON_Delete(msg);
        return;
    }
    TELEPHONY_LOGI("recv data, msg type %{public}d", msgType);
    if (msgType == static_cast<int32_t>(InteroperableMsgType::DATA_TYPE_MUTE)) {
        HandleMuted(msg);
    } else if (msgType == static_cast<int32_t>(InteroperableMsgType::DATA_TYPE_MUTE_RINGER)) {
        HandleMuteRinger();
    }
    cJSON_Delete(msg);
}
 
void InteroperableDataController::SetMuted(bool isMute)
{
    if (session_ == nullptr) {
        TELEPHONY_LOGE("session is null");
        return;
    }
    TELEPHONY_LOGI("send mute %{public}d msg", isMute);
    auto data = CreateMuteMsg(InteroperableMsgType::DATA_TYPE_MUTE, isMute);
    if (data.empty()) {
        return;
    }
    session_->SendMsg(data.c_str(), static_cast<uint32_t>(data.length()));
}
 
void InteroperableDataController::MuteRinger()
{
    if (session_ == nullptr) {
        TELEPHONY_LOGE("session is null");
        return;
    }
    TELEPHONY_LOGI("send mute ringer msg");
    auto data = CreateMuteRingerMsg(InteroperableMsgType::DATA_TYPE_MUTE_RINGER);
    if (data.empty()) {
        return;
    }
    session_->SendMsg(data.c_str(), static_cast<uint32_t>(data.length()));
}
 
void InteroperableDataController::HandleMuted(const cJSON *msg)
{
    bool isMute = false;
    if (!GetBoolValue(msg, INTEROPERABLE_ITEM_MUTE, isMute)) {
        return;
    }
    TELEPHONY_LOGI("set muted %{public}d", isMute);
    DelayedSingleton<CallControlManager>::GetInstance()->SetMuted(isMute);
}
 
void InteroperableDataController::HandleMuteRinger()
{
    TELEPHONY_LOGI("set mute ringer");
    auto controlManager = DelayedSingleton<CallControlManager>::GetInstance()->MuteRinger();
}
 
std::string InteroperableDataController::CreateMuteMsg(InteroperableMsgType msgType, bool isMute)
{
    std::string data = "";
    cJSON *muteMsg = cJSON_CreateObject();
    if (muteMsg == nullptr) {
        TELEPHONY_LOGE("create json msg fail");
        return data;
    }
 
    cJSON_AddNumberToObject(muteMsg, DATA_TYPE, static_cast<int32_t>(msgType));
    cJSON_AddBoolToObject(muteMsg, INTEROPERABLE_ITEM_MUTE, isMute);
    char *jsonData = cJSON_PrintUnformatted(muteMsg);
    if (jsonData != nullptr) {
        data = jsonData;
        free(jsonData);
        jsonData = nullptr;
    }
    cJSON_Delete(muteMsg);
    return data;
}
 
std::string InteroperableDataController::CreateMuteRingerMsg(InteroperableMsgType msgType)
{
    std::string data = "";
    cJSON *muteRingerMsg = cJSON_CreateObject();
    if (muteRingerMsg == nullptr) {
        TELEPHONY_LOGE("create json msg fail");
        return data;
    }
 
    cJSON_AddNumberToObject(muteRingerMsg, DATA_TYPE, static_cast<int32_t>(msgType));
    char *jsonData = cJSON_PrintUnformatted(muteRingerMsg);
    if (jsonData != nullptr) {
        data = jsonData;
        free(jsonData);
        jsonData = nullptr;
    }
    cJSON_Delete(muteRingerMsg);
    return data;
}
 
bool InteroperableDataController::GetInt32Value(const cJSON *msg, const std::string &name, int32_t &value)
{
    cJSON *dataJson = cJSON_GetObjectItem(msg, name.c_str());
    if (dataJson == nullptr || !cJSON_IsNumber(dataJson)) {
        TELEPHONY_LOGE("%{public}s not contain or not number", name.c_str());
        return false;
    }
    value = static_cast<int32_t>(dataJson->valueint);
    return true;
}
 
bool InteroperableDataController::GetBoolValue(const cJSON *msg, const std::string &name, bool &value)
{
    cJSON *dataJson = cJSON_GetObjectItem(msg, name.c_str());
    if (dataJson == nullptr || !cJSON_IsBool(dataJson)) {
        TELEPHONY_LOGE("%{public}s not contain or not bool", name.c_str());
        return false;
    }
    value = cJSON_IsTrue(dataJson);
    return true;
}
}
}