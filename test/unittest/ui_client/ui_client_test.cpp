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

#include <securec.h>
#include <string_ex.h>

#include "audio_system_manager.h"
#include "system_ability_definition.h"
#include "input/camera_manager.h"
#include "permission_kit.h"
#include "i_call_manager_service.h"
#include "call_manager_client.h"

#include "audio_player.h"
#include "call_manager_inner_type.h"
#include "call_manager_errors.h"
#include "call_manager_callback_test.h"
#include "common_event_subscriber_test.h"

namespace OHOS {
namespace Telephony {
std::shared_ptr<CallManagerClient> g_clientPtr = nullptr;
using CallManagerServiceFunc = void (*)();
std::map<uint32_t, CallManagerServiceFunc> g_memberFuncMap;
std::vector<Security::Permission::PermissionDef> permDefList;
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
const std::string EVENT_BLUETOOTH_SCO_CONNECTED = "usual.event.BLUETOOTH_SCO_CONNECTED";
const std::string EVENT_BLUETOOTH_SCO_DISCONNECTED = "usual.event.BLUETOOTH_SCO_DISCONNECTED";
constexpr size_t DEFAULT_NET_TYPE = 0;
constexpr size_t DEFAULT_ITEM_VALUE = 0;
const int32_t DEFINE_INIT_PERMISSIONS = 93;
const int32_t DEFINE_VERIFY_PERMISSIONS = 94;
const int32_t DEFINE_CONNECT_BT_SCO = 95;
const int32_t DEFINE_DISCONNECT_BT_SCO = 96;
const int32_t DEFINE_SUBSCRIBERCOMMON_EVENT = 97;
const std::string TEST_BUNDLE_NAME = "com.ohos.callManagerTest";
const std::string CALL_UI_BUNDLE_NAME = "com.ohos.videocall";
const std::string TEST_PERMISSION_NAME_CAMERA = "ohos.permission.camera";
const std::string TEST_LABEL = "test label";
const std::string TEST_DESCRIPTION = "test description";

const int32_t TEST_LABEL_ID = 9527;
const int32_t TEST_DESCRIPTION_ID = 9528;
const int32_t TEST_USER_ID = 0;

void DialCall()
{
    int32_t accountId = DEFAULT_ACCOUNT_ID;
    int32_t videoState = DEFAULT_VIDEO_STATE;
    int32_t dialScene = DEFAULT_DIAL_SCENE;
    int32_t dialType = DEFAULT_DIAL_TYPE;
    int32_t callType = DEFAULT_CALL_TYPE;
    std::u16string phoneNumber;
    std::string tmpStr;
    AppExecFwk::PacMap dialInfo;
    std::cout << "------Dial------" << std::endl;
    std::cout << "please input phone number:" << std::endl;
    phoneNumber.clear();
    tmpStr.clear();
    std::cin >> tmpStr;
    phoneNumber = Str8ToStr16(tmpStr);
    std::cout << "you want to call " << tmpStr << std::endl;
    std::cout << "please input accountId:" << std::endl;
    std::cin >> accountId;
    std::cout << "please input videoState[0:audio,1:video]:" << std::endl;
    std::cin >> videoState;
    std::cout << "please input dialScene[0:normal,1:privileged,2:emergency]:" << std::endl;
    std::cin >> dialScene;
    std::cout << "please input dialType[0:carrier,1:voice mail,2:ott]:" << std::endl;
    std::cin >> dialType;
    std::cout << "please input callType[0:cs,1:ims,2:ott]:" << std::endl;
    std::cin >> callType;

    dialInfo.PutIntValue("accountId", accountId);
    dialInfo.PutIntValue("videoState", videoState);
    dialInfo.PutIntValue("dialScene", dialScene);
    dialInfo.PutIntValue("dialType", dialType);
    dialInfo.PutIntValue("callType", callType);
    if (g_clientPtr == nullptr) {
        std::cout << "g_clientPtr is nullptr" << std::endl;
        return;
    }
    int32_t ret = g_clientPtr->DialCall(phoneNumber, dialInfo);
    std::cout << "return value:" << ret << std::endl;
}

void AnswerCall()
{
    int32_t callId = DEFAULT_CALL_ID;
    int32_t videoState = DEFAULT_VIDEO_STATE;
    std::cout << "------Answer------" << std::endl;
    std::cout << "please input callId:" << std::endl;
    std::cin >> callId;
    std::cout << "please input videoState[0:audio,1:video]:" << std::endl;
    std::cin >> videoState;
    int32_t ret = TELEPHONY_SUCCESS;
    if (g_clientPtr == nullptr) {
        std::cout << "g_clientPtr is nullptr" << std::endl;
        return;
    }
    ret = g_clientPtr->AnswerCall(callId, videoState);
    std::cout << "return value:" << ret << std::endl;
}

void RejectCall()
{
    int32_t callId = DEFAULT_CALL_ID;
    int32_t boolValue = DEFAULT_VALUE;
    bool flag = false;
    std::u16string content;
    std::string tmpStr;
    content.clear();
    std::cout << "------Reject------" << std::endl;
    std::cout << "please input callId:" << std::endl;
    std::cin >> callId;
    std::cout << "Whether to enter the reason for rejection?[0:no,1:yes]:" << std::endl;
    std::cin >> boolValue;
    if (boolValue != DEFAULT_VALUE) {
        flag = true;
        std::cout << "please input reject message:" << std::endl;
        tmpStr.clear();
        std::cin >> tmpStr;
        content = Str8ToStr16(tmpStr);
    }
    if (g_clientPtr == nullptr) {
        std::cout << "g_clientPtr is nullptr" << std::endl;
        return;
    }
    int32_t ret = g_clientPtr->RejectCall(callId, flag, content);
    std::cout << "return value:" << ret << std::endl;
}

void HoldCall()
{
    int32_t callId = DEFAULT_CALL_ID;
    std::cout << "------HoldCall------" << std::endl;
    std::cout << "please input callId:" << std::endl;
    std::cin >> callId;
    if (g_clientPtr == nullptr) {
        std::cout << "g_clientPtr is nullptr" << std::endl;
        return;
    }
    int32_t ret = g_clientPtr->HoldCall(callId);
    std::cout << "return value:" << ret << std::endl;
}

void UnHoldCall()
{
    int32_t callId = DEFAULT_CALL_ID;
    std::cout << "------UnHoldCall------" << std::endl;
    std::cout << "please input callId:" << std::endl;
    std::cin >> callId;
    if (g_clientPtr == nullptr) {
        std::cout << "g_clientPtr is nullptr" << std::endl;
        return;
    }
    int32_t ret = g_clientPtr->UnHoldCall(callId);
    std::cout << "return value:" << ret << std::endl;
}

void HangUpCall()
{
    int32_t callId = DEFAULT_CALL_ID;
    std::cout << "------HangUpCall------" << std::endl;
    std::cout << "please input callId:" << std::endl;
    std::cin >> callId;
    if (g_clientPtr == nullptr) {
        std::cout << "g_clientPtr is nullptr" << std::endl;
        return;
    }
    int32_t ret = g_clientPtr->HangUpCall(callId);
    std::cout << "return value:" << ret << std::endl;
}

void CombineConference()
{
    int32_t mainCallId = DEFAULT_CALL_ID;
    std::cout << "------CombineConference------" << std::endl;
    std::cout << "please input mainCallId:" << std::endl;
    std::cin >> mainCallId;
    if (g_clientPtr == nullptr) {
        std::cout << "g_clientPtr is nullptr" << std::endl;
        return;
    }
    int32_t ret = g_clientPtr->CombineConference(mainCallId);
    std::cout << "return value:" << ret << std::endl;
}

void SeparateConference()
{
    int32_t callId = DEFAULT_CALL_ID;
    std::cout << "------SeparateConference------" << std::endl;
    std::cout << "please input callId:" << std::endl;
    std::cin >> callId;
    if (g_clientPtr == nullptr) {
        std::cout << "g_clientPtr is nullptr" << std::endl;
        return;
    }
    int32_t ret = g_clientPtr->SeparateConference(callId);
    std::cout << "return value:" << ret << std::endl;
}

void GetCallState()
{
    std::cout << "------GetCallState------" << std::endl;
    if (g_clientPtr == nullptr) {
        std::cout << "g_clientPtr is nullptr" << std::endl;
        return;
    }
    int32_t ret = g_clientPtr->GetCallState();
    std::cout << "return value:" << ret << std::endl;
}

void SwitchCall()
{
    int32_t callId = DEFAULT_CALL_ID;
    std::cout << "------SwitchCall------" << std::endl;
    std::cout << "please input callId:" << std::endl;
    std::cin >> callId;
    if (g_clientPtr == nullptr) {
        std::cout << "g_clientPtr is nullptr" << std::endl;
        return;
    }
    int32_t ret = g_clientPtr->SwitchCall(callId);
    std::cout << "return value:" << ret << std::endl;
}

void HasCall()
{
    std::cout << "------HasCall------" << std::endl;
    if (g_clientPtr == nullptr) {
        std::cout << "g_clientPtr is nullptr" << std::endl;
        return;
    }
    int32_t ret = g_clientPtr->HasCall();
    std::cout << "return value:" << ret << std::endl;
}

void IsNewCallAllowed()
{
    std::cout << "------IsNewCallAllowed------" << std::endl;
    if (g_clientPtr == nullptr) {
        std::cout << "g_clientPtr is nullptr" << std::endl;
        return;
    }
    int32_t ret = g_clientPtr->IsNewCallAllowed();
    std::cout << "return value:" << ret << std::endl;
}

void IsRinging()
{
    std::cout << "------IsRinging------" << std::endl;
    if (g_clientPtr == nullptr) {
        std::cout << "g_clientPtr is nullptr" << std::endl;
        return;
    }
    int32_t ret = g_clientPtr->IsRinging();
    std::cout << "return value:" << ret << std::endl;
}

void IsInEmergencyCall()
{
    std::cout << "------IsInEmergencyCall------" << std::endl;
    if (g_clientPtr == nullptr) {
        std::cout << "g_clientPtr is nullptr" << std::endl;
        return;
    }
    int32_t ret = g_clientPtr->IsInEmergencyCall();
    std::cout << "return value:" << ret << std::endl;
}

void StartDtmf()
{
    char c = DEFAULT_VALUE;
    int32_t callId = DEFAULT_CALL_ID;
    std::cout << "please input StartDtmf callId:" << std::endl;
    std::cin >> callId;
    std::cout << "Please enter to send dtmf characters:" << std::endl;
    std::cin >> c;
    int32_t ret = g_clientPtr->StartDtmf(callId, c);
    std::cout << "return value:" << ret << std::endl;
}

void StopDtmf()
{
    int32_t callId = DEFAULT_CALL_ID;
    std::cout << "please input StopDtmf callId:" << std::endl;
    std::cin >> callId;
    int32_t ret = g_clientPtr->StopDtmf(callId);
    std::cout << "return value:" << ret << std::endl;
}

void GetCallWaiting()
{
    int32_t slotId = SIM1_SLOTID;
    std::cout << "------GetCallWaiting------" << std::endl;
    std::cout << "please input slotId:" << std::endl;
    std::cin >> slotId;
    if (g_clientPtr == nullptr) {
        std::cout << "g_clientPtr is nullptr" << std::endl;
        return;
    }
    int32_t ret = g_clientPtr->GetCallWaiting(slotId);
    std::cout << "return value:" << ret << std::endl;
}

void SetCallWaiting()
{
    int32_t slotId = SIM1_SLOTID;
    int32_t flag = DEFAULT_VALUE;
    std::cout << "------SetCallWaiting------" << std::endl;
    std::cout << "please input slotId:" << std::endl;
    std::cin >> slotId;
    std::cout << "whether open(0:no 1:yes):" << std::endl;
    std::cin >> flag;
    if (g_clientPtr == nullptr) {
        std::cout << "g_clientPtr is nullptr" << std::endl;
        return;
    }
    int32_t ret = g_clientPtr->SetCallWaiting(slotId, (flag == 1) ? true : false);
    std::cout << "return value:" << ret << std::endl;
}

void GetCallRestriction()
{
    int32_t slotId = SIM1_SLOTID;
    int32_t tmpType = DEFAULT_VALUE;
    CallRestrictionType type;
    std::cout << "------GetCallRestriction------" << std::endl;
    std::cout << "please input slotId:" << std::endl;
    std::cin >> slotId;
    std::cout << "please input restriction type:" << std::endl;
    std::cin >> tmpType;
    type = static_cast<CallRestrictionType>(tmpType);
    if (g_clientPtr == nullptr) {
        std::cout << "g_clientPtr is nullptr" << std::endl;
        return;
    }
    int32_t ret = g_clientPtr->GetCallRestriction(slotId, type);
    std::cout << "return value:" << ret << std::endl;
}

void SetCallRestriction()
{
    int32_t slotId = SIM1_SLOTID;
    int32_t tmpType = DEFAULT_VALUE;
    CallRestrictionInfo info;
    std::cout << "------SetCallRestriction------" << std::endl;
    std::cout << "please input slotId:" << std::endl;
    std::cin >> slotId;
    std::cout << "please input restriction type:" << std::endl;
    std::cin >> tmpType;
    info.fac = static_cast<CallRestrictionType>(tmpType);
    std::cout << "is open(1: open, 0: close):" << std::endl;
    std::cin >> tmpType;
    info.mode = static_cast<CallRestrictionMode>(tmpType);
    std::cout << "please input password:" << std::endl;
    std::cin >> info.password;
    if (g_clientPtr == nullptr) {
        std::cout << "g_clientPtr is nullptr" << std::endl;
        return;
    }
    int32_t ret = g_clientPtr->SetCallRestriction(slotId, info);
    std::cout << "return value:" << ret << std::endl;
}

void SetCallPreferenceMode()
{
    int32_t slotId = SIM1_SLOTID;
    int32_t mode = DEFAULT_PREFERENCEMODE;
    std::cout << "------CallPreferenceMode------" << std::endl;
    std::cout << "please input slotId:" << std::endl;
    std::cin >> slotId;
    std::cout << "please input PreferenceMode:" << std::endl;
    std::cout << "CS_VOICE_ONLY = 1" << std::endl;
    std::cout << "CS_VOICE_PREFERRED = 2" << std::endl;
    std::cout << "IMS_PS_VOICE_PREFERRED = 3" << std::endl;
    std::cout << "IMS_PS_VOICE_ONLY = 4" << std::endl;
    std::cin >> mode;
    if (g_clientPtr == nullptr) {
        std::cout << "g_clientPtr is nullptr" << std::endl;
        return;
    }
    int32_t ret = g_clientPtr->SetCallPreferenceMode(slotId, mode);
    std::cout << "return value:" << ret << std::endl;
}

void GetCallTransferInfo()
{
    int32_t slotId = SIM1_SLOTID;
    int32_t tmpType = DEFAULT_VALUE;
    CallTransferType type;
    std::cout << "------GetCallTransferInfo------" << std::endl;
    std::cout << "please input slotId:" << std::endl;
    std::cin >> slotId;
    std::cout << "please input transfer type:" << std::endl;
    std::cin >> tmpType;
    type = static_cast<CallTransferType>(tmpType);
    if (g_clientPtr == nullptr) {
        std::cout << "g_clientPtr is nullptr" << std::endl;
        return;
    }
    int32_t ret = g_clientPtr->GetCallTransferInfo(slotId, type);
    std::cout << "return value:" << ret << std::endl;
}

void SetCallTransferInfo()
{
    int32_t slotId = SIM1_SLOTID;
    int32_t tmpType = DEFAULT_VALUE;
    CallTransferInfo info;
    std::cout << "------SetCallTransferInfo------" << std::endl;
    std::cout << "please input slotId:" << std::endl;
    std::cin >> slotId;
    std::cout << "please input transfer type:" << std::endl;
    std::cin >> tmpType;
    info.type = static_cast<CallTransferType>(tmpType);
    std::cout << "please input transfer setting type:" << std::endl;
    std::cin >> tmpType;
    info.settingType = static_cast<CallTransferSettingType>(tmpType);
    std::cout << "please input phone number:" << std::endl;
    std::cin >> info.transferNum;
    if (g_clientPtr == nullptr) {
        std::cout << "g_clientPtr is nullptr" << std::endl;
        return;
    }
    int32_t ret = g_clientPtr->SetCallTransferInfo(slotId, info);
    std::cout << "return value:" << ret << std::endl;
}

void IsEmergencyPhoneNumber()
{
    int32_t slotId = SIM1_SLOTID;
    int32_t errorCode = TELEPHONY_ERROR;
    std::u16string phoneNumber;
    std::string tmpStr;
    std::cout << "------IsEmergencyPhoneNumber------" << std::endl;
    std::cout << "please input phone number:" << std::endl;
    phoneNumber.clear();
    tmpStr.clear();
    std::cin >> tmpStr;
    phoneNumber = Str8ToStr16(tmpStr);
    std::cout << "The number is " << tmpStr << std::endl;
    std::cout << "please input slotId:" << std::endl;
    std::cin >> slotId;
    if (g_clientPtr == nullptr) {
        std::cout << "g_clientPtr is nullptr" << std::endl;
        return;
    }
    int32_t ret = g_clientPtr->IsEmergencyPhoneNumber(phoneNumber, slotId, errorCode);
    std::cout << "return value:" << ret << std::endl;
    std::cout << "return errorCode:" << errorCode << std::endl;
}

void FormatPhoneNumber()
{
    std::u16string phoneNumber;
    std::u16string countryCode;
    std::u16string formatNumber;
    std::string tmpStr;
    std::cout << "------FormatPhoneNumber------" << std::endl;
    std::cout << "please input phone number:" << std::endl;
    phoneNumber.clear();
    countryCode.clear();
    formatNumber.clear();
    tmpStr.clear();
    std::cin >> tmpStr;
    phoneNumber = Str8ToStr16(tmpStr);
    std::cout << "The number is " << tmpStr << std::endl;
    tmpStr.clear();
    std::cout << "please input countryCode:" << std::endl;
    std::cin >> tmpStr;
    countryCode = Str8ToStr16(tmpStr);
    if (g_clientPtr == nullptr) {
        std::cout << "g_clientPtr is nullptr" << std::endl;
        return;
    }
    int32_t ret = g_clientPtr->FormatPhoneNumber(phoneNumber, countryCode, formatNumber);
    std::cout << "return value:" << ret << std::endl;
    std::cout << "return number:" << Str16ToStr8(formatNumber) << std::endl;
}

void FormatPhoneNumberToE164()
{
    std::u16string phoneNumber;
    std::u16string countryCode;
    std::u16string formatNumber;
    std::string tmpStr;
    std::cout << "------FormatPhoneNumberToE164------" << std::endl;
    std::cout << "please input phone number:" << std::endl;
    phoneNumber.clear();
    countryCode.clear();
    formatNumber.clear();
    tmpStr.clear();
    std::cin >> tmpStr;
    phoneNumber = Str8ToStr16(tmpStr);
    std::cout << "The number is " << tmpStr << std::endl;
    tmpStr.clear();
    std::cout << "please input countryCode:" << std::endl;
    std::cin >> tmpStr;
    countryCode = Str8ToStr16(tmpStr);
    if (g_clientPtr == nullptr) {
        std::cout << "g_clientPtr is nullptr" << std::endl;
        return;
    }
    int32_t ret = g_clientPtr->FormatPhoneNumberToE164(phoneNumber, countryCode, formatNumber);
    std::cout << "return value:" << ret << std::endl;
    std::cout << "return number:" << Str16ToStr8(formatNumber) << std::endl;
}

void GetMainCallId()
{
    int callId = DEFAULT_CALL_ID;
    std::cout << "please input callId:" << std::endl;
    std::cin >> callId;
    int32_t ret = g_clientPtr->GetMainCallId(callId);
    std::cout << "return value:" << ret << std::endl;
}

void GetSubCallIdList()
{
    int32_t callId = DEFAULT_CALL_ID;
    std::cout << "please input callId:" << std::endl;
    std::cin >> callId;
    std::vector<std::u16string> ret = g_clientPtr->GetSubCallIdList(callId);
    std::vector<std::u16string>::iterator it = ret.begin();
    for (; it != ret.end(); it++) {
        std::cout << "callId:" << Str16ToStr8(*it) << std::endl;
    }
}

void GetCallIdListForConference()
{
    int32_t callId = DEFAULT_CALL_ID;
    std::cout << "please input callId:" << std::endl;
    std::cin >> callId;
    std::vector<std::u16string> ret = g_clientPtr->GetCallIdListForConference(callId);
    std::vector<std::u16string>::iterator it = ret.begin();
    for (; it != ret.end(); it++) {
        std::cout << "callId:" << Str16ToStr8(*it) << std::endl;
    }
}

void InviteToConference()
{
    int32_t callId = DEFAULT_CALL_ID;
    std::cout << "please input callId:" << std::endl;
    std::cin >> callId;
    std::string number;
    std::vector<std::u16string> numberList;
    std::cout << "please input participate phone number:[-1]end" << std::endl;
    while (std::cin >> number) {
        numberList.push_back(Str8ToStr16(number));
        if (number == "-1") {
            break;
        }
    }
    int32_t ret = g_clientPtr->JoinConference(callId, numberList);
    std::cout << "return value:" << ret << std::endl;
}

void SetMute()
{
    int32_t isMute = DEFAULT_VALUE;
    std::cout << "------SetMute------" << std::endl;
    std::cout << "please input mute state(0:false 1:true):" << std::endl;
    std::cin >> isMute;
    if (g_clientPtr == nullptr) {
        std::cout << "g_clientPtr is nullptr" << std::endl;
        return;
    }
    int32_t ret = g_clientPtr->SetMuted((isMute == 1) ? true : false);
    std::cout << "return value:" << ret << std::endl;
}

void MuteRinger()
{
    std::cout << "------MuteRinger------" << std::endl;
    if (g_clientPtr == nullptr) {
        std::cout << "g_clientPtr is nullptr" << std::endl;
        return;
    }
    int32_t ret = g_clientPtr->MuteRinger();
    std::cout << "return value:" << ret << std::endl;
}

void SetAudioDevice()
{
    int32_t deviceType = DEFAULT_VALUE;
    std::cout << "------SetAudioDevice------" << std::endl;
    std::cout << "please input device type(0:earpiece 1:speaker 2:wired headset 3:bluetooth sco):" << std::endl;
    std::cin >> deviceType;
    if (g_clientPtr == nullptr) {
        std::cout << "g_clientPtr is nullptr" << std::endl;
        return;
    }
    AudioDevice device = AudioDevice::DEVICE_UNKNOWN;
    device = static_cast<AudioDevice>(deviceType);
    int32_t ret = g_clientPtr->SetAudioDevice(device);
    std::cout << "return value:" << ret << std::endl;
}

void GetVolume()
{
    int32_t type = DEFAULT_VALUE;
    std::cout << "------GetVolume------" << std::endl;
    std::cout << "please input volume type(3:ring 4:music)" << std::endl;
    std::cin >> type;
    AudioStandard::AudioSystemManager::AudioVolumeType volumeType =
        AudioStandard::AudioSystemManager::AudioVolumeType::STREAM_MUSIC;
    switch (type) {
        case AudioStandard::AudioSystemManager::AudioVolumeType::STREAM_RING:
            volumeType = AudioStandard::AudioSystemManager::AudioVolumeType::STREAM_RING;
            break;
        case AudioStandard::AudioSystemManager::AudioVolumeType::STREAM_MUSIC:
            volumeType = AudioStandard::AudioSystemManager::AudioVolumeType::STREAM_MUSIC;
            break;
        default:
            break;
    }
    AudioStandard::AudioSystemManager *audioSystemMgr = AudioStandard::AudioSystemManager::GetInstance();
    int32_t ret = audioSystemMgr->GetVolume(volumeType);
    std::cout << "return value:" << ret << std::endl;
}

void SetVolume()
{
    int32_t volume = DEFAULT_VALUE;
    int32_t type = DEFAULT_VALUE;
    std::cout << "------SetVolume------" << std::endl;
    std::cout << "please input volume value(0~15) :" << std::endl;
    std::cin >> volume;
    std::cout << "please input volume type(3:ring 4:music)" << std::endl;
    std::cin >> type;
    if (volume < MIN_VOLUME || volume > MAX_VOLUME) {
        std::cout << "volume value error" << std::endl;
        return;
    }
    AudioStandard::AudioSystemManager::AudioVolumeType volumeType =
        AudioStandard::AudioSystemManager::AudioVolumeType::STREAM_MUSIC;
    switch (type) {
        case AudioStandard::AudioSystemManager::AudioVolumeType::STREAM_RING:
            volumeType = AudioStandard::AudioSystemManager::AudioVolumeType::STREAM_RING;
            break;
        case AudioStandard::AudioSystemManager::AudioVolumeType::STREAM_MUSIC:
            volumeType = AudioStandard::AudioSystemManager::AudioVolumeType::STREAM_MUSIC;
            break;
        default:
            break;
    }
    AudioStandard::AudioSystemManager *audioSystemMgr = AudioStandard::AudioSystemManager::GetInstance();
    int32_t ret = audioSystemMgr->SetVolume(volumeType, volume);
    std::cout << "return value:" << ret << std::endl;
}

bool InitRenderer(const std::unique_ptr<AudioStandard::AudioRenderer> &audioRenderer, const wav_hdr &wavHeader)
{
    AudioStandard::AudioRendererParams rendererParams;
    rendererParams.sampleFormat = static_cast<AudioStandard::AudioSampleFormat>(wavHeader.bitsPerSample);
    rendererParams.sampleRate = static_cast<AudioStandard::AudioSamplingRate>(wavHeader.SamplesPerSec);
    rendererParams.channelCount = static_cast<AudioStandard::AudioChannel>(wavHeader.NumOfChan);
    rendererParams.encodingType = static_cast<AudioStandard::AudioEncodingType>(AudioStandard::ENCODING_PCM);
    if (audioRenderer->SetParams(rendererParams) != TELEPHONY_SUCCESS) {
        std::cout << "audio renderer set params error" << std::endl;
        if (!audioRenderer->Release()) {
            std::cout << "audio renderer release error" << std::endl;
        }
        return false;
    }
    if (!audioRenderer->Start()) {
        std::cout << "audio renderer start error" << std::endl;
        return false;
    }
    uint32_t frameCount;
    if (audioRenderer->GetFrameCount(frameCount)) {
        return false;
    }
    std::cout << "frame count : " << frameCount << std::endl;
    return true;
}

bool PlayRingtone()
{
    wav_hdr wavHeader;
    std::cout << "please input ringtone file path : " << std::endl;
    char path[RING_PATH_MAX_LENGTH];
    std::cin >> path;
    FILE *wavFile = fopen(path, "rb");
    if (wavFile == nullptr) {
        std::cout << "wav file nullptr" << std::endl;
        return false;
    }
    (void)fread(&wavHeader, READ_SIZE, sizeof(wav_hdr), wavFile);
    std::unique_ptr<AudioStandard::AudioRenderer> audioRenderer =
        AudioStandard::AudioRenderer::Create(AudioStandard::AudioStreamType::STREAM_MUSIC);
    if (!InitRenderer(audioRenderer, wavHeader)) {
        (void)fclose(wavFile);
        return false;
    }
    size_t bufferLen, bytesToWrite = DEFAULT_SIZE, bytesWritten = DEFAULT_SIZE;
    if (audioRenderer->GetBufferSize(bufferLen)) {
        (void)fclose(wavFile);
        return false;
    }
    std::unique_ptr<uint8_t> buffer = std::make_unique<uint8_t>(bufferLen + bufferLen);
    if (buffer == nullptr) {
        std::cout << "malloc memory nullptr" << std::endl;
        (void)fclose(wavFile);
        return false;
    }
    while (!feof(wavFile)) {
        bytesToWrite = fread(buffer.get(), READ_SIZE, bufferLen, wavFile);
        bytesWritten = DEFAULT_SIZE;
        while ((bytesWritten < bytesToWrite) && ((bytesToWrite - bytesWritten) > MIN_BYTES)) {
            bytesWritten += audioRenderer->Write(buffer.get() + bytesWritten, bytesToWrite - bytesWritten);
        }
    }
    audioRenderer->Flush();
    audioRenderer->Drain();
    audioRenderer->Stop();
    audioRenderer->Release();
    (void)fclose(wavFile);
    std::cout << "audio renderer plackback done" << std::endl;
    return true;
}

void ControlCamera()
{
    std::cout << "------ControlCamera test------" << std::endl;
    std::string tmpStr = "";
    sptr<CameraStandard::CameraManager> camManagerObj = CameraStandard::CameraManager::GetInstance();
    std::vector<sptr<CameraStandard::CameraInfo>> cameraObjList = camManagerObj->GetCameras();

    for (auto &it : cameraObjList) {
        tmpStr = it->GetID();
        std::cout << "camManagerObj->GetCameras Camera ID:" << tmpStr.c_str() << std::endl;
        break;
    }

    std::u16string CameraID;
    std::u16string callingPackage;
    CameraID.clear();
    callingPackage.clear();
    std::string cp = "com.ohos.videocall";
    CameraID = Str8ToStr16(cp);
    callingPackage = Str8ToStr16(cp);
    int32_t ret = g_clientPtr->ControlCamera(CameraID, callingPackage);
    std::cout << "error return value:" << ret << std::endl;

    CameraID.clear();
    callingPackage.clear();
    CameraID = Str8ToStr16(tmpStr);
    callingPackage = Str8ToStr16(cp);
    ret = g_clientPtr->ControlCamera(CameraID, callingPackage);
    std::cout << "ok return value:" << ret << std::endl;

    std::cout << "ControlCamera done" << std::endl;
}

void SetPreviewWindow()
{
    std::cout << "------SetPreviewWindow test------" << std::endl;
    VideoWindow window;
    window.x = WINDOWS_X_START;
    window.y = WINDOWS_Y_START;
    window.z = WINDOWS_Z_ERROR;
    window.width = WINDOWS_WIDTH;
    window.height = WINDOWS_HEIGHT;
    int32_t ret = g_clientPtr->SetPreviewWindow(window);
    std::cout << "error return value:" << ret << std::endl;

    window.z = WINDOWS_Z_BOTTOM;
    ret = g_clientPtr->SetPreviewWindow(window);
    std::cout << "return value:" << ret << std::endl;

    window.z = WINDOWS_Z_TOP;
    ret = g_clientPtr->SetPreviewWindow(window);
    std::cout << "return value:" << ret << std::endl;

    std::cout << "SetPreviewWindow done" << std::endl;
}

void SetDisplayWindow()
{
    std::cout << "------SetDisplayWindow test------" << std::endl;
    VideoWindow window;
    window.x = WINDOWS_X_START;
    window.y = WINDOWS_Y_START;
    window.z = WINDOWS_WIDTH;
    window.width = WINDOWS_WIDTH;
    window.height = WINDOWS_HEIGHT;
    int32_t ret = g_clientPtr->SetDisplayWindow(window);
    std::cout << "error return value:" << ret << std::endl;

    window.z = WINDOWS_Z_TOP;
    ret = g_clientPtr->SetDisplayWindow(window);
    std::cout << "ok return value:" << ret << std::endl;

    window.z = WINDOWS_Z_BOTTOM;
    ret = g_clientPtr->SetDisplayWindow(window);
    std::cout << "ok return value:" << ret << std::endl;

    std::cout << "SetDisplayWindow done" << std::endl;
}

void SetCameraZoom()
{
    const float CameraZoomMax = 12.0;
    const float CameraZoomMin = -0.1;
    const float CameraZoom = 2.0;
    std::cout << "------SetCameraZoom test------" << std::endl;
    int32_t ret = g_clientPtr->SetCameraZoom(CameraZoomMax);
    std::cout << "return value:" << ret << std::endl;

    ret = g_clientPtr->SetCameraZoom(CameraZoomMin);
    std::cout << "return value:" << ret << std::endl;

    ret = g_clientPtr->SetCameraZoom(CameraZoom);
    std::cout << "return value:" << ret << std::endl;
    std::cout << "SetCameraZoom done" << std::endl;
}

void SetPausePicture()
{
    std::cout << "------SetPausePicture test------" << std::endl;
    std::u16string path;
    std::string tmpStr = "/system/bin/1.png";
    path.clear();
    path = Str8ToStr16(tmpStr);
    int32_t ret = g_clientPtr->SetPausePicture(path);
    std::cout << "\n return value:" << ret << std::endl;
    std::cout << "SetPausePicture done" << std::endl;
}

void SetDeviceDirection()
{
    const int32_t DeviceDirectionError1 = 50;
    const int32_t DeviceDirectionError2 = 350;
    const int32_t DeviceDirection90 = 90;
    std::cout << "------SetDeviceDirection test------" << std::endl;
    int32_t ret = g_clientPtr->SetDeviceDirection(DeviceDirectionError1);
    std::cout << "\n return value:" << ret << std::endl;

    ret = g_clientPtr->SetDeviceDirection(DeviceDirectionError2);
    std::cout << "\n return value:" << ret << std::endl;

    ret = g_clientPtr->SetDeviceDirection(DeviceDirection90);
    std::cout << "\n return value:" << ret << std::endl;
    std::cout << "SetDeviceDirection done" << std::endl;
}

void SubscribeCommonEvent()
{
    std::cout << "------SubscribeCommonEvent------" << std::endl;
    std::cout << "please input common event type : " << std::endl;
    char eventType[RING_PATH_MAX_LENGTH];
    std::cin >> eventType;
    OHOS::EventFwk::MatchingSkills matchingSkills;
    std::string event(eventType);
    matchingSkills.AddEvent(event);
    // make subcriber info
    OHOS::EventFwk::CommonEventSubscribeInfo subscriberInfo(matchingSkills);
    // make a subcriber object
    std::shared_ptr<CommonEventSubscriberTest> subscriberTest =
        std::make_shared<CommonEventSubscriberTest>(subscriberInfo);
    if (subscriberTest == nullptr) {
        std::cout << "subscriber nullptr" << std::endl;
    }
    // subscribe a common event
    bool result = OHOS::EventFwk::CommonEventManager::SubscribeCommonEvent(subscriberTest);
    std::cout << "subscribe common event : " << eventType << ", result : " << result << std::endl;
}

void SendConnectBluetoothScoBroadcast()
{
    AAFwk::Want want;
    want.SetAction(EVENT_BLUETOOTH_SCO_CONNECTED);
    EventFwk::CommonEventData data;
    data.SetWant(want);
    data.SetCode(EVENT_BLUETOOTH_SCO_CONNECTED_CODE);
    OHOS::EventFwk::CommonEventPublishInfo publishInfo;
    publishInfo.SetOrdered(true);
    bool result = EventFwk::CommonEventManager::PublishCommonEvent(data, publishInfo, nullptr);
    std::cout << "publish common event : EVENT_BLUETOOTH_SCO_CONNECTED , result : " << result << std::endl;
}

void SendDisconnectBluetoothScoBroadcast()
{
    AAFwk::Want want;
    want.SetAction(EVENT_BLUETOOTH_SCO_DISCONNECTED);
    EventFwk::CommonEventData data;
    data.SetWant(want);
    data.SetCode(EVENT_BLUETOOTH_SCO_DISCONNECTED_CODE);
    OHOS::EventFwk::CommonEventPublishInfo publishInfo;
    publishInfo.SetOrdered(true);
    bool result = EventFwk::CommonEventManager::PublishCommonEvent(data, publishInfo, nullptr);
    std::cout << "publish common event : EVENT_BLUETOOTH_SCO_DISCONNECTED , result : " << result << std::endl;
}

void GetImsConfig()
{
    int32_t slotId = SIM1_SLOTID;
    int32_t item = DEFAULT_ITEM_VALUE;
    std::cout << "------GetImsConfig------" << std::endl;
    std::cout << "please input slotId:" << std::endl;
    std::cin >> slotId;
    std::cout << "please input item:" << std::endl;
    std::cin >> item;
    int32_t ret = TELEPHONY_SUCCESS;
    if (g_clientPtr == nullptr) {
        std::cout << "g_clientPtr is nullptr" << std::endl;
        return;
    }
    ret = g_clientPtr->GetImsConfig(slotId, static_cast<ImsConfigItem>(item));
    std::cout << "return value:" << ret << std::endl;
}

void SetImsConfig()
{
    int32_t slotId = SIM1_SLOTID;
    int32_t item = DEFAULT_ITEM_VALUE;
    std::string tmpValue;
    std::u16string value;
    std::cout << "------SetImsConfig------" << std::endl;
    std::cout << "please input slotId:" << std::endl;
    std::cin >> slotId;
    std::cout << "please input item:" << std::endl;
    std::cin >> item;
    std::cout << "please input item value:" << std::endl;
    std::cin >> tmpValue;
    int32_t ret = TELEPHONY_SUCCESS;
    if (g_clientPtr == nullptr) {
        std::cout << "g_clientPtr is nullptr" << std::endl;
        return;
    }
    value = Str8ToStr16(tmpValue);
    ret = g_clientPtr->SetImsConfig(slotId, static_cast<ImsConfigItem>(item), value);
    std::cout << "return value:" << ret << std::endl;
}

void GetImsFeatureValue()
{
    int32_t slotId = SIM1_SLOTID;
    int32_t tmpType = FeatureType::TYPE_VOICE_OVER_LTE;
    FeatureType type = FeatureType::TYPE_VOICE_OVER_LTE;
    std::cout << "------GetImsFeatureValue------" << std::endl;
    std::cout << "please input slotId:" << std::endl;
    std::cin >> slotId;
    std::cout << "please input feature type:" << std::endl;
    std::cin >> tmpType;
    type = static_cast<FeatureType>(tmpType);
    int32_t ret = TELEPHONY_SUCCESS;
    if (g_clientPtr == nullptr) {
        std::cout << "g_clientPtr is nullptr" << std::endl;
        return;
    }
    ret = g_clientPtr->GetImsFeatureValue(slotId, type);
    std::cout << "return value:" << ret << std::endl;
}

void SetImsFeatureValue()
{
    int32_t slotId = SIM1_SLOTID;
    int32_t tmpType = FeatureType::TYPE_VOICE_OVER_LTE;
    FeatureType type;
    int32_t value = DEFAULT_NET_TYPE;
    std::cout << "------SetImsNetworkValue------" << std::endl;
    std::cout << "please input slotId:" << std::endl;
    std::cin >> slotId;
    std::cout << "please input feature type:" << std::endl;
    std::cin >> tmpType;
    type = (FeatureType)tmpType;
    std::cout << "please input value:" << std::endl;
    std::cin >> value;
    int32_t ret = TELEPHONY_SUCCESS;
    if (g_clientPtr == nullptr) {
        std::cout << "g_clientPtr is nullptr" << std::endl;
        return;
    }
    ret = g_clientPtr->SetImsFeatureValue(slotId, type, value);
    std::cout << "return value:" << ret << std::endl;
}

void UpdateCallMediaMode()
{
    int32_t callId = DEFAULT_CALL_ID;
    uint32_t mediaMode = DEFAULT_VIDEO_STATE;
    std::cout << "------UpdateCallMediaMode------" << std::endl;
    std::cout << "please input callId:" << std::endl;
    std::cin >> callId;
    std::cout << "please input media mode[0:voice, 1:video]:" << std::endl;
    std::cin >> mediaMode;
    CallMediaMode mode = static_cast<CallMediaMode>(mediaMode);
    int32_t ret = TELEPHONY_SUCCESS;
    if (g_clientPtr == nullptr) {
        std::cout << "g_clientPtr is nullptr" << std::endl;
        return;
    }
    ret = g_clientPtr->UpdateCallMediaMode(callId, mode);
    std::cout << "return value:" << ret << std::endl;
}

void EnableVoLte()
{
    int32_t slotId = SIM1_SLOTID;
    std::cout << "------EnableVoLte------" << std::endl;
    std::cout << "please input slot id:" << std::endl;
    std::cin >> slotId;
    int32_t ret = TELEPHONY_SUCCESS;
    if (g_clientPtr == nullptr) {
        std::cout << "g_clientPtr is nullptr" << std::endl;
        return;
    }
    ret = g_clientPtr->EnableVoLte(slotId);
    std::cout << "return value:" << ret << std::endl;
}

void DisableVoLte()
{
    int32_t slotId = SIM1_SLOTID;
    std::cout << "------DisableVoLte------" << std::endl;
    std::cout << "please input slot id:" << std::endl;
    std::cin >> slotId;
    int32_t ret = TELEPHONY_SUCCESS;
    if (g_clientPtr == nullptr) {
        std::cout << "g_clientPtr is nullptr" << std::endl;
        return;
    }
    ret = g_clientPtr->DisableVoLte(slotId);
    std::cout << "return value:" << ret << std::endl;
}

void IsVoLteEnabled()
{
    int32_t slotId = SIM1_SLOTID;
    std::cout << "------IsVoLteEnabled------" << std::endl;
    std::cout << "please input slot id:" << std::endl;
    std::cin >> slotId;
    int32_t ret = TELEPHONY_SUCCESS;
    if (g_clientPtr == nullptr) {
        std::cout << "g_clientPtr is nullptr" << std::endl;
        return;
    }
    ret = g_clientPtr->IsVoLteEnabled(slotId);
    std::cout << "return value:" << ret << std::endl;
}

void EnableLteEnhanceMode()
{
    int32_t slotId = SIM1_SLOTID;
    std::cout << "------EnableLteEnhanceMode------" << std::endl;
    std::cout << "please input slot id:" << std::endl;
    std::cin >> slotId;
    int32_t ret = TELEPHONY_SUCCESS;
    if (g_clientPtr == nullptr) {
        std::cout << "g_clientPtr is nullptr" << std::endl;
        return;
    }
    ret = g_clientPtr->EnableLteEnhanceMode(slotId);
    std::cout << "return value:" << ret << std::endl;
}

void DisableLteEnhanceMode()
{
    int slotId = SIM1_SLOTID;
    std::cout << "------DisableLteEnhanceMode------" << std::endl;
    std::cout << "please input slot id:" << std::endl;
    std::cin >> slotId;
    int32_t ret = TELEPHONY_SUCCESS;
    if (g_clientPtr == nullptr) {
        std::cout << "g_clientPtr is nullptr" << std::endl;
        return;
    }
    ret = g_clientPtr->DisableLteEnhanceMode(slotId);
    std::cout << "return value:" << ret << std::endl;
}

void IsLteEnhanceModeEnabled()
{
    int32_t slotId = SIM1_SLOTID;
    std::cout << "------IsLteEnhanceModeEnabled------" << std::endl;
    std::cout << "please input slot id:" << std::endl;
    std::cin >> slotId;
    int32_t ret = TELEPHONY_SUCCESS;
    if (g_clientPtr == nullptr) {
        std::cout << "g_clientPtr is nullptr" << std::endl;
        return;
    }
    ret = g_clientPtr->IsLteEnhanceModeEnabled(slotId);
    std::cout << "return value:" << ret << std::endl;
}

void StartRtt()
{
    int32_t callId = DEFAULT_CALL_ID;
    std::u16string msg;
    std::string tmpMsg;
    std::cout << "------StartRtt------" << std::endl;
    std::cout << "please input call id:" << std::endl;
    std::cin >> callId;
    std::cout << "please input Rtt msg:" << std::endl;
    msg.clear();
    tmpMsg.clear();
    std::cin >> tmpMsg;
    msg = Str8ToStr16(tmpMsg);
    if (g_clientPtr == nullptr) {
        std::cout << "g_clientPtr is nullptr" << std::endl;
        return;
    }
    int32_t ret = g_clientPtr->StartRtt(callId, msg);
    std::cout << "return value:" << ret << std::endl;
}

void StopRtt()
{
    int32_t callId = DEFAULT_CALL_ID;
    std::cout << "------StopRtt------" << std::endl;
    std::cout << "please input call id:" << std::endl;
    std::cin >> callId;
    if (g_clientPtr == nullptr) {
        std::cout << "g_clientPtr is nullptr" << std::endl;
        return;
    }
    int32_t ret = g_clientPtr->StopRtt(callId);
    std::cout << "return value:" << ret << std::endl;
}

void AddPermission()
{
    using namespace OHOS::Security::Permission;
    PermissionDef permissionDefAlpha = {.permissionName = TEST_PERMISSION_NAME_CAMERA,
        .bundleName = TEST_BUNDLE_NAME,
        .grantMode = GrantMode::USER_GRANT,
        .availableScope = AVAILABLE_SCOPE_ALL,
        .label = TEST_LABEL,
        .labelId = TEST_LABEL_ID,
        .description = TEST_DESCRIPTION,
        .descriptionId = TEST_DESCRIPTION_ID};

    PermissionDef permissionDefBeta = {.permissionName = TEST_PERMISSION_NAME_CAMERA,
        .bundleName = CALL_UI_BUNDLE_NAME,
        .grantMode = GrantMode::SYSTEM_GRANT,
        .availableScope = AVAILABLE_SCOPE_ALL,
        .label = TEST_LABEL,
        .labelId = TEST_LABEL_ID,
        .description = TEST_DESCRIPTION,
        .descriptionId = TEST_DESCRIPTION_ID};

    permDefList.emplace_back(permissionDefAlpha);
    permDefList.emplace_back(permissionDefBeta);
    PermissionKit::AddDefPermissions(permDefList);

    std::vector<std::string> permList;
    permList.push_back(TEST_PERMISSION_NAME_CAMERA);
    int32_t ret = PermissionKit::AddUserGrantedReqPermissions(TEST_BUNDLE_NAME, permList, TEST_USER_ID);
    std::cout << TEST_BUNDLE_NAME << " AddPermission AddUserGrantedReqPermissions return:" << ret << std::endl;
    ret = PermissionKit::GrantUserGrantedPermission(TEST_BUNDLE_NAME, TEST_PERMISSION_NAME_CAMERA, TEST_USER_ID);
    std::cout << TEST_BUNDLE_NAME << " AddPermission GrantUserGrantedPermission return:" << ret << std::endl;

    ret = PermissionKit::AddUserGrantedReqPermissions(CALL_UI_BUNDLE_NAME, permList, TEST_USER_ID);
    std::cout << CALL_UI_BUNDLE_NAME << " AddPermission AddUserGrantedReqPermissions return:" << ret << std::endl;
    ret = PermissionKit::GrantUserGrantedPermission(CALL_UI_BUNDLE_NAME, TEST_PERMISSION_NAME_CAMERA, TEST_USER_ID);
    std::cout << CALL_UI_BUNDLE_NAME << " AddPermission GrantUserGrantedPermission return:" << ret << std::endl;
}

void InitPermission()
{
    using namespace OHOS::Security::Permission;
    std::cout << "------InitPermission------" << std::endl;
    int32_t ret = PermissionKit::VerifyPermission(TEST_BUNDLE_NAME, TEST_PERMISSION_NAME_CAMERA, TEST_USER_ID);
    std::cout << "VerifyPermission return ret:" << ret << std::endl;

    int32_t retCallUI =
        PermissionKit::VerifyPermission(CALL_UI_BUNDLE_NAME, TEST_PERMISSION_NAME_CAMERA, TEST_USER_ID);
    std::cout << "VerifyPermission return retCallUI:" << retCallUI << std::endl;
    if (ret != PermissionKitRet::RET_SUCCESS || retCallUI != PermissionKitRet::RET_SUCCESS) {
        AddPermission();
    }
}

void VerifyPermission()
{
    using namespace OHOS::Security::Permission;
    std::cout << "------VerifyPermission------" << std::endl;
    int32_t ret = PermissionKit::VerifyPermission(TEST_BUNDLE_NAME, TEST_PERMISSION_NAME_CAMERA, TEST_USER_ID);
    if (ret == PermissionKitRet::RET_SUCCESS) {
        std::cout << TEST_BUNDLE_NAME << " VerifyPermission success:" << ret << std::endl;
    } else {
        std::cout << TEST_BUNDLE_NAME << " VerifyPermission failed:" << ret << std::endl;
    }

    ret = PermissionKit::VerifyPermission(CALL_UI_BUNDLE_NAME, TEST_PERMISSION_NAME_CAMERA, TEST_USER_ID);
    if (ret == PermissionKitRet::RET_SUCCESS) {
        std::cout << CALL_UI_BUNDLE_NAME << " VerifyPermission success:" << ret << std::endl;
    } else {
        std::cout << CALL_UI_BUNDLE_NAME << " VerifyPermission failed:" << ret << std::endl;
    }

    const std::string bundleName = "com.ohos.errorpkg";
    ret = PermissionKit::VerifyPermission(bundleName, TEST_PERMISSION_NAME_CAMERA, TEST_USER_ID);
    if (ret == PermissionKitRet::RET_SUCCESS) {
        std::cout << bundleName << " VerifyPermission success:" << ret << std::endl;
    } else {
        std::cout << bundleName << " VerifyPermission failed:" << ret << std::endl;
    }
}

void InitCallBasicPower()
{
    g_memberFuncMap[OHOS::Telephony::INTERFACE_DIAL_CALL] = &OHOS::Telephony::DialCall;
    g_memberFuncMap[OHOS::Telephony::INTERFACE_ANSWER_CALL] = &OHOS::Telephony::AnswerCall;
    g_memberFuncMap[OHOS::Telephony::INTERFACE_REJECT_CALL] = &OHOS::Telephony::RejectCall;
    g_memberFuncMap[OHOS::Telephony::INTERFACE_HOLD_CALL] = &OHOS::Telephony::HoldCall;
    g_memberFuncMap[OHOS::Telephony::INTERFACE_UNHOLD_CALL] = &OHOS::Telephony::UnHoldCall;
    g_memberFuncMap[OHOS::Telephony::INTERFACE_DISCONNECT_CALL] = &OHOS::Telephony::HangUpCall;
    g_memberFuncMap[OHOS::Telephony::INTERFACE_GET_CALL_STATE] = &OHOS::Telephony::GetCallState;
    g_memberFuncMap[OHOS::Telephony::INTERFACE_SWAP_CALL] = &OHOS::Telephony::SwitchCall;
    g_memberFuncMap[OHOS::Telephony::INTERFACE_START_RTT] = &OHOS::Telephony::StartRtt;
    g_memberFuncMap[OHOS::Telephony::INTERFACE_STOP_RTT] = &OHOS::Telephony::StopRtt;
}

void InitCallUtils()
{
    g_memberFuncMap[OHOS::Telephony::INTERFACE_HAS_CALL] = &OHOS::Telephony::HasCall;
    g_memberFuncMap[OHOS::Telephony::INTERFACE_IS_NEW_CALL_ALLOWED] = &OHOS::Telephony::IsNewCallAllowed;
    g_memberFuncMap[OHOS::Telephony::INTERFACE_IS_RINGING] = &OHOS::Telephony::IsRinging;
    g_memberFuncMap[OHOS::Telephony::INTERFACE_IS_EMERGENCY_CALL] = &OHOS::Telephony::IsInEmergencyCall;
    g_memberFuncMap[OHOS::Telephony::INTERFACE_IS_EMERGENCY_NUMBER] = &OHOS::Telephony::IsEmergencyPhoneNumber;
    g_memberFuncMap[OHOS::Telephony::INTERFACE_IS_FORMAT_NUMBER] = &OHOS::Telephony::FormatPhoneNumber;
    g_memberFuncMap[OHOS::Telephony::INTERFACE_IS_FORMAT_NUMBER_E164] = &OHOS::Telephony::FormatPhoneNumberToE164;
}

void InitCallConferencePower()
{
    g_memberFuncMap[OHOS::Telephony::INTERFACE_COMBINE_CONFERENCE] = &OHOS::Telephony::CombineConference;
    g_memberFuncMap[OHOS::Telephony::INTERFACE_SEPARATE_CONFERENCE] = &OHOS::Telephony::SeparateConference;
}

void InitCallDtmfPower()
{
    g_memberFuncMap[OHOS::Telephony::INTERFACE_START_DTMF] = &OHOS::Telephony::StartDtmf;
    g_memberFuncMap[OHOS::Telephony::INTERFACE_STOP_DTMF] = &OHOS::Telephony::StopDtmf;
}

void InitCallSupplementPower()
{
    g_memberFuncMap[OHOS::Telephony::INTERFACE_GET_CALL_WAITING] = &OHOS::Telephony::GetCallWaiting;
    g_memberFuncMap[OHOS::Telephony::INTERFACE_SET_CALL_WAITING] = &OHOS::Telephony::SetCallWaiting;
    g_memberFuncMap[OHOS::Telephony::INTERFACE_GET_CALL_RESTRICTION] = &OHOS::Telephony::GetCallRestriction;
    g_memberFuncMap[OHOS::Telephony::INTERFACE_SET_CALL_RESTRICTION] = &OHOS::Telephony::SetCallRestriction;
    g_memberFuncMap[OHOS::Telephony::INTERFACE_GET_CALL_TRANSFER] = &OHOS::Telephony::GetCallTransferInfo;
    g_memberFuncMap[OHOS::Telephony::INTERFACE_SET_CALL_TRANSFER] = &OHOS::Telephony::SetCallTransferInfo;
    g_memberFuncMap[OHOS::Telephony::INTERFACE_SETCALL_PREFERENCEMODE] = &OHOS::Telephony::SetCallPreferenceMode;
}

void initCallConferenceExPower()
{
    g_memberFuncMap[OHOS::Telephony::INTERFACE_GET_MAINID] = &OHOS::Telephony::GetMainCallId;
    g_memberFuncMap[OHOS::Telephony::INTERFACE_GET_SUBCALL_LIST_ID] = &OHOS::Telephony::GetSubCallIdList;
    g_memberFuncMap[OHOS::Telephony::INTERFACE_GET_CALL_LIST_ID_FOR_CONFERENCE] =
        &OHOS::Telephony::GetCallIdListForConference;
    g_memberFuncMap[OHOS::Telephony::INTERFACE_JOIN_CONFERENCE] = &OHOS::Telephony::InviteToConference;
}

void InitCallMultimediaPower()
{
    g_memberFuncMap[OHOS::Telephony::INTERFACE_SET_MUTE] = &OHOS::Telephony::SetMute;
    g_memberFuncMap[OHOS::Telephony::INTERFACE_MUTE_RINGER] = &OHOS::Telephony::MuteRinger;
    g_memberFuncMap[OHOS::Telephony::INTERFACE_SET_AUDIO_DEVICE] = &OHOS::Telephony::SetAudioDevice;
    g_memberFuncMap[OHOS::Telephony::INTERFACE_CTRL_CAMERA] = &OHOS::Telephony::ControlCamera;
    g_memberFuncMap[OHOS::Telephony::INTERFACE_SET_PREVIEW_WINDOW] = &OHOS::Telephony::SetPreviewWindow;
    g_memberFuncMap[OHOS::Telephony::INTERFACE_SET_DISPLAY_WINDOW] = &OHOS::Telephony::SetDisplayWindow;
    g_memberFuncMap[OHOS::Telephony::INTERFACE_SET_CAMERA_ZOOM] = &OHOS::Telephony::SetCameraZoom;
    g_memberFuncMap[OHOS::Telephony::INTERFACE_SET_PAUSE_IMAGE] = &OHOS::Telephony::SetPausePicture;
    g_memberFuncMap[OHOS::Telephony::INTERFACE_SET_DEVICE_DIRECTION] = &OHOS::Telephony::SetDeviceDirection;
    g_memberFuncMap[OHOS::Telephony::INTERFACE_UPDATE_CALL_MEDIA_MODE] = &OHOS::Telephony::UpdateCallMediaMode;

    g_memberFuncMap[DEFINE_INIT_PERMISSIONS] = &OHOS::Telephony::InitPermission;
    g_memberFuncMap[DEFINE_VERIFY_PERMISSIONS] = &OHOS::Telephony::VerifyPermission;
}

void InitImsServicePower()
{
    g_memberFuncMap[OHOS::Telephony::INTERFACE_GET_IMS_CONFIG] = &OHOS::Telephony::GetImsConfig;
    g_memberFuncMap[OHOS::Telephony::INTERFACE_SET_IMS_CONFIG] = &OHOS::Telephony::SetImsConfig;
    g_memberFuncMap[OHOS::Telephony::INTERFACE_GET_IMS_FEATURE_VALUE] = &OHOS::Telephony::GetImsFeatureValue;
    g_memberFuncMap[OHOS::Telephony::INTERFACE_SET_IMS_FEATURE_VALUE] = &OHOS::Telephony::SetImsFeatureValue;
    g_memberFuncMap[OHOS::Telephony::INTERFACE_ENABLE_VOLTE] = &OHOS::Telephony::EnableVoLte;
    g_memberFuncMap[OHOS::Telephony::INTERFACE_DISABLE_VOLTE] = &OHOS::Telephony::DisableVoLte;
    g_memberFuncMap[OHOS::Telephony::INTERFACE_IS_VOLTE_ENABLED] = &OHOS::Telephony::IsVoLteEnabled;
    g_memberFuncMap[OHOS::Telephony::INTERFACE_ENABLE_LTE_ENHANCE_MODE] = &OHOS::Telephony::EnableLteEnhanceMode;
    g_memberFuncMap[OHOS::Telephony::INTERFACE_DISABLE_LTE_ENHANCE_MODE] = &OHOS::Telephony::DisableLteEnhanceMode;
    g_memberFuncMap[OHOS::Telephony::INTERFACE_IS_LTE_ENHANCE_MODE_ENABLED] =
        &OHOS::Telephony::IsLteEnhanceModeEnabled;
}

void InitBluetooth()
{
    g_memberFuncMap[DEFINE_CONNECT_BT_SCO] = &OHOS::Telephony::SendConnectBluetoothScoBroadcast;
    g_memberFuncMap[DEFINE_DISCONNECT_BT_SCO] = &OHOS::Telephony::SendDisconnectBluetoothScoBroadcast;
    g_memberFuncMap[DEFINE_SUBSCRIBERCOMMON_EVENT] = &OHOS::Telephony::SubscribeCommonEvent;
}

int32_t Init()
{
    g_clientPtr = DelayedSingleton<CallManagerClient>::GetInstance();
    if (g_clientPtr == nullptr) {
        std::cout << "g_clientPtr is nullptr" << std::endl;
        return TELEPHONY_ERROR;
    }
    std::u16string bundleName = Str8ToStr16(TEST_BUNDLE_NAME);
    g_clientPtr->Init(TELEPHONY_CALL_MANAGER_SYS_ABILITY_ID, bundleName);
    std::unique_ptr<CallManagerCallbackTest> callbackPtr = std::make_unique<CallManagerCallbackTest>();
    if (callbackPtr == nullptr) {
        std::cout << "make_unique NapiCallManagerCallback failed!" << std::endl;
        return TELEPHONY_ERROR;
    }
    int32_t ret = g_clientPtr->RegisterCallBack(std::move(callbackPtr));
    if (ret != TELEPHONY_SUCCESS) {
        std::cout << "RegisterCallBack failed!" << std::endl;
        return TELEPHONY_ERROR;
    }
    std::cout << "RegisterCallBack success!" << std::endl;
    InitCallBasicPower();
    InitCallUtils();
    InitCallConferencePower();
    InitCallDtmfPower();
    InitCallSupplementPower();
    initCallConferenceExPower();
    InitCallMultimediaPower();
    InitImsServicePower();
    return TELEPHONY_SUCCESS;
}

void PrintfCallBasisInterface()
{
    std::cout << "\n\n-----------start--------------\n"
              << "usage:please input a cmd num:\n"
              << "2:dial\n"
              << "3:answer\n"
              << "4:reject\n"
              << "5:hold\n"
              << "6:unhold\n"
              << "7:hangUpCall\n"
              << "8:getCallState\n"
              << "9:switchCall\n";
}

void PrintfCallUtilsInterface()
{
    std::cout << "10:hasCall\n"
              << "11:isNewCallAllowed\n"
              << "12:isRinging\n"
              << "13:isInEmergencyCall\n"
              << "14:isEmergencyPhoneNumber\n"
              << "15:formatPhoneNumber\n"
              << "16:formatPhoneNumberToE164\n";
}

void PrintfCallConferenceInterface()
{
    std::cout << "17:combine conference\n"
              << "18:separate conference\n";
}

void PrintfCallDtmfInterface()
{
    std::cout << "19:StartDtmf\n"
              << "20:StopDtmf\n";
}

void PrintfCallSupplementInterface()
{
    std::cout << "21:getCallWaiting\n"
              << "22:setCallWaiting\n"
              << "23:getCallRestriction\n"
              << "24:setCallRestriction\n"
              << "25:getCallTransferInfo\n"
              << "26:setCallTransferInfo\n";
}

void PrintfCallConferenceExInterface()
{
    std::cout << "27:GetMainCallId\n"
              << "28:GetSubCallIdList\n"
              << "29:GetCallIdListForConference\n";
}

void PrintfCallMultimediaInterface()
{
    std::cout << "30:SetMute\n"
              << "31:MuteRinger\n"
              << "32:SetAudioDevice\n"
              << "33:ControlCamera\n"
              << "34:SetPreviewWindow\n"
              << "35:SetDisplayWindow\n"
              << "36:SetCameraZoom\n"
              << "37:SetPausePicture\n"
              << "38:SetDeviceDirection\n"
              << "39:SetCallPreferenceMode\n"
              << "40:GetImsConfig\n"
              << "41:SetImsConfig\n"
              << "42:GetImsNetworkValue\n"
              << "43:SetImsNetworkValue\n"
              << "44:UpdateCallMediaMode\n"
              << "45:EnableVoLte\n"
              << "46:DisableVoLte\n"
              << "47:IsVoLteEnabled\n"
              << "48:EnableLteEnhanceMode\n"
              << "49:DisableLteEnhanceMode\n"
              << "50:IsLteEnhanceModeEnabled\n"
              << "51:StartRtt\n"
              << "52:StopRtt\n"
              << "93:InitPermission\n"
              << "94:VerifyPermission\n"
              << "95:SendConnectBluetoothScoBroadcast\n"
              << "96:SendDisconnectBluetoothScoBroadcast\n"
              << "97:SubscribeCommonEvent\n"
              << "98:GetVolume\n"
              << "99:SetVolume\n"
              << "100:PlayRintone\n";
}

void PrintfUsage()
{
    PrintfCallBasisInterface();
    PrintfCallUtilsInterface();
    PrintfCallConferenceInterface();
    PrintfCallDtmfInterface();
    PrintfCallSupplementInterface();
    PrintfCallConferenceExInterface();
    PrintfCallMultimediaInterface();
    std::cout << "1000:exit\n";
}

int32_t mainExit()
{
    if (OHOS::Telephony::g_clientPtr == nullptr) {
        std::cout << "g_clientPtr is nullptr" << std::endl;
        return OHOS::Telephony::TELEPHONY_ERR_FAIL;
    }
    OHOS::Telephony::g_memberFuncMap.clear();
    OHOS::Telephony::g_clientPtr->UnInit();
    std::cout << "exit success" << std::endl;
    return OHOS::Telephony::TELEPHONY_SUCCESS;
}
} // namespace Telephony
} // namespace OHOS

