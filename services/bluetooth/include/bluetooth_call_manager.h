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

#ifndef TELEPHONY_BLUETOOTH_CALL_MANAGER_H
#define TELEPHONY_BLUETOOTH_CALL_MANAGER_H

#include "singleton.h"

#include "bluetooth_connection.h"

namespace OHOS {
namespace Telephony {
constexpr int16_t DEFAULT_CALL_NUMBER_TYPE = 86;

class BluetoothCallManager : public std::enable_shared_from_this<BluetoothCallManager> {
public:
    BluetoothCallManager();
    ~BluetoothCallManager();
    int32_t SendBtCallState(int32_t numActive, int32_t numHeld, int32_t callState, const std::string &number);
    int32_t SendCallDetailsChange(int32_t callId, int32_t callState);
    bool IsBtAvailble();

private:
    std::shared_ptr<BluetoothConnection> btConnection_;
};
} // namespace Telephony
} // namespace OHOS
#endif