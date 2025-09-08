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

#ifndef INTEROPERABLE_CLIENT_MANAGER_H
#define INTEROPERABLE_CLIENT_MANAGER_H

#include <map>
#include "ffrt.h"
#include "interoperable_data_controller.h"

namespace OHOS {
namespace Telephony {
class InteroperableClientManager : public InteroperableDataController,
                                    public std::enable_shared_from_this<InteroperableClientManager> {
public:
    InteroperableClientManager() = default;
    ~InteroperableClientManager() override = default;
    void OnDeviceOnline(const std::string &networkId, const std::string &devName, uint16_t devType) override {}
    void OnDeviceOffline(const std::string &networkId, const std::string &devName, uint16_t devType) override;
    void OnConnected() override;
    void OnCallCreated(const sptr<CallBase> &call, const std::string &networkId) override;
    void OnCallDestroyed() override;
    void CallCreated(const sptr<CallBase> &call, const std::string &networkId) override;
    void HandleSpecificMsg(int32_t msgType, const cJSON *msg) override {}
 
private:
    void ConnectRemote(const std::string &networkId);
    ffrt::mutex mutex_{};
    std::string phoneNum_{""};
};
}
}
#endif // INTEROPERABLE_CLIENT_MANAGER_H