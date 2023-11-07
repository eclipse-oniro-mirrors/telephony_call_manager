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

#ifndef TELEPHONY_CALL_DATA_BASE_HELPER_H
#define TELEPHONY_CALL_DATA_BASE_HELPER_H

#include "ability_manager_interface.h"
#include "common_type.h"
#include "data_ability_observer_stub.h"
#include "datashare_helper.h"
#include "datashare_predicates.h"
#include "if_system_ability_manager.h"
#include "refbase.h"
#include "singleton.h"
#include "system_ability.h"
#include "system_ability_definition.h"

namespace OHOS {
namespace Telephony {
constexpr const char *CALL_PHONE_NUMBER = "phone_number";
constexpr const char *CALL_DISPLAY_NAME = "display_name";
constexpr const char *CALL_DIRECTION = "call_direction";
constexpr const char *CALL_VOICEMAIL_URI = "voicemail_uri";
constexpr const char *CALL_SIM_TYPE = "sim_type";
constexpr const char *CALL_IS_HD = "is_hd";
constexpr const char *CALL_IS_READ = "is_read";
constexpr const char *CALL_RING_DURATION = "ring_duration";
constexpr const char *CALL_TALK_DURATION = "talk_duration";
constexpr const char *CALL_FORMAT_NUMBER = "format_number";
constexpr const char *CALL_QUICKSEARCH_KEY = "quicksearch_key";
constexpr const char *CALL_NUMBER_TYPE = "number_type";
constexpr const char *CALL_NUMBER_TYPE_NAME = "number_type_name";
constexpr const char *CALL_BEGIN_TIME = "begin_time";
constexpr const char *CALL_END_TIME = "end_time";
constexpr const char *CALL_ANSWER_STATE = "answer_state";
constexpr const char *CALL_CREATE_TIME = "create_time";
constexpr const char *CALL_NUMBER_LOCATION = "number_location";
constexpr const char *CALL_PHOTO_ID = "photo_id";
constexpr const char *CALL_DETAIL_INFO = "detail_info";
constexpr const char *CALL_CONTENT_TYPE = "content_type";
constexpr const char *CALL_PHONE = "phone";
constexpr const char *CALL_SLOT_ID = "slot_id";

enum class CallLogReadState {
    CALL_IS_UNREAD,
    CALL_IS_READ,
};

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
    bool Insert(DataShare::DataShareValuesBucket &values);
    bool Query(std::vector<std::string> *phones, DataShare::DataSharePredicates &predicates);
    bool Query(ContactInfo &contactInfo, DataShare::DataSharePredicates &predicates);
    bool Update(DataShare::DataSharePredicates &predicates, DataShare::DataShareValuesBucket &values);
    bool Delete(DataShare::DataSharePredicates &predicates);
    bool QueryCallLog(
        std::map<std::string, int32_t> &phonesAndUnreadCountMap, DataShare::DataSharePredicates &predicates);
    int32_t QueryIsBlockPhoneNumber(const std::string &phoneNum, bool &result);

public:
    const int16_t CALL_LOG_DEFAULT_COUNT = 1;

private:
    sptr<CallDataRdbObserver> callDataRdbObserverPtr_;
    std::shared_ptr<DataShare::DataShareHelper> CreateDataShareHelper(std::string uri);
};
} // namespace Telephony
} // namespace OHOS
#endif
