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

#ifndef NUMBER_IDENTITY_SERVICE_H
#define NUMBER_IDENTITY_SERVICE_H

#include "ability_connect_callback_stub.h"
#include "ffrt.h"
#include "ipc_object_stub.h"
#include "iremote_broker.h"
#include "iremote_proxy.h"
#include "message_option.h"
#include "message_parcel.h"
#include "singleton.h"
#include <atomic>
#include <cstdint>
#include <functional>

namespace OHOS {
namespace Telephony {

using std::atomic;
using std::function;
using ffrt::recursive_mutex;
using ConnectedCallback = function<void(sptr<IRemoteObject>)>;
using DisconnectedCallback = function<void()>;
using RemoteRequestCallBack = function<int32_t(uint32_t, MessageParcel &, MessageParcel &, MessageOption &)>;

class NumberIdentityCallbackStub : public IPCObjectStub {
  public:
    DECLARE_INTERFACE_DESCRIPTOR(u"OHOS::Telephony::NumberIdentityCallback");

    explicit NumberIdentityCallbackStub(RemoteRequestCallBack callback);

    ~NumberIdentityCallbackStub() = default;

    virtual int32_t OnRemoteRequest(
        uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option) override;

  private:
    RemoteRequestCallBack callback_;
};

class NumberIdentityConnection : public AAFwk::AbilityConnectionStub {
  public:
    explicit NumberIdentityConnection(ConnectedCallback onConnected, DisconnectedCallback onDisconnected);

    ~NumberIdentityConnection();

    void OnAbilityConnectDone(
        const AppExecFwk::ElementName &element, const sptr<IRemoteObject> &remoteObject, int resultCode) override;

    void OnAbilityDisconnectDone(const AppExecFwk::ElementName &element, int resultCode) override;

    bool IsAlive();

    sptr<IRemoteObject> GetAbilityProxy();

  private:
    ConnectedCallback connectedCallback_;

    DisconnectedCallback disconnectedCallback_;

    recursive_mutex remoteProxyMutex_;

    sptr<IRemoteObject> remoteObject_;
};

class NumberIdentityServiceHelper {
    DECLARE_DELAYED_REF_SINGLETON(NumberIdentityServiceHelper);

  public:
    void NotifyNumberMarkDataUpdate();

  private:
    int Connect(ConnectedCallback connectedCallback, DisconnectedCallback disconnectedCallback);

    void Disconnect();

    int Request(ConnectedCallback connectedCallback);

    atomic<bool> working_ = false;

    recursive_mutex connectionMutex_;

    sptr<NumberIdentityConnection> connection_;
};

enum NumberIdentityServiceMessageCode {
    IMPORT_NUMBER_MARK_DATA_FILE = 0,
};

class INumberIdentityService : public IRemoteBroker {
  public:
    DECLARE_INTERFACE_DESCRIPTOR(u"com.numberidentity:NumberIdentityService");

    virtual void NotifyNumberMarkDataUpdate() = 0;
};

class NumberIdentityServiceProxy : IRemoteProxy<INumberIdentityService> {
  public:
    explicit NumberIdentityServiceProxy(const sptr<IRemoteObject> &remote);

    virtual ~NumberIdentityServiceProxy() = default;

    virtual void NotifyNumberMarkDataUpdate() override;
};

} // namespace Telephony
} // namespace OHOS

#endif /* NUMBER_IDENTITY_SERVICE_H */