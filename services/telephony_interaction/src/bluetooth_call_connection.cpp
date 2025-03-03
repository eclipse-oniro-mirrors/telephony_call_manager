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

#include "voip_call_connection.h"

#include "bluetooth_call_connection.h"
#include "telephony_log_wrapper.h"
#include "call_control_manager.h"
#include "settings_datashare_helper.h"
#include "report_call_info_handler.h"

namespace OHOS {
namespace Telephony {
BluetoothCallConnection::BluetoothCallConnection() {}
BluetoothCallConnection::~BluetoothCallConnection() {}

int32_t BluetoothCallConnection::Dial(DialParaInfo &info)
{
    TELEPHONY_LOGI("bluetooth dial start!");
    if (!GetSupportBtCall()) {
        return CALL_ERR_BLUETOOTH_CONNECTION_FAILED;
    }
    std::string number = info.number;
    if (number.empty()) {
        TELEPHONY_LOGE("bluetooth call number is null!");
        return CALL_ERR_DIAL_FAILED;
    }
    if (macAddress_.empty()) {
        TELEPHONY_LOGE("bluetooth call macaddress is empty");
        return CALL_ERR_DIAL_FAILED;
    }
    Bluetooth::HandsFreeUnit *profile = Bluetooth::HandsFreeUnit::GetProfile();
    if (profile == nullptr) {
        TELEPHONY_LOGE("profile is nullptr");
        return CALL_ERR_DIAL_FAILED;
    }

    Bluetooth::BluetoothRemoteDevice device(macAddress_);
    profile->StartDial(device, number);
    return TELEPHONY_SUCCESS;
}

void BluetoothCallConnection::SetMacAddress(const std::string &macAddress)
{
    macAddress_ = macAddress;
    if (macAddress_.empty()) {
        TELEPHONY_LOGE("BluetoothCallConnection macAddress is empty");
    }
}

std::string BluetoothCallConnection::GetMacAddress()
{
    return macAddress_;
}

int32_t BluetoothCallConnection::ConnectBtSco()
{
    TELEPHONY_LOGI("connectBtSco event");
    Bluetooth::HandsFreeUnit *profile = Bluetooth::HandsFreeUnit::GetProfile();
    if (profile != nullptr) {
        Bluetooth::BluetoothRemoteDevice device(macAddress_, 1);
        bool isOK = profile->ConnectSco(device);
        if (!isOK) {
            TELEPHONY_LOGE("connectBtSco failed!");
            return TELEPHONY_ERR_FAIL;
        }
        return TELEPHONY_SUCCESS;
    } else {
        TELEPHONY_LOGE("profile is nullptr");
    }
    return TELEPHONY_ERR_FAIL;
}

int32_t BluetoothCallConnection::DisConnectBtSco()
{
    TELEPHONY_LOGI("disconnectBtSco event");
    Bluetooth::HandsFreeUnit *profile = Bluetooth::HandsFreeUnit::GetProfile();
    if (profile != nullptr) {
        Bluetooth::BluetoothRemoteDevice device(macAddress_, 1);
        bool isOK = profile->DisconnectSco(device);
        if (!isOK) {
            TELEPHONY_LOGE("disconnectBtSco failed!");
            return TELEPHONY_ERR_FAIL;
        }
        return TELEPHONY_SUCCESS;
    } else {
        TELEPHONY_LOGE("profile is nullptr");
    }
    return TELEPHONY_ERR_FAIL;
}

bool BluetoothCallConnection::GetBtScoIsConnected()
{
    TELEPHONY_LOGI("getBtScoconnect state event");
    Bluetooth::HandsFreeUnit *profile = Bluetooth::HandsFreeUnit::GetProfile();
    if (profile != nullptr) {
        Bluetooth::BluetoothRemoteDevice device(macAddress_, 1);
        int state = profile->GetScoState(device);
        if (state == static_cast<int>(Bluetooth::HfpScoConnectState::SCO_CONNECTED)) {
            TELEPHONY_LOGE("BtScoconnect on!");
            return true;
        }
    } else {
        TELEPHONY_LOGE("profile is nullptr");
    }
    TELEPHONY_LOGI("BtScoconnect off!");
    return false;
}

void BluetoothCallConnection::SetHfpConnected(bool isHfpConnected)
{
    isHfpConnected_ = isHfpConnected;
    TELEPHONY_LOGI("Set hfpCconnectd=%{public}d", isHfpConnected_);
    if (!isHfpConnected_) {
        HfpDisConnectedEndBtCall();
    }
}

bool BluetoothCallConnection::GetSupportBtCall()
{
    Bluetooth::BluetoothRemoteDevice device(macAddress_);
    bool isAclConnected = device.IsAclConnected();
    if (isAclConnected) {
        TELEPHONY_LOGI("Watch Support Bluetooth Call.");
    } else {
        TELEPHONY_LOGE("Watch not Support Bluetooth Call.");
    }
    return isAclConnected;
}

void BluetoothCallConnection::SetBtCallScoConnected(bool isBtCallScoConnected)
{
    isBtCallScoConnected_ = isBtCallScoConnected;
    TELEPHONY_LOGI("Set BtCallScoConnected=%{public}d", isBtCallScoConnected_);
}

bool BluetoothCallConnection::GetBtCallScoConnected()
{
    TELEPHONY_LOGI("Get BtCallScoConnected=%{public}d", isBtCallScoConnected_);
    return isBtCallScoConnected_;
}

void BluetoothCallConnection::HfpDisConnectedEndBtCall()
{
    sptr<CallBase> call = CallObjectManager::GetOneCallObject(CallRunningState::CALL_RUNNING_STATE_DIALING);
    if (call == nullptr || call->GetCallType() != CallType::TYPE_BLUETOOTH) {
        return;
    }
    TELEPHONY_LOGW("When BluetoothCall dialing Hfp is disconnected.");
    CallAttributeInfo info;
    (void)memset_s(&info, sizeof(CallAttributeInfo), 0, sizeof(CallAttributeInfo));
    call->GetCallAttributeBaseInfo(info);
    CallDetailInfo detailInfo;
    detailInfo.callType = info.callType;
    detailInfo.accountId = info.accountId;
    detailInfo.index = info.index;
    detailInfo.state = TelCallState::CALL_STATUS_DISCONNECTED;
    (void)memcpy_s(detailInfo.phoneNum, kMaxNumberLen, info.accountNumber, kMaxNumberLen);
    (void)memset_s(detailInfo.bundleName, kMaxBundleNameLen  + 1, 0, kMaxBundleNameLen + 1);
    int32_t ret = DelayedSingleton<ReportCallInfoHandler>::GetInstance()->UpdateCallReportInfo(detailInfo);
    if (ret != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("UpdateCallReportInfo failed! errCode:%{public}d", ret);
    } else {
        TELEPHONY_LOGW("UpdateCallReportInfo success! state:%{public}d, index:%{public}d",
            detailInfo.state, detailInfo.index);
    }
}

void BluetoothCallConnection::SetHfpPhoneNumber(const std::string &hfpPhoneNumber)
{
    TELEPHONY_LOGI("hfpPhoneNumber length = %{public}lu", hfpPhoneNumber.length());
    hfpPhoneNumber_ = hfpPhoneNumber;
}

void BluetoothCallConnection::SetHfpContactName(const std::string &hfpContactName)
{
    TELEPHONY_LOGI("hfpContactName length = %{public}lu", hfpContactName.length());
    hfpContactName_ = hfpContactName;
}

std::string BluetoothCallConnection::GetHfpContactName(const std::string &hfpPhoneNumber)
{
    if (!hfpPhoneNumber_.empty() && hfpPhoneNumber == hfpPhoneNumber_) {
        TELEPHONY_LOGI("got name.");
        return hfpContactName_;
    }
    return "";
}
} // namespace Telephony
} // namespace OHOS
