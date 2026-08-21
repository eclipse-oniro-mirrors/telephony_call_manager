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
 
#ifndef INTEROPERABLE_DATA_CONTROLLER_H
#define INTEROPERABLE_DATA_CONTROLLER_H

#include "cJSON.h"
#include "call_base.h"
#include "i_interoperable_device_state_callback.h"
#include "session_adapter.h"

namespace OHOS {
namespace Telephony {
constexpr const char* DATA_TYPE = "dataType";
constexpr const char* INTEROPERABLE_ITEM_MUTE = "mute";
constexpr const char* INTEROPERABLE_ITEM_SLOT = "slotId";
constexpr const char* INTEROPERABLE_ITEM_PHONE_NUMBER = "phoneNumber";
constexpr const char* INTEROPERABLE_ITEM_CALL_TYPE = "callType";
constexpr uint32_t INTEROPERABLE_MAX_RECV_DATA_LEN = 2048;
constexpr int32_t DEFAULT_SLOT_ID = 0;
constexpr int32_t QOS_BW_BT = 4 * 64 * 1024;    // 小于384*1024才可选路蓝牙，优先wifi，可选蓝牙
 
enum class InteroperableMsgType : int32_t {
    DATA_TYPE_UNKNOWN = -1,
    DATA_TYPE_REQUISITES = 0,
    DATA_TYPE_MUTE_RINGER = 102,
    DATA_TYPE_MUTE = 104,
    DATA_TYPE_QUERY_REQUISITES_DATA = 105,
};
 
class InteroperableDataController : public IInteroperableDeviceStateCallback, public ISessionCallback {
public:
    InteroperableDataController() = default;
    ~InteroperableDataController() override = default;
    void OnReceiveMsg(const char* data, uint32_t dataLen) override;
    void SetMuted(bool isMute);
    void MuteRinger();
    void SendRequisiteDataQueryToPeer(const std::string &phoneNum);
    void SendRequisiteDataToPeer(int32_t slotId, const std::string &phoneNum);
    void DeleteBtSlotIdByPhoneNumber(const std::string &phoneNum);
    void WaitForBtSlotId(const std::string &phoneNum);
    int32_t GetBtSlotIdByPhoneNumber(const std::string &phoneNum);
    bool IsSlotIdVisible();
    void SetIsSlotIdVisible(bool isVisible);

    virtual void OnCallCreated(const sptr<CallBase> &call, const std::string &networkId) = 0;
    virtual void OnCallDestroyed() = 0;
    virtual void HandleSpecificMsg(int32_t msgType, const cJSON *msg) = 0;
    virtual void CallCreated(const sptr<CallBase> &call, const std::string &networkId) = 0;
 
protected:
    bool GetInt32Value(const cJSON *msg, const std::string &name, int32_t &value);
    bool GetBoolValue(const cJSON *msg, const std::string &name, bool &value);
    bool GetStringValue(const cJSON *msg, const std::string &name, std::string &value);
    std::string CreateQueryRequisitesDataMsg(InteroperableMsgType msgType, const std::string &phoneNum);
    void ClearBtSlotId();
 
protected:
    std::shared_ptr<SessionAdapter> session_{nullptr};
    std::atomic<bool> isSlotIdVisible_{false};

private:
    void HandleMuted(const cJSON *msg);
    void HandleMuteRinger();
    std::string CreateMuteMsg(InteroperableMsgType msgType, bool isMute);
    std::string CreateMuteRingerMsg(InteroperableMsgType msgType);
    void HandleRequisitesData(const cJSON *msg);
    std::string CreateRequisitesDataMsg(InteroperableMsgType msgType, int32_t slotId,
        const std::string &phoneNum);
    void SaveBtSlotId(const std::string &phoneNum, int32_t slotId);

private:
    ffrt::mutex slotIdMutex_{};
    ffrt::condition_variable slotIdCv_{};
    std::map<std::string, int32_t> slotIdMap_{};
};
}
}
#endif // INTEROPERABLE_DATA_CONTROLLER_H