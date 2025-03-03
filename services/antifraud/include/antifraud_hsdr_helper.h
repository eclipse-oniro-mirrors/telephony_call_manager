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

#ifndef ANTIFRAUD_HSDR_HELPER_H
#define ANTIFRAUD_HSDR_HELPER_H

#include <functional>
#include <mutex>

#include "ability_connect_callback_stub.h"
#include "iremote_broker.h"
#include "iremote_object.h"
#include "iremote_proxy.h"
#include "message_parcel.h"
#include "singleton.h"

namespace OHOS {
namespace Telephony {
const std::string HSDR_BUNDLE_NAME = "com.huawei.hmos.hsdr";
const std::string HSDR_ABILITY_NAME = "HSDRService";
constexpr int HSDR_USERID = 100;

enum class HsdrCommands {
    COMMAND_QUERY_CURRENT_INFO = 1,
    COMMAND_CLOUD_CONNECT = 2,
    COMMAND_UCS_REQUEST = 3,
    COMMAND_FILE_REQUEST = 4,
};

enum class UcsMethod {
    Encrypt = 0,
    Decrypt = 1,
    Sign = 2,
    Unsign = 3,
};

struct HsdrRequest {
public:
    std::string requestId_;
    std::string serviceName_;
    HsdrCommands command_;
    std::string body_;
};

struct HsdrResponse {
public:
    std::string requestId_;
    std::string body_;
};

using ConnectedCallback = std::function<void(sptr<IRemoteObject>)>;
using OnResponse = std::function<void(const HsdrResponse &)>;
using OnError = std::function<void(int)>;

class HsdrCallback : public IRemoteBroker {
public:
    DECLARE_INTERFACE_DESCRIPTOR(u"OHOS.Security.CloudConnectCallback");
};

class HsdrCallbackStub : public IRemoteStub<HsdrCallback> {
public:
    HsdrCallbackStub(const std::string &requestId, OnResponse onResponse, OnError onError)
        : requestId_(requestId), onResponse_(onResponse), onError_(onError) {}

    ~HsdrCallbackStub() = default;

    int32_t OnRemoteRequest(uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option) override;

private:
    std::string requestId_;
    OnResponse onResponse_;
    OnError onError_;
};

class HsdrConnection : public AAFwk::AbilityConnectionStub {
public:
    explicit HsdrConnection(ConnectedCallback connectedCallback) : connectedCallback_(connectedCallback) {}

    ~HsdrConnection() = default;

    void OnAbilityConnectDone(
        const AppExecFwk::ElementName &element, const sptr<IRemoteObject> &remoteObject, int resultCode) override;

    void OnAbilityDisconnectDone(const AppExecFwk::ElementName &element, int resultCode) override;

    bool IsAlive();

    sptr<IRemoteObject> GetAbilityProxy();

private:
    ConnectedCallback connectedCallback_;
    std::mutex remoteProxyMutex_;
    sptr<IRemoteObject> remoteObject_;
};

class HsdrHelper {
DECLARE_DELAYED_REF_SINGLETON(HsdrHelper);
public:
    int ConnectHsdr(ConnectedCallback connectedCallback);

    void DisconnectHsdr();

private:
    std::mutex connectionMutex_;
    sptr<HsdrConnection> connection_;
};

class HsdrInterface : public IRemoteBroker {
public:
    DECLARE_INTERFACE_DESCRIPTOR(u"OHOS.Security.HSDR");

    virtual int RequestHsdrServiceSync(const HsdrRequest &request, std::string &result) = 0;

    virtual int RequestHsdrServiceAsync(sptr<IRemoteObject> callbackStub, const HsdrRequest &request) = 0;
};

class HsdrProxy : IRemoteProxy<HsdrInterface> {
public:
    explicit HsdrProxy(const sptr<IRemoteObject> &remote) : IRemoteProxy<HsdrInterface>(remote) {}

    virtual ~HsdrProxy() {}

    virtual int RequestHsdrServiceSync(const HsdrRequest &request, std::string &result);

    virtual int RequestHsdrServiceAsync(sptr<IRemoteObject> callbackStub, const HsdrRequest &request);
private:
    bool WriteMessageParcel(MessageParcel &data, const HsdrRequest &request);
};

} // namespace Telephony
} // namespace OHOS

#endif