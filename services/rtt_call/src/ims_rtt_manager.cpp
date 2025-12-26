/*
 * Copyright (C) 2025 Huawei Device Co., Ltd.
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

#include "ims_rtt_manager.h"

#include <unistd.h>
#include <cerrno>
#include <fcntl.h>

#include "telephony_log_wrapper.h"
#include "call_manager_base.h"
#include "ims_rtt_errcode.h"
#include "call_ability_report_proxy.h"

namespace OHOS {
namespace Telephony {
constexpr const int32_t PROXY_RTT_RX_MIN_SIZE = sizeof(VoiceProxyRttRxInd);
constexpr const int32_t PROXY_RTT_RX_MAX_SIZE = PROXY_RTT_RX_MIN_SIZE + MAX_RTT_DATA_LEN;
constexpr const int32_t PROXY_RTT_TX_MIN_SIZE = sizeof(ProxyVoiceRttTxNtf);
constexpr const int32_t PROXY_RTT_TX_MAX_SIZE = PROXY_RTT_TX_MIN_SIZE + MAX_RTT_DATA_LEN;

ImsRttManager::ImsRttManager(const int32_t callId, const uint16_t channelId)
    : callId_(callId), channelId_(channelId), sendThread_(nullptr), recvThread_(nullptr) {}

ImsRttManager::~ImsRttManager()
{
    DestroyRtt();
}

int32_t ImsRttManager::InitRtt()
{
    TELEPHONY_LOGI("start to open proxy");
    if (devFd_ > 0) {
        TELEPHONY_LOGI("proxy already opened");
        return RTT_SUCCESS;
    }

    devFd_ = open(PROXY_RTT_DEV, O_RDWR);
    TELEPHONY_LOGI("devFd_: %{public}d", devFd_.load());
    if (devFd_ <= 0) {
        TELEPHONY_LOGE("open proxy failed, reason:%{public}d", errno);
        return RTT_ERR_FAIL;
    }

    return CreateRttThread();
}

int32_t ImsRttManager::CloseProxy()
{
    TELEPHONY_LOGI("close proxy devFd_:%{public}d", devFd_.load());
    if (devFd_ <= 0) {
        TELEPHONY_LOGI("proxy already closed");
        return RTT_SUCCESS;
    }
    WakeUpKernelRead();
    close(devFd_);
    devFd_ = PROXY_IS_OFF;
    sendStream_ = "";
    TELEPHONY_LOGI("close proxy success.");
    return RTT_SUCCESS;
}

void ImsRttManager::WakeUpKernelRead()
{
    TELEPHONY_LOGI("wake up kernel read");
    int arg = 0;
    int retCode = ioctl(devFd_, VOICE_PROXY_RTT_WAKE_UP_READ, &arg);
    if (retCode < 0) {
        TELEPHONY_LOGE(
            "call cmd VOICE_PROXY_RTT_WAKE_UP_READ failed: arg:%{public}d, ret:%{public}d", arg, retCode);
    }
}

int32_t ImsRttManager::CreateRttThread()
{
    int32_t retCode = CreateSendThread();
    if (retCode != RTT_SUCCESS) {
        TELEPHONY_LOGE("create rtt send thread failed!");
        DestroyRtt();
        return RTT_ERR_FAIL;
    }

    retCode = CreateRecvThread();
    if (retCode != RTT_SUCCESS) {
        TELEPHONY_LOGE("create rtt recv thread failed!");
        DestroyRtt();
        return RTT_ERR_FAIL;
    }

    return RTT_SUCCESS;
}

int32_t ImsRttManager::CreateSendThread()
{
    TELEPHONY_LOGI("create rtt send thread.");
    sendThreadActive_ = true;
    sendThread_ = std::make_unique<ffrt::thread>([this] { this->SendThreadLoop();});
    if (!sendThread_->joinable()) {
        TELEPHONY_LOGI("sendThread_ cannot joinable create rtt send thread failed!");
        return RTT_ERR_FAIL;
    }
    return RTT_SUCCESS;
}

void ImsRttManager::SendThreadLoop()
{
    std::string messageToSend = "";

    while (sendThreadActive_) {
        std::unique_lock<ffrt::mutex> lock(sendMtx_);
        sendCond_.wait_for(lock, std::chrono::milliseconds(SEND_WAIT_TIME_MS), [this]() {
            return !sendStream_.empty() || !sendThreadActive_;
        });
        if (!sendThreadActive_) {
            break;
        }
        // each RTT message should not exceed 500 characters
        messageToSend = "";
        ReadStreamData(MAX_SEND_MSG_LEN, messageToSend);
        if (messageToSend.empty()) {
            TELEPHONY_LOGI("messageToSend is empty");
            continue;
        }
        lock.unlock();

        // use the ProxyVoiceRttTxNtf struct to assembe and send the RTT message
        int32_t retLength = SendDataToProxy(messageToSend);
        if (retLength < 0) {
            TELEPHONY_LOGI("send rtt data to proxy failed");
        }
    }

    TELEPHONY_LOGI("rtt send thread loop exit...");
}

void ImsRttManager::ReadStreamData(const int32_t msgSendLength, std::string &sendMessage)
{
    int32_t numToSend = std::min(static_cast<int32_t>(sendStream_.length()), msgSendLength);
    if (numToSend == 0) {
        return;
    }

    sendMessage = sendStream_.substr(0, numToSend);
    sendStream_.erase(0, numToSend);
}

int32_t ImsRttManager::SendDataToProxy(const std::string &sendMessage)
{
    if (devFd_ == PROXY_IS_OFF) {
        TELEPHONY_LOGE("SendDataToProxy failed devFd_: %{public}d", devFd_.load());
        return RTT_ERR_PROXY_CLOSED;
    }

    uint8_t totalData[PROXY_RTT_TX_MAX_SIZE] = {0};
    ProxyVoiceRttTxNtf *txNtf = (ProxyVoiceRttTxNtf *)totalData;
    txNtf->channelId = channelId_;
    txNtf->dataLen = sendMessage.length();
    txNtf->msgId = ID_PROXY_VOICE_RTT_TX_NTF;
    if (memcpy_s(txNtf->data, MAX_RTT_DATA_LEN, sendMessage.c_str(), sendMessage.length()) != EOK) {
        TELEPHONY_LOGE("SendDataToProxy memcpy_s fail, error length: %{public}d",
            static_cast<int32_t>(sendMessage.length()));
        return RTT_ERR_FAIL;
    }
    int32_t retLength = write(devFd_, txNtf, PROXY_RTT_TX_MIN_SIZE + sendMessage.length());
    if (retLength >= 0) {
        return retLength;
    }

    TELEPHONY_LOGE("write failed, reason: %{public}d", errno);
    if ((errno != EAGAIN) && (errno != EBUSY)) {
        return RTT_ERR_FAIL;
    }

    return RTT_SUCCESS;
}

int32_t ImsRttManager::CreateRecvThread()
{
    TELEPHONY_LOGI("create rtt recv thread.");
    recvThreadActive_ = true;
    recvThread_ = std::make_unique<ffrt::thread>([this] { this->RecvThreadLoop();});
    if (!recvThread_->joinable()) {
        TELEPHONY_LOGE("recvThread_ cannot joinable create rtt recv thread failed!");
        return RTT_ERR_FAIL;
    }
    return RTT_SUCCESS;
}

void ImsRttManager::RecvThreadLoop()
{
    std::string recvMessage;

    while (recvThreadActive_) {
        recvMessage = "";
        RecvDataFromProxy(recvMessage);
        if (recvMessage.length() > 0) {
            ReportRecvMessage(recvMessage);
        }
    }

    TELEPHONY_LOGI("recv thread loop exit..");
}

void ImsRttManager::ReportRecvMessage(const std::string &recvMessage)
{
    std::string currRecvMessage = recvMessage;
    AppExecFwk::PacMap resultInfo;
    resultInfo.PutIntValue("callId", callId_);
    resultInfo.PutStringValue("rttMessage", currRecvMessage);
    DelayedSingleton<CallAbilityReportProxy>::GetInstance()->ReportRttCallMessage(resultInfo);
}

void ImsRttManager::RecvDataFromProxy(std::string &recvMessage)
{
    if (devFd_ == PROXY_IS_OFF) {
        TELEPHONY_LOGE("RecvDataFromProxy: proxy already closed.");
        return;
    }

    uint8_t data[PROXY_RTT_RX_MAX_SIZE] = {0};
    VoiceProxyRttRxInd *rxInd = (VoiceProxyRttRxInd *)data;

    int32_t readSize = read(devFd_, rxInd, PROXY_RTT_RX_MAX_SIZE);
    if (readSize <= PROXY_RTT_RX_MIN_SIZE || readSize > PROXY_RTT_RX_MAX_SIZE) {
        TELEPHONY_LOGE("readSize is error %{public}d", readSize);
        return;
    }
    if (rxInd->dataLen == 0 || rxInd->dataLen > MAX_RTT_DATA_LEN) {
        TELEPHONY_LOGE("readSize error: datalen is invalidate");
        return;
    }
    if (readSize != static_cast<int32_t>(rxInd->dataLen + PROXY_RTT_RX_MIN_SIZE)) {
        TELEPHONY_LOGE("readSize error: readSize is not equeal message len");
        return;
    }
    recvMessage = RttDataStreamToString(rxInd->data, rxInd->dataLen);
}

void ImsRttManager::DestroyRtt()
{
    std::lock_guard<ffrt::mutex> lock(destroyRttThreadMtx_);
    recvThreadActive_ = false;
    sendThreadActive_ = false;
    CloseProxy();

    {
        std::lock_guard<ffrt::mutex> lock(sendMtx_);
        sendCond_.notify_one();
    }

    if (sendThread_ != nullptr && sendThread_->joinable()) {
        sendThread_->join();
        sendThread_.reset();
    }
    if (recvThread_ != nullptr && recvThread_->joinable()) {
        recvThread_->join();
        recvThread_.reset();
    }
    TELEPHONY_LOGI("destroy rtt send and recv thread success.");
}

int32_t ImsRttManager::SendRttMessage(const std::string &rttMessage)
{
    std::lock_guard<ffrt::mutex> lock(sendMtx_);
    sendStream_.append(rttMessage);
    sendCond_.notify_one();
    return RTT_SUCCESS;
}

std::string ImsRttManager::RttDataStreamToString(const uint8_t* rttStreamData, int32_t dataLen)
{
    std::vector<uint8_t> output;
    int32_t i = 0;
    while (i < dataLen) {
        if (ProcEscapeSeq(rttStreamData, dataLen, i, output)) {
            i += STEP_THREE;
            continue;
        }
        if (ProcBellSeq(rttStreamData, i)) {
            i += STEP_ONE;
            continue;
        }
        if (ProcControlSeq(rttStreamData, dataLen, i)) {
            i += STEP_TWO;
            continue;
        }
        if (ProcOtherControlBytes(rttStreamData, dataLen, i)) {
            i += STEP_TWO;
            continue;
        }
        if (ProcessOrderMark(rttStreamData, dataLen, i)) {
            i += STEP_THREE;
            continue;
        }
        output.push_back(rttStreamData[i]);
        i++;
    }
    return std::string(output.begin(), output.end());
}

bool ImsRttManager::ProcEscapeSeq(
    const uint8_t* input, int32_t dataLen, int32_t index, std::vector<uint8_t>& output)
{
    if (index + STEP_TWO < dataLen &&
        (input[index] == 0xE2) && (input[index + STEP_ONE] == 0x80) && (input[index + STEP_TWO] == 0xA8)) {
        output.push_back(0x0D);
        output.push_back(0x0A);
        return true;
    }
    return false;
}

bool ImsRttManager::ProcBellSeq(const uint8_t* input, int32_t index)
{
    if (input[index] == 0x07) {
        return true;
    }
    return false;
}

bool ImsRttManager::ProcControlSeq(const uint8_t* input, int32_t dataLen, int32_t index)
{
    if (index + STEP_ONE < dataLen && (input[index] == 0x1B) && (input[index + STEP_ONE] == 0x61)) {
        return true;
    }
    return false;
}

bool ImsRttManager::ProcOtherControlBytes(const uint8_t* input, int32_t dataLen, int32_t index)
{
    if (index + STEP_ONE < dataLen &&
        ((input[index] == 0xC2 && input[index + STEP_ONE] == 0x98) ||
        (input[index] == 0xC2 && input[index + STEP_ONE] == 0x9C))) {
        return true;
    }
    return false;
}

bool ImsRttManager::ProcessOrderMark(const uint8_t* input, int32_t dataLen, int32_t index)
{
    if (index + STEP_TWO < dataLen && (input[index] == 0xEF) &&
        (input[index + STEP_ONE] == 0xBB) && (input[index + STEP_TWO] == 0xBF)) {
        return true;
    }
    return false;
}

void ImsRttManager::SetChannelID(int32_t channelId)
{
    channelId_ = channelId;
}

void ImsRttManager::SetCallID(int32_t callId)
{
    callId_ = callId;
}
}
}
