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
#include "common_type.h"
#include "hitrace/tracechain.h"
#include "telephony_log_wrapper.h"
#include "iservice_registry.h"

namespace OHOS {
namespace Telephony {
constexpr const int32_t MAX_VOICE_TEXT_LENGTH = 1000;
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
        if (switchName == ANTIFRAUD_CONTACTS_ENABLED_VOICE || switchName == ANTIFRAUD_CONTACTS_ENABLED_VIDEO) {
            return true;
        }
        return false;
    }

    TELEPHONY_LOGI("Antifraud switch: %{public}s query end, the result value is %{public}s",
        switchName.c_str(), resultValue.c_str());
    if (resultValue == "1" || resultValue == "true") {
        return true;
    } else {
        return false;
    }
}

bool AntiFraudService::IsAntiFraudSwitchOn(const VideoStateType videoStateType)
{
    if (videoStateType == VideoStateType::TYPE_VIDEO) {
        return IsSwitchOn(ANTIFRAUD_SWITCH_VIDEO);
    }
    return IsSwitchOn(ANTIFRAUD_SWITCH_VOICE);
}

bool AntiFraudService::IsAntiFraudContactsEnabled(const VideoStateType videoStateType)
{
    if (videoStateType == VideoStateType::TYPE_VIDEO) {
        return IsSwitchOn(ANTIFRAUD_CONTACTS_ENABLED_VIDEO);
    }
    return IsSwitchOn(ANTIFRAUD_CONTACTS_ENABLED_VOICE);
}

bool AntiFraudService::IsUserImprovementPlanSwitchOn()
{
    return IsSwitchOn(USER_IMPROPLAN_SWITCH);
}

void AntiFraudService::InitParams()
{
    fraudDetectText_ = "";
    antiFraudState_ = static_cast<int32_t>(AntiFraudState::ANTIFRAUD_STATE_DEFAULT);
}

int32_t AntiFraudService::GetStoppedSlotId()
{
    std::lock_guard<ffrt::mutex> lock(mutex_);
    return stoppedSlotId_;
}
 
int32_t AntiFraudService::GetStoppedIndex()
{
    std::lock_guard<ffrt::mutex> lock(mutex_);
    return stoppedIndex_;
}
 
void AntiFraudService::SetStoppedSlotId(int32_t slotId)
{
    std::lock_guard<ffrt::mutex> lock(mutex_);
    stoppedSlotId_ = slotId;
}
 
void AntiFraudService::SetStoppedIndex(int32_t index)
{
    std::lock_guard<ffrt::mutex> lock(mutex_);
    stoppedIndex_ = index;
}

void AntiFraudService::RecordDetectResult(const OHOS::AntiFraudService::StartDetectionResult &antiFraudResult,
    std::string resultPhoneNum, int32_t resultSlotId, int32_t resultIndex)
{
    if (GetStoppedSlotId() == resultSlotId && GetStoppedIndex() == resultIndex) {
        TELEPHONY_LOGI("detect stopped, no need to record result");
        SetStoppedSlotId(-1);
        SetStoppedIndex(-1);
        return;
    }

    std::lock_guard<ffrt::mutex> lock(fraudMutex_);
    antiFraudResultExt_.isVoiceSemanticFraud = antiFraudResult.voiceDetectionResult.result;
    antiFraudResultExt_.isSpeechSynthesisFraud = antiFraudResult.speechSynthesisResult.result;
    antiFraudResultExt_.speechSynthesisProb = antiFraudResult.speechSynthesisResult.pvalue;
    antiFraudResultExt_.isXoipFraud = antiFraudResult.voipCallTransferResult.result;
    if (!antiFraudResult.voiceDetectionResult.voiceText.empty() &&
        antiFraudResult.voiceDetectionResult.voiceText.length() <= MAX_VOICE_TEXT_LENGTH) {
        fraudDetectText_ = antiFraudResult.voiceDetectionResult.voiceText;
    }

    if (antiFraudResultExt_.isVoiceSemanticFraud || antiFraudResultExt_.isSpeechSynthesisFraud ||
        antiFraudResultExt_.isXoipFraud) {
        TELEPHONY_LOGI("AntiFraud detect finish, is fraud call");
        antiFraudState_ = static_cast<int32_t>(AntiFraudState::ANTIFRAUD_STATE_RISK);
    } else {
        TELEPHONY_LOGI("AntiFraud detect finish, is not fraud call");
        antiFraudState_ = static_cast<int32_t>(AntiFraudState::ANTIFRAUD_STATE_NOT_RISK_OR_STOPPED);
    }
    if (callStatusManagerPtr_ != nullptr) {
        callStatusManagerPtr_->TriggerAntiFraud(antiFraudState_, antiFraudResultExt_);
    }

    if (antiFraudResultExt_.isVoiceSemanticFraud && IsUserImprovementPlanSwitchOn()) {
        TELEPHONY_LOGI("text reported to the cloud after anonymize");
        int ret = AnonymizeText();
        if (ret != 0) {
            TELEPHONY_LOGE("Anonymize text fail");
            return;
        }
        OHOS::AntiFraudService::AntiFraudResult fraudResult = antiFraudResult.voiceDetectionResult;
        std::make_shared<AntiFraudCloudService>(resultPhoneNum)->UploadPostRequest(fraudResult);
    }
}

