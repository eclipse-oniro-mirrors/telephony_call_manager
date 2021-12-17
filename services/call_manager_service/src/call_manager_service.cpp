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

#include "call_ability_handler.h"
#include "call_ability_report_proxy.h"
#include "call_manager_errors.h"
#include "telephony_log_wrapper.h"
#include "common_type.h"

#include "call_manager_dump_helper.h"
#include "cellular_call_info_handler.h"
#include "cellular_call_info_handler.h"
#include "cellular_call_ipc_interface_proxy.h"
#include "call_records_manager.h"

namespace OHOS {
namespace Telephony {
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
    DelayedSingleton<CallStateReportProxy>::GetInstance()->Init(TELEPHONY_STATE_REGISTRY_SYS_ABILITY_ID);
    if (!DelayedSingleton<CallControlManager>::GetInstance()->Init()) {
        TELEPHONY_LOGE("CallControlManager init failed!");
        return false;
    }
    callControlManagerPtr_ = DelayedSingleton<CallControlManager>::GetInstance();
    if (callControlManagerPtr_ == nullptr) {
        TELEPHONY_LOGE("callControlManagerPtr_ is nullptr!");
        return false;
    }
    DelayedSingleton<CellularCallInfoHandlerService>::GetInstance()->Start();
    DelayedSingleton<CellularCallIpcInterfaceProxy>::GetInstance()->Init(TELEPHONY_CELLULAR_CALL_SYS_ABILITY_ID);
    DelayedSingleton<CallAbilityHandlerService>::GetInstance()->Start();
    DelayedSingleton<CallRecordsManager>::GetInstance()->Init();
    return true;
}

void CallManagerService::UnInit()
{
    if (callControlManagerPtr_ != nullptr) {
        callControlManagerPtr_ = nullptr;
    }
}

void CallManagerService::OnStart()
{
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
    struct tm *timeNow;
    time_t second = time(0);
    if (second < 0) {
        return;
    }
    timeNow = localtime(&second);
    if (timeNow != nullptr) {
        TELEPHONY_LOGI(
            "CallManagerService start time:%{public}d-%{public}d-%{public}d %{public}d:%{public}d:%{public}d",
            timeNow->tm_year + startTime_, timeNow->tm_mon + extraMonth_, timeNow->tm_mday, timeNow->tm_hour,
            timeNow->tm_min, timeNow->tm_sec);
    }
}

void CallManagerService::OnStop()
{
    std::lock_guard<std::mutex> guard(lock_);
    struct tm *timeNow;
    time_t second = time(0);
    if (second < 0) {
        return;
    }
    timeNow = localtime(&second);
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
        return TELEPHONY_ERR_FAIL;
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
            return TELEPHONY_ERR_FAIL;
        }
        return TELEPHONY_SUCCESS;
    }
    TELEPHONY_LOGW("dumpHelper failed");
    return TELEPHONY_ERR_FAIL;
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

int32_t CallManagerService::RegisterCallBack(const sptr<ICallAbilityCallback> &callback, std::u16string &bundleName)
{
    std::string strName = Str16ToStr8(bundleName);
    return DelayedSingleton<CallAbilityReportProxy>::GetInstance()->RegisterCallBack(callback, strName);
}

int32_t CallManagerService::UnRegisterCallBack(std::u16string &bundleName)
{
    std::string strName = Str16ToStr8(bundleName);
    return DelayedSingleton<CallAbilityReportProxy>::GetInstance()->UnRegisterCallBack(strName);
}

int32_t CallManagerService::DialCall(std::u16string number, AppExecFwk::PacMap &extras)
{
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
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
}

bool CallManagerService::IsNewCallAllowed()
{
    if (callControlManagerPtr_ != nullptr) {
        return callControlManagerPtr_->IsNewCallAllowed();
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
        return TELEPHONY_ERR_FAIL;
    } else {
        TELEPHONY_LOGE("callControlManagerPtr_ is nullptr!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
}

bool CallManagerService::IsRinging()
{
    if (callControlManagerPtr_ != nullptr) {
        return callControlManagerPtr_->IsRinging();
    } else {
        TELEPHONY_LOGE("callControlManagerPtr_ is nullptr!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
}

bool CallManagerService::IsInEmergencyCall()
{
    if (callControlManagerPtr_ != nullptr) {
        return callControlManagerPtr_->HasEmergency();
    } else {
        TELEPHONY_LOGE("callControlManagerPtr_ is nullptr!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
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

int32_t CallManagerService::SendDtmf(int32_t callId, char str)
{
    if (callControlManagerPtr_ != nullptr) {
        return callControlManagerPtr_->SendDtmf(callId, str);
    } else {
        TELEPHONY_LOGE("callControlManagerPtr_ is nullptr!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
}

int32_t CallManagerService::SendBurstDtmf(int32_t callId, std::u16string str, int32_t on, int32_t off)
{
    if (callControlManagerPtr_ != nullptr) {
        return callControlManagerPtr_->SendBurstDtmf(callId, str, on, off);
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

int32_t CallManagerService::CombineConference(int32_t mainCallId)
{
    RETURN_FAILURE_IF_NULLPTR(
        callControlManagerPtr_, "callControlManagerPtr_ is null", TELEPHONY_ERR_LOCAL_PTR_NULL);
    return callControlManagerPtr_->CombineConference(mainCallId);
}

int32_t CallManagerService::SeparateConference(int32_t callId)
{
    RETURN_FAILURE_IF_NULLPTR(
        callControlManagerPtr_, "callControlManagerPtr_ is null", TELEPHONY_ERR_LOCAL_PTR_NULL);
    return callControlManagerPtr_->SeparateConference(callId);
}

int32_t CallManagerService::ControlCamera(std::u16string cameraId, std::u16string callingPackage)
{
    if (callControlManagerPtr_ != nullptr) {
        return callControlManagerPtr_->ControlCamera(
            cameraId, callingPackage, IPCSkeleton::GetCallingUid(), IPCSkeleton::GetCallingPid());
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
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
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
    RETURN_FAILURE_IF_NULLPTR(
        callControlManagerPtr_, "callControlManagerPtr_ is null", TELEPHONY_ERR_LOCAL_PTR_NULL);
    return callControlManagerPtr_->GetMainCallId(callId);
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

int32_t CallManagerService::CancelMissedCallsNotification(int32_t id)
{
    if (callControlManagerPtr_ != nullptr) {
        return callControlManagerPtr_->CancelMissedCallsNotification(id);
    } else {
        TELEPHONY_LOGE("callControlManagerPtr_ is nullptr!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
}
} // namespace Telephony
} // namespace OHOS
