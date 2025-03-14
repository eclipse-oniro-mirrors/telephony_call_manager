/*
 * Copyright (C) 2025-2025 Huawei Device Co., Ltd.
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
 
#include "antifraud_service.h"

#include <string>
#include "antifraud_adapter.h"
#include "anonymize_adapter.h"
#include "antifraud_cloud_service.h"
#include "anti_fraud_service_client.h"
#include "common_type.h"
#include "telephony_log_wrapper.h"
#include "iservice_registry.h"
 
namespace OHOS {
namespace Telephony {
AntiFraudService::AntiFraudService()
{}
 
AntiFraudService::~AntiFraudService()
{}
 
void AntiFraudService::SetCallStatusManager(std::shared_ptr<CallStatusManager> callStatusManager)
{
    callStatusManagerPtr_ = callStatusManager;
}
 
std::shared_ptr<DataShare::DataShareHelper> AntiFraudService::CreateDataShareHelper(
    int32_t systemAbilityId, const char *uri)
{
    sptr<ISystemAbilityManager> saManager = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (saManager == nullptr) {
        TELEPHONY_LOGE("CellularDataRdbHelper GetSystemAbilityManager failed.");
        return nullptr;
    }
    sptr<IRemoteObject> remoteObj = saManager->GetSystemAbility(systemAbilityId);
    if (remoteObj == nullptr) {
        TELEPHONY_LOGE("CellularDataRdbHelper GetSystemAbility Service Failed.");
        return nullptr;
    }
    return DataShare::DataShareHelper::Creator(remoteObj, uri);
}
 
bool AntiFraudService::IsSwitchOn(const std::string switchName)
{
    auto settingHelper = CreateDataShareHelper(TELEPHONY_CALL_MANAGER_SYS_ABILITY_ID,
        USER_SETTINGSDATA_URI.c_str());
    if (settingHelper == nullptr) {
        TELEPHONY_LOGE("settingHelper is null");
        return false;
    }
 
    DataShare::DataSharePredicates predicates;
    std::vector<std::string> columns;
    std::string keyword = "KEYWORD";
    predicates.EqualTo(keyword, switchName);
    Uri userStttingsDataUri(USER_SETTINGSDATA_URI);
    auto result = settingHelper->Query(userStttingsDataUri, predicates, columns);
    if (result == nullptr) {
        TELEPHONY_LOGE("setting DB: query error");
        settingHelper->Release();
        return false;
    }
 
    result->GoToFirstRow();
    int32_t columnIndex;
    std::string resultValue;
    std::string valueStr = "VALUE";
    result->GetColumnIndex(valueStr, columnIndex);
    result->GetString(columnIndex, resultValue);
    result->Close();
    settingHelper->Release();
    if (resultValue.empty()) {
        TELEPHONY_LOGE("resultValue is empty");
        return false;
    }
 
    TELEPHONY_LOGI("Query end resultValue is %{public}s", resultValue.c_str());
    int32_t value = atoi(resultValue.c_str());
    if (value == 1) {
        return true;
    } else {
        return false;
    }
}
 
bool AntiFraudService::IsAntiFraudSwitchOn()
{
    return IsSwitchOn(ANTIFRAUD_SWITCH);
}
 
bool AntiFraudService::IsUserImprovementPlanSwitchOn()
{
    return IsSwitchOn(USER_IMPROPLAN_SWITCH);
}
 
void AntiFraudService::InitParams()
{
    fraudDetectErr_ = 0;
    isResultFraud_ = false;
    fraudDetectVersion_ = 0;
    fraudDetectType_ = 0;
    fraudDetectText_ = "";
    fraudDetectProb_ = 0.0;
    antiFraudState_ = static_cast<int32_t>(AntiFraudState::ANTIFRAUD_STATE_DEFAULT);
}
 
void AntiFraudService::RecordDetectResult(const OHOS::AntiFraudService::AntiFraudResult &antiFraudResult,
                                          int32_t resultSlotId, int32_t resultIndex)
{
    std::lock_guard<std::mutex> lock(queueMutex_);
    std::string phoneNum = "";
    if (!phoneNumQueue_.empty()) {
        phoneNum = phoneNumQueue_.front();
        phoneNumQueue_.pop();
    }
    if (resultSlotId == stoppedSlotId_ && resultIndex == stoppedIndex_) {
        TELEPHONY_LOGI("detect stopped, no need to record result");
        stoppedSlotId_ = -1;
        stoppedIndex_ = -1;
        return;
    }
    fraudDetectErr_ = antiFraudResult.errCode;
    isResultFraud_ = antiFraudResult.result;
    fraudDetectVersion_ = antiFraudResult.modelVersion;
    fraudDetectType_ = antiFraudResult.fraudType;
    fraudDetectText_ = antiFraudResult.text;
    fraudDetectProb_ = antiFraudResult.pvalue;
 
    auto antiFraudAdapter = DelayedSingleton<AntiFraudAdapter>::GetInstance();
    if (isResultFraud_) {
        TELEPHONY_LOGI("AntiFraud detect finish, is fraud call");
        antiFraudState_ = static_cast<int32_t>(AntiFraudState::ANTIFRAUD_STATE_RISK);
    } else {
        TELEPHONY_LOGI("AntiFraud detect finish, is not fraud call");
        antiFraudState_ = static_cast<int32_t>(AntiFraudState::ANTIFRAUD_STATE_FINISHED);
    }
    if (callStatusManagerPtr_ != nullptr) {
        callStatusManagerPtr_->TriggerAntiFraud(antiFraudState_);
        }
 
    if (isResultFraud_ && IsUserImprovementPlanSwitchOn()) {
        TELEPHONY_LOGI("text reported to the cloud after anonymize");
        int ret = AnonymizeText();
        if (ret != 0) {
            TELEPHONY_LOGE("Anonymize text fail");
            return;
        }
        OHOS::AntiFraudService::AntiFraudResult fraudResult = antiFraudResult;
        fraudResult.text = fraudDetectText_;
        std::make_shared<AntiFraudCloudService>(phoneNum)->UploadPostRequest(fraudResult);
    }
}
 
void AntiFraudService::InitAntiFraudService(const std::string &phoneNum, int32_t slotId, int32_t index)
{
    InitParams();
    auto antiFraudAdapter = DelayedSingleton<AntiFraudAdapter>::GetInstance();
    auto listener = std::make_shared<AntiFraudDetectResListenerImpl>(slotId, index);
    int32_t antiFraudErrCode = antiFraudAdapter->DetectAntiFraud(listener);
    if (antiFraudErrCode == 0) {
        TELEPHONY_LOGI("AntiFraud begin detect, slotId=%{public}d, index=%{public}d", slotId, index);
        std::lock_guard<std::mutex> lock(queueMutex_);
        phoneNumQueue_.push(phoneNum);
        antiFraudState_ = static_cast<int32_t>(AntiFraudState::ANTIFRAUD_STATE_STARTED);
        if (callStatusManagerPtr_ != nullptr) {
            callStatusManagerPtr_->TriggerAntiFraud(antiFraudState_);
        }
    }
}

void AntiFraudService::StopAntiFraudService(int32_t slotId, int32_t index)
{
    auto antiFraudAdatper = DelayedSingleton<AntiFraudAdapter>::GetInstance();
    int32_t antiFraudErrCode = antiFraudAdapter->StopAntiFraud();
    if (antiFraudErrCode == 0) {
        TELEPHONY_LOGI("AntiFraud stop detect, slotId=%{public}d, index=%{public}d", slotId, index);
        stoppedSlotId_ = slotId;
        stoppedIndex_ = index;
    }
}
 
void AntiFraudService::AntiFraudDetectResListenerImpl::HandleAntiFraudDetectRes(
    const OHOS::AntiFraudService::AntiFraudResult &antiFraudResult)
{
    int32_t resultSlotId = slotId_;
    int32_t resultIndex = index_;
    TELEPHONY_LOGI("HandleAntiFraudDetectRes, result=%{public}d, slotId=%{public}d, index=%{public}d",
        antiFraudResult.result, slotId_, index_);
    DelayedSingleton<AntiFraudService>::GetInstance()->RecordDetectResult(antiFraudResult, resultSlotId, resultIndex);
}

void AntiFraudService::AddRuleToConfig(const std::string rulesName, void *config)
{
    DIA_String key;
    std::string skey = "SELECT_RULE";
    key.data = skey.data();
    key.dataLength = SELECT_RULE_LENGTH;
    DIA_Rule rule;
    std::string rName = rulesName;
    rule.ruleName = rName.data();
    if ((rName == "CHINA_RESIDENT_PASSPORT") || (rName == "PERMIT_HM_TO_LAND") ||
        (rName == "PERMIT_TW_TO_LAND") || (rName == "POLICE_OFFICER_CARD")) {
        rule.isOpenKeywords = true;
    }
    auto anonymizeAdapter = DelayedSingleton<AnonymizeAdapter>::GetInstance();
    int ret = anonymizeAdapter->SetRule(config, &key, &rule);
    if (ret != 0) {
        TELEPHONY_LOGE("AddRuleToConfig fail, rulename=%{public}s", rulesName.c_str());
    }
}

int AntiFraudService::AnonymizeText()
{
    void *config = nullptr;
    auto anonymizeAdapter = DelayedSingleton<AnonymizeAdapter>::GetInstance();
    int ret = anonymizeAdapter->InitConfig(&config);
    if (ret != 0) {
        TELEPHONY_LOGE("InitConfig fail");
        return ret;
    }
    AddRuleToConfig("PRC", config);
    AddRuleToConfig("CHINA_RESIDENT_PASSPORT", config);
    AddRuleToConfig("MILITARY_IDENTITY_CARD_NUMBER", config);
    AddRuleToConfig("BANK_CARD_NUMBER", config);
    AddRuleToConfig("PERMIT_LAND_TO_HM", config);
    AddRuleToConfig("PERMIT_LAND_TO_TW", config);
    AddRuleToConfig("PERMIT_HM_TO_LAND", config);
    AddRuleToConfig("PERMIT_TW_TO_LAND", config);
    AddRuleToConfig("BIRTH_CERTIFICATE", config);
    AddRuleToConfig("SEAFARER_PASSPORT", config);
    AddRuleToConfig("POLICE_OFFICER_CARD", config);

    void *assistant = nullptr;
    ret = anonymizeAdapter->CreateAnonymize(config, &assistant);
    if (ret != 0) {
        TELEPHONY_LOGE("CreateAnonymize fail");
        return ret;
    }

    DIA_String input;
    std::string fraudDetectText = fraudDetectText_;
    input.data = fraudDetectText.data();
    input.dataLength = strlen(input.data);
    DIA_String *output = nullptr;
    ret = anonymizeAdapter->IdentifyAnonymize(assistant, &input, &output);
    if (ret != 0) {
        TELEPHONY_LOGE("IdentifyAnonymize fail");
        return ret;
    } else {
        fraudDetectText_ = (std::string)output->data;
    }
    anonymizeAdapter->ReleaseConfig(&config);
    anonymizeAdapter->ReleaseAnonymize(&assistant);
    anonymizeAdapter->ReleaseOutputData(&output);
    return 0;
}
}
}