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

import {AsyncCallback} from "./basic";
import {ResultSet} from "./@ohos.resultset";
import {PacMap} from "./@ohos.pacmap"

/**
 * Provides methods related to call management.
 *
 * @since 6
 */
declare namespace call {
  //need ohos.permission.PLACE_CALL
  function dial(phoneNumber: string, callback: AsyncCallback<boolean>): void;
  function dial(phoneNumber: string, options: DialOptions, callback: AsyncCallback<boolean>): void;
  function dial(phoneNumber: string, options?: DialOptions): Promise<boolean>;

  function getCallState(callback: AsyncCallback<CallState>): void;
  function getCallState(): Promise<CallState>;

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
     * boolean means whether the call to be made is a video call, and dependence on PacMap.
     */
    extras?: boolean | PacMap;
  }
}

export default call;