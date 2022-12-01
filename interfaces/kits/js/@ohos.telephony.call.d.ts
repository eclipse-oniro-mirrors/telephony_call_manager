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

import {AsyncCallback, Callback} from "./basic";

/**
 * Provides methods related to call management.
 *
 * @since 6
 * @syscap SystemCapability.Telephony.CallManager
 */
declare namespace call {
  /**
   * Makes a call.
   *
   * @param phoneNumber Indicates the called number.
   * @param options Indicates additional information carried in the call.
   * @param callback Returns {@code true} if the call request is successful; returns {@code false} otherwise.
   * Note that the value {@code true} indicates only the successful processing of the request; it does not mean
   * that the call is or can be connected.
   * @permission ohos.permission.PLACE_CALL
   */
  function dial(phoneNumber: string, callback: AsyncCallback<boolean>): void;
  function dial(phoneNumber: string, options: DialOptions, callback: AsyncCallback<boolean>): void;
  function dial(phoneNumber: string, options?: DialOptions): Promise<boolean>;

  /**
   * Go to the dial screen and the called number is displayed.
   *
   * @param phoneNumber Indicates the called number.
   * @syscap SystemCapability.Applications.Contacts
   * @since 7
   */
  function makeCall(phoneNumber: string, callback: AsyncCallback<void>): void;
  function makeCall(phoneNumber: string): Promise<void>;

  /**
   * Checks whether a call is ongoing.
   *
   * @param callback Returns {@code true} if at least one call is not in the {@link CallState#CALL_STATE_IDLE}
   * state; returns {@code false} otherwise.
   */
  function hasCall(callback: AsyncCallback<boolean>): void;
  function hasCall(): Promise<boolean>;

  /**
   * Obtains the call state.
   *
   * <p>If an incoming call is ringing or waiting, the system returns {@code CallState#CALL_STATE_RINGING}.
   * If at least one call is in the active, hold, or dialing state, the system returns
   * {@code CallState#CALL_STATE_OFFHOOK}.
   * In other cases, the system returns {@code CallState#CALL_STATE_IDLE}.
   *
   * @param callback Returns the call state.
   */
  function getCallState(callback: AsyncCallback<CallState>): void;
  function getCallState(): Promise<CallState>;

  /**
   * Stops the ringtone.
   *
   * <p>If an incoming call is ringing, the phone stops ringing. Otherwise, this method does not function.
   *
   * @permission ohos.permission.SET_TELEPHONY_STATE
   * @systemapi Hide this for inner system use.
   * @since 8
   */
  function muteRinger(callback: AsyncCallback<void>): void;
  function muteRinger(): Promise<void>;

  /**
   * Checks whether a device supports voice calls.
   *
   * <p>The system checks whether the device has the capability to initiate a circuit switching (CS) or IP multimedia
   * subsystem domain (IMS) call on a telephone service network. If the device supports only packet switching
   * (even if the device supports OTT calls), {@code false} is returned.
   *
   * @return Returns {@code true} if the device supports voice calls; returns {@code false} otherwise.
   * @since 7
   */
  function hasVoiceCapability(): boolean;

  /**
   * Checks whether a phone number is on the emergency number list.
   *
   * @param phoneNumber Indicates the phone number to check.
   * @param callback Returns {@code true} if the phone number is on the emergency number list;
   * returns {@code false} otherwise.
   * @since 7
   */
  function isEmergencyPhoneNumber(phoneNumber: string, callback: AsyncCallback<boolean>): void;
  function isEmergencyPhoneNumber(phoneNumber: string, options: EmergencyNumberOptions, callback: AsyncCallback<boolean>): void;
  function isEmergencyPhoneNumber(phoneNumber: string, options?: EmergencyNumberOptions): Promise<boolean>;

