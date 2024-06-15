/*
 * Copyright (C) 2021-2023 Huawei Device Co., Ltd.
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

#ifndef OHOS_APPSECURITYPRIVACY_SECURITYPRIVACY_SUPERPRIVACY_ISUPERPRIVACYMANAGER_H
#define OHOS_APPSECURITYPRIVACY_SECURITYPRIVACY_SUPERPRIVACY_ISUPERPRIVACYMANAGER_H

#include <string_ex.h>
#include <cstdint>
#include <iremote_broker.h>

namespace OHOS {
namespace Telephony {
class ISuperPrivacyManager : public IRemoteBroker {
public:
    DECLARE_INTERFACE_DESCRIPTOR(u"OHOS.AppSecurityPrivacy.SecurityPrivacyServer.SuperPrivacy.ISuperPrivacyManager");
    virtual ErrCode SetSuperPrivacyMode(int32_t mode, int32_t source) = 0;

protected:
    const int VECTOR_MAX_SIZE = 102400;
    const int LIST_MAX_SIZE = 102400;
    const int MAP_MAX_SIZE = 102400;
};
} // namespace Telephony
} // namespace OHOS

#endif
//OHOS_APPSECURITYPRIVACY_SECURITYPRIVACY_SUPERPRIVACY_ISUPERPRIVACYMANAGER_H
