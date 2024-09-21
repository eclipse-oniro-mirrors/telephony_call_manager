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

#ifndef TELEPHONY_DISTRIBUTED_DATA_SINK_CONTROLLER_H
#define TELEPHONY_DISTRIBUTED_DATA_SINK_CONTROLLER_H

#include <map>
#include "ffrt.h"
#include "distributed_data_controller.h"

namespace OHOS {
namespace Telephony {
class DistributedDataSinkController : public DistributedDataController,
                                      public std::enable_shared_from_this<DistributedDataSinkController> {
public:
    DistributedDataSinkController() = default;
    ~DistributedDataSinkController() override = default;
    void OnDeviceOnline(const std::string &devId, const std::string &devName, AudioDeviceType devType) override {}
    void OnDeviceOffline(const std::string &devId, const std::string &devName, AudioDeviceType devType) override {}
    void OnCallCreated(const sptr<CallBase> &call, const std::string &devId) override;
    void OnCallDestroyed() override;
    void ProcessCallInfo(const sptr<CallBase> &call, DistributedDataType type) override;
    void OnConnected() override;

protected:
    void HandleRecvMsg(int32_t msgType, const cJSON *msg) override;

private:
    void ConnectRemote(const std::string &devId);
    void CheckLocalData(const sptr<CallBase> &call, DistributedDataType type);
    std::string CreateDataReqMsg(DistributedMsgType msgType, uint32_t itemType, const std::string &num);
    void SendDataQueryReq();
    void HandleDataQueryRsp(const cJSON *msg);
    void UpdateCallName(sptr<CallBase> &call, const cJSON *msg);
    void UpdateCallLocation(sptr<CallBase> &call, const cJSON *msg);
    void ReportCallInfo(const sptr<CallBase> &call);
    std::string CreateCurrentDataReqMsg(const std::string &num);
    void SendCurrentDataQueryReq();
    void HandleCurrentDataQueryRsp(const cJSON *msg);

private:
    ffrt::mutex mutex_{};
    std::map<std::string, uint32_t> queryInfo_{};
};

} // namespace Telephony
} // namespace OHOS

#endif // TELEPHONY_DISTRIBUTED_DATA_SINK_CONTROLLER_H
