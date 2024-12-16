/*
 * Copyright (C) 2024-2024 Huawei Device Co., Ltd.
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

#ifndef TELEPHONY_CUST_WRAPPER_H
#define TELEPHONY_CUST_WRAPPER_H

#include "nocopyable.h"
#include "singleton.h"
#include "call_manager_info.h"

namespace OHOS {
namespace Telephony {
class TelephonyCustWrapper final {
DECLARE_DELAYED_REF_SINGLETON(TelephonyCustWrapper);
    
public:
    DISALLOW_COPY_AND_MOVE(TelephonyCustWrapper);
    void InitTelephonyCustWrapper();

    typedef bool (*IS_HIDE_VIOLENCE_OR_EMC_NUMBERS_IN_CALLLOG)(const CallAttributeInfo &info);
    typedef bool (*IS_CHANGE_DIAL_NUMBER_TO_TW_EMC)(std::u16string &oriNum, int32_t slotId);

    IS_HIDE_VIOLENCE_OR_EMC_NUMBERS_IN_CALLLOG isHideViolenceOrEmcNumbersInCallLog_ = nullptr;
    IS_CHANGE_DIAL_NUMBER_TO_TW_EMC isChangeDialNumberToTwEmc_ = nullptr;

private:
    void* telephonyCustWrapperHandle_ = nullptr;
};

#define TELEPHONY_CUST_WRAPPER ::OHOS::DelayedRefSingleton<TelephonyCustWrapper>::GetInstance()
} // namespace Telephony
} // namespace OHOS
#endif // TELEPHONY_CUST_WRAPPER_H