/*
 * Copyright (C) 2021 Huawei Device Co., Ltd.
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

#include "cellular_call_service_test.h"

#include "system_ability_definition.h"

#include "call_manager_log.h"

namespace OHOS {
namespace TelephonyCallManager {
bool g_cellularRegisterResult =
    SystemAbility::MakeAndRegisterAbility(DelayedSingleton<CellularCallServiceTest>::GetInstance().get());

CellularCallServiceTest::CellularCallServiceTest() : SystemAbility(TELEPHONY_CELLULAR_CALL_SYS_ABILITY_ID, true) {}

CellularCallServiceTest::~CellularCallServiceTest() {}

bool CellularCallServiceTest::Init() const
{
    CALLMANAGER_DEBUG_LOG("CellularCallService Init");

    CALLMANAGER_DEBUG_LOG("init success");
    return true;
}

void CellularCallServiceTest::OnStart()
{
    std::lock_guard<std::mutex> guard(lock_);
    CALLMANAGER_DEBUG_LOG("CellularCallService OnStart");
    if (state_ == ServiceRunningState::STATE_RUNNING) {
        CALLMANAGER_INFO_LOG("CellularCallService has already started.");
        return;
    }
    if (!Init()) {
        CALLMANAGER_ERR_LOG("failed to init service.");
        return;
    }
    state_ = ServiceRunningState::STATE_RUNNING;
    bool ret = Publish(DelayedSingleton<CellularCallServiceTest>::GetInstance().get());
    if (!ret) {
        CALLMANAGER_ERR_LOG("Init Publish failed!");
    }
    CALLMANAGER_DEBUG_LOG("CellularCallService start success.");
}

void CellularCallServiceTest::OnStop()
{
    CALLMANAGER_DEBUG_LOG("CellularCallService stop service");
    std::lock_guard<std::mutex> guard(lock_);
    state_ = ServiceRunningState::STATE_STOPPED;
}

void CellularCallServiceTest::OnDump()
{
    CALLMANAGER_DEBUG_LOG("CellularCallService OnDump");
}
} // namespace TelephonyCallManager
} // namespace OHOS