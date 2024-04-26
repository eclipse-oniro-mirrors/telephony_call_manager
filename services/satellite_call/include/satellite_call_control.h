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

#ifndef SATELLITE_CALL_CONTROL_H
#define SATELLITE_CALL_CONTROL_H

#include <cstring>
#include <memory>

#include "call_manager_inner_type.h"
#include "satellite_call.h"
#include "settings_datashare_helper.h"
#include "singleton.h"

namespace OHOS {
namespace Telephony {
class SatelliteCallControl {
    DECLARE_DELAYED_SINGLETON(SatelliteCallControl);
public:
    void SetSatcommTempLevel(int32_t level);
    int32_t IsAllowedSatelliteDialCall();
    int32_t IsSatelliteSwitchEnable();
    SatCommTempLevel GetSatcommTempLevel();
    void HandleSatelliteCallStateUpdate(sptr<CallBase> &call, TelCallState priorState, TelCallState nextState);
    bool IsShowDialog();
    void SetShowDialog(bool isShowDialog);
private:
    void PublishSatelliteConnectEvent();
    int32_t SetSatelliteCallDurationProcessing();
    int32_t SetSatelliteCallCountDownProcessing();
    void RemoveCallDurationEventHandlerTask();
    void RemoveCallCountDownEventHandlerTask();

private:
    SatCommTempLevel SatCommTempLevel_ = SatCommTempLevel::TEMP_LEVEL_LOW;
    bool isShowingDialog_ = false;
    std::shared_ptr<AppExecFwk::EventHandler> CallDurationLimitHandler_;
    std::shared_ptr<AppExecFwk::EventHandler> CallCountDownHandler_;
};
} // namespace Telephony
} // namespace OHOS

#endif //SATELLITE_CALL_CONTROL_H