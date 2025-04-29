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

#ifndef TELEPHONY_DISTRIBUTED_COMMUNICATION_DATA_CONTROLLER_H
#define TELEPHONY_DISTRIBUTED_COMMUNICATION_DATA_CONTROLLER_H

#include "cJSON.h"
#include "call_base.h"
#include "i_distributed_device_state_callback.h"
#include "session_adapter.h"

namespace OHOS {
namespace Telephony {
constexpr const char* DISTRIBUTED_MSG_TYPE = "dataType";
constexpr const char* DISTRIBUTED_ITEM_TYPE = "itemType";

constexpr const char* DISTRIBUTED_ITEM_NUM = "num";
constexpr const char* DISTRIBUTED_ITEM_NAME = "name";
constexpr const char* DISTRIBUTED_ITEM_LOCATION = "location";
constexpr const char* DISTRIBUTED_ITEM_MUTE = "mute";
constexpr const char* DISTRIBUTED_ITEM_DIRECTION = "direction";

constexpr uint32_t DISTRIBUTED_DATA_TYPE_OFFSET_BASE = 1;
constexpr uint32_t DISTRIBUTED_MAX_RECV_DATA_LEN = 2048;
constexpr int32_t QOS_MIN_BW = 4 * 1024 * 1024;

enum class DistributedDataType : int32_t {
    NAME = 0,
    LOCATION = 1,
    MAX = 2
};

enum class DistributedMsgType : int32_t {
    UNKNOWN = -1,
    DATA_REQ = 100,
    DATA_RSP = 101,
    MUTE_RINGER = 102,
    MUTE = 104,
    CURRENT_DATA_REQ = 105,
    CURRENT_DATA_RSP = 106,
};

class DistributedDataController : public IDistributedDeviceStateCallback, public ISessionCallback {
public:
    DistributedDataController() = default;
    ~DistributedDataController() override = default;
    void OnDistributedAudioDeviceChange(const std::string &devId, const std::string &devName,
        AudioDeviceType devType, int32_t devRole) override {}
    void OnRemoveSystemAbility() override {}
    void OnReceiveMsg(const char* data, uint32_t dataLen) override;

    virtual void OnCallCreated(const sptr<CallBase> &call, const std::string &devId) = 0;
    virtual void OnCallDestroyed() = 0;
    virtual void ProcessCallInfo(const sptr<CallBase> &call, DistributedDataType type) = 0;

    void SetMuted(bool isMute);
    void MuteRinger();

protected:
    virtual void HandleRecvMsg(int32_t msgType, const cJSON *msg) = 0;
    bool GetInt32Value(const cJSON *msg, const std::string &name, int32_t &value);
    bool GetStringValue(const cJSON *msg, const std::string &name, std::string &value);
    bool GetBoolValue(const cJSON *msg, const std::string &name, bool &value);
    void HandleMuted(const cJSON *msg);

protected:
    std::shared_ptr<SessionAdapter> session_{nullptr};

private:
    std::string CreateMuteMsg(DistributedMsgType msgType, bool isMute);
    std::string CreateMuteRingerMsg(DistributedMsgType msgType);
    void HandleMuteRinger();
};

} // namespace Telephony
} // namespace OHOS

#endif // TELEPHONY_DISTRIBUTED_COMMUNICATION_DATA_CONTROLLER_H
