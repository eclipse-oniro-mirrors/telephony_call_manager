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

#include <mutex>
#include <new>

#include "c/type_def.h"
#include "cpp/task.h"
#include "errors.h"
#include "extension_manager_client.h"
#include "message_option.h"
#include "singleton.h"
#include "telephony_errors.h"
#include "telephony_log_wrapper.h"
#include "want.h"

#include "number_identity_service.h"

namespace OHOS {
namespace Telephony {

using Client = AAFwk::ExtensionManagerClient;
using ffrt::qos_default;
using ffrt::task_attr;
using std::lock_guard;

inline constexpr const int USER_ID = 100;
const char *NUMBER_IDENTITY_BUNDLE_NAME = "com.numberidentity";
const char *NUMBER_IDENTITY_SERVICE_EXT_ABILITY = "NumberIdentityServiceExtAbility";

NumberIdentityConnection::NumberIdentityConnection(ConnectedCallback onConnected, DisconnectedCallback onDisconnected)
    : connectedCallback_(onConnected), disconnectedCallback_(onDisconnected)
{
    TELEPHONY_LOGI("Create connection.");
}

NumberIdentityConnection::~NumberIdentityConnection()
{
    TELEPHONY_LOGI("Connection teardown.");
}

void NumberIdentityConnection::OnAbilityConnectDone(
    const AppExecFwk::ElementName &element, const sptr<IRemoteObject> &remoteObject, int resultCode)
{
    lock_guard lock(remoteProxyMutex_);
    TELEPHONY_LOGI("NumberIdentityService connect done.");
    if (remoteObject == nullptr) {
        TELEPHONY_LOGE("remoteObject is nullptr.");
        return;
    }
    remoteObject_ = remoteObject;
    if (connectedCallback_ == nullptr) {
        TELEPHONY_LOGE("connectedCallback_ is nullptr.");
        return;
    }
    if (IsAlive()) {
        ffrt::submit(std::bind(connectedCallback_, remoteObject_), task_attr().qos(qos_default::qos_background));
    }
}

void NumberIdentityConnection::OnAbilityDisconnectDone(const AppExecFwk::ElementName &element, int resultCode)
{
    lock_guard lock(remoteProxyMutex_);
    TELEPHONY_LOGI("NumberIdentityService disconnect done.");
    remoteObject_ = nullptr;
    if (disconnectedCallback_ == nullptr) {
        TELEPHONY_LOGE("disconnectedCallback_ is nullptr.");
        return;
    }
    ffrt::submit(disconnectedCallback_, task_attr().qos(qos_default::qos_background));
}

bool NumberIdentityConnection::IsAlive()
{
    lock_guard lock(remoteProxyMutex_);
    return remoteObject_ != nullptr && !remoteObject_->IsObjectDead();
}

sptr<IRemoteObject> NumberIdentityConnection::GetAbilityProxy()
{
    lock_guard lock(remoteProxyMutex_);
    return remoteObject_;
}

NumberIdentityServiceHelper::NumberIdentityServiceHelper()
{
    TELEPHONY_LOGI("create helper");
}

int NumberIdentityServiceHelper::Connect(ConnectedCallback onConnected, DisconnectedCallback onDisconnected)
{
    lock_guard connectionLock(connectionMutex_);
    if (connection_ != nullptr && connection_->IsAlive()) {
        TELEPHONY_LOGI("Already connected, use old connection.");
        auto remote = connection_->GetAbilityProxy();
        ffrt::submit(std::bind(onConnected, remote), ffrt::task_attr().qos(ffrt::qos_default::qos_background));
        return TELEPHONY_SUCCESS;
    }
    AAFwk::Want want;
    want.SetElementName(NUMBER_IDENTITY_BUNDLE_NAME, NUMBER_IDENTITY_SERVICE_EXT_ABILITY);
    connection_ = new (std::nothrow) NumberIdentityConnection(onConnected, onDisconnected);
    if (connection_ == nullptr) {
        TELEPHONY_LOGE("new NumberIdentityConnection failed.");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    auto ret = Client::GetInstance().ConnectServiceExtensionAbility(want, connection_, USER_ID);
    if (ret != ERR_OK) {
        TELEPHONY_LOGE("ConnectServiceExtensionAbility failed, errCode = %{public}d", ret);
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    TELEPHONY_LOGI("Connect done.");
    return TELEPHONY_SUCCESS;
}

void NumberIdentityServiceHelper::Disconnect()
{
    lock_guard connectionLock(connectionMutex_);
    if (connection_ == nullptr) {
        TELEPHONY_LOGI("Already disconnected.");
        return;
    }
    auto ret = Client::GetInstance().DisconnectAbility(connection_);
    TELEPHONY_LOGI("DisconnectAbility errCode = %{public}d", ret);
    connection_ = nullptr;
}

void NumberIdentityServiceHelper::NotifyNumberMarkDataUpdate()
{
    TELEPHONY_LOGI("Notify task update begin.");
    auto &helper = DelayedRefSingleton<NumberIdentityServiceHelper>::GetInstance();
    ConnectedCallback onConnected = [&helper](sptr<IRemoteObject> remote) {
        if (helper.working_) {
            TELEPHONY_LOGI("NumberIdentityService notify task is working, skip this notify.");
            return;
        }
        helper.working_ = true;
        TELEPHONY_LOGI("NumberIdentityService async notify begin.");
        NumberIdentityServiceProxy service(remote);
        service.NotifyNumberMarkDataUpdate();
        DelayedRefSingleton<NumberIdentityServiceHelper>::GetInstance().Disconnect();
        TELEPHONY_LOGI("NumberIdentityService async notify end.");
    };
    DisconnectedCallback onDisconnected = [&helper]() {
        TELEPHONY_LOGI("NumberIdentityService notify task disconnected.");
        helper.working_ = false;
    };
    auto ret = helper.Connect(onConnected, onDisconnected);
    if (ret != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("Connect failed: ret = %{public}d", ret);
        return;
    }
    TELEPHONY_LOGI("Notify task scheduled.");
}

NumberIdentityServiceProxy::NumberIdentityServiceProxy(const sptr<IRemoteObject> &remote)
    : IRemoteProxy<INumberIdentityService>(remote)
{
}

void NumberIdentityServiceProxy::NotifyNumberMarkDataUpdate()
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    auto remote = Remote();
    if (remote == nullptr) {
        TELEPHONY_LOGF("Remote is nullptr!");
        return;
    }
    if (remote->IsObjectDead()) {
        TELEPHONY_LOGE("Remote died.");
    }
    data.WriteInterfaceToken(GetDescriptor());
    auto ret = remote->SendRequest(NumberIdentityServiceMessageCode::IMPORT_NUMBER_MARK_DATA_FILE, data, reply, option);
    if (ret != ERR_OK) {
        TELEPHONY_LOGE("SendRequest failed, ret = %{public}d", ret);
        return;
    }
    ret = reply.ReadInt32();
    if (ret != ERR_OK) {
        TELEPHONY_LOGE("Notify failed, reply = %{public}d", ret);
        return;
    }
    TELEPHONY_LOGI("Received ok reply, notify done.");
}

} // namespace Telephony
} // namespace OHOS