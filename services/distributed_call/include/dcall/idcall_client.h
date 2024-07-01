/*
 * Copyright (C) 2023 Huawei Device Co., Ltd.
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

#ifndef OHOS_IDCALL_CLIENT_H
#define OHOS_IDCALL_CLIENT_H

#include <cstdint>
#include <iostream>
#include <string>

#include "idcall_device_callback.h"
#include "dcall/dcall_device_info.h"

namespace OHOS {
namespace DistributedHardware {
const std::string DCALL_LOADER_GET_DCALL_CLIENT_HANDLER = "GetDCallClientInstance";

class IDCallClient {
public:
    virtual ~IDCallClient() = default;
    virtual int32_t Init() = 0;
    virtual int32_t UnInit() = 0;
    virtual int32_t SwitchDevice(const std::string& devId, int32_t flag) = 0;
    virtual int32_t GetOnlineDeviceList(std::vector<std::string>& devList) = 0;
    virtual int32_t RegisterDeviceCallback(const std::string name,
        const std::shared_ptr<IDCallDeviceCallback>& callback) = 0;
    virtual int32_t UnRegisterDeviceCallback(const std::string& name) = 0;
    virtual int32_t GetDCallDeviceInfo(const std::string &devId, DCallDeviceInfo& devInfo) = 0;
    virtual bool IsSelectVirtualModem() = 0;
};

extern "C" __attribute__((visibility("default"))) IDCallClient* GetDCallClientInstance();

} // namespace DistributedHardware
} // namespace OHOS

#endif // OHOS_IDCALL_CLIENT_H
