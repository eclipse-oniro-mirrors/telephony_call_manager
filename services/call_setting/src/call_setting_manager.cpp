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

#include "call_setting_manager.h"

#include "call_manager_errors.h"
#include "telephony_log_wrapper.h"

#include "call_number_utils.h"
#include "cellular_call_ipc_interface_proxy.h"

namespace OHOS {
namespace Telephony {
CallSettingManager::CallSettingManager() {}

CallSettingManager::~CallSettingManager() {}

int32_t CallSettingManager::GetCallWaiting(int32_t slotId)
{
    int32_t ret = CallWaitingPolicy(slotId);
    if (ret != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("Invalid data!");
        return ret;
    }
    return DelayedSingleton<CellularCallIpcInterfaceProxy>::GetInstance()->GetCallWaiting(slotId);
}

int32_t CallSettingManager::SetCallWaiting(int32_t slotId, bool activate)
{
    int32_t ret = CallWaitingPolicy(slotId);
    if (ret != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("Invalid data!");
        return ret;
    }
    return DelayedSingleton<CellularCallIpcInterfaceProxy>::GetInstance()->SetCallWaiting(activate, slotId);
}

int32_t CallSettingManager::GetCallRestriction(int32_t slotId, CallRestrictionType type)
{
    int32_t ret = GetCallRestrictionPolicy(slotId, type);
    if (ret != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("Invalid data!");
        return ret;
    }
    return DelayedSingleton<CellularCallIpcInterfaceProxy>::GetInstance()->GetCallRestriction(type, slotId);
}

int32_t CallSettingManager::SetCallRestriction(int32_t slotId, CallRestrictionInfo &info)
{
    int32_t ret = SetCallRestrictionPolicy(slotId, info);
    if (ret != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("Invalid data!");
        return ret;
    }
    return DelayedSingleton<CellularCallIpcInterfaceProxy>::GetInstance()->SetCallRestriction(info, slotId);
}

int32_t CallSettingManager::GetCallTransferInfo(int32_t slotId, CallTransferType type)
{
    int32_t ret = GetCallTransferInfoPolicy(slotId, type);
    if (ret != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("Invalid data!");
        return ret;
    }
    return DelayedSingleton<CellularCallIpcInterfaceProxy>::GetInstance()->GetCallTransferInfo(type, slotId);
}

int32_t CallSettingManager::SetCallTransferInfo(int32_t slotId, CallTransferInfo &info)
{
    int32_t ret = SetCallTransferInfoPolicy(slotId, info);
    if (ret != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("Invalid data!");
        return ret;
    }
    return DelayedSingleton<CellularCallIpcInterfaceProxy>::GetInstance()->SetCallTransferInfo(info, slotId);
}

int32_t CallSettingManager::SetCallPreferenceMode(int32_t slotId, int32_t mode)
{
    int32_t preferenceMode = 3;
    if (mode == 0) {
        preferenceMode = 1;
    }
    return DelayedSingleton<CellularCallIpcInterfaceProxy>::GetInstance()->SetCallPreferenceMode(
        slotId, preferenceMode);
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
    return TELEPHONY_SUCCESS;
}
} // namespace Telephony
} // namespace OHOS