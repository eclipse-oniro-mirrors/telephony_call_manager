/*
 * Copyright (C) 2021-2024 Huawei Device Co., Ltd.
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

#include "audio_control_manager.h"

#include "call_ability_report_proxy.h"
#include "call_control_manager.h"
#include "call_dialog.h"
#include "call_state_processor.h"
#include "common_type.h"
#include "distributed_call_manager.h"
#include "telephony_log_wrapper.h"
#include "audio_system_manager.h"
#include "audio_routing_manager.h"
#include "audio_device_info.h"
#include "audio_info.h"
#include "audio_device_descriptor.h"
#include "voip_call_connection.h"
#include "settings_datashare_helper.h"
#include "distributed_communication_manager.h"
#include "os_account_manager.h"
#include "ringtone_player.h"
#include "int_wrapper.h"

namespace OHOS {
namespace Telephony {
using namespace AudioStandard;
constexpr int32_t DTMF_PLAY_TIME = 30;
constexpr int32_t VOICE_TYPE = 0;
constexpr int32_t CRS_TYPE = 2;
constexpr int32_t CALL_ENDED_PLAY_TIME = 300;

AudioControlManager::AudioControlManager()
    : isLocalRingbackNeeded_(false), ring_(nullptr), tone_(nullptr), sound_(nullptr)
{}

AudioControlManager::~AudioControlManager()
{}

void AudioControlManager::Init()
{
    DelayedSingleton<AudioDeviceManager>::GetInstance()->Init();
    DelayedSingleton<AudioSceneProcessor>::GetInstance()->Init();
}

void AudioControlManager::UnInit()
{
    DelayedSingleton<AudioProxy>::GetInstance()->UnsetDeviceChangeCallback();
    DelayedSingleton<AudioProxy>::GetInstance()->UnsetAudioPreferDeviceChangeCallback();
    DelayedSingleton<AudioProxy>::GetInstance()->UnsetAudioMicStateChangeCallback();
}

void AudioControlManager::UpdateForegroundLiveCall()
{
    int32_t callId = DelayedSingleton<CallStateProcessor>::GetInstance()->GetAudioForegroundLiveCall();
    if (callId == INVALID_CALLID) {
        frontCall_ = nullptr;
        DelayedSingleton<AudioProxy>::GetInstance()->SetMicrophoneMute(false);
        TELEPHONY_LOGE("callId is invalid");
        return;
    }

    sptr<CallBase> liveCall = CallObjectManager::GetOneCallObject(callId);
    if (liveCall == nullptr) {
        TELEPHONY_LOGE("liveCall is nullptr");
        return;
    }
    if (liveCall->GetTelCallState() == TelCallState::CALL_STATUS_ACTIVE ||
        liveCall->GetTelCallState() == TelCallState::CALL_STATUS_DIALING ||
        liveCall->GetTelCallState() == TelCallState::CALL_STATUS_ALERTING) {
        if (frontCall_ == nullptr) {
            frontCall_ = liveCall;
        } else {
            int32_t frontCallId = frontCall_->GetCallID();
            int32_t liveCallId = liveCall->GetCallID();
            if (frontCallId != liveCallId || (frontCall_->GetCallIndex() == 0 && liveCall->GetCallIndex() != 0)) {
                frontCall_ = liveCall;
            }
        }
        bool frontCallMute = frontCall_->IsMuted();
        bool currentMute = DelayedSingleton<AudioProxy>::GetInstance()->IsMicrophoneMute();
        if (frontCallMute != currentMute) {
            SetMute(frontCallMute);
        }
    }
}

void AudioControlManager::HandleCallStateUpdatedForVoip(
    sptr<CallBase> &callObjectPtr, TelCallState priorState, TelCallState nextState)
{
    TELEPHONY_LOGI("control audio for voip start, callId:%{public}d, priorState:%{public}d, nextState:%{public}d",
        callObjectPtr->GetCallID(), priorState, nextState);
    if (priorState == TelCallState::CALL_STATUS_INCOMING && nextState == TelCallState::CALL_STATUS_INCOMING) {
        if (DelayedSingleton<CallObjectManager>::GetInstance()->GetVoipCallNum() == 1) {
            AudioDevice device = {
                .deviceType = AudioDeviceType::DEVICE_EARPIECE,
                .address = { 0 },
            };
            if (DelayedSingleton<AudioProxy>::GetInstance()->GetPreferredOutputAudioDevice(device, true) ==
                TELEPHONY_SUCCESS) {
                DelayedSingleton<AudioDeviceManager>::GetInstance()->SetCurrentAudioDevice(device.deviceType);
                TELEPHONY_LOGI("control audio for voip finish, callId:%{public}d", callObjectPtr->GetCallID());
            } else {
                TELEPHONY_LOGE("current audio device nullptr when control audio for voip");
            }
        }
    }
}

void AudioControlManager::CallStateUpdated(
    sptr<CallBase> &callObjectPtr, TelCallState priorState, TelCallState nextState)
{
    if (callObjectPtr == nullptr) {
        TELEPHONY_LOGE("call object nullptr");
        return;
    }
    if (callObjectPtr->GetCallType() == CallType::TYPE_VOIP) {
        HandleCallStateUpdatedForVoip(callObjectPtr, priorState, nextState);
        return;
    }
    std::lock_guard<std::mutex> lock(mutex_);
    if (totalCalls_.count(callObjectPtr) == 0) {
        int32_t callId = callObjectPtr->GetCallID();
        TelCallState callState = callObjectPtr->GetTelCallState();
        TELEPHONY_LOGI("add new call, callid:%{public}d , callstate:%{public}d", callId, callState);
        totalCalls_.insert(callObjectPtr);
    }
    HandleCallStateUpdated(callObjectPtr, priorState, nextState);
    if (nextState == TelCallState::CALL_STATUS_DISCONNECTED && totalCalls_.count(callObjectPtr) > 0) {
        totalCalls_.erase(callObjectPtr);
    }
    UpdateForegroundLiveCall();
}

void AudioControlManager::VideoStateUpdated(
    sptr<CallBase> &callObjectPtr, VideoStateType priorVideoState, VideoStateType nextVideoState)
{
    if (callObjectPtr == nullptr) {
        TELEPHONY_LOGE("call object nullptr");
        return;
    }
    if (callObjectPtr->GetCallType() != CallType::TYPE_IMS &&
        callObjectPtr->GetCallType() != CallType::TYPE_BLUETOOTH) {
        TELEPHONY_LOGE("other call not need control audio");
        return;
    }
    AudioDevice device = {
        .deviceType = AudioDeviceType::DEVICE_SPEAKER,
        .address = { 0 },
    };
    AudioDeviceType initDeviceType = GetInitAudioDeviceType();
    if (callObjectPtr->GetCrsType() == CRS_TYPE && !IsVoIPCallActived()) {
        AudioStandard::AudioRingerMode ringMode = DelayedSingleton<AudioProxy>::GetInstance()->GetRingerMode();
        if (ringMode != AudioStandard::AudioRingerMode::RINGER_MODE_NORMAL) {
            if (initDeviceType == AudioDeviceType::DEVICE_WIRED_HEADSET ||
                initDeviceType == AudioDeviceType::DEVICE_BLUETOOTH_SCO ||
                initDeviceType == AudioDeviceType::DEVICE_NEARLINK) {
                device.deviceType = initDeviceType;
                SetAudioDevice(device);
                TELEPHONY_LOGI("VideoStateUpdated DEVICE_BLUETOOTH_SCO or DEVICE_WIRED_HEADSET or DEVICE_NEARLINK");
            }
        } else {
            TELEPHONY_LOGI("crs ring tone should be speaker");
            SetAudioDevice(device);
        }
        return;
    }
    CheckTypeAndSetAudioDevice(callObjectPtr, priorVideoState, nextVideoState, initDeviceType, device);
}

void AudioControlManager::CheckInitType(AudioDeviceType initDeviceType)
{
    return (initDeviceType == AudioDeviceType::DEVICE_WIRED_HEADSET ||
        initDeviceType == AudioDeviceType::DEVICE_BLUETOOTH_SCO ||
        initDeviceType == AudioDeviceType::DEVICE_NEARLINK ||
        initDeviceType == AudioDeviceType::DEVICE_DISTRIBUTED_AUTOMOTIVE);
}

void AudioControlManager::CheckTypeAndSetAudioDevice(sptr<CallBase> &callObjectPtr, VideoStateType priorVideoState,
    VideoStateType nextVideoState, AudioDeviceType &initDeviceType, AudioDevice &device)
{
    TelCallState telCallState = callObjectPtr->GetTelCallState();
    if (!IsVideoCall(priorVideoState) && IsVideoCall(nextVideoState) &&
        (telCallState != TelCallState::CALL_STATUS_INCOMING && telCallState != TelCallState::CALL_STATUS_WAITING)) {
        if (callObjectPtr->GetOriginalCallType() == VOICE_TYPE &&
            (telCallState == TelCallState::CALL_STATUS_DIALING || telCallState == TelCallState::CALL_STATUS_ALERTING ||
            telCallState == TelCallState::CALL_STATUS_DISCONNECTED)) {
            TELEPHONY_LOGI("before modify set device to EARPIECE, now not set");
            return;
        }
        if (CheckInitType(initDeviceType)) {
            device.deviceType = initDeviceType;
        }
        TELEPHONY_LOGI("set device type, type: %{public}d", static_cast<int32_t>(device.deviceType));
        SetAudioDevice(device);
    } else if (!DelayedSingleton<DistributedCommunicationManager>::GetInstance()->IsAudioOnSink() &&
               !isSetAudioDeviceByUser_ && IsVideoCall(priorVideoState) && !IsVideoCall(nextVideoState)) {
        device.deviceType = AudioDeviceType::DEVICE_EARPIECE;
        if (CheckInitType(initDeviceType)) {
            device.deviceType = initDeviceType;
        }
        TELEPHONY_LOGI("set device type, type: %{public}d", static_cast<int32_t>(device.deviceType));
        SetAudioDevice(device);
    }
}

void AudioControlManager::UpdateDeviceTypeForVideoOrSatelliteCall()
{
    sptr<CallBase> foregroundCall = CallObjectManager::GetForegroundCall();
    if (foregroundCall == nullptr) {
        TELEPHONY_LOGE("call object nullptr");
        return;
    }
    if (foregroundCall->GetCallType() != CallType::TYPE_IMS ||
        foregroundCall->GetCallType() != CallType::TYPE_SATELLITE ||
        foregroundCall->GetCallType() != CallType::TYPE_BLUETOOTH) {
        TELEPHONY_LOGE("other call not need control audio");
        return;
    }
    AudioDevice device = {
        .deviceType = AudioDeviceType::DEVICE_SPEAKER,
        .address = { 0 },
    };
    AudioDeviceType initDeviceType = GetInitAudioDeviceType();
    if (IsVideoCall(foregroundCall->GetVideoStateType()) ||
        foregroundCall->GetCallType() == CallType::TYPE_SATELLITE) {
        if (CheckInitType(initDeviceType)) {
            device.deviceType = initDeviceType;
        }
        TELEPHONY_LOGI("set device type, type: %{public}d", static_cast<int32_t>(device.deviceType));
        SetAudioDevice(device);
    }
}

void AudioControlManager::UpdateDeviceTypeForCrs(AudioDeviceType deviceType)
{
    sptr<CallBase> incomingCall = CallObjectManager::GetOneCallObject(CallRunningState::CALL_RUNNING_STATE_RINGING);
    if (incomingCall == nullptr || incomingCall->IsAnsweredCall()) {
        return;
    }
    if (incomingCall->GetCrsType() == CRS_TYPE && !IsVoIPCallActived()) {
        AudioDevice device = {
            .deviceType = AudioDeviceType::DEVICE_SPEAKER,
            .address = { 0 },
        };
        AudioStandard::AudioRingerMode ringMode = DelayedSingleton<AudioProxy>::GetInstance()->GetRingerMode();
        if (ringMode != AudioStandard::AudioRingerMode::RINGER_MODE_NORMAL) {
            AudioDeviceType initDeviceType = GetInitAudioDeviceType();
            if (initDeviceType == AudioDeviceType::DEVICE_WIRED_HEADSET ||
                initDeviceType == AudioDeviceType::DEVICE_BLUETOOTH_SCO ||
                initDeviceType == AudioDeviceType::DEVICE_NEARLINK) {
                device.deviceType = initDeviceType;
            }
        }
        if (device.deviceType == deviceType) {
            return;
        }
        TELEPHONY_LOGI("crs ring tone should be speaker");
        SetAudioDevice(device);
    }
}

void AudioControlManager::UpdateDeviceTypeForVideoDialing()
{
    sptr<CallBase> dialingCall = CallObjectManager::GetOneCallObject(CallRunningState::CALL_RUNNING_STATE_DIALING);
    if (dialingCall == nullptr) {
        return;
    }
    if (dialingCall->GetVideoStateType() == VideoStateType::TYPE_VIDEO) {
        AudioDevice device = {
            .deviceType = AudioDeviceType::DEVICE_SPEAKER,
            .address = { 0 },
        };
        AudioDeviceType initDeviceType = GetInitAudioDeviceType();
        if (initDeviceType == AudioDeviceType::DEVICE_WIRED_HEADSET ||
            initDeviceType == AudioDeviceType::DEVICE_BLUETOOTH_SCO ||
            initDeviceType == AudioDeviceType::DEVICE_NEARLINK) {
            device.deviceType = initDeviceType;
        }
        TELEPHONY_LOGI("dialing video call should be speaker");
        SetAudioDevice(device);
    }
}

void AudioControlManager::IncomingCallActivated(sptr<CallBase> &callObjectPtr) {}

void AudioControlManager::IncomingCallHungUp(sptr<CallBase> &callObjectPtr, bool isSendSms, std::string content)
{
    if (callObjectPtr == nullptr) {
        TELEPHONY_LOGE("call object ptr nullptr");
        return;
    }
    StopWaitingTone();
}

void AudioControlManager::HandleCallStateUpdated(
    sptr<CallBase> &callObjectPtr, TelCallState priorState, TelCallState nextState)
{
    if (callObjectPtr == nullptr) {
        TELEPHONY_LOGE("call object is nullptr");
        return;
    }
    TELEPHONY_LOGI("HandleCallStateUpdated priorState:%{public}d, nextState:%{public}d", priorState, nextState);
    if (nextState == TelCallState::CALL_STATUS_ANSWERED) {
        TELEPHONY_LOGI("user answered, mute ringer instead of release renderer");
        if (priorState == TelCallState::CALL_STATUS_INCOMING) {
            DelayedSingleton<CallStateProcessor>::GetInstance()->DeleteCall(callObjectPtr->GetCallID(), priorState);
        }
        MuteRinger();
        return;
    }
    HandleNextState(callObjectPtr, nextState);
    if (priorState == nextState) {
        TELEPHONY_LOGI("prior state equals next state");
        return;
    }
    HandlePriorState(callObjectPtr, priorState);
}

void AudioControlManager::HandleNextState(sptr<CallBase> &callObjectPtr, TelCallState nextState)
{
    TELEPHONY_LOGI("handle next state.");
    AudioEvent event = AudioEvent::UNKNOWN_EVENT;
    DelayedSingleton<CallStateProcessor>::GetInstance()->AddCall(callObjectPtr->GetCallID(), nextState);
    switch (nextState) {
        case TelCallState::CALL_STATUS_DIALING:
            event = AudioEvent::NEW_DIALING_CALL;
            audioInterruptState_ = AudioInterruptState::INTERRUPT_STATE_RINGING;
            break;
        case TelCallState::CALL_STATUS_ALERTING:
            event = AudioEvent::NEW_ALERTING_CALL;
            audioInterruptState_ = AudioInterruptState::INTERRUPT_STATE_RINGING;
            break;
        case TelCallState::CALL_STATUS_ACTIVE:
            HandleNewActiveCall(callObjectPtr);
            audioInterruptState_ = AudioInterruptState::INTERRUPT_STATE_ACTIVATED;
            break;
        case TelCallState::CALL_STATUS_WAITING:
        case TelCallState::CALL_STATUS_INCOMING:
            event = AudioEvent::NEW_INCOMING_CALL;
            audioInterruptState_ = AudioInterruptState::INTERRUPT_STATE_RINGING;
            break;
        case TelCallState::CALL_STATUS_DISCONNECTING:
        case TelCallState::CALL_STATUS_DISCONNECTED:
            if (isCrsVibrating_) {
                DelayedSingleton<AudioProxy>::GetInstance()->StopVibrator();
                isCrsVibrating_ = false;
            }
            audioInterruptState_ = AudioInterruptState::INTERRUPT_STATE_DEACTIVATED;
            break;
        default:
            break;
    }
    TELEPHONY_LOGI("HandleNextState ProcessEvent event=%{public}d", event);
    if (event == AudioEvent::UNKNOWN_EVENT) {
        return;
    }
    TELEPHONY_LOGI("handle next state, event: %{public}d.", event);
    DelayedSingleton<AudioSceneProcessor>::GetInstance()->ProcessEvent(event);
}

void AudioControlManager::HandlePriorState(sptr<CallBase> &callObjectPtr, TelCallState priorState)
{
    TELEPHONY_LOGI("handle prior state.");
    AudioEvent event = AudioEvent::UNKNOWN_EVENT;
    DelayedSingleton<CallStateProcessor>::GetInstance()->DeleteCall(callObjectPtr->GetCallID(), priorState);
    int32_t stateNumber = DelayedSingleton<CallStateProcessor>::GetInstance()->GetCallNumber(priorState);
    switch (priorState) {
        case TelCallState::CALL_STATUS_DIALING:
            if (stateNumber == EMPTY_VALUE) {
                event = AudioEvent::NO_MORE_DIALING_CALL;
            }
            break;
        case TelCallState::CALL_STATUS_ALERTING:
            if (stateNumber == EMPTY_VALUE) {
                event = AudioEvent::NO_MORE_ALERTING_CALL;
            }
            break;
        case TelCallState::CALL_STATUS_INCOMING:
        case TelCallState::CALL_STATUS_WAITING:
            ProcessAudioWhenCallActive(callObjectPtr);
            event = AudioEvent::NO_MORE_INCOMING_CALL;
            break;
        case TelCallState::CALL_STATUS_ACTIVE:
            if (stateNumber == EMPTY_VALUE) {
                event = AudioEvent::NO_MORE_ACTIVE_CALL;
            }
            break;
        case TelCallState::CALL_STATUS_HOLDING:
            if (stateNumber == EMPTY_VALUE) {
                event = AudioEvent::NO_MORE_HOLDING_CALL;
            }
            break;
        default:
            break;
    }
    TELEPHONY_LOGI("HandlePriorState ProcessEvent event=%{public}d", event);
    if (event == AudioEvent::UNKNOWN_EVENT) {
        return;
    }
    TELEPHONY_LOGI("handle prior state, event: %{public}d.", event);
    DelayedSingleton<AudioSceneProcessor>::GetInstance()->ProcessEvent(event);
}

void AudioControlManager::ProcessAudioWhenCallActive(sptr<CallBase> &callObjectPtr)
{
    if (callObjectPtr->GetCallRunningState() == CallRunningState::CALL_RUNNING_STATE_ACTIVE) {
        if (isCrsVibrating_) {
            DelayedSingleton<AudioProxy>::GetInstance()->StopVibrator();
            isCrsVibrating_ = false;
        }
        int ringCallCount = CallObjectManager::GetCallNumByRunningState(CallRunningState::CALL_RUNNING_STATE_RINGING);
        if ((CallObjectManager::GetCurrentCallNum() - ringCallCount) < MIN_MULITY_CALL_COUNT) {
            StopSoundtone();
            PlaySoundtone();
        }
        UpdateDeviceTypeForVideoOrSatelliteCall();
    }
}

void AudioControlManager::HandleNewActiveCall(sptr<CallBase> &callObjectPtr)
{
    CallType callType = callObjectPtr->GetCallType();
    AudioEvent event = AudioEvent::UNKNOWN_EVENT;
    switch (callType) {
        case CallType::TYPE_CS:
        case CallType::TYPE_SATELLITE:
            event = AudioEvent::NEW_ACTIVE_CS_CALL;
            break;
        case CallType::TYPE_IMS:
        case CallType::TYPE_BLUETOOTH:
            event = AudioEvent::NEW_ACTIVE_IMS_CALL;
            break;
        case CallType::TYPE_OTT:
            event = AudioEvent::NEW_ACTIVE_OTT_CALL;
            break;
        default:
            break;
    }
    if (event == AudioEvent::UNKNOWN_EVENT) {
        return;
    }
    DelayedSingleton<AudioSceneProcessor>::GetInstance()->ProcessEvent(event);
}

/**
 * @param device , audio device
 * usually called by the ui interaction , in purpose of switching to another audio device
 */
