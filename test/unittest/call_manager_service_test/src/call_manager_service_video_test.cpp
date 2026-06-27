/*
 * Copyright (C) 2021-2024 Huawei Device Co., Ltd.
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

#include "call_manager_service_test_base.h"

namespace OHOS {
namespace Telephony {

/**
 * @tc.number   Telephony_CallManagerService_SetCameraZoom_0100
 * @tc.name     test SetCameraZoom with null callControlManagerPtr_
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerServiceTest, Telephony_CallManagerService_SetCameraZoom_0100, TestSize.Level1)
{
    SetCallControlManagerNull();
    int32_t ret = service_->SetCameraZoom(1.0f);
    EXPECT_EQ(ret, TELEPHONY_ERR_LOCAL_PTR_NULL);
}

/**
 * @tc.number   Telephony_CallManagerService_SetCameraZoom_0200
 * @tc.name     test SetCameraZoom with valid callControlManagerPtr_
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerServiceTest, Telephony_CallManagerService_SetCameraZoom_0200, TestSize.Level1)
{
    int32_t ret = service_->SetCameraZoom(2.0f);
    EXPECT_EQ(ret, TELEPHONY_ERR_LOCAL_PTR_NULL);
}

/**
 * @tc.number   Telephony_CallManagerService_ControlCamera_0100
 * @tc.name     test ControlCamera
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerServiceTest, Telephony_CallManagerService_ControlCamera_0100, TestSize.Level1)
{
    std::u16string cameraId = Str8ToStr16("0");
    int32_t ret = service_->ControlCamera(1, cameraId);
    EXPECT_NE(ret, TELEPHONY_ERR_PERMISSION_ERR);
}

/**
 * @tc.number   Telephony_CallManagerService_SetPreviewWindow_0100
 * @tc.name     test SetPreviewWindow
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerServiceTest, Telephony_CallManagerService_SetPreviewWindow_0100, TestSize.Level1)
{
    std::string surfaceId = "test_surface";
    sptr<Surface> surface = nullptr;
    int32_t ret = service_->SetPreviewWindow(1, surfaceId, surface);
    EXPECT_NE(ret, TELEPHONY_ERR_PERMISSION_ERR);
}

/**
 * @tc.number   Telephony_CallManagerService_SetDisplayWindow_0100
 * @tc.name     test SetDisplayWindow
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerServiceTest, Telephony_CallManagerService_SetDisplayWindow_0100, TestSize.Level1)
{
    std::string surfaceId = "test_surface";
    sptr<Surface> surface = nullptr;
    int32_t ret = service_->SetDisplayWindow(1, surfaceId, surface);
    EXPECT_NE(ret, TELEPHONY_ERR_PERMISSION_ERR);
}

/**
 * @tc.number   Telephony_CallManagerService_SetDisplayWindow_0200
 * @tc.name     test SetDisplayWindow with null videoControlManager
 * @tc.desc     Branch coverage: videoControlManager == nullptr path
 */
HWTEST_F(CallManagerServiceTest, Telephony_CallManagerService_SetDisplayWindow_0200, TestSize.Level1)
{
    std::string surfaceId = "test_surface_id";
    sptr<Surface> surface = nullptr;
    int32_t ret = service_->SetDisplayWindow(1, surfaceId, surface);
    EXPECT_NE(ret, TELEPHONY_ERR_LOCAL_PTR_NULL);
}

/**
 * @tc.number   Telephony_CallManagerService_SetPausePicture_0100
 * @tc.name     test SetPausePicture
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerServiceTest, Telephony_CallManagerService_SetPausePicture_0100, TestSize.Level1)
{
    std::u16string path = Str8ToStr16("/test/path");
    int32_t ret = service_->SetPausePicture(1, path);
    EXPECT_NE(ret, TELEPHONY_ERR_PERMISSION_ERR);
}

/**
 * @tc.number   Telephony_CallManagerService_SetPausePicture_0300
 * @tc.name     test SetPausePicture with null videoControlManager
 * @tc.desc     Branch coverage: videoControlManager == nullptr path
 */
HWTEST_F(CallManagerServiceTest, Telephony_CallManagerService_SetPausePicture_0300, TestSize.Level1)
{
    std::u16string path = Str8ToStr16("/test/path");
    int32_t ret = service_->SetPausePicture(1, path);
    EXPECT_NE(ret, TELEPHONY_ERR_LOCAL_PTR_NULL);
}

/**
 * @tc.number   Telephony_CallManagerService_SetDeviceDirection_0100
 * @tc.name     test SetDeviceDirection
 * @tc.desc     Function test
 */
HWTEST_F(CallManagerServiceTest, Telephony_CallManagerService_SetDeviceDirection_0100, TestSize.Level1)
{
    int32_t ret = service_->SetDeviceDirection(1, 90);
    EXPECT_NE(ret, TELEPHONY_ERR_PERMISSION_ERR);
}

/**
 * @tc.number   Telephony_CallManagerService_SetDeviceDirection_0200
 * @tc.name     test SetDeviceDirection with null videoControlManager
 * @tc.desc     Branch coverage: videoControlManager == nullptr path
 */
HWTEST_F(CallManagerServiceTest, Telephony_CallManagerService_SetDeviceDirection_0200, TestSize.Level1)
{
    int32_t ret = service_->SetDeviceDirection(1, 90);
    EXPECT_NE(ret, TELEPHONY_ERR_LOCAL_PTR_NULL);
}

/**
 * @tc.number   Telephony_CallManagerService_ControlCamera_NullManager_0200
 * @tc.name     test ControlCamera with null videoControlManager
 * @tc.desc     Branch coverage: videoControlManager == nullptr path
 */
HWTEST_F(CallManagerServiceTest, Telephony_CallManagerService_ControlCamera_NullManager_0200, TestSize.Level1)
{
    std::u16string cameraId = Str8ToStr16("camera_0");
    int32_t ret = service_->ControlCamera(1, cameraId);
    EXPECT_NE(ret, TELEPHONY_ERR_LOCAL_PTR_NULL);
}

} // namespace Telephony
} // namespace OHOS
