/*
 * Copyright (C) 2021-2022 Huawei Device Co., Ltd.
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

#ifndef CALL_MANAGER_CONNECT_H
#define CALL_MANAGER_CONNECT_H
#include <string>
#include <map>
#include <list>
#include <mutex>
#include <string_ex.h>

#include "iservice_registry.h"
#include "system_ability.h"
#include "system_ability_definition.h"
#include "if_system_ability_manager.h"
#include "pac_map.h"
#include "refbase.h"
#include "rwlock.h"
#include "iremote_object.h"
#include "iremote_stub.h"

#include "call_manager_errors.h"
#include "telephony_log_wrapper.h"
#include "call_manager_service_proxy.h"
#include "i_call_ability_callback.h"

namespace OHOS {
namespace Telephony {
std::unordered_map<int32_t, std::unordered_set<int32_t>> g_callStateMap;
int32_t newCallId_ = -1;
class CallInfoManager {
public:
    static int32_t CallDetailsChange(const CallAttributeInfo &info);
    static int32_t CallEventChange(const CallEventInfo &info);
    static void Init();
    // replace LOCK_NUM_WHILE_EQ and LOCK_NUM_WHILE_NE
    static void LockCallId(bool eq, int32_t originVal, int32_t slipMs, int32_t timeoutMs);
    static void LockCallState(bool eq, int32_t originVal, int32_t slipMs, int32_t timeoutMs);
    static bool HasState(int32_t callId, int32_t callState);
    static bool HasActiveStatus();

private:
    static std::mutex mutex_;
    static int16_t newCallState_;
    static CallAttributeInfo updateCallInfo_;
    static std::unordered_set<int32_t> callIdSet_;
};

std::mutex CallInfoManager::mutex_;
int16_t CallInfoManager::newCallState_;
CallAttributeInfo CallInfoManager::updateCallInfo_;
std::unordered_set<int32_t> CallInfoManager::callIdSet_;

class CallAbilityCallbackStub : public IRemoteStub<ICallAbilityCallback> {
public:
    CallAbilityCallbackStub()
    {
        memberFuncMap_[UPDATE_CALL_STATE_INFO] = &CallAbilityCallbackStub::OnUpdateCallStateInfoRequest;
        memberFuncMap_[UPDATE_CALL_EVENT] = &CallAbilityCallbackStub::OnUpdateCallEventRequest;
        memberFuncMap_[UPDATE_CALL_ASYNC_RESULT_REQUEST] = &CallAbilityCallbackStub::OnUpdateAsyncResultRequest;
    }

    ~CallAbilityCallbackStub()
    {
        memberFuncMap_.clear();
    }

    int32_t OnRemoteRequest(uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option)
    {
        std::u16string myDescriptor = CallAbilityCallbackStub::GetDescriptor();
        std::u16string remoteDescriptor = data.ReadInterfaceToken();
        if (myDescriptor != remoteDescriptor) {
            TELEPHONY_LOGE("descriptor checked failed");
            return TELEPHONY_ERR_DESCRIPTOR_MISMATCH;
        }
        TELEPHONY_LOGI("OnReceived, cmd = %{public}u", code);
        auto itFunc = memberFuncMap_.find(code);
        if (itFunc != memberFuncMap_.end()) {
            auto memberFunc = itFunc->second;
            if (memberFunc != nullptr) {
                return (this->*memberFunc)(data, reply);
            }
        }
        return IPCObjectStub::OnRemoteRequest(code, data, reply, option);
    }

    int32_t OnCallDetailsChange(const CallAttributeInfo &info)
    {
        TELEPHONY_LOGI("UCallDetailsChange callA!");
        return CallInfoManager::CallDetailsChange(info);
    }

    int32_t OnCallEventChange(const CallEventInfo &info)
    {
        TELEPHONY_LOGI("UpdateCallEvent success!");
        return CallInfoManager::CallEventChange(info);
    }

    int32_t OnCallDisconnectedCause(DisconnectedDetails cause)
    {
        TELEPHONY_LOGI("OnCallDisconnectedCause success!");
        return TELEPHONY_SUCCESS;
    }

    int32_t OnReportAsyncResults(CallResultReportId reportId, AppExecFwk::PacMap &resultInfo)
    {
        TELEPHONY_LOGI("OnReportAsyncResults success!");
        return TELEPHONY_SUCCESS;
    }

    int32_t OnReportMmiCodeResult(const MmiCodeInfo &info)
    {
        TELEPHONY_LOGI("OnReportMmiCodeResult success!");
        return TELEPHONY_SUCCESS;
    }

    int32_t OnOttCallRequest(OttCallRequestId requestId, AppExecFwk::PacMap &info)
    {
        TELEPHONY_LOGI("OnOttCallRequest success!");
        return TELEPHONY_SUCCESS;
    }

private:
    using CallAbilityCallbackFunc = int32_t (CallAbilityCallbackStub::*)(MessageParcel &data, MessageParcel &reply);

    int32_t OnUpdateCallStateInfoRequest(MessageParcel &data, MessageParcel &reply)
    {
        const CallAttributeInfo *parcelPtr = nullptr;
        int32_t len = data.ReadInt32();
        if (len <= 0) {
            TELEPHONY_LOGE("Invalid parameter, len = %{public}d", len);
            return TELEPHONY_ERR_ARGUMENT_INVALID;
        }
        if (!data.ContainFileDescriptors()) {
            TELEPHONY_LOGW("sent raw data is less than 32k");
        }
        if ((parcelPtr = reinterpret_cast<const CallAttributeInfo *>(data.ReadRawData(len))) == nullptr) {
            TELEPHONY_LOGE("reading raw data failed, length = %{public}d", len);
            return TELEPHONY_ERR_LOCAL_PTR_NULL;
        }

        int32_t result = OnCallDetailsChange(*parcelPtr);
        if (!reply.WriteInt32(result)) {
            TELEPHONY_LOGE("writing parcel failed");
            return TELEPHONY_ERR_WRITE_REPLY_FAIL;
        }
        return TELEPHONY_SUCCESS;
    }

    int32_t OnUpdateCallEventRequest(MessageParcel &data, MessageParcel &reply)
    {
        const CallEventInfo *parcelPtr = nullptr;
        int32_t len = data.ReadInt32();
        if (len <= 0) {
            TELEPHONY_LOGE("Invalid parameter, len = %{public}d", len);
            return TELEPHONY_ERR_ARGUMENT_INVALID;
        }
        if (!data.ContainFileDescriptors()) {
            TELEPHONY_LOGW("sent raw data is less than 32k");
        }
        if ((parcelPtr = reinterpret_cast<const CallEventInfo *>(data.ReadRawData(len))) == nullptr) {
            TELEPHONY_LOGE("reading raw data failed, length = %d", len);
            return TELEPHONY_ERR_LOCAL_PTR_NULL;
        }

        int32_t result = OnCallEventChange(*parcelPtr);
        if (!reply.WriteInt32(result)) {
            TELEPHONY_LOGE("writing parcel failed");
            return TELEPHONY_ERR_WRITE_REPLY_FAIL;
        }
        return TELEPHONY_SUCCESS;
    }

    int32_t OnUpdateAsyncResultRequest(MessageParcel &data, MessageParcel &reply)
    {
        AppExecFwk::PacMap resultInfo;
        CallResultReportId reportId = static_cast<CallResultReportId>(data.ReadInt32());
        resultInfo.PutIntValue("result", data.ReadInt32());
        switch (reportId) {
            case CallResultReportId::GET_CALL_WAITING_REPORT_ID:
            case CallResultReportId::GET_CALL_RESTRICTION_REPORT_ID:
                resultInfo.PutIntValue("status", data.ReadInt32());
                resultInfo.PutIntValue("classCw", data.ReadInt32());
                break;
            case CallResultReportId::GET_CALL_TRANSFER_REPORT_ID:
                resultInfo.PutIntValue("status", data.ReadInt32());
                resultInfo.PutIntValue("classx", data.ReadInt32());
                resultInfo.PutStringValue("number", data.ReadString());
                resultInfo.PutIntValue("type", data.ReadInt32());
                resultInfo.PutIntValue("reason", data.ReadInt32());
                resultInfo.PutIntValue("time", data.ReadInt32());
                break;
            case CallResultReportId::GET_CALL_CLIP_ID:
                resultInfo.PutIntValue("action", data.ReadInt32());
                resultInfo.PutIntValue("clipStat", data.ReadInt32());
                break;
            case CallResultReportId::GET_CALL_CLIR_ID:
                resultInfo.PutIntValue("action", data.ReadInt32());
                resultInfo.PutIntValue("clirStat", data.ReadInt32());
                break;
            case CallResultReportId::GET_CALL_VOTLE_REPORT_ID:
            case CallResultReportId::START_RTT_REPORT_ID:
                resultInfo.PutIntValue("active", data.ReadInt32());
                break;
            case CallResultReportId::GET_IMS_CONFIG_REPORT_ID:
            case CallResultReportId::GET_IMS_FEATURE_VALUE_REPORT_ID:
            case CallResultReportId::GET_LTE_ENHANCE_MODE_REPORT_ID:
                resultInfo.PutIntValue("value", data.ReadInt32());
                break;
            case CallResultReportId::STOP_RTT_REPORT_ID:
                resultInfo.PutIntValue("inactive", data.ReadInt32());
                break;
            default:
                break;
        }
        if (!data.ContainFileDescriptors()) {
            TELEPHONY_LOGW("sent raw data is less than 32k");
        }
        int32_t result = OnReportAsyncResults(reportId, resultInfo);
        if (!reply.WriteInt32(result)) {
            TELEPHONY_LOGE("writing parcel failed");
            return TELEPHONY_ERR_WRITE_REPLY_FAIL;
        }
        return TELEPHONY_SUCCESS;
    }

private:
    std::map<uint32_t, CallAbilityCallbackFunc> memberFuncMap_;
};

class CallManagerConnect {
public:
    CallManagerConnect()
    {
        callAbilityCallbackPtr_ = nullptr;
        callManagerServicePtr_ = nullptr;
        systemAbilityId_ = TELEPHONY_CALL_MANAGER_SYS_ABILITY_ID;
    }

    ~CallManagerConnect()
    {
        if (callManagerServicePtr_) {
            callManagerServicePtr_.clear();
            callManagerServicePtr_ = nullptr;
        }
    }

    int32_t Init(int32_t systemAbilityId)
    {
        TELEPHONY_LOGI("Enter CallManagerIpcClient::Init,systemAbilityId:%d\n", systemAbilityId);
        systemAbilityId_ = systemAbilityId;
        int32_t result = ConnectService();
        TELEPHONY_LOGI("Connect service: %X\n", result);
        return result;
    }

    void UnInit()
    {
        DisconnectService();
    }

    int32_t DialCall(std::u16string number, AppExecFwk::PacMap &extras) const
    {
        if (callManagerServicePtr_ != nullptr) {
            return callManagerServicePtr_->DialCall(number, extras);
        }
        TELEPHONY_LOGE("callManagerServicePtr_ is nullptr!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }

    int32_t AnswerCall(int32_t callId, int32_t videoState) const
    {
        if (callManagerServicePtr_ != nullptr) {
            return callManagerServicePtr_->AnswerCall(callId, videoState);
        }
        TELEPHONY_LOGE("callManagerServicePtr_ is nullptr!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }

    int32_t RejectCall(int32_t callId, bool isSendSms, std::u16string content) const
    {
        if (callManagerServicePtr_ != nullptr) {
            return callManagerServicePtr_->RejectCall(callId, isSendSms, content);
        }
        TELEPHONY_LOGE("callManagerServicePtr_ is nullptr!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }

    int32_t HoldCall(int32_t callId) const
    {
        if (callManagerServicePtr_ != nullptr) {
            return callManagerServicePtr_->HoldCall(callId);
        }
        TELEPHONY_LOGE("callManagerServicePtr_ is nullptr!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }

    int32_t UnHoldCall(int32_t callId) const
    {
        if (callManagerServicePtr_ != nullptr) {
            return callManagerServicePtr_->UnHoldCall(callId);
        }
        TELEPHONY_LOGE("callManagerServicePtr_ is nullptr!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }

    int32_t HangUpCall(int32_t callId) const
    {
        if (callManagerServicePtr_ != nullptr) {
            return callManagerServicePtr_->HangUpCall(callId);
        }
        TELEPHONY_LOGE("callManagerServicePtr_ is nullptr!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }

    int32_t GetCallState() const
    {
        if (callManagerServicePtr_ != nullptr) {
            return callManagerServicePtr_->GetCallState();
        }
        TELEPHONY_LOGE("callManagerServicePtr_ is nullptr!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }

    int32_t SwitchCall(int32_t callId) const
    {
        if (callManagerServicePtr_ != nullptr) {
            return callManagerServicePtr_->SwitchCall(callId);
        }
        TELEPHONY_LOGE("callManagerServicePtr_ is nullptr!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }

    bool HasCall() const
    {
        if (callManagerServicePtr_ != nullptr) {
            return callManagerServicePtr_->HasCall();
        }
        TELEPHONY_LOGE("callManagerServicePtr_ is nullptr!");
        return false;
    }

    bool IsNewCallAllowed() const
    {
        if (callManagerServicePtr_ != nullptr) {
            return callManagerServicePtr_->IsNewCallAllowed();
        }
        TELEPHONY_LOGE("callManagerServicePtr_ is nullptr!");
        return false;
    }

    bool IsRinging() const
    {
        if (callManagerServicePtr_ != nullptr) {
            return callManagerServicePtr_->IsRinging();
        }
        TELEPHONY_LOGE("callManagerServicePtr_ is nullptr!");
        return false;
    }

    bool IsInEmergencyCall() const
    {
        if (callManagerServicePtr_ != nullptr) {
            return callManagerServicePtr_->IsInEmergencyCall();
        }
        TELEPHONY_LOGE("callManagerServicePtr_ is nullptr!");
        return false;
    }

    int32_t StartDtmf(int32_t callId, char c) const
    {
        if (callManagerServicePtr_ != nullptr) {
            return callManagerServicePtr_->StartDtmf(callId, c);
        }
        TELEPHONY_LOGE("callManagerServicePtr_ is nullptr!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }

    int32_t StopDtmf(int32_t callId) const
    {
        if (callManagerServicePtr_ != nullptr) {
            return callManagerServicePtr_->StopDtmf(callId);
        }
        TELEPHONY_LOGE("callManagerServicePtr_ is nullptr!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }

    int32_t GetCallWaiting(int32_t slotId) const
    {
        if (callManagerServicePtr_ != nullptr) {
            return callManagerServicePtr_->GetCallWaiting(slotId);
        }
        TELEPHONY_LOGE("callManagerServicePtr_ is nullptr!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }

    int32_t SetCallWaiting(int32_t slotId, bool activate) const
    {
        if (callManagerServicePtr_ != nullptr) {
            return callManagerServicePtr_->SetCallWaiting(slotId, activate);
        }
        TELEPHONY_LOGE("callManagerServicePtr_ is nullptr!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }

    int32_t GetCallRestriction(int32_t slotId, CallRestrictionType type)
    {
        if (callManagerServicePtr_ != nullptr) {
            return callManagerServicePtr_->GetCallRestriction(slotId, type);
        }
        TELEPHONY_LOGE("callManagerServicePtr_ is nullptr!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }

    int32_t SetCallRestriction(int32_t slotId, CallRestrictionInfo &info)
    {
        if (callManagerServicePtr_ != nullptr) {
            return callManagerServicePtr_->SetCallRestriction(slotId, info);
        }
        TELEPHONY_LOGE("callManagerServicePtr_ is nullptr!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }

    int32_t GetCallTransferInfo(int32_t slotId, CallTransferType type)
    {
        if (callManagerServicePtr_ != nullptr) {
            return callManagerServicePtr_->GetCallTransferInfo(slotId, type);
        }
        TELEPHONY_LOGE("callManagerServicePtr_ is nullptr!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }

    int32_t SetCallTransferInfo(int32_t slotId, CallTransferInfo &info)
    {
        if (callManagerServicePtr_ != nullptr) {
            return callManagerServicePtr_->SetCallTransferInfo(slotId, info);
        }
        TELEPHONY_LOGE("callManagerServicePtr_ is nullptr!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }

    int32_t CombineConference(int32_t mainCallId) const
    {
        if (callManagerServicePtr_ != nullptr) {
            return callManagerServicePtr_->CombineConference(mainCallId);
        }
        TELEPHONY_LOGE("callManagerServicePtr_ is nullptr!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }

    int32_t SeparateConference(int32_t callId) const
    {
        if (callManagerServicePtr_ != nullptr) {
            return callManagerServicePtr_->SeparateConference(callId);
        }
        TELEPHONY_LOGE("callManagerServicePtr_ is nullptr!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }

    bool IsEmergencyPhoneNumber(std::u16string &number, int32_t slotId, int32_t &errorCode) const
    {
        if (callManagerServicePtr_ != nullptr) {
            return callManagerServicePtr_->IsEmergencyPhoneNumber(number, slotId, errorCode);
        }
        TELEPHONY_LOGE("callManagerServicePtr_ is nullptr!");
        return false;
    }

    int32_t FormatPhoneNumber(
        std::u16string &number, std::u16string &countryCode, std::u16string &formatNumber) const
    {
        if (callManagerServicePtr_ != nullptr) {
            return callManagerServicePtr_->FormatPhoneNumber(number, countryCode, formatNumber);
        }
        TELEPHONY_LOGE("callManagerServicePtr_ is nullptr!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }

    int32_t FormatPhoneNumberToE164(
        std::u16string &number, std::u16string &countryCode, std::u16string &formatNumber) const
    {
        if (callManagerServicePtr_ != nullptr) {
            return callManagerServicePtr_->FormatPhoneNumberToE164(number, countryCode, formatNumber);
        }
        TELEPHONY_LOGE("callManagerServicePtr_ is nullptr!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }

    int32_t GetMainCallId(int32_t callId)
    {
        if (callManagerServicePtr_ != nullptr) {
            return callManagerServicePtr_->GetMainCallId(callId);
        }
        TELEPHONY_LOGE("callManagerServicePtr_ is nullptr!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }

    std::vector<std::u16string> GetSubCallIdList(int32_t callId)
    {
        if (callManagerServicePtr_ != nullptr) {
            return callManagerServicePtr_->GetSubCallIdList(callId);
        }
        std::vector<std::u16string> vec;
        vec.clear();
        return vec;
    }

    std::vector<std::u16string> GetCallIdListForConference(int32_t callId)
    {
        if (callManagerServicePtr_ != nullptr) {
            return callManagerServicePtr_->GetCallIdListForConference(callId);
        }
        std::vector<std::u16string> vec;
        vec.clear();
        return vec;
    }

    int32_t ControlCamera(std::u16string cameraId)
    {
        if (callManagerServicePtr_ != nullptr) {
            return callManagerServicePtr_->ControlCamera(cameraId);
        }
        TELEPHONY_LOGE("callManagerServicePtr_ is nullptr!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }

    int32_t SetAudioDevice(AudioDevice deviceType, const std::string &bluetoothAddress)
    {
        if (callManagerServicePtr_ == nullptr) {
            TELEPHONY_LOGE("callManagerServicePtr_ is nullptr!");
            return TELEPHONY_ERR_LOCAL_PTR_NULL;
        }
        return callManagerServicePtr_->SetAudioDevice(deviceType, bluetoothAddress);
    }

    int32_t SetPreviewWindow(VideoWindow &window)
    {
        if (callManagerServicePtr_ != nullptr) {
            return callManagerServicePtr_->SetPreviewWindow(window);
        }
        TELEPHONY_LOGE("callManagerServicePtr_ is nullptr!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }

    int32_t SetDisplayWindow(VideoWindow &window)
    {
        if (callManagerServicePtr_ != nullptr) {
            return callManagerServicePtr_->SetDisplayWindow(window);
        }
        TELEPHONY_LOGE("callManagerServicePtr_ is nullptr!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }

    int32_t SetCameraZoom(float zoomRatio)
    {
        if (callManagerServicePtr_ != nullptr) {
            return callManagerServicePtr_->SetCameraZoom(zoomRatio);
        }
        TELEPHONY_LOGE("callManagerServicePtr_ is nullptr!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }

    int32_t SetPausePicture(std::u16string path)
    {
        if (callManagerServicePtr_ != nullptr) {
            return callManagerServicePtr_->SetPausePicture(path);
        }
        TELEPHONY_LOGE("callManagerServicePtr_ is nullptr!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }

    int32_t SetDeviceDirection(int32_t rotation)
    {
        if (callManagerServicePtr_ != nullptr) {
            return callManagerServicePtr_->SetDeviceDirection(rotation);
        }
        TELEPHONY_LOGE("callManagerServicePtr_ is nullptr!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }

    int32_t EnableImsSwitch(int32_t slotId)
    {
        if (callManagerServicePtr_ != nullptr) {
            return callManagerServicePtr_->EnableImsSwitch(slotId);
        }
        TELEPHONY_LOGE("callManagerServicePtr_ is nullptr!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }

    int32_t DisableImsSwitch(int32_t slotId)
    {
        if (callManagerServicePtr_ != nullptr) {
            return callManagerServicePtr_->DisableImsSwitch(slotId);
        }
        TELEPHONY_LOGE("callManagerServicePtr_ is nullptr!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }

    int32_t IsImsSwitchEnabled(int32_t slotId)
    {
        if (callManagerServicePtr_ != nullptr) {
            return callManagerServicePtr_->IsImsSwitchEnabled(slotId);
        }
        TELEPHONY_LOGE("callManagerServicePtr_ is nullptr!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }

    int32_t SetMuted(bool isMuted) const
    {
        if (callManagerServicePtr_ != nullptr) {
            return callManagerServicePtr_->SetMuted(isMuted);
        }
        TELEPHONY_LOGE("callManagerServicePtr_ is nullptr!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }

    int32_t RegisterCallBack()
    {
        if (callManagerServicePtr_ == nullptr) {
            TELEPHONY_LOGE("callManagerServicePtr_ is null");
            return TELEPHONY_ERR_LOCAL_PTR_NULL;
        }
        callAbilityCallbackPtr_ = (std::make_unique<CallAbilityCallbackStub>()).release();
        if (callAbilityCallbackPtr_ == nullptr) {
            DisconnectService();
            TELEPHONY_LOGE("create CallAbilityCallbackStub object failed!");
            return TELEPHONY_ERR_LOCAL_PTR_NULL;
        }
        int32_t ret = callManagerServicePtr_->RegisterCallBack(callAbilityCallbackPtr_);
        if (ret != TELEPHONY_SUCCESS) {
            DisconnectService();
            TELEPHONY_LOGE("register callback to call manager service failed,result: %{public}d", ret);
            return TELEPHONY_ERR_REGISTER_CALLBACK_FAIL;
        }
        TELEPHONY_LOGI("register call ability callback success!");
        return TELEPHONY_SUCCESS;
    }

private:
    int32_t ConnectService()
    {
        Utils::UniqueWriteGuard<Utils::RWLock> guard(rwClientLock_);
        if (callManagerServicePtr_ != nullptr) {
            return TELEPHONY_SUCCESS;
        }
        sptr<ISystemAbilityManager> managerPtr =
            SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
        if (managerPtr == nullptr) {
            return TELEPHONY_ERR_LOCAL_PTR_NULL;
        }
        sptr<ICallManagerService> callManagerServicePtr = nullptr;
        sptr<IRemoteObject> iRemoteObjectPtr = managerPtr->GetSystemAbility(systemAbilityId_);
        if (iRemoteObjectPtr == nullptr) {
            return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
        }
        callManagerServicePtr = iface_cast<ICallManagerService>(iRemoteObjectPtr);
        if (!callManagerServicePtr) {
            return TELEPHONY_ERR_LOCAL_PTR_NULL;
        }
        callManagerServicePtr_ = callManagerServicePtr;
        int32_t ret = RegisterCallBack();
        if (ret != TELEPHONY_SUCCESS) {
            return ret;
        }
        return TELEPHONY_SUCCESS;
    }

    void DisconnectService()
    {
        Utils::UniqueWriteGuard<Utils::RWLock> guard(rwClientLock_);
        if (callManagerServicePtr_) {
            callManagerServicePtr_.clear();
            callManagerServicePtr_ = nullptr;
        }
        if (callAbilityCallbackPtr_) {
            callAbilityCallbackPtr_.clear();
            callAbilityCallbackPtr_ = nullptr;
        }
    }

private:
    int32_t systemAbilityId_;
    sptr<ICallManagerService> callManagerServicePtr_;
    sptr<ICallAbilityCallback> callAbilityCallbackPtr_;
    Utils::RWLock rwClientLock_;
};
} // namespace Telephony
} // namespace OHOS

#endif
