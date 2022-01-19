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

#include "ott_call_connection.h"

#include "call_manager_errors.h"
#include "telephony_log_wrapper.h"

#include "call_ability_report_proxy.h"

namespace OHOS {
namespace Telephony {
int32_t OTTCallConnection::Answer(const OttCallRequestInfo &requestInfo)
{
    AppExecFwk::PacMap info;
    PackCellularCallInfo(requestInfo, info);
    return DelayedSingleton<CallAbilityReportProxy>::GetInstance()->OttCallRequest(
        OttCallRequestId::OTT_REQUEST_ANSWER, info);
}

int32_t OTTCallConnection::HangUp(const OttCallRequestInfo &requestInfo)
{
    AppExecFwk::PacMap info;
    PackCellularCallInfo(requestInfo, info);
    return DelayedSingleton<CallAbilityReportProxy>::GetInstance()->OttCallRequest(
        OttCallRequestId::OTT_REQUEST_HANG_UP, info);
}

int32_t OTTCallConnection::Reject(const OttCallRequestInfo &requestInfo)
{
    AppExecFwk::PacMap info;
    PackCellularCallInfo(requestInfo, info);
    return DelayedSingleton<CallAbilityReportProxy>::GetInstance()->OttCallRequest(
        OttCallRequestId::OTT_REQUEST_REJECT, info);
}

int32_t OTTCallConnection::HoldCall(const OttCallRequestInfo &requestInfo)
{
    AppExecFwk::PacMap info;
    PackCellularCallInfo(requestInfo, info);
    return DelayedSingleton<CallAbilityReportProxy>::GetInstance()->OttCallRequest(
        OttCallRequestId::OTT_REQUEST_HOLD, info);
}

int32_t OTTCallConnection::UnHoldCall(const OttCallRequestInfo &requestInfo)
{
    AppExecFwk::PacMap info;
    PackCellularCallInfo(requestInfo, info);
    return DelayedSingleton<CallAbilityReportProxy>::GetInstance()->OttCallRequest(
        OttCallRequestId::OTT_REQUEST_UN_HOLD, info);
}

int32_t OTTCallConnection::SwitchCall(const OttCallRequestInfo &requestInfo)
{
    AppExecFwk::PacMap info;
    PackCellularCallInfo(requestInfo, info);
    return DelayedSingleton<CallAbilityReportProxy>::GetInstance()->OttCallRequest(
        OttCallRequestId::OTT_REQUEST_SWITCH, info);
}

int32_t OTTCallConnection::CombineConference(const OttCallRequestInfo &requestInfo)
{
    AppExecFwk::PacMap info;
    PackCellularCallInfo(requestInfo, info);
    return DelayedSingleton<CallAbilityReportProxy>::GetInstance()->OttCallRequest(
        OttCallRequestId::OTT_REQUEST_COMBINE_CONFERENCE, info);
}

int32_t OTTCallConnection::SeparateConference(const OttCallRequestInfo &requestInfo)
{
    AppExecFwk::PacMap info;
    PackCellularCallInfo(requestInfo, info);
    return DelayedSingleton<CallAbilityReportProxy>::GetInstance()->OttCallRequest(
        OttCallRequestId::OTT_REQUEST_SEPARATE_CONFERENCE, info);
}

int32_t OTTCallConnection::InviteToConference(
    const OttCallRequestInfo &requestInfo, const std::vector<std::string> &numberList)
{
    if (numberList.empty()) {
        TELEPHONY_LOGE("numberList is empty!");
        return TELEPHONY_ERR_ARGUMENT_INVALID;
    }
    AppExecFwk::PacMap info;
    std::vector<std::string> list = numberList;
    PackCellularCallInfo(requestInfo, info);
    info.PutIntValue("listCnt", list.size());
    std::vector<std::string>::iterator it = list.begin();
    for (; it != list.end(); it++) {
        if (!(*it).empty()) {
            info.PutStringValue("number", *it);
        }
    }
    return DelayedSingleton<CallAbilityReportProxy>::GetInstance()->OttCallRequest(
        OttCallRequestId::OTT_REQUEST_INVITE_TO_CONFERENCE, info);
}

int32_t OTTCallConnection::UpdateImsCallMode(const OttCallRequestInfo &requestInfo, ImsCallMode mode)
{
    AppExecFwk::PacMap info;
    PackCellularCallInfo(requestInfo, info);
    info.PutIntValue("callMediaMode", mode);
    return DelayedSingleton<CallAbilityReportProxy>::GetInstance()->OttCallRequest(
        OttCallRequestId::OTT_REQUEST_UPDATE_CALL_MEDIA_MODE, info);
}

void OTTCallConnection::PackCellularCallInfo(const OttCallRequestInfo &requestInfo, AppExecFwk::PacMap &info)
{
    std::string number(requestInfo.phoneNum);
    std::string name(requestInfo.bundleName);
    info.PutStringValue("phoneNum", number);
    info.PutStringValue("bundleName", name);
    info.PutIntValue("videoState", static_cast<int32_t>(requestInfo.videoState));
}
} // namespace Telephony
} // namespace OHOS