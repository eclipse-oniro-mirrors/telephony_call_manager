/*
 * Copyright (C) 2021 Huawei Device Co., Ltd.
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

#ifndef NAPI_CALL_H
#define NAPI_CALL_H
#include <codecvt>
#include <initializer_list>
#include <locale>
#include <string>

#include "napi/native_api.h"
#include "napi/native_node_api.h"
namespace OHOS {
namespace TelephonyNapi {
#define MEMORY_CLEAR 0
constexpr int32_t DEFAULT_ERROR = -1;
constexpr int RESOLVED = 1;
constexpr int REJECT = 0;
constexpr int MINUS_ONE = -1;
constexpr int ZERO = 0;
constexpr int ONE = 1;
constexpr int TWO = 2;
constexpr int THREE = 3;
constexpr int THIRTYTWO = 32;
constexpr size_t SIZE_T_ONE = 1;
constexpr size_t BUF_SIZE = 31;

constexpr size_t NONE_PARAMETER = 0;
constexpr size_t ONE_PARAMETER = 1;
constexpr size_t TWO_PARAMETER = 2;
constexpr size_t THREE_PARAMETER = 3;

constexpr int INDEX_ZERO = 0;
constexpr int INDEX_ONE = 1;
constexpr int INDEX_TWO = 2;

constexpr const int MAXSIZE = 1024;

struct AsyncContext {
    napi_env env;
    char number[THIRTYTWO];
    size_t numberLen;
    napi_value value[THREE];
    size_t valueLen;
    int32_t callId;
    napi_async_work work;
    napi_deferred deferred;
    napi_ref callbackRef;
    int status;
};

enum CallState {
    /**
     * Indicates an invalid state, which is used when the call state fails to be obtained.
     */
    CALL_STATE_UNKNOWN = -1,

    /**
     * Indicates that there is no ongoing call.
     */
    CALL_STATE_IDLE = 0,

    /**
     * Indicates that an incoming call is ringing or waiting.
     */
    CALL_STATE_RINGING = 1,

    /**
     * Indicates that a least one call is in the dialing, active, or hold state, and there is no new incoming call
     * ringing or waiting.
     */
    CALL_STATE_OFFHOOK = 2
};
} // namespace TelephonyNapi
} // namespace OHOS
#endif // NAPI_CALL_H