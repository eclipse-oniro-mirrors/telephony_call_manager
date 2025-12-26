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

#ifndef IMS_RTT_MANAGER_H
#define IMS_RTT_MANAGER_H

#include <cstdint>
#include <string.h>
#include <vector>
#include <atomic>
#include <sys/ioctl.h>

#include "ffrt_inner.h"

namespace OHOS {
namespace Telephony {
constexpr const unsigned int VOICE_PROXY_RTT_WAKE_UP_READ = _IO('P', 0x1);
constexpr const int32_t PROXY_IS_OFF = -1;
constexpr const char* PROXY_RTT_DEV = "/dev/voice_proxy_rtt";
constexpr const int32_t MAX_RTT_DATA_LEN = 500;
constexpr const int32_t MAX_SEND_MSG_LEN = 30;
constexpr const int8_t STEP_ONE = 1;
constexpr const int8_t STEP_TWO = 2;
constexpr const int8_t STEP_THREE = 3;
constexpr size_t SEND_WAIT_TIME_MS = 3000;

enum VoiceProxyMsgId {
    ID_PROXY_VOICE_RTT_TX_NTF = 0xDFD1,
    ID_VOICE_PROXY_RTT_RX_IND = 0xDFD2,
};

struct ProxyVoiceRttTxNtf {
    uint16_t msgId;
    uint16_t channelId;
    uint32_t dataLen;
    uint8_t  data[0];
};

struct VoiceProxyRttRxInd {
    uint16_t msgId;
    uint16_t channelId;
    uint32_t dataLen;
    uint8_t  data[0];
};

class ImsRttManager {
public:
    ImsRttManager(const int32_t callId, const uint16_t channelId);
    ~ImsRttManager();
    int32_t InitRtt();
    int32_t SendRttMessage(const std::string &rttMessage);
    void DestroyRtt();
    void SetChannelID(int32_t channelId);
    void SetCallID(int32_t callId);

private:
    int32_t CreateRttThread();
    int32_t CreateSendThread();
    int32_t CreateRecvThread();
    int32_t CloseProxy();
    void RecvThreadLoop();
    void SendThreadLoop();
    void ReadStreamData(const int32_t len, std::string &sendMessage);
    int32_t SendDataToProxy(const std::string &sendMessage);
    void RecvDataFromProxy(std::string &recvMessage);
    void ReportRecvMessage(const std::string &recvMessage);
    void WakeUpKernelRead();
    std::string RttDataStreamToString(const uint8_t* rttStreamData, int32_t dataLen);
    bool ProcEscapeSeq(const uint8_t* input, int32_t dataLen, int32_t index, std::vector<uint8_t>& output);
    bool ProcBellSeq(const uint8_t* input, int32_t index);
    bool ProcControlSeq(const uint8_t* input, int32_t dataLen, int32_t index);
    bool ProcOtherControlBytes(const uint8_t* input, int32_t dataLen, int32_t index);
    bool ProcessOrderMark(const uint8_t* input, int32_t dataLen, int32_t index);

    std::atomic<int32_t> devFd_{-1};
    std::atomic<bool> sendThreadActive_{false};
    std::atomic<bool> recvThreadActive_{false};
    std::string sendStream_{""};
    std::atomic<int32_t> callId_{-1};
    std::atomic<uint16_t> channelId_{0};
    std::unique_ptr<ffrt::thread> sendThread_;
    std::unique_ptr<ffrt::thread> recvThread_;
    ffrt::mutex destroyRttThreadMtx_;
    ffrt::mutex sendMtx_;
    ffrt::condition_variable sendCond_{};
};
} // namespace Telephony
} // namespace OHOS
#endif // IMS_RTT_MANAGER_H
