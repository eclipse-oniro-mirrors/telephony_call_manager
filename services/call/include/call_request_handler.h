/*
 * Copyright (C) 2021-2024 Huawei Device Co., Ltd.
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

#ifndef CALL_REQUEST_HANDLER_H
#define CALL_REQUEST_HANDLER_H

#include <map>
#include <memory>
#include "ffrt.h"

#include "call_request_process.h"
#include "common_type.h"
#include "event_handler.h"
#include "event_runner.h"

namespace OHOS {
namespace Telephony {
struct AnswerCallPara {
    int32_t callId = 0;
    int32_t videoState = 0;
};

struct RejectCallPara {
    int32_t callId = 0;
    bool isSendSms = 0;
    char content[REJECT_CALL_MSG_MAX_LEN + 1] = { 0 };
};

struct StartRttPara {
    int32_t callId = 0;
    std::u16string msg = u"";
};

struct CallMediaUpdatePara {
    int32_t callId = 0;
    ImsCallMode mode = ImsCallMode::CALL_MODE_AUDIO_ONLY;
};

struct JoinConferencePara {
    int32_t callId = 0;
    std::vector<std::string> numberList {};
};

/**
 * @ClassName: CallRequestHandler
 * @Description: inner event-handle mechanism on harmony platform, used by callcontrolmanager
 * to handle downflowed telephone business, factually act as work thread.
 */
class CallRequestHandler {
public:
    CallRequestHandler();
    ~CallRequestHandler();
    void Init();
    int32_t DialCall();
    int32_t AnswerCall(int32_t callId, int32_t videoState);
    int32_t RejectCall(int32_t callId, bool isSendSms, std::string &content);
    int32_t HangUpCall(int32_t callId);
    int32_t HoldCall(int32_t callId);
    int32_t UnHoldCall(int32_t callId);
    int32_t SwitchCall(int32_t callId);
    int32_t CombineConference(int32_t mainCallId);
    int32_t SeparateConference(int32_t callId);
    int32_t KickOutFromConference(int32_t callId);
    int32_t StartRtt(int32_t callId, std::u16string &msg);
    int32_t StopRtt(int32_t callId);
    int32_t JoinConference(int32_t callId, std::vector<std::string> &numberList);

private:
    std::shared_ptr<CallRequestProcess> callRequestProcessPtr_;
};
} // namespace Telephony
} // namespace OHOS
#endif // CALL_REQUEST_HANDLER_H
