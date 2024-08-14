/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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
const call = requireInternal('telephony.call');
const ARGUMENTS_LEN_TWO = 2;
const ARGUMENTS_LEN_ONE = 1;
async function makeCallFunc(...args) {
    if ((arguments.length === ARGUMENTS_LEN_TWO && typeof arguments[1] === 'function') ||
        (arguments.length === ARGUMENTS_LEN_ONE)) {
        try {
            let context = getContext(this);
            await startAbility(arguments, context);
            if (arguments.length === ARGUMENTS_LEN_TWO && typeof arguments[1] === 'function') {
                return arguments[1](undefined, undefined);
            }
            return new Promise((resolve, reject) => {
                resolve();
            });
        } catch (error) {
            console.log("[call] makeCall error: " + error);
            if (arguments.length === ARGUMENTS_LEN_TWO && typeof arguments[1] === 'function') {
                return arguments[1](error, undefined);
            }
            return new Promise((resolve, reject) => {
                reject(error);
            });
        }
    } else if (arguments.length === ARGUMENTS_LEN_TWO && typeof arguments[1] === 'string') {
        try {
            let context = arguments[0];
            await startAbility(arguments, context);
            return new Promise((resolve, reject) => {
                resolve();
            });
        } catch (error) {
            console.log("[call] makeCall error: " + error);
            return new Promise((resolve, reject) => {
                reject(error);
            });
        }
    } else {
        console.log('[call] makeCall callback invalid');
        throw Error('invalid callback');
    }
    
}

async function startAbility(args, context) {
    let config = {
        parameters: {
            'phoneNumber': '',
            'pageFlag': 'page_flag_edit_before_calling'
        },
        bundleName: 'com.ohos.contacts',
        abilityName: 'com.ohos.contacts.MainAbility'
    };
    if (args.length > 0 && typeof args[0] === 'string') {
        config.parameters.phoneNumber = args[0];
    } else if (args.length > 0 && typeof args[1] === 'string') {
        config.parameters.phoneNumber = args[1];
    }
    if (context) {
        await context.startAbility(config);
    } else {
        call.makeCall(config.parameters.phoneNumber);
    }
}