int32_t main()
{
    std::cout << "callManager test start...." << std::endl;
    int32_t interfaceNum = OHOS::Telephony::DEFAULT_VALUE;
    const int32_t exitNumber = 1000;
    const int32_t getVolumeNumber = 98;
    const int32_t setVolumeNumber = 99;
    const int32_t playRingtoneNumber = 100;
    if (OHOS::Telephony::Init() != OHOS::Telephony::TELEPHONY_SUCCESS) {
        std::cout << "callManager test init failed!" << std::endl;
        return OHOS::Telephony::TELEPHONY_SUCCESS;
    }
    while (true) {
        OHOS::Telephony::PrintfUsage();
        std::cin >> interfaceNum;
        if (interfaceNum == exitNumber) {
            std::cout << "start to exit now...." << std::endl;
            break;
        } else if (interfaceNum == playRingtoneNumber) {
            OHOS::Telephony::PlayRingtone();
            continue;
        } else if (interfaceNum == setVolumeNumber) {
            OHOS::Telephony::SetVolume();
            continue;
        } else if (interfaceNum == getVolumeNumber) {
            OHOS::Telephony::GetVolume();
            continue;
        }
        auto itFunc = OHOS::Telephony::g_memberFuncMap.find(interfaceNum);
        if (itFunc != OHOS::Telephony::g_memberFuncMap.end() && itFunc->second != nullptr) {
            auto memberFunc = itFunc->second;
            (*memberFunc)();
            continue;
        }
        std::cout << "err: invalid input!" << std::endl;
    }
    return OHOS::Telephony::mainExit();
}
