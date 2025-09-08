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
#include "call_object_manager.h"
#include "call_ability_report_proxy.h"
#include "report_call_info_handler.h"

namespace OHOS {
namespace Telephony {
constexpr int32_t BT_CALL_WAIT_SLOT_TIME = 2;
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
    } else if (msgType == static_cast<int32_t>(InteroperableMsgType::DATA_TYPE_REQUISITES)) {
        HandleRequisitesData(msg);
    } else {
        HandleSpecificMsg(msgType, msg);
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
    DelayedSingleton<CallControlManager>::GetInstance()->MuteRinger();
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

void InteroperableDataController::SendRequisiteDataToPeer(int32_t slotId, const std::string &phoneNum)
{
    if (session_ == nullptr) {
        TELEPHONY_LOGE("session is null");
        return;
    }

    auto data = CreateRequisitesDataMsg(InteroperableMsgType::DATA_TYPE_REQUISITES, slotId, phoneNum);
    if (data.empty()) {
        return;
    }
    session_->SendMsg(data.c_str(), static_cast<uint32_t>(data.length()));
}

std::string InteroperableDataController::CreateRequisitesDataMsg(InteroperableMsgType msgType,
    int32_t slotId, const std::string &phoneNum)
{
    std::string data = "";
    cJSON *slotIdMsg = cJSON_CreateObject();
    if (slotIdMsg == nullptr) {
        TELEPHONY_LOGE("create json msg fail");
        return data;
    }

    cJSON_AddNumberToObject(slotIdMsg, DATA_TYPE, static_cast<int32_t>(msgType));
    cJSON_AddStringToObject(slotIdMsg, INTEROPERABLE_ITEM_PHONE_NUMBER, phoneNum.c_str());
    cJSON_AddNumberToObject(slotIdMsg, INTEROPERABLE_ITEM_SLOT, slotId);
    cJSON_AddBoolToObject(slotIdMsg, INTEROPERABLE_ITEM_CALL_TYPE, false); // is caas call
    char *jsonData = cJSON_PrintUnformatted(slotIdMsg);
    if (jsonData != nullptr) {
        data = jsonData;
        free(jsonData);
        jsonData = nullptr;
    }
    cJSON_Delete(slotIdMsg);
    return data;
}

void InteroperableDataController::HandleRequisitesData(const cJSON *msg)
{
    int32_t slotId = DEFAULT_SIM_SLOT_ID;
    std::string phoneNum = "";
    bool isCaasVoip = false;
    if (!GetStringValue(msg, INTEROPERABLE_ITEM_PHONE_NUMBER, phoneNum)) {
        TELEPHONY_LOGE("OnRequisitesDataReceived get phone num failed");
        return;
    }
    if (!GetInt32Value(msg, INTEROPERABLE_ITEM_SLOT, slotId)) {
        TELEPHONY_LOGE("OnRequisitesDataReceived get slotId failed");
        return;
    }
    if (!GetBoolValue(msg, INTEROPERABLE_ITEM_CALL_TYPE, isCaasVoip)) {
        TELEPHONY_LOGE("OnRequisitesDataReceived get isCaasVoip failed");
        return;
    }

    TELEPHONY_LOGI("parse data success, slot id[%{public}d]", slotId);
    sptr<CallBase> callPtr = CallObjectManager::GetOneCallObject(phoneNum);
    if (callPtr == nullptr) {
        TELEPHONY_LOGI("OnRequisitesDataReceived, query call failed, save slotId");
        SaveBtSlotId(phoneNum, slotId);
        std::lock_guard<ffrt::mutex> lock(slotIdMutex_);
        slotIdCv_.notify_all();
        return;
    }
    TELEPHONY_LOGI("recv slotId, set true");
    SetIsSlotIdVisible(true);
    CallAttributeInfo info;
    callPtr->SetAccountId(slotId);
    callPtr->GetCallAttributeBaseInfo(info);
    auto abilityProxy = DelayedSingleton<CallAbilityReportProxy>::GetInstance();
    if (abilityProxy != nullptr) {
        abilityProxy->ReportCallStateInfo(info);
    }
}

bool InteroperableDataController::GetStringValue(const cJSON *msg, const std::string &name,
    std::string &value)
{
    cJSON *dataJson = cJSON_GetObjectItem(msg, name.c_str());
    if (dataJson == nullptr || !cJSON_IsString(dataJson)) {
        TELEPHONY_LOGE("%{public}s is not string", name.c_str());
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

std::string InteroperableDataController::CreateQueryRequisitesDataMsg(InteroperableMsgType msgType,
    const std::string &phoneNum)
{
    std::string data = "";
    cJSON *querySlotMsg = cJSON_CreateObject();
    if (querySlotMsg == nullptr) {
        TELEPHONY_LOGE("create json msg fail");
        return data;
    }
 
    cJSON_AddNumberToObject(querySlotMsg, DATA_TYPE, static_cast<int32_t>(msgType));
    cJSON_AddStringToObject(querySlotMsg, INTEROPERABLE_ITEM_PHONE_NUMBER, phoneNum.c_str());
    char *jsonData = cJSON_PrintUnformatted(querySlotMsg);
    if (jsonData != nullptr) {
        data = jsonData;
        free(jsonData);
        jsonData = nullptr;
    }
    cJSON_Delete(querySlotMsg);
    return data;
}

void InteroperableDataController::SendRequisiteDataQueryToPeer(const std::string &phoneNum)
{
    TELEPHONY_LOGI("SendQueryRequisitesDataMsgWithPhoneNum enter");
    if (session_ == nullptr || !session_->IsReady()) {
        TELEPHONY_LOGE("session invalid");
        return;
    }

    auto data = CreateQueryRequisitesDataMsg(InteroperableMsgType::DATA_TYPE_QUERY_REQUISITES_DATA, phoneNum);
    if (data.empty()) {
        return;
    }
    session_->SendMsg(data.c_str(), static_cast<uint32_t>(data.length()));
}

void InteroperableDataController::WaitForBtSlotId(const std::string &phoneNum)
{
    TELEPHONY_LOGI("wait slot id 2s");
    std::unique_lock<ffrt::mutex> lock(slotIdMutex_);
    auto status = slotIdCv_.wait_for(lock, std::chrono::seconds(BT_CALL_WAIT_SLOT_TIME), [&] {
        return slotIdMap_.find(phoneNum) != slotIdMap_.end();
    });
    if (!status) {
        TELEPHONY_LOGE("wait slot id timeout");
    }
}

void InteroperableDataController::SaveBtSlotId(const std::string &phoneNum, int32_t slotId)
{
    std::lock_guard<ffrt::mutex> lock(slotIdMutex_);
    slotIdMap_[phoneNum] = slotId;
}

void InteroperableDataController::DeleteBtSlotIdByPhoneNumber(const std::string &phoneNum)
{
    std::lock_guard<ffrt::mutex> lock(slotIdMutex_);
    if (!slotIdMap_.erase(phoneNum)) {
        TELEPHONY_LOGE("delete slot failed!");
    }
}

void InteroperableDataController::ClearBtSlotId()
{
    TELEPHONY_LOGI("clear slot id map");
    std::lock_guard<ffrt::mutex> lock(slotIdMutex_);
    slotIdMap_.clear();
}

int32_t InteroperableDataController::GetBtSlotIdByPhoneNumber(const std::string &phoneNum)
{
    std::lock_guard<ffrt::mutex> lock(slotIdMutex_);
    if (slotIdMap_.find(phoneNum) != slotIdMap_.end()) {
        return slotIdMap_[phoneNum];
    }
    return BT_CALL_INVALID_SLOT;
}

bool InteroperableDataController::IsSlotIdVisible()
{
    if (session_ == nullptr || !session_->IsReady()) {
        TELEPHONY_LOGE("session invalid");
        return false;
    }
    return isSlotIdVisible_.load();
}

void InteroperableDataController::SetIsSlotIdVisible(bool isVisible)
{
    isSlotIdVisible_.store(isVisible);
}
}
}