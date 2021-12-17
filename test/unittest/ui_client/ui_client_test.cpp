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
constexpr int32_t MIN_VOLUME = 0;
constexpr int32_t MAX_VOLUME = 15;
constexpr int32_t READ_SIZE = 1;
constexpr int32_t MIN_BYTES = 4;
constexpr int32_t RING_PATH_MAX_LENGTH = 100;
constexpr int32_t SIM1_SLOTID = 0;
constexpr int32_t DEFAULT_ACCOUNT_ID = 0;
constexpr int32_t DEFAULT_VIDEO_STATE = 0;
constexpr int32_t DEFAULT_DIAL_SCENE = 0;
constexpr int32_t DEFAULT_DIAL_TYPE = 0;
constexpr int32_t DEFAULT_CALL_TYPE = 0;
constexpr int32_t DEFAULT_CALL_ID = 0;
constexpr int32_t DEFAULT_VALUE = 0;
constexpr size_t DEFAULT_SIZE = 0;
constexpr int32_t WINDOWS_X_START = 0;
constexpr int32_t WINDOWS_Y_START = 0;
constexpr int32_t WINDOWS_Z_ERROR = -1;
constexpr int32_t WINDOWS_Z_TOP = 1;
constexpr int32_t WINDOWS_Z_BOTTOM = 0;
constexpr int32_t WINDOWS_WIDTH = 200;
constexpr int32_t WINDOWS_HEIGHT = 200;
constexpr size_t DEFAULT_PREFERENCEMODE = 3;

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
    int callId = DEFAULT_CALL_ID;
    int videoState = DEFAULT_VIDEO_STATE;
    std::cout << "------Answer------" << std::endl;
    std::cout << "please input callId:" << std::endl;
    std::cin >> callId;
    std::cout << "please input videoState[0:audio,1:video]:" << std::endl;
    std::cin >> videoState;
    int ret = TELEPHONY_SUCCESS;
    if (g_clientPtr == nullptr) {
        std::cout << "g_clientPtr is nullptr" << std::endl;
        return;
    }
    ret = g_clientPtr->AnswerCall(callId, videoState);
    std::cout << "return value:" << ret << std::endl;
}

void RejectCall()
{
    int callId = DEFAULT_CALL_ID;
    int boolValue = DEFAULT_VALUE;
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
    int callId = DEFAULT_CALL_ID;
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
    int callId = DEFAULT_CALL_ID;
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
    int callId = DEFAULT_CALL_ID;
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
    int mainCallId = DEFAULT_CALL_ID;
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
    int callId = DEFAULT_CALL_ID;
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
    int callId = DEFAULT_CALL_ID;
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
    int callId = DEFAULT_CALL_ID;
    std::cout << "please input StartDtmf callId:" << std::endl;
    std::cin >> callId;
    std::cout << "Please enter to send dtmf characters:" << std::endl;
    std::cin >> c;
    int32_t ret = g_clientPtr->StartDtmf(callId, c);
    std::cout << "return value:" << ret << std::endl;
}

void StopDtmf()
{
    int callId = DEFAULT_CALL_ID;
    std::cout << "please input StopDtmf callId:" << std::endl;
    std::cin >> callId;
    int32_t ret = g_clientPtr->StopDtmf(callId);
    std::cout << "return value:" << ret << std::endl;
}

void SendDtmf()
{
    char c = DEFAULT_VALUE;
    int callId = DEFAULT_CALL_ID;
    std::cout << "please input SendDtmf callId:" << std::endl;
    std::cin >> callId;
    std::cout << "Please enter to send dtmf characters:" << std::endl;
    std::cin >> c;
    int32_t ret = g_clientPtr->SendDtmf(callId, c);
    std::cout << "return value:" << ret << std::endl;
}