  /**
   * Formats a phone number according to the Chinese Telephone Code Plan. Before the formatting,
   * a phone number is in the format of country code (if any) + 3-digit service provider code
   * + 4-digit area code + 4-digit subscriber number. After the formatting,
   * each part is separated by a space.
   *
   * @param phoneNumber Indicates the phone number to format.
   * @param callback Returns the phone number after being formatted; returns an empty string
   * if the input phone number is invalid.
   * @since 7
   */
  function formatPhoneNumber(phoneNumber: string, callback: AsyncCallback<string>): void;
  function formatPhoneNumber(phoneNumber: string, options: NumberFormatOptions, callback: AsyncCallback<string>): void;
  function formatPhoneNumber(phoneNumber: string, options?: NumberFormatOptions): Promise<string>;

  /**
   * Formats a phone number into an E.164 representation.
   *
   * @param phoneNumber Indicates the phone number to format.
   * @param countryCode Indicates a two-digit country code defined in ISO 3166-1.
   * @param callback Returns an E.164 number; returns an empty string if the input phone number is invalid.
   * @since 7
   */
  function formatPhoneNumberToE164(phoneNumber: string, countryCode: string, callback: AsyncCallback<string>): void;
  function formatPhoneNumberToE164(phoneNumber: string, countryCode: string): Promise<string>;

  /**
   * Answers the incoming call.
   *
   * @param callId Indicates the identifier of the call to answer.
   * @permission ohos.permission.ANSWER_CALL
   * @systemapi Hide this for inner system use.
   * @since 7
   */
  function answer(callId: number, callback: AsyncCallback<void>): void;
  function answer(callId?: number): Promise<void>;

  /**
   * Answers the incoming call without callId.
   *
   * @permission ohos.permission.ANSWER_CALL
   * @systemapi Hide this for inner system use.
   * @since 9
   */
  function answer(callback: AsyncCallback<void>): void;

  /**
   * Hangups the foreground call.
   *
   * @param callId Indicates the identifier of the call to hangup.
   * @permission ohos.permission.ANSWER_CALL
   * @systemapi Hide this for inner system use.
   * @since 7
   */
  function hangup(callId: number, callback: AsyncCallback<void>): void;
  function hangup(callId?: number): Promise<void>;

  /**
   * Hangups the foreground call without callId.
   *
   * @systemapi Hide this for inner system use.
   * @since 9
   */
  function hangup(callback: AsyncCallback<void>): void;

  /**
   * Rejects the incoming call.
   *
   * @param callId Indicates the identifier of the call to reject.
   * @param options Indicates the text message to reject.
   * @permission ohos.permission.ANSWER_CALL
   * @systemapi Hide this for inner system use.
   * @since 7
   */
  function reject(callId: number, callback: AsyncCallback<void>): void;
  function reject(callId: number, options: RejectMessageOptions, callback: AsyncCallback<void>): void;
  function reject(callId?: number, options?: RejectMessageOptions): Promise<void>;

  /**
   * Rejects the incoming call without callId.
   *
   * @param options Indicates the text message to reject.
   * @permission ohos.permission.ANSWER_CALL
   * @systemapi Hide this for inner system use.
   * @since 9
   */
  function reject(callback: AsyncCallback<void>): void;
  function reject(options: RejectMessageOptions, callback: AsyncCallback<void>): void;

  /**
   * @permission ohos.permission.ANSWER_CALL
   * @systemapi Hide this for inner system use.
   * @since 7
   */
  function holdCall(callId: number, callback: AsyncCallback<void>): void;
  function holdCall(callId: number): Promise<void>;

  /**
   * @permission ohos.permission.ANSWER_CALL
   * @systemapi Hide this for inner system use.
   * @since 7
   */
  function unHoldCall(callId: number, callback: AsyncCallback<void>): void;
  function unHoldCall(callId: number): Promise<void>;

  /**
   * @permission ohos.permission.ANSWER_CALL
   * @systemapi Hide this for inner system use.
   * @since 7
   */
  function switchCall(callId: number, callback: AsyncCallback<void>): void;
  function switchCall(callId: number): Promise<void>;

