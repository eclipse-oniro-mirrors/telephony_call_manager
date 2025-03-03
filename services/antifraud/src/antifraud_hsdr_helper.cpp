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

#include "antifraud_hsdr_helper.h"

#include <string>
#include <vector>
#include <cJSON.h>
#include "errors.h"
#include "extension_manager_client.h"
#include "ipc_types.h"
#include "iremote_object.h"
#include "message_option.h"
#include "telephony_log_wrapper.h"
#include "want.h"

namespace OHOS {
namespace Telephony {
using Client = AAFwk::ExtensionManagerClient;
constexpr int CALLMANAGER_HSDR_ERR_SUCCESS = 0;
constexpr int CALLMANAGER_HSDR_ERR_LOCAL_PTR_NULL = -1;
constexpr int CALLMANAGER_HSDR_ERR_WRITE_DATA_FAIL = -2;
constexpr int CALLMANAGER_HSDR_ERR_READ_DATA_FAIL = -3;
constexpr int CALLMANAGER_HSDR_ERR_PERMISSION_ERR = -4;
constexpr int UNEXPECT_DISCONNECT_CODE = -1;

static bool ReadStr16(MessageParcel &parcel, std::string &str8)
{
    std::u16string str16;
    auto isOk = parcel.ReadString16(str16);
    str8 = Str16ToStr8(str16);
    return isOk;
}

void HsdrConnection::OnAbilityConnectDone(
    const AppExecFwk::ElementName &element, const sptr<IRemoteObject> &remoteObject, int resultCode)
{
    std::lock_guard<std::mutex> lock(remoteProxyMutex_);
    TELEPHONY_LOGI("HsdrConnection::OnAbilityConnectDone begin.");
    if (resultCode != 0) {
        TELEPHONY_LOGE("connect failed, resultCode: %{public}d", resultCode);
        return;
    }
    if (remoteObject == nullptr || connectedCallback_ == nullptr) {
        TELEPHONY_LOGE("remoteObject or connectedCallback_ is null.");
        return;
    }
    remoteObject_ = remoteObject;
    connectedCallback_(remoteObject_);
}

void HsdrConnection::OnAbilityDisconnectDone(const AppExecFwk::ElementName &element, int resultCode)
{
    std::lock_guard<std::mutex> lock(remoteProxyMutex_);
    if (resultCode == UNEXPECT_DISCONNECT_CODE) {
        TELEPHONY_LOGE("unexpected disconnect!");
    }
    TELEPHONY_LOGI("HsdrConnection::OnAbilityDisconnectDone");
    remoteObject_ = nullptr;
}

bool HsdrConnection::IsAlive()
{
    std::lock_guard<std::mutex> lock(remoteProxyMutex_);
    return remoteObject_ != nullptr && !remoteObject_->IsObjectDead();
}

sptr<IRemoteObject> HsdrConnection::GetAbilityProxy()
{
    std::lock_guard<std::mutex> lock(remoteProxyMutex_);
    return remoteObject_;
}

HsdrHelper::HsdrHelper() {}
int HsdrHelper::ConnectHsdr(ConnectedCallback connectedCallback)
{
    std::lock_guard<std::mutex> lock(connectionMutex_);
    TELEPHONY_LOGI("ConnectHsdr begin.");
    if (connection_ != nullptr && connection_->IsAlive()) {
        TELEPHONY_LOGI("Already connected, use old connection.");
        connectedCallback(connection_->GetAbilityProxy());
        return OHOS::ERR_OK;
    }
    AAFwk::Want want;
    want.SetElementName(HSDR_BUNDLE_NAME, HSDR_ABILITY_NAME);
    want.SetAction("NO_PUSH");
    connection_ = new (std::nothrow) HsdrConnection(connectedCallback);
    if (connection_ == nullptr) {
        TELEPHONY_LOGE("connection_ is nullptr.");
        return CALLMANAGER_HSDR_ERR_LOCAL_PTR_NULL;
    }
    auto errCode = Client::GetInstance().ConnectServiceExtensionAbility(want, connection_, HSDR_USERID);
    if (errCode != OHOS::ERR_OK) {
        connection_ = nullptr;
        TELEPHONY_LOGE("ConnectServiceExtensionAbility failed, errCode: %{public}d", errCode);
        return errCode;
    }
    TELEPHONY_LOGI("ConnectHsdr done.");
    return CALLMANAGER_HSDR_ERR_SUCCESS;
}

void HsdrHelper::DisconnectHsdr()
{
    std::lock_guard<std::mutex> lock(connectionMutex_);
    TELEPHONY_LOGI("HsdrHelper::DisconnectHsdr begin.");
    if (connection_ == nullptr) {
        return;
    }
    auto errCode = Client::GetInstance().DisconnectAbility(connection_);
    connection_ = nullptr;
    if (errCode != OHOS::ERR_OK) {
        TELEPHONY_LOGE("DisconnectAbility failed, errCode: %{public}d", errCode);
    }
}

int32_t HsdrCallbackStub::OnRemoteRequest(
    uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option)
{
    TELEPHONY_LOGI("HsdrCallbackStub::OnRemoteRequest begin, code = %{public}u", code);
    auto descriptor = GetDescriptor();
    auto remoteDescriptor = data.ReadInterfaceToken();
    if (descriptor != remoteDescriptor) {
        onError_(CALLMANAGER_HSDR_ERR_PERMISSION_ERR);
        return CALLMANAGER_HSDR_ERR_PERMISSION_ERR;
    }
    HsdrResponse response;
    if (!ReadStr16(data, response.requestId_)) {
        onError_(CALLMANAGER_HSDR_ERR_READ_DATA_FAIL);
        return CALLMANAGER_HSDR_ERR_READ_DATA_FAIL;
    }
    if (response.requestId_ != requestId_) {
        TELEPHONY_LOGW("expect %{public}s, received %{public}s", requestId_.c_str(), response.requestId_.c_str());
    }
    int cloudErrCode = data.ReadInt32();
    TELEPHONY_LOGI("cloud ErrCode: %{public}d", cloudErrCode);
    std::vector<std::u16string> body;
    if (!data.ReadString16Vector(&body)) {
        onError_(CALLMANAGER_HSDR_ERR_READ_DATA_FAIL);
        return CALLMANAGER_HSDR_ERR_READ_DATA_FAIL;
    }
    for (const auto &str : body) {
        response.body_ += OHOS::Str16ToStr8(str);
    }
    onResponse_(response);
    TELEPHONY_LOGI("HsdrCallbackStub::OnRemoteRequest done.");
    return CALLMANAGER_HSDR_ERR_SUCCESS;
}

int HsdrProxy::RequestHsdrServiceSync(const HsdrRequest &request, std::string &result)
{
    int errCode = CALLMANAGER_HSDR_ERR_SUCCESS;
    MessageParcel data;
    if (!WriteMessageParcel(data, request)) {
        return CALLMANAGER_HSDR_ERR_WRITE_DATA_FAIL;
    }

    MessageParcel reply;
    MessageOption option;
    auto remote = Remote();
    if (remote == nullptr) {
        return CALLMANAGER_HSDR_ERR_LOCAL_PTR_NULL;
    }
    errCode = remote->SendRequest(static_cast<uint32_t>(request.command_), data, reply, option);
    if (errCode != OHOS::ERR_OK) {
        TELEPHONY_LOGE("SendRequest failed, errCode: %{public}d", errCode);
        return errCode;
    }
    int replyCode = reply.ReadInt32();
    TELEPHONY_LOGI("reply code: %{public}d", replyCode);
    std::vector<std::u16string> responseBuffer;
    if (!reply.ReadString16Vector(&responseBuffer)) {
        return CALLMANAGER_HSDR_ERR_READ_DATA_FAIL;
    }
    for (const auto &part : responseBuffer) {
        result += OHOS::Str16ToStr8(part);
    }
    return errCode;
}

int HsdrProxy::RequestHsdrServiceAsync(sptr<IRemoteObject> callbackStub, const HsdrRequest &request)
{
    int errCode = CALLMANAGER_HSDR_ERR_SUCCESS;
    MessageParcel data;
    if (!WriteMessageParcel(data, request)) {
        return CALLMANAGER_HSDR_ERR_WRITE_DATA_FAIL;
    }
    if (!data.WriteRemoteObject(callbackStub)) {
        return CALLMANAGER_HSDR_ERR_WRITE_DATA_FAIL;
    }
    MessageParcel reply;
    MessageOption option;
    auto remote = Remote();
    if (remote == nullptr) {
        return CALLMANAGER_HSDR_ERR_LOCAL_PTR_NULL;
    }
    errCode = remote->SendRequest(static_cast<uint32_t>(request.command_), data, reply, option);
    if (errCode != OHOS::ERR_OK) {
        TELEPHONY_LOGE("SendRequest failed, errCode: %{public}d", errCode);
        return errCode;
    }
    auto replyCode = reply.ReadInt32();
    TELEPHONY_LOGI("reply code: %{public}d", replyCode);
    return errCode;
}

bool HsdrProxy::WriteMessageParcel(MessageParcel &data, const HsdrRequest &request)
{
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        return false;
    }

    std::vector<std::u16string> payload = { OHOS::Str8ToStr16(request.body_) };
    if (!data.WriteString16(OHOS::Str8ToStr16(request.serviceName_)) ||
        !data.WriteString16(OHOS::Str8ToStr16(request.requestId_)) ||
        !data.WriteString16Vector(payload)) {
        return false;
    }
    return true;
}
} // namespace Telephony
} // namespace OHOS