void SendBurstDtmf()
{
    std::u16string strCode;
    std::string tmpStr;
    int callId = DEFAULT_CALL_ID;
    int on = DEFAULT_VALUE;
    int off = DEFAULT_VALUE;
    std::cout << "please input SendBurstDtmf callId:" << std::endl;
    std::cin >> callId;
    std::cout << "please input Send dtmf string:" << std::endl;
    strCode.clear();
    tmpStr.clear();
    std::cin >> tmpStr;
    strCode = Str8ToStr16(tmpStr);
    std::cout << "please input on:" << std::endl;
    std::cin >> on;
    std::cout << "please input off:" << std::endl;
    std::cin >> off;
    int32_t ret = g_clientPtr->SendBurstDtmf(callId, strCode, on, off);
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
    int callId = DEFAULT_CALL_ID;
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
    int callId = DEFAULT_CALL_ID;
    std::cout << "please input callId:" << std::endl;
    std::cin >> callId;
    std::vector<std::u16string> ret = g_clientPtr->GetCallIdListForConference(callId);
    std::vector<std::u16string>::iterator it = ret.begin();
    for (; it != ret.end(); it++) {
        std::cout << "callId:" << Str16ToStr8(*it) << std::endl;
    }
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
    switch (deviceType) {
        case AudioDevice::DEVICE_EARPIECE:
            device = AudioDevice::DEVICE_EARPIECE;
            break;
        case AudioDevice::DEVICE_SPEAKER:
            device = AudioDevice::DEVICE_SPEAKER;
            break;
        case AudioDevice::DEVICE_WIRED_HEADSET:
            device = AudioDevice::DEVICE_WIRED_HEADSET;
            break;
        case AudioDevice::DEVICE_BLUETOOTH_SCO:
            device = AudioDevice::DEVICE_BLUETOOTH_SCO;
            break;
        default:
            break;
    }
    int32_t ret = g_clientPtr->SetAudioDevice(device);
    std::cout << "return value:" << ret << std::endl;
}

void GetVolume()
{
    int32_t type = DEFAULT_VALUE;
    std::cout << "------GetVolume------" << std::endl;
    std::cout << "please input volume type(3:ring 4:music)" << std::endl;
    std::cin >> type;
    AudioSystemManager::AudioVolumeType volumeType = AudioSystemManager::AudioVolumeType::STREAM_MUSIC;
    switch (type) {
        case AudioSystemManager::AudioVolumeType::STREAM_RING:
            volumeType = AudioSystemManager::AudioVolumeType::STREAM_RING;
            break;
        case AudioSystemManager::AudioVolumeType::STREAM_MUSIC:
            volumeType = AudioSystemManager::AudioVolumeType::STREAM_MUSIC;
            break;
        default:
            break;
    }
    AudioSystemManager *audioSystemMgr = AudioSystemManager::GetInstance();
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
    AudioSystemManager::AudioVolumeType volumeType = AudioSystemManager::AudioVolumeType::STREAM_MUSIC;
    switch (type) {
        case AudioSystemManager::AudioVolumeType::STREAM_RING:
            volumeType = AudioSystemManager::AudioVolumeType::STREAM_RING;
            break;
        case AudioSystemManager::AudioVolumeType::STREAM_MUSIC:
            volumeType = AudioSystemManager::AudioVolumeType::STREAM_MUSIC;
            break;
        default:
            break;
    }
    AudioSystemManager *audioSystemMgr = AudioSystemManager::GetInstance();
    int32_t ret = audioSystemMgr->SetVolume(volumeType, volume);
    std::cout << "return value:" << ret << std::endl;
}