  /**
   * @systemapi Hide this for inner system use.
   * @since 7
   */
  function combineConference(callId: number, callback: AsyncCallback<void>): void;
  function combineConference(callId: number): Promise<void>;

  /**
   * @systemapi Hide this for inner system use.
   * @since 7
   */
  function getMainCallId(callId: number, callback: AsyncCallback<number>): void;
  function getMainCallId(callId: number): Promise<number>;

  /**
   * @systemapi Hide this for inner system use.
   * @since 7
   */
  function getSubCallIdList(callId: number, callback: AsyncCallback<Array<string>>): void;
  function getSubCallIdList(callId: number): Promise<Array<string>>;

  /**
   * @systemapi Hide this for inner system use.
   * @since 7
   */
  function getCallIdListForConference(callId: number, callback: AsyncCallback<Array<string>>): void;
  function getCallIdListForConference(callId: number): Promise<Array<string>>;

  /**
   * @permission ohos.permission.GET_TELEPHONY_STATE
   * @systemapi Hide this for inner system use.
   * @since 7
   */
  function getCallWaitingStatus(slotId: number, callback: AsyncCallback<CallWaitingStatus>): void;
  function getCallWaitingStatus(slotId: number): Promise<CallWaitingStatus>;

  /**
   * @permission ohos.permission.SET_TELEPHONY_STATE
   * @systemapi Hide this for inner system use.
   * @since 7
   */
  function setCallWaiting(slotId: number, activate: boolean, callback: AsyncCallback<void>): void;
  function setCallWaiting(slotId: number, activate: boolean): Promise<void>;

  /**
   * @systemapi Hide this for inner system use.
   * @since 7
   */
  function startDTMF(callId: number, character: string, callback: AsyncCallback<void>): void;
  function startDTMF(callId: number, character: string): Promise<void>;

  /**
   * @systemapi Hide this for inner system use.
   * @since 7
   */
  function stopDTMF(callId: number, callback: AsyncCallback<void>): void;
  function stopDTMF(callId: number): Promise<void>;

  /**
   * @permission ohos.permission.SET_TELEPHONY_STATE
   * @systemapi Hide this for inner system use.
   * @since 7
   */
  function isInEmergencyCall(callback: AsyncCallback<boolean>): void;
  function isInEmergencyCall(): Promise<boolean>;

  /**
   * @permission ohos.permission.SET_TELEPHONY_STATE
   * @systemapi Hide this for inner system use.
   * @since 7
   */
  function on(type: 'callDetailsChange', callback: Callback<CallAttributeOptions>): void;

  /**
   * @permission ohos.permission.SET_TELEPHONY_STATE
   * @systemapi Hide this for inner system use.
   * @since 7
   */
  function off(type: 'callDetailsChange', callback?: Callback<CallAttributeOptions>): void;

  /**
   * @permission ohos.permission.SET_TELEPHONY_STATE
   * @systemapi Hide this for inner system use.
   * @since 8
   */
  function on(type: 'callEventChange', callback: Callback<CallEventOptions>): void;

  /**
   * @permission ohos.permission.SET_TELEPHONY_STATE
   * @systemapi Hide this for inner system use.
   * @since 8
   */
  function off(type: 'callEventChange', callback?: Callback<CallEventOptions>): void;

  /**
   * @permission ohos.permission.SET_TELEPHONY_STATE
   * @systemapi Hide this for inner system use.
   * @since 8
   */
  function on(type: 'callDisconnectedCause', callback: Callback<DisconnectedDetails>): void;

  /**
   * @permission ohos.permission.SET_TELEPHONY_STATE
   * @systemapi Hide this for inner system use.
   * @since 8
   */
  function off(type: 'callDisconnectedCause', callback?: Callback<DisconnectedDetails>): void;

