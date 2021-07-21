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

import { AsyncCallback } from "./basic";

declare namespace call {
    function dial(phoneNumber: string, callback: AsyncCallback<boolean>): void;
    function dial(phoneNumber: string, options: DialOptions, callback: AsyncCallback<boolean>): void;
    function dial(phoneNumber: string, options?: DialOptions): Promise<boolean>;

    //@SystemApi
    function answer(callId: number, callback: AsyncCallback<boolean>): void;
    function answer(callId: number): Promise<boolean>;

    //@SystemApi
    function reject(callId: number, isSendSms: Boolean, msg: string, callback: AsyncCallback<boolean>): void;
    function reject(callId: number, isSendSms: Boolean, msg: string): Promise<boolean>;

    //@SystemApi
    function holdCall(callId: number, callback: AsyncCallback<boolean>): void;
    function holdCall(callId: number): Promise<boolean>;

    //@SystemApi
    function unHoldCall(callId: number, callback: AsyncCallback<boolean>): void;
    function unHoldCall(callId: number): Promise<boolean>;

    //@SystemApi
    function hangup(callId: number, callback: AsyncCallback<boolean>): void;
    function hangup(callId: number): Promise<boolean>;

    //@SystemApi
    function switchCall(callId: number, callback: AsyncCallback<boolean>): void;
    function switchCall(callId: number): Promise<boolean>;

    //@SystemApi
    function combineConference(callId: number, callback: AsyncCallback<boolean>): void;
    function combineConference(callId: number): Promise<boolean>;

    //@SystemApi
    function getMainCallId(callId: number, callback: AsyncCallback<number>): void;
    function getMainCallId(callId: number): Promise<number>;

    //@SystemApi
    function getSubCallIdList(callId: number, callback: AsyncCallback<Array<string>>): void;
    function getSubCallIdList(callId: number): Promise<Array<string>>;

    //@SystemApi
    function getCallIdListForConference(callId: number, callback: AsyncCallback<Array<string>>): void;
    function getCallIdListForConference(callId: number): Promise<Array<string>>;

    //@SystemApi
    function getCallWaiting(slotId: number, callback: AsyncCallback<CallWaitingCallbackInfo>): void;
    function getCallWaiting(slotId: number): Promise<CallWaitingCallbackInfo>;

    //@SystemApi
    function setCallWaiting(slotId: number, activate: boolean, callback: AsyncCallback<boolean>): void;
    function setCallWaiting(slotId: number, activate: boolean): Promise<boolean>;

    //@SystemApi
    function startDtmf(callId: number, str: number, callback: AsyncCallback<boolean>): void;
    function startDtmf(callId: number, str: number): Promise<boolean>;

    //@SystemApi
    function stopDtmf(callId: number, callback: AsyncCallback<boolean>): void;
    function stopDtmf(callId: number): Promise<boolean>;

    function getCallState(callback: AsyncCallback<CallState>): void;
    function getCallState(): Promise<CallState>;

    function hasCall(callback: AsyncCallback<boolean>): void;
    function hasCall(): Promise<boolean>;

    function isEmergencyPhoneNumber(phoneNumber: string, callback: AsyncCallback<boolean>): void;
    function isEmergencyPhoneNumber(phoneNumber: string, options: EmergencyNumberOptions, callback: AsyncCallback<boolean>): void
    function isEmergencyPhoneNumber(phoneNumber: string, options?: EmergencyNumberOptions): Promise<boolean>

    function formatPhoneNumber(phoneNumber: string, callback: AsyncCallback<string>): void;
    function formatPhoneNumber(phoneNumber: string, options: NumberFormatOptions, callback: AsyncCallback<string>): void;
    function formatPhoneNumber(phoneNumber: string, options?: NumberFormatOptions): Promise<string>;

    function formatPhoneNumberToE164(phoneNumber: string, countryCode: string, callback: AsyncCallback<string>): void;
    function formatPhoneNumberToE164(phoneNumber: string, countryCode: string): Promise<string>;

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
        accountId: number;
        videoState: number;
        dialScene: DialScene;
        dialType: DialType;
    }

    export interface CallWaitingCallbackInfo {
        result: number,
        status: number,
        classCw: number,
    }

    export enum DialScene {
        CALL_NORMAL = 0,
        CALL_PRIVILEGED = 1,
        CALL_EMERGENCY = 2,
    }

    export enum DialType {
        DIAL_CARRIER_TYPE = 0,
        DIAL_VOICE_MAIL_TYPE,
        DIAL_OTT_TYPE,
        DIAL_UNKNOW_TYPE,
    };

    export enum VideoState {
        TYPE_VOICE = 0,
        TYPE_VIDEO = 1,
    }

    export enum CallType {
        TYPE_CS = 0, // CS
        TYPE_IMS = 1, // IMS
        TYPE_OTT = 2, // OTT
        TYPE_ERR_CALL = 3, // OTHER
    }

    // 3GPP TS 22.030 V4.0.0 (2001-03)
    // 3GPP TS 22.088 V4.0.0 (2001-03)
    export enum CallRestrictionType {
        RESTRICTION_TYPE_ALL_OUTGOING = 0,
        RESTRICTION_TYPE_INTERNATIONAL = 1,
        RESTRICTION_TYPE_INTERNATIONAL_EXCLUDING_HOME = 2,
        RESTRICTION_TYPE_ALL_INCOMING = 3,
        RESTRICTION_TYPE_ROAMING_INCOMING = 4,
    };

    // 3GPP TS 22.088 V4.0.0 (2001-03)
    export enum CallRestrictionMode {
        RESTRICTION_MODE_DEACTIVATION = 0,
        RESTRICTION_MODE_ACTIVATION = 1,
        RESTRICTION_MODE_QUERY = 2,
    };

    export interface CallRestrictionInfo {
        fac: CallRestrictionType;
        password: string;
        mode: CallRestrictionMode;
    };

    /**
     * 27007-430_2001 7.11	Call forwarding number and conditions +CCFC
     * 3GPP TS 22.082 [4]
     * <mode>:
     * 0	disable
     * 1	enable
     * 2	query status
     * 3	registration
     * 4	erasure
     */
    export enum CallTransferSettingType {
        DISABLE = 0,
        ENABLE = 1,
        QUERY_STATUS = 2,
        REGISTRATION = 3,
        ERASURE = 4,
    };

    /**
     * 27007-430_2001 7.11	Call forwarding number and conditions +CCFC
     * 3GPP TS 22.082 [4]
     * <reason>:
     * 0	unconditional
     * 1	mobile busy
     * 2	no reply
     * 3	not reachable
     */
    export enum CallTransferType {
        UNCONDITIONAL = 0,
        MOBILE_BUSY = 1,
        NO_REPLY = 2,
        NOT_REACHABLE = 3,
    };

    export interface CallTransferInfo {
        settingType: CallTransferSettingType;
        type: CallTransferType;
        transferNum: string;
    };

    export interface NumberFormatOptions {
        countryCode?: string;
    }

    export interface EmergencyNumberOptions {
        slotId: number;
    }

    export enum SupplementErrType {
        HTEL_ERR_GENERIC_FAILURE = 1,
        HTEL_ERR_INVALID_PARAMETER = 2,
        HTEL_ERR_MEMORY_FULL = 3,
        HTEL_ERR_CMD_SEND_FAILURE = 4,
        HTEL_ERR_CMD_NO_CARRIER = 5,
        HTEL_ERR_INVALID_RESPONSE = 6,
    }
}