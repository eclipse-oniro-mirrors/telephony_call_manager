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

#include "call_object_manager.h"

#include "call_manager_errors.h"
#include "call_manager_log.h"

namespace OHOS {
namespace TelephonyCallManager {
std::list<sptr<CallBase>> CallObjectManager::callObjectPtrList_;
std::mutex CallObjectManager::listMutex_;
int32_t CallObjectManager::callId_ = kCallStartId;

CallObjectManager::CallObjectManager()
{
    callObjectPtrList_.clear();
}

CallObjectManager::~CallObjectManager()
{
    std::list<sptr<CallBase>>::iterator it = callObjectPtrList_.begin();
    while (it != callObjectPtrList_.end()) {
        (*it) = nullptr;
        callObjectPtrList_.erase(it++);
    }
}

int32_t CallObjectManager::AddOneCallObject(sptr<CallBase> call)
{
    if (call == nullptr) {
        return CALL_MANAGER_CALL_NULL;
    }
    std::lock_guard<std::mutex> lock(listMutex_);
    callObjectPtrList_.emplace_back(call);
    return TELEPHONY_NO_ERROR;
}

int32_t CallObjectManager::DeleteOneCallObject(int32_t callId)
{
    std::lock_guard<std::mutex> lock(listMutex_);
    std::list<sptr<CallBase>>::iterator it;
    for (it = callObjectPtrList_.begin(); it != callObjectPtrList_.end(); it++) {
        CALLMANAGER_DEBUG_LOG("local callId:%{public}d,callId:%{public}d", (*it)->GetCallID(), callId);
        if ((*it)->GetCallID() == callId) {
            callObjectPtrList_.erase(it);
            break;
        }
    }
    return TELEPHONY_NO_ERROR;
}

void CallObjectManager::DeleteOneCallObject(sptr<CallBase> call)
{
    if (call == nullptr) {
        CALLMANAGER_DEBUG_LOG("call is null!");
        return;
    }
    std::lock_guard<std::mutex> lock(listMutex_);
    callObjectPtrList_.remove(call);
}

sptr<CallBase> CallObjectManager::GetOneCallObject(int32_t callId)
{
    sptr<CallBase> retPtr = nullptr;
    std::lock_guard<std::mutex> lock(listMutex_);
    std::list<sptr<CallBase>>::iterator it = CallObjectManager::callObjectPtrList_.begin();
    for (; it != callObjectPtrList_.end(); it++) {
        CALLMANAGER_DEBUG_LOG("local callId:%{public}d,callId:%{public}d", (*it)->GetCallID(), callId);
        if ((*it)->GetCallID() == callId) {
            retPtr = *it;
            break;
        }
    }
    return retPtr;
}

sptr<CallBase> CallObjectManager::GetOneCallObject(std::string &phoneNumber)
{
    if (phoneNumber.empty()) {
        CALLMANAGER_DEBUG_LOG("call is null!");
        return nullptr;
    }
    sptr<CallBase> retPtr = nullptr;
    std::lock_guard<std::mutex> lock(listMutex_);
    std::list<sptr<CallBase>>::iterator it = callObjectPtrList_.begin();
    for (; it != callObjectPtrList_.end(); it++) {
        if ((*it)->GetPhoneNumber() == phoneNumber) {
            CALLMANAGER_DEBUG_LOG("GetOneCallObject success!");
            retPtr = *it;
            break;
        }
    }
    return retPtr;
}

int32_t CallObjectManager::HasNewCall()
{
    std::lock_guard<std::mutex> lock(listMutex_);
    std::list<sptr<CallBase>>::iterator it;
    for (it = callObjectPtrList_.begin(); it != callObjectPtrList_.end(); it++) {
        CALLMANAGER_DEBUG_LOG("callid %{public}d, state %{public}d", (*it)->GetCallID(), (*it)->GetState());
        if ((*it)->GetState() == CallStateType::CALL_STATE_CREATE_TYPE ||
            (*it)->GetState() == CallStateType::CALL_STATE_CONNECTING_TYPE) {
            return CALL_MANAGER_HAS_NEW_CALL;
        }
    }
    return TELEPHONY_NO_ERROR;
}

int32_t CallObjectManager::GetActiveCallList(std::list<sptr<CallBase>> &list)
{
    list.clear();
    std::lock_guard<std::mutex> lock(listMutex_);
    std::list<sptr<CallBase>>::iterator it;
    for (it = callObjectPtrList_.begin(); it != callObjectPtrList_.end(); it++) {
        if ((*it)->GetState() == CallStateType::CALL_STATE_ACTIVE_TYPE) {
            list.emplace_back(*it);
        }
    }
    return TELEPHONY_NO_ERROR;
}

int32_t CallObjectManager::GetHoldCallList(std::list<sptr<CallBase>> &list)
{
    list.clear();
    std::lock_guard<std::mutex> lock(listMutex_);
    std::list<sptr<CallBase>>::iterator it;
    for (it = callObjectPtrList_.begin(); it != callObjectPtrList_.end(); it++) {
        if ((*it)->GetState() == CallStateType::CALL_STATE_HOLD_TYPE) {
            list.emplace_back(*it);
        }
    }
    return TELEPHONY_NO_ERROR;
}

bool CallObjectManager::HasRingingMaximum()
{
    int32_t ringingCount = 0;
    std::lock_guard<std::mutex> lock(listMutex_);
    std::list<sptr<CallBase>>::iterator it;
    for (it = callObjectPtrList_.begin(); it != callObjectPtrList_.end(); it++) {
        // Count the number of calls in the ringing state
        if ((*it)->GetState() == CallStateType::CALL_STATE_RINGING_TYPE) {
            ringingCount++;
        }
    }
    if (ringingCount >= kMaxRingingCallNumberLen) {
        return true;
    }
    return false;
}

bool CallObjectManager::HasDialingMaximum()
{
    int32_t dialingCount = 0;
    std::lock_guard<std::mutex> lock(listMutex_);
    std::list<sptr<CallBase>>::iterator it;
    for (it = callObjectPtrList_.begin(); it != callObjectPtrList_.end(); it++) {
        // Count the number of calls in the active state
        if ((*it)->GetState() == CallStateType::CALL_STATE_ACTIVE_TYPE) {
            dialingCount++;
        }
    }
    if (dialingCount >= kMaxDialingCallNumberLen) {
        return true;
    }
    return false;
}

bool CallObjectManager::HasEmergencyCall()
{
    std::lock_guard<std::mutex> lock(listMutex_);
    std::list<sptr<CallBase>>::iterator it;
    for (it = callObjectPtrList_.begin(); it != callObjectPtrList_.end(); it++) {
        if ((*it)->GetEmergencyState()) {
            return true;
        }
    }
    return false;
}

int32_t CallObjectManager::GetNextCallId()
{
    int32_t ret = 0;
    std::lock_guard<std::mutex> lock(listMutex_);
    ret = ++callId_;
    return ret;
}

bool CallObjectManager::IsCallExist(std::string &phoneNumber)
{
    if (phoneNumber.empty()) {
        return CALL_MANAGER_PHONENUM_NULL;
    }
    std::lock_guard<std::mutex> lock(listMutex_);
    std::list<sptr<CallBase>>::iterator it = callObjectPtrList_.begin();
    for (; it != callObjectPtrList_.end(); it++) {
        if ((*it)->GetPhoneNumber() == phoneNumber) {
            return true;
        }
    }
    return false;
}

bool CallObjectManager::HasCallExist()
{
    std::lock_guard<std::mutex> lock(listMutex_);
    if (callObjectPtrList_.empty()) {
        return false;
    }
    return true;
}

bool CallObjectManager::HasRingingCall()
{
    bool ret = false;
    std::lock_guard<std::mutex> lock(listMutex_);
    std::list<sptr<CallBase>>::iterator it;
    for (it = callObjectPtrList_.begin(); it != callObjectPtrList_.end(); it++) {
        // Count the number of calls in the ringing state
        if ((*it)->GetState() == CallStateType::CALL_STATE_RINGING_TYPE) {
            ret = true;
            break;
        }
    }
    return ret;
}
} // namespace TelephonyCallManager
} // namespace OHOS
