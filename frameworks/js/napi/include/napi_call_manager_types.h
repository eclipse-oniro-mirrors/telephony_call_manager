/*
 * Copyright (C) 2021-2022 Huawei Device Co., Ltd.
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

#ifndef NATIVE_COMMON_TYPE_H
#define NATIVE_COMMON_TYPE_H

#include "call_manager_inner_type.h"
#include "napi/native_api.h"
#include "napi/native_node_api.h"
#include "pac_map.h"
#include "telephony_errors.h"

namespace OHOS {
namespace Telephony {
const int16_t BOOL_VALUE_IS_TRUE = 1;
const int16_t NATIVE_VERSION = 1;
const int16_t NATIVE_FLAGS = 0;
const int16_t ZERO_VALUE = 0;
const int16_t ONLY_ONE_VALUE = 1;
const int16_t TWO_VALUE_LIMIT = 2;
const int16_t VALUE_MAXIMUM_LIMIT = 3;
const int16_t THREE_VALUE_MAXIMUM_LIMIT = 3;
const int16_t FOUR_VALUE_MAXIMUM_LIMIT = 4;
const int16_t FIVE_VALUE_MAXIMUM_LIMIT = 5;
const int16_t ARRAY_INDEX_FIRST = 0;
const int16_t ARRAY_INDEX_SECOND = 1;
const int16_t ARRAY_INDEX_THIRD = 2;
const int16_t ARRAY_INDEX_FOURTH = 3;
const int16_t DATA_LENGTH_ONE = 1;
const int16_t DATA_LENGTH_TWO = 2;
const int16_t DTMF_DEFAULT_OFF = 10;
const int16_t PHONE_NUMBER_MAXIMUM_LIMIT = 31;
const int16_t MESSAGE_CONTENT_MAXIMUM_LIMIT = 160;
const int16_t NAPI_MAX_TIMEOUT_SECOND = 10;
const int16_t UNKNOWN_EVENT = 0;

struct AsyncContext {
    virtual ~AsyncContext() {}
    napi_env env;
    napi_async_work work;
    napi_deferred deferred;
    napi_ref callbackRef;
    int32_t callId;
    int32_t resolved = TELEPHONY_ERROR;
    char number[kMaxNumberLen + 1];
    size_t numberLen;
    napi_value value[VALUE_MAXIMUM_LIMIT];
    size_t valueLen;
    int32_t errorCode = TELEPHONY_SUCCESS;
    int32_t eventId = UNKNOWN_EVENT;
};

struct DialAsyncContext : AsyncContext {
    int32_t accountId;
    int32_t videoState;
    int32_t dialScene;
    int32_t dialType;
};

struct AnswerAsyncContext : AsyncContext {
    int32_t videoState;
};

struct BoolResultAsyncContext : AsyncContext {
    bool enabled;
};

struct IntResultAsyncContext : AsyncContext {
    int32_t result;
};

struct RejectAsyncContext : AsyncContext {
    bool isSendSms;
    std::string messageContent;
};

struct ListAsyncContext : AsyncContext {
    std::vector<std::u16string> listResult;
};

struct SupplementAsyncContext : AsyncContext {
    int32_t slotId;
    int32_t type;
    int32_t mode;
    std::string content;
    bool flag;
    napi_ref thisVar;
};

struct ImsSwitchAsyncContext : AsyncContext {
    int32_t slotId;
    bool enabled;
};

struct CallRestrictionAsyncContext : AsyncContext {
    int32_t slotId;
    CallRestrictionInfo info;
    napi_ref thisVar;
};

struct CallTransferAsyncContext : AsyncContext {
    int32_t slotId;
    CallTransferInfo info;
    napi_ref thisVar;
};

struct UtilsAsyncContext : AsyncContext {
    int32_t slotId;
    std::u16string formatNumber;
    std::string code;
};

struct EventCallback {
    EventCallback() : env(nullptr), thisVar(nullptr), callbackRef(nullptr), deferred(nullptr),
                      callbackBeginTime_(0) {}
    napi_env env;
    napi_ref thisVar;
    napi_ref callbackRef;
    napi_deferred deferred;
    time_t callbackBeginTime_;
};

struct AudioAsyncContext : AsyncContext {
    bool isMute;
    std::string address;
    int32_t audioDevice;
};

struct VideoAsyncContext : AsyncContext {
    int32_t x;
    int32_t y;
    int32_t z;
    int32_t width;
    int32_t height;
    double zoomRatio;
    int32_t rotation;
    int32_t callingUid;
    int32_t callingPid;
    std::string cameraId;
    std::u16string callingPackage;
    char path[kMaxNumberLen + 1];
};

struct OttCallAsyncContext : AsyncContext {
    std::vector<OttCallDetailsInfo> ottVec;
};

struct OttEventAsyncContext : AsyncContext {
    OttCallEventInfo eventInfo;
};

enum CallWaitingStatus {
    CALL_WAITING_DISABLE = 0,
    CALL_WAITING_ENABLE,
};

enum RestrictionStatus {
    RESTRICTION_DISABLE = 0,
    RESTRICTION_ENABLE,
};

enum TransferStatus {
    TRANSFER_DISABLE = 0,
    TRANSFER_ENABLE = 1,
};

struct CallStateWorker {
    CallAttributeInfo info;
    EventCallback callback;
};

struct CallEventWorker {
    CallEventInfo info;
    EventCallback callback;
};

struct CallSupplementWorker {
    AppExecFwk::PacMap info;
    EventCallback callback;
};

struct CallDisconnectedCauseWorker {
    DisconnectedDetails details;
    EventCallback callback;
};

struct CallOttWorker {
    OttCallRequestId requestId;
    AppExecFwk::PacMap info;
    EventCallback callback;
};

struct MmiCodeWorker {
    MmiCodeInfo info;
    EventCallback callback;
};
} // namespace Telephony
} // namespace OHOS

#endif // NAPI_CALL_MANAGER_TYPES_H