bool InitRenderer(const std::unique_ptr<AudioRenderer> &audioRenderer, const wav_hdr &wavHeader)
{
    AudioRendererParams rendererParams;
    rendererParams.sampleFormat = static_cast<AudioSampleFormat>(wavHeader.bitsPerSample);
    rendererParams.sampleRate = static_cast<AudioSamplingRate>(wavHeader.SamplesPerSec);
    rendererParams.channelCount = static_cast<AudioChannel>(wavHeader.NumOfChan);
    rendererParams.encodingType = static_cast<AudioEncodingType>(ENCODING_PCM);
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
    std::cout << "------PlayRingtone------" << std::endl;
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
    std::unique_ptr<AudioRenderer> audioRenderer = AudioRenderer::Create(AudioStreamType::STREAM_MUSIC);
    if (!InitRenderer(audioRenderer, wavHeader)) {
        return false;
    }
    size_t bufferLen;
    if (audioRenderer->GetBufferSize(bufferLen)) {
        return false;
    }
    std::unique_ptr<uint8_t> buffer = std::make_unique<uint8_t>(bufferLen + bufferLen);
    if (buffer == nullptr) {
        std::cout << "malloc memory nullptr" << std::endl;
        return false;
    }
    size_t bytesToWrite = DEFAULT_SIZE;
    size_t bytesWritten = DEFAULT_SIZE;
    uint64_t latency;
    while (!feof(wavFile)) {
        bytesToWrite = fread(buffer.get(), READ_SIZE, bufferLen, wavFile);
        bytesWritten = DEFAULT_SIZE;
        if (audioRenderer->GetLatency(latency)) {
            break;
        }
        while ((bytesWritten < bytesToWrite) && ((bytesToWrite - bytesWritten) > MIN_BYTES)) {
            bytesWritten += audioRenderer->Write(buffer.get() + bytesWritten, bytesToWrite - bytesWritten);
            std::cout << "audio bytes written : " << bytesWritten << std::endl;
            if (bytesWritten < DEFAULT_SIZE) {
                break;
            }
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

void CancelMissedCallsNotification()
{
    int32_t notificationId = DEFAULT_VALUE;
    std::cout << "------CancelMissedCallsNotification------" << std::endl;
    std::cout << "please input notification id : " << std::endl;
    std::cin >> notificationId;
    if (g_clientPtr == nullptr) {
        std::cout << "g_clientPtr is nullptr" << std::endl;
        return;
    } else if (notificationId < DEFAULT_VALUE) {
        std::cout << "notification id should be greater than or equal to zero" << std::endl;
        return;
    }
    int32_t ret = g_clientPtr->CancelMissedCallsNotification(notificationId);
    std::cout << "return value:" << ret << std::endl;
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

void InitCallBasicPower()
{
    g_memberFuncMap[(uint32_t)OHOS::Telephony::CallManagerSurfaceCode::INTERFACE_DIAL_CALL] =
        &OHOS::Telephony::DialCall;
    g_memberFuncMap[(uint32_t)OHOS::Telephony::CallManagerSurfaceCode::INTERFACE_ANSWER_CALL] =
        &OHOS::Telephony::AnswerCall;
    g_memberFuncMap[(uint32_t)OHOS::Telephony::CallManagerSurfaceCode::INTERFACE_REJECT_CALL] =
        &OHOS::Telephony::RejectCall;
    g_memberFuncMap[(uint32_t)OHOS::Telephony::CallManagerSurfaceCode::INTERFACE_HOLD_CALL] =
        &OHOS::Telephony::HoldCall;
    g_memberFuncMap[(uint32_t)OHOS::Telephony::CallManagerSurfaceCode::INTERFACE_UNHOLD_CALL] =
        &OHOS::Telephony::UnHoldCall;
    g_memberFuncMap[(uint32_t)OHOS::Telephony::CallManagerSurfaceCode::INTERFACE_DISCONNECT_CALL] =
        &OHOS::Telephony::HangUpCall;
    g_memberFuncMap[(uint32_t)OHOS::Telephony::CallManagerSurfaceCode::INTERFACE_GET_CALL_STATE] =
        &OHOS::Telephony::GetCallState;
    g_memberFuncMap[(uint32_t)OHOS::Telephony::CallManagerSurfaceCode::INTERFACE_SWAP_CALL] =
        &OHOS::Telephony::SwitchCall;
}

void InitCallUtils()
{
    g_memberFuncMap[(uint32_t)OHOS::Telephony::CallManagerSurfaceCode::INTERFACE_HAS_CALL] =
        &OHOS::Telephony::HasCall;
    g_memberFuncMap[(uint32_t)OHOS::Telephony::CallManagerSurfaceCode::INTERFACE_IS_NEW_CALL_ALLOWED] =
        &OHOS::Telephony::IsNewCallAllowed;
    g_memberFuncMap[(uint32_t)OHOS::Telephony::CallManagerSurfaceCode::INTERFACE_IS_RINGING] =
        &OHOS::Telephony::IsRinging;
    g_memberFuncMap[(uint32_t)OHOS::Telephony::CallManagerSurfaceCode::INTERFACE_IS_EMERGENCY_CALL] =
        &OHOS::Telephony::IsInEmergencyCall;
    g_memberFuncMap[(uint32_t)OHOS::Telephony::CallManagerSurfaceCode::INTERFACE_IS_EMERGENCY_NUMBER] =
        &OHOS::Telephony::IsEmergencyPhoneNumber;
    g_memberFuncMap[(uint32_t)OHOS::Telephony::CallManagerSurfaceCode::INTERFACE_IS_FORMAT_NUMBER] =
        &OHOS::Telephony::FormatPhoneNumber;
    g_memberFuncMap[(uint32_t)OHOS::Telephony::CallManagerSurfaceCode::INTERFACE_IS_FORMAT_NUMBER_E164] =
        &OHOS::Telephony::FormatPhoneNumberToE164;
}

void InitCallConferencePower()
{
    g_memberFuncMap[(uint32_t)OHOS::Telephony::CallManagerSurfaceCode::INTERFACE_COMBINE_CONFERENCE] =
        &OHOS::Telephony::CombineConference;
    g_memberFuncMap[(uint32_t)OHOS::Telephony::CallManagerSurfaceCode::INTERFACE_SEPARATE_CONFERENCE] =
        &OHOS::Telephony::SeparateConference;
}

void InitCallDtmfPower()
{
    g_memberFuncMap[(uint32_t)OHOS::Telephony::CallManagerSurfaceCode::INTERFACE_START_DTMF] =
        &OHOS::Telephony::StartDtmf;
    g_memberFuncMap[(uint32_t)OHOS::Telephony::CallManagerSurfaceCode::INTERFACE_STOP_DTMF] =
        &OHOS::Telephony::StopDtmf;
    g_memberFuncMap[(uint32_t)OHOS::Telephony::CallManagerSurfaceCode::INTERFACE_SEND_DTMF] =
        &OHOS::Telephony::SendDtmf;
    g_memberFuncMap[(uint32_t)OHOS::Telephony::CallManagerSurfaceCode::INTERFACE_SEND_DTMF_BUNCH] =
        &OHOS::Telephony::SendBurstDtmf;
}

void InitCallSupplementPower()
{
    g_memberFuncMap[(uint32_t)OHOS::Telephony::CallManagerSurfaceCode::INTERFACE_GET_CALL_WAITING] =
        &OHOS::Telephony::GetCallWaiting;
    g_memberFuncMap[(uint32_t)OHOS::Telephony::CallManagerSurfaceCode::INTERFACE_SET_CALL_WAITING] =
        &OHOS::Telephony::SetCallWaiting;
    g_memberFuncMap[(uint32_t)OHOS::Telephony::CallManagerSurfaceCode::INTERFACE_GET_CALL_RESTRICTION] =
        &OHOS::Telephony::GetCallRestriction;
    g_memberFuncMap[(uint32_t)OHOS::Telephony::CallManagerSurfaceCode::INTERFACE_SET_CALL_RESTRICTION] =
        &OHOS::Telephony::SetCallRestriction;
    g_memberFuncMap[(uint32_t)OHOS::Telephony::CallManagerSurfaceCode::INTERFACE_GET_CALL_TRANSFER] =
        &OHOS::Telephony::GetCallTransferInfo;
    g_memberFuncMap[(uint32_t)OHOS::Telephony::CallManagerSurfaceCode::INTERFACE_SET_CALL_TRANSFER] =
        &OHOS::Telephony::SetCallTransferInfo;
    g_memberFuncMap[(uint32_t)OHOS::Telephony::CallManagerSurfaceCode::INTERFACE_SETCALL_PREFERENCEMODE] =
        &OHOS::Telephony::SetCallPreferenceMode;
}

void initCallConferenceExPower()
{
    g_memberFuncMap[(uint32_t)OHOS::Telephony::CallManagerSurfaceCode::INTERFACE_GET_MAINID] =
        &OHOS::Telephony::GetMainCallId;
    g_memberFuncMap[(uint32_t)OHOS::Telephony::CallManagerSurfaceCode::INTERFACE_GET_SUBCALL_LIST_ID] =
        &OHOS::Telephony::GetSubCallIdList;
    g_memberFuncMap[(uint32_t)OHOS::Telephony::CallManagerSurfaceCode::INTERFACE_GET_CALL_LIST_ID_FOR_CONFERENCE] =
        &OHOS::Telephony::GetCallIdListForConference;
}

void InitCallMultimediaPower()
{
    g_memberFuncMap[(uint32_t)OHOS::Telephony::CallManagerSurfaceCode::INTERFACE_SET_MUTE] =
        &OHOS::Telephony::SetMute;
    g_memberFuncMap[(uint32_t)OHOS::Telephony::CallManagerSurfaceCode::INTERFACE_MUTE_RINGER] =
        &OHOS::Telephony::MuteRinger;
    g_memberFuncMap[(uint32_t)OHOS::Telephony::CallManagerSurfaceCode::INTERFACE_SET_AUDIO_DEVICE] =
        &OHOS::Telephony::SetAudioDevice;
    g_memberFuncMap[(uint32_t)OHOS::Telephony::CallManagerSurfaceCode::INTERFACE_CANCEL_MISSED_CALLS_NOTIFICATION] =
        &OHOS::Telephony::CancelMissedCallsNotification;
    g_memberFuncMap[(uint32_t)OHOS::Telephony::CallManagerSurfaceCode::INTERFACE_CTRL_CAMERA] =
        &OHOS::Telephony::ControlCamera;
    g_memberFuncMap[(uint32_t)OHOS::Telephony::CallManagerSurfaceCode::INTERFACE_SET_PREVIEW_WINDOW] =
        &OHOS::Telephony::SetPreviewWindow;
    g_memberFuncMap[(uint32_t)OHOS::Telephony::CallManagerSurfaceCode::INTERFACE_SET_DISPLAY_WINDOW] =
        &OHOS::Telephony::SetDisplayWindow;
    g_memberFuncMap[(uint32_t)OHOS::Telephony::CallManagerSurfaceCode::INTERFACE_SET_CAMERA_ZOOM] =
        &OHOS::Telephony::SetCameraZoom;
    g_memberFuncMap[(uint32_t)OHOS::Telephony::CallManagerSurfaceCode::INTERFACE_SET_PAUSE_PICTURE] =
        &OHOS::Telephony::SetPausePicture;
    g_memberFuncMap[(uint32_t)OHOS::Telephony::CallManagerSurfaceCode::INTERFACE_SET_DEVICE_DIRECTION] =
        &OHOS::Telephony::SetDeviceDirection;
}

int32_t Init()
{
    g_clientPtr = DelayedSingleton<CallManagerClient>::GetInstance();
    if (g_clientPtr == nullptr) {
        std::cout << "g_clientPtr is nullptr" << std::endl;
        return TELEPHONY_ERROR;
    }
    std::u16string bundleName = Str8ToStr16("com.ohos.callManagerTest");
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
              << "20:StopDtmf\n"
              << "21:SendDtmf\n"
              << "22:SendBurstDtmf\n";
}

void PrintfCallSupplementInterface()
{
    std::cout << "23:getCallWaiting\n"
              << "24:setCallWaiting\n"
              << "25:getCallRestriction\n"
              << "26:setCallRestriction\n"
              << "27:getCallTransferInfo\n"
              << "28:setCallTransferInfo\n";
}

void PrintfCallConferenceExInterface()
{
    std::cout << "29:GetMainCallId\n"
              << "30:GetSubCallIdList\n"
              << "31:GetCallIdListForConference\n";
}

void PrintfCallMultimediaInterface()
{
    std::cout << "32:SetMute\n"
              << "33:MuteRinger\n"
              << "34:SetAudioDevice\n"
              << "35:CancelMissedCallsNotification\n"
              << "36:ControlCamera\n"
              << "37:SetPreviewWindow\n"
              << "38:SetDisplayWindow\n"
              << "39:SetCameraZoom\n"
              << "40:SetPausePicture\n"
              << "41:SetDeviceDirection\n"
              << "42:SetCallPreferenceMode\n"
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

int MainExit()
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

int main()
{
    std::cout << "callManager test start...." << std::endl;
    int32_t interfaceNum = OHOS::Telephony::DEFAULT_VALUE;
    const int32_t exitNumber = 1000;
    const int32_t subscribeCommonEvent = 97;
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
        } else if (interfaceNum == subscribeCommonEvent) {
            OHOS::Telephony::SubscribeCommonEvent();
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
    return OHOS::Telephony::MainExit();
}
