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

#ifndef CELLULAR_CALL_STUB_TEST_H
#define CELLULAR_CALL_STUB_TEST_H
#include <map>
#include <string>
#include <vector>

#include "iremote_stub.h"
#include "message_parcel.h"
#include "refbase.h"

#include "cellular_call_interface.h"
#include "cellular_call_types.h"
#include "i_call_status_callback.h"

namespace OHOS {
namespace TelephonyCallManager {
class CellularCallStubTest : public IRemoteStub<CellularCall::CellularCallInterface> {
public:
    /**
     * CellularCallStub
     */
    CellularCallStubTest();

    virtual ~CellularCallStubTest();
    /**
     * OnRemoteRequest
     *
     * @param code
     * @param data
     * @param reply
     * @param option
     * @return
     */
    virtual int OnRemoteRequest(
        uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option) override;

    /**
     * Call management diale interface
     *
     * @param CellularCallInfo, dial param.
     * @return Returns TELEPHONY_NO_ERROR on success, others on failure.
     */
    int Dial(const CellularCall::CellularCallInfo &callInfo) override;

    /**
     * End.
     *
     * @param CellularCallInfo, End param.
     * @return Returns TELEPHONY_NO_ERROR on success, others on failure.
     */
    int End(const CellularCall::CellularCallInfo &callInfo) override;

    /**
     * Reject.
     *
     * @param CellularCallInfo, Reject param.
     * @return Returns TELEPHONY_NO_ERROR on success, others on failure.
     */
    int Reject(const CellularCall::CellularCallInfo &callInfo) override;

    /**
     * Answer.
     *
     * @param CellularCallInfo, Answer param.
     * @return Returns TELEPHONY_NO_ERROR on success, others on failure.
     */
    int Answer(const CellularCall::CellularCallInfo &callInfo) override;

    /**
     * @brief Is it an emergency call
     * @param string &phoneNum
     * @param slotId
     * @return Returns TELEPHONY_NO_ERROR on success, others on failure.
     */
    int IsUrgentCall(const std::string &phoneNum, const int32_t slotId) override;

    /**
     * RegisterCallBack
     *
     * @return Returns TELEPHONY_NO_ERROR on success, others on failure.
     */
    int RegisterCallManagerCallBack(const sptr<ICallStatusCallback> &callback) override;

    /**
     * UnRegister CallBack
     *
     * @return Returns TELEPHONY_NO_ERROR on success, others on failure.
     */
    int UnRegisterCallManagerCallBack() override;

private:
    using CellularCallServiceFunc = int32_t (CellularCallStubTest::*)(MessageParcel &data, MessageParcel &reply);

    int32_t DialRequest(MessageParcel &data, MessageParcel &reply);
    int32_t AnswerRequest(MessageParcel &data, MessageParcel &reply);
    int32_t RejectRequest(MessageParcel &data, MessageParcel &reply);
    int32_t EndRequest(MessageParcel &data, MessageParcel &reply);
    int32_t RegisterCallBackRequest(MessageParcel &data, MessageParcel &reply);

    int32_t ReportDialingState(int32_t callId) const;
    int32_t ReportAlertingState(int32_t callId) const;
    int32_t ReportActiveState(int32_t callId) const;
    int32_t ReportHoldingState(int32_t callId) const;
    int32_t ReportIncomingState() const;
    int32_t ReportWaitingState() const;
    int32_t ReportDisconnectingState(int32_t callId) const;
    int32_t ReportDisconnectedState(int32_t callId) const;

    std::map<uint32_t, CellularCallServiceFunc> memberFuncMap_;
    sptr<ICallStatusCallback> callback_;
    std::vector<CellularCall::CellularCallInfo> callInfoVec_;
};
} // namespace TelephonyCallManager
} // namespace OHOS

#endif // CELLULAR_CALL_STUB_TEST_H
