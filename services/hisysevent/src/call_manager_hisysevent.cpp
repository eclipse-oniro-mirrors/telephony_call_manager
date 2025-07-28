/*
 * Copyright (C) 2024 Huawei Device Co., Ltd.
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

#include "call_manager_hisysevent.h"

#include "app_mgr_interface.h"
#include "bundle_mgr_interface.h"
#include "bundle_mgr_proxy.h"
#include "iservice_registry.h"

#include "call_control_manager.h"
#include "call_manager_errors.h"
#include "telephony_errors.h"
#include "telephony_log_wrapper.h"
#include "voip_call.h"

namespace OHOS {
namespace Telephony {
// EVENT
static constexpr const char *CALL_DIAL_FAILED_EVENT = "CALL_DIAL_FAILED";
static constexpr const char *CALL_INCOMING_FAILED_EVENT = "CALL_INCOMING_FAILED";
static constexpr const char *CALL_ANSWER_FAILED_EVENT = "CALL_ANSWER_FAILED";
static constexpr const char *CALL_HANGUP_FAILED_EVENT = "CALL_HANGUP_FAILED";
static constexpr const char *INCOMING_CALL_EVENT = "INCOMING_CALL";
static constexpr const char *CALL_STATE_CHANGED_EVENT = "CALL_STATE";
static constexpr const char *CALL_INCOMING_NUM_IDENTITY_EVENT = "CALL_INCOMING_NUM_IDENTITY";
static  const std::string VOIP_CALL_STATISTICAL = "VOIP_CALL_STATISTICS";

// KEY
static constexpr const char *MODULE_NAME_KEY = "MODULE";
static constexpr const char *SLOT_ID_KEY = "SLOT_ID";
static constexpr const char *INDEX_ID_KEY = "INDEX_ID";
static constexpr const char *STATE_KEY = "STATE";
static constexpr const char *CALL_ID_KEY = "CALL_ID";
static constexpr const char *CALL_TYPE_KEY = "CALL_TYPE";
static constexpr const char *VIDEO_STATE_KEY = "VIDEO_STATE";
static constexpr const char *ERROR_TYPE_KEY = "ERROR_TYPE";
static constexpr const char *ERROR_MSG_KEY = "ERROR_MSG";
static constexpr const char *MARK_TYPE_KEY = "MARK_TYPE";
static constexpr const char *IS_BLOCK = "IS_BLOCK";
static constexpr const char *BLOCK_REASON = "BLOCK_REASON";
static  const std::string BUNDLE_NAME_KEY = "BUNDLE_NAME";
static  const std::string STATISTICAL_FIELD_KEY = "STATISTICAL_FIELD";
static  const std::string APP_INDEX_KEY = "APP_INDEX";

// VALUE
static constexpr const char *CALL_MANAGER_MODULE = "CALL_MANAGER";
static const int32_t CS_CALL_TYPE = 0;
static const int32_t IMS_CALL_TYPE = 1;
static const int32_t SATELLITE_CALL_TYPE = 5;
static const int32_t VOICE_TYPE = 0;
static const int32_t VIDEO_TYPE = 1;
using namespace OHOS::AppExecFwk;
void CallManagerHisysevent::WriteCallStateBehaviorEvent(const int32_t slotId, const int32_t state, const int32_t index)
{
    HiWriteBehaviorEvent(CALL_STATE_CHANGED_EVENT, SLOT_ID_KEY, slotId, STATE_KEY, state, INDEX_ID_KEY, index);
}

void CallManagerHisysevent::WriteIncomingCallBehaviorEvent(const int32_t slotId, int32_t callType, int32_t callMode)
{
    int32_t type = 0;
    if (callType == IMS_CALL_TYPE && callMode == VOICE_TYPE) {
        type = static_cast<int32_t>(IncomingCallType::IMS_VOICE_INCOMING);
    } else if (callType == IMS_CALL_TYPE && callMode == VIDEO_TYPE) {
        type = static_cast<int32_t>(IncomingCallType::IMS_VIDEO_INCOMING);
    } else if ((callType == CS_CALL_TYPE || callType == SATELLITE_CALL_TYPE) && callMode == VOICE_TYPE) {
        type = static_cast<int32_t>(IncomingCallType::CS_VOICE_INCOMING);
    } else {
        TELEPHONY_LOGE("WriteIncomingCallBehaviorEvent call incomming arges is out of range!");
        return;
    }
    HiWriteBehaviorEvent(INCOMING_CALL_EVENT, SLOT_ID_KEY, slotId, CALL_TYPE_KEY, type);
}

void CallManagerHisysevent::WriteIncomingNumIdentityBehaviorEvent(const int32_t markType, const bool isBlock,
    const int32_t blockReason)
{
    TELEPHONY_LOGI("WriteIncomingNumIdentityBehaviorEvent markType: %{public}d, isBlock: %{public}d, "
        "blockReason: %{public}d", markType, isBlock, blockReason);
    HiWriteBehaviorEventPhoneUE(CALL_INCOMING_NUM_IDENTITY_EVENT, PNAMEID_KEY, "callmanager", PVERSIONID_KEY, "",
        MARK_TYPE_KEY, markType, IS_BLOCK, isBlock, BLOCK_REASON, blockReason);
}

void CallManagerHisysevent::WriteIncomingCallFaultEvent(const int32_t slotId, const int32_t callType,
    const int32_t videoState, const int32_t errCode, const std::string &desc)
{
    CallErrorCode value = CallErrorCode::CALL_ERROR_NONE;
    if (ErrorCodeConversion(errCode, value)) {
        HiWriteFaultEvent(CALL_INCOMING_FAILED_EVENT, MODULE_NAME_KEY, CALL_MANAGER_MODULE, SLOT_ID_KEY, slotId,
            CALL_TYPE_KEY, callType, VIDEO_STATE_KEY, videoState, ERROR_TYPE_KEY, static_cast<int32_t>(value),
            ERROR_MSG_KEY, desc);
    } else {
        HiWriteFaultEvent(CALL_INCOMING_FAILED_EVENT, MODULE_NAME_KEY, CALL_MANAGER_MODULE, SLOT_ID_KEY, slotId,
            CALL_TYPE_KEY, callType, VIDEO_STATE_KEY, videoState, ERROR_TYPE_KEY, errCode, ERROR_MSG_KEY, desc);
    }
}

void CallManagerHisysevent::WriteDialCallFaultEvent(const int32_t slotId, const int32_t callType,
    const int32_t videoState, const int32_t errCode, const std::string &desc)
{
    CallErrorCode value = CallErrorCode::CALL_ERROR_NONE;
    if (ErrorCodeConversion(errCode, value)) {
        HiWriteFaultEvent(CALL_DIAL_FAILED_EVENT, MODULE_NAME_KEY, CALL_MANAGER_MODULE, SLOT_ID_KEY, slotId,
            CALL_TYPE_KEY, callType, VIDEO_STATE_KEY, videoState, ERROR_TYPE_KEY, static_cast<int32_t>(value),
            ERROR_MSG_KEY, desc);
    } else {
        HiWriteFaultEvent(CALL_DIAL_FAILED_EVENT, MODULE_NAME_KEY, CALL_MANAGER_MODULE, SLOT_ID_KEY, slotId,
            CALL_TYPE_KEY, callType, VIDEO_STATE_KEY, videoState, ERROR_TYPE_KEY, errCode, ERROR_MSG_KEY, desc);
    }
}

void CallManagerHisysevent::WriteAnswerCallFaultEvent(const int32_t slotId, const int32_t callId,
    const int32_t videoState, const int32_t errCode, const std::string &desc)
{
    CallErrorCode value = CallErrorCode::CALL_ERROR_NONE;
    if (ErrorCodeConversion(errCode, value)) {
        HiWriteFaultEvent(CALL_ANSWER_FAILED_EVENT, MODULE_NAME_KEY, CALL_MANAGER_MODULE, SLOT_ID_KEY, slotId,
            CALL_ID_KEY, callId, VIDEO_STATE_KEY, videoState, ERROR_TYPE_KEY, static_cast<int32_t>(value),
            ERROR_MSG_KEY, desc);
    } else {
        HiWriteFaultEvent(CALL_ANSWER_FAILED_EVENT, MODULE_NAME_KEY, CALL_MANAGER_MODULE, SLOT_ID_KEY, slotId,
            CALL_ID_KEY, callId, VIDEO_STATE_KEY, videoState, ERROR_TYPE_KEY, errCode, ERROR_MSG_KEY, desc);
    }
}

void CallManagerHisysevent::WriteHangUpFaultEvent(
    const int32_t slotId, const int32_t callId, const int32_t errCode, const std::string &desc)
{
    CallErrorCode value = CallErrorCode::CALL_ERROR_NONE;
    if (ErrorCodeConversion(errCode, value)) {
        HiWriteFaultEvent(CALL_HANGUP_FAILED_EVENT, MODULE_NAME_KEY, CALL_MANAGER_MODULE, SLOT_ID_KEY, slotId,
            CALL_ID_KEY, callId, ERROR_TYPE_KEY, static_cast<int32_t>(value), ERROR_MSG_KEY, desc);
    } else {
        HiWriteFaultEvent(CALL_HANGUP_FAILED_EVENT, MODULE_NAME_KEY, CALL_MANAGER_MODULE, SLOT_ID_KEY, slotId,
            CALL_ID_KEY, callId, ERROR_TYPE_KEY, errCode, ERROR_MSG_KEY, desc);
    }
}

void CallManagerHisysevent::GetErrorDescription(const int32_t errCode, std::string &errordesc)
{
    switch (errCode) {
        case CALL_ERR_PHONE_NUMBER_EMPTY:
        case CALL_ERR_NUMBER_OUT_OF_RANGE:
            errordesc = "NumberLegalityCheck failed";
            break;
        case CALL_ERR_UNKNOW_DIAL_TYPE:
        case CALL_ERR_INVALID_SLOT_ID:
        case CALL_ERR_UNKNOW_CALL_TYPE:
        case CALL_ERR_INVALID_DIAL_SCENE:
        case CALL_ERR_INVALID_VIDEO_STATE:
            errordesc = "DialPolicy failed";
            break;
        case TELEPHONY_ERR_LOCAL_PTR_NULL:
            errordesc = errordesc = "CallRequestHandlerPtr_ or handler_ is nullptr";
            break;
        case CALL_ERR_SYSTEM_EVENT_HANDLE_FAILURE:
            errordesc = "Send HANDLER_DIAL_CALL_REQUEST event failed";
            break;
        default:
            break;
    }
}

int32_t CallManagerHisysevent::ErrorCodeConversion(const int32_t errCode, CallErrorCode &eventValue)
{
    if (CallDataErrorCodeConversion(errCode, eventValue) || CallInterfaceErrorCodeConversion(errCode, eventValue) ||
        TelephonyErrorCodeConversion(errCode, eventValue)) {
        TELEPHONY_LOGI("CallManagerHisysevent::ErrorCodeConversion in %{public}d out %{public}d", errCode, eventValue);
        return true;
    }
    return false;
}

int32_t CallManagerHisysevent::TelephonyErrorCodeConversion(const int32_t errCode, CallErrorCode &eventValue)
{
    switch (errCode) {
        case static_cast<int32_t>(TELEPHONY_ERR_LOCAL_PTR_NULL):
            eventValue = CallErrorCode::CALL_ERROR_CALL_LOCAL_PTR_NULL;
            break;
        case static_cast<int32_t>(TELEPHONY_ERR_ARGUMENT_INVALID):
            eventValue = CallErrorCode::CALL_ERROR_ARGUMENT_INVALID;
            break;
        case static_cast<int32_t>(TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL):
            eventValue = CallErrorCode::CALL_ERROR_IPC_CONNECT_STUB_FAIL;
            break;
        case static_cast<int32_t>(TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL):
            eventValue = CallErrorCode::CALL_ERROR_IPC_WRITE_DESCRIPTOR_TOKEN_FAIL;
            break;
        case static_cast<int32_t>(TELEPHONY_ERR_WRITE_DATA_FAIL):
            eventValue = CallErrorCode::CALL_ERROR_IPC_WRITE_DATA_FAIL;
            break;
        case static_cast<int32_t>(TELEPHONY_ERR_PERMISSION_ERR):
            eventValue = CallErrorCode::CALL_ERROR_PERMISSION_ERR;
            break;
        case static_cast<int32_t>(TELEPHONY_ERR_MEMSET_FAIL):
            eventValue = CallErrorCode::CALL_ERROR_MEMSET_FAIL;
            break;
        case static_cast<int32_t>(TELEPHONY_ERR_MEMCPY_FAIL):
            eventValue = CallErrorCode::CALL_ERROR_MEMCPY_FAIL;
            break;
        default:
            return false;
    }

    return true;
}

int32_t CallManagerHisysevent::CallDataErrorCodeConversion(const int32_t errCode, CallErrorCode &eventValue)
{
    switch (errCode) {
        case static_cast<int32_t>(CALL_ERR_INVALID_SLOT_ID):
            eventValue = CallErrorCode::CALL_ERROR_INVALID_SLOT_ID;
            break;
        case static_cast<int32_t>(CALL_ERR_INVALID_CALLID):
            eventValue = CallErrorCode::CALL_ERROR_INVALID_CALLID;
            break;
        case static_cast<int32_t>(CALL_ERR_PHONE_NUMBER_EMPTY):
            eventValue = CallErrorCode::CALL_ERROR_PHONE_NUMBER_EMPTY;
            break;
        case static_cast<int32_t>(CALL_ERR_NUMBER_OUT_OF_RANGE):
            eventValue = CallErrorCode::CALL_ERROR_NUMBER_OUT_OF_RANGE;
            break;
        case static_cast<int32_t>(CALL_ERR_UNSUPPORTED_NETWORK_TYPE):
            eventValue = CallErrorCode::CALL_ERROR_UNSUPPORTED_NETWORK_TYPE;
            break;
        case static_cast<int32_t>(CALL_ERR_INVALID_DIAL_SCENE):
            eventValue = CallErrorCode::CALL_ERROR_INVALID_DIAL_SCENE;
            break;
        case static_cast<int32_t>(CALL_ERR_INVALID_VIDEO_STATE):
            eventValue = CallErrorCode::CALL_ERROR_INVALID_VIDEO_STATE;
            break;
        case static_cast<int32_t>(CALL_ERR_UNKNOW_DIAL_TYPE):
            eventValue = CallErrorCode::CALL_ERROR_UNKNOW_DIAL_TYPE;
            break;
        case static_cast<int32_t>(CALL_ERR_UNKNOW_CALL_TYPE):
            eventValue = CallErrorCode::CALL_ERROR_UNKNOW_CALL_TYPE;
            break;
        case static_cast<int32_t>(CALL_ERR_CALL_OBJECT_IS_NULL):
            eventValue = CallErrorCode::CALL_ERROR_CALL_OBJECT_IS_NULL;
            break;
        default:
            return false;
    }
    return true;
}

int32_t CallManagerHisysevent::CallInterfaceErrorCodeConversion(const int32_t errCode, CallErrorCode &eventValue)
{
    switch (errCode) {
        case static_cast<int32_t>(CALL_ERR_DIAL_IS_BUSY):
            eventValue = CallErrorCode::CALL_ERROR_DIAL_IS_BUSY;
            break;
        case static_cast<int32_t>(CALL_ERR_ILLEGAL_CALL_OPERATION):
            eventValue = CallErrorCode::CALL_ERROR_ILLEGAL_CALL_OPERATION;
            break;
        case static_cast<int32_t>(CALL_ERR_PHONE_CALLSTATE_NOTIFY_FAILED):
            eventValue = CallErrorCode::CALL_ERROR_PHONE_CALLSTATE_NOTIFY_FAILED;
            break;
        case static_cast<int32_t>(CALL_ERR_SYSTEM_EVENT_HANDLE_FAILURE):
            eventValue = CallErrorCode::CALL_ERROR_SYSTEM_EVENT_HANDLE_FAILURE;
            break;
        case static_cast<int32_t>(CALL_ERR_CALL_COUNTS_EXCEED_LIMIT):
            eventValue = CallErrorCode::CALL_ERROR_CALL_COUNTS_EXCEED_LIMIT;
            break;
        case static_cast<int32_t>(CALL_ERR_GET_RADIO_STATE_FAILED):
            eventValue = CallErrorCode::CALL_ERROR_GET_RADIO_STATE_FAILED;
            break;
        default:
            return false;
    }

    return true;
}

void CallManagerHisysevent::SetDialStartTime()
{
    dialStartTime_ =
        std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch())
            .count();
}

void CallManagerHisysevent::SetIncomingStartTime()
{
    incomingStartTime_ =
        std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch())
            .count();
}

void CallManagerHisysevent::SetAnswerStartTime()
{
    answerStartTime_ =
        std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch())
            .count();
}

void CallManagerHisysevent::JudgingDialTimeOut(const int32_t slotId, const int32_t callType, const int32_t videoState)
{
    int64_t dialEndTime =
        std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch())
            .count();
    if (dialEndTime - dialStartTime_ > NORMAL_DIAL_TIME) {
        WriteDialCallFaultEvent(slotId, callType, videoState,
            static_cast<int32_t>(CallErrorCode::CALL_ERROR_DIAL_TIME_OUT),
            "dial time out " + std::to_string(dialEndTime - dialStartTime_));
    }
}

void CallManagerHisysevent::JudgingIncomingTimeOut(
    const int32_t slotId, const int32_t callType, const int32_t videoState)
{
    int64_t incomingEndTime =
        std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch())
            .count();
    if (incomingEndTime - incomingStartTime_ > NORMAL_INCOMING_TIME) {
        WriteIncomingCallFaultEvent(slotId, callType, videoState,
            static_cast<int32_t>(CallErrorCode::CALL_ERROR_INCOMING_TIME_OUT),
            "incoming time out " + std::to_string(incomingEndTime - incomingStartTime_));
    }
}

void CallManagerHisysevent::JudgingAnswerTimeOut(const int32_t slotId, const int32_t callId, const int32_t videoState)
{
    int64_t answerEndTime =
        std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch())
            .count();
    if (answerEndTime - answerStartTime_ > NORMAL_ANSWER_TIME) {
        WriteAnswerCallFaultEvent(slotId, callId, videoState,
            static_cast<int32_t>(CallErrorCode::CALL_ERROR_ANSWER_TIME_OUT),
            "answer time out " + std::to_string(answerEndTime - answerStartTime_));
    }
}

void CallManagerHisysevent::WriteVoipCallStatisticalEvent(const std::string &voipCallId, const std::string &bundleName,
    const int32_t &uid, const std::string statisticalField)
{
    ffrt::submit([voipCallId, bundleName, uid, statisticalField]() {
        int32_t appIndex = -1;
        GetAppIndexByBundleName(bundleName, uid, appIndex);
        HiSysEventWrite(DOMAIN_NAME, VOIP_CALL_STATISTICAL, EventType::FAULT, CALL_ID_KEY, voipCallId,
            BUNDLE_NAME_KEY, bundleName, STATISTICAL_FIELD_KEY, statisticalField, APP_INDEX_KEY, appIndex);
    });
}
void CallManagerHisysevent::WriteVoipCallStatisticalEvent(const int32_t &callId, const std::string statisticalField)
{
    auto call = CallControlManager::GetOneCallObject(callId);
    if (call == nullptr || call->GetCallType() != CallType::TYPE_VOIP) {
        return;
    }

    sptr<VoIPCall> voipCall = reinterpret_cast<VoIPCall *>(call.GetRefPtr());
    WriteVoipCallStatisticalEvent(voipCall->GetVoipCallId(), voipCall->GetVoipBundleName(), voipCall->GetVoipUid(),
        statisticalField);
}
void CallManagerHisysevent::GetAppIndexByBundleName(std::string bundleName, int32_t uid, int32_t &appIndex)
{
    sptr<ISystemAbilityManager> systemAbilityManager =
        SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (systemAbilityManager == nullptr) {
        TELEPHONY_LOGE("system ability manager is nullptr");
        return;
    }
    sptr<IRemoteObject> remoteObject = systemAbilityManager->GetSystemAbility(BUNDLE_MGR_SERVICE_SYS_ABILITY_ID);
    if (remoteObject == nullptr) {
        TELEPHONY_LOGE("Get system ability failed");
        return;
    }
    sptr<IBundleMgr> bundleMgr = iface_cast<IBundleMgr>(remoteObject);
    bundleMgr->GetNameAndIndexForUid(uid, bundleName, appIndex);
}

} // namespace Telephony
} // namespace OHOS
