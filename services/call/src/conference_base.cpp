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

#include "conference_base.h"

#include <string_ex.h>

#include "call_object_manager.h"
#include "call_manager_errors.h"
#include "telephony_log_wrapper.h"

namespace OHOS {
namespace Telephony {
ConferenceBase::ConferenceBase() : mainCallId_(ERR_ID), state_(CONFERENCE_STATE_IDLE), beginTime_(0)
{
    subCallIdSet_.clear();
}

ConferenceBase::~ConferenceBase()
{
    std::lock_guard<std::mutex> lock(conferenceMutex_);
    subCallIdSet_.clear();
}

int32_t ConferenceBase::GetMainCall()
{
    std::lock_guard<std::mutex> lock(conferenceMutex_);
    return mainCallId_;
}

int32_t ConferenceBase::SetMainCall(int32_t callId)
{
    if (callId <= ERR_ID) {
        TELEPHONY_LOGE("callId is invalid:%{public}d", callId);
        return CALL_ERR_CALLID_INVALID;
    }
    int32_t ret = CanCombineConference();
    if (ret != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("join conference failed, errorCode:%{public}d", ret);
        return ret;
    }
    std::lock_guard<std::mutex> lock(conferenceMutex_);
    mainCallId_ = callId;
    state_ = CONFERENCE_STATE_CREATING;
    return TELEPHONY_SUCCESS;
}

ConferenceState ConferenceBase::GetConferenceState()
{
    std::lock_guard<std::mutex> lock(conferenceMutex_);
    return state_;
}

void ConferenceBase::SetConferenceState(ConferenceState state)
{
    std::lock_guard<std::mutex> lock(conferenceMutex_);
    state_ = state;
}

std::vector<std::u16string> ConferenceBase::GetSubCallIdList(int32_t callId)
{
    std::vector<std::u16string> vec;
    bool flag = false;
    vec.clear();
    std::lock_guard<std::mutex> lock(conferenceMutex_);
    for (auto it = subCallIdSet_.begin(); it != subCallIdSet_.end(); ++it) {
        if (*it == callId) {
            flag = true;
        }
        vec.push_back(Str8ToStr16(std::to_string(*it)));
        TELEPHONY_LOGI("subCallId_:%{public}d", *it);
    }
    if (!flag) {
        vec.clear();
        TELEPHONY_LOGW("the call is not in the conference, callId:%{public}d", callId);
    }
    return vec;
}

std::vector<std::u16string> ConferenceBase::GetCallIdListForConference(int32_t callId)
{
    std::vector<std::u16string> vec;
    bool flag = false;
    vec.clear();
    std::lock_guard<std::mutex> lock(conferenceMutex_);
    for (auto it = subCallIdSet_.begin(); it != subCallIdSet_.end(); ++it) {
        if (*it == callId) {
            flag = true;
        }
        vec.push_back(Str8ToStr16(std::to_string(*it)));
        TELEPHONY_LOGI("subCallId_:%{public}d", *it);
    }
    if (mainCallId_ == callId) {
        flag = true;
    }
    vec.push_back(Str8ToStr16(std::to_string(mainCallId_)));
    if (!flag) {
        vec.clear();
        TELEPHONY_LOGW("the call is not in the conference, callId:%{public}d", callId);
    }
    return vec;
}
} // namespace Telephony
} // namespace OHOS