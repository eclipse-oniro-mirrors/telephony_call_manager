/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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
#include "string_ex.h"
#include "singleton.h"
#include "call_manager_client.h"
#include "system_ability_definition.h"
#include "telephony_types.h"
#include "string_wrapper.h"
#include "bool_wrapper.h"
#include "ability_manager_client.h"
#include "telephony_errors.h"
#include "call_manager_errors.h"

#include "telephony_call_log.h"
#include "telephony_call_impl.h"

using namespace OHOS::Telephony;

namespace OHOS {
namespace Telephony {
    static int32_t ConvertCJErrCode(int32_t errCode)
    {
        LOGI("The original error code is displayed: %{public}d", errCode);
        switch (errCode) {
            case TELEPHONY_ERR_ARGUMENT_MISMATCH:
            case TELEPHONY_ERR_ARGUMENT_INVALID:
            case TELEPHONY_ERR_ARGUMENT_NULL:
            case TELEPHONY_ERR_SLOTID_INVALID:
                // 83000001
                return CJ_ERROR_TELEPHONY_ARGUMENT_ERROR;
            case TELEPHONY_ERR_DESCRIPTOR_MISMATCH:
            case TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL:
            case TELEPHONY_ERR_WRITE_DATA_FAIL:
            case TELEPHONY_ERR_WRITE_REPLY_FAIL:
            case TELEPHONY_ERR_READ_DATA_FAIL:
            case TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL:
            case TELEPHONY_ERR_REGISTER_CALLBACK_FAIL:
            case TELEPHONY_ERR_CALLBACK_ALREADY_REGISTERED:
            case TELEPHONY_ERR_UNINIT:
            case TELEPHONY_ERR_UNREGISTER_CALLBACK_FAIL:
                // 83000002
                return CJ_ERROR_TELEPHONY_SERVICE_ERROR;
            case TELEPHONY_ERR_VCARD_FILE_INVALID:
            case TELEPHONY_ERR_FAIL:
            case TELEPHONY_ERR_MEMCPY_FAIL:
            case TELEPHONY_ERR_MEMSET_FAIL:
            case TELEPHONY_ERR_STRCPY_FAIL:
            case TELEPHONY_ERR_LOCAL_PTR_NULL:
            case TELEPHONY_ERR_SUBSCRIBE_BROADCAST_FAIL:
            case TELEPHONY_ERR_PUBLISH_BROADCAST_FAIL:
            case TELEPHONY_ERR_ADD_DEATH_RECIPIENT_FAIL:
            case TELEPHONY_ERR_STRTOINT_FAIL:
            case TELEPHONY_ERR_RIL_CMD_FAIL:
            case TELEPHONY_ERR_DATABASE_WRITE_FAIL:
            case TELEPHONY_ERR_DATABASE_READ_FAIL:
            case TELEPHONY_ERR_UNKNOWN_NETWORK_TYPE:
            case CALL_ERR_INVALID_VIDEO_STATE:
                // 83000003
                return CJ_ERROR_TELEPHONY_SYSTEM_ERROR;
            case TELEPHONY_ERR_NO_SIM_CARD:
                // 83000004
                return CJ_ERROR_TELEPHONY_NO_SIM_CARD;
            case TELEPHONY_ERR_AIRPLANE_MODE_ON:
                // 83000005
                return CJ_ERROR_TELEPHONY_AIRPLANE_MODE_ON;
            case TELEPHONY_ERR_NETWORK_NOT_IN_SERVICE:
                // 83000006
                return CJ_ERROR_TELEPHONY_NETWORK_NOT_IN_SERVICE;
            case TELEPHONY_ERR_PERMISSION_ERR:
                // 201
                return CJ_ERROR_TELEPHONY_PERMISSION_DENIED;
            case TELEPHONY_ERR_ILLEGAL_USE_OF_SYSTEM_API:
                // 202
                return CJ_ERROR_TELEPHONY_PERMISSION_DENIED;
            default:
                return errCode;
        }
    }

    bool TelephonyCallImpl::IsRinging(int32_t *errCode)
    {
        bool result = true;
        int32_t err = DelayedSingleton<CallManagerClient>::GetInstance()->IsRinging(result);
        *errCode = ConvertCJErrCode(err);
        return result;
    }

