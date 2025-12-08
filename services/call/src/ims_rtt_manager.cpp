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
#include <unistd.h>
#include <sys/ioctl.h>
#include <cerrno>
#include <cstdlib>
#include <fcntl.h>
#include <pthread.h>

#include "ipc_types.h"
#include "telephony_log_wrapper.h"
#include "call_manager_base.h"
#include "call_ability_report_proxy.h"
#include "ims_rtt_constants.h"
#include "ims_rtt_errcode.h"
#include "ims_rtt_manager.h"

namespace OHOS {
namespace Telephony {
int32_t ImsRttManager::devFd_ = PROXY_IS_OFF;
bool ImsRttManager::readThreadActive_ = false;
bool ImsRttManager::writeThreadActive_ = false;
uint16_t ImsRttManager::channelId_ = -1;
int32_t ImsRttManager::callId_ = -1;
std::string ImsRttManager::sendStream_ = "";
ffrt::mutex ImsRttManager::destroyRttThreadMtx_;

ImsRttManager::ImsRttManager(const int32_t callId, const uint16_t channelId) {
    callId_ = callId;
    channelId_ = channelId;
}

ImsRttManager::~ImsRttManager() {}

int32_t ImsRttManager::InitRtt()
{
    TELEPHONY_LOGI("start to open proxy");
    if (devFd_ > 0) {
        TELEPHONY_LOGI("proxy already opened");
        return RTT_SUCCESS;
    }

    devFd_ = open(PROXY_RTT_DEV, O_RDWR);
    TELEPHONY_LOGI("devFd_: %{public}d", devFd_);
    if (devFd_ <= 0) {
        TELEPHONY_LOGE("open proxy failed, reason:%{public}d", errno);
        return RTT_ERR_FAIL;
    }
    
    return CreateRttThread();
}

int32_t ImsRttManager::CloseProxy()
{
    TELEPHONY_LOGI("close proxy devFd_:%{public}d", devFd_);
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
    int ret = ioctl(devFd_, VOICE_PROXY_RTT_WAKE_UP_READ, &arg);
    if (ret < 0) {
        TELEPHONY_LOGE("call cmd VOICE_PROXY_RTT_WAKE_UP_READ failed: arg-%{public}d, ret-%{public}d", arg, ret);
    }
}

int32_t ImsRttManager::CreateRttThread()
{
    TELEPHONY_LOGI("create rtt thread.");
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
    int32_t retCode;
    pthread_t readThread;
    pthread_attr_t attr;
    readThreadActive_ = true;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
    retCode = pthread_create(&readThread, &attr, SendThreadLoop, NULL);
    pthread_attr_destroy(&attr);
    if (retCode != 0) {
        TELEPHONY_LOGE("create rtt send thread failed, ret: %{public}d!", retCode);
        return retCode;
    }

    return RTT_SUCCESS;
}

void *ImsRttManager::SendThreadLoop(void *arg)
{
    TELEPHONY_LOGI("rtt send thread loop start...");    
    std::string messageToSend = "";

    while (readThreadActive_) {
        // each RTT message should not exceed 500 characters
        messageToSend = "";
        ReadStreamData(MAX_SEND_MSG_LEN, messageToSend);
        if (messageToSend.size() > 0) {
            // use the ProxyVoiceRttTxNtf struct to assembe and send the RTT message
            int32_t retLength = SendDataToProxy(messageToSend);
            if (retLength < 0) {
                TELEPHONY_LOGI("send rtt data to proxy failed, exist send thread.");
            }
        } else {
            TELEPHONY_LOGE("get data from sink is 0");
        }
    }

    TELEPHONY_LOGI("rtt send thread loop exit...");
    std::lock_guard<ffrt::mutex> lock(destroyRttThreadMtx_);
    DestroyRtt();

    return NULL;
}

void ImsRttManager::ReadStreamData(const int32_t msgSendLength, std::string &sendMessage)
{
    int32_t numToSend = std::min(static_cast<int32_t>(sendStream_.length()), msgSendLength);
    if (numToSend == 0) {
        // delete after debug
        TELEPHONY_LOGI("send message size: %{public}d", numToSend);
        return;
    }

    sendMessage = sendStream_.substr(0, numToSend);
    sendStream_ = sendStream_.substr(numToSend);
}

int32_t ImsRttManager::SendDataToProxy(const std::string &sendMessage)
{
    if (devFd_ == PROXY_IS_OFF) {
        TELEPHONY_LOGE("SendDataToProxy failed devFd_: %{public}d", devFd_);
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
    TELEPHONY_LOGI("retLength: %{public}d", retLength);
    if (retLength >= 0) {
        TELEPHONY_LOGI("send rtt data to proxy, data size: %{public}d", retLength);
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
    int32_t ret;
    pthread_t writeThread;
    pthread_attr_t attr;
    writeThreadActive_ = true;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
    ret = pthread_create(&writeThread, &attr, RecvThreadLoop, NULL);
    pthread_attr_destroy(&attr);
    if (ret) {
        TELEPHONY_LOGE("create rtt recv thread failed, ret: %{public}d!", ret);
        return RTT_ERR_FAIL;
    }

    return RTT_SUCCESS;
}

void *ImsRttManager::RecvThreadLoop(void *arg)
{
    TELEPHONY_LOGI("rtt recv thread loop start...");
    std::string recvMessage;

    while (writeThreadActive_) {
        recvMessage = "";
        RecvDataFromProxy(recvMessage);
        if (recvMessage.length() > 0) {
            ReportRecvMessage(recvMessage);
        }
    }

    TELEPHONY_LOGI("recv thread loop exit..");
    std::lock_guard<ffrt::mutex> lock(destroyRttThreadMtx_);
    DestroyRtt();

    return NULL;
}

void ImsRttManager::ReportRecvMessage(const std::string &recvMessage)
{
    std::string currRecvMessage = recvMessage;
    AppExecFwk::PacMap resultInfo;
    resultInfo.PutIntValue("callId", callId_);
    resultInfo.PutStringValue("rttMessage", currRecvMessage);
    DelayedSingleton<CallAbilityReportProxy>::GetInstance()->ReportRttCallMessage(resultInfo);
    TELEPHONY_LOGI("ReportRecvMessage: callId: %{public}d, channelId: %{public}d",
        callId_, static_cast<int32_t>(channelId_));
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
    if (readSize <= (int32_t)PROXY_RTT_RX_MIN_SIZE || readSize > PROXY_RTT_RX_MAX_SIZE) {
        TELEPHONY_LOGE("readSize is error %{public}d", readSize);
        return;
    }
    if (rxInd->dataLen == 0 || rxInd->dataLen > MAX_RTT_DATA_LEN) {
        TELEPHONY_LOGE("readSize error: datalen is invalidate");
        return;
    }
    if (readSize != rxInd->dataLen + PROXY_RTT_RX_MIN_SIZE) {
        TELEPHONY_LOGE("readSize error: readSize is not equeal message len");
        return;
    }

    recvMessage = reinterpret_cast<char *>(rxInd->data);
    TELEPHONY_LOGI("get rtt data from proxy success, channelId: %{public}d",
        static_cast<int32_t>(rxInd->channelId));
    recvConditionVar_.notify_one();
}

void ImsRttManager::DestroyRtt()
{
    TELEPHONY_LOGI("destroy rtt read and send thread.");
    writeThreadActive_ = false;
    readThreadActive_ = false;

    if (devFd_ != PROXY_IS_OFF) {
        int32_t retCode = CloseProxy();
        if (retCode != RTT_SUCCESS) {
            TELEPHONY_LOGE("close proxy failed.");
            return;
        }
    }

    TELEPHONY_LOGI("destroy rtt send and recv thread success.");
}

int32_t ImsRttManager::SendRttMessage(const std::string &rttMessage)
{
    sendStream_.append(rttMessage);
    TELEPHONY_LOGI("SendRttMessage rttMessage size: %{public}d, remain size: %{public}d",
        static_cast<int32_t>(rttMessage.length()), static_cast<int32_t>(sendStream_.length()));

    return RTT_SUCCESS;
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