int32_t AudioControlManager::SetAudioDevice(const AudioDevice &device)
{
    return SetAudioDevice(device, false);
}

/**
 * @param device , audio device
 * @param isByUser , call from callui or not
 * usually called by the ui interaction , in purpose of switching to another audio device
 */
int32_t AudioControlManager::SetAudioDevice(const AudioDevice &device, bool isByUser)
{
    bool hasCall = DelayedSingleton<CallControlManager>::GetInstance()->HasCall() ||
        DelayedSingleton<CallControlManager>::GetInstance()->HasVoipCall();
    if (!hasCall) {
        TELEPHONY_LOGE("no call exists, set audio device failed");
        return CALL_ERR_AUDIO_SET_AUDIO_DEVICE_FAILED;
    }
    TELEPHONY_LOGI("set audio device, type: %{public}d", static_cast<int32_t>(device.deviceType));
    AudioDeviceType audioDeviceType = AudioDeviceType::DEVICE_UNKNOWN;
    if (CallObjectManager::HasSatelliteCallExist() && device.deviceType == AudioDeviceType::DEVICE_EARPIECE) {
        DelayedSingleton<CallDialog>::GetInstance()->DialogConnectExtension("SATELLITE_CALL_NOT_SUPPORT_EARPIECE");
        return CALL_ERR_AUDIO_SET_AUDIO_DEVICE_FAILED;
    }
    isSetAudioDeviceByUser_ = isByUser;
    switch (device.deviceType) {
        case AudioDeviceType::DEVICE_SPEAKER:
        case AudioDeviceType::DEVICE_EARPIECE:
        case AudioDeviceType::DEVICE_WIRED_HEADSET:
            audioDeviceType = device.deviceType;
            break;
        case AudioDeviceType::DEVICE_DISTRIBUTED_AUTOMOTIVE:
        case AudioDeviceType::DEVICE_DISTRIBUTED_PHONE:
        case AudioDeviceType::DEVICE_DISTRIBUTED_PAD:
        case AudioDeviceType::DEVICE_DISTRIBUTED_PC:
            return HandleDistributeAudioDevice(device);
        case AudioDeviceType::DEVICE_BLUETOOTH_SCO:
        case AudioDeviceType::DEVICE_NEARLINK: {
            if (HandleBluetoothOrNearlinkAudioDevice(device) != TELEPHONY_SUCCESS) {
                return CALL_ERR_AUDIO_SET_AUDIO_DEVICE_FAILED;
            }
            audioDeviceType = device.deviceType;
            break;
        }
        default:
            break;
    }
    return SwitchAudioDevice(audioDeviceType);
}

