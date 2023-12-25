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

#ifndef TELEPHONY_DISTRIBUTED_CALL_PROXY_H
#define TELEPHONY_DISTRIBUTED_CALL_PROXY_H

#include <mutex>
#include <memory>

#include "idcall_device_callback.h"
#include "dcall/idcall_client.h"

namespace OHOS {
namespace Telephony {
class DistributedCallProxy {
public:
    DistributedCallProxy();
    ~DistributedCallProxy();

    int32_t Init();
    int32_t UnInit();
    int32_t SwitchDevice(const std::string& devId, int32_t flag);
    int32_t GetOnlineDeviceList(std::vector<std::string>& devList);
    int32_t RegisterDeviceCallback(const std::string name,
        const std::shared_ptr<OHOS::DistributedHardware::IDCallDeviceCallback>& callback);
    int32_t UnRegisterDeviceCallback(const std::string& name);
    int32_t GetDCallDeviceInfo(const std::string &devId, OHOS::DistributedHardware::DCallDeviceInfo& devInfo);

private:
    OHOS::DistributedHardware::IDCallClient* GetDCallClient();

private:
    std::mutex dcallClientMtx_;
    OHOS::DistributedHardware::IDCallClient* dcallClient_ = nullptr;
    void *dCallClientHandler_ = nullptr;
};
} // namespace Telephony
} // namespace OHOS

#endif // TELEPHONY_DISTRIBUTED_CALL_PROXY_H