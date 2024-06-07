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

#ifndef CALL_OBJECT_MANAGER_H
#define CALL_OBJECT_MANAGER_H

#include <condition_variable>
#include <cstdio>
#include <cstdlib>
#include <list>
#include <memory>
#include <mutex>

#include "refbase.h"

#include "call_base.h"
#include "common_type.h"
#include "call_manager_inner_type.h"

namespace OHOS {
namespace Telephony {
class CallObjectManager {
public:
    CallObjectManager();
    virtual ~CallObjectManager();

    static int32_t AddOneCallObject(sptr<CallBase> &call);
    static int32_t DeleteOneCallObject(int32_t callId);
    static void DeleteOneCallObject(sptr<CallBase> &call);
    static sptr<CallBase> GetOneCallObject(int32_t callId);
    static sptr<CallBase> GetOneCallObject(std::string &phoneNumber);
    static sptr<CallBase> GetOneCallObjectByIndex(int32_t index);
    static sptr<CallBase> GetOneCallObjectByIndexAndSlotId(int32_t index, int32_t slotId);
    static sptr<CallBase> GetOneCallObjectByVoipCallId(std::string voipCallId);
    static void UpdateOneCallObjectByCallId(int32_t callId, TelCallState nextCallState);
    static int32_t HasNewCall();
    static int32_t IsNewCallAllowedCreate(bool &enabled);
    static int32_t GetCurrentCallNum();
    static int32_t GetCarrierCallList(std::list<int32_t> &list);
    static int32_t GetVoipCallList(std::list<int32_t> &list);
    static bool HasRingingMaximum();
    static bool HasDialingMaximum();
    static int32_t HasEmergencyCall(bool &enabled);
    static int32_t GetNewCallId();
    static bool IsCallExist(int32_t callId);
    static bool IsCallExist(std::string &phoneNumber);
    static bool HasCallExist();
    static int32_t HasRingingCall(bool &enabled);
    static TelCallState GetCallState(int32_t callId);
    static sptr<CallBase> GetOneCallObject(CallRunningState callState);
    static bool IsCallExist(CallType type, TelCallState callState);
    static bool IsCallExist(TelCallState callState);
    static bool IsCallExist(TelCallState callState, int32_t &callId);
    static bool IsConferenceCallExist(TelConferenceState state, int32_t &callId);
    static int32_t GetCallNum(TelCallState callState);
    static std::string GetCallNumber(TelCallState callState);
    static std::vector<CallAttributeInfo> GetCallInfoList(int32_t slotId);
    static sptr<CallBase> GetForegroundCall(bool isIncludeVoipCall = true);
    static sptr<CallBase> GetForegroundLiveCall();
    static std::vector<CallAttributeInfo> GetAllCallInfoList();
    int32_t DealFailDial(sptr<CallBase> call);
    static bool HasVideoCall();
    static std::list<sptr<CallBase>> GetAllCallList();
    static bool HasCellularCallExist();
    static bool HasVoipCallExist();
    static CellularCallInfo GetDialCallInfo();
    static bool HasSatelliteCallExist();
    static int32_t GetSatelliteCallList(std::list<int32_t> &list);

protected:
    static std::condition_variable cv_;
    static bool isFirstDialCallAdded_;
    static bool needWaitHold_;
    static CellularCallInfo dialCallInfo_;

private:
    static std::list<sptr<CallBase>> callObjectPtrList_;
    static std::mutex listMutex_;
    static int32_t callId_;
};
} // namespace Telephony
} // namespace OHOS

#endif // CALL_OBJECT_MANAGER_H
