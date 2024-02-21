/*
 * Copyright (C) 2023-2024 Huawei Device Co., Ltd.
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

#include "app_state_observer.h"

#include "iservice_registry.h"
#include "telephony_log_wrapper.h"
#include "i_call_ability_callback.h"
#include "call_ability_report_proxy.h"
#include "call_control_manager.h"
#include "ffrt.h"

namespace OHOS {
namespace Telephony {
namespace {
    const std::string TAG = "AppStateObserver";
    ffrt::queue unregisterCallbackQueue { "unregister_callback" };
}
ApplicationStateObserver::ApplicationStateObserver() {}

ApplicationStateObserver::~ApplicationStateObserver() {}

void ApplicationStateObserver::OnProcessDied(const AppExecFwk::ProcessData& processData)
{
    auto report = DelayedSingleton<CallAbilityReportProxy>::GetInstance();
    if (report == nullptr) {
        TELEPHONY_LOGE("report is nullptr");
        return;
    }
    unregisterCallbackQueue.submit([=]() { report->UnRegisterCallBack(std::to_string(processData.pid)); });
}

} //namespace Telephony
} //namespace OHOS