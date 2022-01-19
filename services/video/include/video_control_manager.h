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

#ifndef TELEPHONY_VIDEO_MANAGER_H
#define TELEPHONY_VIDEO_MANAGER_H

#include "singleton.h"

#include "call_manager_inner_type.h"

#include "call_state_listener_base.h"

namespace OHOS {
namespace Telephony {
class VideoControlManager : public std::enable_shared_from_this<VideoControlManager> {
    DECLARE_DELAYED_SINGLETON(VideoControlManager)
public:
    int32_t ControlCamera(std::u16string cameraId, int32_t callingUid, int32_t callingPid);
    int32_t SetPreviewWindow(VideoWindow &window);
    int32_t SetDisplayWindow(VideoWindow &window);
    int32_t SetCameraZoom(float zoomRatio);
    int32_t SetPausePicture(std::u16string path);
    int32_t SetDeviceDirection(int32_t rotation);

private:
    int32_t OpenCamera(std::u16string cameraId, int32_t callingUid, int32_t callingPid);
    int32_t CloseCamera(std::u16string cameraId, int32_t callingUid, int32_t callingPid);
    bool CheckWindow(VideoWindow &window);
    bool ContainCameraID(std::string id);
    bool IsPngFile(std::string fileName);
    bool isOpenCamera_;
};
} // namespace Telephony
} // namespace OHOS
#endif // TELEPHONY_VIDEO_MANAGER_H
