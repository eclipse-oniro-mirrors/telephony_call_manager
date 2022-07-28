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

#ifndef CALL_MANAGER_TEST_TYPES_H
#define CALL_MANAGER_TEST_TYPES_H

namespace OHOS {
namespace Telephony {
constexpr int16_t MIN_VOLUME = 0;
constexpr int16_t MAX_VOLUME = 15;
constexpr int16_t READ_SIZE = 1;
constexpr int16_t MIN_BYTES = 4;
constexpr int16_t RING_PATH_MAX_LENGTH = 100;
constexpr int16_t SIM1_SLOTID = 0;
constexpr int16_t DEFAULT_ACCOUNT_ID = 0;
constexpr int16_t DEFAULT_VIDEO_STATE = 0;
constexpr int16_t DEFAULT_DIAL_SCENE = 0;
constexpr int16_t DEFAULT_DIAL_TYPE = 0;
constexpr int16_t DEFAULT_CALL_TYPE = 0;
constexpr int16_t DEFAULT_CALL_ID = 0;
constexpr int16_t DEFAULT_VALUE = 0;
constexpr size_t DEFAULT_SIZE = 0;
constexpr int16_t WINDOWS_X_START = 0;
constexpr int16_t WINDOWS_Y_START = 0;
constexpr int16_t WINDOWS_Z_ERROR = -1;
constexpr int16_t WINDOWS_Z_TOP = 1;
constexpr int16_t WINDOWS_Z_BOTTOM = 0;
constexpr int16_t WINDOWS_WIDTH = 200;
constexpr int16_t WINDOWS_HEIGHT = 200;
constexpr size_t DEFAULT_PREFERENCEMODE = 3;
constexpr int16_t EVENT_BLUETOOTH_SCO_CONNECTED_CODE = 0;
constexpr int16_t EVENT_BLUETOOTH_SCO_DISCONNECTED_CODE = 1;
constexpr size_t DEFAULT_NET_TYPE = 0;
constexpr size_t DEFAULT_ITEM_VALUE = 0;
const int32_t DEFINE_INIT_PERMISSIONS = 93;
const int32_t DEFINE_VERIFY_PERMISSIONS = 94;
const int32_t DEFINE_CONNECT_BT_SCO = 95;
const int32_t DEFINE_DISCONNECT_BT_SCO = 96;
const int32_t DEFINE_SUBSCRIBERCOMMON_EVENT = 97;
} // namespace Telephony
} // namespace OHOS
#endif