  /**
   * Observe the result of MMI code
   *
   * @param type Indicates the observer type.
   * @param callback Return the result of MMI code.
   * @permission ohos.permission.SET_TELEPHONY_STATE
   * @throws {BusinessError} 201 - Permission denied.
   * @throws {BusinessError} 401 - Parameter error.
   * @throws {BusinessError} 8300001 - Invalid parameter value.
   * @throws {BusinessError} 8300002 - Operation failed. Cannot connect to service.
   * @throws {BusinessError} 8300003 - System internal error.
   * @throws {BusinessError} 8300004 - Do not have sim card.
   * @throws {BusinessError} 8300999 - Unknown error code.
   * @systemapi Hide this for inner system use.
   * @since 9
   */
  function on(type: 'mmiCodeResult', callback: Callback<MmiCodeResults>): void;

  /**
   * Unobserve the result of MMI code
   *
   * @param type Indicates the observer type.
   * @param callback Return the result of MMI code.
   * @permission ohos.permission.SET_TELEPHONY_STATE
   * @throws {BusinessError} 201 - Permission denied.
   * @throws {BusinessError} 401 - Parameter error.
   * @throws {BusinessError} 8300001 - Invalid parameter value.
   * @throws {BusinessError} 8300002 - Operation failed. Cannot connect to service.
   * @throws {BusinessError} 8300003 - System internal error.
   * @throws {BusinessError} 8300004 - Do not have sim card.
   * @throws {BusinessError} 8300999 - Unknown error code.
   * @systemapi Hide this for inner system use.
   * @since 9
   */
  function off(type: 'mmiCodeResult', callback?: Callback<MmiCodeResults>): void;

  /**
   * @systemapi Hide this for inner system use.
   * @since 8
   */
  function isNewCallAllowed(callback: AsyncCallback<boolean>): void;
  function isNewCallAllowed(): Promise<boolean>;

  /**
   * @systemapi Hide this for inner system use.
   * @since 8
   */
  function separateConference(callId: number, callback: AsyncCallback<void>): void;
  function separateConference(callId: number): Promise<void>;

  /**
   * @permission ohos.permission.GET_TELEPHONY_STATE
   * @systemapi Hide this for inner system use.
   * @since 8
   */
  function getCallRestrictionStatus(slotId: number, type: CallRestrictionType, callback: AsyncCallback<RestrictionStatus>): void;
  function getCallRestrictionStatus(slotId: number, type: CallRestrictionType): Promise<RestrictionStatus>;

  /**
   * @permission ohos.permission.SET_TELEPHONY_STATE
   * @systemapi Hide this for inner system use.
   * @since 8
   */
  function setCallRestriction(slotId: number, info: CallRestrictionInfo, callback: AsyncCallback<void>): void;
  function setCallRestriction(slotId: number, info: CallRestrictionInfo): Promise<void>;

  /**
   * @permission ohos.permission.GET_TELEPHONY_STATE
   * @systemapi Hide this for inner system use.
   * @since 8
   */
  function getCallTransferInfo(slotId: number, type: CallTransferType, callback: AsyncCallback<CallTransferResult>): void;
  function getCallTransferInfo(slotId: number, type: CallTransferType): Promise<CallTransferResult>;

  /**
   * @permission ohos.permission.SET_TELEPHONY_STATE
   * @systemapi Hide this for inner system use.
   * @since 8
   */
  function setCallTransfer(slotId: number, info: CallTransferInfo, callback: AsyncCallback<void>): void;
  function setCallTransfer(slotId: number, info: CallTransferInfo): Promise<void>;

  /**
   * @permission ohos.permission.SET_TELEPHONY_STATE
   * @systemapi Hide this for inner system use.
   * @since 8
   */
  function isRinging(callback: AsyncCallback<boolean>): void;
  function isRinging(): Promise<boolean>;

  /**
   * @systemapi Hide this for inner system use.
   * @since 8
   */
  function setMuted(callback: AsyncCallback<void>): void;
  function setMuted(): Promise<void>;

  /**
   * @systemapi Hide this for inner system use.
   * @since 8
   */
  function cancelMuted(callback: AsyncCallback<void>): void;
  function cancelMuted(): Promise<void>;

