/*
 * Copyright (C) 2023 Huawei Device Co., Ltd.
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

#include "call_manager_connect.h"

namespace OHOS {
namespace Telephony {
int32_t CallAbilityCallbackStub::OnUpdateCallStateInfoRequest(MessageParcel &data, MessageParcel &reply)
{
    const CallAttributeInfo *parcelPtr = nullptr;
    int32_t length = data.ReadInt32();
    if (length <= 0 || length >= MAX_LEN) {
        TELEPHONY_LOGE("Invalid parameter, length = %{public}d", length);
        return TELEPHONY_ERR_ARGUMENT_INVALID;
    }
    if (!data.ContainFileDescriptors()) {
        TELEPHONY_LOGW("sent raw data is less than 32k");
    }
    if ((parcelPtr = reinterpret_cast<const CallAttributeInfo *>(data.ReadRawData(length))) == nullptr) {
        TELEPHONY_LOGE("reading raw data failed, length = %{public}d", length);
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }

    int32_t result = OnCallDetailsChange(*parcelPtr);
    if (!reply.WriteInt32(result)) {
        TELEPHONY_LOGE("writing parcel failed");
        return TELEPHONY_ERR_WRITE_REPLY_FAIL;
    }
    return TELEPHONY_SUCCESS;
}

int32_t CallAbilityCallbackStub::OnUpdateCallEventRequest(MessageParcel &data, MessageParcel &reply)
{
    const CallEventInfo *parcelPtr = nullptr;
    int32_t length = data.ReadInt32();
    if (length <= 0 || length >= MAX_LEN) {
        TELEPHONY_LOGE("Invalid parameter, length = %{public}d", length);
        return TELEPHONY_ERR_ARGUMENT_INVALID;
    }
    if (!data.ContainFileDescriptors()) {
        TELEPHONY_LOGW("sent raw data is less than 32k");
    }
    if ((parcelPtr = reinterpret_cast<const CallEventInfo *>(data.ReadRawData(length))) == nullptr) {
        TELEPHONY_LOGE("reading raw data failed, length = %d", length);
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }

    int32_t result = OnCallEventChange(*parcelPtr);
    if (!reply.WriteInt32(result)) {
        TELEPHONY_LOGE("writing parcel failed");
        return TELEPHONY_ERR_WRITE_REPLY_FAIL;
    }
    return TELEPHONY_SUCCESS;
}

int32_t CallAbilityCallbackStub::OnUpdateAsyncResultRequest(MessageParcel &data, MessageParcel &reply)
{
    AppExecFwk::PacMap info;
    CallResultReportId reportId = static_cast<CallResultReportId>(data.ReadInt32());
    info.PutIntValue("result", data.ReadInt32());
    switch (reportId) {
        case CallResultReportId::GET_CALL_WAITING_REPORT_ID:
        case CallResultReportId::GET_CALL_RESTRICTION_REPORT_ID:
            info.PutIntValue("status", data.ReadInt32());
            info.PutIntValue("classCw", data.ReadInt32());
            break;
        case CallResultReportId::GET_CALL_TRANSFER_REPORT_ID:
            info.PutIntValue("status", data.ReadInt32());
            info.PutIntValue("classx", data.ReadInt32());
            info.PutStringValue("number", data.ReadString());
            info.PutIntValue("type", data.ReadInt32());
            info.PutIntValue("reason", data.ReadInt32());
            info.PutIntValue("time", data.ReadInt32());
            break;
        case CallResultReportId::GET_CALL_CLIP_ID:
            info.PutIntValue("action", data.ReadInt32());
            info.PutIntValue("clipStat", data.ReadInt32());
            break;
        case CallResultReportId::GET_CALL_CLIR_ID:
            info.PutIntValue("action", data.ReadInt32());
            info.PutIntValue("clirStat", data.ReadInt32());
            break;
        case CallResultReportId::START_RTT_REPORT_ID:
            info.PutIntValue("active", data.ReadInt32());
            break;
        case CallResultReportId::GET_IMS_CONFIG_REPORT_ID:
        case CallResultReportId::GET_IMS_FEATURE_VALUE_REPORT_ID:
            info.PutIntValue("value", data.ReadInt32());
            break;
        case CallResultReportId::STOP_RTT_REPORT_ID:
            info.PutIntValue("inactive", data.ReadInt32());
            break;
        default:
            break;
    }
    if (!data.ContainFileDescriptors()) {
        TELEPHONY_LOGW("sent raw data is less than 32k");
    }
    int32_t result = OnReportAsyncResults(reportId, info);
    if (!reply.WriteInt32(result)) {
        TELEPHONY_LOGE("writing parcel failed");
        return TELEPHONY_ERR_WRITE_REPLY_FAIL;
    }
    return TELEPHONY_SUCCESS;
}

CallManagerConnect::CallManagerConnect()
{
    callAbilityCallbackPtr_ = nullptr;
    callManagerServicePtr_ = nullptr;
    systemAbilityId_ = TELEPHONY_CALL_MANAGER_SYS_ABILITY_ID;
}

CallManagerConnect::~CallManagerConnect()
{
    if (callManagerServicePtr_) {
        callManagerServicePtr_.clear();
        callManagerServicePtr_ = nullptr;
    }
}

int32_t CallManagerConnect::Init(int32_t systemAbilityId)
{
    AccessToken token;
    TELEPHONY_LOGI("Enter CallManagerIpcClient::Init,systemAbilityId:%d\n", systemAbilityId);
    systemAbilityId_ = systemAbilityId;
    int32_t result = ConnectService();
    TELEPHONY_LOGI("Connect service: %X\n", result);
    return result;
}

void CallManagerConnect::UnInit()
{
    DisconnectService();
}

int32_t CallManagerConnect::DialCall(std::u16string number, AppExecFwk::PacMap &extras) const
{
    if (callManagerServicePtr_ != nullptr) {
        return callManagerServicePtr_->DialCall(number, extras);
    }
    TELEPHONY_LOGE("callManagerServicePtr_ is nullptr!");
    return TELEPHONY_ERR_LOCAL_PTR_NULL;
}

int32_t CallManagerConnect::AnswerCall(int32_t callId, int32_t videoState) const
{
    if (callManagerServicePtr_ != nullptr) {
        return callManagerServicePtr_->AnswerCall(callId, videoState);
    }
    TELEPHONY_LOGE("callManagerServicePtr_ is nullptr!");
    return TELEPHONY_ERR_LOCAL_PTR_NULL;
}

int32_t CallManagerConnect::RejectCall(int32_t callId, bool isSendSms, std::u16string content) const
{
    if (callManagerServicePtr_ != nullptr) {
        return callManagerServicePtr_->RejectCall(callId, isSendSms, content);
    }
    TELEPHONY_LOGE("callManagerServicePtr_ is nullptr!");
    return TELEPHONY_ERR_LOCAL_PTR_NULL;
}

int32_t CallManagerConnect::HoldCall(int32_t callId) const
{
    if (callManagerServicePtr_ != nullptr) {
        return callManagerServicePtr_->HoldCall(callId);
    }
    TELEPHONY_LOGE("callManagerServicePtr_ is nullptr!");
    return TELEPHONY_ERR_LOCAL_PTR_NULL;
}

int32_t CallManagerConnect::UnHoldCall(int32_t callId) const
{
    if (callManagerServicePtr_ != nullptr) {
        return callManagerServicePtr_->UnHoldCall(callId);
    }
    TELEPHONY_LOGE("callManagerServicePtr_ is nullptr!");
    return TELEPHONY_ERR_LOCAL_PTR_NULL;
}

int32_t CallManagerConnect::HangUpCall(int32_t callId) const
{
    if (callManagerServicePtr_ != nullptr) {
        return callManagerServicePtr_->HangUpCall(callId);
    }
    TELEPHONY_LOGE("callManagerServicePtr_ is nullptr!");
    return TELEPHONY_ERR_LOCAL_PTR_NULL;
}

int32_t CallManagerConnect::GetCallState() const
{
    if (callManagerServicePtr_ != nullptr) {
        return callManagerServicePtr_->GetCallState();
    }
    TELEPHONY_LOGE("callManagerServicePtr_ is nullptr!");
    return TELEPHONY_ERR_LOCAL_PTR_NULL;
}

int32_t CallManagerConnect::SwitchCall(int32_t callId) const
{
    if (callManagerServicePtr_ != nullptr) {
        return callManagerServicePtr_->SwitchCall(callId);
    }
    TELEPHONY_LOGE("callManagerServicePtr_ is nullptr!");
    return TELEPHONY_ERR_LOCAL_PTR_NULL;
}

bool CallManagerConnect::HasCall() const
{
    if (callManagerServicePtr_ != nullptr) {
        return callManagerServicePtr_->HasCall();
    }
    TELEPHONY_LOGE("callManagerServicePtr_ is nullptr!");
    return false;
}

int32_t CallManagerConnect::IsNewCallAllowed(bool &enabled) const
{
    if (callManagerServicePtr_ != nullptr) {
        return callManagerServicePtr_->IsNewCallAllowed(enabled);
    }
    TELEPHONY_LOGE("callManagerServicePtr_ is nullptr!");
    return TELEPHONY_ERR_LOCAL_PTR_NULL;
}

int32_t CallManagerConnect::IsRinging(bool &enabled) const
{
    if (callManagerServicePtr_ != nullptr) {
        return callManagerServicePtr_->IsRinging(enabled);
    }
    TELEPHONY_LOGE("callManagerServicePtr_ is nullptr!");
    return TELEPHONY_ERR_LOCAL_PTR_NULL;
}

int32_t CallManagerConnect::IsInEmergencyCall(bool &enabled) const
{
    if (callManagerServicePtr_ != nullptr) {
        return callManagerServicePtr_->IsInEmergencyCall(enabled);
    }
    TELEPHONY_LOGE("callManagerServicePtr_ is nullptr!");
    return TELEPHONY_ERR_LOCAL_PTR_NULL;
}

int32_t CallManagerConnect::StartDtmf(int32_t callId, char c) const
{
    if (callManagerServicePtr_ != nullptr) {
        return callManagerServicePtr_->StartDtmf(callId, c);
    }
    TELEPHONY_LOGE("callManagerServicePtr_ is nullptr!");
    return TELEPHONY_ERR_LOCAL_PTR_NULL;
}

int32_t CallManagerConnect::StopDtmf(int32_t callId) const
{
    if (callManagerServicePtr_ != nullptr) {
        return callManagerServicePtr_->StopDtmf(callId);
    }
    TELEPHONY_LOGE("callManagerServicePtr_ is nullptr!");
    return TELEPHONY_ERR_LOCAL_PTR_NULL;
}

int32_t CallManagerConnect::GetCallWaiting(int32_t slotId) const
{
    if (callManagerServicePtr_ != nullptr) {
        return callManagerServicePtr_->GetCallWaiting(slotId);
    }
    TELEPHONY_LOGE("callManagerServicePtr_ is nullptr!");
    return TELEPHONY_ERR_LOCAL_PTR_NULL;
}

int32_t CallManagerConnect::SetCallWaiting(int32_t slotId, bool activate) const
{
    if (callManagerServicePtr_ != nullptr) {
        return callManagerServicePtr_->SetCallWaiting(slotId, activate);
    }
    TELEPHONY_LOGE("callManagerServicePtr_ is nullptr!");
    return TELEPHONY_ERR_LOCAL_PTR_NULL;
}

int32_t CallManagerConnect::GetCallRestriction(int32_t slotId, CallRestrictionType type)
{
    if (callManagerServicePtr_ != nullptr) {
        return callManagerServicePtr_->GetCallRestriction(slotId, type);
    }
    TELEPHONY_LOGE("callManagerServicePtr_ is nullptr!");
    return TELEPHONY_ERR_LOCAL_PTR_NULL;
}

int32_t CallManagerConnect::SetCallRestriction(int32_t slotId, CallRestrictionInfo &info)
{
    if (callManagerServicePtr_ != nullptr) {
        return callManagerServicePtr_->SetCallRestriction(slotId, info);
    }
    TELEPHONY_LOGE("callManagerServicePtr_ is nullptr!");
    return TELEPHONY_ERR_LOCAL_PTR_NULL;
}

int32_t CallManagerConnect::GetCallTransferInfo(int32_t slotId, CallTransferType type)
{
    if (callManagerServicePtr_ != nullptr) {
        return callManagerServicePtr_->GetCallTransferInfo(slotId, type);
    }
    TELEPHONY_LOGE("callManagerServicePtr_ is nullptr!");
    return TELEPHONY_ERR_LOCAL_PTR_NULL;
}

int32_t CallManagerConnect::SetCallTransferInfo(int32_t slotId, CallTransferInfo &info)
{
    if (callManagerServicePtr_ != nullptr) {
        return callManagerServicePtr_->SetCallTransferInfo(slotId, info);
    }
    TELEPHONY_LOGE("callManagerServicePtr_ is nullptr!");
    return TELEPHONY_ERR_LOCAL_PTR_NULL;
}

int32_t CallManagerConnect::CombineConference(int32_t mainCallId) const
{
    if (callManagerServicePtr_ != nullptr) {
        return callManagerServicePtr_->CombineConference(mainCallId);
    }
    TELEPHONY_LOGE("callManagerServicePtr_ is nullptr!");
    return TELEPHONY_ERR_LOCAL_PTR_NULL;
}

int32_t CallManagerConnect::SeparateConference(int32_t callId) const
{
    if (callManagerServicePtr_ != nullptr) {
        return callManagerServicePtr_->SeparateConference(callId);
    }
    TELEPHONY_LOGE("callManagerServicePtr_ is nullptr!");
    return TELEPHONY_ERR_LOCAL_PTR_NULL;
}

int32_t CallManagerConnect::KickOutFromConference(int32_t callId) const
{
    if (callManagerServicePtr_ != nullptr) {
        return callManagerServicePtr_->KickOutFromConference(callId);
    }
    TELEPHONY_LOGE("callManagerServicePtr_ is nullptr!");
    return TELEPHONY_ERR_LOCAL_PTR_NULL;
}

int32_t CallManagerConnect::IsEmergencyPhoneNumber(std::u16string &number, int32_t slotId, bool &enabled) const
{
    if (callManagerServicePtr_ != nullptr) {
        return callManagerServicePtr_->IsEmergencyPhoneNumber(number, slotId, enabled);
    }
    TELEPHONY_LOGE("callManagerServicePtr_ is nullptr!");
    return TELEPHONY_ERR_LOCAL_PTR_NULL;
}

int32_t CallManagerConnect::FormatPhoneNumber(
    std::u16string &number, std::u16string &countryCode, std::u16string &formatNumber) const
{
    if (callManagerServicePtr_ != nullptr) {
        return callManagerServicePtr_->FormatPhoneNumber(number, countryCode, formatNumber);
    }
    TELEPHONY_LOGE("callManagerServicePtr_ is nullptr!");
    return TELEPHONY_ERR_LOCAL_PTR_NULL;
}

int32_t CallManagerConnect::FormatPhoneNumberToE164(
    std::u16string &number, std::u16string &countryCode, std::u16string &formatNumber) const
{
    if (callManagerServicePtr_ != nullptr) {
        return callManagerServicePtr_->FormatPhoneNumberToE164(number, countryCode, formatNumber);
    }
    TELEPHONY_LOGE("callManagerServicePtr_ is nullptr!");
    return TELEPHONY_ERR_LOCAL_PTR_NULL;
}

int32_t CallManagerConnect::GetMainCallId(int32_t callId, int32_t &mainCallId)
{
    if (callManagerServicePtr_ != nullptr) {
        return callManagerServicePtr_->GetMainCallId(callId, mainCallId);
    }
    TELEPHONY_LOGE("callManagerServicePtr_ is nullptr!");
    return TELEPHONY_ERR_LOCAL_PTR_NULL;
}

int32_t CallManagerConnect::GetSubCallIdList(int32_t callId, std::vector<std::u16string> &callIdList)
{
    if (callManagerServicePtr_ != nullptr) {
        return callManagerServicePtr_->GetSubCallIdList(callId, callIdList);
    }
    callIdList.clear();
    return TELEPHONY_ERR_LOCAL_PTR_NULL;
}

int32_t CallManagerConnect::GetCallIdListForConference(int32_t callId, std::vector<std::u16string> &callIdList)
{
    if (callManagerServicePtr_ != nullptr) {
        return callManagerServicePtr_->GetCallIdListForConference(callId, callIdList);
    }
    callIdList.clear();
    return TELEPHONY_ERR_LOCAL_PTR_NULL;
}

int32_t CallManagerConnect::ControlCamera(std::u16string cameraId)
{
    if (callManagerServicePtr_ != nullptr) {
        return callManagerServicePtr_->ControlCamera(cameraId);
    }
    TELEPHONY_LOGE("callManagerServicePtr_ is nullptr!");
    return TELEPHONY_ERR_LOCAL_PTR_NULL;
}

int32_t CallManagerConnect::SetAudioDevice(const AudioDevice &audioDevice)
{
    if (callManagerServicePtr_ == nullptr) {
        TELEPHONY_LOGE("callManagerServicePtr_ is nullptr!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    return callManagerServicePtr_->SetAudioDevice(audioDevice);
}

int32_t CallManagerConnect::SetPreviewWindow(VideoWindow &window)
{
    if (callManagerServicePtr_ != nullptr) {
        return callManagerServicePtr_->SetPreviewWindow(window);
    }
    TELEPHONY_LOGE("callManagerServicePtr_ is nullptr!");
    return TELEPHONY_ERR_LOCAL_PTR_NULL;
}

int32_t CallManagerConnect::SetDisplayWindow(VideoWindow &window)
{
    if (callManagerServicePtr_ != nullptr) {
        return callManagerServicePtr_->SetDisplayWindow(window);
    }
    TELEPHONY_LOGE("callManagerServicePtr_ is nullptr!");
    return TELEPHONY_ERR_LOCAL_PTR_NULL;
}

int32_t CallManagerConnect::SetCameraZoom(float zoomRatio)
{
    if (callManagerServicePtr_ != nullptr) {
        return callManagerServicePtr_->SetCameraZoom(zoomRatio);
    }
    TELEPHONY_LOGE("callManagerServicePtr_ is nullptr!");
    return TELEPHONY_ERR_LOCAL_PTR_NULL;
}

int32_t CallManagerConnect::SetPausePicture(std::u16string path)
{
    if (callManagerServicePtr_ != nullptr) {
        return callManagerServicePtr_->SetPausePicture(path);
    }
    TELEPHONY_LOGE("callManagerServicePtr_ is nullptr!");
    return TELEPHONY_ERR_LOCAL_PTR_NULL;
}

int32_t CallManagerConnect::SetDeviceDirection(int32_t rotation)
{
    if (callManagerServicePtr_ != nullptr) {
        return callManagerServicePtr_->SetDeviceDirection(rotation);
    }
    TELEPHONY_LOGE("callManagerServicePtr_ is nullptr!");
    return TELEPHONY_ERR_LOCAL_PTR_NULL;
}

int32_t CallManagerConnect::EnableImsSwitch(int32_t slotId)
{
    if (callManagerServicePtr_ != nullptr) {
        return callManagerServicePtr_->EnableImsSwitch(slotId);
    }
    TELEPHONY_LOGE("callManagerServicePtr_ is nullptr!");
    return TELEPHONY_ERR_LOCAL_PTR_NULL;
}

int32_t CallManagerConnect::DisableImsSwitch(int32_t slotId)
{
    if (callManagerServicePtr_ != nullptr) {
        return callManagerServicePtr_->DisableImsSwitch(slotId);
    }
    TELEPHONY_LOGE("callManagerServicePtr_ is nullptr!");
    return TELEPHONY_ERR_LOCAL_PTR_NULL;
}

int32_t CallManagerConnect::IsImsSwitchEnabled(int32_t slotId)
{
    if (callManagerServicePtr_ != nullptr) {
        bool enabled;
        return callManagerServicePtr_->IsImsSwitchEnabled(slotId, enabled);
    }
    TELEPHONY_LOGE("callManagerServicePtr_ is nullptr!");
    return TELEPHONY_ERR_LOCAL_PTR_NULL;
}

int32_t CallManagerConnect::SetMuted(bool isMuted) const
{
    if (callManagerServicePtr_ != nullptr) {
        return callManagerServicePtr_->SetMuted(isMuted);
    }
    TELEPHONY_LOGE("callManagerServicePtr_ is nullptr!");
    return TELEPHONY_ERR_LOCAL_PTR_NULL;
}

int32_t CallManagerConnect::RegisterCallBack()
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

int32_t CallManagerConnect::ConnectService()
{
    Utils::UniqueWriteGuard<Utils::RWLock> guard(rwClientLock_);
    if (callManagerServicePtr_ != nullptr) {
        return TELEPHONY_SUCCESS;
    }
    sptr<ISystemAbilityManager> managerPtr = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
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

void CallManagerConnect::DisconnectService()
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
} // namespace Telephony
} // namespace OHOS