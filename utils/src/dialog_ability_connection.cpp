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
 
#include "dialog_ability_connection.h"
 
#include "want.h"
#include "want_params.h"
#include "extension_manager_client.h"
#include "telephony_log_wrapper.h"
#include "parameter.h"
#include "cJSON.h"
 
namespace OHOS {
namespace Telephony {
constexpr int32_t MESSAGE_PARCEL_KEY_SIZE = 3;
constexpr int32_t SEND_OPEN_DIALOG_CMD = 1;
constexpr int32_t FAILURE = -1;
 
DialogAbilityConnection::DialogAbilityConnection() {}
DialogAbilityConnection::~DialogAbilityConnection() {}
 
struct DialogParams {
    std::string bundleName;
    std::string abilityName;
    std::string phoneNumber;
    std::string callingBundleName;
};
 
int32_t DialogAbilityConnection::OpenSystemDialog(const std::string &bundleName, const std::string &abilityName,
    const std::string &phoneNumber, const std::string &callingBundleName)
{
    std::unique_lock<ffrt::mutex> lock(mutex_);
    bundleName_ = bundleName; //"com.ohos.contacts", "CardDialogServiceExtAbility"
    abilityName_ = abilityName;
    phoneNumber_ = phoneNumber;
    callingBundleName_ = callingBundleName;
    lock.unlock();
    auto ret = ConnectExtension();
    return ret;
}
 
int32_t DialogAbilityConnection::ConnectExtension()
{
    AAFwk::Want want;
    std::string sceneboardName = "com.ohos.sceneboard";
    std::string abilityName = "com.ohos.sceneboard.systemdialog";
    want.SetElementName(sceneboardName, abilityName);
    auto abilityManager = AAFwk::AbilityManagerClient::GetInstance();
    if (abilityManager == nullptr) {
        TELEPHONY_LOGE("AbilityManagerClient is nullptr");
        return FAILURE;
    }
    sptr<DialogAbilityConnection> doubleialogAbilityConnection = this;
    auto ret = abilityManager->ConnectAbility(want, doubleialogAbilityConnection, -1);
    TELEPHONY_LOGI("ConnectAbility ret: %{public}d", ret);
    return ret;
}
 
void PrepareDialogData(const DialogParams& params, MessageParcel& data)
{
    data.WriteInt32(MESSAGE_PARCEL_KEY_SIZE);
    data.WriteString16(u"bundleName");
    data.WriteString16(Str8ToStr16(params.bundleName));
    data.WriteString16(u"abilityName");
    data.WriteString16(Str8ToStr16(params.abilityName));
 
    cJSON* root = cJSON_CreateObject();
    if (root == nullptr) {
        return;
    }
 
    cJSON* uiExtensionType = cJSON_CreateString("sysDialog/common");
    if (uiExtensionType != nullptr) {
        cJSON_AddItemToObject(root, "ability.want.params.uiExtensionType", uiExtensionType);
    }
 
    cJSON* number = cJSON_CreateString(params.phoneNumber.c_str());
    if (number != nullptr) {
        cJSON_AddItemToObject(root, "phoneNumber", number);
    }
 
    cJSON* callingBundleName = cJSON_CreateString(params.callingBundleName.c_str());
    if (callingBundleName != nullptr) {
        cJSON_AddItemToObject(root, "callingBundleName", callingBundleName);
    }
    
    char* parameters = cJSON_Print(root);
    if (parameters != nullptr) {
        data.WriteString16(u"parameters");
        data.WriteString16(Str8ToStr16(std::string(parameters)));
        cJSON_free(parameters);
    }
    
    cJSON_Delete(root);
}
 
void DialogAbilityConnection::OnAbilityConnectDone(
    const AppExecFwk::ElementName &element, const sptr<IRemoteObject> &remoteObject, int resultCode)
{
    if (remoteObject == nullptr) {
        TELEPHONY_LOGE("remoteObject is nullptr");
        return;
    }
    std::unique_lock<ffrt::mutex> lock(mutex_);
    DialogParams params = {
        .bundleName = bundleName_,
        .abilityName = abilityName_,
        .phoneNumber = phoneNumber_,
        .callingBundleName = callingBundleName_,
    };
    lock.unlock();
    MessageParcel reply;
    MessageParcel data;
    MessageOption option;
    PrepareDialogData(params, data);
    int32_t ret = remoteObject->SendRequest(SEND_OPEN_DIALOG_CMD, data, reply, option);
    dialogContext_ = remoteObject;
    if (ret != ERR_OK) {
        TELEPHONY_LOGE("Show makeDirectCall dialog is failed: %{public}d", ret);
        return;
    }
    TELEPHONY_LOGI("Show makeDirectCall dialog success");
}
 
void DialogAbilityConnection::OnAbilityDisconnectDone(const AppExecFwk::ElementName &element, int resultCode)
{
    TELEPHONY_LOGI("OnAbilityDisconnectDone");
    int32_t ret = Destroy();
    if (ret == FAILURE) {
        TELEPHONY_LOGE("Destory makeDirectCall dialog failed");
    }
}
 
int32_t DialogAbilityConnection::Destroy()
{
    dialogContext_ = nullptr;
    auto dialog = AAFwk::AbilityManagerClient::GetInstance();
    if (dialog == nullptr) {
        TELEPHONY_LOGE("dialog is nullptr");
        return FAILURE;
    }
    return dialog->DisconnectAbility(this);
}
} // namespace Telephony
} // namespace OHOS