/*
 * Copyright (C) 2021-2023 Huawei Device Co., Ltd.
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

#ifndef CALL_MANAGER_CONNECT_H
#define CALL_MANAGER_CONNECT_H
#include <string_ex.h>
#include <list>
#include <map>
#include <mutex>
#include <string>
#include <unordered_set>

#include "accesstoken_kit.h"
#include "call_manager_client.h"
#include "call_manager_errors.h"
#include "call_manager_service_proxy.h"
#include "native_call_manager_hisysevent.h"
#include "i_call_ability_callback.h"
#include "if_system_ability_manager.h"
#include "iremote_object.h"
#include "iremote_stub.h"
#include "iservice_registry.h"
#include "pac_map.h"
#include "refbase.h"
#include "rwlock.h"
#include "surface_utils.h"
#include "system_ability.h"
#include "system_ability_definition.h"
#include "telephony_log_wrapper.h"
#include "token_setproc.h"

namespace OHOS {
namespace Telephony {
const int32_t MAX_LEN = 100000;
using namespace Security::AccessToken;
using Security::AccessToken::AccessTokenID;

inline HapInfoParams testInfoParams = {
    .bundleName = "tel_call_manager_gtest",
    .userID = 1,
    .instIndex = 0,
    .appIDDesc = "test",
    .isSystemApp = true,
};

inline HapInfoParams testInfoParamsfalse = {
    .bundleName = "tel_call_manager_gtest",
    .userID = 1,
    .instIndex = 0,
    .appIDDesc = "test",
    .isSystemApp = false,
};

inline PermissionDef testPermPlaceCallDef = {
    .permissionName = "ohos.permission.PLACE_CALL",
    .bundleName = "tel_call_manager_gtest",
    .grantMode = 1, // SYSTEM_GRANT
    .label = "label",
    .labelId = 1,
    .description = "Test call manager",
    .descriptionId = 1,
    .availableLevel = APL_SYSTEM_BASIC,
};

inline PermissionStateFull testPlaceCallState = {
    .grantFlags = { 2 }, // PERMISSION_USER_SET
    .grantStatus = { PermissionState::PERMISSION_GRANTED },
    .isGeneral = true,
    .permissionName = "ohos.permission.PLACE_CALL",
    .resDeviceID = { "local" },
};

inline PermissionDef testPermSetTelephonyStateDef = {
    .permissionName = "ohos.permission.SET_TELEPHONY_STATE",
    .bundleName = "tel_call_manager_gtest",
    .grantMode = 1, // SYSTEM_GRANT
    .label = "label",
    .labelId = 1,
    .description = "Test call manager",
    .descriptionId = 1,
    .availableLevel = APL_SYSTEM_BASIC,
};

inline PermissionStateFull testSetTelephonyState = {
    .grantFlags = { 2 }, // PERMISSION_USER_SET
    .grantStatus = { PermissionState::PERMISSION_GRANTED },
    .isGeneral = true,
    .permissionName = "ohos.permission.SET_TELEPHONY_STATE",
    .resDeviceID = { "local" },
};

inline PermissionDef testPermGetTelephonyStateDef = {
    .permissionName = "ohos.permission.GET_TELEPHONY_STATE",
    .bundleName = "tel_call_manager_gtest",
    .grantMode = 1, // SYSTEM_GRANT
    .label = "label",
    .labelId = 1,
    .description = "Test call manager",
    .descriptionId = 1,
    .availableLevel = APL_SYSTEM_BASIC,
};

inline PermissionStateFull testGetTelephonyState = {
    .grantFlags = { 2 }, // PERMISSION_USER_SET
    .grantStatus = { PermissionState::PERMISSION_GRANTED },
    .isGeneral = true,
    .permissionName = "ohos.permission.GET_TELEPHONY_STATE",
    .resDeviceID = { "local" },
};

inline PermissionDef testPermAnswerCallDef = {
    .permissionName = "ohos.permission.ANSWER_CALL",
    .bundleName = "tel_call_manager_gtest",
    .grantMode = 1, // SYSTEM_GRANT
    .label = "label",
    .labelId = 1,
    .description = "Test call manager",
    .descriptionId = 1,
    .availableLevel = APL_SYSTEM_BASIC,
};

inline PermissionStateFull testAnswerCallState = {
    .grantFlags = { 2 }, // PERMISSION_USER_SET
    .grantStatus = { PermissionState::PERMISSION_GRANTED },
    .isGeneral = true,
    .permissionName = "ohos.permission.ANSWER_CALL",
    .resDeviceID = { "local" },
};

inline PermissionDef testReadCallLogDef = {
    .permissionName = "ohos.permission.READ_CALL_LOG",
    .bundleName = "tel_call_manager_gtest",
    .grantMode = 1, // SYSTEM_GRANT
    .label = "label",
    .labelId = 1,
    .description = "Test call manager",
    .descriptionId = 1,
    .availableLevel = APL_SYSTEM_BASIC,
};

inline PermissionStateFull testPermReadCallLog = {
    .grantFlags = { 2 }, // PERMISSION_USER_SET
    .grantStatus = { PermissionState::PERMISSION_GRANTED },
    .isGeneral = true,
    .permissionName = "ohos.permission.READ_CALL_LOG",
    .resDeviceID = { "local" },
};

inline PermissionDef testWriteCallLogDef = {
    .permissionName = "ohos.permission.WRITE_CALL_LOG",
    .bundleName = "tel_call_manager_gtest",
    .grantMode = 1, // SYSTEM_GRANT
    .label = "label",
    .labelId = 1,
    .description = "Test call manager",
    .descriptionId = 1,
    .availableLevel = APL_SYSTEM_BASIC,
};

inline PermissionStateFull testPermWriteCallLog = {
    .grantFlags = { 2 }, // PERMISSION_USER_SET
    .grantStatus = { PermissionState::PERMISSION_GRANTED },
    .isGeneral = true,
    .permissionName = "ohos.permission.WRITE_CALL_LOG",
    .resDeviceID = { "local" },
};

inline PermissionDef testStartAbilityFromBGDef = {
    .permissionName = "ohos.permission.START_ABILITIES_FROM_BACKGROUND",
    .bundleName = "tel_call_manager_gtest",
    .grantMode = 1, // SYSTEM_GRANT
    .label = "label",
    .labelId = 1,
    .description = "Test call manager",
    .descriptionId = 1,
    .availableLevel = APL_SYSTEM_BASIC,
};

inline PermissionStateFull testPermStartAbilityFromBG = {
    .grantFlags = { 2 }, // PERMISSION_USER_SET
    .grantStatus = { PermissionState::PERMISSION_GRANTED },
    .isGeneral = true,
    .permissionName = "ohos.permission.START_ABILITIES_FROM_BACKGROUND",
    .resDeviceID = { "local" },
};

inline PermissionDef testConCellularCallDef = {
    .permissionName = "ohos.permission.CONNECT_CELLULAR_CALL_SERVICE",
    .bundleName = "tel_cellular_call_cs_gtest",
    .grantMode = 1, // SYSTEM_GRANT
    .label = "label",
    .labelId = 1,
    .description = "Test cellular call",
    .descriptionId = 1,
    .availableLevel = APL_SYSTEM_BASIC,
};

inline PermissionStateFull testPermConCellularCall = {
    .grantFlags = { 2 }, // PERMISSION_USER_SET
    .grantStatus = { PermissionState::PERMISSION_GRANTED },
    .isGeneral = true,
    .permissionName = "ohos.permission.CONNECT_CELLULAR_CALL_SERVICE",
    .resDeviceID = { "local" },
};

inline PermissionStateFull addPermissonManagerLocalAccounts = {
    .permissionName = "ohos.permission.MANAGER_LOCAL_ACCOUNTS",
    .isGeneral = true,
    .resDeviceID = { "local" },
    .grantStatus = { PermissionState::PERMISSION_GRANTED },
    .grantFlags = { 1 },
};

inline HapPolicyParams testPolicyParams = {
    .apl = APL_SYSTEM_BASIC,
    .domain = "test.domain",
    .permList = { testPermPlaceCallDef, testPermSetTelephonyStateDef, testPermGetTelephonyStateDef,
        testPermAnswerCallDef, testReadCallLogDef, testWriteCallLogDef, testStartAbilityFromBGDef,
        testConCellularCallDef },
    .permStateList = { testPlaceCallState, testSetTelephonyState, testGetTelephonyState, testAnswerCallState,
        testPermReadCallLog, testPermWriteCallLog, testPermStartAbilityFromBG, testPermConCellularCall,
        addPermissonManagerLocalAccounts },
};

class AccessToken {
public:
    AccessToken()
    {
        currentID_ = GetSelfTokenID();
        AccessTokenIDEx tokenIdEx = AccessTokenKit::AllocHapToken(testInfoParams, testPolicyParams);
        accessID_ = tokenIdEx.tokenIdExStruct.tokenID;
        SetSelfTokenID(tokenIdEx.tokenIDEx);
    }
    ~AccessToken()
    {
        AccessTokenKit::DeleteToken(accessID_);
        SetSelfTokenID(currentID_);
    }
private:
    AccessTokenID currentID_ = 0;
    AccessTokenID accessID_ = 0;
};

class AccessFalseToken {
public:
    AccessFalseToken()
    {
        currentID_ = GetSelfTokenID();
        AccessTokenIDEx tokenIdEx = AccessTokenKit::AllocHapToken(testInfoParamsfalse, testPolicyParams);
        accessID_ = tokenIdEx.tokenIdExStruct.tokenID;
        SetSelfTokenID(tokenIdEx.tokenIDEx);
    }
    ~AccessFalseToken()
    {
        AccessTokenKit::DeleteToken(accessID_);
        SetSelfTokenID(currentID_);
    }
private:
    AccessTokenID currentID_ = 0;
    AccessTokenID accessID_ = 0;
};
} // namespace Telephony
} // namespace OHOS

#endif