int32_t AntiFraudService::CheckAntiFraudService(const OHOS::AntiFraudService::AfsDetectType &detectType)
{
    auto antiFraudAdapter = DelayedSingleton<AntiFraudAdapter>::GetInstance();
    int32_t antiFraudErrCode = antiFraudAdapter->AntiFraudDetectCheck(detectType);
    if (antiFraudErrCode != 0) {
        TELEPHONY_LOGE("Check AntiFraud, no need to detect, ErrCode:%{public}d", antiFraudErrCode);
        return antiFraudErrCode;
    }
    return 0;
}

int32_t AntiFraudService::StartAntiFraudService(const std::string &phoneNum, int32_t slotId, int32_t index,
    const OHOS::AntiFraudService::AfsDetectType &detectType)
{
    int32_t antiFraudErrCode = CheckAntiFraudService(detectType);
    if (antiFraudErrCode != 0) {
        return antiFraudErrCode;
    }
    if (callStatusManagerPtr_ != nullptr) {
        if (callStatusManagerPtr_->GetAntiFraudSlotId() != slotId ||
            callStatusManagerPtr_->GetAntiFraudIndex() != index) {
            TELEPHONY_LOGI("call ending, no need to antifraud detect");
            return -1;
        }
    }
    OHOS::HiviewDFX::HiTraceId chainId = OHOS::HiviewDFX::HiTraceChain::GetId();
    if (!chainId.IsValid()) {
        chainId =
            OHOS::HiviewDFX::HiTraceChain::Begin("StartAntiFraudService", HiTraceFlag::HITRACE_FLAG_INCLUDE_ASYNC);
    }
    auto antiFraudAdapter = DelayedSingleton<AntiFraudAdapter>::GetInstance();
    auto listener = std::make_shared<AntiFraudStartDetectResListenerImpl>(phoneNum, slotId, index);
    antiFraudErrCode = antiFraudAdapter->AntiFraudStartDetect(listener, detectType);
    if (antiFraudErrCode != 0) {
        TELEPHONY_LOGE("AntiFraudStartDetect failed, ErrCode=%{public}d", antiFraudErrCode);
        return antiFraudErrCode;
    }
    TELEPHONY_LOGI("AntiFraud begin detect, slotId=%{public}d, index=%{public}d", slotId, index);
    std::lock_guard<ffrt::mutex> lock(fraudMutex_);
    antiFraudState_ = static_cast<int32_t>(AntiFraudState::ANTIFRAUD_STATE_STARTED);
    if (callStatusManagerPtr_ != nullptr) {
        callStatusManagerPtr_->TriggerAntiFraud(antiFraudState_, antiFraudResultExt_);
    }
    InitParams();
    return 0;
}

void AntiFraudService::UpdateVideoState(VideoStateType priorVideoState, VideoStateType nextVideoState)
{
    if (priorVideoState == VideoStateType::TYPE_VOICE && nextVideoState == VideoStateType::TYPE_VIDEO) {
        OHOS::AntiFraudService::AfsDetectType detectType(
            OHOS::AntiFraudService::ANTIFRAUD_DETECT_TYPE_XOIP_TRANSFER, false, "", 0);
        StopAntiFraudDetectByType(detectType);
    }
}

int32_t AntiFraudService::StopAntiFraudService(int32_t slotId, int32_t index)
{
    auto antiFraudAdapter = DelayedSingleton<AntiFraudAdapter>::GetInstance();
    int32_t antiFraudErrCode = antiFraudAdapter->StopAntiFraud();
    if (antiFraudErrCode != 0) {
        TELEPHONY_LOGE("Stop AntiFraud failed, ErrCode=%{public}d", antiFraudErrCode);
        return antiFraudErrCode;
    }
    TELEPHONY_LOGI("AntiFraud stop detect, slotId=%{public}d, index=%{public}d", slotId, index);
    SetStoppedSlotId(slotId);
    SetStoppedIndex(index);
    return 0;
}

int32_t AntiFraudService::StopAntiFraudDetectByType(const OHOS::AntiFraudService::AfsDetectType &detectType)
{
    auto antiFraudAdapter = DelayedSingleton<AntiFraudAdapter>::GetInstance();
    if (antiFraudAdapter == nullptr) {
        return -1;
    }
    int32_t antiFraudErrCode = antiFraudAdapter->StopAntiFraudByType(detectType);
    if (antiFraudErrCode != 0) {
        TELEPHONY_LOGE("Stop AntiFraud by type failed, ErrCode=%{public}d", antiFraudErrCode);
        return antiFraudErrCode;
    }
    TELEPHONY_LOGI("AntiFraud stop detect by type, type=%{public}u", detectType.type_);
    return 0;
}

void AntiFraudService::AntiFraudStartDetectResListenerImpl::HandleAntiFraudStartDetectRes(
    const OHOS::AntiFraudService::StartDetectionResult &startDetectionResult)
{
    TELEPHONY_LOGI(
        "HandleAntiFraudStartDetectRes: voiceDetectionResult=%{public}d, speechSynthesisResult=%{public}d, "
        "voipCallTransferResult=%{public}d, slotId=%{public}d, index=%{public}d",
        startDetectionResult.voiceDetectionResult.result, startDetectionResult.speechSynthesisResult.result,
        startDetectionResult.voipCallTransferResult.result, slotId_, index_);
    auto antiFraudService = DelayedSingleton<AntiFraudService>::GetInstance();
    if (antiFraudService != nullptr) {
        antiFraudService->RecordDetectResult(startDetectionResult, phoneNum_, slotId_, index_);
    }
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