export default {
    makeCall: makeCallFunc,
    dialCall: call.dialCall,
    muteRinger: call.muteRinger,
    answerCall: call.answerCall,
    hangUpCall: call.hangUpCall,
    hangup: call.hangup,
    rejectCall: call.rejectCall,
    reject: call.reject,
    holdCall: call.holdCall,
    unHoldCall: call.unHoldCall,
    switchCall: call.switchCall,
    setCallPreferenceMode: call.setCallPreferenceMode,
    combineConference: call.combineConference,
    kickOutFromConference: call.kickOutFromConference,
    getMainCallId: call.getMainCallId,
    getSubCallIdList: call.getSubCallIdList,
    getCallIdListForConference: call.getCallIdListForConference,
    getCallWaitingStatus: call.getCallWaitingStatus,
    setCallWaiting: call.setCallWaiting,
    startDTMF: call.startDTMF,
    stopDTMF: call.stopDTMF,
    postDialProceed: call.postDialProceed,
    isInEmergencyCall: call.isInEmergencyCall,
    on: call.on,
    off: call.off,
    isNewCallAllowed: call.isNewCallAllowed,
    separateConference: call.separateConference,
    getCallRestrictionStatus: call.getCallRestrictionStatus,
    setCallRestriction: call.setCallRestriction,
    setCallRestrictionPassword: call.setCallRestrictionPassword,
    getCallTransferInfo: call.getCallTransferInfo,
    setCallTransfer: call.setCallTransfer,
    isRinging: call.isRinging,
    setMuted: call.setMuted,
    cancelMuted: call.cancelMuted,
    setAudioDevice: call.setAudioDevice,
    joinConference: call.joinConference,
    updateImsCallMode: call.updateImsCallMode,
    enableImsSwitch: call.enableImsSwitch,
    disableImsSwitch: call.disableImsSwitch,
    isImsSwitchEnabled: call.isImsSwitchEnabled,
    isImsSwitchEnabledSync: call.isImsSwitchEnabledSync,
    closeUnfinishedUssd: call.closeUnfinishedUssd,
    setVoNRState: call.setVoNRState,
    getVoNRState: call.getVoNRState,
    canSetCallTransferTime: call.canSetCallTransferTime,
    inputDialerSpecialCode: call.inputDialerSpecialCode,
    reportOttCallDetailsInfo: call.reportOttCallDetailsInfo,
    reportOttCallEventInfo: call.reportOttCallEventInfo,
    removeMissedIncomingCallNotification: call.removeMissedIncomingCallNotification,
    setVoIPCallState: call.setVoIPCallState,
    sendCallUiEvent: call.sendCallUiEvent,
    DialOptions: call.DialOptions,
    DialCallOptions: call.DialCallOptions,
    ImsCallMode: call.ImsCallMode,
    VoNRState: call.VoNRState,
    AudioDevice: call.AudioDevice,
    AudioDeviceType: call.AudioDeviceType,
    AudioDeviceCallbackInfo: call.AudioDeviceCallbackInfo,
    CallRestrictionType: call.CallRestrictionType,
    CallTransferInfo: call.CallTransferInfo,
    CallTransferType: call.CallTransferType,
    CallTransferSettingType: call.CallTransferSettingType,
    CallAttributeOptions: call.CallAttributeOptions,
    VoipCallAttribute: call.VoipCallAttribute,
    ConferenceState: call.ConferenceState,
    CallType: call.CallType,
    VideoStateType: call.VideoStateType,
    DetailedCallState: call.DetailedCallState,
    CallRestrictionInfo: call.CallRestrictionInfo,
    CallRestrictionMode: call.CallRestrictionMode,
    CallEventOptions: call.CallEventOptions,
    CallAbilityEventId: call.CallAbilityEventId,
    DialScene: call.DialScene,
    DialType: call.DialType,
    RejectMessageOptions: call.RejectMessageOptions,
    CallTransferResult: call.CallTransferResult,
    CallWaitingStatus: call.CallWaitingStatus,
    RestrictionStatus: call.RestrictionStatus,
    TransferStatus: call.TransferStatus,
    DisconnectedDetails: call.DisconnectedDetails,
    DisconnectedReason: call.DisconnectedReason,
    MmiCodeResults: call.MmiCodeResults,
    MmiCodeResult: call.MmiCodeResult,
    answer: call.answer,
    cancelCallUpgrade: call.cancelCallUpgrade,
    controlCamera: call.controlCamera,
    setPreviewSurface: call.setPreviewSurface,
    setCameraZoom: call.setCameraZoom,
    setDisplaySurface: call.setDisplaySurface,
    setDeviceDirection: call.setDeviceDirection,
    VideoRequestResultType: call.VideoRequestResultType,
    DeviceDirection: call.DeviceDirection,
    CallSessionEventId: call.CallSessionEventId,
    ImsCallModeInfo: call.ImsCallModeInfo,
    CallSessionEvent: call.CallSessionEvent,
    PeerDimensionsDetail: call.PeerDimensionsDetail,
    CameraCapabilities: call.CameraCapabilities,
    NumberMarkInfo: call.NumberMarkInfo,
    MarkType: call.MarkType,
    dial: call.dial,
    hasCall: call.hasCall,
    hasCallSync: call.hasCallSync,
    getCallState: call.getCallState,
    getCallStateSync: call.getCallStateSync,
    hasVoiceCapability: call.hasVoiceCapability,
    isEmergencyPhoneNumber: call.isEmergencyPhoneNumber,
    formatPhoneNumber: call.formatPhoneNumber,
    formatPhoneNumberToE164: call.formatPhoneNumberToE164,
    CallState: call.CallState,
    EmergencyNumberOptions: call.EmergencyNumberOptions,
    NumberFormatOptions: call.NumberFormatOptions,
    startRTT: call.startRTT,
    stopRTT: call.stopRTT
};