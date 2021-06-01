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

#ifndef CELLULAR_CALL_SERVICE_TEST_H
#define CELLULAR_CALL_SERVICE_TEST_H

#include <memory>
#include <mutex>

#include "singleton.h"
#include "system_ability.h"

#include "cellular_call_stub_test.h"

namespace OHOS {
namespace TelephonyCallManager {
enum class ServiceRunningState { STATE_STOPPED, STATE_RUNNING };

class CellularCallServiceTest : public SystemAbility,
                                public CellularCallStubTest,
                                public std::enable_shared_from_this<CellularCallServiceTest> {
    DECLARE_DELAYED_SINGLETON(CellularCallServiceTest)
    DECLEAR_SYSTEM_ABILITY(CellularCallServiceTest)

public:
    /**
     * OnStart
     */
    void OnStart() override;

    /**
     * OnStop
     */
    void OnStop() override;
    /**
     * OnDump
     */
    void OnDump() override;

private:
    /**
     * ServiceRunningState
     */
    ServiceRunningState state_ {ServiceRunningState::STATE_STOPPED};

    std::mutex lock_;

    bool Init() const;
};
} // namespace TelephonyCallManager
} // namespace OHOS

#endif // CELLULAR_CALL_SERVICE_H
