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

#include "call_manager_callback_test.h"

namespace OHOS {
namespace Telephony {
constexpr int16_t RETURN_SUCCESS = 0;

int32_t CallManagerCallbackTest::OnCallDetailsChange(const CallAttributeInfo &info)
{
    std::cout << "----------OnCallDetailsChange--------" << std::endl
              << "callId:" << info.callId << std::endl
              << "callType:" << (int32_t)info.callType << std::endl
              << "callState:" << (int32_t)info.callState << std::endl
              << "conferenceState:" << (int32_t)info.conferenceState << std::endl
              << "accountNumber:" << info.accountNumber << std::endl;
    return RETURN_SUCCESS;
}

int32_t CallManagerCallbackTest::OnMeeTimeDetailsChange(const CallAttributeInfo &info)
{
    std::cout << "----------OnMeeTimeDetailsChange--------" << std::endl
              << "callId:" << info.callId << std::endl
              << "callType:" << (int32_t)info.callType << std::endl
              << "callState:" << (int32_t)info.callState << std::endl
              << "conferenceState:" << (int32_t)info.conferenceState << std::endl
              << "accountNumber:" << info.accountNumber << std::endl;
    return RETURN_SUCCESS;
}

int32_t CallManagerCallbackTest::OnCallEventChange(const CallEventInfo &info)
{
    std::cout << "----------OnCallEventChange--------" << std::endl
              << "eventId:" << (int32_t)info.eventId << std::endl;
    return RETURN_SUCCESS;
}

int32_t CallManagerCallbackTest::OnCallDisconnectedCause(const DisconnectedDetails &details)
{
    std::cout << "----------OnCallDisconnectedCause--------" << std::endl
              << "cause:" << (int32_t)(details.reason) << std::endl;
    return RETURN_SUCCESS;
}

int32_t CallManagerCallbackTest::OnReportAsyncResults(CallResultReportId reportId, AppExecFwk::PacMap &resultInfo)
{
    std::cout << "----------OnReportAsyncResults--------" << std::endl
              << "reportId:" << (int32_t)reportId << std::endl;
    switch (reportId) {
        case CallResultReportId::GET_CALL_WAITING_REPORT_ID:
            std::cout << "result:" << (int32_t)resultInfo.GetIntValue("result", -1) << std::endl
                      << "status:" << (int32_t)resultInfo.GetIntValue("status", -1) << std::endl
                      << "classCw:" << (int32_t)resultInfo.GetIntValue("classCw", -1) << std::endl;
            break;
        case CallResultReportId::SET_CALL_WAITING_REPORT_ID:
            std::cout << "result:" << (int32_t)resultInfo.GetIntValue("result", -1) << std::endl;
            break;
        case CallResultReportId::GET_CALL_RESTRICTION_REPORT_ID:
            std::cout << "result:" << (int32_t)resultInfo.GetIntValue("result", -1) << std::endl
                      << "status:" << (int32_t)resultInfo.GetIntValue("status", -1) << std::endl
                      << "classCw:" << (int32_t)resultInfo.GetIntValue("classCw", -1) << std::endl;
            break;
        case CallResultReportId::SET_CALL_RESTRICTION_REPORT_ID:
            std::cout << "result:" << (int32_t)resultInfo.GetIntValue("result", -1) << std::endl;
            break;
        case CallResultReportId::SET_CALL_RESTRICTION_PWD_REPORT_ID:
            std::cout << "result:" << (int32_t)resultInfo.GetIntValue("result", -1) << std::endl;
            break;
        case CallResultReportId::GET_CALL_TRANSFER_REPORT_ID:
            std::cout << "result:" << (int32_t)resultInfo.GetIntValue("result", -1) << std::endl
                      << "status:" << (int32_t)resultInfo.GetIntValue("status", -1) << std::endl
                      << "classx:" << (int32_t)resultInfo.GetIntValue("classx", -1) << std::endl
                      << "number:" << resultInfo.GetStringValue("number", "") << std::endl
                      << "type:" << (int32_t)resultInfo.GetIntValue("type", -1) << std::endl
                      << "reason:" << (int32_t)resultInfo.GetIntValue("reason", -1) << std::endl
                      << "time:" << (int32_t)resultInfo.GetIntValue("time", -1) << std::endl
                      << "startHour:" << (int32_t)resultInfo.GetIntValue("startHour", -1) << std::endl
                      << "startMinute:" << (int32_t)resultInfo.GetIntValue("startMinute", -1) << std::endl
                      << "endHour:" << (int32_t)resultInfo.GetIntValue("endHour", -1) << std::endl
                      << "endMinute:" << (int32_t)resultInfo.GetIntValue("endMinute", -1) << std::endl;
            break;
        case CallResultReportId::SET_CALL_TRANSFER_REPORT_ID:
            std::cout << "result:" << (int32_t)resultInfo.GetIntValue("result", -1) << std::endl;
            break;
        default:
            break;
    }
    return RETURN_SUCCESS;
}

int32_t CallManagerCallbackTest::OnOttCallRequest(OttCallRequestId requestId, AppExecFwk::PacMap &info)
{
    std::cout << "----------OnOttCallRequest--------" << std::endl
              << "requestId:" << (int32_t)requestId << std::endl;
    return RETURN_SUCCESS;
}

int32_t CallManagerCallbackTest::OnReportMmiCodeResult(const MmiCodeInfo &info)
{
    std::cout << "----------OnReportMmiCodeResult--------" << std::endl
              << "result:" << info.result << std::endl
              << "message:" << info.message << std::endl;
    return RETURN_SUCCESS;
}

int32_t CallManagerCallbackTest::OnReportAudioDeviceChange(const AudioDeviceInfo &info)
{
    std::cout << "----------OnReportAudioDeviceChange--------" << std::endl
              << "currentAudioDeviceType:" << static_cast<int32_t>(info.currentAudioDevice.deviceType) << " "
              << "address:" << info.currentAudioDevice.address << std::endl
              << "isMute:" << info.isMuted << std::endl;
    for (size_t i = 0; i < info.audioDeviceList.size(); i++) {
        std::cout << "audioDeviceList[" << i
                  << "] deviceType:" << static_cast<int32_t>(info.audioDeviceList[i].deviceType) << " "
                  << "audioDeviceList[" << i << "] address:" << info.audioDeviceList[i].address << std::endl;
    }
    return RETURN_SUCCESS;
}

int32_t CallManagerCallbackTest::OnReportPostDialDelay(const std::string &str)
{
    std::cout << "----------OnReportPostDialDelay--------" << std::endl << "str:" << str << std::endl;
    return RETURN_SUCCESS;
}

int32_t CallManagerCallbackTest::OnUpdateImsCallModeChange(const CallMediaModeInfo &imsCallModeInfo)
{
    std::cout << "----------OnUpdateImsCallModeChange--------" << std::endl
              << "callId:" << imsCallModeInfo.callId << std::endl
              << "isRequestInfo:" << imsCallModeInfo.isRequestInfo << std::endl
              << "result:" << (int32_t)imsCallModeInfo.result << std::endl
              << "callMode:" << (int32_t)imsCallModeInfo.callMode << std::endl;
    return RETURN_SUCCESS;
}

int32_t CallManagerCallbackTest::OnCallSessionEventChange(const CallSessionEvent &callSessionEventOptions)
{
    std::cout << "----------OnCallSessionEventChange--------" << std::endl
              << "callId:" << callSessionEventOptions.callId << std::endl
              << "eventId:" << (int32_t)callSessionEventOptions.eventId << std::endl;
    return RETURN_SUCCESS;
}

int32_t CallManagerCallbackTest::OnPeerDimensionsChange(const PeerDimensionsDetail &peerDimensionsDetail)
{
    std::cout << "----------OnPeerDimensionsChange--------" << std::endl
              << "callId:" << peerDimensionsDetail.callId << std::endl
              << "width:" << peerDimensionsDetail.width << std::endl
              << "height:" << peerDimensionsDetail.height << std::endl;
    return RETURN_SUCCESS;
}

int32_t CallManagerCallbackTest::OnCallDataUsageChange(const int64_t dataUsage)
{
    std::cout << "----------OnCallDataUsageChange--------" << std::endl
              << "dataUsage:" << dataUsage << std::endl;
    return RETURN_SUCCESS;
}

int32_t CallManagerCallbackTest::OnUpdateCameraCapabilities(const CameraCapabilities &cameraCapabilities)
{
    std::cout << "----------OnUpdateCameraCapabilities--------" << std::endl
              << "callId:" << cameraCapabilities.callId << std::endl
              << "width:" << cameraCapabilities.width << std::endl
              << "height:" << cameraCapabilities.height << std::endl;
    return RETURN_SUCCESS;
}
} // namespace Telephony
} // namespace OHOS
