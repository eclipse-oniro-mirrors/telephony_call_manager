/*
 * Copyright (C) 2021-2023 Huawei Device Co., Ltd.
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

#include "accesstoken_kit.h"
#include "audio_device_manager.h"
#include "bluetooth_call_service.h"
#include "call_ability_report_proxy.h"
#include "call_manager_dump_helper.h"
#include "call_manager_errors.h"
#include "call_manager_hisysevent.h"
#include "call_records_manager.h"
#include "cellular_call_connection.h"
#include "common_type.h"
#include "core_manager_inner.h"
#include "hitrace_meter.h"
#include "ipc_skeleton.h"
#include "privacy_kit.h"
#include "report_call_info_handler.h"
#include "telephony_log_wrapper.h"
#include "telephony_permission.h"
#include "video_control_manager.h"
#include "voip_call_connection.h"
#include "distributed_call_manager.h"
#include "call_earthquake_alarm_subscriber.h"

namespace OHOS {
namespace Telephony {
using namespace Security::AccessToken;
static constexpr const char *OHOS_PERMISSION_SET_TELEPHONY_STATE = "ohos.permission.SET_TELEPHONY_STATE";
static constexpr const char *OHOS_PERMISSION_GET_TELEPHONY_STATE = "ohos.permission.GET_TELEPHONY_STATE";
static constexpr const char *OHOS_PERMISSION_PLACE_CALL = "ohos.permission.PLACE_CALL";
static constexpr const char *OHOS_PERMISSION_ANSWER_CALL = "ohos.permission.ANSWER_CALL";
static constexpr const char *OHOS_PERMISSION_READ_CALL_LOG = "ohos.permission.READ_CALL_LOG";
static constexpr const char *OHOS_PERMISSION_WRITE_CALL_LOG = "ohos.permission.WRITE_CALL_LOG";
static constexpr const char *SLOT_ID = "accountId";
static constexpr const char *CALL_TYPE = "callType";
static constexpr const char *VIDEO_STATE = "videoState";
static constexpr int32_t CLEAR_VOICE_MAIL_COUNT = 0;

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
    DelayedSingleton<ReportCallInfoHandler>::GetInstance()->Init();
    DelayedSingleton<CellularCallConnection>::GetInstance()->Init(TELEPHONY_CELLULAR_CALL_SYS_ABILITY_ID);
    DelayedSingleton<CallRecordsManager>::GetInstance()->Init();
    DelayedSingleton<BluetoothConnection>::GetInstance()->Init();
    DelayedSingleton<DistributedCallManager>::GetInstance()->Init();
    return true;
}

void CallManagerService::UnInit()
{
    DelayedSingleton<CellularCallConnection>::GetInstance()->UnInit();
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
    struct tm nowTime = { 0 };
    time_t second = time(0);
    if (second < 0) {
        return;
    }
    timeNow = localtime_r(&second, &nowTime);
    if (timeNow != nullptr) {
        spendTime_ = duration_cast<std::chrono::milliseconds>(high_resolution_clock::now() - beginTime).count();
        TELEPHONY_LOGI(
            "CallManagerService start time:%{public}d-%{public}d-%{public}d %{public}d:%{public}d:%{public}d",
            timeNow->tm_year + startTime_, timeNow->tm_mon + extraMonth_, timeNow->tm_mday, timeNow->tm_hour,
            timeNow->tm_min, timeNow->tm_sec);
        TELEPHONY_LOGI("CallManagerService start service cost time:%{public}d(milliseconds)", spendTime_);
    }
    LocationSystemAbilityListener::SystemAbilitySubscriber();
    LocationSubscriber::Subscriber();
}

void CallManagerService::OnStop()
{
    std::lock_guard<std::mutex> guard(lock_);
    struct tm *timeNow = nullptr;
    struct tm nowTime = { 0 };
    time_t second = time(0);
    if (second < 0) {
        return;
    }
    timeNow = localtime_r(&second, &nowTime);
    if (timeNow != nullptr) {
        TELEPHONY_LOGI(
            "CallManagerService dump time:%{public}d-%{public}d-%{public}d %{public}d:%{public}d:%{public}d",
            timeNow->tm_year + startTime_, timeNow->tm_mon + extraMonth_, timeNow->tm_mday, timeNow->tm_hour,
            timeNow->tm_min, timeNow->tm_sec);
    }
    DelayedSingleton<CellularCallConnection>::GetInstance()->UnInit();
    state_ = ServiceRunningState::STATE_STOPPED;
}

int32_t CallManagerService::GetServiceRunningState()
{
    return static_cast<int32_t>(state_);
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
    if (!TelephonyPermission::CheckCallerIsSystemApp()) {
        TELEPHONY_LOGE("Non-system applications use system APIs!");
        return TELEPHONY_ERR_ILLEGAL_USE_OF_SYSTEM_API;
    }
    if (!TelephonyPermission::CheckPermission(OHOS_PERMISSION_SET_TELEPHONY_STATE) &&
        !TelephonyPermission::CheckPermission(OHOS_PERMISSION_GET_TELEPHONY_STATE)) {
        TELEPHONY_LOGD("Permission denied.");
        return TELEPHONY_ERR_PERMISSION_ERR;
    }
    return DelayedSingleton<CallAbilityReportProxy>::GetInstance()->RegisterCallBack(callback, GetBundleInfo());
}

int32_t CallManagerService::UnRegisterCallBack()
{
    if (!TelephonyPermission::CheckCallerIsSystemApp()) {
        TELEPHONY_LOGE("Non-system applications use system APIs!");
        return TELEPHONY_ERR_ILLEGAL_USE_OF_SYSTEM_API;
    }
    if (!TelephonyPermission::CheckPermission(OHOS_PERMISSION_SET_TELEPHONY_STATE)) {
        TELEPHONY_LOGE("Permission denied!");
        return TELEPHONY_ERR_PERMISSION_ERR;
    }
    return DelayedSingleton<CallAbilityReportProxy>::GetInstance()->UnRegisterCallBack(GetBundleInfo());
}

int32_t CallManagerService::ObserverOnCallDetailsChange()
{
    if (!TelephonyPermission::CheckCallerIsSystemApp()) {
        TELEPHONY_LOGE("Non-system applications use system APIs!");
        return TELEPHONY_ERR_ILLEGAL_USE_OF_SYSTEM_API;
    }
    if (!TelephonyPermission::CheckPermission(OHOS_PERMISSION_SET_TELEPHONY_STATE) &&
        !TelephonyPermission::CheckPermission(OHOS_PERMISSION_GET_TELEPHONY_STATE)) {
        TELEPHONY_LOGE("Permission denied!");
        return TELEPHONY_ERR_PERMISSION_ERR;
    }

    std::vector<CallAttributeInfo> callAttributeInfo = CallObjectManager::GetAllCallInfoList();
    for (auto info : callAttributeInfo) {
        DelayedSingleton<CallAbilityReportProxy>::GetInstance()->ReportCallStateInfo(info, GetBundleInfo());
    }
    return TELEPHONY_SUCCESS;
}

int32_t CallManagerService::DialCall(std::u16string number, AppExecFwk::PacMap &extras)
{
    if (!TelephonyPermission::CheckCallerIsSystemApp()) {
        TELEPHONY_LOGE("Non-system applications use system APIs!");
        return TELEPHONY_ERR_ILLEGAL_USE_OF_SYSTEM_API;
    }
    DelayedSingleton<CallManagerHisysevent>::GetInstance()->SetDialStartTime();
    StartAsyncTrace(HITRACE_TAG_OHOS, "DialCall", getpid());
    int32_t uid = IPCSkeleton::GetCallingUid();
    std::string bundleName = "";
    TelephonyPermission::GetBundleNameByUid(uid, bundleName);
    extras.PutStringValue("bundleName", bundleName);
    if (!TelephonyPermission::CheckPermission(OHOS_PERMISSION_PLACE_CALL)) {
        TELEPHONY_LOGE("Permission denied!");
        CallManagerHisysevent::WriteDialCallFaultEvent(extras.GetIntValue(SLOT_ID), extras.GetIntValue(CALL_TYPE),
            extras.GetIntValue(VIDEO_STATE), TELEPHONY_ERR_PERMISSION_ERR, OHOS_PERMISSION_PLACE_CALL);
        FinishAsyncTrace(HITRACE_TAG_OHOS, "DialCall", getpid());
        return TELEPHONY_ERR_PERMISSION_ERR;
    }
    if (callControlManagerPtr_ != nullptr) {
        int32_t ret = callControlManagerPtr_->DialCall(number, extras);
        if (ret == TELEPHONY_SUCCESS) {
            std::u16string voiceMailNumber;
            CoreManagerInner::GetInstance().GetVoiceMailNumber(extras.GetIntValue(SLOT_ID), voiceMailNumber);
            if (voiceMailNumber == number) {
                CoreManagerInner::GetInstance().SetVoiceMailCount(extras.GetIntValue(SLOT_ID), CLEAR_VOICE_MAIL_COUNT);
            }
        } else {
            std::string errordesc = "";
            DelayedSingleton<CallManagerHisysevent>::GetInstance()->GetErrorDescription(ret, errordesc);
            CallManagerHisysevent::WriteDialCallFaultEvent(extras.GetIntValue(SLOT_ID), extras.GetIntValue(CALL_TYPE),
                extras.GetIntValue(VIDEO_STATE), ret, errordesc);
            FinishAsyncTrace(HITRACE_TAG_OHOS, "DialCall", getpid());
        }
        return ret;
    } else {
        TELEPHONY_LOGE("callControlManagerPtr_ is nullptr!");
        FinishAsyncTrace(HITRACE_TAG_OHOS, "DialCall", getpid());
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
}

int32_t CallManagerService::AnswerCall(int32_t callId, int32_t videoState)
{
    if (!TelephonyPermission::CheckCallerIsSystemApp()) {
        TELEPHONY_LOGE("Non-system applications use system APIs!");
        return TELEPHONY_ERR_ILLEGAL_USE_OF_SYSTEM_API;
    }
    if (!TelephonyPermission::CheckPermission(OHOS_PERMISSION_ANSWER_CALL)) {
        TELEPHONY_LOGE("Permission denied!");
        return TELEPHONY_ERR_PERMISSION_ERR;
    }
    DelayedSingleton<CallManagerHisysevent>::GetInstance()->SetAnswerStartTime();
    if (callControlManagerPtr_ != nullptr) {
        return callControlManagerPtr_->AnswerCall(callId, videoState);
    } else {
        TELEPHONY_LOGE("callControlManagerPtr_ is nullptr!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
}

int32_t CallManagerService::RejectCall(int32_t callId, bool rejectWithMessage, std::u16string textMessage)
{
    if (!TelephonyPermission::CheckCallerIsSystemApp()) {
        TELEPHONY_LOGE("Non-system applications use system APIs!");
        return TELEPHONY_ERR_ILLEGAL_USE_OF_SYSTEM_API;
    }
    if (!TelephonyPermission::CheckPermission(OHOS_PERMISSION_ANSWER_CALL)) {
        TELEPHONY_LOGE("Permission denied!");
        return TELEPHONY_ERR_PERMISSION_ERR;
    }
    if (callControlManagerPtr_ != nullptr) {
        return callControlManagerPtr_->RejectCall(callId, rejectWithMessage, textMessage);
    } else {
        TELEPHONY_LOGE("callControlManagerPtr_ is nullptr!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
}

int32_t CallManagerService::HangUpCall(int32_t callId)
{
    if (!TelephonyPermission::CheckCallerIsSystemApp()) {
        TELEPHONY_LOGE("Non-system applications use system APIs!");
        return TELEPHONY_ERR_ILLEGAL_USE_OF_SYSTEM_API;
    }
    if (!TelephonyPermission::CheckPermission(OHOS_PERMISSION_ANSWER_CALL) &&
        !TelephonyPermission::CheckPermission(OHOS_PERMISSION_SET_TELEPHONY_STATE)) {
        TELEPHONY_LOGE("Permission denied!");
        return TELEPHONY_ERR_PERMISSION_ERR;
    }
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
    if (!TelephonyPermission::CheckCallerIsSystemApp()) {
        TELEPHONY_LOGE("Non-system applications use system APIs!");
        return TELEPHONY_ERR_ILLEGAL_USE_OF_SYSTEM_API;
    }
    if (!TelephonyPermission::CheckPermission(OHOS_PERMISSION_ANSWER_CALL)) {
        TELEPHONY_LOGE("Permission denied!");
        return TELEPHONY_ERR_PERMISSION_ERR;
    }
    if (callControlManagerPtr_ != nullptr) {
        return callControlManagerPtr_->HoldCall(callId);
    } else {
        TELEPHONY_LOGE("callControlManagerPtr_ is nullptr!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
}

int32_t CallManagerService::UnHoldCall(int32_t callId)
{
    if (!TelephonyPermission::CheckCallerIsSystemApp()) {
        TELEPHONY_LOGE("Non-system applications use system APIs!");
        return TELEPHONY_ERR_ILLEGAL_USE_OF_SYSTEM_API;
    }
    if (!TelephonyPermission::CheckPermission(OHOS_PERMISSION_ANSWER_CALL)) {
        TELEPHONY_LOGE("Permission denied!");
        return TELEPHONY_ERR_PERMISSION_ERR;
    }
    if (callControlManagerPtr_ != nullptr) {
        return callControlManagerPtr_->UnHoldCall(callId);
    } else {
        TELEPHONY_LOGE("callControlManagerPtr_ is nullptr!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
}

int32_t CallManagerService::SwitchCall(int32_t callId)
{
    if (!TelephonyPermission::CheckCallerIsSystemApp()) {
        TELEPHONY_LOGE("Non-system applications use system APIs!");
        return TELEPHONY_ERR_ILLEGAL_USE_OF_SYSTEM_API;
    }
    if (!TelephonyPermission::CheckPermission(OHOS_PERMISSION_ANSWER_CALL)) {
        TELEPHONY_LOGE("Permission denied!");
        return TELEPHONY_ERR_PERMISSION_ERR;
    }
    if (callControlManagerPtr_ != nullptr) {
        return callControlManagerPtr_->SwitchCall(callId);
    } else {
        TELEPHONY_LOGE("callControlManagerPtr_ is nullptr!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
}

bool CallManagerService::HasCall()
{
    if (!TelephonyPermission::CheckPermission(OHOS_PERMISSION_GET_TELEPHONY_STATE)) {
        TELEPHONY_LOGE("Permission denied!");
        return false;
    }
    if (callControlManagerPtr_ != nullptr) {
        return callControlManagerPtr_->HasCall();
    } else {
        TELEPHONY_LOGE("callControlManagerPtr_ is nullptr!");
        return false;
    }
}

int32_t CallManagerService::IsNewCallAllowed(bool &enabled)
{
    if (!TelephonyPermission::CheckCallerIsSystemApp()) {
        TELEPHONY_LOGE("Non-system applications use system APIs!");
        return TELEPHONY_ERR_ILLEGAL_USE_OF_SYSTEM_API;
    }
    if (callControlManagerPtr_ != nullptr) {
        return callControlManagerPtr_->IsNewCallAllowed(enabled);
    } else {
        TELEPHONY_LOGE("callControlManagerPtr_ is nullptr!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
}

int32_t CallManagerService::RegisterVoipCallManagerCallback()
{
    std::lock_guard<std::mutex> guard(lock_);
    DelayedSingleton<VoipCallConnection>::GetInstance()->Init(TELEPHONY_VOIP_CALL_MANAGER_SYS_ABILITY_ID);
    voipCallCallbackPtr_ = (std::make_unique<CallStatusCallback>()).release();
    if (voipCallCallbackPtr_ == nullptr) {
        TELEPHONY_LOGE("voipCallCallbackPtr_ is nullptr!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    DelayedSingleton<VoipCallConnection>::GetInstance()->RegisterCallManagerCallBack(voipCallCallbackPtr_);
    return 0;
}

int32_t CallManagerService::UnRegisterVoipCallManagerCallback()
{
    std::lock_guard<std::mutex> guard(lock_);
    return DelayedSingleton<VoipCallConnection>::GetInstance()->UnRegisterCallManagerCallBack();
}

int32_t CallManagerService::IsRinging(bool &enabled)
{
    if (!TelephonyPermission::CheckCallerIsSystemApp()) {
        TELEPHONY_LOGE("Non-system applications use system APIs!");
        return TELEPHONY_ERR_ILLEGAL_USE_OF_SYSTEM_API;
    }
    if (!TelephonyPermission::CheckPermission(OHOS_PERMISSION_SET_TELEPHONY_STATE)) {
        TELEPHONY_LOGE("Permission denied!");
        return TELEPHONY_ERR_PERMISSION_ERR;
    }
    if (callControlManagerPtr_ != nullptr) {
        return callControlManagerPtr_->IsRinging(enabled);
    } else {
        TELEPHONY_LOGE("callControlManagerPtr_ is nullptr!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
}

int32_t CallManagerService::IsInEmergencyCall(bool &enabled)
{
    if (!TelephonyPermission::CheckCallerIsSystemApp()) {
        TELEPHONY_LOGE("Non-system applications use system APIs!");
        return TELEPHONY_ERR_ILLEGAL_USE_OF_SYSTEM_API;
    }
    if (!TelephonyPermission::CheckPermission(OHOS_PERMISSION_SET_TELEPHONY_STATE)) {
        TELEPHONY_LOGE("Permission denied!");
        return TELEPHONY_ERR_PERMISSION_ERR;
    }
    if (callControlManagerPtr_ != nullptr) {
        return callControlManagerPtr_->HasEmergency(enabled);
    } else {
        TELEPHONY_LOGE("callControlManagerPtr_ is nullptr!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
}

int32_t CallManagerService::StartDtmf(int32_t callId, char str)
{
    if (!TelephonyPermission::CheckCallerIsSystemApp()) {
        TELEPHONY_LOGE("Non-system applications use system APIs!");
        return TELEPHONY_ERR_ILLEGAL_USE_OF_SYSTEM_API;
    }
    if (!TelephonyPermission::CheckPermission(OHOS_PERMISSION_SET_TELEPHONY_STATE)) {
        TELEPHONY_LOGE("Permission denied!");
        return TELEPHONY_ERR_PERMISSION_ERR;
    }
    if (callControlManagerPtr_ != nullptr) {
        return callControlManagerPtr_->StartDtmf(callId, str);
    } else {
        TELEPHONY_LOGE("callControlManagerPtr_ is nullptr!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
}

int32_t CallManagerService::StopDtmf(int32_t callId)
{
    if (!TelephonyPermission::CheckCallerIsSystemApp()) {
        TELEPHONY_LOGE("Non-system applications use system APIs!");
        return TELEPHONY_ERR_ILLEGAL_USE_OF_SYSTEM_API;
    }
    if (!TelephonyPermission::CheckPermission(OHOS_PERMISSION_SET_TELEPHONY_STATE)) {
        TELEPHONY_LOGE("Permission denied!");
        return TELEPHONY_ERR_PERMISSION_ERR;
    }
    if (callControlManagerPtr_ != nullptr) {
        return callControlManagerPtr_->StopDtmf(callId);
    } else {
        TELEPHONY_LOGE("callControlManagerPtr_ is nullptr!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
}

int32_t CallManagerService::PostDialProceed(int32_t callId, bool proceed)
{
    if (!TelephonyPermission::CheckCallerIsSystemApp()) {
        TELEPHONY_LOGE("Non-system applications use system APIs!");
        return TELEPHONY_ERR_ILLEGAL_USE_OF_SYSTEM_API;
    }
    if (!TelephonyPermission::CheckPermission(OHOS_PERMISSION_SET_TELEPHONY_STATE)) {
        TELEPHONY_LOGE("Permission denied!");
        return TELEPHONY_ERR_PERMISSION_ERR;
    }
    if (callControlManagerPtr_ != nullptr) {
        return callControlManagerPtr_->PostDialProceed(callId, proceed);
    } else {
        TELEPHONY_LOGE("callControlManagerPtr_ is nullptr!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
}

int32_t CallManagerService::GetCallWaiting(int32_t slotId)
{
    if (!TelephonyPermission::CheckCallerIsSystemApp()) {
        TELEPHONY_LOGE("Non-system applications use system APIs!");
        return TELEPHONY_ERR_ILLEGAL_USE_OF_SYSTEM_API;
    }
    if (!TelephonyPermission::CheckPermission(OHOS_PERMISSION_GET_TELEPHONY_STATE)) {
        TELEPHONY_LOGE("Permission denied!");
        return TELEPHONY_ERR_PERMISSION_ERR;
    }
    if (callControlManagerPtr_ != nullptr) {
        return callControlManagerPtr_->GetCallWaiting(slotId);
    } else {
        TELEPHONY_LOGE("callControlManagerPtr_ is nullptr!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
}

int32_t CallManagerService::SetCallWaiting(int32_t slotId, bool activate)
{
    if (!TelephonyPermission::CheckCallerIsSystemApp()) {
        TELEPHONY_LOGE("Non-system applications use system APIs!");
        return TELEPHONY_ERR_ILLEGAL_USE_OF_SYSTEM_API;
    }
    if (!TelephonyPermission::CheckPermission(OHOS_PERMISSION_SET_TELEPHONY_STATE)) {
        TELEPHONY_LOGE("Permission denied!");
        return TELEPHONY_ERR_PERMISSION_ERR;
    }
    if (callControlManagerPtr_ != nullptr) {
        return callControlManagerPtr_->SetCallWaiting(slotId, activate);
    } else {
        TELEPHONY_LOGE("callControlManagerPtr_ is nullptr!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
}

int32_t CallManagerService::GetCallRestriction(int32_t slotId, CallRestrictionType type)
{
    if (!TelephonyPermission::CheckCallerIsSystemApp()) {
        TELEPHONY_LOGE("Non-system applications use system APIs!");
        return TELEPHONY_ERR_ILLEGAL_USE_OF_SYSTEM_API;
    }
    if (!TelephonyPermission::CheckPermission(OHOS_PERMISSION_GET_TELEPHONY_STATE)) {
        TELEPHONY_LOGE("Permission denied!");
        return TELEPHONY_ERR_PERMISSION_ERR;
    }
    if (callControlManagerPtr_ != nullptr) {
        return callControlManagerPtr_->GetCallRestriction(slotId, type);
    } else {
        TELEPHONY_LOGE("callControlManagerPtr_ is nullptr!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
}

int32_t CallManagerService::SetCallRestriction(int32_t slotId, CallRestrictionInfo &info)
{
    if (!TelephonyPermission::CheckCallerIsSystemApp()) {
        TELEPHONY_LOGE("Non-system applications use system APIs!");
        return TELEPHONY_ERR_ILLEGAL_USE_OF_SYSTEM_API;
    }
    if (!TelephonyPermission::CheckPermission(OHOS_PERMISSION_SET_TELEPHONY_STATE)) {
        TELEPHONY_LOGE("Permission denied!");
        return TELEPHONY_ERR_PERMISSION_ERR;
    }
    if (callControlManagerPtr_ != nullptr) {
        return callControlManagerPtr_->SetCallRestriction(slotId, info);
    } else {
        TELEPHONY_LOGE("callControlManagerPtr_ is nullptr!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
}

int32_t CallManagerService::SetCallRestrictionPassword(
    int32_t slotId, CallRestrictionType fac, const char *oldPassword, const char *newPassword)
{
    if (!TelephonyPermission::CheckCallerIsSystemApp()) {
        TELEPHONY_LOGE("Non-system applications use system APIs!");
        return TELEPHONY_ERR_ILLEGAL_USE_OF_SYSTEM_API;
    }
    if (!TelephonyPermission::CheckPermission(OHOS_PERMISSION_SET_TELEPHONY_STATE)) {
        TELEPHONY_LOGE("Permission denied!");
        return TELEPHONY_ERR_PERMISSION_ERR;
    }
    if (callControlManagerPtr_ != nullptr) {
        return callControlManagerPtr_->SetCallRestrictionPassword(slotId, fac, oldPassword, newPassword);
    } else {
        TELEPHONY_LOGE("callControlManagerPtr_ is nullptr!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
}

int32_t CallManagerService::GetCallTransferInfo(int32_t slotId, CallTransferType type)
{
    if (!TelephonyPermission::CheckCallerIsSystemApp()) {
        TELEPHONY_LOGE("Non-system applications use system APIs!");
        return TELEPHONY_ERR_ILLEGAL_USE_OF_SYSTEM_API;
    }
    if (!TelephonyPermission::CheckPermission(OHOS_PERMISSION_GET_TELEPHONY_STATE)) {
        TELEPHONY_LOGE("Permission denied!");
        return TELEPHONY_ERR_PERMISSION_ERR;
    }
    if (callControlManagerPtr_ != nullptr) {
        return callControlManagerPtr_->GetCallTransferInfo(slotId, type);
    } else {
        TELEPHONY_LOGE("callControlManagerPtr_ is nullptr!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
}

int32_t CallManagerService::SetCallTransferInfo(int32_t slotId, CallTransferInfo &info)
{
    if (!TelephonyPermission::CheckCallerIsSystemApp()) {
        TELEPHONY_LOGE("Non-system applications use system APIs!");
        return TELEPHONY_ERR_ILLEGAL_USE_OF_SYSTEM_API;
    }
    if (!TelephonyPermission::CheckPermission(OHOS_PERMISSION_SET_TELEPHONY_STATE)) {
        TELEPHONY_LOGE("Permission denied!");
        return TELEPHONY_ERR_PERMISSION_ERR;
    }
    if (callControlManagerPtr_ != nullptr) {
        return callControlManagerPtr_->SetCallTransferInfo(slotId, info);
    } else {
        TELEPHONY_LOGE("callControlManagerPtr_ is nullptr!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
}

int32_t CallManagerService::CanSetCallTransferTime(int32_t slotId, bool &result)
{
    if (!TelephonyPermission::CheckCallerIsSystemApp()) {
        TELEPHONY_LOGE("Non-system applications use system APIs!");
        return TELEPHONY_ERR_ILLEGAL_USE_OF_SYSTEM_API;
    }
    if (!TelephonyPermission::CheckPermission(OHOS_PERMISSION_GET_TELEPHONY_STATE)) {
        TELEPHONY_LOGE("Permission denied!");
        return TELEPHONY_ERR_PERMISSION_ERR;
    }
    if (callControlManagerPtr_ != nullptr) {
        return callControlManagerPtr_->CanSetCallTransferTime(slotId, result);
    } else {
        TELEPHONY_LOGE("[slot%{public}d] callControlManagerPtr_ is nullptr!", slotId);
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
}

int32_t CallManagerService::SetCallPreferenceMode(int32_t slotId, int32_t mode)
{
    if (!TelephonyPermission::CheckPermission(OHOS_PERMISSION_SET_TELEPHONY_STATE)) {
        TELEPHONY_LOGE("Permission denied!");
        return TELEPHONY_ERR_PERMISSION_ERR;
    }
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
    if (!TelephonyPermission::CheckCallerIsSystemApp()) {
        TELEPHONY_LOGE("Non-system applications use system APIs!");
        return TELEPHONY_ERR_ILLEGAL_USE_OF_SYSTEM_API;
    }
    if (!TelephonyPermission::CheckPermission(OHOS_PERMISSION_SET_TELEPHONY_STATE)) {
        TELEPHONY_LOGE("Permission denied!");
        return TELEPHONY_ERR_PERMISSION_ERR;
    }
    if (callControlManagerPtr_ != nullptr) {
        return callControlManagerPtr_->CombineConference(mainCallId);
    } else {
        TELEPHONY_LOGE("callControlManagerPtr_ is nullptr!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
}

int32_t CallManagerService::SeparateConference(int32_t callId)
{
    if (!TelephonyPermission::CheckCallerIsSystemApp()) {
        TELEPHONY_LOGE("Non-system applications use system APIs!");
        return TELEPHONY_ERR_ILLEGAL_USE_OF_SYSTEM_API;
    }
    if (!TelephonyPermission::CheckPermission(OHOS_PERMISSION_SET_TELEPHONY_STATE)) {
        TELEPHONY_LOGE("Permission denied!");
        return TELEPHONY_ERR_PERMISSION_ERR;
    }
    if (callControlManagerPtr_ != nullptr) {
        return callControlManagerPtr_->SeparateConference(callId);
    } else {
        TELEPHONY_LOGE("callControlManagerPtr_ is nullptr!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
}

int32_t CallManagerService::KickOutFromConference(int32_t callId)
{
    if (!TelephonyPermission::CheckCallerIsSystemApp()) {
        TELEPHONY_LOGE("Non-system applications use system APIs!");
        return TELEPHONY_ERR_ILLEGAL_USE_OF_SYSTEM_API;
    }
    if (!TelephonyPermission::CheckPermission(OHOS_PERMISSION_PLACE_CALL)) {
        TELEPHONY_LOGE("Permission denied!");
        return TELEPHONY_ERR_PERMISSION_ERR;
    }
    if (callControlManagerPtr_ != nullptr) {
        return callControlManagerPtr_->KickOutFromConference(callId);
    } else {
        TELEPHONY_LOGE("callControlManagerPtr_ is nullptr!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
}

int32_t CallManagerService::SetMuted(bool isMute)
{
    if (!TelephonyPermission::CheckCallerIsSystemApp()) {
        TELEPHONY_LOGE("Non-system applications use system APIs!");
        return TELEPHONY_ERR_ILLEGAL_USE_OF_SYSTEM_API;
    }
    if (!TelephonyPermission::CheckPermission(OHOS_PERMISSION_SET_TELEPHONY_STATE)) {
        TELEPHONY_LOGE("Permission denied!");
        return TELEPHONY_ERR_PERMISSION_ERR;
    }
    if (callControlManagerPtr_ != nullptr) {
        return callControlManagerPtr_->SetMuted(isMute);
    } else {
        TELEPHONY_LOGE("callControlManagerPtr_ is nullptr!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
}

int32_t CallManagerService::MuteRinger()
{
    if (!TelephonyPermission::CheckCallerIsSystemApp()) {
        TELEPHONY_LOGE("Non-system applications use system APIs!");
        return TELEPHONY_ERR_ILLEGAL_USE_OF_SYSTEM_API;
    }
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

int32_t CallManagerService::SetAudioDevice(const AudioDevice &audioDevice)
{
    if (!TelephonyPermission::CheckCallerIsSystemApp()) {
        TELEPHONY_LOGE("Non-system applications use system APIs!");
        return TELEPHONY_ERR_ILLEGAL_USE_OF_SYSTEM_API;
    }
    if (!TelephonyPermission::CheckPermission(OHOS_PERMISSION_SET_TELEPHONY_STATE)) {
        TELEPHONY_LOGE("Permission denied!");
        return TELEPHONY_ERR_PERMISSION_ERR;
    }
    if (callControlManagerPtr_ != nullptr) {
        if (callControlManagerPtr_->SetAudioDevice(audioDevice) == TELEPHONY_SUCCESS) {
            return TELEPHONY_SUCCESS;
        }
        TELEPHONY_LOGE("SetAudioDevice failed!");
        return CALL_ERR_SETTING_AUDIO_DEVICE_FAILED;
    } else {
        TELEPHONY_LOGE("callControlManagerPtr_ is nullptr!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
}

int32_t CallManagerService::ControlCamera(int32_t callId, std::u16string &cameraId)
{
    if (!TelephonyPermission::CheckCallerIsSystemApp()) {
        TELEPHONY_LOGE("Non-system applications use system APIs!");
        return TELEPHONY_ERR_ILLEGAL_USE_OF_SYSTEM_API;
    }
    if (!TelephonyPermission::CheckPermission(OHOS_PERMISSION_SET_TELEPHONY_STATE)) {
        TELEPHONY_LOGE("Permission denied!");
        return TELEPHONY_ERR_PERMISSION_ERR;
    }
    auto videoControlManager = DelayedSingleton<VideoControlManager>::GetInstance();
    if (videoControlManager != nullptr) {
        return videoControlManager->ControlCamera(
            callId, cameraId, IPCSkeleton::GetCallingUid(), IPCSkeleton::GetCallingPid());
    }  else {
        TELEPHONY_LOGE("videoControlManager is nullptr!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
}

int32_t CallManagerService::SetPreviewWindow(int32_t callId, std::string &surfaceId, sptr<Surface> surface)
{
    if (!TelephonyPermission::CheckCallerIsSystemApp()) {
        TELEPHONY_LOGE("Non-system applications use system APIs!");
        return TELEPHONY_ERR_ILLEGAL_USE_OF_SYSTEM_API;
    }
    if (!TelephonyPermission::CheckPermission(OHOS_PERMISSION_SET_TELEPHONY_STATE)) {
        TELEPHONY_LOGE("Permission denied!");
        return TELEPHONY_ERR_PERMISSION_ERR;
    }
    auto videoControlManager = DelayedSingleton<VideoControlManager>::GetInstance();
    if (videoControlManager != nullptr) {
        int32_t callerToken = IPCSkeleton::GetCallingTokenID();
        if (surface == nullptr) {
            PrivacyKit::StopUsingPermission(callerToken, "ohos.permission.CAMERA");
        } else {
            sptr<CallBase> call = CallObjectManager::GetOneCallObjectByIndex(callId);
            if (call == nullptr || call->GetVideoStateType() != VideoStateType::TYPE_RECEIVE_ONLY) {
                PrivacyKit::AddPermissionUsedRecord(callerToken, "ohos.permission.CAMERA", 1, 0);
                PrivacyKit::StartUsingPermission(callerToken, "ohos.permission.CAMERA");
            }
        }
        return videoControlManager->SetPreviewWindow(callId, surfaceId, surface);
    } else {
        TELEPHONY_LOGE("videoControlManager is nullptr!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
}

int32_t CallManagerService::SetDisplayWindow(int32_t callId, std::string &surfaceId, sptr<Surface> surface)
{
    if (!TelephonyPermission::CheckCallerIsSystemApp()) {
        TELEPHONY_LOGE("Non-system applications use system APIs!");
        return TELEPHONY_ERR_ILLEGAL_USE_OF_SYSTEM_API;
    }
    if (!TelephonyPermission::CheckPermission(OHOS_PERMISSION_SET_TELEPHONY_STATE)) {
        TELEPHONY_LOGE("Permission denied!");
        return TELEPHONY_ERR_PERMISSION_ERR;
    }
    auto videoControlManager = DelayedSingleton<VideoControlManager>::GetInstance();
    if (videoControlManager != nullptr) {
        return videoControlManager->SetDisplayWindow(callId, surfaceId, surface);
    } else {
        TELEPHONY_LOGE("videoControlManager is nullptr!");
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

int32_t CallManagerService::SetPausePicture(int32_t callId, std::u16string &path)
{
    if (!TelephonyPermission::CheckCallerIsSystemApp()) {
        TELEPHONY_LOGE("Non-system applications use system APIs!");
        return TELEPHONY_ERR_ILLEGAL_USE_OF_SYSTEM_API;
    }
    if (!TelephonyPermission::CheckPermission(OHOS_PERMISSION_SET_TELEPHONY_STATE)) {
        TELEPHONY_LOGE("Permission denied!");
        return TELEPHONY_ERR_PERMISSION_ERR;
    }
    auto videoControlManager = DelayedSingleton<VideoControlManager>::GetInstance();
    if (videoControlManager != nullptr) {
        return videoControlManager->SetPausePicture(callId, path);
    } else {
        TELEPHONY_LOGE("videoControlManager is nullptr!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
}

int32_t CallManagerService::SetDeviceDirection(int32_t callId, int32_t rotation)
{
    if (!TelephonyPermission::CheckCallerIsSystemApp()) {
        TELEPHONY_LOGE("Non-system applications use system APIs!");
        return TELEPHONY_ERR_ILLEGAL_USE_OF_SYSTEM_API;
    }
    if (!TelephonyPermission::CheckPermission(OHOS_PERMISSION_SET_TELEPHONY_STATE)) {
        TELEPHONY_LOGE("Permission denied!");
        return TELEPHONY_ERR_PERMISSION_ERR;
    }
    auto videoControlManager = DelayedSingleton<VideoControlManager>::GetInstance();
    if (videoControlManager != nullptr) {
        return videoControlManager->SetDeviceDirection(callId, rotation);
    } else {
        TELEPHONY_LOGE("videoControlManager is nullptr!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
}

int32_t CallManagerService::IsEmergencyPhoneNumber(std::u16string &number, int32_t slotId, bool &enabled)
{
    if (callControlManagerPtr_ != nullptr) {
        return callControlManagerPtr_->IsEmergencyPhoneNumber(number, slotId, enabled);
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

int32_t CallManagerService::GetMainCallId(int32_t callId, int32_t &mainCallId)
{
    if (!TelephonyPermission::CheckCallerIsSystemApp()) {
        TELEPHONY_LOGE("Non-system applications use system APIs!");
        return TELEPHONY_ERR_ILLEGAL_USE_OF_SYSTEM_API;
    }
    if (!TelephonyPermission::CheckPermission(OHOS_PERMISSION_GET_TELEPHONY_STATE)) {
        TELEPHONY_LOGE("Permission denied!");
        return TELEPHONY_ERR_PERMISSION_ERR;
    }
    if (callControlManagerPtr_ != nullptr) {
        return callControlManagerPtr_->GetMainCallId(callId, mainCallId);
    } else {
        TELEPHONY_LOGE("callControlManagerPtr_ is nullptr!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
}

int32_t CallManagerService::GetSubCallIdList(int32_t callId, std::vector<std::u16string> &callIdList)
{
    if (!TelephonyPermission::CheckCallerIsSystemApp()) {
        TELEPHONY_LOGE("Non-system applications use system APIs!");
        return TELEPHONY_ERR_ILLEGAL_USE_OF_SYSTEM_API;
    }
    if (!TelephonyPermission::CheckPermission(OHOS_PERMISSION_GET_TELEPHONY_STATE)) {
        TELEPHONY_LOGE("Permission denied!");
        return TELEPHONY_ERR_PERMISSION_ERR;
    }
    if (callControlManagerPtr_ != nullptr) {
        return callControlManagerPtr_->GetSubCallIdList(callId, callIdList);
    }
    callIdList.clear();
    return TELEPHONY_ERR_LOCAL_PTR_NULL;
}

int32_t CallManagerService::GetCallIdListForConference(int32_t callId, std::vector<std::u16string> &callIdList)
{
    if (!TelephonyPermission::CheckCallerIsSystemApp()) {
        TELEPHONY_LOGE("Non-system applications use system APIs!");
        return TELEPHONY_ERR_ILLEGAL_USE_OF_SYSTEM_API;
    }
    if (!TelephonyPermission::CheckPermission(OHOS_PERMISSION_GET_TELEPHONY_STATE)) {
        TELEPHONY_LOGE("Permission denied!");
        return TELEPHONY_ERR_PERMISSION_ERR;
    }
    if (callControlManagerPtr_ != nullptr) {
        return callControlManagerPtr_->GetCallIdListForConference(callId, callIdList);
    }
    callIdList.clear();
    return TELEPHONY_ERR_LOCAL_PTR_NULL;
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
    if (!TelephonyPermission::CheckCallerIsSystemApp()) {
        TELEPHONY_LOGE("Non-system applications use system APIs!");
        return TELEPHONY_ERR_ILLEGAL_USE_OF_SYSTEM_API;
    }
    if (!TelephonyPermission::CheckPermission(OHOS_PERMISSION_SET_TELEPHONY_STATE)) {
        TELEPHONY_LOGE("Permission denied!");
        return TELEPHONY_ERR_PERMISSION_ERR;
    }
    auto videoControlManager = DelayedSingleton<VideoControlManager>::GetInstance();
    if (videoControlManager != nullptr) {
        return videoControlManager->UpdateImsCallMode(callId, mode);
    } else {
        TELEPHONY_LOGE("videoControlManager is nullptr!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
}

int32_t CallManagerService::EnableImsSwitch(int32_t slotId)
{
    if (!TelephonyPermission::CheckCallerIsSystemApp()) {
        TELEPHONY_LOGE("Non-system applications use system APIs!");
        return TELEPHONY_ERR_ILLEGAL_USE_OF_SYSTEM_API;
    }
    if (!TelephonyPermission::CheckPermission(OHOS_PERMISSION_SET_TELEPHONY_STATE)) {
        TELEPHONY_LOGE("Permission denied!");
        return TELEPHONY_ERR_PERMISSION_ERR;
    }
    if (callControlManagerPtr_ != nullptr) {
        return callControlManagerPtr_->EnableImsSwitch(slotId);
    } else {
        TELEPHONY_LOGE("callControlManagerPtr_ is nullptr!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
}

int32_t CallManagerService::DisableImsSwitch(int32_t slotId)
{
    if (!TelephonyPermission::CheckCallerIsSystemApp()) {
        TELEPHONY_LOGE("Non-system applications use system APIs!");
        return TELEPHONY_ERR_ILLEGAL_USE_OF_SYSTEM_API;
    }
    if (!TelephonyPermission::CheckPermission(OHOS_PERMISSION_SET_TELEPHONY_STATE)) {
        TELEPHONY_LOGE("Permission denied!");
        return TELEPHONY_ERR_PERMISSION_ERR;
    }
    if (callControlManagerPtr_ != nullptr) {
        return callControlManagerPtr_->DisableImsSwitch(slotId);
    } else {
        TELEPHONY_LOGE("callControlManagerPtr_ is nullptr!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
}

int32_t CallManagerService::IsImsSwitchEnabled(int32_t slotId, bool &enabled)
{
    if (!TelephonyPermission::CheckCallerIsSystemApp()) {
        TELEPHONY_LOGE("Non-system applications use system APIs!");
        return TELEPHONY_ERR_ILLEGAL_USE_OF_SYSTEM_API;
    }
    if (callControlManagerPtr_ != nullptr) {
        return callControlManagerPtr_->IsImsSwitchEnabled(slotId, enabled);
    } else {
        TELEPHONY_LOGE("callControlManagerPtr_ is nullptr!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
}

int32_t CallManagerService::SetVoNRState(int32_t slotId, int32_t state)
{
    if (!TelephonyPermission::CheckCallerIsSystemApp()) {
        TELEPHONY_LOGE("Non-system applications use system APIs!");
        return TELEPHONY_ERR_ILLEGAL_USE_OF_SYSTEM_API;
    }
    if (!TelephonyPermission::CheckPermission(OHOS_PERMISSION_SET_TELEPHONY_STATE)) {
        TELEPHONY_LOGE("Permission denied!");
        return TELEPHONY_ERR_PERMISSION_ERR;
    }
    if (callControlManagerPtr_ != nullptr) {
        return callControlManagerPtr_->SetVoNRState(slotId, state);
    } else {
        TELEPHONY_LOGE("callControlManagerPtr_ is nullptr!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
}

int32_t CallManagerService::GetVoNRState(int32_t slotId, int32_t &state)
{
    if (!TelephonyPermission::CheckCallerIsSystemApp()) {
        TELEPHONY_LOGE("Non-system applications use system APIs!");
        return TELEPHONY_ERR_ILLEGAL_USE_OF_SYSTEM_API;
    }
    if (!TelephonyPermission::CheckPermission(OHOS_PERMISSION_GET_TELEPHONY_STATE)) {
        TELEPHONY_LOGE("Permission denied!");
        return TELEPHONY_ERR_PERMISSION_ERR;
    }
    if (callControlManagerPtr_ != nullptr) {
        return callControlManagerPtr_->GetVoNRState(slotId, state);
    } else {
        TELEPHONY_LOGE("callControlManagerPtr_ is nullptr!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
}

int32_t CallManagerService::JoinConference(int32_t callId, std::vector<std::u16string> &numberList)
{
    if (!TelephonyPermission::CheckCallerIsSystemApp()) {
        TELEPHONY_LOGE("Non-system applications use system APIs!");
        return TELEPHONY_ERR_ILLEGAL_USE_OF_SYSTEM_API;
    }
    if (!TelephonyPermission::CheckPermission(OHOS_PERMISSION_PLACE_CALL)) {
        TELEPHONY_LOGE("Permission denied!");
        return TELEPHONY_ERR_PERMISSION_ERR;
    }
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
    int32_t ret = DelayedSingleton<ReportCallInfoHandler>::GetInstance()->UpdateCallsReportInfo(detailsInfo);
    if (ret != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("UpdateCallsReportInfo failed! errCode:%{public}d", ret);
    } else {
        TELEPHONY_LOGI("UpdateCallsReportInfo success!");
    }
    return ret;
}

int32_t CallManagerService::ReportOttCallEventInfo(OttCallEventInfo &eventInfo)
{
    int32_t ret = DelayedSingleton<ReportCallInfoHandler>::GetInstance()->UpdateOttEventInfo(eventInfo);
    if (ret != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("UpdateOttEventInfo failed! errCode:%{public}d", ret);
    } else {
        TELEPHONY_LOGI("UpdateOttEventInfo success!");
    }
    return ret;
}

int32_t CallManagerService::CloseUnFinishedUssd(int32_t slotId)
{
    if (!TelephonyPermission::CheckCallerIsSystemApp()) {
        TELEPHONY_LOGE("Non system applications use system APIs!");
        return TELEPHONY_ERR_ILLEGAL_USE_OF_SYSTEM_API;
    }
    if (!TelephonyPermission::CheckPermission(OHOS_PERMISSION_SET_TELEPHONY_STATE)) {
        TELEPHONY_LOGE("Permission denied!");
        return TELEPHONY_ERR_PERMISSION_ERR;
    }
    if (callControlManagerPtr_ != nullptr) {
        return callControlManagerPtr_->CloseUnFinishedUssd(slotId);
    } else {
        TELEPHONY_LOGE("callControlManagerPtr_ is nullptr!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
}

int32_t CallManagerService::InputDialerSpecialCode(const std::string &specialCode)
{
    if (!TelephonyPermission::CheckCallerIsSystemApp()) {
        TELEPHONY_LOGE("Non system applications use system APIs!");
        return TELEPHONY_ERR_ILLEGAL_USE_OF_SYSTEM_API;
    }
    if (!TelephonyPermission::CheckPermission(OHOS_PERMISSION_PLACE_CALL)) {
        TELEPHONY_LOGE("Permission denied!");
        return TELEPHONY_ERR_PERMISSION_ERR;
    }

    auto it = find(supportSpecialCode_.begin(), supportSpecialCode_.end(), specialCode);
    if (it == supportSpecialCode_.end()) {
        TELEPHONY_LOGE("specialCode is not support");
        return TELEPHONY_ERR_ARGUMENT_INVALID;
    }
    AAFwk::Want want;
    want.SetAction(EventFwk::CommonEventSupport::COMMON_EVENT_SPECIAL_CODE);
    EventFwk::CommonEventData commonEventData;
    commonEventData.SetWant(want);
    commonEventData.SetData(specialCode);
    EventFwk::CommonEventPublishInfo publishInfo;
    if (!EventFwk::CommonEventManager::PublishCommonEvent(commonEventData, publishInfo, nullptr)) {
        TELEPHONY_LOGE("PublishCommonEvent fail");
        return TELEPHONY_ERR_PUBLISH_BROADCAST_FAIL;
    }
    return TELEPHONY_SUCCESS;
}

int32_t CallManagerService::RemoveMissedIncomingCallNotification()
{
    if (!TelephonyPermission::CheckCallerIsSystemApp()) {
        TELEPHONY_LOGE("Non-system applications use system APIs!");
        return TELEPHONY_ERR_ILLEGAL_USE_OF_SYSTEM_API;
    }
    if (!TelephonyPermission::CheckPermission(OHOS_PERMISSION_SET_TELEPHONY_STATE) ||
        !TelephonyPermission::CheckPermission(OHOS_PERMISSION_READ_CALL_LOG) ||
        !TelephonyPermission::CheckPermission(OHOS_PERMISSION_WRITE_CALL_LOG)) {
        TELEPHONY_LOGE("Permission denied!");
        return TELEPHONY_ERR_PERMISSION_ERR;
    }
    if (callControlManagerPtr_ == nullptr) {
        TELEPHONY_LOGE("callControlManagerPtr_ is nullptr!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    return callControlManagerPtr_->RemoveMissedIncomingCallNotification();
}

int32_t CallManagerService::SetVoIPCallState(int32_t state)
{
    if (!TelephonyPermission::CheckCallerIsSystemApp()) {
        TELEPHONY_LOGE("Non-system applications use system APIs!");
        return TELEPHONY_ERR_ILLEGAL_USE_OF_SYSTEM_API;
    }
    if (!TelephonyPermission::CheckPermission(OHOS_PERMISSION_SET_TELEPHONY_STATE) &&
        !TelephonyPermission::CheckPermission(OHOS_PERMISSION_GET_TELEPHONY_STATE)) {
        TELEPHONY_LOGE("Permission denied!");
        return TELEPHONY_ERR_PERMISSION_ERR;
    }
    if (callControlManagerPtr_ != nullptr) {
        return callControlManagerPtr_->SetVoIPCallState(state);
    } else {
        TELEPHONY_LOGE("callControlManagerPtr_ is nullptr!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
}

int32_t CallManagerService::GetVoIPCallState(int32_t &state)
{
    if (!TelephonyPermission::CheckCallerIsSystemApp()) {
        TELEPHONY_LOGE("Non-system applications use system APIs!");
        return TELEPHONY_ERR_ILLEGAL_USE_OF_SYSTEM_API;
    }
    if (callControlManagerPtr_ != nullptr) {
        return callControlManagerPtr_->GetVoIPCallState(state);
    } else {
        TELEPHONY_LOGE("callControlManagerPtr_ is nullptr!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
}

sptr<IRemoteObject> CallManagerService::GetProxyObjectPtr(CallManagerProxyType proxyType)
{
    std::lock_guard<std::mutex> guard(lock_);
    auto it = proxyObjectPtrMap_.find(static_cast<uint32_t>(proxyType));
    if (it != proxyObjectPtrMap_.end()) {
        TELEPHONY_LOGI("GetProxyObjectPtr success! proxyType:%{public}d", proxyType);
        return it->second;
    } else {
        switch (proxyType) {
            case PROXY_BLUETOOTH_CALL: {
                sptr<BluetoothCallService> ptr = new (std::nothrow) BluetoothCallService();
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

std::string CallManagerService::GetBundleInfo()
{
    std::string bundleInfo = "";
    bundleInfo.append(std::to_string(IPCSkeleton::GetCallingPid()));
    return bundleInfo;
}

int32_t CallManagerService::ReportAudioDeviceInfo()
{
    if (!TelephonyPermission::CheckPermission(OHOS_PERMISSION_SET_TELEPHONY_STATE)) {
        TELEPHONY_LOGE("Permission denied!");
        return TELEPHONY_ERR_PERMISSION_ERR;
    }
    return DelayedSingleton<AudioDeviceManager>::GetInstance()->ReportAudioDeviceChange();
}

int32_t CallManagerService::CancelCallUpgrade(int32_t callId)
{
    if (!TelephonyPermission::CheckCallerIsSystemApp()) {
        TELEPHONY_LOGE("Non-system applications use system APIs!");
        return TELEPHONY_ERR_ILLEGAL_USE_OF_SYSTEM_API;
    }
    if (!TelephonyPermission::CheckPermission(OHOS_PERMISSION_PLACE_CALL)) {
        TELEPHONY_LOGE("Permission denied!");
        return TELEPHONY_ERR_PERMISSION_ERR;
    }
    auto videoControlManager = DelayedSingleton<VideoControlManager>::GetInstance();
    if (videoControlManager != nullptr) {
        return DelayedSingleton<VideoControlManager>::GetInstance()->CancelCallUpgrade(callId);
    } else {
        TELEPHONY_LOGE("videoControlManager is nullptr!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
}

int32_t CallManagerService::RequestCameraCapabilities(int32_t callId)
{
    if (!TelephonyPermission::CheckCallerIsSystemApp()) {
        TELEPHONY_LOGE("Non-system applications use system APIs!");
        return TELEPHONY_ERR_ILLEGAL_USE_OF_SYSTEM_API;
    }
    if (!TelephonyPermission::CheckPermission(OHOS_PERMISSION_GET_TELEPHONY_STATE)) {
        TELEPHONY_LOGE("Permission denied!");
        return TELEPHONY_ERR_PERMISSION_ERR;
    }
    auto videoControlManager = DelayedSingleton<VideoControlManager>::GetInstance();
    if (videoControlManager != nullptr) {
        return DelayedSingleton<VideoControlManager>::GetInstance()->RequestCameraCapabilities(callId);
    } else {
        TELEPHONY_LOGE("videoControlManager is nullptr!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
}

int32_t CallManagerService::SendCallUiEvent(int32_t callId, std::string &eventName)
{
    if (!TelephonyPermission::CheckPermission(OHOS_PERMISSION_SET_TELEPHONY_STATE)) {
        TELEPHONY_LOGE("Permission denied!");
        return TELEPHONY_ERR_PERMISSION_ERR;
    }
    return TELEPHONY_SUCCESS;
}
} // namespace Telephony
} // namespace OHOS