  /**
   * Set the audio device
   *
   * @param device Indicates the device of audio.
   * @param callback Returns {@code true} if the request is successful; returns {@code false} otherwise.
   * @systemapi Hide this for inner system use.
   * @since 8
   */
  function setAudioDevice(device: AudioDevice, callback: AsyncCallback<void>): void;

  /**
   * Set the audio device with options.
   *
   * @param device Indicates the device of audio.
   * @param options Indicates additional information, such as address of bluetooth.
   * @param callback Returns {@code true} if the request is successful; returns {@code false} otherwise.
   * @systemapi Hide this for inner system use.
   * @since 9
   */
  function setAudioDevice(device: AudioDevice, options: AudioDeviceOptions, callback: AsyncCallback<void>): void;
  function setAudioDevice(device: AudioDevice, options?: AudioDeviceOptions): Promise<void>;

  /**
   * @systemapi Hide this for inner system use.
   * @since 8
   */
  function joinConference(mainCallId: number, callNumberList: Array<string>, callback: AsyncCallback<void>): void;
  function joinConference(mainCallId: number, callNumberList: Array<string>): Promise<void>;

  /**
   * @systemapi Hide this for inner system use.
   * @since 8
   */
  function updateImsCallMode(callId: number, mode: ImsCallMode, callback: AsyncCallback<void>): void;
  function updateImsCallMode(callId: number, mode: ImsCallMode): Promise<void>;

  /**
   * @permission ohos.permission.SET_TELEPHONY_STATE
   * @systemapi Hide this for inner system use.
   * @since 8
   */
  function enableImsSwitch(slotId: number, callback: AsyncCallback<void>): void;
  function enableImsSwitch(slotId: number): Promise<void>;

  /**
   * @permission ohos.permission.SET_TELEPHONY_STATE
   * @systemapi Hide this for inner system use.
   * @since 8
   */
  function disableImsSwitch(slotId: number, callback: AsyncCallback<void>): void;
  function disableImsSwitch(slotId: number): Promise<void>;

  /**
   * @systemapi Hide this for inner system use.
   * @since 8
   */
  function isImsSwitchEnabled(slotId: number, callback: AsyncCallback<boolean>): void;
  function isImsSwitchEnabled(slotId: number): Promise<boolean>;

  /**
   * @systemapi Hide this for inner system use.
   * @since 8
   */
  export enum ImsCallMode {
    CALL_MODE_AUDIO_ONLY = 0,
    CALL_MODE_SEND_ONLY,
    CALL_MODE_RECEIVE_ONLY,
    CALL_MODE_SEND_RECEIVE,
    CALL_MODE_VIDEO_PAUSED,
  }

  /**
   * @systemapi Hide this for inner system use.
   * @since 8
   */
  export enum AudioDevice {
    DEVICE_EARPIECE,
    DEVICE_SPEAKER,
    DEVICE_WIRED_HEADSET,
    DEVICE_BLUETOOTH_SCO,
    DEVICE_MIC,
  }

  /**
   * @systemapi Hide this for inner system use.
   * @since 8
   */
  export enum CallRestrictionType {
    RESTRICTION_TYPE_ALL_INCOMING = 0,
    RESTRICTION_TYPE_ALL_OUTGOING,
    RESTRICTION_TYPE_INTERNATIONAL,
    RESTRICTION_TYPE_INTERNATIONAL_EXCLUDING_HOME,
    RESTRICTION_TYPE_ROAMING_INCOMING,
    RESTRICTION_TYPE_ALL_CALLS,
    RESTRICTION_TYPE_OUTGOING_SERVICES,
    RESTRICTION_TYPE_INCOMING_SERVICES,
  }

  /**
   * @systemapi Hide this for inner system use.
   * @since 8
   */
  export interface CallTransferInfo {
    transferNum: string;
    type: CallTransferType;
    settingType: CallTransferSettingType;
    startHour?: number;
    startMinute?: number;
    endHour?: number;
    endMinute?: number;
  }

