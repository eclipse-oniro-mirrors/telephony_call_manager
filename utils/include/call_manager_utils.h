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

#ifndef CALL_MANAGER_UTILS_H
#define CALL_MANAGER_UTILS_H

#include "call_manager_info.h"
#include "message_parcel.h"
#include "bundle_mgr_proxy.h"
#include "bundle_mgr_interface.h"
#include "iservice_registry.h"
#include "system_ability_definition.h"

namespace OHOS {
namespace Telephony {
class CallManagerUtils {
public:
    static void WriteCallAttributeInfo(const CallAttributeInfo &info, MessageParcel &messageParcel);
    static bool IsBundleInstalled(const std::string &bundleName, int32_t userId);

private:
    static bool IsForcedReportVoiceCall(const CallAttributeInfo &info);
    static void WriteVoipCallInfo(const CallAttributeInfo &info, MessageParcel &messageParcel);
};
} // namespace Telephony
} // namespace OHOS

#endif // CALL_MANAGER_UTILS_H