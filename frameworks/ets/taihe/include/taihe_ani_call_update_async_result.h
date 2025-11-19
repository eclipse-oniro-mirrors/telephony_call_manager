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

#ifndef TAIHE_ANI_UPDATE_ASYNC_RESULT_H
#define TAIHE_ANI_UPDATE_ASYNC_RESULT_H

#include "taihe_ani_callback_common.h"
#include "ohos.telephony.call.proj.hpp"
#include "ohos.telephony.call.impl.hpp"

namespace OHOS {
namespace Telephony {

class AniCallUpdateAsyncResult {
public:
    static AniCallUpdateAsyncResult &GetInstance();

    void UpdateGetCallWaitingStatus(std::shared_ptr<AniCallbackInfo> cb, int32_t errorCode,
        ::ohos::telephony::call::CallWaitingStatus status);
    void UpdateGetCallRestrictionStatus(std::shared_ptr<AniCallbackInfo> cb, int32_t errorCode,
        ::ohos::telephony::call::RestrictionStatus status);
    void UpdateGetCallTransferInfo(std::shared_ptr<AniCallbackInfo> cb, int32_t errorCode,
        ::ohos::telephony::call::CallTransferResult result);
    void UpdateUndefinedCallState(std::shared_ptr<AniCallbackInfo> cb, int32_t errorCode);

private:
    ani_object ConvertTaiheCallTransferResultToAni(ani_env* env,
        ::ohos::telephony::call::CallTransferResult cppObj) const;

private:
    AniCallUpdateAsyncResult() = default;
    ~AniCallUpdateAsyncResult() = default;
};
}
}
#endif