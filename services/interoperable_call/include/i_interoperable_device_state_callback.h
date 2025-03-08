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
 
#ifndef INTEROPERABLE_DEVICE_STATE_CALLBACK_I_H
#define INTEROPERABLE_DEVICE_STATE_CALLBACK_I_H

#include <string>

namespace OHOS {
namespace Telephony {
class IInteroperableDeviceStateCallback {
public:
    virtual ~IInteroperableDeviceStateCallback() = default;
    virtual void OnDeviceOnline(const std::string &networkId, const std::string &devName, uint16_t devType) = 0;
    virtual void OnDeviceOffline(const std::string &networkId, const std::string &devName, uint16_t devType) = 0;
};
}
}
#endif // INTEROPERABLE_DEVICE_STATE_CALLBACK_I_H