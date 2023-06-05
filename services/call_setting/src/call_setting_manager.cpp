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

#include "call_setting_manager.h"

#include "call_manager_errors.h"
#include "telephony_log_wrapper.h"

#include "call_number_utils.h"

namespace OHOS {
namespace Telephony {
CallSettingManager::CallSettingManager()
    : cellularCallConnectionPtr_(DelayedSingleton<CellularCallConnection>::GetInstance())
{}

CallSettingManager::~CallSettingManager() {}

int32_t CallSettingManager::GetCallWaiting(int32_t slotId)
{
    int32_t ret = CallWaitingPolicy(slotId);
    if (ret != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("Invalid data!");
        return ret;
    }
    if (cellularCallConnectionPtr_ == nullptr) {
        TELEPHONY_LOGE("cellularCallConnectionPtr_ is nullptr!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    return cellularCallConnectionPtr_->GetCallWaiting(slotId);
}

int32_t CallSettingManager::SetCallWaiting(int32_t slotId, bool activate)
{
    int32_t ret = CallWaitingPolicy(slotId);
    if (ret != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("Invalid data!");
        return ret;
    }
    if (cellularCallConnectionPtr_ == nullptr) {
        TELEPHONY_LOGE("cellularCallConnectionPtr_ is nullptr!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    return cellularCallConnectionPtr_->SetCallWaiting(activate, slotId);
}

int32_t CallSettingManager::GetCallRestriction(int32_t slotId, CallRestrictionType type)
{
    int32_t ret = GetCallRestrictionPolicy(slotId, type);
    if (ret != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("Invalid data!");
        return ret;
    }
    if (cellularCallConnectionPtr_ == nullptr) {
        TELEPHONY_LOGE("cellularCallConnectionPtr_ is nullptr!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    return cellularCallConnectionPtr_->GetCallRestriction(type, slotId);
}

int32_t CallSettingManager::SetCallRestriction(int32_t slotId, CallRestrictionInfo &info)
{
    TELEPHONY_LOGI(
        "SetCallRestriction: slotId = %{public}d, mode = %{public}d, type = %{public}d",
        slotId, info.mode, info.fac);
    int32_t ret = SetCallRestrictionPolicy(slotId, info);
    if (ret != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("Invalid data!");
        return ret;
    }
    if (cellularCallConnectionPtr_ == nullptr) {
        TELEPHONY_LOGE("cellularCallConnectionPtr_ is nullptr!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    return cellularCallConnectionPtr_->SetCallRestriction(info, slotId);
}

int32_t CallSettingManager::GetCallTransferInfo(int32_t slotId, CallTransferType type)
{
    int32_t ret = GetCallTransferInfoPolicy(slotId, type);
    if (ret != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("Invalid data!");
        return ret;
    }
    if (cellularCallConnectionPtr_ == nullptr) {
        TELEPHONY_LOGE("cellularCallConnectionPtr_ is nullptr!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    return cellularCallConnectionPtr_->GetCallTransferInfo(type, slotId);
}

int32_t CallSettingManager::SetCallTransferInfo(int32_t slotId, CallTransferInfo &info)
{
    TELEPHONY_LOGI(
        "SetCallTransferInfo: slotId = %{public}d, settingType = %{public}d, type = %{public}d, transferNum = "
        "%{public}s",
        slotId, info.settingType, info.type, info.transferNum);
    int32_t ret = SetCallTransferInfoPolicy(slotId, info);
    if (ret != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("Invalid data!");
        return ret;
    }
    if (cellularCallConnectionPtr_ == nullptr) {
        TELEPHONY_LOGE("cellularCallConnectionPtr_ is nullptr!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    return cellularCallConnectionPtr_->SetCallTransferInfo(info, slotId);
}

int32_t CallSettingManager::CanSetCallTransferTime(int32_t slotId, bool &result)
{
    TELEPHONY_LOGI("[slot%{public}d] entry", slotId);
    if (cellularCallConnectionPtr_ == nullptr) {
        TELEPHONY_LOGE("[slot%{public}d] cellularCallConnectionPtr_ is nullptr!", slotId);
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    return cellularCallConnectionPtr_->CanSetCallTransferTime(slotId, result);
}

int32_t CallSettingManager::SetCallPreferenceMode(int32_t slotId, int32_t mode)
{
    int32_t preferenceMode = IMS_PS_VOICE_PREFERRED;
    if (mode != CS_VOICE_ONLY && mode != CS_VOICE_PREFERRED && mode != IMS_PS_VOICE_PREFERRED &&
        mode != IMS_PS_VOICE_ONLY) {
        preferenceMode = CS_VOICE_ONLY;
    }
    return cellularCallConnectionPtr_->SetCallPreferenceMode(slotId, preferenceMode);
}

int32_t CallSettingManager::GetImsConfig(int32_t slotId, ImsConfigItem item)
{
    return cellularCallConnectionPtr_->GetImsConfig(item, slotId);
}

int32_t CallSettingManager::SetImsConfig(int32_t slotId, ImsConfigItem item, std::u16string &value)
{
    if (item == ITEM_VIDEO_QUALITY || item == ITEM_IMS_SWITCH_STATUS) {
        int32_t flag = CONFIG_FAILED;
        bool succ = StrToInt(Str16ToStr8(value), flag);
        if (succ) {
            return cellularCallConnectionPtr_->SetImsConfig(item, flag, slotId);
        }
        return TELEPHONY_ERR_STRTOINT_FAIL;
    }
    return cellularCallConnectionPtr_->SetImsConfig(item, Str16ToStr8(value), slotId);
}

int32_t CallSettingManager::GetImsFeatureValue(int32_t slotId, FeatureType type)
{
    if (cellularCallConnectionPtr_ == nullptr) {
        TELEPHONY_LOGE("cellularCallConnectionPtr_ is nullptr!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    return cellularCallConnectionPtr_->GetImsFeatureValue(type, slotId);
}

int32_t CallSettingManager::SetImsFeatureValue(int32_t slotId, FeatureType type, int32_t value)
{
    if (cellularCallConnectionPtr_ == nullptr) {
        TELEPHONY_LOGE("cellularCallConnectionPtr_ is nullptr!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    return cellularCallConnectionPtr_->SetImsFeatureValue(type, value, slotId);
}

int32_t CallSettingManager::EnableImsSwitch(int32_t slotId)
{
    if (cellularCallConnectionPtr_ == nullptr) {
        TELEPHONY_LOGE("cellularCallConnectionPtr_ is nullptr!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    return cellularCallConnectionPtr_->SetImsSwitchStatus(slotId, true);
}

int32_t CallSettingManager::DisableImsSwitch(int32_t slotId)
{
    if (cellularCallConnectionPtr_ == nullptr) {
        TELEPHONY_LOGE("cellularCallConnectionPtr_ is nullptr!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    return cellularCallConnectionPtr_->SetImsSwitchStatus(slotId, false);
}

int32_t CallSettingManager::IsImsSwitchEnabled(int32_t slotId, bool &enabled)
{
    if (cellularCallConnectionPtr_ == nullptr) {
        TELEPHONY_LOGE("cellularCallConnectionPtr_ is nullptr!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    return cellularCallConnectionPtr_->GetImsSwitchStatus(slotId, enabled);
}

int32_t CallSettingManager::SetVoNRState(int32_t slotId, int32_t state)
{
    if (cellularCallConnectionPtr_ == nullptr) {
        TELEPHONY_LOGE("cellularCallConnectionPtr_ is nullptr!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    return cellularCallConnectionPtr_->SetVoNRState(slotId, state);
}

int32_t CallSettingManager::GetVoNRState(int32_t slotId, int32_t &state)
{
    if (cellularCallConnectionPtr_ == nullptr) {
        TELEPHONY_LOGE("cellularCallConnectionPtr_ is nullptr!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    return cellularCallConnectionPtr_->GetVoNRState(slotId, state);
}

int32_t CallSettingManager::CloseUnFinishedUssd(int32_t slotId)
{
    int32_t ret = CloseUnFinishedUssdPolicy(slotId);
    if (ret != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("Invalid data!");
        return ret;
    }
    if (cellularCallConnectionPtr_ == nullptr) {
        TELEPHONY_LOGE("cellularCallConnectionPtr_ is nullptr!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    return cellularCallConnectionPtr_->CloseUnFinishedUssd(slotId);
}

int32_t CallSettingManager::CallWaitingPolicy(int32_t slotId)
{
    if (!DelayedSingleton<CallNumberUtils>::GetInstance()->IsValidSlotId(slotId)) {
        TELEPHONY_LOGE("invalid slotId!");
        return CALL_ERR_INVALID_SLOT_ID;
    }
    return TELEPHONY_SUCCESS;
}

int32_t CallSettingManager::GetCallRestrictionPolicy(int32_t slotId, CallRestrictionType type)
{
    if (!DelayedSingleton<CallNumberUtils>::GetInstance()->IsValidSlotId(slotId)) {
        TELEPHONY_LOGE("invalid slotId!");
        return CALL_ERR_INVALID_SLOT_ID;
    }
    if (type < CallRestrictionType::RESTRICTION_TYPE_ALL_INCOMING ||
        type > CallRestrictionType::RESTRICTION_TYPE_INCOMING_SERVICES) {
        TELEPHONY_LOGE("invalid restriction type!");
        return CALL_ERR_INVALID_RESTRICTION_TYPE;
    }
    return TELEPHONY_SUCCESS;
}

int32_t CallSettingManager::SetCallRestrictionPolicy(int32_t slotId, CallRestrictionInfo &info)
{
    if (!DelayedSingleton<CallNumberUtils>::GetInstance()->IsValidSlotId(slotId)) {
        TELEPHONY_LOGE("invalid slotId!");
        return CALL_ERR_INVALID_SLOT_ID;
    }
    if (info.fac < CallRestrictionType::RESTRICTION_TYPE_ALL_INCOMING ||
        info.fac > CallRestrictionType::RESTRICTION_TYPE_INCOMING_SERVICES) {
        TELEPHONY_LOGE("invalid restriction type!");
        return CALL_ERR_INVALID_RESTRICTION_TYPE;
    }
    if (info.mode != CallRestrictionMode::RESTRICTION_MODE_DEACTIVATION &&
        info.mode != CallRestrictionMode::RESTRICTION_MODE_ACTIVATION) {
        TELEPHONY_LOGE("invalid restriction mode!");
        return CALL_ERR_INVALID_RESTRICTION_MODE;
    }
    return TELEPHONY_SUCCESS;
}

int32_t CallSettingManager::GetCallTransferInfoPolicy(int32_t slotId, CallTransferType type)
{
    if (!DelayedSingleton<CallNumberUtils>::GetInstance()->IsValidSlotId(slotId)) {
        TELEPHONY_LOGE("invalid slotId!");
        return CALL_ERR_INVALID_SLOT_ID;
    }
    if (type < CallTransferType::TRANSFER_TYPE_UNCONDITIONAL ||
        type > CallTransferType::TRANSFER_TYPE_NOT_REACHABLE) {
        TELEPHONY_LOGE("invalid transfer type!");
        return CALL_ERR_INVALID_TRANSFER_TYPE;
    }
    return TELEPHONY_SUCCESS;
}

int32_t CallSettingManager::SetCallTransferInfoPolicy(int32_t slotId, CallTransferInfo &info)
{
    if (!DelayedSingleton<CallNumberUtils>::GetInstance()->IsValidSlotId(slotId)) {
        TELEPHONY_LOGE("invalid slotId!");
        return CALL_ERR_INVALID_SLOT_ID;
    }
    if (info.type < CallTransferType::TRANSFER_TYPE_UNCONDITIONAL ||
        info.type > CallTransferType::TRANSFER_TYPE_NOT_REACHABLE) {
        TELEPHONY_LOGE("invalid transfer type!");
        return CALL_ERR_INVALID_TRANSFER_TYPE;
    }
    if (info.settingType != CallTransferSettingType::CALL_TRANSFER_DISABLE &&
        info.settingType != CallTransferSettingType::CALL_TRANSFER_ENABLE &&
        info.settingType != CallTransferSettingType::CALL_TRANSFER_REGISTRATION &&
        info.settingType != CallTransferSettingType::CALL_TRANSFER_ERASURE) {
        TELEPHONY_LOGE("invalid transfer setting type!");
        return CALL_ERR_INVALID_TRANSFER_SETTING_TYPE;
    }
    // if haven't transfer the time from app. The default time was 0.
    if (std::min(info.startHour, info.endHour) < MIN_HOUR || std::max(info.startHour, info.endHour) > MAX_HOUR ||
        std::min(info.startMinute, info.startMinute) < MIN_MINUTE ||
        std::max(info.startMinute, info.endMinute) > MAX_MINUTE) {
        TELEPHONY_LOGE("invalid transfer time!");
        return CALL_ERR_INVALID_TRANSFER_TIME;
    }
    if (strlen(info.transferNum) > kMaxNumberLen) {
        TELEPHONY_LOGE("Number out of limit!");
        return CALL_ERR_NUMBER_OUT_OF_RANGE;
    }
    return TELEPHONY_SUCCESS;
}

int32_t CallSettingManager::CloseUnFinishedUssdPolicy(int32_t slotId)
{
    if (!DelayedSingleton<CallNumberUtils>::GetInstance()->IsValidSlotId(slotId)) {
        TELEPHONY_LOGE("invalid slotId!");
        return CALL_ERR_INVALID_SLOT_ID;
    }
    return TELEPHONY_SUCCESS;
}
} // namespace Telephony
} // namespace OHOS
