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
#include "i_call_ability_callback.h"
#include "if_system_ability_manager.h"
#include "iremote_object.h"
#include "iremote_stub.h"
#include "iservice_registry.h"
#include "pac_map.h"
#include "refbase.h"
#include "ffrt.h"
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

inline HapPolicyParams testPolicyParams = {
    .apl = APL_SYSTEM_BASIC,
    .domain = "test.domain",
    .permList = { testPermPlaceCallDef, testPermSetTelephonyStateDef, testPermGetTelephonyStateDef,
        testPermAnswerCallDef, testReadCallLogDef, testWriteCallLogDef, testStartAbilityFromBGDef,
        testConCellularCallDef },
    .permStateList = { testPlaceCallState, testSetTelephonyState, testGetTelephonyState, testAnswerCallState,
        testPermReadCallLog, testPermWriteCallLog, testPermStartAbilityFromBG, testPermConCellularCall },
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

class CallInfoManager {
public:
    static int32_t CallDetailsChange(const CallAttributeInfo &info);
    static int32_t MeeTimeDetailsChange(const CallAttributeInfo &info);
    static int32_t CallEventChange(const CallEventInfo &info);
    static void Init();
    // replace LOCK_NUM_WHILE_EQ and LOCK_NUM_WHILE_NE
    static void LockCallId(bool eq, int32_t originVal, int32_t slipMs, int32_t timeoutMs);
    static void LockCallState(bool eq, int32_t originVal, int32_t slipMs, int32_t timeoutMs);
    static bool HasState(int32_t callId, int32_t callState);
    static bool HasActiveStatus();

private:
    static std::mutex mutex_;
    static int16_t newCallState_;
    static CallAttributeInfo updateCallInfo_;
    static std::unordered_set<int32_t> callIdSet_;
};

class CallAbilityCallbackStub : public IRemoteStub<ICallAbilityCallback> {
public:
    CallAbilityCallbackStub()
    {
        memberFuncMap_[static_cast<uint32_t>(CallManagerCallAbilityInterfaceCode::UPDATE_CALL_STATE_INFO)] =
            &CallAbilityCallbackStub::OnUpdateCallStateInfoRequest;
        memberFuncMap_[static_cast<uint32_t>(CallManagerCallAbilityInterfaceCode::UPDATE_CALL_EVENT)] =
            &CallAbilityCallbackStub::OnUpdateCallEventRequest;
        memberFuncMap_[static_cast<uint32_t>(CallManagerCallAbilityInterfaceCode::UPDATE_CALL_ASYNC_RESULT_REQUEST)] =
            &CallAbilityCallbackStub::OnUpdateAsyncResultRequest;
    }

    ~CallAbilityCallbackStub()
    {
        memberFuncMap_.clear();
    }

    int32_t OnRemoteRequest(uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option)
    {
        std::u16string myDesc = CallAbilityCallbackStub::GetDescriptor();
        std::u16string remoteDesc = data.ReadInterfaceToken();
        if (myDesc != remoteDesc) {
            TELEPHONY_LOGE("descriptor checked failed");
            return TELEPHONY_ERR_DESCRIPTOR_MISMATCH;
        }
        TELEPHONY_LOGI("OnReceived, code = %{public}u", code);
        auto itFunction = memberFuncMap_.find(code);
        if (itFunction != memberFuncMap_.end()) {
            auto memberFunc = itFunction->second;
            if (memberFunc != nullptr) {
                return (this->*memberFunc)(data, reply);
            }
        }
        return IPCObjectStub::OnRemoteRequest(code, data, reply, option);
    }

    int32_t OnCallDetailsChange(const CallAttributeInfo &info)
    {
        TELEPHONY_LOGI("UCallDetailsChange callA!");
        return CallInfoManager::CallDetailsChange(info);
    }

    int32_t OnMeeTimeDetailsChange(const CallAttributeInfo &info)
    {
        TELEPHONY_LOGI("UMeeTimeDetailsChange callA!");
        return CallInfoManager::MeeTimeDetailsChange(info);
    }

    int32_t OnCallEventChange(const CallEventInfo &info)
    {
        TELEPHONY_LOGI("UpdateCallEvent success!");
        return CallInfoManager::CallEventChange(info);
    }

    int32_t OnCallDisconnectedCause(const DisconnectedDetails &details)
    {
        TELEPHONY_LOGI("OnCallDisconnectedCause success!");
        return TELEPHONY_SUCCESS;
    }

    int32_t OnReportAsyncResults(CallResultReportId reportId, AppExecFwk::PacMap &resultInfo)
    {
        TELEPHONY_LOGI("OnReportAsyncResults success!");
        return TELEPHONY_SUCCESS;
    }

    int32_t OnReportMmiCodeResult(const MmiCodeInfo &info)
    {
        TELEPHONY_LOGI("OnReportMmiCodeResult success!");
        return TELEPHONY_SUCCESS;
    }

    int32_t OnOttCallRequest(OttCallRequestId requestId, AppExecFwk::PacMap &info)
    {
        TELEPHONY_LOGI("OnOttCallRequest success!");
        return TELEPHONY_SUCCESS;
    }

    int32_t OnReportAudioDeviceChange(const AudioDeviceInfo &info)
    {
        TELEPHONY_LOGI("OnReportAudioDeviceChange success!");
        return TELEPHONY_SUCCESS;
    }

    int32_t OnReportPostDialDelay(const std::string &str)
    {
        TELEPHONY_LOGI("OnReportPostDialDelay success!");
        return TELEPHONY_SUCCESS;
    }

    int32_t OnReportImsCallModeChange(const CallMediaModeInfo &imsCallModeInfo)
    {
        TELEPHONY_LOGI("OnReportImsCallModeChange success!");
        return TELEPHONY_SUCCESS;
    }

    int32_t OnReportCallSessionEventChange(const CallSessionEvent &callSessionEventOptions)
    {
        TELEPHONY_LOGI("OnReportCallSessionEventChange success!");
        return TELEPHONY_SUCCESS;
    }

    int32_t OnReportPeerDimensionsChange(const PeerDimensionsDetail &peerDimensionsDetail)
    {
        TELEPHONY_LOGI("OnReportPeerDimensionsChange success!");
        return TELEPHONY_SUCCESS;
    }

    int32_t OnReportCallDataUsageChange(const int64_t dataUsage)
    {
        TELEPHONY_LOGI("OnReportCallDataUsageChange success!");
        return TELEPHONY_SUCCESS;
    }

    int32_t OnReportCameraCapabilities(const CameraCapabilities &cameraCapabilities)
    {
        TELEPHONY_LOGI("OnReportCameraCapabilities success!");
        return TELEPHONY_SUCCESS;
    }

    int32_t OnPhoneStateChange(int32_t numActive, int32_t numHeld, int32_t callState,
        const std::string &number)
    {
        TELEPHONY_LOGI("OnPhoneStateChange success!");
        return TELEPHONY_SUCCESS;
    }

private:
    using CallAbilityCallbackFunc = int32_t (CallAbilityCallbackStub::*)(MessageParcel &data, MessageParcel &reply);
    int32_t OnUpdateCallStateInfoRequest(MessageParcel &data, MessageParcel &reply);
    int32_t OnUpdateCallEventRequest(MessageParcel &data, MessageParcel &reply);
    int32_t OnUpdateAsyncResultRequest(MessageParcel &data, MessageParcel &reply);

private:
    std::map<uint32_t, CallAbilityCallbackFunc> memberFuncMap_;
};

class CallManagerCallBackStub : public CallManagerCallback {
public:
    int32_t OnCallDetailsChange(const CallAttributeInfo &info)
    {
        return TELEPHONY_SUCCESS;
    }

    int32_t OnMeeTimeDetailsChange(const CallAttributeInfo &info)
    {
        return TELEPHONY_SUCCESS;
    }

    int32_t OnCallEventChange(const CallEventInfo &info)
    {
        return TELEPHONY_SUCCESS;
    }

    int32_t OnCallDisconnectedCause(const DisconnectedDetails &details)
    {
        return TELEPHONY_SUCCESS;
    }

    int32_t OnReportAsyncResults(CallResultReportId reportId, AppExecFwk::PacMap &resultInfo)
    {
        return TELEPHONY_SUCCESS;
    }

    int32_t OnOttCallRequest(OttCallRequestId requestId, AppExecFwk::PacMap &info)
    {
        return TELEPHONY_SUCCESS;
    }

    int32_t OnReportMmiCodeResult(const MmiCodeInfo &info)
    {
        return TELEPHONY_SUCCESS;
    }

    int32_t OnReportAudioDeviceChange(const AudioDeviceInfo &info)
    {
        return TELEPHONY_SUCCESS;
    }

    int32_t OnReportPostDialDelay(const std::string &str)
    {
        return TELEPHONY_SUCCESS;
    }

    int32_t OnUpdateImsCallModeChange(const CallMediaModeInfo &imsCallModeInfo)
    {
        return TELEPHONY_SUCCESS;
    }

    int32_t OnCallSessionEventChange(const CallSessionEvent &callSessionEventOptions)
    {
        return TELEPHONY_SUCCESS;
    }

    int32_t OnPeerDimensionsChange(const PeerDimensionsDetail &peerDimensionsDetail)
    {
        return TELEPHONY_SUCCESS;
    }

    int32_t OnCallDataUsageChange(const int64_t dataUsage)
    {
        return TELEPHONY_SUCCESS;
    }

    int32_t OnUpdateCameraCapabilities(const CameraCapabilities &cameraCapabilities)
    {
        return TELEPHONY_SUCCESS;
    }

    int32_t OnPhoneStateChange(int32_t numActive, int32_t numHeld, int32_t callState,
        const std::string &number)
    {
        return TELEPHONY_SUCCESS;
    }
};

class CallManagerConnect {
public:
    CallManagerConnect();
    ~CallManagerConnect();
    int32_t Init(int32_t systemAbilityId);
    void UnInit();
    int32_t DialCall(std::u16string number, AppExecFwk::PacMap &extras) const;
    int32_t AnswerCall(int32_t callId, int32_t videoState) const;
    int32_t RejectCall(int32_t callId, bool isSendSms, std::u16string content) const;
    int32_t HoldCall(int32_t callId) const;
    int32_t UnHoldCall(int32_t callId) const;
    int32_t HangUpCall(int32_t callId) const;
    int32_t GetCallState() const;
    int32_t SwitchCall(int32_t callId) const;
    bool HasCall() const;
    int32_t IsNewCallAllowed(bool &enabled) const;
    int32_t IsRinging(bool &enabled) const;
    int32_t IsInEmergencyCall(bool &enabled) const;
    int32_t StartDtmf(int32_t callId, char c) const;
    int32_t StopDtmf(int32_t callId) const;
    int32_t GetCallWaiting(int32_t slotId) const;
    int32_t SetCallWaiting(int32_t slotId, bool activate) const;
    int32_t GetCallRestriction(int32_t slotId, CallRestrictionType type);
    int32_t SetCallRestriction(int32_t slotId, CallRestrictionInfo &info);
    int32_t GetCallTransferInfo(int32_t slotId, CallTransferType type);
    int32_t SetCallTransferInfo(int32_t slotId, CallTransferInfo &info);
    int32_t CombineConference(int32_t mainCallId) const;
    int32_t SeparateConference(int32_t callId) const;
    int32_t KickOutFromConference(int32_t callId) const;
    int32_t IsEmergencyPhoneNumber(std::u16string &number, int32_t slotId, bool &enabled) const;
    int32_t FormatPhoneNumber(std::u16string &number, std::u16string &countryCode, std::u16string &formatNumber) const;
    int32_t FormatPhoneNumberToE164(
        std::u16string &number, std::u16string &countryCode, std::u16string &formatNumber) const;
    int32_t GetMainCallId(int32_t callId, int32_t &mainCallId);
    int32_t GetSubCallIdList(int32_t callId, std::vector<std::u16string> &callIdList);
    int32_t GetCallIdListForConference(int32_t callId, std::vector<std::u16string> &callIdList);
    int32_t ControlCamera(int32_t callId, std::u16string cameraId);
    int32_t SetAudioDevice(const AudioDevice &audioDevice);
    int32_t SetPreviewWindow(int32_t callId, std::string surfaceId);
    int32_t SetDisplayWindow(int32_t callId, std::string surfaceId);
    int32_t SetCameraZoom(float zoomRatio);
    int32_t SetPausePicture(int32_t callId, std::u16string path);
    int32_t SetDeviceDirection(int32_t callId, int32_t rotation);
    int32_t CancelCallUpgrade(int32_t callId);
    int32_t RequestCameraCapabilities(int32_t callId);
    int32_t EnableImsSwitch(int32_t slotId);
    int32_t DisableImsSwitch(int32_t slotId);
    int32_t IsImsSwitchEnabled(int32_t slotId);
    int32_t SetMuted(bool isMuted) const;
    int32_t RegisterCallBack();

private:
    int32_t ConnectService();
    void DisconnectService();

private:
    int32_t systemAbilityId_;
    sptr<ICallManagerService> callManagerServicePtr_;
    sptr<ICallAbilityCallback> callAbilityCallbackPtr_;
    ffrt::shared_mutex clientLock_;
};
} // namespace Telephony
} // namespace OHOS

#endif
