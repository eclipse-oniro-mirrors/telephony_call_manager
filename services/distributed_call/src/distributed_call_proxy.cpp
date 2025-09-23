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

#include <dlfcn.h>
#include <iostream>
#include <string>

#include "distributed_call_proxy.h"
#include "telephony_log_wrapper.h"
#include "telephony_errors.h"

namespace OHOS {
namespace Telephony {
namespace {
using GetDCallClientClass = OHOS::DistributedHardware::IDCallClient *(*)();
}

DistributedCallProxy::DistributedCallProxy()
{
    TELEPHONY_LOGI("DistributedCallProxy constructed.");
    dCallClientHandler_ = dlopen("libdistributed_call_client.z.so", RTLD_LAZY | RTLD_NODELETE);
    if (dCallClientHandler_ == nullptr) {
        TELEPHONY_LOGE("load dcall client sdk failed, fail reason: %{public}s.", dlerror());
        return;
    }
    TELEPHONY_LOGI("load dcall client sdk succeed.");
}

DistributedCallProxy::~DistributedCallProxy()
{
    TELEPHONY_LOGI("DistributedCallProxy destructed.");
    dcallClient_ = nullptr;
    if (dCallClientHandler_ != nullptr) {
        if (dlclose(dCallClientHandler_) != 0) {
            TELEPHONY_LOGE("unload dcall client sdk failed.");
            return;
        }
        dCallClientHandler_ = nullptr;
        TELEPHONY_LOGI("unload dcall client sdk succeed.");
    }
}

OHOS::DistributedHardware::IDCallClient* DistributedCallProxy::GetDCallClient()
{
    TELEPHONY_LOGI("get dcall client.");
    if (dCallClientHandler_ == nullptr) {
        TELEPHONY_LOGE("dcall client sdk not loaded.");
        return nullptr;
    }
    GetDCallClientClass getDCallClientClass = (GetDCallClientClass)dlsym(dCallClientHandler_,
        OHOS::DistributedHardware::DCALL_LOADER_GET_DCALL_CLIENT_HANDLER.c_str());
    if (getDCallClientClass == nullptr) {
        TELEPHONY_LOGE("get dcall client class handler is null, failed reason: %{public}s.", dlerror());
        dlclose(dCallClientHandler_);
        dCallClientHandler_ = nullptr;
        return nullptr;
    }
    OHOS::DistributedHardware::IDCallClient *dcallClient = getDCallClientClass();
    if (dcallClient == nullptr) {
        TELEPHONY_LOGE("fail to get dcall client.");
    } else {
        TELEPHONY_LOGI("get dcall client success.");
    }
    return dcallClient;
}

int32_t DistributedCallProxy::Init()
{
    std::lock_guard<ffrt::ffrt> lock(dcallClientMtx_);
    if (dCallClientHandler_ == nullptr) {
        TELEPHONY_LOGE("fail to load dcall sdk");
        return TELEPHONY_ERR_FAIL;
    }
    dcallClient_ = GetDCallClient();
    if (dcallClient_ == nullptr) {
        TELEPHONY_LOGE("fail to get dcall client");
        return TELEPHONY_ERR_FAIL;
    }
    if (dcallClient_->Init() != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("init dcall client failed");
        return TELEPHONY_ERR_FAIL;
    }
    return TELEPHONY_SUCCESS;
}

int32_t DistributedCallProxy::UnInit()
{
    std::lock_guard<ffrt::ffrt> lock(dcallClientMtx_);
    if (dcallClient_ == nullptr) {
        TELEPHONY_LOGE("dcallClient_ is nullptr");
        return TELEPHONY_ERR_FAIL;
    }
    return dcallClient_->UnInit();
}

int32_t DistributedCallProxy::SwitchDevice(const std::string& devId, int32_t flag)
{
    std::lock_guard<ffrt::ffrt> lock(dcallClientMtx_);
    if (dcallClient_ == nullptr) {
        TELEPHONY_LOGE("dcallClient_ is nullptr");
        return TELEPHONY_ERR_FAIL;
    }
    return dcallClient_->SwitchDevice(devId, flag);
}

int32_t DistributedCallProxy::GetOnlineDeviceList(std::vector<std::string>& devList)
{
    std::lock_guard<ffrt::ffrt> lock(dcallClientMtx_);
    if (dcallClient_ == nullptr) {
        TELEPHONY_LOGE("dcallClient_ is nullptr");
        return TELEPHONY_ERR_FAIL;
    }
    return dcallClient_->GetOnlineDeviceList(devList);
}

int32_t DistributedCallProxy::RegisterDeviceCallback(const std::string name,
    const std::shared_ptr<OHOS::DistributedHardware::IDCallDeviceCallback>& callback)
{
    std::lock_guard<ffrt::ffrt> lock(dcallClientMtx_);
    if (dcallClient_ == nullptr) {
        TELEPHONY_LOGE("dcallClient_ is nullptr");
        return TELEPHONY_ERR_FAIL;
    }
    return dcallClient_->RegisterDeviceCallback(name, callback);
}

int32_t DistributedCallProxy::UnRegisterDeviceCallback(const std::string& name)
{
    std::lock_guard<ffrt::ffrt> lock(dcallClientMtx_);
    if (dcallClient_ == nullptr) {
        TELEPHONY_LOGE("dcallClient_ is nullptr");
        return TELEPHONY_ERR_FAIL;
    }
    return dcallClient_->UnRegisterDeviceCallback(name);
}

int32_t DistributedCallProxy::GetDCallDeviceInfo(const std::string &devId,
    OHOS::DistributedHardware::DCallDeviceInfo& devInfo)
{
    std::lock_guard<ffrt::ffrt> lock(dcallClientMtx_);
    if (dcallClient_ == nullptr) {
        TELEPHONY_LOGE("dcallClient_ is nullptr");
        return TELEPHONY_ERR_FAIL;
    }
    return dcallClient_->GetDCallDeviceInfo(devId, devInfo);
}

bool DistributedCallProxy::IsSelectVirtualModem()
{
    std::lock_guard<ffrt::ffrt> lock(dcallClientMtx_);
    if (dcallClient_ == nullptr) {
        TELEPHONY_LOGE("dcallClient_ is nullptr");
        return false;
    }
    return dcallClient_->IsSelectVirtualModem();
}
} // namespace Telephony
} // namespace OHOS
