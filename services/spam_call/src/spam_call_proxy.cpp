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

#include "spam_call_proxy.h"

#include "message_option.h"
#include "message_parcel.h"

#include "call_manager_errors.h"
#include "telephony_log_wrapper.h"
#include "callback_stub_helper.h"
#include "callback_stub_helper.h"
#include <string_ex.h>

namespace OHOS {
namespace Telephony {
SpamCallProxy::SpamCallProxy(const sptr<IRemoteObject> &impl)
    : IRemoteProxy<ISpamCall>(impl)
{}

int32_t SpamCallProxy::DetectSpamCall(const std::string &phoneNumber, const int32_t &slotId,
    std::shared_ptr<SpamCallAdapter> spamCallAdapter)
{
    MessageParcel dataParcel;
    std::u16string myDescriptor = SpamCallProxy::GetDescriptor();
    if (!dataParcel.WriteInterfaceToken(myDescriptor)) {
        TELEPHONY_LOGE("Write descriptor fail");
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    auto instance = new (std::nothrow) CallbackStubHelper(spamCallAdapter);
    if (instance == nullptr) {
        TELEPHONY_LOGE("CallbackStubHelper is null!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    if (!dataParcel.WriteString16(Str8ToStr16(phoneNumber))) {
        TELEPHONY_LOGE("WriteString16 failed");
    }
    if (!dataParcel.WriteInt32(slotId)) {
        TELEPHONY_LOGE("WriteInt32 failed");
    }
    if (!dataParcel.WriteRemoteObject(instance)) {
        TELEPHONY_LOGE("WriteRemoteObject failed");
    }
    MessageParcel replyParcel;
    auto remote = Remote();
    if (remote == nullptr) {
        TELEPHONY_LOGE("Remote() return nullptr!");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    MessageOption option;
    int32_t ret = remote->SendRequest(COMMAND_DETECT_SPAM_CALL, dataParcel, replyParcel, option);
    if (ret != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("DetectSpamCall:%{public}d errCode:%{public}d", COMMAND_DETECT_SPAM_CALL, ret);
        return ret;
    }
    return replyParcel.ReadInt32();
}

int32_t SpamCallProxy::DetectNeedNotify(const int32_t &slotId, std::shared_ptr<SpamCallAdapter> spamCallAdapter)
{
    MessageParcel dataParcel;
    std::u16string myDescriptor = SpamCallProxy::GetDescriptor();
    if (!dataParcel.WriteInterfaceToken(myDescriptor)) {
        TELEPHONY_LOGE("Write descriptor fail");
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    auto instance = new (std::nothrow) CallbackStubHelper(spamCallAdapter);
    if (instance == nullptr) {
        TELEPHONY_LOGE("CallbackStubHelper is null!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }

    if (!dataParcel.WriteInt32(slotId)) {
        TELEPHONY_LOGE("WriteInt32 failed");
    }
    if (!dataParcel.WriteRemoteObject(instance)) {
        TELEPHONY_LOGE("WriteRemoteObject failed");
    }
    MessageParcel replyParcel;
    auto remote = Remote();
    if (remote == nullptr) {
        TELEPHONY_LOGE("Remote() return nullptr!");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    MessageOption option;
    int32_t ret = remote->SendRequest(COMMAND_DETECT_NEED_NOTIFY, dataParcel, replyParcel, option);
    if (ret != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("DetectSpamCall:%{public}d errCode:%{public}d", COMMAND_DETECT_NEED_NOTIFY, ret);
        return ret;
    }
    return replyParcel.ReadInt32();
}
} // namespace Telephony
} // namespace OHOS
