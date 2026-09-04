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

#ifdef CALL_MANAGER_CALL_TRANSFER
#include "audio_device_manager.h"
#include "bluetooth_call.h"
#include "call_manager_utils.h"
#include "call_ability_report_proxy.h"
#include "call_manager_base.h"
#include "call_dialog.h"
#include "call_number_utils.h"
#include "cpp/task_ext.h"
#include "string_wrapper.h"
#endif

namespace OHOS {
namespace Telephony {
constexpr int32_t ANCS_CONTACT_NAME_CACHE_TIMEOUT = 1000000; // 1s

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
    if (info.phoneIndex == 0 && macAddress_.empty()) {
        TELEPHONY_LOGE("bluetooth call macaddress is empty");
        return CALL_ERR_DIAL_FAILED;
    }
    if (info.phoneIndex == 1 && secondaryPhoneMacAddress_.empty()) {
        TELEPHONY_LOGE("bluetooth call secondaryPhoneMacAddress_ is empty");
        return CALL_ERR_DIAL_FAILED;
    }
    Bluetooth::HandsFreeUnit *profile = Bluetooth::HandsFreeUnit::GetProfile();
    if (profile == nullptr) {
        TELEPHONY_LOGE("profile is nullptr");
        return CALL_ERR_DIAL_FAILED;
    }

    Bluetooth::BluetoothRemoteDevice device(info.phoneIndex == 1 ? secondaryPhoneMacAddress_ : macAddress_);
    profile->StartDial(device, number);
    return TELEPHONY_SUCCESS;
}

void BluetoothCallConnection::SetMacAddress(int32_t phoneIndex, const std::string &macAddress)
{
    if (phoneIndex == 1) {
        secondaryPhoneMacAddress_ = macAddress;
        if (secondaryPhoneMacAddress_.empty()) {
            TELEPHONY_LOGE("secondary phone BluetoothCallConnection macAddress is empty");
        }
    } else {
        macAddress_ = macAddress;
        if (macAddress_.empty()) {
            TELEPHONY_LOGE("BluetoothCallConnection macAddress is empty");
        }
    }
}

std::string BluetoothCallConnection::GetMacAddress(int32_t phoneIndex)
{
    if (phoneIndex == 1) {
        return secondaryPhoneMacAddress_;
    } else {
        return macAddress_;
    }
}
 
std::string BluetoothCallConnection::GetDeviceName(int32_t phoneIndex)
{
    Bluetooth::BluetoothRemoteDevice device(phoneIndex == 1 ? secondaryPhoneMacAddress_ : macAddress_);
    std::string deviceName = device.GetDeviceName();
    return deviceName;
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
#ifdef CALL_MANAGER_CALL_TRANSFER
        CancelHfpWaitContactTask();
#endif
    }
}

bool BluetoothCallConnection::GetSupportBtCall()
{
    Bluetooth::BluetoothRemoteDevice device(macAddress_);
    Bluetooth::BluetoothRemoteDevice secondDevice(secondaryPhoneMacAddress_);
    bool isAclConnected = device.IsAclConnected();
    bool isSecondAclConnected = secondDevice.IsAclConnected();
    if (isAclConnected || isSecondAclConnected) {
        TELEPHONY_LOGI("Watch Support Bluetooth Call.");
    } else {
        TELEPHONY_LOGE("Watch not Support Bluetooth Call.");
    }
    return isAclConnected || isSecondAclConnected;
}

void BluetoothCallConnection::SetBtCallScoConnected(bool isBtCallScoConnected)
{
    std::lock_guard<ffrt::mutex> lock(mutex_);
    isBtCallScoConnected_ = isBtCallScoConnected;
    TELEPHONY_LOGI("Set BtCallScoConnected=%{public}d", isBtCallScoConnected_);
}

bool BluetoothCallConnection::GetBtCallScoConnected()
{
    std::lock_guard<ffrt::mutex> lock(mutex_);
    TELEPHONY_LOGI("Get BtCallScoConnected=%{public}d", isBtCallScoConnected_);
    return isBtCallScoConnected_;
}

