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
 
#ifndef INTEROPERABLE_SETTINGS_HANDLER_H
#define INTEROPERABLE_SETTINGS_HANDLER_H
 
#include "settings_datashare_helper.h"
#include "data_ability_observer_stub.h"
#include "datashare_helper.h"
#include "iservice_registry.h"
 
namespace OHOS {
namespace Telephony {
class InteroperableRecvObserver : public AAFwk::DataAbilityObserverStub {
public:
    InteroperableRecvObserver() = default;
    ~InteroperableRecvObserver() = default;
    void OnChange() override;
};
 
class InteroperableSettingsHandler {
public:
    InteroperableSettingsHandler() = default;
    ~InteroperableSettingsHandler() = default;
    static void RegisterObserver();
    static std::string QueryMuteRinger();
    static void RecoverMuteRinger(std::string muteRinger);
    static void SendMuteRinger();
 
private:
    static sptr<InteroperableRecvObserver> recvObserver_;
};
}
}
#endif // INTEROPERABLE_SETTINGS_HANDLER_H