int32_t AudioControlManager::SwitchAudioDevice(AudioDeviceType audioDeviceType)
{
    if (audioDeviceType != AudioDeviceType::DEVICE_UNKNOWN) {
        if (DelayedSingleton<DistributedCommunicationManager>::GetInstance()->IsAudioOnSink()) {
            DelayedSingleton<DistributedCommunicationManager>::GetInstance()->SwitchToSourceDevice();
        }
        if (DelayedSingleton<DistributedCallManager>::GetInstance()->IsDCallDeviceSwitchedOn()) {
            DelayedSingleton<DistributedCallManager>::GetInstance()->SwitchOffDCallDeviceSync();
        }
        if (DelayedSingleton<AudioDeviceManager>::GetInstance()->SwitchDevice(audioDeviceType)) {
            return TELEPHONY_SUCCESS;
        }
    }
    return CALL_ERR_AUDIO_SET_AUDIO_DEVICE_FAILED;
}

int32_t AudioControlManager::HandleDistributeAudioDevice(const AudioDevice &device)
{
    if (DelayedSingleton<DistributedCommunicationManager>::GetInstance()->IsDistributedDev(device)) {
        if (DelayedSingleton<DistributedCommunicationManager>::GetInstance()->SwitchToSinkDevice(device)) {
            return TELEPHONY_SUCCESS;
        }
        return CALL_ERR_AUDIO_SET_AUDIO_DEVICE_FAILED;
    }
    if (!DelayedSingleton<DistributedCallManager>::GetInstance()->IsDCallDeviceSwitchedOn()) {
        if (DelayedSingleton<DistributedCallManager>::GetInstance()->SwitchOnDCallDeviceSync(device)) {
            return TELEPHONY_SUCCESS;
        }
        return CALL_ERR_AUDIO_SET_AUDIO_DEVICE_FAILED;
    }
    return TELEPHONY_SUCCESS;
}