void BluetoothCallConnection::HfpDisConnectedEndBtCall()
{
    TELEPHONY_LOGI("hfp disconnected, hangup all bt call.");
    std::list<sptr<CallBase>> allCallList = CallObjectManager::GetAllCallList();
    for (auto call : allCallList) {
        if (call == nullptr || call->GetCallType() != CallType::TYPE_BLUETOOTH) {
            continue;
        }
        CallAttributeInfo info;
        (void)memset_s(&info, sizeof(CallAttributeInfo), 0, sizeof(CallAttributeInfo));
        call->GetCallAttributeBaseInfo(info);
        CallDetailInfo detailInfo;
        detailInfo.callType = info.callType;
        detailInfo.accountId = info.accountId;
        detailInfo.index = call->GetCallIndex();
        detailInfo.state = TelCallState::CALL_STATUS_DISCONNECTED;
        (void)memcpy_s(detailInfo.phoneNum, kMaxNumberLen, info.accountNumber, kMaxNumberLen);
        (void)memset_s(detailInfo.bundleName, kMaxBundleNameLen  + 1, 0, kMaxBundleNameLen + 1);
        int32_t ret = DelayedSingleton<ReportCallInfoHandler>::GetInstance()->UpdateCallReportInfo(detailInfo);
        if (ret != TELEPHONY_SUCCESS) {
            TELEPHONY_LOGE("UpdateCallReportInfo failed! errCode:%{public}d", ret);
        } else {
            TELEPHONY_LOGI("UpdateCallReportInfo success! state:%{public}d, index:%{public}d",
                detailInfo.state, detailInfo.index);
        }
    }
}

