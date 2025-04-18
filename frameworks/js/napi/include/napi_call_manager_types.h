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
#include "want_params.h"

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
const int16_t PHONE_NUMBER_MAXIMUM_LIMIT = 255;
const int16_t MESSAGE_CONTENT_MAXIMUM_LIMIT = 160;
const int16_t NAPI_MAX_TIMEOUT_SECOND = 10;
const int16_t UNKNOWN_EVENT = 0;
const int16_t USSD_CONTENT_MAX_LEN = 255;

struct AsyncContext {
    virtual ~AsyncContext() {}
    napi_env env = nullptr;
    napi_async_work work = nullptr;
    napi_deferred deferred = nullptr;
    napi_ref callbackRef = nullptr;
    int32_t callId = 0;
    int32_t resolved = TELEPHONY_ERROR;
    char number[kMaxNumberLen + 1] = { 0 };
    size_t numberLen = 0;
    napi_value value[VALUE_MAXIMUM_LIMIT] = { 0 };
    size_t valueLen = 0;
    int32_t errorCode = TELEPHONY_SUCCESS;
    int32_t eventId = UNKNOWN_EVENT;
};

struct DialAsyncContext : AsyncContext {
    int32_t accountId = -1;
    int32_t videoState = 0;
    int32_t dialScene = 0;
    int32_t dialType = 0;
    AAFwk::WantParams extraParams;
};

struct UssdAsyncContext : AsyncContext {
    int32_t slotId = 0;
    char content[USSD_CONTENT_MAX_LEN] = { 0 };
};

struct AnswerAsyncContext : AsyncContext {
    int32_t videoState = 0;
};

struct BoolResultAsyncContext : AsyncContext {
    int32_t slotId = 0;
    bool enabled = false;
};

struct IntResultAsyncContext : AsyncContext {
    int32_t result = 0;
};

struct RejectAsyncContext : AsyncContext {
    bool isSendSms = false;
    std::string messageContent = "";
};

struct ListAsyncContext : AsyncContext {
    std::vector<std::u16string> listResult {};
};

struct SupplementAsyncContext : AsyncContext {
    int32_t slotId = 0;
    int32_t type = 0;
    int32_t mode = 0;
    std::string content = "";
    bool flag = false;
    napi_ref thisVar = nullptr;
};

struct ImsSwitchAsyncContext : AsyncContext {
    int32_t slotId = 0;
    bool enabled = false;
};

struct VoNRSwitchAsyncContext : AsyncContext {
    int32_t slotId = 0;
    int32_t state = 0;
};

struct CallRestrictionAsyncContext : AsyncContext {
    int32_t slotId = 0;
    CallRestrictionInfo info;
    napi_ref thisVar = nullptr;
};

struct CallBarringPasswordAsyncContext : AsyncContext {
    int32_t slotId = 0;
    char oldPassword[kMaxNumberLen + 1] = { 0 };
    char newPassword[kMaxNumberLen + 1] = { 0 };
    napi_ref thisVar = nullptr;
};

struct CallTransferAsyncContext : AsyncContext {
    int32_t slotId = 0;
    CallTransferInfo info;
    napi_ref thisVar = nullptr;
};

struct UtilsAsyncContext : AsyncContext {
    int32_t slotId = 0;
    bool enabled = false;
    std::u16string formatNumber = u"";
    std::string code = "";
};

struct EventCallback {
    EventCallback() : env(nullptr), thisVar(nullptr), callbackRef(nullptr), deferred(nullptr),
                      callbackBeginTime_(0) {}
    napi_env env = nullptr;
    napi_ref thisVar = nullptr;
    napi_ref callbackRef = nullptr;
    napi_deferred deferred = nullptr;
    time_t callbackBeginTime_;
};

struct AudioAsyncContext : AsyncContext {
    bool isMute = false;
    std::string address = "";
    int32_t deviceType = 0;
};

struct VideoAsyncContext : AsyncContext {
    double zoomRatio = 0;
    int32_t rotation = 0;
    int32_t callingUid = 0;
    int32_t callingPid = 0;
    std::string cameraId = "";
    std::string surfaceId = "";
    std::string picturePath = "";
    std::u16string callingPackage = u"";
    char path[kMaxNumberLen + 1] = { 0 };
};

struct OttCallAsyncContext : AsyncContext {
    std::vector<OttCallDetailsInfo> ottVec {};
};

struct OttEventAsyncContext : AsyncContext {
    OttCallEventInfo eventInfo;
};

struct PostDialAsyncContext : AsyncContext {
    bool proceed = false;
};

struct VoIPCallStateAsyncContext : AsyncContext {
    int32_t state = 0;
};

struct VoIPCallInfoAsyncContext : AsyncContext {
    int32_t callId = -1;
    int32_t state = 0;
    std::string phoneNumber = "";
};
struct SendCallUiEventAsyncContext : AsyncContext {
    std::string eventName = "";
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

struct MeeTimeStateWorker {
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

struct AudioDeviceWork {
    AudioDeviceInfo info;
    EventCallback callback;
};

struct PostDialDelayWorker {
    std::string postDialStr;
    EventCallback callback;
};

struct ImsCallModeInfoWorker {
    CallMediaModeInfo callModeInfo;
    EventCallback callback;
};

struct CallSessionEventWorker {
    CallSessionEvent sessionEvent;
    EventCallback callback;
};

struct PeerDimensionsWorker {
    PeerDimensionsDetail peerDimensionsDetail;
    EventCallback callback;
};

struct CallDataUsageWorker {
    int64_t callDataUsage;
    EventCallback callback;
};

struct CameraCapbilitiesWorker {
    CameraCapabilities cameraCapabilities;
    EventCallback callback;
};
} // namespace Telephony
} // namespace OHOS

#endif // NAPI_CALL_MANAGER_TYPES_H
