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
#define VOICE_PROXY_RTT_WAKE_UP_READ _IO('P',  0x1)

#define PROXY_RTT_RX_MIN_SIZE sizeof(VoiceProxyRttRxInd)
#define PROXY_RTT_RX_MAX_SIZE (PROXY_RTT_RX_MIN_SIZE + MAX_RTT_DATA_LEN)
#define PROXY_RTT_TX_MIN_SIZE sizeof(ProxyVoiceRttTxNtf)
#define PROXY_RTT_TX_MAX_SIZE (PROXY_RTT_TX_MIN_SIZE + MAX_RTT_DATA_LEN)

enum VoiceProxyMsgId {
    ID_PROXY_VOICE_RTT_TX_NTF = 0xDFD1,
    ID_VOICE_PROXY_RTT_RX_IND = 0xDFD2,
};

struct ProxyVoiceRttTxNtf{
    uint16_t msgId;
    uint16_t channelId;
    uint32_t dataLen;
    uint8_t  data[0];
};

struct VoiceProxyRttRxInd{
    uint16_t msgId;
    uint16_t channelId;
    uint32_t dataLen;
    uint8_t  data[0];
};

struct RttDataStream{
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
