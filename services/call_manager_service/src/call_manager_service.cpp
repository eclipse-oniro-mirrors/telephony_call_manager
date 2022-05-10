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

#include "call_manager_service.h"

#include "ipc_skeleton.h"

#include "call_manager_errors.h"
#include "telephony_log_wrapper.h"
#include "telephony_permission.h"

#include "bluetooth_call_service.h"
#include "call_ability_report_proxy.h"
#include "call_manager_dump_helper.h"
#include "report_call_info_handler.h"
#include "cellular_call_connection.h"
#include "call_records_manager.h"
#include "common_type.h"

namespace OHOS {
namespace Telephony {
const std::string OHOS_PERMISSION_SET_TELEPHONY_STATE = "ohos.permission.SET_TELEPHONY_STATE";
const std::string OHOS_PERMISSION_PLACE_CALL = "ohos.permission.PLACE_CALL";

const bool g_registerResult =
    SystemAbility::MakeAndRegisterAbility(DelayedSingleton<CallManagerService>::GetInstance().get());

CallManagerService::CallManagerService()
    : SystemAbility(TELEPHONY_CALL_MANAGER_SYS_ABILITY_ID, true), callControlManagerPtr_(nullptr)
{}

CallManagerService::~CallManagerService()
{
    UnInit();
}

bool CallManagerService::Init()
{
    if (!DelayedSingleton<CallControlManager>::GetInstance()->Init()) {
        TELEPHONY_LOGE("CallControlManager init failed!");
        return false;
    }
    callControlManagerPtr_ = DelayedSingleton<CallControlManager>::GetInstance();
    if (callControlManagerPtr_ == nullptr) {
        TELEPHONY_LOGE("callControlManagerPtr_ is nullptr!");
        return false;
    }
    DelayedSingleton<ReportCallInfoHandlerService>::GetInstance()->Start();
    DelayedSingleton<CellularCallConnection>::GetInstance()->Init(TELEPHONY_CELLULAR_CALL_SYS_ABILITY_ID);
    DelayedSingleton<CallRecordsManager>::GetInstance()->Init();
    DelayedSingleton<BluetoothConnection>::GetInstance()->Init();
    return true;
}

void CallManagerService::UnInit()
{
    callControlManagerPtr_ = nullptr;
}

void CallManagerService::OnStart()
{
    using namespace std::chrono;
    time_point<high_resolution_clock> beginTime = high_resolution_clock::now();
    std::lock_guard<std::mutex> guard(lock_);
    if (state_ == ServiceRunningState::STATE_RUNNING) {
        return;
    }

    if (!Init()) {
        TELEPHONY_LOGE("Leave, init failed!");
        return;
    }

    bool ret = SystemAbility::Publish(DelayedSingleton<CallManagerService>::GetInstance().get());
    if (!ret) {
        TELEPHONY_LOGE("Leave, publishing CallManagerService failed!");
        return;
    }
    TELEPHONY_LOGI("Publish CallManagerService SUCCESS");

    state_ = ServiceRunningState::STATE_RUNNING;
    struct tm *timeNow = nullptr;
    struct tm nowTime = {0};
    time_t second = time(0);
    if (second < 0) {
        return;
    }
    timeNow = localtime_r(&second,  &nowTime);
    if (timeNow != nullptr) {
        spendTime_ = duration_cast<std::chrono::milliseconds>(high_resolution_clock::now() - beginTime).count();
        TELEPHONY_LOGI(
            "CallManagerService start time:%{public}d-%{public}d-%{public}d %{public}d:%{public}d:%{public}d",
            timeNow->tm_year + startTime_, timeNow->tm_mon + extraMonth_, timeNow->tm_mday, timeNow->tm_hour,
            timeNow->tm_min, timeNow->tm_sec);
        TELEPHONY_LOGI("CallManagerService start service cost time:%{public}d(milliseconds)", spendTime_);
    }
}

void CallManagerService::OnStop()
{
    std::lock_guard<std::mutex> guard(lock_);
    struct tm *timeNow = nullptr;
    struct tm nowTime = {0};
    time_t second = time(0);
    if (second < 0) {
        return;
    }
    timeNow = localtime_r(&second,  &nowTime);
    if (timeNow != nullptr) {
        TELEPHONY_LOGI(
            "CallManagerService dump time:%{public}d-%{public}d-%{public}d %{public}d:%{public}d:%{public}d",
            timeNow->tm_year + startTime_, timeNow->tm_mon + extraMonth_, timeNow->tm_mday, timeNow->tm_hour,
            timeNow->tm_min, timeNow->tm_sec);
    }
    state_ = ServiceRunningState::STATE_STOPPED;
}

int32_t CallManagerService::Dump(std::int32_t fd, const std::vector<std::u16string> &args)
{
    if (fd < 0) {
        TELEPHONY_LOGE("dump fd invalid");
        return TELEPHONY_ERR_ARGUMENT_INVALID;
    }
    std::vector<std::string> argsInStr;
    for (const auto &arg : args) {
        TELEPHONY_LOGI("Dump args: %s", Str16ToStr8(arg).c_str());
        argsInStr.emplace_back(Str16ToStr8(arg));
    }
    std::string result;
    CallManagerDumpHelper dumpHelper;
    if (dumpHelper.Dump(argsInStr, result)) {
        TELEPHONY_LOGI("%s", result.c_str());
        std::int32_t ret = dprintf(fd, "%s", result.c_str());
        if (ret < 0) {
            TELEPHONY_LOGE("dprintf to dump fd failed");
            return CALL_ERR_SERVICE_DUMP_FAILED;
        }
        return TELEPHONY_SUCCESS;
    }
    TELEPHONY_LOGW("dumpHelper failed");
    return CALL_ERR_SERVICE_DUMP_FAILED;
}

std::string CallManagerService::GetBindTime()
{
    bindTime_ =
        std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch())
            .count();
    std::ostringstream oss;
    oss << bindTime_;
    return oss.str();
}

std::string CallManagerService::GetStartServiceSpent()
{
    std::ostringstream oss;
    oss << spendTime_;
    return oss.str();
}

int32_t CallManagerService::RegisterCallBack(const sptr<ICallAbilityCallback> &callback)
{
    int32_t uid = IPCSkeleton::GetCallingUid();
    std::string bundleName = "";
    TelephonyPermission::GetBundleNameByUid(uid, bundleName);
    if (bundleName.empty()) {
        bundleName.append(std::to_string(uid));
        bundleName.append(std::to_string(IPCSkeleton::GetCallingPid()));
    } else {
        if (CheckBundleName(bundleName) != TELEPHONY_SUCCESS) {
            TELEPHONY_LOGE("%{public}s no permission to register callback", bundleName.c_str());
            return TELEPHONY_ERR_PERMISSION_ERR;
        }
    }
    return DelayedSingleton<CallAbilityReportProxy>::GetInstance()->RegisterCallBack(callback, bundleName);
}

int32_t CallManagerService::UnRegisterCallBack()
{
    int32_t uid = IPCSkeleton::GetCallingUid();
    std::string bundleName = "";
    TelephonyPermission::GetBundleNameByUid(uid, bundleName);
    if (bundleName.empty()) {
        bundleName.append(std::to_string(uid));
        bundleName.append(std::to_string(IPCSkeleton::GetCallingPid()));
    }
    return DelayedSingleton<CallAbilityReportProxy>::GetInstance()->UnRegisterCallBack(bundleName);
}

int32_t CallManagerService::DialCall(std::u16string number, AppExecFwk::PacMap &extras)
{
    int32_t uid = IPCSkeleton::GetCallingUid();
    std::string bundleName = "";
    TelephonyPermission::GetBundleNameByUid(uid, bundleName);
    extras.PutStringValue("bundleName", bundleName);
    if (!TelephonyPermission::CheckPermission(OHOS_PERMISSION_PLACE_CALL)) {
        TELEPHONY_LOGE("Permission denied!");
        return TELEPHONY_ERR_PERMISSION_ERR;
    }
    if (callControlManagerPtr_ != nullptr) {
        return callControlManagerPtr_->DialCall(number, extras);
    } else {
        TELEPHONY_LOGE("callControlManagerPtr_ is nullptr!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
}

int32_t CallManagerService::AnswerCall(int32_t callId, int32_t videoState)
{
    if (callControlManagerPtr_ != nullptr) {
        return callControlManagerPtr_->AnswerCall(callId, videoState);
    } else {
        TELEPHONY_LOGE("callControlManagerPtr_ is nullptr!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
}

int32_t CallManagerService::RejectCall(int32_t callId, bool rejectWithMessage, std::u16string textMessage)
{
    if (callControlManagerPtr_ != nullptr) {
        return callControlManagerPtr_->RejectCall(callId, rejectWithMessage, textMessage);
    } else {
        TELEPHONY_LOGE("callControlManagerPtr_ is nullptr!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
}

int32_t CallManagerService::HangUpCall(int32_t callId)
{
    if (callControlManagerPtr_ != nullptr) {
        return callControlManagerPtr_->HangUpCall(callId);
    } else {
        TELEPHONY_LOGE("callControlManagerPtr_ is nullptr!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
}

int32_t CallManagerService::GetCallState()
{
    if (callControlManagerPtr_ != nullptr) {
        return callControlManagerPtr_->GetCallState();
    } else {
        TELEPHONY_LOGE("callControlManagerPtr_ is nullptr!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
}

int32_t CallManagerService::HoldCall(int32_t callId)
{
    if (callControlManagerPtr_ != nullptr) {
        return callControlManagerPtr_->HoldCall(callId);
    } else {
        TELEPHONY_LOGE("callControlManagerPtr_ is nullptr!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
}

int32_t CallManagerService::UnHoldCall(int32_t callId)
{
    if (callControlManagerPtr_ != nullptr) {
        return callControlManagerPtr_->UnHoldCall(callId);
    } else {
        TELEPHONY_LOGE("callControlManagerPtr_ is nullptr!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
}

int32_t CallManagerService::SwitchCall(int32_t callId)
{
    if (callControlManagerPtr_ != nullptr) {
        return callControlManagerPtr_->SwitchCall(callId);
    } else {
        TELEPHONY_LOGE("callControlManagerPtr_ is nullptr!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
}

bool CallManagerService::HasCall()
{
    if (callControlManagerPtr_ != nullptr) {
        return callControlManagerPtr_->HasCall();
    } else {
        TELEPHONY_LOGE("callControlManagerPtr_ is nullptr!");
        return false;
    }
}

bool CallManagerService::IsNewCallAllowed()
{
    if (callControlManagerPtr_ != nullptr) {
        return callControlManagerPtr_->IsNewCallAllowed();
    } else {
        TELEPHONY_LOGE("callControlManagerPtr_ is nullptr!");
        return false;
    }
}

bool CallManagerService::IsRinging()
{
    if (!TelephonyPermission::CheckPermission(OHOS_PERMISSION_SET_TELEPHONY_STATE)) {
        TELEPHONY_LOGE("Permission denied!");
        return false;
    }
    if (callControlManagerPtr_ != nullptr) {
        return callControlManagerPtr_->IsRinging();
    } else {
        TELEPHONY_LOGE("callControlManagerPtr_ is nullptr!");
        return false;
    }
}

bool CallManagerService::IsInEmergencyCall()
{
    if (!TelephonyPermission::CheckPermission(OHOS_PERMISSION_SET_TELEPHONY_STATE)) {
        TELEPHONY_LOGE("Permission denied!");
        return false;
    }
    if (callControlManagerPtr_ != nullptr) {
        return callControlManagerPtr_->HasEmergency();
    } else {
        TELEPHONY_LOGE("callControlManagerPtr_ is nullptr!");
        return false;
    }
}

int32_t CallManagerService::StartDtmf(int32_t callId, char str)
{
    if (callControlManagerPtr_ != nullptr) {
        return callControlManagerPtr_->StartDtmf(callId, str);
    } else {
        TELEPHONY_LOGE("callControlManagerPtr_ is nullptr!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
}

int32_t CallManagerService::StopDtmf(int32_t callId)
{
    if (callControlManagerPtr_ != nullptr) {
        return callControlManagerPtr_->StopDtmf(callId);
    } else {
        TELEPHONY_LOGE("callControlManagerPtr_ is nullptr!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
}

int32_t CallManagerService::GetCallWaiting(int32_t slotId)
{
    if (callControlManagerPtr_ != nullptr) {
        return callControlManagerPtr_->GetCallWaiting(slotId);
    } else {
        TELEPHONY_LOGE("callControlManagerPtr_ is nullptr!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
}

int32_t CallManagerService::SetCallWaiting(int32_t slotId, bool activate)
{
    if (callControlManagerPtr_ != nullptr) {
        return callControlManagerPtr_->SetCallWaiting(slotId, activate);
    } else {
        TELEPHONY_LOGE("callControlManagerPtr_ is nullptr!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
}

int32_t CallManagerService::GetCallRestriction(int32_t slotId, CallRestrictionType type)
{
    if (callControlManagerPtr_ != nullptr) {
        return callControlManagerPtr_->GetCallRestriction(slotId, type);
    } else {
        TELEPHONY_LOGE("callControlManagerPtr_ is nullptr!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
}

int32_t CallManagerService::SetCallRestriction(int32_t slotId, CallRestrictionInfo &info)
{
    if (callControlManagerPtr_ != nullptr) {
        return callControlManagerPtr_->SetCallRestriction(slotId, info);
    } else {
        TELEPHONY_LOGE("callControlManagerPtr_ is nullptr!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
}

int32_t CallManagerService::GetCallTransferInfo(int32_t slotId, CallTransferType type)
{
    if (callControlManagerPtr_ != nullptr) {
        return callControlManagerPtr_->GetCallTransferInfo(slotId, type);
    } else {
        TELEPHONY_LOGE("callControlManagerPtr_ is nullptr!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
}

int32_t CallManagerService::SetCallTransferInfo(int32_t slotId, CallTransferInfo &info)
{
    if (callControlManagerPtr_ != nullptr) {
        return callControlManagerPtr_->SetCallTransferInfo(slotId, info);
    } else {
        TELEPHONY_LOGE("callControlManagerPtr_ is nullptr!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
}

int32_t CallManagerService::SetCallPreferenceMode(int32_t slotId, int32_t mode)
{
    if (callControlManagerPtr_ != nullptr) {
        return callControlManagerPtr_->SetCallPreferenceMode(slotId, mode);
    } else {
        TELEPHONY_LOGE("callControlManagerPtr_ is nullptr!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
}

int32_t CallManagerService::StartRtt(int32_t callId, std::u16string &msg)
{
    if (callControlManagerPtr_ != nullptr) {
        return callControlManagerPtr_->StartRtt(callId, msg);
    } else {
        TELEPHONY_LOGE("callControlManagerPtr_ is nullptr!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
}

int32_t CallManagerService::StopRtt(int32_t callId)
{
    if (callControlManagerPtr_ != nullptr) {
        return callControlManagerPtr_->StopRtt(callId);
    } else {
        TELEPHONY_LOGE("callControlManagerPtr_ is nullptr!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
}

int32_t CallManagerService::CombineConference(int32_t mainCallId)
{
    if (callControlManagerPtr_ != nullptr) {
        return callControlManagerPtr_->CombineConference(mainCallId);
    } else {
        TELEPHONY_LOGE("callControlManagerPtr_ is nullptr!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
}

int32_t CallManagerService::SeparateConference(int32_t callId)
{
    if (callControlManagerPtr_ != nullptr) {
        return callControlManagerPtr_->SeparateConference(callId);
    } else {
        TELEPHONY_LOGE("callControlManagerPtr_ is nullptr!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
}

int32_t CallManagerService::SetMuted(bool isMute)
{
    if (callControlManagerPtr_ != nullptr) {
        return callControlManagerPtr_->SetMuted(isMute);
    } else {
        TELEPHONY_LOGE("callControlManagerPtr_ is nullptr!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
}

int32_t CallManagerService::MuteRinger()
{
    if (!TelephonyPermission::CheckPermission(OHOS_PERMISSION_SET_TELEPHONY_STATE)) {
        TELEPHONY_LOGE("Permission denied!");
        return TELEPHONY_ERR_PERMISSION_ERR;
    }
    if (callControlManagerPtr_ != nullptr) {
        return callControlManagerPtr_->MuteRinger();
    } else {
        TELEPHONY_LOGE("callControlManagerPtr_ is nullptr!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
}

int32_t CallManagerService::SetAudioDevice(AudioDevice deviceType)
{
    if (callControlManagerPtr_ != nullptr) {
        if (callControlManagerPtr_->SetAudioDevice(deviceType) == TELEPHONY_SUCCESS) {
            return TELEPHONY_SUCCESS;
        }
        TELEPHONY_LOGE("SetAudioDevice failed!");
        return CALL_ERR_SETTING_AUDIO_DEVICE_FAILED;
    } else {
        TELEPHONY_LOGE("callControlManagerPtr_ is nullptr!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
}

int32_t CallManagerService::ControlCamera(std::u16string cameraId)
{
    if (callControlManagerPtr_ != nullptr) {
        return callControlManagerPtr_->ControlCamera(
            cameraId, IPCSkeleton::GetCallingUid(), IPCSkeleton::GetCallingPid());
    } else {
        TELEPHONY_LOGE("callControlManagerPtr_ is nullptr!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
}

int32_t CallManagerService::SetPreviewWindow(VideoWindow &window)
{
    if (callControlManagerPtr_ != nullptr) {
        return callControlManagerPtr_->SetPreviewWindow(window);
    } else {
        TELEPHONY_LOGE("callControlManagerPtr_ is nullptr!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
}

int32_t CallManagerService::SetDisplayWindow(VideoWindow &window)
{
    if (callControlManagerPtr_ != nullptr) {
        return callControlManagerPtr_->SetDisplayWindow(window);
    } else {
        TELEPHONY_LOGE("callControlManagerPtr_ is nullptr!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
}

int32_t CallManagerService::SetCameraZoom(float zoomRatio)
{
    if (callControlManagerPtr_ != nullptr) {
        return callControlManagerPtr_->SetCameraZoom(zoomRatio);
    } else {
        TELEPHONY_LOGE("callControlManagerPtr_ is nullptr!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
}

int32_t CallManagerService::SetPausePicture(std::u16string path)
{
    if (callControlManagerPtr_ != nullptr) {
        return callControlManagerPtr_->SetPausePicture(path);
    } else {
        TELEPHONY_LOGE("callControlManagerPtr_ is nullptr!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
}

int32_t CallManagerService::SetDeviceDirection(int32_t rotation)
{
    if (callControlManagerPtr_ != nullptr) {
        return callControlManagerPtr_->SetDeviceDirection(rotation);
    } else {
        TELEPHONY_LOGE("callControlManagerPtr_ is nullptr!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
}

bool CallManagerService::IsEmergencyPhoneNumber(std::u16string &number, int32_t slotId, int32_t &errorCode)
{
    if (callControlManagerPtr_ != nullptr) {
        return callControlManagerPtr_->IsEmergencyPhoneNumber(number, slotId, errorCode);
    } else {
        TELEPHONY_LOGE("callControlManagerPtr_ is nullptr!");
        return false;
    }
}

int32_t CallManagerService::FormatPhoneNumber(
    std::u16string &number, std::u16string &countryCode, std::u16string &formatNumber)
{
    if (callControlManagerPtr_ != nullptr) {
        return callControlManagerPtr_->FormatPhoneNumber(number, countryCode, formatNumber);
    } else {
        TELEPHONY_LOGE("callControlManagerPtr_ is nullptr!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
}

int32_t CallManagerService::FormatPhoneNumberToE164(
    std::u16string &number, std::u16string &countryCode, std::u16string &formatNumber)
{
    if (callControlManagerPtr_ != nullptr) {
        return callControlManagerPtr_->FormatPhoneNumberToE164(number, countryCode, formatNumber);
    } else {
        TELEPHONY_LOGE("callControlManagerPtr_ is nullptr!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
}

int32_t CallManagerService::GetMainCallId(int32_t callId)
{
    if (callControlManagerPtr_ != nullptr) {
        return callControlManagerPtr_->GetMainCallId(callId);
    } else {
        TELEPHONY_LOGE("callControlManagerPtr_ is nullptr!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
}

std::vector<std::u16string> CallManagerService::GetSubCallIdList(int32_t callId)
{
    if (callControlManagerPtr_ != nullptr) {
        return callControlManagerPtr_->GetSubCallIdList(callId);
    }
    std::vector<std::u16string> vec;
    vec.clear();
    return vec;
}

std::vector<std::u16string> CallManagerService::GetCallIdListForConference(int32_t callId)
{
    if (callControlManagerPtr_ != nullptr) {
        return callControlManagerPtr_->GetCallIdListForConference(callId);
    }
    std::vector<std::u16string> vec;
    vec.clear();
    return vec;
}

int32_t CallManagerService::GetImsConfig(int32_t slotId, ImsConfigItem item)
{
    if (callControlManagerPtr_ != nullptr) {
        return callControlManagerPtr_->GetImsConfig(slotId, item);
    } else {
        TELEPHONY_LOGE("callControlManagerPtr_ is nullptr!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
}

int32_t CallManagerService::SetImsConfig(int32_t slotId, ImsConfigItem item, std::u16string &value)
{
    if (callControlManagerPtr_ != nullptr) {
        return callControlManagerPtr_->SetImsConfig(slotId, item, value);
    } else {
        TELEPHONY_LOGE("callControlManagerPtr_ is nullptr!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
}

int32_t CallManagerService::GetImsFeatureValue(int32_t slotId, FeatureType type)
{
    if (callControlManagerPtr_ != nullptr) {
        return callControlManagerPtr_->GetImsFeatureValue(slotId, type);
    } else {
        TELEPHONY_LOGE("callControlManagerPtr_ is nullptr!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
}

int32_t CallManagerService::SetImsFeatureValue(int32_t slotId, FeatureType type, int32_t value)
{
    if (callControlManagerPtr_ != nullptr) {
        return callControlManagerPtr_->SetImsFeatureValue(slotId, type, value);
    } else {
        TELEPHONY_LOGE("callControlManagerPtr_ is nullptr!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
}

int32_t CallManagerService::UpdateImsCallMode(int32_t callId, ImsCallMode mode)
{
    if (callControlManagerPtr_ != nullptr) {
        return callControlManagerPtr_->UpdateImsCallMode(callId, mode);
    } else {
        TELEPHONY_LOGE("callControlManagerPtr_ is nullptr!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
}

int32_t CallManagerService::EnableImsSwitch(int32_t slotId)
{
    if (callControlManagerPtr_ != nullptr) {
        return callControlManagerPtr_->EnableImsSwitch(slotId);
    } else {
        TELEPHONY_LOGE("callControlManagerPtr_ is nullptr!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
}

int32_t CallManagerService::DisableImsSwitch(int32_t slotId)
{
    if (callControlManagerPtr_ != nullptr) {
        return callControlManagerPtr_->DisableImsSwitch(slotId);
    } else {
        TELEPHONY_LOGE("callControlManagerPtr_ is nullptr!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
}

int32_t CallManagerService::IsImsSwitchEnabled(int32_t slotId)
{
    if (callControlManagerPtr_ != nullptr) {
        return callControlManagerPtr_->IsImsSwitchEnabled(slotId);
    } else {
        TELEPHONY_LOGE("callControlManagerPtr_ is nullptr!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
}

int32_t CallManagerService::EnableLteEnhanceMode(int32_t slotId)
{
    if (callControlManagerPtr_ != nullptr) {
        return callControlManagerPtr_->SetLteEnhanceMode(slotId, true);
    } else {
        TELEPHONY_LOGE("callControlManagerPtr_ is nullptr!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
}

int32_t CallManagerService::DisableLteEnhanceMode(int32_t slotId)
{
    if (callControlManagerPtr_ != nullptr) {
        return callControlManagerPtr_->SetLteEnhanceMode(slotId, false);
    } else {
        TELEPHONY_LOGE("callControlManagerPtr_ is nullptr!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
}

int32_t CallManagerService::IsLteEnhanceModeEnabled(int32_t slotId)
{
    if (callControlManagerPtr_ != nullptr) {
        return callControlManagerPtr_->GetLteEnhanceMode(slotId);
    } else {
        TELEPHONY_LOGE("callControlManagerPtr_ is nullptr!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
}

int32_t CallManagerService::JoinConference(int32_t callId, std::vector<std::u16string> &numberList)
{
    if (callControlManagerPtr_ != nullptr) {
        return callControlManagerPtr_->JoinConference(callId, numberList);
    }
    TELEPHONY_LOGE("callControlManagerPtr_ is nullptr!");
    return TELEPHONY_ERR_LOCAL_PTR_NULL;
}

int32_t CallManagerService::ReportOttCallDetailsInfo(std::vector<OttCallDetailsInfo> &ottVec)
{
    if (ottVec.empty()) {
        TELEPHONY_LOGE("ottVec is empty!");
        return TELEPHONY_ERR_ARGUMENT_INVALID;
    }
    CallDetailsInfo detailsInfo;
    CallDetailInfo detailInfo;
    detailsInfo.slotId = ERR_ID;
    (void)memcpy_s(detailsInfo.bundleName, kMaxBundleNameLen, ottVec[0].bundleName, kMaxBundleNameLen);
    detailInfo.callType = CallType::TYPE_OTT;
    detailInfo.accountId = ERR_ID;
    detailInfo.index = ERR_ID;
    detailInfo.voiceDomain = ERR_ID;
    std::vector<OttCallDetailsInfo>::iterator it = ottVec.begin();
    for (; it != ottVec.end(); ++it) {
        detailInfo.callMode = (*it).videoState;
        detailInfo.state = (*it).callState;
        (void)memcpy_s(detailInfo.phoneNum, kMaxNumberLen, (*it).phoneNum, kMaxNumberLen);
        (void)memcpy_s(detailInfo.bundleName, kMaxBundleNameLen, (*it).bundleName, kMaxBundleNameLen);
        detailsInfo.callVec.push_back(detailInfo);
    }
    int32_t ret = DelayedSingleton<ReportCallInfoHandlerService>::GetInstance()->UpdateCallsReportInfo(detailsInfo);
    if (ret != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("UpdateCallsReportInfo failed! errCode:%{public}d", ret);
    } else {
        TELEPHONY_LOGI("UpdateCallsReportInfo success!");
    }
    return ret;
}

int32_t CallManagerService::ReportOttCallEventInfo(OttCallEventInfo &eventInfo)
{
    int32_t ret = DelayedSingleton<ReportCallInfoHandlerService>::GetInstance()->UpdateOttEventInfo(eventInfo);
    if (ret != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("UpdateOttEventInfo failed! errCode:%{public}d", ret);
    } else {
        TELEPHONY_LOGI("UpdateOttEventInfo success!");
    }
    return ret;
}

sptr<IRemoteObject> CallManagerService::GetProxyObjectPtr(CallManagerProxyType proxyType)
{
    auto it = proxyObjectPtrMap_.find(static_cast<uint32_t>(proxyType));
    if (it != proxyObjectPtrMap_.end()) {
        TELEPHONY_LOGI("GetProxyObjectPtr success! proxyType:%{public}d", proxyType);
        return it->second;
    } else {
        switch (proxyType) {
            case PROXY_BLUETOOTH_CALL: {
                sptr<BluetoothCallService> ptr = (std::make_unique<BluetoothCallService>()).release();
                if (ptr == nullptr) {
                    TELEPHONY_LOGE("create BluetoothCallService object failed!");
                    return nullptr;
                }
                proxyObjectPtrMap_[proxyType] = ptr->AsObject().GetRefPtr();
                TELEPHONY_LOGI("create BluetoothCallService object success! proxyType:%{public}d", proxyType);
                return ptr->AsObject().GetRefPtr();
            }
            default:
                TELEPHONY_LOGE("invalid proxyType!");
                break;
        }
    }
    TELEPHONY_LOGE("GetProxyObjectPtr failed! proxyType:%{public}d", proxyType);
    return nullptr;
}

int32_t CallManagerService::CheckBundleName(std::string bundleName)
{
    std::string bundleNameList[] = {
        "com.ohos.callui",
    };
    for (int32_t i = 0; i < end(bundleNameList) - begin(bundleNameList); i++) {
        if (strcmp(bundleName.c_str(), bundleNameList[i].c_str()) == 0) {
            return TELEPHONY_SUCCESS;
        }
    }
    return TELEPHONY_ERROR;
}
} // namespace Telephony
} // namespace OHOS
