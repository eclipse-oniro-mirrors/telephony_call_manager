/*
 * Copyright (C) 2026 Huawei Device Co., Ltd.
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

#ifndef TRANSFER_CONTROL_MOCK_H
#define TRANSFER_CONTROL_MOCK_H

#include <gmock/gmock.h>
#include "transfer_control.h"

namespace OHOS {
namespace Telephony {
class MockTransferControl : public TransferControl {
public:
    MOCK_METHOD(bool, IsCallNeedTransfer, (const TransferCallInfo &info, TransferResultInfo &resultInfo), (override));
    MOCK_METHOD(bool, NotifyCallState, (const TransferCallInfo &info), (override));
    MOCK_METHOD(bool, DisconnectSco, (const std::string &devMac), (override));
};
} // namespace Telephony
} // namespace OHOS

#endif // TRANSFER_CONTROL_MOCK_H
