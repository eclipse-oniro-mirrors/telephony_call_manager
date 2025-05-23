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

#include "cs_conference.h"

#include <string>
#include <string_ex.h>
#include <list>

#include "call_base.h"
#include "call_object_manager.h"
#include "call_manager_errors.h"
#include "telephony_log_wrapper.h"
#include "call_manager_inner_type.h"

namespace OHOS {
namespace Telephony {
CsConference::CsConference() : ConferenceBase()
{
    conferenceType_ = CallType::TYPE_CS;
    maxSubCallLimits_ = CS_CONFERENCE_MAX_CALLS_CNT;
    // get from configuration file
#ifdef ABILITY_CONFIG_SUPPORT
    maxSubCallLimits_ = GetConfig(CS_CONFERENCE_SUB_CALL_LIMITS);
#endif
}

CsConference::~CsConference() {}

int32_t CsConference::JoinToConference(int32_t callId)
{
    std::lock_guard<std::mutex> lock(conferenceMutex_);
    if (state_ != CONFERENCE_STATE_CREATING && state_ != CONFERENCE_STATE_ACTIVE &&
        state_ != CONFERENCE_STATE_LEAVING && state_ != CONFERENCE_STATE_HOLDING) {
        TELEPHONY_LOGE("the current conference status does not allow CombineConference");
        return CALL_ERR_ILLEGAL_CALL_OPERATION;
    }
    subCallIdSet_.insert(callId);
    state_ = CONFERENCE_STATE_ACTIVE;
    oldState_ = state_;
    beginTime_ = time(nullptr);
    return TELEPHONY_SUCCESS;
}

int32_t CsConference::LeaveFromConference(int32_t callId)
{
    std::lock_guard<std::mutex> lock(conferenceMutex_);
    if (subCallIdSet_.find(callId) != subCallIdSet_.end()) {
        subCallIdSet_.erase(callId);
        if (mainCallId_ == callId) {
            mainCallId_ = *subCallIdSet_.begin();
        }
    } else {
        TELEPHONY_LOGE("leave conference failed, callId %{public}d not in conference", callId);
        return CALL_ERR_CONFERENCE_SEPERATE_FAILED;
    }
    if (subCallIdSet_.empty()) {
        mainCallId_ = ERR_ID;
        state_ = CONFERENCE_STATE_IDLE;
        oldState_ = state_;
        beginTime_ = 0;
    }
    return TELEPHONY_SUCCESS;
}

int32_t CsConference::HoldConference(int32_t callId)
{
    std::lock_guard<std::mutex> lock(conferenceMutex_);
    if (state_ == CONFERENCE_STATE_HOLDING) {
        TELEPHONY_LOGI("HoldConference success");
        return TELEPHONY_SUCCESS;
    }
    if (subCallIdSet_.find(callId) == subCallIdSet_.end()) {
        TELEPHONY_LOGE("separate conference failed, callId %{public}d not in conference", callId);
        return CALL_ERR_CONFERENCE_SEPERATE_FAILED;
    }
    if (subCallIdSet_.empty()) {
        mainCallId_ = ERR_ID;
        state_ = CONFERENCE_STATE_IDLE;
        oldState_ = state_;
        beginTime_ = 0;
        return CALL_ERR_CONFERENCE_SEPERATE_FAILED;
    }
    state_ = CONFERENCE_STATE_HOLDING;
    oldState_ = state_;
    return TELEPHONY_SUCCESS;
}

int32_t CsConference::CanCombineConference()
{
    std::lock_guard<std::mutex> lock(conferenceMutex_);
    if (subCallIdSet_.size() >= maxSubCallLimits_) {
        TELEPHONY_LOGE("there is %{public}zu calls in the conference yet!", subCallIdSet_.size());
        return CALL_ERR_CONFERENCE_CALL_EXCEED_LIMIT;
    }
    return TELEPHONY_SUCCESS;
}

int32_t CsConference::CanSeparateConference()
{
    std::lock_guard<std::mutex> lock(conferenceMutex_);
    if (subCallIdSet_.empty()) {
        TELEPHONY_LOGE("no call is currently in the conference!");
        return CALL_ERR_CONFERENCE_NOT_EXISTS;
    }
    if (state_ != CONFERENCE_STATE_ACTIVE) {
        TELEPHONY_LOGE("call is not active!");
        return CALL_ERR_CONFERENCE_CALL_IS_NOT_ACTIVE;
    }
    return TELEPHONY_SUCCESS;
}

int32_t CsConference::CanKickOutFromConference()
{
    std::lock_guard<std::mutex> lock(conferenceMutex_);
    if (subCallIdSet_.empty()) {
        TELEPHONY_LOGE("no call is currently in the conference!");
        return CALL_ERR_CONFERENCE_NOT_EXISTS;
    }
    return TELEPHONY_SUCCESS;
}
} // namespace Telephony
} // namespace OHOS
