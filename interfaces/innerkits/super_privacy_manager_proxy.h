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

#ifndef OHOS_APPSECURITYPRIVACY_SECURITYPRIVACY_SUPERPRIVACY_ISUPERPRIVACYMANAGER_H
#define OHOS_APPSECURITYPRIVACY_SECURITYPRIVACY_SUPERPRIVACY_ISUPERPRIVACYMANAGER_H

#include <iremote_proxy.h>

#include "isuper_privacy_manager.h"

namespace OHOS {
namespace Telephony {
class SuperPrivacyManagerProxy : public IRemoteProxy<ISuperPrivacyManager> {
public:
    explicit SuperPrivacyManagerProxy(const sptr<IRemoteObject>& remote)
        : IRemoteProxy<ISuperPrivacyManager>(remote) 
    {
    }

    virtual ~SuperPrivacyManagerProxy()
    {
    }

    ErrCode SetSuperPrivacyMode(
        int32_t mode, int32_t source)override;

private:
    static constexpr int32_t COMMAND_SET_SUPER_PRIVACY_MODE = MIN_TRANSACTION_ID + 0;
};
} // namespace Telephony
} // namespace OHOS

#endif
//OHOS_APPSECURITYPRIVACY_SECURITYPRIVACY_SUPERPRIVACY_ISUPERPRIVACYMANAGER_H