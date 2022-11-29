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
#include <algorithm>

#include "telephony_errors.h"

#include "telephony_log_wrapper.h"

#include "input/camera_manager.h"
#include "file_ex.h"

#include "cellular_call_connection.h"
#include "video_control_manager.h"

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

int32_t VideoControlManager::ControlCamera(std::u16string cameraId, int32_t callingUid, int32_t callingPid)
{
    if (cameraId.empty()) {
        return CloseCamera(cameraId, callingUid, callingPid);
    } else {
        return OpenCamera(cameraId, callingUid, callingPid);
    }
}

int32_t VideoControlManager::SetPreviewWindow(VideoWindow &window)
{
    return CALL_ERR_VIDEO_INVALID_COORDINATES;
}

int32_t VideoControlManager::SetDisplayWindow(VideoWindow &window)
{
    return CALL_ERR_VIDEO_INVALID_COORDINATES;
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

int32_t VideoControlManager::SetPausePicture(std::u16string path)
{
    std::string tempPath(Str16ToStr8(path));
    // param check
    if (FileExists(tempPath) && IsPngFile(tempPath)) {
        return DelayedSingleton<CellularCallConnection>::GetInstance()->SetPausePicture(path);
    } else {
        TELEPHONY_LOGE("invalid path");
        return CALL_ERR_INVALID_PATH;
    }
}

int32_t VideoControlManager::SetDeviceDirection(int32_t rotation)
{
    // param check
    if (rotation == CAMERA_ROTATION_0 || rotation == CAMERA_ROTATION_90 || rotation == CAMERA_ROTATION_180 ||
        rotation == CAMERA_ROTATION_270) {
        return DelayedSingleton<CellularCallConnection>::GetInstance()->SetDeviceDirection(rotation);
    }
    TELEPHONY_LOGE("error rotation:%{public}d", rotation);
    return CALL_ERR_VIDEO_INVALID_ROTATION;
}

int32_t VideoControlManager::OpenCamera(std::u16string cameraId, int32_t callingUid, int32_t callingPid)
{
    // cameraId check
    std::string id(Str16ToStr8(cameraId));
    bool bRet = ContainCameraID(id);
    if (!bRet) {
        TELEPHONY_LOGE("camera id is error!!");
        return CALL_ERR_VIDEO_INVALID_CAMERA_ID;
    }
    int32_t errCode =
        DelayedSingleton<CellularCallConnection>::GetInstance()->ControlCamera(cameraId, callingUid, callingPid);
    if (errCode == TELEPHONY_SUCCESS) {
        isOpenCamera_ = true;
    }
    return errCode;
}

int32_t VideoControlManager::CloseCamera(std::u16string cameraId, int32_t callingUid, int32_t callingPid)
{
    if (isOpenCamera_) {
        int32_t errCode = DelayedSingleton<CellularCallConnection>::GetInstance()->ControlCamera(
            cameraId, callingUid, callingPid);
        if (errCode == TELEPHONY_SUCCESS) {
            isOpenCamera_ = false;
        }
        return errCode;
    }
    TELEPHONY_LOGE("Camera not turned on");
    return CALL_ERR_CAMERA_NOT_TURNED_ON;
}

bool VideoControlManager::ContainCameraID(std::string id)
{
    using namespace OHOS::CameraStandard;
    sptr<CameraManager> camManagerObj = CameraManager::GetInstance();
    std::vector<sptr<CameraStandard::CameraDevice>> cameraObjList = camManagerObj->GetSupportedCameras();
    bool bRet = false;
    for (auto &it : cameraObjList) {
        if (id.compare(it->GetID()) == 0) {
            bRet = true;
            TELEPHONY_LOGI("Contain Camera ID:  : %{public}s", id.c_str());
            break;
        }
    }
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
