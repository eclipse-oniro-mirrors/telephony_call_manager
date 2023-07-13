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

#include "bluetooth_call_policy.h"

#include "call_manager_errors.h"
#include "telephony_log_wrapper.h"

namespace OHOS {
namespace Telephony {
BluetoothCallPolicy::BluetoothCallPolicy() {}

BluetoothCallPolicy::~BluetoothCallPolicy() {}

int32_t BluetoothCallPolicy::AnswerCallPolicy(int32_t &callId)
{
    if (IsCallExist(TelCallState::CALL_STATUS_INCOMING, callId)) {
        TELEPHONY_LOGI("incoming call is exist, callId:%{public}d", callId);
        return TELEPHONY_SUCCESS;
    }
    if (IsCallExist(TelCallState::CALL_STATUS_WAITING, callId)) {
        TELEPHONY_LOGI("waiting call is exist, callId:%{public}d", callId);
        return TELEPHONY_SUCCESS;
    }
    TELEPHONY_LOGW("AnswerCallPolicy failed");
    return CALL_ERR_ILLEGAL_CALL_OPERATION;
}

int32_t BluetoothCallPolicy::RejectCallPolicy(int32_t &callId)
{
    if (IsCallExist(TelCallState::CALL_STATUS_INCOMING, callId)) {
        TELEPHONY_LOGI("incoming call is exist, callId:%{public}d", callId);
        return TELEPHONY_SUCCESS;
    }
    if (IsCallExist(TelCallState::CALL_STATUS_WAITING, callId)) {
        TELEPHONY_LOGI("waiting call is exist, callId:%{public}d", callId);
        return TELEPHONY_SUCCESS;
    }
    TELEPHONY_LOGW("RejectCallPolicy failed");
    return CALL_ERR_ILLEGAL_CALL_OPERATION;
}

int32_t BluetoothCallPolicy::HoldCallPolicy(int32_t &callId)
{
    if (IsCallExist(TelCallState::CALL_STATUS_ACTIVE, callId)) {
        TELEPHONY_LOGI("active call is exist, callId:%{public}d", callId);
        return TELEPHONY_SUCCESS;
    }
    TELEPHONY_LOGW("HoldCallPolicy failed");
    return CALL_ERR_ILLEGAL_CALL_OPERATION;
}

int32_t BluetoothCallPolicy::UnHoldCallPolicy(int32_t &callId)
{
    if (IsCallExist(TelCallState::CALL_STATUS_HOLDING, callId)) {
        TELEPHONY_LOGI("holding call is exist, callId:%{public}d", callId);
        return TELEPHONY_SUCCESS;
    }
    TELEPHONY_LOGW("HoldCallPolicy failed");
    return CALL_ERR_ILLEGAL_CALL_OPERATION;
}

int32_t BluetoothCallPolicy::HangUpPolicy(int32_t &callId)
{
    if (IsCallExist(TelCallState::CALL_STATUS_INCOMING, callId)) {
        TELEPHONY_LOGI("incoming call is exist, callId:%{public}d", callId);
        return TELEPHONY_SUCCESS;
    }
    if (IsCallExist(TelCallState::CALL_STATUS_WAITING, callId)) {
        TELEPHONY_LOGI("waiting call is exist, callId:%{public}d", callId);
        return TELEPHONY_SUCCESS;
    }
    if (IsCallExist(TelCallState::CALL_STATUS_ACTIVE, callId)) {
        TELEPHONY_LOGI("active call is exist, callId:%{public}d", callId);
        return TELEPHONY_SUCCESS;
    }
    if (IsCallExist(TelCallState::CALL_STATUS_ALERTING, callId)) {
        TELEPHONY_LOGI("alerting call is exist, callId:%{public}d", callId);
        return TELEPHONY_SUCCESS;
    }
    if (IsCallExist(TelCallState::CALL_STATUS_DIALING, callId)) {
        TELEPHONY_LOGI("dialing call is exist, callId:%{public}d", callId);
        return TELEPHONY_SUCCESS;
    }
    if (IsCallExist(TelCallState::CALL_STATUS_HOLDING, callId)) {
        TELEPHONY_LOGI("holding call is exist, callId:%{public}d", callId);
        return TELEPHONY_SUCCESS;
    }
    TELEPHONY_LOGW("HangUpPolicy failed");
    return CALL_ERR_ILLEGAL_CALL_OPERATION;
}

int32_t BluetoothCallPolicy::SwitchCallPolicy(int32_t &callId)
{
    if (IsCallExist(TelCallState::CALL_STATUS_ACTIVE) &&
        IsCallExist(TelCallState::CALL_STATUS_HOLDING, callId)) {
        TELEPHONY_LOGI("active call and holding call are exist, callId:%{public}d", callId);
        return TELEPHONY_SUCCESS;
    }
    TELEPHONY_LOGW("SwitchCallPolicy failed");
    return CALL_ERR_ILLEGAL_CALL_OPERATION;
}

int32_t BluetoothCallPolicy::StartDtmfPolicy(int32_t &callId)
{
    if (IsCallExist(TelCallState::CALL_STATUS_ACTIVE, callId)) {
        TELEPHONY_LOGI("active call is exist, callId:%{public}d", callId);
        return TELEPHONY_SUCCESS;
    }
    if (IsCallExist(TelCallState::CALL_STATUS_ALERTING, callId)) {
        TELEPHONY_LOGI("alerting call is exist, callId:%{public}d", callId);
        return TELEPHONY_SUCCESS;
    }
    if (IsCallExist(TelCallState::CALL_STATUS_DIALING, callId)) {
        TELEPHONY_LOGI("dialing call is exist, callId:%{public}d", callId);
        return TELEPHONY_SUCCESS;
    }
    if (IsCallExist(TelCallState::CALL_STATUS_HOLDING, callId)) {
        TELEPHONY_LOGI("holding call is exist, callId:%{public}d", callId);
        return TELEPHONY_SUCCESS;
    }
    TELEPHONY_LOGW("StartDtmfPolicy failed");
    return CALL_ERR_ILLEGAL_CALL_OPERATION;
}

int32_t BluetoothCallPolicy::CombineConferencePolicy(int32_t &callId)
{
    if (IsCallExist(TelCallState::CALL_STATUS_ACTIVE, callId) &&
        IsCallExist(TelCallState::CALL_STATUS_HOLDING)) {
        TELEPHONY_LOGI("active call and holding call are exist, callId:%{public}d", callId);
        return TELEPHONY_SUCCESS;
    }
    TELEPHONY_LOGW("CombineConferencePolicy failed");
    return CALL_ERR_ILLEGAL_CALL_OPERATION;
}

int32_t BluetoothCallPolicy::SeparateConferencePolicy(int32_t &callId)
{
    if (IsConferenceCallExist(TelConferenceState::TEL_CONFERENCE_ACTIVE, callId) ||
        IsConferenceCallExist(TelConferenceState::TEL_CONFERENCE_HOLDING, callId)) {
        TELEPHONY_LOGI("conference call is exist, callId:%{public}d", callId);
        return TELEPHONY_SUCCESS;
    }
    TELEPHONY_LOGW("SeparateConferencePolicy failed");
    return CALL_ERR_ILLEGAL_CALL_OPERATION;
}

int32_t BluetoothCallPolicy::KickOutFromConferencePolicy(int32_t &callId)
{
    if (IsConferenceCallExist(TelConferenceState::TEL_CONFERENCE_ACTIVE, callId) ||
        IsConferenceCallExist(TelConferenceState::TEL_CONFERENCE_HOLDING, callId)) {
        TELEPHONY_LOGI("conference call is exist, callId:%{public}d", callId);
        return TELEPHONY_SUCCESS;
    }
    TELEPHONY_LOGW("KickOutFromConferencePolicy failed");
    return CALL_ERR_ILLEGAL_CALL_OPERATION;
}
} // namespace Telephony
} // namespace OHOS
