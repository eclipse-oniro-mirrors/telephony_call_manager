/*
 * Copyright (C) 2025-2025 Huawei Device Co., Ltd.
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

#include "taihe_ani_call_update_async_result.h"
#include "telephony_log_wrapper.h"
#include "taihe/platform/ani.hpp"
#include "napi_util.h"

namespace OHOS {
namespace Telephony {

static OHOS::Telephony::JsError ConvertErrorForJsBusinessError(int32_t errorCode)
{
    if (errorCode == TELEPHONY_ERR_PERMISSION_ERR) {
        JsError error = {};
        error.errorCode = JS_ERROR_TELEPHONY_PERMISSION_DENIED;
        error.errorMessage = "BusinessError 201: Permission denied.";
        return error;
    }

    return NapiUtil::ConverErrorMessageForJs(errorCode);
}

AniCallUpdateAsyncResult &AniCallUpdateAsyncResult::GetInstance()
{
    static AniCallUpdateAsyncResult instance;
    return instance;
}

void AniCallUpdateAsyncResult::UpdateGetCallWaitingStatus(std::shared_ptr<AniCallbackInfo> cb, int32_t errorCode,
    ::ohos::telephony::call::CallWaitingStatus status)
{
    if (cb == nullptr) {
        TELEPHONY_LOGE("AniCallbackInfo cb is nullptr");
        return;
    }

    auto env = cb->envT_;
    ani_ref aniError;
    if (errorCode) {
        auto error = ConvertErrorForJsBusinessError(errorCode);
        if (ANI_OK != AniCommonUtils::CreateBusinessError(env, aniError, error.errorCode, error.errorMessage)) {
            TELEPHONY_LOGE("CreateBusinessError failed");
            return;
        }
    } else {
        if (ANI_OK != env->GetNull(&aniError)) {
            TELEPHONY_LOGE("CreateBusinessError get null value failed");
            return;
        }
    }

    ani_enum_item aniEnumSatus = {};
    env->Enum_GetEnumItemByIndex(TH_ANI_FIND_ENUM(env, "@ohos.telephony.call.call.CallWaitingStatus"),
        static_cast<ani_size>(status.get_key()), &aniEnumSatus);

    if (cb->promise_ != nullptr) {
        AniCommonUtils::ExecAsyncCallbackPromise(env, cb->deferred_,
            static_cast<ani_object>(aniEnumSatus), aniError);
    } else {
        AniCommonUtils::ExecAsyncCallBack(env, static_cast<ani_object>(aniError),
            static_cast<ani_object>(aniEnumSatus), static_cast<ani_object>(cb->funRef_));
    }
}

void AniCallUpdateAsyncResult::UpdateGetCallRestrictionStatus(std::shared_ptr<AniCallbackInfo> cb, int32_t errorCode,
    ::ohos::telephony::call::RestrictionStatus status)
{
    if (cb == nullptr) {
        TELEPHONY_LOGE("AniCallbackInfo cb is nullptr");
        return;
    }

    auto env = cb->envT_;
    ani_ref aniError;
    if (errorCode) {
        auto error = ConvertErrorForJsBusinessError(errorCode);
        if (ANI_OK != AniCommonUtils::CreateBusinessError(env, aniError, error.errorCode, error.errorMessage)) {
            TELEPHONY_LOGE("CreateBusinessError failed");
            return;
        }
    } else {
        if (ANI_OK != env->GetNull(&aniError)) {
            TELEPHONY_LOGE("CreateBusinessError get null value failed");
            return;
        }
    }

    ani_enum_item aniEnumSatus = {};
    env->Enum_GetEnumItemByIndex(TH_ANI_FIND_ENUM(env, "@ohos.telephony.call.call.RestrictionStatus"),
        static_cast<ani_size>(status.get_key()), &aniEnumSatus);

    if (cb->promise_ != nullptr) {
        AniCommonUtils::ExecAsyncCallbackPromise(env, cb->deferred_,
            static_cast<ani_object>(aniEnumSatus), aniError);
    } else {
        AniCommonUtils::ExecAsyncCallBack(env, static_cast<ani_object>(aniError),
            static_cast<ani_object>(aniEnumSatus), static_cast<ani_object>(cb->funRef_));
    }
}

void AniCallUpdateAsyncResult::UpdateGetCallTransferInfo(std::shared_ptr<AniCallbackInfo> cb, int32_t errorCode,
    ::ohos::telephony::call::CallTransferResult result)
{
    if (cb == nullptr) {
        TELEPHONY_LOGE("AniCallbackInfo cb is nullptr");
        return;
    }

    auto env = cb->envT_;
    ani_ref aniError;
    if (errorCode) {
        auto error = ConvertErrorForJsBusinessError(errorCode);
        if (ANI_OK != AniCommonUtils::CreateBusinessError(env, aniError, error.errorCode, error.errorMessage)) {
            TELEPHONY_LOGE("CreateBusinessError failed");
            return;
        }
    } else {
        if (ANI_OK != env->GetNull(&aniError)) {
            TELEPHONY_LOGE("CreateBusinessError get null value failed");
            return;
        }
    }

    auto aniTransferResult = ConvertTaiheCallTransferResultToAni(env, result);

    if (cb->promise_ != nullptr) {
        AniCommonUtils::ExecAsyncCallbackPromise(env, cb->deferred_, aniTransferResult, aniError);
    } else {
        AniCommonUtils::ExecAsyncCallBack(env, static_cast<ani_object>(aniError),
            static_cast<ani_object>(aniTransferResult), static_cast<ani_object>(cb->funRef_));
    }
}

void AniCallUpdateAsyncResult::UpdateUndefinedCallState(std::shared_ptr<AniCallbackInfo> cb, int32_t errorCode)
{
    if (cb == nullptr) {
        TELEPHONY_LOGE("AniCallbackInfo cb is nullptr");
        return;
    }

    auto env = cb->envT_;
    ani_ref aniUndefined;
    ani_ref aniError;
    if (errorCode) {
        auto error = ConvertErrorForJsBusinessError(errorCode);
        if (ANI_OK != AniCommonUtils::CreateBusinessError(env, aniError, error.errorCode, error.errorMessage)) {
            TELEPHONY_LOGE("CreateBusinessError failed");
            return;
        }
    } else {
        if (ANI_OK != env->GetNull(&aniError)) {
            TELEPHONY_LOGE("CreateBusinessError get null value failed");
            return;
        }
    }

    if (ANI_OK != env->GetUndefined(&aniUndefined)) {
        TELEPHONY_LOGE("GetUndefined failed");
        return;
    }

    if (cb->promise_ != nullptr) {
        AniCommonUtils::ExecAsyncCallbackPromise(env, cb->deferred_,
            static_cast<ani_object>(aniUndefined), aniError);
    } else {
        AniCommonUtils::ExecAsyncCallBack(env, static_cast<ani_object>(aniError),
            static_cast<ani_object>(aniUndefined), static_cast<ani_object>(cb->funRef_));
    }
}

ani_object AniCallUpdateAsyncResult::ConvertTaiheCallTransferResultToAni(ani_env* env,
    ::ohos::telephony::call::CallTransferResult cppObj) const
{
    ani_enum_item aniFieldStatus = {};
    env->Enum_GetEnumItemByIndex(TH_ANI_FIND_ENUM(env, "@ohos.telephony.call.call.TransferStatus"),
        static_cast<ani_size>(cppObj.status.get_key()), &aniFieldStatus);
    ani_string aniFieldTeleNumber = {};
    env->String_NewUTF8(cppObj.teleNumber.c_str(), cppObj.teleNumber.size(), &aniFieldTeleNumber);
    ani_int aniFieldStartHour = static_cast<ani_int>(cppObj.startHour);
    ani_int aniFieldStartMinute = static_cast<ani_int>(cppObj.startMinute);
    ani_int aniFieldEndHour = static_cast<ani_int>(cppObj.endHour);
    ani_int aniFieldEndMinute = static_cast<ani_int>(cppObj.endMinute);
    ani_object aniObj = {};
    env->Object_New(TH_ANI_FIND_CLASS(env, "@ohos.telephony.call.call._taihe_CallTransferResult_inner"),
        TH_ANI_FIND_CLASS_METHOD(env, "@ohos.telephony.call.call._taihe_CallTransferResult_inner", "<ctor>", nullptr),
        &aniObj, aniFieldStatus, aniFieldTeleNumber, aniFieldStartHour, aniFieldStartMinute,
        aniFieldEndHour, aniFieldEndMinute);
    return aniObj;
}
} // namespace Telephony
} // namespace OHOS