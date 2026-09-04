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

#ifndef BLUETOOTH_CALL_CONNECTION_H
#define BLUETOOTH_CALL_CONNECTION_H

#include "ffrt.h"

#include "common_type.h"
#include "singleton.h"
#include "bluetooth_hfp_hf.h"

#ifdef CALL_MANAGER_CALL_TRANSFER
#include "i_transfer_control_callback.h"
#endif

namespace OHOS {
namespace Telephony {
#ifdef CALL_MANAGER_CALL_TRANSFER
class BluetoothCall;
class CallBase;
#endif

class BluetoothCallConnection : public std::enable_shared_from_this<BluetoothCallConnection> {
    DECLARE_DELAYED_SINGLETON(BluetoothCallConnection)

public:
    int32_t Dial(DialParaInfo &info);
    void SetMacAddress(int32_t phoneIndex, const std::string &macAddress);
    std::string GetMacAddress(int32_t phoneIndex);
    std::string GetDeviceName(int32_t phoneIndex);

    int32_t ConnectBtSco();
    int32_t DisConnectBtSco();
    bool GetBtScoIsConnected();

    void SetHfpConnected(bool isHfpConnected);
    bool GetSupportBtCall();
    void SetBtCallScoConnected(bool isBtCallScoConnected);
    bool GetBtCallScoConnected();
    void HfpDisConnectedEndBtCall();
    void SetHfpContactName(const std::string &hfpPhoneNumber, const std::string &hfpContactName);
    std::string GetHfpContactName(const std::string &hfpPhoneNumber);

#ifdef CALL_MANAGER_CALL_TRANSFER
    void NotifyTransferCall(const sptr<BluetoothCall> call);
    void UpdateTransferCall(sptr<BluetoothCall> call);
    void NotifyLocalAliveAndTransferIncoming();

    int32_t RegisterTransferController(const sptr<ITransferControlCallback> &callback);
    int32_t UnRegisterTransferController();
    void ScoDisconnectedEndTransferCall();
    void CacheAncsContactName(const std::string &contactName);
    std::string ConsumeAncsContactName();
    void StartHfpWaitContactTask(sptr<BluetoothCall> call);
    void CancelHfpWaitContactTask();
    void NotifyAncsContactArrived(const std::string &contactName, sptr<BluetoothCall> call);
    bool HasAncsAlreadyArrived();
    bool IsAncsPhoneNumber();
    void NotifyFirstTransferCallSwitchBack(const sptr<BluetoothCall> secondActiveCall);
    void DisconnectScoByTransferControl();
    void HandleTransferCallSwitchBack(const sptr<CallBase> &activeCall);
#endif

private:
    std::string macAddress_;
    std::string secondaryPhoneMacAddress_;
    bool isHfpConnected_ = false;
    bool isBtCallScoConnected_ = false;
    std::string hfpPhoneNumber_;
    std::string hfpContactName_;
    ffrt::mutex mutex_;

#ifdef CALL_MANAGER_CALL_TRANSFER
    void GetTransferCallInfo(const sptr<BluetoothCall> call, TransferCallInfo &transferCallInfo);

    sptr<ITransferControlCallback> transferControlCallback_ = nullptr;
    std::list<CallDetailInfo> cachedTransferCallList_;
    std::map<int32_t, std::string> cachedTransferContactMap_;
    std::string ancsContactName_;
    bool ancsAlreadyArrived_ = false;
    bool isAncsNotifiedPhoneNumber = false;
    ffrt::mutex ancsMutex_;
    ffrt::task_handle hfpWaitContactHandle_ = nullptr;
    ffrt::task_handle ancsCacheTimeoutHandle_ = nullptr;
#endif
};
} // namespace Telephony
} // namespace OHOS

#endif
