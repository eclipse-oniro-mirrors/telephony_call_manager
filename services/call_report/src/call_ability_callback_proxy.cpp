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

#include "call_ability_callback_proxy.h"

#include "message_option.h"
#include "message_parcel.h"

#include "call_manager_errors.h"

namespace OHOS {
namespace Telephony {
CallAbilityCallbackProxy::CallAbilityCallbackProxy(const sptr<IRemoteObject> &impl)
    : IRemoteProxy<ICallAbilityCallback>(impl)
{}

int32_t CallAbilityCallbackProxy::OnCallDetailsChange(const CallAttributeInfo &info)
{
    MessageParcel dataParcel;
    MessageParcel replyParcel;
    MessageOption option;
    if (!dataParcel.WriteInterfaceToken(CallAbilityCallbackProxy::GetDescriptor())) {
        TELEPHONY_LOGE("write descriptor fail");
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    int32_t length = sizeof(CallAttributeInfo);
    dataParcel.WriteInt32(length);
    dataParcel.WriteRawData((const void *)&info, length);
    if (Remote() == nullptr) {
        TELEPHONY_LOGE("function Remote() return nullptr!");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    int32_t error = Remote()->SendRequest(UPDATE_CALL_STATE_INFO, dataParcel, replyParcel, option);
    if (error != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("update call state info failed, error: %{public}d", error);
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    return replyParcel.ReadInt32();
}

int32_t CallAbilityCallbackProxy::OnCallEventChange(const CallEventInfo &info)
{
    MessageParcel dataParcel;
    MessageParcel replyParcel;
    MessageOption option;
    if (!dataParcel.WriteInterfaceToken(CallAbilityCallbackProxy::GetDescriptor())) {
        TELEPHONY_LOGE("write descriptor fail");
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    int32_t length = sizeof(CallEventInfo);
    dataParcel.WriteInt32(length);
    dataParcel.WriteRawData((const void *)&info, length);
    if (Remote() == nullptr) {
        TELEPHONY_LOGE("function Remote() return nullptr!");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    int32_t error = Remote()->SendRequest(UPDATE_CALL_EVENT, dataParcel, replyParcel, option);
    if (error != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("update call event failed, error: %{public}d", error);
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    return replyParcel.ReadInt32();
}

int32_t CallAbilityCallbackProxy::OnCallDisconnectedCause(DisconnectedDetails cause)
{
    MessageParcel dataParcel;
    MessageParcel replyParcel;
    MessageOption option;
    if (!dataParcel.WriteInterfaceToken(CallAbilityCallbackProxy::GetDescriptor())) {
        TELEPHONY_LOGE("write descriptor fail");
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    dataParcel.WriteInt32(static_cast<int32_t>(cause));
    if (Remote() == nullptr) {
        TELEPHONY_LOGE("function Remote() return nullptr!");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    int32_t error = Remote()->SendRequest(UPDATE_CALL_DISCONNECTED_CAUSE, dataParcel, replyParcel, option);
    if (error != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("report call disconnected cause failed, error: %{public}d", error);
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    return replyParcel.ReadInt32();
}

int32_t CallAbilityCallbackProxy::OnReportAsyncResults(CallResultReportId reportId, AppExecFwk::PacMap &resultInfo)
{
    MessageParcel dataParcel;
    MessageParcel replyParcel;
    MessageOption option;
    if (!dataParcel.WriteInterfaceToken(CallAbilityCallbackProxy::GetDescriptor())) {
        TELEPHONY_LOGE("write descriptor fail");
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    PackDataParcel(reportId, resultInfo, dataParcel);
    if (Remote() == nullptr) {
        TELEPHONY_LOGE("function Remote() return nullptr!");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    int32_t error = Remote()->SendRequest(UPDATE_CALL_ASYNC_RESULT_REQUEST, dataParcel, replyParcel, option);
    if (error != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("report async results failed, error: %{public}d", error);
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    return replyParcel.ReadInt32();
}

int32_t CallAbilityCallbackProxy::OnReportMmiCodeResult(const MmiCodeInfo &info)
{
    MessageParcel dataParcel;
    MessageParcel replyParcel;
    MessageOption option;
    if (!dataParcel.WriteInterfaceToken(CallAbilityCallbackProxy::GetDescriptor())) {
        TELEPHONY_LOGE("write descriptor fail");
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    int32_t length = sizeof(MmiCodeInfo);
    dataParcel.WriteInt32(length);
    dataParcel.WriteRawData((const void *)&info, length);
    if (Remote() == nullptr) {
        TELEPHONY_LOGE("function Remote() return nullptr!");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    int32_t error = Remote()->SendRequest(UPDATE_MMI_CODE_RESULT_REQUEST, dataParcel, replyParcel, option);
    if (error != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("report async results failed, error: %{public}d", error);
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    return replyParcel.ReadInt32();
}

void CallAbilityCallbackProxy::PackDataParcel(
    CallResultReportId reportId, AppExecFwk::PacMap &resultInfo, MessageParcel &dataParcel)
{
    dataParcel.WriteInt32(static_cast<int32_t>(reportId));
    dataParcel.WriteInt32(resultInfo.GetIntValue("result"));
    switch (reportId) {
        case CallResultReportId::GET_CALL_WAITING_REPORT_ID:
        case CallResultReportId::GET_CALL_RESTRICTION_REPORT_ID:
            dataParcel.WriteInt32(resultInfo.GetIntValue("status"));
            dataParcel.WriteInt32(resultInfo.GetIntValue("classCw"));
            break;
        case CallResultReportId::GET_CALL_TRANSFER_REPORT_ID:
            dataParcel.WriteInt32(resultInfo.GetIntValue("status"));
            dataParcel.WriteInt32(resultInfo.GetIntValue("classx"));
            dataParcel.WriteString(resultInfo.GetStringValue("number"));
            dataParcel.WriteInt32(resultInfo.GetIntValue("type"));
            break;
        case CallResultReportId::GET_CALL_CLIP_ID:
            dataParcel.WriteInt32(resultInfo.GetIntValue("action"));
            dataParcel.WriteInt32(resultInfo.GetIntValue("clipStat"));
            break;
        case CallResultReportId::GET_CALL_CLIR_ID:
            dataParcel.WriteInt32(resultInfo.GetIntValue("action"));
            dataParcel.WriteInt32(resultInfo.GetIntValue("clirStat"));
            break;
        case CallResultReportId::GET_CALL_VOTLE_REPORT_ID:
            dataParcel.WriteInt32(resultInfo.GetIntValue("active"));
            break;
        case CallResultReportId::GET_IMS_CONFIG_REPORT_ID:
        case CallResultReportId::GET_IMS_FEATURE_VALUE_REPORT_ID:
        case CallResultReportId::GET_LTE_ENHANCE_MODE_REPORT_ID:
            dataParcel.WriteInt32(resultInfo.GetIntValue("value"));
            break;
        case CallResultReportId::START_RTT_REPORT_ID:
            dataParcel.WriteInt32(resultInfo.GetIntValue("active"));
            break;
        case CallResultReportId::STOP_RTT_REPORT_ID:
            dataParcel.WriteInt32(resultInfo.GetIntValue("inactive"));
            break;
        default:
            break;
    }
}

int32_t CallAbilityCallbackProxy::OnOttCallRequest(OttCallRequestId requestId, AppExecFwk::PacMap &info)
{
    MessageParcel dataParcel;
    MessageParcel replyParcel;
    MessageOption option;
    if (!dataParcel.WriteInterfaceToken(CallAbilityCallbackProxy::GetDescriptor())) {
        TELEPHONY_LOGE("write descriptor fail");
        return TELEPHONY_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    dataParcel.WriteInt32(static_cast<int32_t>(requestId));
    dataParcel.WriteString(info.GetStringValue("phoneNumber"));
    dataParcel.WriteString(info.GetStringValue("bundleName"));
    dataParcel.WriteInt32(info.GetIntValue("videoState"));
    switch (requestId) {
        case OttCallRequestId::OTT_REQUEST_INVITE_TO_CONFERENCE:
            if (info.GetIntValue("listCnt") > 0) {
                dataParcel.WriteString(info.GetStringValue("number"));
            }
            break;
        case OttCallRequestId::OTT_REQUEST_UPDATE_CALL_MEDIA_MODE:
            dataParcel.WriteInt32(info.GetIntValue("callMediaMode"));
            break;
        default:
            break;
    }
    if (Remote() == nullptr) {
        TELEPHONY_LOGE("function Remote() return nullptr!");
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    int32_t error = Remote()->SendRequest(REPORT_OTT_CALL_REQUEST, dataParcel, replyParcel, option);
    if (error != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("report ott call request failed, error: %{public}d", error);
        return TELEPHONY_ERR_IPC_CONNECT_STUB_FAIL;
    }
    return replyParcel.ReadInt32();
}
} // namespace Telephony
} // namespace OHOS