int32_t AudioControlManager::HandleBluetoothOrNearlinkAudioDevice(const AudioDevice &device)
{
    std::string address = device.address;
    if (address.empty()) {
        if (device.deviceType == AudioDeviceType::DEVICE_BLUETOOTH_SCO) {
            std::shared_ptr<AudioStandard::AudioDeviceDescriptor> activeBluetoothDevice =
                AudioStandard::AudioRoutingManager::GetInstance()->GetActiveBluetoothDevice();
            if (activeBluetoothDevice == nullptr || activeBluetoothDevice->macAddress_.empty()) {
                TELEPHONY_LOGE("Get active bluetooth device failed.");
                return CALL_ERR_AUDIO_SET_AUDIO_DEVICE_FAILED;
            }
            address = activeBluetoothDevice->macAddress_;
        } else {
            AudioDevice preferredAudioDevice;
            if (!AudioDeviceManager::IsNearlinkActived(preferredAudioDevice)) {
                TELEPHONY_LOGE("Get active nearlink device failed.");
                return CALL_ERR_AUDIO_SET_AUDIO_DEVICE_FAILED;
            }
            address = preferredAudioDevice.address;
        }
    }
    std::shared_ptr<AudioStandard::AudioDeviceDescriptor> audioDev =
        std::make_shared<AudioStandard::AudioDeviceDescriptor>();
    audioDev->macAddress_ = address;
    audioDev->deviceType_ = (device.deviceType == AudioDeviceType::DEVICE_BLUETOOTH_SCO) ?
        AudioStandard::DEVICE_TYPE_BLUETOOTH_SCO : AudioStandard::DEVICE_TYPE_NEARLINK;
    audioDev->deviceRole_ = AudioStandard::OUTPUT_DEVICE;
    audioDev->networkId_ = AudioStandard::LOCAL_NETWORK_ID;
    std::vector<std::shared_ptr<AudioDeviceDescriptor>> remoteDevice;
    remoteDevice.push_back(audioDev);
    AudioSystemManager* audioSystemManager = AudioSystemManager::GetInstance();
    sptr<AudioRendererFilter> audioRendererFilter = new(std::nothrow) AudioRendererFilter();
    if (audioRendererFilter == nullptr) {
        TELEPHONY_LOGE("audioRendererFilter is nullptr");
        return CALL_ERR_AUDIO_SET_AUDIO_DEVICE_FAILED;
    }
    audioRendererFilter->rendererInfo.streamUsage = StreamUsage::STREAM_USAGE_VOICE_MODEM_COMMUNICATION;
    int32_t ret = audioSystemManager->SelectOutputDevice(audioRendererFilter, remoteDevice);
    if (ret != 0) {
        TELEPHONY_LOGE("SelectOutputDevice failed");
        return CALL_ERR_AUDIO_SET_AUDIO_DEVICE_FAILED;
    }
    return TELEPHONY_SUCCESS;
}

