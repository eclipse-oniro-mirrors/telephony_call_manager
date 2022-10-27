/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include "addcalltoken_fuzzer.h"

#include <iostream>

#include "nativetoken_kit.h"
#include "token_setproc.h"

namespace OHOS {
AddCallTokenFuzzer::AddCallTokenFuzzer()
{
    const char **perms = new const char *[4];
    perms[0] = "ohos.permission.PLACE_CALL";
    perms[1] = "ohos.permission.ANSWER_CALL";
    perms[2] = "ohos.permission.SET_TELEPHONY_STATE";
    perms[3] = "ohos.permission.GET_TELEPHONY_STATE";

    NativeTokenInfoParams testCallInfoParams = {
        .dcapsNum = 0,
        .permsNum = 4,
        .aclsNum = 0,
        .dcaps = nullptr,
        .perms = perms,
        .acls = nullptr,
        .processName = "call_fuzzer",
        .aplStr = "system_basic",
    };
    currentID_ = GetAccessTokenId(&testCallInfoParams);
    std::cout << "AddCallTokenFuzzer currentID_ : " << currentID_ << std::endl;
    SetSelfTokenID(currentID_);
    Security::AccessToken::AccessTokenKit::ReloadNativeTokenInfo();
}
AddCallTokenFuzzer::~AddCallTokenFuzzer()
{
    std::cout << "AddCallTokenFuzzer ~AddCallTokenFuzzer" << std::endl;
}
} // namespace OHOS