  /**
   * @systemapi Hide this for inner system use.
   * @since 8
   */
  export enum CallTransferType {
    TRANSFER_TYPE_UNCONDITIONAL = 0,
    TRANSFER_TYPE_BUSY,
    TRANSFER_TYPE_NO_REPLY,
    TRANSFER_TYPE_NOT_REACHABLE,
  }

  /**
   * @systemapi Hide this for inner system use.
   * @since 8
   */
  export enum CallTransferSettingType {
    CALL_TRANSFER_DISABLE = 0,
    CALL_TRANSFER_ENABLE = 1,
    CALL_TRANSFER_REGISTRATION = 3,
    CALL_TRANSFER_ERASURE = 4,
  }

  /**
   * @systemapi Hide this for inner system use.
   * @since 7
   */
  export interface CallAttributeOptions {
    accountNumber: string;
    speakerphoneOn: boolean;
    accountId: number;
    videoState: VideoStateType;
    startTime: number;
    isEcc: boolean;
    callType: CallType;
    callId: number;
    callState: DetailedCallState;
    conferenceState: ConferenceState;
  }

  /**
   * @systemapi Hide this for inner system use.
   * @since 7
   */
  export enum ConferenceState {
    TEL_CONFERENCE_IDLE = 0,
    TEL_CONFERENCE_ACTIVE,
    TEL_CONFERENCE_DISCONNECTING,
    TEL_CONFERENCE_DISCONNECTED,
  }

  /**
   * @systemapi Hide this for inner system use.
   * @since 7
   */
  export enum CallType {
    TYPE_CS = 0, // CS
    TYPE_IMS = 1, // IMS
    TYPE_OTT = 2, // OTT
    TYPE_ERR_CALL = 3, // OTHER
  }

  /**
   * @systemapi Hide this for inner system use.
   * @since 7
   */
  export enum VideoStateType {
    TYPE_VOICE = 0, // Voice
    TYPE_VIDEO, // Video
  }

  /**
   * @systemapi Hide this for inner system use.
   * @since 7
   */
  export enum DetailedCallState {
    CALL_STATUS_ACTIVE = 0,
    CALL_STATUS_HOLDING,
    CALL_STATUS_DIALING,
    CALL_STATUS_ALERTING,
    CALL_STATUS_INCOMING,
    CALL_STATUS_WAITING,
    CALL_STATUS_DISCONNECTED,
    CALL_STATUS_DISCONNECTING,
    CALL_STATUS_IDLE,
  }

  /**
   * @systemapi Hide this for inner system use.
   * @since 8
   */
  export interface CallRestrictionInfo {
    type: CallRestrictionType;
    password: string;
    mode: CallRestrictionMode;
  }

  /**
   * @systemapi Hide this for inner system use.
   * @since 8
   */
  export enum CallRestrictionMode {
    RESTRICTION_MODE_DEACTIVATION = 0,
    RESTRICTION_MODE_ACTIVATION,
  }

  /**
   * @systemapi Hide this for inner system use.
   * @since 8
   */
  export interface CallEventOptions {
    eventId: CallAbilityEventId,
  }

  /**
   * @systemapi Hide this for inner system use.
   * @since 8
   */
  export enum CallAbilityEventId {
    EVENT_DIAL_NO_CARRIER = 1,
    EVENT_INVALID_FDN_NUMBER,
  }

  export enum CallState {
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
  }

  export interface DialOptions {
    /**
     * boolean means whether the call to be made is a video call. The value {@code false} indicates a voice call.
     */
    extras?: boolean;
    /**
     * @systemapi Hide this for inner system use.
     * @since 8
     */
    accountId?: number;
    /**
     * @systemapi Hide this for inner system use.
     * @since 8
     */
    videoState?: VideoStateType;
    /**
     * @systemapi Hide this for inner system use.
     * @since 8
     */
    dialScene?: DialScene;
    /**
     * @systemapi Hide this for inner system use.
     * @since 8
     */
    dialType?: DialType;
  }