    char* TelephonyCallImpl::FormatPhoneNumberToE164(char *cPhoneNumber, char *cCountryCode, int32_t *errCode)
    {
        std::u16string formatNumber = u"";
        std::string tmpCountryCode = cCountryCode;
        std::string tmpPhoneNumber = cPhoneNumber;
        std::u16string phoneNumber = Str8ToStr16(tmpPhoneNumber);
        std::u16string countryCode = Str8ToStr16(tmpCountryCode);
        int32_t err = DelayedSingleton<CallManagerClient>::GetInstance()->FormatPhoneNumber(
            phoneNumber, countryCode, formatNumber);
        *errCode = ConvertCJErrCode(err);
        std::string result = Str16ToStr8(formatNumber);
        return MallocCString(result);
    }

    int32_t TelephonyCallImpl::MakeCall(OHOS::AbilityRuntime::Context* context, char* phoneNumber)
    {
        AAFwk::Want want;
        AppExecFwk::ElementName element("", "com.ohos.contacts", "com.ohos.contacts.MainAbility");
        want.SetElement(element);
        AAFwk::WantParams wantParams;
        wantParams.SetParam("phoneNumber", AAFwk::String::Box(std::string(phoneNumber)));
        wantParams.SetParam("pageFlag", AAFwk::String::Box("page_flag_edit_before_calling"));
        wantParams.SetParam(AAFwk::Want::PARAM_BACK_TO_OTHER_MISSION_STACK, AAFwk::Boolean::Box(true));
        want.SetParams(wantParams);
        if (context == nullptr) {
            return CJ_ERROR_TELEPHONY_ARGUMENT_ERROR;
        }
        int32_t err = AAFwk::AbilityManagerClient::GetInstance()->StartAbility(want, context->GetToken());
        return ConvertCJErrCode(err);
    }

    static bool IsValidSlotId(int32_t slotId)
    {
        return ((slotId >= DEFAULT_SIM_SLOT_ID) && (slotId < SIM_SLOT_COUNT));
    }

    bool TelephonyCallImpl::IsEmergencyPhoneNumber(char *cPhoneNumber, int32_t slotId, int32_t* errCode)
    {
        if (!IsValidSlotId(slotId)) {
            *errCode = ConvertCJErrCode(TELEPHONY_ERR_SLOTID_INVALID);
            return false;
        }
        std::string temPhoneNumber = cPhoneNumber;
        std::u16string phoneNumberU16 = Str8ToStr16(temPhoneNumber);
        bool result = false;
        int32_t err = DelayedSingleton<CallManagerClient>::GetInstance()->IsEmergencyPhoneNumber(
            phoneNumberU16, slotId, result);
        *errCode = ConvertCJErrCode(err);
        return result;
    }

    bool TelephonyCallImpl::HasCall()
    {
        return DelayedSingleton<CallManagerClient>::GetInstance()->HasCall();
    }

    int32_t TelephonyCallImpl::GetCallState()
    {
        return DelayedSingleton<CallManagerClient>::GetInstance()->GetCallState();
    }

    void TelephonyCallImpl::Init()
    {
        DelayedSingleton<CallManagerClient>::GetInstance()->Init(TELEPHONY_CALL_MANAGER_SYS_ABILITY_ID);
    }

    int32_t TelephonyCallImpl::DialCall(char *phoneNumber, DialCallOptions options)
    {
        OHOS::AppExecFwk::PacMap dialInfo;
        dialInfo.PutIntValue("accountId", options.accountId);
        dialInfo.PutIntValue("videoState", options.videoState);
        dialInfo.PutIntValue("dialScene", options.dialScene);
        dialInfo.PutIntValue("dialType", options.dialType);
        std::string temPhoneNumber = phoneNumber;
        int32_t err = DelayedSingleton<CallManagerClient>::GetInstance()->DialCall(
            Str8ToStr16(temPhoneNumber), dialInfo);
        return ConvertCJErrCode(err);
    }

    bool TelephonyCallImpl::HasVoiceCapability()
    {
        return DelayedSingleton<CallManagerClient>::GetInstance()->HasVoiceCapability();
    }

    char* TelephonyCallImpl::FormatPhoneNumber(char *cPhoneNumber, char *cCountryCode, int32_t *errCode)
    {
        std::u16string formatNumber = u"";
        std::string tmpPhoneNumber = cPhoneNumber;
        std::string tmpCountryCode = cCountryCode;
        std::u16string phoneNumber = Str8ToStr16(tmpPhoneNumber);
        std::u16string countryCode = Str8ToStr16(tmpCountryCode);
        int32_t err = DelayedSingleton<CallManagerClient>::GetInstance()->FormatPhoneNumber(
            phoneNumber, countryCode, formatNumber);
        *errCode = ConvertCJErrCode(err);
        std::string result = Str16ToStr8(formatNumber);
        return MallocCString(result);
    }
}
}