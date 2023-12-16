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
#include "video_control_manager.h"

#include <algorithm>

#include "call_ability_report_proxy.h"
#include "cellular_call_connection.h"
#include "file_ex.h"
#include "telephony_errors.h"
#include "telephony_log_wrapper.h"

#ifdef ABILITY_CAMERA_SUPPORT
#include "input/camera_manager.h"
#endif

namespace OHOS {
namespace Telephony {
namespace {
const int16_t CAMERA_ROTATION_0 = 0;
const int16_t CAMERA_ROTATION_90 = 90;
const int16_t CAMERA_ROTATION_180 = 180;
const int16_t CAMERA_ROTATION_270 = 270;
const int16_t VIDEO_WINDOWS_Z_BOTTOM = 0;
const int16_t VIDEO_WINDOWS_Z_TOP = 1;
const float MIN_CAMERA_ZOOM = 0.1;
const float MAX_CAMERA_ZOOM = 10.0;
const std::string SUPPORT_PICTURE_EXT = "png";
} // namespace

VideoControlManager::VideoControlManager() : isOpenCamera_(false) {}

VideoControlManager::~VideoControlManager() {}

int32_t VideoControlManager::ControlCamera(
    int32_t callId, std::u16string &cameraId, int32_t callingUid, int32_t callingPid)
{
    if (cameraId.empty()) {
        return CloseCamera(callId, cameraId, callingUid, callingPid);
    } else {
        return OpenCamera(callId, cameraId, callingUid, callingPid);
    }
}

int32_t VideoControlManager::SetPreviewWindow(int32_t callId, std::string &surfaceId, sptr<Surface> surface)
{
    int32_t ret = TELEPHONY_ERR_FAIL;
    ret = CallPolicy::VideoCallPolicy(callId);
    if (ret != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("check prerequisites failed !");
        return ret;
    }
    sptr<CallBase> callPtr = CallObjectManager::GetOneCallObject(callId);
    if (callPtr == nullptr) {
        TELEPHONY_LOGE("the call object is nullptr, callId:%{public}d", callId);
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    sptr<IMSCall> netCall = reinterpret_cast<IMSCall *>(callPtr.GetRefPtr());
    ret = netCall->SetPreviewWindow(surfaceId, surface);
    if (ret != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("SetPreviewWindow failed!");
        return ret;
    }
    return TELEPHONY_SUCCESS;
}

int32_t VideoControlManager::SetDisplayWindow(int32_t callId, std::string &surfaceId, sptr<Surface> surface)
{
    int32_t ret = TELEPHONY_ERR_FAIL;
    ret = CallPolicy::VideoCallPolicy(callId);
    if (ret != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("check prerequisites failed !");
        return ret;
    }
    sptr<CallBase> callPtr = CallObjectManager::GetOneCallObject(callId);
    if (callPtr == nullptr) {
        TELEPHONY_LOGE("the call object is nullptr, callId:%{public}d", callId);
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    sptr<IMSCall> netCall = reinterpret_cast<IMSCall *>(callPtr.GetRefPtr());
    ret = netCall->SetDisplayWindow(surfaceId, surface);
    if (ret != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("SetPreviewWindow failed!");
        return ret;
    }
    return TELEPHONY_SUCCESS;
}

int32_t VideoControlManager::SetCameraZoom(float zoomRatio)
{
    // param check
    if (zoomRatio < MIN_CAMERA_ZOOM || zoomRatio > MAX_CAMERA_ZOOM) {
        TELEPHONY_LOGE("camera zoom error!!");
        return CALL_ERR_VIDEO_INVALID_ZOOM;
    }
    return DelayedSingleton<CellularCallConnection>::GetInstance()->SetCameraZoom(zoomRatio);
}

int32_t VideoControlManager::SetPausePicture(int32_t callId, std::u16string &path)
{
    int32_t ret = TELEPHONY_ERR_FAIL;
    ret = CallPolicy::VideoCallPolicy(callId);
    if (ret != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("check prerequisites failed !");
        return ret;
    }
    std::string tempPath(Str16ToStr8(path));
    sptr<CallBase> callPtr = CallObjectManager::GetOneCallObject(callId);
    if (callPtr == nullptr) {
        TELEPHONY_LOGE("the call object is nullptr, callId:%{public}d", callId);
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    sptr<IMSCall> netCall = reinterpret_cast<IMSCall *>(callPtr.GetRefPtr());
    ret = netCall->SetPausePicture(tempPath);
    if (ret != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("SetPreviewWindow failed!");
        return ret;
    }
    return TELEPHONY_SUCCESS;
}

int32_t VideoControlManager::SetDeviceDirection(int32_t callId, int32_t rotation)
{
    int32_t ret = TELEPHONY_ERR_FAIL;
    ret = CallPolicy::VideoCallPolicy(callId);
    if (ret != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("check prerequisites failed !");
        return ret;
    }
    // param check
    if (rotation == CAMERA_ROTATION_0 || rotation == CAMERA_ROTATION_90 || rotation == CAMERA_ROTATION_180 ||
        rotation == CAMERA_ROTATION_270) {
        sptr<CallBase> callPtr = CallObjectManager::GetOneCallObject(callId);
        if (callPtr == nullptr) {
            TELEPHONY_LOGE("the call object is nullptr, callId:%{public}d", callId);
            return TELEPHONY_ERR_LOCAL_PTR_NULL;
        }
        sptr<IMSCall> netCall = reinterpret_cast<IMSCall *>(callPtr.GetRefPtr());
        ret = netCall->SetDeviceDirection(rotation);
        if (ret != TELEPHONY_SUCCESS) {
            TELEPHONY_LOGE("SetPreviewWindow failed!");
            return ret;
        }
        return TELEPHONY_SUCCESS;
    }
    TELEPHONY_LOGE("error rotation:%{public}d", rotation);
    return CALL_ERR_VIDEO_INVALID_ROTATION;
}

int32_t VideoControlManager::UpdateImsCallMode(int32_t callId, ImsCallMode callMode)
{
    int32_t ret = TELEPHONY_ERR_FAIL;
    ret = CallPolicy::VideoCallPolicy(callId);
    if (ret != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("check prerequisites failed !");
        return ret;
    }
    sptr<CallBase> callPtr = CallObjectManager::GetOneCallObject(callId);
    if (callPtr == nullptr) {
        TELEPHONY_LOGE("the call object is nullptr, callId:%{public}d", callId);
        return ret;
    }
    // only netcall type support update call media mode
    sptr<IMSCall> netCall = reinterpret_cast<IMSCall *>(callPtr.GetRefPtr());
    TELEPHONY_LOGI("ims call update media request");
    ret = netCall->UpdateImsCallMode(callMode);
    if (ret != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("UpdateImsCallMode failed!. %{public}d", ret);
    }
    return ret;
}

int32_t VideoControlManager::ReportImsCallModeInfo(CallMediaModeInfo &imsCallModeInfo)
{
    return DelayedSingleton<CallAbilityReportProxy>::GetInstance()->ReportImsCallModeChange(imsCallModeInfo);
}

int32_t VideoControlManager::OpenCamera(
    int32_t callId, std::u16string &cameraId, int32_t callingUid, int32_t callingPid)
{
    // cameraId check
    std::string id(Str16ToStr8(cameraId));
    int32_t ret = TELEPHONY_ERR_FAIL;
    ret = CallPolicy::VideoCallPolicy(callId);
    if (ret != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("check prerequisites failed !");
        return ret;
    }
    sptr<CallBase> callPtr = CallObjectManager::GetOneCallObject(callId);
    if (callPtr == nullptr) {
        TELEPHONY_LOGE("the call object is nullptr, callId:%{public}d", callId);
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    sptr<IMSCall> netCall = reinterpret_cast<IMSCall *>(callPtr.GetRefPtr());
    ret = netCall->ControlCamera(id, callingUid, callingPid);
    if (ret == TELEPHONY_SUCCESS) {
        isOpenCamera_ = true;
    }
    ret = netCall->RequestCameraCapabilities();
    if (ret != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("RequestCameraCapabilities failed!");
        return ret;
    }
    return ret;
}

int32_t VideoControlManager::CloseCamera(
    int32_t callId, std::u16string &cameraId, int32_t callingUid, int32_t callingPid)
{
    std::string id(Str16ToStr8(cameraId));
    int32_t ret = TELEPHONY_ERR_FAIL;
    ret = CallPolicy::VideoCallPolicy(callId);
    if (ret != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("check prerequisites failed !");
        return ret;
    }
    if (isOpenCamera_) {
        sptr<CallBase> callPtr = CallObjectManager::GetOneCallObject(callId);
        if (callPtr == nullptr) {
            TELEPHONY_LOGE("the call object is nullptr, callId:%{public}d", callId);
            return TELEPHONY_ERR_LOCAL_PTR_NULL;
        }
        sptr<IMSCall> netCall = reinterpret_cast<IMSCall *>(callPtr.GetRefPtr());
        ret = netCall->ControlCamera(id, callingUid, callingPid);
        if (ret == TELEPHONY_SUCCESS) {
            isOpenCamera_ = true;
        }
        return ret;
    }
    TELEPHONY_LOGE("Camera not turned on");
    return CALL_ERR_CAMERA_NOT_TURNED_ON;
}

bool VideoControlManager::ContainCameraID(std::string id)
{
    bool bRet = false;
#ifdef ABILITY_CAMERA_SUPPORT
    using namespace OHOS::CameraStandard;
    sptr<CameraManager> camManagerObj = CameraManager::GetInstance();
    std::vector<sptr<CameraStandard::CameraDevice>> cameraObjList = camManagerObj->GetSupportedCameras();

    for (auto &it : cameraObjList) {
        if (id.compare(it->GetID()) == 0) {
            bRet = true;
            TELEPHONY_LOGI("Contain Camera ID:  : %{public}s", id.c_str());
            break;
        }
    }
#endif
    return bRet;
}

bool VideoControlManager::IsPngFile(std::string fileName)
{
    size_t len = SUPPORT_PICTURE_EXT.length();
    if (fileName.length() <= len + 1) {
        TELEPHONY_LOGE("file not support: %{public}s", fileName.c_str());
        return false;
    }
    std::string ext = fileName.substr(fileName.length() - len, len);
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    if (!((ext == SUPPORT_PICTURE_EXT))) {
        TELEPHONY_LOGE("file not support: %{public}s", fileName.c_str());
        return false;
    }
    return true;
}

int32_t VideoControlManager::CancelCallUpgrade(int32_t callId)
{
    int32_t ret = TELEPHONY_ERR_FAIL;
    ret = CallPolicy::VideoCallPolicy(callId);
    if (ret != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("check prerequisites failed !");
        return ret;
    }
    sptr<CallBase> callPtr = CallObjectManager::GetOneCallObject(callId);
    if (callPtr == nullptr) {
        TELEPHONY_LOGE("the call object is nullptr, callId:%{public}d", callId);
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    sptr<IMSCall> netCall = reinterpret_cast<IMSCall *>(callPtr.GetRefPtr());
    ret = netCall->CancelCallUpgrade();
    if (ret != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("SetPreviewWindow failed!");
        return ret;
    }
    return TELEPHONY_SUCCESS;
}

int32_t VideoControlManager::RequestCameraCapabilities(int32_t callId)
{
    int32_t ret = TELEPHONY_ERR_FAIL;
    ret = CallPolicy::VideoCallPolicy(callId);
    if (ret != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("check prerequisites failed !");
        return ret;
    }
    sptr<CallBase> callPtr = CallObjectManager::GetOneCallObject(callId);
    if (callPtr == nullptr) {
        TELEPHONY_LOGE("the call object is nullptr, callId:%{public}d", callId);
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    sptr<IMSCall> netCall = reinterpret_cast<IMSCall *>(callPtr.GetRefPtr());
    ret = netCall->RequestCameraCapabilities();
    if (ret != TELEPHONY_SUCCESS) {
        TELEPHONY_LOGE("SetPreviewWindow failed!");
        return ret;
    }
    return TELEPHONY_SUCCESS;
}

bool VideoControlManager::CheckWindow(VideoWindow &window)
{
    if (window.width <= 0 || window.height <= 0) {
        TELEPHONY_LOGE("width or height value error");
        return false;
    }
    if (window.z != VIDEO_WINDOWS_Z_BOTTOM && window.z != VIDEO_WINDOWS_Z_TOP) {
        TELEPHONY_LOGE("z value error %{public}d", window.z);
        return false;
    }
    return true;
}
} // namespace Telephony
} // namespace OHOS