  /**
   * @systemapi Hide this for inner system use.
   * @since 8
   */
  export enum DialScene {
    CALL_NORMAL = 0,
    CALL_PRIVILEGED = 1,
    CALL_EMERGENCY = 2,
  }

  /**
   * @systemapi Hide this for inner system use.
   * @since 8
   */
  export enum DialType {
    DIAL_CARRIER_TYPE = 0,
    DIAL_VOICE_MAIL_TYPE = 1,
    DIAL_OTT_TYPE = 2,
  }

  /**
   * @systemapi Hide this for inner system use.
   * @since 7
   */
  export interface RejectMessageOptions {
    messageContent: string;
  }

  /**
   * @systemapi Hide this for inner system use.
   * @since 8
   */
  export interface CallTransferResult {
    status: TransferStatus;
    number: string;
    startHour: number;
    startMinute: number;
    endHour: number;
    endMinute: number;
  }

  /**
   * @systemapi Hide this for inner system use.
   * @since 7
   */
  export enum CallWaitingStatus {
    CALL_WAITING_DISABLE = 0,
    CALL_WAITING_ENABLE = 1
  }

  /**
   * @systemapi Hide this for inner system use.
   * @since 8
   */
  export enum RestrictionStatus {
    RESTRICTION_DISABLE = 0,
    RESTRICTION_ENABLE = 1
  }

  /**
   * @systemapi Hide this for inner system use.
   * @since 8
   */
  export enum TransferStatus {
    TRANSFER_DISABLE = 0,
    TRANSFER_ENABLE = 1
  }

  /**
   * @since 7
   */
  export interface EmergencyNumberOptions {
    slotId?: number;
  }

  /**
   * @since 7
   */
  export interface NumberFormatOptions {
    countryCode?: string;
  }

  /**
   * @systemapi Hide this for inner system use.
   * @since 9
   */
  export interface AudioDeviceOptions {
    bluetoothAddress?: string;
  }

  /**
   * @systemapi Hide this for inner system use.
   * @since 9
   */
  export interface MmiCodeResults {
    /** Indicates the result of MMI code. */
    result: MmiCodeResult;
    /** Indicates the message of MMI code. */
    message: string;
  }

  /**
   * @systemapi Hide this for inner system use.
   * @since 9
   */
  export enum MmiCodeResult {
    /** Indicates the result of MMI code with successfully. */
    MMI_CODE_SUCCESS = 0,
    /** Indicates the result of MMI code with failed. */
    MMI_CODE_FAILED = 1
  }

