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
#include <stdint.h>
#include <vector>
#include <cstdint>
#include <string.h>
#include "ffrt.h"

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

struct RttDataStream {
    uint16_t channelId;
    uint32_t length;
    std::vector<uint8_t> data;
};

class ImsRttManager {
public:
    ImsRttManager(const int32_t callId, const uint16_t channelId);
    ~ImsRttManager();
    int32_t InitRtt();
    int32_t SendRttMessage(const std::string &rttMessage);
    static void DestroyRtt();
    void SetChannelID(int32_t channelId);
    void SetCallID(int32_t callId);

private:
    int32_t CreateRttThread();
    int32_t CreateSendThread();
    int32_t CreateRecvThread();
    static int32_t CloseProxy();
    static void *RecvThreadLoop(void *arg);
    static void *SendThreadLoop(void *arg);
    static void ReadStreamData(const int32_t len, std::string &sendMessage);
    static int32_t SendDataToProxy(const std::string &sendMessage);
    static void RecvDataFromProxy(std::string &recvMessage);
    static void ReportRecvMessage(const std::string &recvMessage);
    static void WakeUpKernelRead();
    static std::string RttDataStreamToString(const uint8_t* rttStreamData, int32_t dataLen);
    static bool ProcEscapeSeq(const uint8_t* input, int32_t dataLen, int32_t index, std::vector<uint8_t>& output);
    static bool ProcBellSeq(const uint8_t* input, int32_t index);
    static bool ProcControlSeq(const uint8_t* input, int32_t dataLen, int32_t index);
    static bool ProcOtherControlBytes(const uint8_t* input, int32_t dataLen, int32_t index);
    static bool ProcessOrderMark(const uint8_t* input, int32_t dataLen, int32_t index);

    static uint16_t channelId_;
    static int32_t callId_;
    static ffrt::mutex destroyRttThreadMtx_;
    static int32_t devFd_;
    static bool writeThreadActive_;
    static bool readThreadActive_;
    static std::string sendStream_;
};
} // namespace Telephony
} // namespace OHOS
#endif // IMS_RTT_MANAGER_H