bool AudioControlManager::PlayRingtone()
{
    int32_t ret;
    if (!ShouldPlayRingtone()) {
        TELEPHONY_LOGE("should not play ringtone");
        return false;
    }
    ring_ = std::make_unique<Ring>();
    if (ring_ == nullptr) {
        TELEPHONY_LOGE("create ring object failed");
        return false;
    }
    sptr<CallBase> incomingCall = CallObjectManager::GetOneCallObject(CallRunningState::CALL_RUNNING_STATE_RINGING);
    if (incomingCall == nullptr) {
        TELEPHONY_LOGE("incomingCall is nullptr");
        return false;
    }
    CallAttributeInfo info;
    incomingCall->GetCallAttributeBaseInfo(info);
    ContactInfo contactInfo = incomingCall->GetCallerInfo();
    AudioStandard::AudioRingerMode ringMode = DelayedSingleton<AudioProxy>::GetInstance()->GetRingerMode();
    if (incomingCall->GetCrsType() == CRS_TYPE) {
        if (!isCrsVibrating_ && (ringMode != AudioStandard::AudioRingerMode::RINGER_MODE_SILENT)) {
            if (ringMode == AudioStandard::AudioRingerMode::RINGER_MODE_VIBRATE || IsRingingVibrateModeOn()) {
                isCrsVibrating_ = (DelayedSingleton<AudioProxy>::GetInstance()->StartVibrator() == TELEPHONY_SUCCESS);
            }
        }
        if ((ringMode == AudioStandard::AudioRingerMode::RINGER_MODE_NORMAL) || IsBtOrWireHeadPlugin()) {
            AdjustVolumesForCrs();
            if (PlaySoundtone()) {
                TELEPHONY_LOGI("play soundtone success");
                return true;
            }
            return false;
        }
        TELEPHONY_LOGI("type_crs but not play ringtone");
        return false;
    }

    if (incomingCall->GetCallType() == CallType::TYPE_BLUETOOTH) {
        ret = ring_->Play(info.accountId, contactInfo.ringtonePath, Media::HapticStartupMode::FAST);
    } else {
        ret = ring_->Play(info.accountId, contactInfo.ringtonePath, Media::HapticStartupMode::DEFAULT);
    }
    if (ret != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("play ringtone failed");
        return false;
    }
    TELEPHONY_LOGI("play ringtone success");
    PostProcessRingtone();
    return true;
}

void AudioControlManager::PostProcessRingtone()
{
    sptr<CallBase> incomingCall = CallObjectManager::GetOneCallObject(CallRunningState::CALL_RUNNING_STATE_RINGING);
    if (incomingCall == nullptr) {
        TELEPHONY_LOGI("play ringtone success but incoming call is null stop it");
        StopRingtone();
        return;
    }
    AAFwk::WantParams params = incomingCall->GetExtraParams();
    bool isNeedMuteRing = static_cast<bool>(params.GetIntParam("isNeedMuteRing", 0));
    if (isNeedMuteRing) {
        TELEPHONY_LOGI("play ringtone success but need mute it");
        MuteRinger();
        params.SetParam("isNeedMuteRing", AAFwk::Integer::Box(0));
        incomingCall->SetExtraParams(params);
    }
}

bool AudioControlManager::IsDistributeCallSinkStatus()
{
    std::string dcStatus = "";
    auto settingHelper = SettingsDataShareHelper::GetInstance();
    if (settingHelper != nullptr) {
        OHOS::Uri settingUri(SettingsDataShareHelper::SETTINGS_DATASHARE_URI);
        settingHelper->Query(settingUri, "distributed_modem_state", dcStatus);
    }
    TELEPHONY_LOGI("distributed communication modem status: %{public}s", dcStatus.c_str());
    if (dcStatus != "1_sink") {
        return false;
    }
    return true;
}

bool AudioControlManager::PlaySoundtone()
{
    if (IsDistributeCallSinkStatus()) {
        TELEPHONY_LOGI("distribute call sink status, no need to play sound tone");
        return true;
    }
    if (soundState_ == SoundState::SOUNDING) {
        TELEPHONY_LOGE("should not play soundTone");
        return false;
    }
    if (sound_ == nullptr) {
        sound_ = std::make_unique<Sound>();
        if (sound_ == nullptr) {
            TELEPHONY_LOGE("create sound object failed");
            return false;
        }
    }
    if (sound_->Play() != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("play soundtone failed");
        return false;
    }
    TELEPHONY_LOGI("play soundtone success");
    return true;
}

bool AudioControlManager::StopSoundtone()
{
    if (soundState_ == SoundState::STOPPED) {
        TELEPHONY_LOGI("soundtone already stopped");
        return true;
    }
    if (sound_ == nullptr) {
        TELEPHONY_LOGE("sound_ is nullptr");
        return false;
    }
    DelayedSingleton<AudioProxy>::GetInstance()->SetVoiceRingtoneMute(false);
    if (sound_->Stop() != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("stop soundtone failed");
        return false;
    }
    sound_->ReleaseRenderer();
    TELEPHONY_LOGI("stop soundtone success");
    RestoreVoiceValumeIfNecessary();
    return true;
}

bool AudioControlManager::StopRingtone()
{
    if (ringState_ == RingState::STOPPED) {
        TELEPHONY_LOGI("ringtone already stopped");
        return true;
    }
    if (ring_ == nullptr) {
        TELEPHONY_LOGE("ring_ is nullptr");
        return false;
    }
    if (ring_->Stop() != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("stop ringtone failed");
        return false;
    }
    ring_->ReleaseRenderer();
    TELEPHONY_LOGI("stop ringtone success");
    return true;
}

/**
 * while audio state changed , maybe need to reinitialize the audio device
 * in order to get the initialization status of audio device , need to consider varieties of  audio conditions
 */
AudioDeviceType AudioControlManager::GetInitAudioDeviceType() const
{
    if (audioInterruptState_ == AudioInterruptState::INTERRUPT_STATE_DEACTIVATED) {
        return AudioDeviceType::DEVICE_DISABLE;
    } else {
        if (DelayedSingleton<DistributedCommunicationManager>::GetInstance()->IsConnected()) {
            AudioDevice device = {
                .deviceType = AudioDeviceType::DEVICE_UNKNOWN,
            };
            (void)DelayedSingleton<AudioProxy>::GetInstance()->GetPreferredOutputAudioDevice(device);
            return device.deviceType;
        }

        /**
         * Init audio device type according to the priority in different call state:
         * In voice call state, bluetooth sco > wired headset > earpiece > speaker
         * In video call state, bluetooth sco > wired headset > speaker > earpiece
         */
        if (AudioDeviceManager::IsDistributedCallConnected()) {
            return AudioDeviceType::DEVICE_DISTRIBUTED_AUTOMOTIVE;
        }
        AudioDevice device;
        if (AudioDeviceManager::IsNearlinkActived(device)) {
            return AudioDeviceType::DEVICE_NEARLINK;
        }
        if (AudioDeviceManager::IsBtActived()) {
            return AudioDeviceType::DEVICE_BLUETOOTH_SCO;
        }
        if (AudioDeviceManager::IsWiredHeadsetConnected()) {
            return AudioDeviceType::DEVICE_WIRED_HEADSET;
        }
        sptr<CallBase> liveCall = CallObjectManager::GetForegroundCall();
        if (liveCall != nullptr && (liveCall->GetVideoStateType() == VideoStateType::TYPE_VIDEO ||
            liveCall->GetCallType() == CallType::TYPE_SATELLITE ||
            liveCall->GetCallType() == CallType::TYPE_BLUETOOTH)) {
            TELEPHONY_LOGI("current video or satellite call speaker is active");
            return AudioDeviceType::DEVICE_SPEAKER;
        }
        if (AudioDeviceManager::IsEarpieceAvailable()) {
            return AudioDeviceType::DEVICE_EARPIECE;
        }
        return AudioDeviceType::DEVICE_SPEAKER;
    }
}

