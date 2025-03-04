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
 
#include "anonymize_adapter.h"

#include "telephony_log_wrapper.h"

namespace OHOS {
namespace Telephony {
AnonymizeAdapter::AnonymizeAdapter()
{}

AnonymizeAdapter::~AnonymizeAdapter()
{}

void *AnonymizeAdapter::GetLibAnonymize()
{
    if (libAnonymize_ != nullptr) {
        return libAnonymize_;
    }

    libAnonymize_ = dlopen("libdia_sdk.z.so", RTLD_LAZY);
    if (libAnonymize_ == nullptr) {
        TELEPHONY_LOGE("libAnonymize_ is null");
    }

    return libAnonymize_;
}

void AnonymizeAdapter::ReleaseLibAnonymize()
{
    TELEPHONY_LOGI("ReleaseLibAnonymize");
    if (libAnonymize_ != nullptr) {
        dlclose(libAnonymize_);
        libAnonymize_ = nullptr;
    }
}

int AnonymizeAdapter::InitConfig(void **config)
{
    libAnonymize_ = GetLibAnonymize();
    if (libAnonymize_ == nullptr) {
        return -1;
    }

    PfnInitConfig func = reinterpret_cast<PfnInitConfig>(dlsym(libAnonymize_, "DIA_InitConfig"));
    if (func == nullptr) {
        TELEPHONY_LOGE("func is null");
        return -1;
    }

    return func(config);
}

int AnonymizeAdapter::SetRule(void *config, const DIA_String *key, const DIA_Rule_C *value)
{
    libAnonymize_ = GetLibAnonymize();
    if (libAnonymize_ == nullptr) {
        return -1;
    }

    PfnSetRule func = reinterpret_cast<PfnSetRule>(dlsym(libAnonymize_, "DIA_SetRule"));
    if (func == nullptr) {
        TELEPHONY_LOGE("func is null");
        return -1;
    }

    return func(config, key, value);
}

int AnonymizeAdapter::CreateAnonymize(void *config, void **anonymize)
{
    libAnonymize_ = GetLibAnonymize();
    if (libAnonymize_ == nullptr) {
        return -1;
    }

    PfnCreateAnonymize func = reinterpret_cast<PfnCreateAnonymize>(dlsym(libAnonymize_, "DIA_CreateAnonymize"));
    if (func == nullptr) {
        TELEPHONY_LOGE("func is null");
        return -1;
    }

    return func(config, anonymize);
}

int AnonymizeAdapter::IdentifyAnonymize(void *anonymize, const DIA_String *input, DIA_String **output)
{
    libAnonymize_ = GetLibAnonymize();
    if (libAnonymize_ == nullptr) {
        return -1;
    }

    PfnIdentifyAnonymize func = reinterpret_cast<PfnIdentifyAnonymize>(dlsym(libAnonymize_, "DIA_IdentifyAnonymize"));
    if (func == nullptr) {
        TELEPHONY_LOGE("func is null");
        return -1;
    }

    return func(anonymize, input, output);
}

int AnonymizeAdapter::ReleaseConfig(void **config)
{
    libAnonymize_ = GetLibAnonymize();
    if (libAnonymize_ == nullptr) {
        return -1;
    }

    PfnReleaseConfig func = reinterpret_cast<PfnReleaseConfig>(dlsym(libAnonymize_, "DIA_ReleaseConfig"));
    if (func == nullptr) {
        TELEPHONY_LOGE("func is null");
        return -1;
    }

    return func(config);
}

int AnonymizeAdapter::ReleaseAnonymize(void **anonymize)
{
    libAnonymize_ = GetLibAnonymize();
    if (libAnonymize_ == nullptr) {
        return -1;
    }

    PfnReleaseAonoymize func = reinterpret_cast<PfnReleaseAonoymize>(dlsym(libAnonymize_, "DIA_ReleaseAnonymize"));
    if (func == nullptr) {
        TELEPHONY_LOGE("func is null");
        return -1;
    }

    return func(anonymize);
}

int AnonymizeAdapter::ReleaseOutputData(DIA_String **output)
{
    libAnonymize_ = GetLibAnonymize();
    if (libAnonymize_ == nullptr) {
        return -1;
    }

    PfnReleaseOutputData func = reinterpret_cast<PfnReleaseOutputData>(dlsym(libAnonymize_, "DIA_ReleaseOutputData"));
    if (func == nullptr) {
        TELEPHONY_LOGE("func is null");
        return -1;
    }

    return func(output);
}
}
}