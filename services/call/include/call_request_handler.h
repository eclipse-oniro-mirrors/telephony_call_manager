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

#ifndef CALL_REQUEST_HANDLER_H
#define CALL_REQUEST_HANDLER_H

#include <map>
#include <memory>
#include <mutex>

#include "event_handler.h"
#include "event_runner.h"

#include "common_type.h"
#include "call_request_process.h"

namespace OHOS {
namespace Telephony {
struct AnswerCallPara {
    int32_t callId;
    int32_t videoState;
};

struct RejectCallPara {
    int32_t callId;
    bool isSendSms;
    char content[REJECT_CALL_MSG_MAX_LEN + 1];
};

struct StartRttPara {
    int32_t callId;
    std::u16string msg;
};

struct CallMediaUpdatePara {
    int32_t callId;
    CallMediaMode mode;
};

struct JoinConferencePara {
    int32_t callId;
    std::vector<std::string> numberList;
};

/**
 * @ClassName: CallRequestHandler
 * @Description: inner event-handle mechanism on harmony platform, used by callcontrolmanager
 * to handle downflowed telephone business, factually act as work thread.
 */
class CallRequestHandler : public AppExecFwk::EventHandler {
public:
    CallRequestHandler(const std::shared_ptr<AppExecFwk::EventRunner> &runner);
    virtual ~CallRequestHandler();

    void Init();
    void ProcessEvent(const AppExecFwk::InnerEvent::Pointer &event);

private:
    using CallRequestFunc = void (CallRequestHandler::*)(const AppExecFwk::InnerEvent::Pointer &event);

    void DialCallEvent(const AppExecFwk::InnerEvent::Pointer &event);
    void AcceptCallEvent(const AppExecFwk::InnerEvent::Pointer &event);
    void RejectCallEvent(const AppExecFwk::InnerEvent::Pointer &event);
    void HangUpCallEvent(const AppExecFwk::InnerEvent::Pointer &event);
    void HoldCallEvent(const AppExecFwk::InnerEvent::Pointer &event);
    void UnHoldCallEvent(const AppExecFwk::InnerEvent::Pointer &event);
    void SwitchCallEvent(const AppExecFwk::InnerEvent::Pointer &event);
    void CombineConferenceEvent(const AppExecFwk::InnerEvent::Pointer &event);
    void SeparateConferenceEvent(const AppExecFwk::InnerEvent::Pointer &event);
    void UpdateCallMediaModeEvent(const AppExecFwk::InnerEvent::Pointer &event);
    void StartRttEvent(const AppExecFwk::InnerEvent::Pointer &event);
    void StopRttEvent(const AppExecFwk::InnerEvent::Pointer &event);
    void JoinConferenceEvent(const AppExecFwk::InnerEvent::Pointer &event);
    std::map<uint32_t, CallRequestFunc> memberFuncMap_;
    std::unique_ptr<CallRequestProcess> callRequestProcessPtr_;
};

class CallRequestHandlerService {
public:
    CallRequestHandlerService();
    ~CallRequestHandlerService();
    void Start();
    int32_t DialCall();
    int32_t AnswerCall(int32_t callId, int32_t videoState);
    int32_t RejectCall(int32_t callId, bool isSendSms, std::string &content);
    int32_t HangUpCall(int32_t callId);
    int32_t HoldCall(int32_t callId);
    int32_t UnHoldCall(int32_t callId);
    int32_t SwitchCall(int32_t callId);
    int32_t CombineConference(int32_t mainCallId);
    int32_t SeparateConference(int32_t callId);
    int32_t UpdateCallMediaMode(int32_t callId, CallMediaMode mode);
    int32_t StartRtt(int32_t callId, std::u16string &msg);
    int32_t StopRtt(int32_t callId);
    int32_t JoinConference(int32_t callId, std::vector<std::string> &numberList);
    enum {
        HANDLER_DIAL_CALL_REQUEST = 0,
        HANDLER_ANSWER_CALL_REQUEST,
        HANDLER_REJECT_CALL_REQUEST,
        HANDLER_HANGUP_CALL_REQUEST,
        HANDLER_HOLD_CALL_REQUEST,
        HANDLER_UNHOLD_CALL_REQUEST,
        HANDLER_SWAP_CALL_REQUEST,
        HANDLER_COMBINE_CONFERENCE_REQUEST,
        HANDLER_SEPARATE_CONFERENCE_REQUEST,
        HANDLER_UPDATE_CALL_MEDIA_MODE_REQUEST,
        HANDLER_STARTRTT_REQUEST,
        HANDLER_STOPRTT_REQUEST,
        HANDLER_INVITE_TO_CONFERENCE,
    };

private:
    std::shared_ptr<AppExecFwk::EventRunner> eventLoop_;
    std::shared_ptr<CallRequestHandler> handler_;
};
} // namespace Telephony
} // namespace OHOS
#endif // CALL_CONTROL_MANAGER_HANDLER_H