/**
 * @param isMute , mute state
 * usually called by the ui interaction , mute or unmute microphone
 */
int32_t AudioControlManager::SetMute(bool isMute)
{
    bool hasCall = DelayedSingleton<CallControlManager>::GetInstance()->HasCall();
    if (!hasCall) {
        TELEPHONY_LOGE("no call exists, set mute failed");
        return CALL_ERR_AUDIO_SETTING_MUTE_FAILED;
    }
    bool enabled = false;
    if ((DelayedSingleton<CallControlManager>::GetInstance()->HasEmergency(enabled) == TELEPHONY_SUCCESS) && enabled) {
        isMute = false;
    }
    sptr<CallBase> currentCall = frontCall_;
    if (currentCall == nullptr) {
        TELEPHONY_LOGE("frontCall_ is nullptr");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    if (DelayedSingleton<CallControlManager>::GetInstance()->IsCallExist(CallType::TYPE_BLUETOOTH,
        TelCallState::CALL_STATUS_ACTIVE)) {
        std::string strMute = isMute ? "true" : "false";
        TELEPHONY_LOGI("SetMute strMute=%{public}s", strMute.c_str());
        std::vector<std::pair<std::string, std::string>> vec = {
            std::pair<std::string, std::string>("hfp_set_mic_mute", strMute)
        };
        OHOS::AudioStandard::AudioSystemManager::GetInstance()->SetExtraParameters("hfp_extra", vec);
        currentCall->SetMicPhoneState(isMute);
    } else {
        if (!DelayedSingleton<AudioProxy>::GetInstance()->SetMicrophoneMute(isMute)) {
            TELEPHONY_LOGE("set mute failed");
            return CALL_ERR_AUDIO_SETTING_MUTE_FAILED;
        }
    }
    DelayedSingleton<AudioDeviceManager>::GetInstance()->ReportAudioDeviceInfo();
    if (currentCall->GetCallType() != CallType::TYPE_BLUETOOTH) {
        bool muted = DelayedSingleton<AudioProxy>::GetInstance()->IsMicrophoneMute();
        currentCall->SetMicPhoneState(muted);
        TELEPHONY_LOGI("SetMute success callId:%{public}d, mute:%{public}d", currentCall->GetCallID(), muted);
    }
    return TELEPHONY_SUCCESS;
}

int32_t AudioControlManager::MuteRinger()
{
    sptr<CallBase> incomingCall = CallObjectManager::GetOneCallObject(CallRunningState::CALL_RUNNING_STATE_RINGING);
    if (incomingCall != nullptr) {
        if (incomingCall->GetCrsType() == CRS_TYPE && !IsVoIPCallActived() &&
            soundState_ == SoundState::SOUNDING) {
            TELEPHONY_LOGI("Mute network ring tone.");
            MuteNetWorkRingTone();
        }
    }
    SendMuteRingEvent();
    if (ringState_ == RingState::STOPPED) {
        TELEPHONY_LOGI("ring already stopped");
        if (incomingCall != nullptr) {
            AAFwk::WantParams params = incomingCall->GetExtraParams();
            params.SetParam("isNeedMuteRing", AAFwk::Integer::Box(1));
            incomingCall->SetExtraParams(params);
        }
        return TELEPHONY_SUCCESS;
    }
    if (ring_ == nullptr) {
        TELEPHONY_LOGE("ring is nullptr");
        return CALL_ERR_AUDIO_SETTING_MUTE_FAILED;
    }
    if (ring_->SetMute() != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("SetMute fail");
        return CALL_ERR_AUDIO_SETTING_MUTE_FAILED;
    }
    TELEPHONY_LOGI("mute ring success");
    return TELEPHONY_SUCCESS;
}

void AudioControlManager::SendMuteRingEvent()
{
    CallEventInfo eventInfo;
    eventInfo.eventId = CallAbilityEventId::EVENT_MUTE_RING;
    DelayedSingleton<CallAbilityReportProxy>::GetInstance()->CallEventUpdated(eventInfo);
}

void AudioControlManager::PlayCallEndedTone(CallEndedType type)
{
    int32_t state;
    DelayedSingleton<CallControlManager>::GetInstance()->GetVoIPCallState(state);
    if (state != static_cast<int32_t>(CallStateToApp::CALL_STATE_IDLE)) {
        TELEPHONY_LOGI("not play callEndTone when has voip call");
        return;
    }
    AudioStandard::AudioRingerMode ringMode = DelayedSingleton<AudioProxy>::GetInstance()->GetRingerMode();
    if (ringMode != AudioStandard::AudioRingerMode::RINGER_MODE_NORMAL) {
        HandleNotNormalRingerMode(type);
        return;
    }
    switch (type) {
        case CallEndedType::PHONE_IS_BUSY:
            PlayCallTone(ToneDescriptor::TONE_ENGAGED);
            break;
        case CallEndedType::CALL_ENDED_NORMALLY:
            if (toneState_ == ToneState::TONEING) {
                StopCallTone();
            }
            TELEPHONY_LOGI("play call ended tone");
            if (IsBtCallDisconnected()) {
                return;
            }
            if (PlayCallTone(ToneDescriptor::TONE_FINISHED) != TELEPHONY_SUCCESS) {
                StopCallTone();
                TELEPHONY_LOGE("play call ended tone failed");
                return;
            }
            toneState_ = ToneState::CALLENDED;
            std::this_thread::sleep_for(std::chrono::milliseconds(CALL_ENDED_PLAY_TIME));
            toneState_ = ToneState::TONEING;
            if (StopCallTone() != TELEPHONY_SUCCESS) {
                TELEPHONY_LOGE("stop call ended tone failed");
                return;
            }
            break;
        case CallEndedType::UNKNOWN:
            PlayCallTone(ToneDescriptor::TONE_UNKNOWN);
            break;
        case CallEndedType::INVALID_NUMBER:
            PlayCallTone(ToneDescriptor::TONE_INVALID_NUMBER);
            break;
        default:
            break;
    }
}

void AudioControlManager::HandleNotNormalRingerMode(CallEndedType type)
{
    TELEPHONY_LOGE("ringer mode is not normal");
    switch (type) {
        case CallEndedType::CALL_ENDED_NORMALLY:
            if (toneState_ == ToneState::TONEING) {
                StopCallTone();
                TELEPHONY_LOGI("ringer mode is not normal, stop call tone!");
            }
            break;
        default:
            break;
    }
}

std::set<sptr<CallBase>> AudioControlManager::GetCallList()
{
    std::lock_guard<std::mutex> lock(mutex_);
    return totalCalls_;
}

sptr<CallBase> AudioControlManager::GetCurrentActiveCall()
{
    int32_t callId = DelayedSingleton<CallStateProcessor>::GetInstance()->GetCurrentActiveCall();
    if (callId != INVALID_CALLID) {
        return GetCallBase(callId);
    }
    return nullptr;
}

sptr<CallBase> AudioControlManager::GetCallBase(int32_t callId)
{
    sptr<CallBase> callBase = nullptr;
    std::lock_guard<std::mutex> lock(mutex_);
    for (auto &call : totalCalls_) {
        if (call->GetCallID() == callId) {
            callBase = call;
            break;
        }
    }
    return callBase;
}

bool AudioControlManager::IsEmergencyCallExists()
{
    std::lock_guard<std::mutex> lock(mutex_);
    for (auto call : totalCalls_) {
        if (call->GetEmergencyState()) {
            return true;
        }
    }
    return false;
}

AudioInterruptState AudioControlManager::GetAudioInterruptState()
{
    return audioInterruptState_;
}

void AudioControlManager::SetVolumeAudible()
{
    DelayedSingleton<AudioProxy>::GetInstance()->SetVolumeAudible();
}

void AudioControlManager::SetRingState(RingState state)
{
    ringState_ = state;
}

void AudioControlManager::SetSoundState(SoundState state)
{
    soundState_ = state;
}

void AudioControlManager::SetToneState(ToneState state)
{
    std::lock_guard<std::recursive_mutex> lock(toneStateLock_);
    toneState_ = state;
}

void AudioControlManager::SetLocalRingbackNeeded(bool isNeeded)
{
    if (isLocalRingbackNeeded_ && !isNeeded) {
        StopRingback();
    }
    isLocalRingbackNeeded_ = isNeeded;
}

bool AudioControlManager::IsNumberAllowed(const std::string &phoneNum)
{
    // check whether the phone number is allowed or not , should not ring if number is not allowed
    return true;
}

bool AudioControlManager::ShouldPlayRingtone() const
{
    auto processor = DelayedSingleton<CallStateProcessor>::GetInstance();
    int32_t alertingCallNum = processor->GetCallNumber(TelCallState::CALL_STATUS_ALERTING);
    int32_t incomingCallNum = processor->GetCallNumber(TelCallState::CALL_STATUS_INCOMING);
    if (incomingCallNum == EMPTY_VALUE || alertingCallNum > EMPTY_VALUE || ringState_ == RingState::RINGING
        || (soundState_ == SoundState::SOUNDING && CallObjectManager::HasIncomingCallCrsType())) {
        return false;
    }
    return true;
}

bool AudioControlManager::IsAudioActivated() const
{
    return audioInterruptState_ == AudioInterruptState::INTERRUPT_STATE_ACTIVATED ||
        audioInterruptState_ == AudioInterruptState::INTERRUPT_STATE_RINGING;
}

int32_t AudioControlManager::PlayCallTone(ToneDescriptor type)
{
    std::lock_guard<std::recursive_mutex> lock(toneStateLock_);
    if (toneState_ == ToneState::TONEING) {
        TELEPHONY_LOGE("callTone is already playing");
        return CALL_ERR_AUDIO_TONE_PLAY_FAILED;
    }
    toneState_ = ToneState::TONEING;
    tone_ = std::make_unique<Tone>(type);
    if (tone_ == nullptr) {
        TELEPHONY_LOGE("create tone failed");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    if (tone_->Play() != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("play calltone failed");
        return CALL_ERR_AUDIO_TONE_PLAY_FAILED;
    }
    TELEPHONY_LOGI("play calltone success");
    return TELEPHONY_SUCCESS;
}

int32_t AudioControlManager::StopCallTone()
{
    TELEPHONY_LOGI("stop call tone enter");
    std::lock_guard<std::recursive_mutex> lock(toneStateLock_);
    if (toneState_ == ToneState::STOPPED) {
        TELEPHONY_LOGI("tone is already stopped");
        return TELEPHONY_SUCCESS;
    }
    if (toneState_ == ToneState::CALLENDED) {
        TELEPHONY_LOGE("call ended tone is running");
        return CALL_ERR_AUDIO_TONE_STOP_FAILED;
    }
    if (tone_ == nullptr) {
        TELEPHONY_LOGE("tone_ is nullptr");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    if (tone_->Stop() != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("stop calltone failed");
        return CALL_ERR_AUDIO_TONE_STOP_FAILED;
    }
    tone_->ReleaseRenderer();
    toneState_ = ToneState::STOPPED;
    TELEPHONY_LOGI("stop call tone success");
    return TELEPHONY_SUCCESS;
}

bool AudioControlManager::IsTonePlaying()
{
    std::lock_guard<std::recursive_mutex> lock(toneStateLock_);
    return toneState_ == ToneState::TONEING;
}

bool AudioControlManager::IsCurrentRinging() const
{
    return ringState_ == RingState::RINGING;
}

int32_t AudioControlManager::PlayRingback()
{
    if (!isLocalRingbackNeeded_) {
        return CALL_ERR_AUDIO_TONE_PLAY_FAILED;
    }
    return PlayCallTone(ToneDescriptor::TONE_RINGBACK);
}

int32_t AudioControlManager::StopRingback()
{
    if (tone_ != nullptr && tone_->getCurrentToneType() == ToneDescriptor::TONE_RINGBACK) {
        return StopCallTone();
    }
    return TELEPHONY_SUCCESS;
}

int32_t AudioControlManager::PlayWaitingTone()
{
    return PlayCallTone(ToneDescriptor::TONE_WAITING);
}

int32_t AudioControlManager::StopWaitingTone()
{
    if (tone_ != nullptr && tone_->getCurrentToneType() == ToneDescriptor::TONE_WAITING) {
        return StopCallTone();
    }
    return TELEPHONY_SUCCESS;
}

int32_t AudioControlManager::PlayDtmfTone(char str)
{
    ToneDescriptor dtmfTone = Tone::ConvertDigitToTone(str);
    std::unique_ptr<Tone> tone = std::make_unique<Tone>(dtmfTone);
    if (tone == nullptr) {
        TELEPHONY_LOGE("create dtmf tone failed");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    if (tone->Play() != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("play dtmftone failed");
        return CALL_ERR_AUDIO_TONE_PLAY_FAILED;
    }
    TELEPHONY_LOGI("play dtmftone success");
    std::this_thread::sleep_for(std::chrono::milliseconds(DTMF_PLAY_TIME));
    if (tone->Stop() != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("stop dtmftone failed");
        return CALL_ERR_AUDIO_TONE_STOP_FAILED;
    }
    tone->ReleaseRenderer();
    TELEPHONY_LOGI("stop dtmf tone success");
    return TELEPHONY_SUCCESS;
}

int32_t AudioControlManager::StopDtmfTone()
{
    return StopCallTone();
}

int32_t AudioControlManager::OnPostDialNextChar(char str)
{
    int32_t result = PlayDtmfTone(str);
    if (result != TELEPHONY_SUCCESS) {
        return result;
    }
    return TELEPHONY_SUCCESS;
}

void AudioControlManager::NewCallCreated(sptr<CallBase> &callObjectPtr) {}

void AudioControlManager::CallDestroyed(const DisconnectedDetails &details) {}

bool AudioControlManager::IsSoundPlaying()
{
    return soundState_ == SoundState::SOUNDING;
}

void AudioControlManager::MuteNetWorkRingTone()
{
    bool result =
        DelayedSingleton<AudioProxy>::GetInstance()->SetVoiceRingtoneMute(true);
    TELEPHONY_LOGI("Set Voice Ringtone mute, result: %{public}d", result);
    if (isCrsVibrating_) {
        DelayedSingleton<AudioProxy>::GetInstance()->StopVibrator();
        isCrsVibrating_ = false;
    }
}

bool AudioControlManager::IsVideoCall(VideoStateType videoState)
{
    return videoState == VideoStateType::TYPE_VIDEO;
}

bool AudioControlManager::IsBtOrWireHeadPlugin()
{
    return AudioDeviceManager::IsBtActived() || AudioDeviceManager::IsWiredHeadsetConnected();
}

bool AudioControlManager::IsRingingVibrateModeOn()
{
    auto datashareHelper = SettingsDataShareHelper::GetInstance();
    std::string ringingVibrateModeEnable {"1"};
    std::vector<int> activedOsAccountIds;
    OHOS::AccountSA::OsAccountManager::QueryActiveOsAccountIds(activedOsAccountIds);
    if (activedOsAccountIds.empty()) {
        TELEPHONY_LOGW("activedOsAccountIds is empty");
        return false;
    }
    int userId = activedOsAccountIds[0];
    OHOS::Uri uri(
        "datashare:///com.ohos.settingsdata/entry/settingsdata/USER_SETTINGSDATA_"
        + std::to_string(userId) + "?Proxy=true");
    int resp = datashareHelper->Query(uri, "hw_vibrate_when_ringing", ringingVibrateModeEnable);
    if (resp == TELEPHONY_SUCCESS && ringingVibrateModeEnable == "1") {
        TELEPHONY_LOGI("RingingVibrateModeOpen:true");
        return true;
    }
    return false;
}
bool AudioControlManager::IsVoIPCallActived()
{
    int32_t state;
    DelayedSingleton<CallControlManager>::GetInstance()->GetVoIPCallState(state);
    if (state == static_cast<int32_t>(CallStateToApp::CALL_STATE_IDLE) ||
        state == static_cast<int32_t>(CallStateToApp::CALL_STATE_UNKNOWN)) {
        return false;
    }
    TELEPHONY_LOGI("VoIP Call is actived");
    return true;
}

bool AudioControlManager::IsBtCallDisconnected()
{
    sptr<CallBase> call = CallObjectManager::GetOneCallObject(CallRunningState::CALL_RUNNING_STATE_ENDED);
    if (call != nullptr && call->GetCallType() == CallType::TYPE_BLUETOOTH) {
        return true;
    }
    return false;
}

void AudioControlManager::SetRingToneVolume(float volume)
{
    if (ring_ == nullptr) {
        TELEPHONY_LOGE("ring_ is nullptr ignore SetRingToneVolume");
        return;
    }
    if (volume >= 0.0f && volume <= 1.0f) {
        ring_->SetRingToneVolume(volume);
        return;
    } else {
        TELEPHONY_LOGE("volume is valid");
    }
}
bool AudioControlManager::IsScoTemporarilyDisabled()
{
    return isScoTemporarilyDisabled_;
}
void AudioControlManager::ExcludeBluetoothSco()
{
    TELEPHONY_LOGI("ExcludeBluetoothSco start");
    AudioSystemManager *audioSystemMananger = AudioSystemManager::GetInstance();
    if (audioSystemMananger == nullptr) {
        TELEPHONY_LOGI("audioSystemMananger nullptr");
        return;
    }
    std::shared_ptr<AudioStandard::AudioDeviceDescriptor> audioDev =
        std::make_shared<AudioStandard::AudioDeviceDescriptor>();
    if (audioDev != nullptr) {
        audioDev->macAddress_ = "";
        audioDev->deviceType_ = AudioStandard::DEVICE_TYPE_BLUETOOTH_SCO;
    }
    std::vector<std::shared_ptr<AudioDeviceDescriptor>> target;
    target.push_back(audioDev);
    if (target.size() <= 0) {
        TELEPHONY_LOGI("target.size <= 0");
        return;
    }
    int32_t result = audioSystemMananger->ExcludeOutputDevices(CALL_OUTPUT_DEVICES, target);
    if (result != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGI("ExcludeOutputDevices failed");
        return;
    }
    isScoTemporarilyDisabled_ = true;
    TELEPHONY_LOGI("ExcludeBluetoothSco end");
}
void AudioControlManager::UnexcludeBluetoothSco()
{
    TELEPHONY_LOGI("UnexcludeBluetoothSco start");
    AudioSystemManager *audioSystemMananger = AudioSystemManager::GetInstance();
    if (audioSystemMananger == nullptr) {
        TELEPHONY_LOGI("audioSystemMananger nullptr");
        return;
    }
    std::shared_ptr<AudioStandard::AudioDeviceDescriptor> audioDev =
        std::make_shared<AudioStandard::AudioDeviceDescriptor>();
    if (audioDev != nullptr) {
        audioDev->macAddress_ = "";
        audioDev->deviceType_ = AudioStandard::DEVICE_TYPE_BLUETOOTH_SCO;
    }
    std::vector<std::shared_ptr<AudioDeviceDescriptor>> target;
    target.push_back(audioDev);
    if (target.size() <= 0) {
        TELEPHONY_LOGI("target.size <= 0");
        return;
    }
    int32_t result = audioSystemMananger->UnexcludeOutputDevices(CALL_OUTPUT_DEVICES, target);
    if (result != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGI("UnexcludeOutputDevices failed");
        return;
    }
    isScoTemporarilyDisabled_ = false;
    TELEPHONY_LOGI("UnexcludeBluetoothSco end");
}

int32_t AudioControlManager::GetBackupVoiceVolume()
{
    return voiceVolume_;
}

void AudioControlManager::SaveVoiceVolume(int32_t volume)
{
    voiceVolume_ = volume;
}

void AudioControlManager::AdjustVolumesForCrs()
{
    auto audioProxy = DelayedSingleton<AudioProxy>::GetInstance();
    int32_t ringVolume = audioProxy->GetVolume(AudioStandard::AudioVolumeType::STREAM_RING);
    int32_t voiceVolume = audioProxy->GetVolume(AudioStandard::AudioVolumeType::STREAM_VOICE_CALL);
    TELEPHONY_LOGI("now ringVolume is %{public}d, voiceVolume is %{public}d", ringVolume, voiceVolume);
    audioProxy->SetVolume(AudioStandard::AudioVolumeType::STREAM_VOICE_CALL, ringVolume);
    SaveVoiceVolume(voiceVolume);
}

void AudioControlManager::RestoreVoiceValumeIfNecessary()
{
    auto voiceVolume = GetBackupVoiceVolume();
    TELEPHONY_LOGI("now voiceVolume is %{public}d", voiceVolume);
    if (voiceVolume >= 0) {
        DelayedSingleton<AudioProxy>::GetInstance()->SetVolume(
            AudioStandard::AudioVolumeType::STREAM_VOICE_CALL, voiceVolume);
        SaveVoiceVolume(-1);
    }
}
} // namespace Telephony
} // namespace OHOS