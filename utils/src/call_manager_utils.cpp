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

#include "call_manager_utils.h"
#include "call_manager_base.h"
#include "call_object_manager.h"
#include "want_params_wrapper.h"

namespace OHOS {
namespace Telephony {

bool CallManagerUtils::IsForcedReportVoiceCall(const CallAttributeInfo &info)
{
    if (info.callState != TelCallState::CALL_STATUS_INCOMING) {
        return false;
    }
    auto call = CallObjectManager::GetOneCallObject(info.callId);
    if (call == nullptr) {
        return false;
    }
    return call->IsForcedReportVoiceCall();
}

void CallManagerUtils::WriteCallAttributeInfo(const CallAttributeInfo &info, MessageParcel &messageParcel)
{
    messageParcel.WriteCString(info.accountNumber);
    messageParcel.WriteCString(info.bundleName);
    messageParcel.WriteBool(info.speakerphoneOn);
    messageParcel.WriteInt32(info.accountId);
    if (IsForcedReportVoiceCall(info)) {
        messageParcel.WriteInt32(static_cast<int32_t>(VideoStateType::TYPE_VOICE));
    } else {
        messageParcel.WriteInt32(static_cast<int32_t>(info.videoState));
    }
    messageParcel.WriteInt64(info.startTime);
    messageParcel.WriteBool(info.isEcc);
    messageParcel.WriteInt32(static_cast<int32_t>(info.callType));
    messageParcel.WriteInt32(info.callId);
    messageParcel.WriteInt32(static_cast<int32_t>(info.callState));
    messageParcel.WriteInt32(static_cast<int32_t>(info.conferenceState));
    messageParcel.WriteInt64(info.callBeginTime);
    messageParcel.WriteInt64(info.callEndTime);
    messageParcel.WriteInt64(info.ringBeginTime);
    messageParcel.WriteInt64(info.ringEndTime);
    messageParcel.WriteInt32(static_cast<int32_t>(info.callDirection));
    messageParcel.WriteInt32(static_cast<int32_t>(info.answerType));
    messageParcel.WriteInt32(info.index);
    messageParcel.WriteInt32(info.crsType);
    messageParcel.WriteInt32(info.originalCallType);
    messageParcel.WriteCString(info.numberLocation);
    messageParcel.WriteInt32(static_cast<int32_t>(info.numberMarkInfo.markType));
    messageParcel.WriteCString(info.numberMarkInfo.markContent);
    messageParcel.WriteInt32(info.numberMarkInfo.markCount);
    messageParcel.WriteCString(info.numberMarkInfo.markSource);
    messageParcel.WriteBool(info.numberMarkInfo.isCloud);
    messageParcel.WriteCString(info.numberMarkInfo.markDetails);
    messageParcel.WriteCString(info.contactName);
    messageParcel.WriteString(info.extraParamsString);
    WriteVoipCallInfo(info, messageParcel);
    messageParcel.WriteInt32(info.phoneOrWatch);
}

bool CallManagerUtils::IsBundleInstalled(const std::string &bundleName, int32_t userId)
{
    bool isInstalled = false;
    sptr<ISystemAbilityManager> systemAbilityManager =
        SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (!systemAbilityManager) {
        return false;
    }
    sptr<IRemoteObject> remoteObject = systemAbilityManager->GetSystemAbility(BUNDLE_MGR_SERVICE_SYS_ABILITY_ID);
    sptr<AppExecFwk::IBundleMgr> proxy = iface_cast<AppExecFwk::IBundleMgr>(remoteObject);
    if (proxy == nullptr) {
        return false;
    }
    if (FAILED(proxy->IsBundleInstalled(bundleName, userId, 0, isInstalled))) {
        return false;
    }
    return isInstalled;
}

void CallManagerUtils::WriteVoipCallInfo(const CallAttributeInfo &info, MessageParcel &messageParcel)
{
    if (info.callType == CallType::TYPE_VOIP) {
    messageParcel.WriteString(info.voipCallInfo.voipCallId);
    messageParcel.WriteString(info.voipCallInfo.userName);
    messageParcel.WriteString(info.voipCallInfo.abilityName);
    messageParcel.WriteString(info.voipCallInfo.extensionId);
    messageParcel.WriteString(info.voipCallInfo.voipBundleName);
    messageParcel.WriteBool(info.voipCallInfo.showBannerForIncomingCall);
    messageParcel.WriteBool(info.voipCallInfo.isConferenceCall);
    messageParcel.WriteBool(info.voipCallInfo.isVoiceAnswerSupported);
    messageParcel.WriteBool(info.voipCallInfo.isUserMuteRingToneAllowed);
    messageParcel.WriteBool(info.voipCallInfo.isRemoteDeviceControlAllowed);
    messageParcel.WriteBool(info.voipCallInfo.isDialingAllowedDuringCarrierCall);
    messageParcel.WriteBool(info.voipCallInfo.hasMicPermission);
    messageParcel.WriteBool(info.voipCallInfo.isCapsuleSticky);
    messageParcel.WriteInt32(info.voipCallInfo.uid);
    messageParcel.WriteUInt8Vector(info.voipCallInfo.userProfile);
    }
}
} // namespace Telephony
} // namespace OHOS