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

#include <mutex>

#include "common_type.h"
#include "singleton.h"
#include "bluetooth_hfp_hf.h"

namespace OHOS {
namespace Telephony {
class BluetoothCallConnection : public std::enable_shared_from_this<BluetoothCallConnection> {
    DECLARE_DELAYED_SINGLETON(BluetoothCallConnection)

public:
    int32_t Dial(DialParaInfo &info);
    void SetMacAddress(const std::string &macAddress);
    std::string GetMacAddress();

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

private:
    std::string macAddress_;
    bool isHfpConnected_ = false;
    bool isBtCallScoConnected_ = false;
    std::string hfpPhoneNumber_;
    std::string hfpContactName_;
};
} // namespace Telephony
} // namespace OHOS

#endif
