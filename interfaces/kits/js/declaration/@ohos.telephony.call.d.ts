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

import {AsyncCallback, Callback} from "./basic";

/**
 * Provides methods related to call management.
 *
 * @since 7
 * @sysCap SystemCapability.Telephony.DCall
 * @devices phone, tablet, wearable
 */
declare namespace call {
    /**
     * Makes a call.
     *
     * <p>Applications must have the {@code ohos.permission.PLACE_CALL} permission to call this method.
     *
     * @param phoneNumber Indicates the called number.
     * @param options Indicates additional information carried in the call.
     * @param callback Returns {@code true} if the call request is successful; returns {@code false} otherwise.
     * Note that the value {@code true} indicates only the successful processing of the request; it does not mean
     * that the call is or can be connected.
     * @permission ohos.permission.PLACE_CALL
     * @since 6
     */
    function dial(phoneNumber: string, callback: AsyncCallback<boolean>): void;
    function dial(phoneNumber: string, options: DialOptions, callback: AsyncCallback<boolean>): void;
    function dial(phoneNumber: string, options?: DialOptions): Promise<boolean>;

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

    function isEmergencyPhoneNumber(phoneNumber: string, callback: AsyncCallback<boolean>): void;
    function isEmergencyPhoneNumber(phoneNumber: string, options: EmergencyNumberOptions, callback: AsyncCallback<boolean>): void;
    function isEmergencyPhoneNumber(phoneNumber: string, options?: EmergencyNumberOptions): Promise<boolean>;

    function formatPhoneNumber(phoneNumber: string, callback: AsyncCallback<string>): void;
    function formatPhoneNumber(phoneNumber: string, options: NumberFormatOptions, callback: AsyncCallback<string>): void;
    function formatPhoneNumber(phoneNumber: string, options?: NumberFormatOptions): Promise<string>;

    function formatPhoneNumberToE164(phoneNumber: string, countryCode: string, callback: AsyncCallback<string>): void;
    function formatPhoneNumberToE164(phoneNumber: string, countryCode: string): Promise<string>;

    /**
     * @systemapi Hide this for inner system use.
     */
    function answer(callId: number, callback: AsyncCallback<void>): void;
    function answer(callId: number): Promise<void>;

    /**
     * @systemapi Hide this for inner system use.
     */
    function hangup(callId: number, callback: AsyncCallback<void>): void;
    function hangup(callId: number): Promise<void>;

    /**
     * @systemapi Hide this for inner system use.
     */
    function reject(callId: number, callback: AsyncCallback<void>): void;
    function reject(callId: number, options: RejectMessageOptions, callback: AsyncCallback<void>): void;
    function reject(callId: number, options?: RejectMessageOptions): Promise<void>;

    /**
     * @systemapi Hide this for inner system use.
     */
    function holdCall(callId: number, callback: AsyncCallback<void>): void;
    function holdCall(callId: number): Promise<void>;

    /**
     * @systemapi Hide this for inner system use.
     */
    function unHoldCall(callId: number, callback: AsyncCallback<void>): void;
    function unHoldCall(callId: number): Promise<void>;

    /**
     * @systemapi Hide this for inner system use.
     */
    function switchCall(callId: number, callback: AsyncCallback<void>): void;
    function switchCall(callId: number): Promise<void>;

    /**
     * @systemapi Hide this for inner system use.
     */
    function combineConference(callId: number, callback: AsyncCallback<void>): void;
    function combineConference(callId: number): Promise<void>;

    /**
     * @systemapi Hide this for inner system use.
     */
    function getMainCallId(callId: number, callback: AsyncCallback<number>): void;
    function getMainCallId(callId: number): Promise<number>;

    /**
     * @systemapi Hide this for inner system use.
     */
    function getSubCallIdList(callId: number, callback: AsyncCallback<Array<string>>): void;
    function getSubCallIdList(callId: number): Promise<Array<string>>;

    /**
     * @systemapi Hide this for inner system use.
     */
    function getCallIdListForConference(callId: number, callback: AsyncCallback<Array<string>>): void;
    function getCallIdListForConference(callId: number): Promise<Array<string>>;

    /**
     * @systemapi Hide this for inner system use.
     */
    function getCallWaitingStatus(slotId: number, callback: AsyncCallback<CallWaitingStatus>): void;
    function getCallWaitingStatus(slotId: number): Promise<CallWaitingStatus>;

    /**
     * @systemapi Hide this for inner system use.
     */
    function setCallWaiting(slotId: number, activate: boolean, callback: AsyncCallback<void>): void;
    function setCallWaiting(slotId: number, activate: boolean): Promise<void>;

    /**
     * @systemapi Hide this for inner system use.
     */
    function startDTMF(callId: number, character: string, callback: AsyncCallback<void>): void;
    function startDTMF(callId: number, character: string): Promise<void>;

    /**
     * @systemapi Hide this for inner system use.
     */
    function stopDTMF(callId: number, callback: AsyncCallback<void>): void;
    function stopDTMF(callId: number): Promise<void>;

    /**
     * @permission ohos.permission.SET_TELEPHONY_STATE
     * @systemapi Hide this for inner system use.
     */
    function isInEmergencyCall(callback: AsyncCallback<boolean>): void;
    function isInEmergencyCall(): Promise<boolean>;

    /**
     * @systemapi Hide this for inner system use.
     */
    function on(type: 'callDetailsChange', callback: Callback<CallAttributeOptions>): void;
    function off(type: 'callDetailsChange', callback?: Callback<CallAttributeOptions>): void;

    /**
     * @systemapi Hide this for inner system use.
     */
    export interface CallAttributeOptions {
        accountNumber: string,
        speakerphoneOn: boolean,
        accountId: number,
        videoState: VideoStateType,
        startTime: number,
        isEcc: boolean,
        callType: CallType,
        callId: number,
        callState: DetailedCallState,
        conferenceState: ConferenceState,
    }

    /**
     * @systemapi Hide this for inner system use.
     */
    export enum ConferenceState {
        TEL_CONFERENCE_IDLE = 0,
        TEL_CONFERENCE_ACTIVE,
        TEL_CONFERENCE_DISCONNECTING,
        TEL_CONFERENCE_DISCONNECTED,
    }

    /**
     * @systemapi Hide this for inner system use.
     */
    export enum CallType {
        TYPE_CS = 0, // CS
        TYPE_IMS = 1, // IMS
        TYPE_OTT = 2, // OTT
        TYPE_ERR_CALL = 3, // OTHER
    }

    /**
     * @systemapi Hide this for inner system use.
     */
    export enum VideoStateType {
        TYPE_VOICE = 0, // Voice
        TYPE_VIDEO, // Video
    }

    /**
     * @systemapi Hide this for inner system use.
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
     */
    export interface RejectMessageOptions {
        messageContent: string,
    }

    /**
     * @systemapi Hide this for inner system use.
     */
    export enum CallWaitingStatus {
        CALL_WAITING_DISABLE = 0,
        CALL_WAITING_ENABLE = 1
    }

    export interface EmergencyNumberOptions {
        slotId?: number;
    }

    export interface NumberFormatOptions {
        countryCode?: string;
    }

    export interface NumberCompareOptions {
        numA: string;
        netISOA: string;
        numB: string;
        netISOB: string;
    }
}

export default call;