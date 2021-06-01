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

#include <ctime>

#include "call_manager_errors.h"
#include "call_manager_log.h"

#include "cellular_call_ipc_interface_proxy.h"
#include "cellular_call_info_handler.h"
#include "audio_control_manager.h"
#include "common_type.h"

namespace OHOS {
namespace TelephonyCallManager {
const bool g_registerResult =
    SystemAbility::MakeAndRegisterAbility(DelayedSingleton<CallManagerService>::GetInstance().get());

CallManagerService::CallManagerService()
    : SystemAbility(TELEPHONY_CALL_MANAGER_SYS_ABILITY_ID, true), telephonyCallsManagerPtr_(nullptr)
{}

CallManagerService::~CallManagerService()
{
    UnInit();
}

bool CallManagerService::Init()
{
    DelayedSingleton<CallStateReportProxy>::GetInstance()->Init(TELEPHONY_STATE_REGISTRY_SYS_ABILITY_ID);
    if (!DelayedSingleton<CallControlManager>::GetInstance()->Init()) {
        CALLMANAGER_DEBUG_LOG("CallControlManager init failed!");
        return false;
    }
    telephonyCallsManagerPtr_ = DelayedSingleton<CallControlManager>::GetInstance();
    if (telephonyCallsManagerPtr_ == nullptr) {
        CALLMANAGER_DEBUG_LOG("telephonyCallsManagerPtr_ is nullptr!");
        return false;
    }
    DelayedSingleton<AudioControlManager>::GetInstance()->Init();
    DelayedSingleton<CellularCallInfoHandlerService>::GetInstance()->Start();
    DelayedSingleton<CellularCallIpcInterfaceProxy>::GetInstance()->Init(TELEPHONY_CELLULAR_CALL_SYS_ABILITY_ID);
    return true;
}

void CallManagerService::UnInit()
{
    if (telephonyCallsManagerPtr_ != nullptr) {
        telephonyCallsManagerPtr_ = nullptr;
    }
}

void CallManagerService::OnDump()
{
    std::lock_guard<std::mutex> guard(lock_);
    struct tm *timeNow;
    int32_t startTime = 1900;
    int32_t extralMonth = 1;
    time_t second = time(0);
    timeNow = localtime(&second);
    if (timeNow != nullptr) {
        CALLMANAGER_INFO_LOG(
            "CallManagerService dump time:%{public}d-%{public}d-%{public}d %{public}d:%{public}d:%{public}d",
            timeNow->tm_year + startTime, timeNow->tm_mon + extralMonth, timeNow->tm_mday, timeNow->tm_hour,
            timeNow->tm_min, timeNow->tm_sec);
    }
}

void CallManagerService::OnStart()
{
    std::lock_guard<std::mutex> guard(lock_);
    if (state_ == ServiceRunningState::STATE_RUNNING) {
        return;
    }

    if (!Init()) {
        CALLMANAGER_ERR_LOG("Leave, init failed!");
        return;
    }

    bool ret = SystemAbility::Publish(DelayedSingleton<CallManagerService>::GetInstance().get());
    if (!ret) {
        CALLMANAGER_ERR_LOG("Leave, publishing CallManagerService failed!");
        return;
    }
    CALLMANAGER_INFO_LOG("Publish CallManagerService SUCCESS");

    state_ = ServiceRunningState::STATE_RUNNING;
    struct tm *timeNow;
    int32_t startTime = 1900;
    int32_t extralMonth = 1;
    time_t second = time(0);
    timeNow = localtime(&second);
    if (timeNow != nullptr) {
        CALLMANAGER_INFO_LOG(
            "CallManagerService start time:%{public}d-%{public}d-%{public}d %{public}d:%{public}d:%{public}d",
            timeNow->tm_year + startTime, timeNow->tm_mon + extralMonth, timeNow->tm_mday, timeNow->tm_hour,
            timeNow->tm_min, timeNow->tm_sec);
    }
}

void CallManagerService::OnStop()
{
    std::lock_guard<std::mutex> guard(lock_);
    state_ = ServiceRunningState::STATE_STOPPED;
}

int32_t CallManagerService::DialCall(std::u16string number, AppExecFwk::PacMap &extras, int32_t &callId)
{
    if (telephonyCallsManagerPtr_ != nullptr) {
        return telephonyCallsManagerPtr_->DialCall(number, extras, callId);
    } else {
        CALLMANAGER_ERR_LOG("telephonyCallsManagerPtr_ is nullptr!");
        return TELEPHONY_LOCAL_PTR_NULL;
    }
}

int32_t CallManagerService::AcceptCall(int32_t callId, int32_t videoState)
{
    if (telephonyCallsManagerPtr_ != nullptr) {
        return telephonyCallsManagerPtr_->AcceptCall(callId, videoState);
    } else {
        CALLMANAGER_ERR_LOG("telephonyCallsManagerPtr_ is nullptr!");
        return TELEPHONY_LOCAL_PTR_NULL;
    }
}

int32_t CallManagerService::RejectCall(int32_t callId, bool isSendSms, std::u16string content)
{
    if (telephonyCallsManagerPtr_ != nullptr) {
        return telephonyCallsManagerPtr_->RejectCall(callId, isSendSms, content);
    } else {
        CALLMANAGER_ERR_LOG("telephonyCallsManagerPtr_ is nullptr!");
        return TELEPHONY_LOCAL_PTR_NULL;
    }
}

int32_t CallManagerService::HangUpCall(int32_t callId)
{
    if (telephonyCallsManagerPtr_ != nullptr) {
        return telephonyCallsManagerPtr_->HangUpCall(callId);
    } else {
        CALLMANAGER_ERR_LOG("telephonyCallsManagerPtr_ is nullptr!");
        return TELEPHONY_LOCAL_PTR_NULL;
    }
}

int32_t CallManagerService::GetCallState()
{
    if (telephonyCallsManagerPtr_ != nullptr) {
        return telephonyCallsManagerPtr_->GetCallState();
    } else {
        CALLMANAGER_ERR_LOG("telephonyCallsManagerPtr_ is nullptr!");
        return TELEPHONY_LOCAL_PTR_NULL;
    }
}
} // namespace TelephonyCallManager
} // namespace OHOS
