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

#ifndef CALL_DATA_BASE_HELPER_H
#define CALL_DATA_BASE_HELPER_H

#include "ability_manager_interface.h"
#include "if_system_ability_manager.h"

#include "refbase.h"
#include "singleton.h"

#include "system_ability.h"
#include "system_ability_definition.h"

#include "data_ability_helper.h"
#include "data_ability_predicates.h"
#include "values_bucket.h"
#include "abs_shared_result_set.h"
#include "data_ability_observer_stub.h"

namespace OHOS {
namespace Telephony {
const std::string CALL_PHONE_NUMBER = "phone_number";
const std::string CALL_DISPLAY_NAME = "display_name";
const std::string CALL_DIRECTION = "call_direction";
const std::string CALL_VOICEMAIL_URI = "voicemail_uri";
const std::string CALL_SIM_TYPE = "sim_type";
const std::string CALL_IS_HD = "is_hd";
const std::string CALL_IS_READ = "is_read";
const std::string CALL_RING_DURATION = "ring_duration";
const std::string CALL_TALK_DURATION = "talk_duration";
const std::string CALL_FORMAT_NUMBER = "format_number";
const std::string CALL_QUICKSEARCH_KEY = "quicksearch_key";
const std::string CALL_NUMBER_TYPE = "number_type";
const std::string CALL_NUMBER_TYPE_NAME = "number_type_name";
const std::string CALL_BEGIN_TIME = "begin_time";
const std::string CALL_END_TIME = "end_time";
const std::string CALL_ANSWER_STATE = "answer_state";
const std::string CALL_CREATE_TIME = "create_time";
const std::string CALL_NUMBER_LOCATION = "number_location";
const std::string CALL_PHOTO_ID = "photo_id";

class CallDataRdbObserver : public AAFwk::DataAbilityObserverStub {
public:
    CallDataRdbObserver(std::vector<std::string> *phones);
    ~CallDataRdbObserver();
    void OnChange() override;

private:
    std::vector<std::string> *phones;
};

class CallDataBaseHelper {
    DECLARE_DELAYED_SINGLETON(CallDataBaseHelper)
public:
    void RegisterObserver(std::vector<std::string> *phones);
    void UnRegisterObserver();
    bool Insert(NativeRdb::ValuesBucket &values);
    bool Query(std::vector<std::string> *phones, NativeRdb::DataAbilityPredicates &predicates);
    bool Delete(NativeRdb::DataAbilityPredicates &predicates);

private:
    sptr<CallDataRdbObserver> callDataRdbObserverPtr_;
    std::shared_ptr<AppExecFwk::DataAbilityHelper> CreateDataAHelper();
    void ResultSetConvertToIndexer(const std::shared_ptr<NativeRdb::AbsSharedResultSet> &resultSet);
    const std::string CALL_SUBSECTION = "dataability:///com.ohos.calllogability/calls/calllog";
    const std::string CALL_BLOCK = "dataability:///com.ohos.contactsdataability/contacts/contact_blocklist";
};
} // namespace Telephony
} // namespace OHOS
#endif