  /**
   * @systemapi Hide this for inner system use.
   * @since 9
   */
  export enum DisconnectedReason {
    UNASSIGNED_NUMBER = 1,
    NO_ROUTE_TO_DESTINATION = 3,
    CHANNEL_UNACCEPTABLE = 6,
    OPERATOR_DETERMINED_BARRING = 8,
    CALL_COMPLETED_ELSEWHERE = 13,
    NORMAL_CALL_CLEARING = 16,
    USER_BUSY = 17,
    NO_USER_RESPONDING = 18,
    USER_ALERTING_NO_ANSWER = 19,
    CALL_REJECTED = 21,
    NUMBER_CHANGED = 22,
    CALL_REJECTED_DUE_TO_FEATURE_AT_THE_DESTINATION = 24,
    FAILED_PRE_EMPTION = 25,
    NON_SELECTED_USER_CLEARING = 26,
    DESTINATION_OUT_OF_ORDER = 27,
    INVALID_NUMBER_FORMAT = 28,
    FACILITY_REJECTED = 29,
    RESPONSE_TO_STATUS_ENQUIRY = 30,
    NORMAL_UNSPECIFIED = 31,
    NO_CIRCUIT_CHANNEL_AVAILABLE = 34,
    NETWORK_OUT_OF_ORDER = 38,
    TEMPORARY_FAILURE = 41,
    SWITCHING_EQUIPMENT_CONGESTION = 42,
    ACCESS_INFORMATION_DISCARDED = 43,
    REQUEST_CIRCUIT_CHANNEL_NOT_AVAILABLE = 44,
    RESOURCES_UNAVAILABLE_UNSPECIFIED = 47,
    QUALITY_OF_SERVICE_UNAVAILABLE = 49,
    REQUESTED_FACILITY_NOT_SUBSCRIBED = 50,
    INCOMING_CALLS_BARRED_WITHIN_THE_CUG = 55,
    BEARER_CAPABILITY_NOT_AUTHORIZED = 57,
    BEARER_CAPABILITY_NOT_PRESENTLY_AVAILABLE = 58,
    SERVICE_OR_OPTION_NOT_AVAILABLE_UNSPECIFIED = 63,
    BEARER_SERVICE_NOT_IMPLEMENTED = 65,
    ACM_EQUALTO_OR_GREATE_THAN_ACMMAX = 68,
    REQUESTED_FACILITY_NOT_IMPLEMENTED = 69,
    ONLY_RESTRICTED_DIGITAL_INFO_BEARER_CAPABILITY_IS_AVAILABLE = 70,
    SERVICE_OR_OPTION_NOT_IMPLEMENTED_UNSPECIFIED = 79,
    INVALID_TRANSACTION_IDENTIFIER_VALUE = 81,
    USER_NOT_MEMBER_OF_CUG = 87,
    INCOMPATIBLE_DESTINATION = 88,
    INVALID_TRANSIT_NETWORK_SELECTION = 91,
    SEMANTICALLY_INCORRECT_MESSAGE = 95,
    INVALID_MANDATORY_INFORMATION = 96,
    MESSAGE_TYPE_NON_EXISTENT_OR_NOT_IMPLEMENTED = 97,
    MESSAGE_TYPE_NOT_COMPATIBLE_WITH_PROTOCOL_STATE = 98,
    INFORMATION_ELEMENT_NON_EXISTENT_OR_NOT_IMPLEMENTED = 99,
    CONDITIONAL_IE_ERROR = 100,
    MESSAGE_NOT_COMPATIBLE_WITH_PROTOCOL_STATE = 101,
    RECOVERY_ON_TIMER_EXPIRED = 102,
    PROTOCOL_ERROR_UNSPECIFIED = 111,
    INTERWORKING_UNSPECIFIED = 127,
    CALL_BARRED = 240,
    FDN_BLOCKED = 241,
    IMSI_UNKNOWN_IN_VLR = 242,
    IMEI_NOT_ACCEPTED = 243,
    DIAL_MODIFIED_TO_USSD = 244,
    DIAL_MODIFIED_TO_SS = 245,
    DIAL_MODIFIED_TO_DIAL = 246,
    RADIO_OFF = 247,
    OUT_OF_SERVICE = 248,
    NO_VALID_SIM = 249,
    RADIO_INTERNAL_ERROR = 250,
    NETWORK_RESP_TIMEOUT = 251,
    NETWORK_REJECT = 252,
    RADIO_ACCESS_FAILURE = 253,
    RADIO_LINK_FAILURE = 254,
    RADIO_LINK_LOST = 255,
    RADIO_UPLINK_FAILURE = 256,
    RADIO_SETUP_FAILURE = 257,
    RADIO_RELEASE_NORMAL = 258,
    RADIO_RELEASE_ABNORMAL = 259,
    ACCESS_CLASS_BLOCKED = 260,
    NETWORK_DETACH = 261,
    INVALID_PARAMETER = 1025,
    SIM_NOT_EXIT = 1026,
    SIM_PIN_NEED = 1027,
    CALL_NOT_ALLOW = 1029,
    SIM_INVALID = 1045,
    UNKNOWN = 1279,
  }

  /**
   * @systemapi Hide this for inner system use.
   * @since 9
   */
  export interface DisconnectedDetails {
    reason: DisconnectedReason;
    message: string;
  }
}

export default call;