void BluetoothCallConnection::SetHfpContactName(const std::string &hfpPhoneNumber, const std::string &hfpContactName)
{
    TELEPHONY_LOGI("hfpPhoneNumber length = %{public}zu, hfpContactName length = %{public}zu",
        hfpPhoneNumber.length(), hfpContactName.length());
    hfpPhoneNumber_ = hfpPhoneNumber;
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

#ifdef CALL_MANAGER_CALL_TRANSFER
int32_t BluetoothCallConnection::RegisterTransferController(const sptr<ITransferControlCallback> &callback)
{
    std::lock_guard<ffrt::mutex> lock(mutex_);
    transferControlCallback_ = callback;
    return 0;
}

int32_t BluetoothCallConnection::UnRegisterTransferController()
{
    std::lock_guard<ffrt::mutex> lock(mutex_);
    transferControlCallback_ = nullptr;
    return 0;
}

void BluetoothCallConnection::NotifyLocalAliveAndTransferIncoming()
{
    CallEventInfo eventInfo;
    (void)memset_s(&eventInfo, sizeof(CallEventInfo), 0, sizeof(CallEventInfo));
    eventInfo.eventId = CallAbilityEventId::EVENT_LOCAL_ALIVE_AND_TRANSFER_INCOMING;
    TELEPHONY_LOGI("BluetoothCallConnection::RemindTransferCallUnhandled, EVENT_LOCAL_ALIVE_AND_TRANSFER_INCOMING");
    DelayedSingleton<CallAbilityReportProxy>::GetInstance()->CallEventUpdated(eventInfo);
}

void BluetoothCallConnection::NotifyTransferCall(const sptr<BluetoothCall> call)
{
    TELEPHONY_LOGI("BluetoothCallConnection::NotifyTransferCall");
    if (call == nullptr) {
        TELEPHONY_LOGE("BluetoothCallConnection::NotifyTransferCall call is nullptr");
        return;
    }

    std::lock_guard<ffrt::mutex> lock(mutex_);
    if (transferControlCallback_ != nullptr) {
        TransferCallInfo transferCallInfo;
        GetTransferCallInfo(call, transferCallInfo);
        int32_t result = transferControlCallback_->OnUpdateTransferCall(transferCallInfo);
        TELEPHONY_LOGI("BluetoothCallConnection::NotifyTransferCall result = %{public}d", result);
    } else {
        TELEPHONY_LOGE("BluetoothCallConnection::NotifyTransferCall transferControlCallback_ is nullptr");
    }

    TelCallState state = call->GetTelCallState();
    bool isBtHeadsetConnect = DelayedSingleton<AudioDeviceManager>::GetInstance()->IsBluetoothHeadsetConnect();
    bool isBluetoothHeadsetWarned = call->IsBluetoothHeadsetWarned();
    TELEPHONY_LOGI("BluetoothCallConnection::NotifyTransferCall state=%{public}d, isBtHeadsetConnect=%{public}d"
        ", isBluetoothHeadsetWarned=%{public}d", state, isBtHeadsetConnect, isBluetoothHeadsetWarned);
    if (state == TelCallState::CALL_STATUS_INCOMING || state == TelCallState::CALL_STATUS_WAITING) {
        if (isBtHeadsetConnect && !isBluetoothHeadsetWarned) {
            TELEPHONY_LOGI("BluetoothCallConnection::NotifyTransferCall, CALL_WARN_OPEN_BLUETOOTH");
            DelayedSingleton<CallDialog>::GetInstance()->DialogConnectExtension("CALL_WARN_OPEN_BLUETOOTH");
            call->SetIsBluetoothHeadsetWarned(true);
        }
    }
}

void BluetoothCallConnection::GetTransferCallInfo(const sptr<BluetoothCall> call, TransferCallInfo &transferCallInfo)
{
    transferCallInfo.devMac = call->GetRemoteAddr();
    transferCallInfo.callId = static_cast<uint32_t>(call->GetCallID());
    transferCallInfo.callDirect = call->GetCallDirection();
    transferCallInfo.callState = call->GetTelCallState();
    transferCallInfo.callType = call->GetCallType();
}

void BluetoothCallConnection::UpdateTransferCall(sptr<BluetoothCall> call)
{
    if (call == nullptr) {
        TELEPHONY_LOGE("BluetoothCallConnection::UpdateTransferCall call is nullptr");
        return;
    }

    if (!CallManagerUtils::IsTransferControlEnable()) {
        if (call->GetCallDirection() == CallDirection::CALL_DIRECTION_OUT) {
            call->SetIsTransferCallAllow(false);
        } else {
            call->SetIsTransferCallAllow(true);
        }

        call->SetRemoteName("unknow");
        return;
    }

    if (transferControlCallback_ == nullptr) {
        TELEPHONY_LOGE("BluetoothCallConnection::UpdateTransferCall transferControlCallback_ is nullptr");
        call->SetIsTransferCallAllow(false);
        return;
    }

    TransferCallInfo transferCallInfo;
    GetTransferCallInfo(call, transferCallInfo);
    TransferResultInfo resultInfo;
    int32_t result = transferControlCallback_->OnGetTransferResult(transferCallInfo, resultInfo);
    TELEPHONY_LOGI("BluetoothCallConnection::UpdateTransferCall result=%{public}d, secPolicy=%{public}d",
        result, resultInfo.secPolicy);
    if (resultInfo.deviceShowName.empty()) {
        TELEPHONY_LOGE("BluetoothCallConnection::CheckTransferCall deviceShowName is invalid");
        call->SetIsTransferCallAllow(false);
        return;
    }

    if (result == 1) {
        call->SetIsTransferCallAllow(true);
        call->SetRemoteName(resultInfo.deviceShowName);
    } else {
        call->SetIsTransferCallAllow(false);
    }
}

void BluetoothCallConnection::ScoDisconnectedEndTransferCall()
{
    TELEPHONY_LOGI("sco disconnected, hangup transfer call.");
    std::list<sptr<CallBase>> allCallList = CallObjectManager::GetAllCallList();
    for (auto call : allCallList) {
        if (call == nullptr || call->GetCallType() != CallType::TYPE_BLUETOOTH) {
            continue;
        }
        sptr<BluetoothCall> btCall = reinterpret_cast<BluetoothCall *>(call.GetRefPtr());
        if (btCall == nullptr || !btCall->IsTransferCall()) {
            continue;
        }
        CallAttributeInfo info;
        (void)memset_s(&info, sizeof(CallAttributeInfo), 0, sizeof(CallAttributeInfo));
        call->GetCallAttributeBaseInfo(info);
        CallDetailInfo detailInfo;
        detailInfo.callType = info.callType;
        detailInfo.accountId = info.accountId;
        detailInfo.index = call->GetCallIndex();
        detailInfo.state = TelCallState::CALL_STATUS_DISCONNECTED;
        (void)memcpy_s(detailInfo.phoneNum, kMaxNumberLen, info.accountNumber, kMaxNumberLen);
        (void)memset_s(detailInfo.bundleName, kMaxBundleNameLen  + 1, 0, kMaxBundleNameLen + 1);
        auto reportCallInfoHandler = DelayedSingleton<ReportCallInfoHandler>::GetInstance();
        if (reportCallInfoHandler == nullptr) {
            break;
        }
        int32_t ret = reportCallInfoHandler->UpdateCallReportInfo(detailInfo);
        if (ret != TELEPHONY_SUCCESS) {
            TELEPHONY_LOGE("UpdateCallReportInfo failed! errCode:%{public}d", ret);
        }
    }
}

void BluetoothCallConnection::CacheAncsContactName(const std::string &contactName)
{
    ConsumeAncsContactName();
    {
        std::lock_guard<ffrt::mutex> lock(ancsMutex_);
        ancsAlreadyArrived_ = true;
    }
    if (CallManagerUtils::IsPurePhoneNumber(contactName)) {
        TELEPHONY_LOGI("contactName is pure phone number, will query yellow page when call arrives");
        std::lock_guard<ffrt::mutex> lock(ancsMutex_);
        isAncsNotifiedPhoneNumber = true;
        return;
    }
    {
        std::lock_guard<ffrt::mutex> lock(ancsMutex_);
        ancsContactName_ = contactName;
    }
    ancsCacheTimeoutHandle_ = ffrt::submit_h([this]() {
        std::lock_guard<ffrt::mutex> lock(ancsMutex_);
        TELEPHONY_LOGI("ANCS contact cache timeout, clearing");
        ancsContactName_.clear();
    }, {}, {}, ffrt::task_attr().delay(ANCS_CONTACT_NAME_CACHE_TIMEOUT));
}

void BluetoothCallConnection::StartHfpWaitContactTask(sptr<BluetoothCall> call)
{
    if (call == nullptr) {
        TELEPHONY_LOGE("StartHfpWaitContactTask call is nullptr");
        return;
    }
    TELEPHONY_LOGI("StartHfpWaitContactTask, index:%{public}d", call->GetCallIndex());
    CancelHfpWaitContactTask();
    wptr<BluetoothCall> callWeakPtr(call);
    hfpWaitContactHandle_ = ffrt::submit_h([callWeakPtr]() {
        sptr<BluetoothCall> btCall = callWeakPtr.promote();
        if (btCall == nullptr) {
            TELEPHONY_LOGE("StartHfpWaitContactTask call is gone after 1s");
            return;
        }
        TelCallState state = btCall->GetTelCallState();
        if (state != TelCallState::CALL_STATUS_INCOMING &&
            state != TelCallState::CALL_STATUS_WAITING) {
            TELEPHONY_LOGI("StartHfpWaitContactTask call state changed, skip number identity");
            return;
        }
        auto callNumberUtils = DelayedSingleton<CallNumberUtils>::GetInstance();
        if (callNumberUtils == nullptr) {
            TELEPHONY_LOGE("CallNumberUtils is nullptr");
            return;
        }
        callNumberUtils->YellowPageAndMarkUpdate(btCall);
    }, {}, {}, ffrt::task_attr().delay(ANCS_CONTACT_NAME_CACHE_TIMEOUT));
}

std::string BluetoothCallConnection::ConsumeAncsContactName()
{
    TELEPHONY_LOGI("ConsumeAncsContactName");
    if (ancsCacheTimeoutHandle_ != nullptr) {
        ffrt::skip(ancsCacheTimeoutHandle_);
        ancsCacheTimeoutHandle_ = nullptr;
    }
    std::lock_guard<ffrt::mutex> lock(ancsMutex_);
    std::string result = ancsContactName_;
    ancsAlreadyArrived_ = false;
    isAncsNotifiedPhoneNumber = false;
    ancsContactName_.clear();
    return result;
}

void BluetoothCallConnection::CancelHfpWaitContactTask()
{
    TELEPHONY_LOGI("CancelHfpWaitContactTask");
    if (hfpWaitContactHandle_ != nullptr) {
        ffrt::skip(hfpWaitContactHandle_);
        hfpWaitContactHandle_ = nullptr;
    }
}

void BluetoothCallConnection::NotifyAncsContactArrived(
    const std::string &contactName, sptr<BluetoothCall> call)
{
    if (call == nullptr) {
        TELEPHONY_LOGE("NotifyAncsContactArrived call is nullptr");
        return;
    }
    TELEPHONY_LOGI("NotifyAncsContactArrived, index:%{public}d", call->GetCallIndex());
    CancelHfpWaitContactTask();
    call->SetTransferCallContactName(contactName);
    ContactInfo contactInfo = call->GetCallerInfo();
    contactInfo.name = contactName;
    call->SetCallerInfo(contactInfo);
    AAFwk::WantParams params = call->GetExtraParams();
    params.SetParam("name", AAFwk::String::Box(contactName));
    call->SetExtraParams(params);
    CallAttributeInfo attrInfo;
    call->GetCallAttributeInfo(attrInfo);
    auto abilityProxy = DelayedSingleton<CallAbilityReportProxy>::GetInstance();
    if (abilityProxy != nullptr) {
        abilityProxy->ReportCallStateInfo(attrInfo);
    }
}

bool BluetoothCallConnection::HasAncsAlreadyArrived()
{
    std::lock_guard<ffrt::mutex> lock(ancsMutex_);
    return ancsAlreadyArrived_;
}

bool BluetoothCallConnection::IsAncsPhoneNumber()
{
    std::lock_guard<ffrt::mutex> lock(ancsMutex_);
    return isAncsNotifiedPhoneNumber;
}

void BluetoothCallConnection::NotifyFirstTransferCallSwitchBack(const sptr<BluetoothCall> secondActiveCall)
{
    if (secondActiveCall == nullptr) {
        TELEPHONY_LOGE("NotifyFirstTransferCallSwitchBack secondActiveCall is nullptr");
        return;
    }
    std::lock_guard<ffrt::mutex> lock(mutex_);
    if (transferControlCallback_ == nullptr) {
        TELEPHONY_LOGE("NotifyFirstTransferCallSwitchBack transferControlCallback_ is nullptr");
        return;
    }
    std::string devMac = secondActiveCall->GetRemoteAddr();
    int32_t result = transferControlCallback_->OnDisconnectSco(devMac);
    TELEPHONY_LOGI("NotifyFirstTransferCallSwitchBack OnDisconnectSco result=%{public}d", result);
}

void BluetoothCallConnection::DisconnectScoByTransferControl()
{
    std::lock_guard<ffrt::mutex> lock(mutex_);
    if (macAddress_.empty()) {
        TELEPHONY_LOGE("BluetoothCallConnection::DisconnectScoByTransferControl macAddress_ is empty");
        return;
    }

    if (transferControlCallback_ == nullptr) {
        TELEPHONY_LOGE("DisconnectScoByTransferControl transferControlCallback_ is nullptr");
        return;
    }
    int32_t result = transferControlCallback_->OnDisconnectSco(macAddress_);
    TELEPHONY_LOGI("BluetoothCallConnection::DisconnectScoByTransferControl result=%{public}d", result);
}

void BluetoothCallConnection::HandleTransferCallSwitchBack(const sptr<CallBase> &activeCall)
{
    if (activeCall == nullptr) {
        return;
    }
    sptr<CallBase> otherTransferCall = CallObjectManager::GetOtherBtTransferCall(activeCall->GetCallID());
    if (otherTransferCall == nullptr) {
        return;
    }
    BluetoothCall *secondActiveCall = reinterpret_cast<BluetoothCall *>(activeCall.GetRefPtr());
    if (secondActiveCall == nullptr) {
        return;
    }
    TELEPHONY_LOGI("Two transfer calls exist, notify first transfer call switch back");
    NotifyFirstTransferCallSwitchBack(secondActiveCall);
}
#endif
} // namespace Telephony
} // namespace OHOS
