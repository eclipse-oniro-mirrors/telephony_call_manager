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
 
#ifndef INTEROPERABLE_SERVER_MANAGER_H
#define INTEROPERABLE_SERVER_MANAGER_H
 
#include <map>
#include "ffrt.h"
#include "interoperable_data_controller.h"
 
namespace OHOS {
namespace Telephony {
class InteroperableServerManager : public InteroperableDataController,
                                    public std::enable_shared_from_this<InteroperableServerManager> {
public:
    InteroperableServerManager() = default;
    ~InteroperableServerManager() override = default;
    void OnDeviceOnline(const std::string &networkId, const std::string &devName, uint16_t devType) override;
    void OnDeviceOffline(const std::string &networkId, const std::string &devName, uint16_t devType) override;
    void OnConnected() override {}
    void OnCallCreated(const sptr<CallBase> &call, const std::string &networkId) override {}
    void OnCallDestroyed() override {}
};
}
}
#endif // INTEROPERABLE_SERVER_MANAGER_H