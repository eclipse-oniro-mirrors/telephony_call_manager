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
constexpr uint32_t INTEROPERABLE_MAX_RECV_DATA_LEN = 2048;
 
enum class InteroperableMsgType : int32_t {
    DATA_TYPE_UNKNOWN = -1,
    DATA_TYPE_MUTE_RINGER = 102,
    DATA_TYPE_MUTE = 104,
};
 
class InteroperableDataController : public IInteroperableDeviceStateCallback, public ISessionCallback {
public:
    InteroperableDataController() = default;
    ~InteroperableDataController() override = default;
    void OnReceiveMsg(const char* data, uint32_t dataLen) override;
    void SetMuted(bool isMute);
    void MuteRinger();
 
    virtual void OnCallCreated(const sptr<CallBase> &call, const std::string &networkId) = 0;
    virtual void OnCallDestroyed() = 0;
 
protected:
    bool GetInt32Value(const cJSON *msg, const std::string &name, int32_t &value);
    bool GetBoolValue(const cJSON *msg, const std::string &name, bool &value);
 
    std::shared_ptr<SessionAdapter> session_{nullptr};
 
private:
    void HandleMuted(const cJSON *msg);
    void HandleMuteRinger();
    std::string CreateMuteMsg(InteroperableMsgType msgType, bool isMute);
    std::string CreateMuteRingerMsg(InteroperableMsgType msgType);
};
}
}
#endif // INTEROPERABLE_DATA_CONTROLLER_H