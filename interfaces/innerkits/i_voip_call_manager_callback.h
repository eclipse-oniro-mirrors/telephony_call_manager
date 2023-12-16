/*
 * Copyright (C) 2023 Huawei Device Co., Ltd.
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

#ifndef I_VOIP_CALL_MANAGER_CALLBACK_H
#define I_VOIP_CALL_MANAGER_CALLBACK_H

#include "iremote_broker.h"
#include "pac_map.h"
#include "voip_call_manager_info.h"

namespace OHOS {
namespace Telephony {
class IVoipCallManagerCallback : public IRemoteBroker {
public:
    IVoipCallManagerCallback() : bundleName_("") {}
    virtual ~IVoipCallManagerCallback() = default;

    enum {
            INTERFACE_REPORT_VOIP_CALL_EVENT_CHANGE = 0,
        };

    virtual int32_t OnVoipCallEventChange(const VoipCallEvents &event) = 0;

    void SetBundleName(const std::string &name)
    {
        bundleName_ = name;
    }

    std::string GetBundleName()
    {
        return bundleName_;
    }

public:
    std::string bundleName_;
    DECLARE_INTERFACE_DESCRIPTOR(u"OHOS.Telephony.IVoipCallManagerCallback");
};
} // namespace Telephony
} // namespace OHOS

#endif
