/*
 * Copyright (C) 2025 Huawei Device Co., Ltd.
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
#define private public
#define protected public

#include <gtest/gtest.h>

#include "accesstoken_kit.h"
#include "audio_control_manager.h"
#include "audio_device_manager.h"
#include "audio_proxy.h"
#include "bluetooth_call_client.h"
#include "bluetooth_call_service.h"
#include "call_ability_callback.h"
#include "call_ability_report_proxy.h"
#include "call_control_manager.h"
#include "ims_call.h"
#include "voip_call.h"
#include "incoming_flash_reminder.h"
#include "nativetoken_kit.h"
#include "nearlink_call_client.h"
#include "telephony_errors.h"
#include "token_setproc.h"
#ifdef ABILITY_CAMERA_FRAMEWORK_SUPPORT
#include "input/camera_manager.h"
#endif
#ifdef ABILITY_SCREENLOCKMGR_SUPPORT
#include "screenlock_manager.h"
#endif
#include "call_state_report_proxy.h"

namespace OHOS::Telephony {
using namespace testing::ext;
constexpr int WAIT_TIME = 3;
constexpr int32_t CRS_TYPE = 2;
class ZeroBranch10Test : public testing::Test {
public:
    void SetUp();
    void TearDown();
    static void SetUpTestCase();
    static void TearDownTestCase();
};
void ZeroBranch10Test::SetUp() {}

void ZeroBranch10Test::TearDown() {}

void ZeroBranch10Test::SetUpTestCase()
{
    const char *perms[1] = {
        "ohos.permission.SET_TELEPHONY_STATE",
    };
    NativeTokenInfoParams infoInstance = {
        .dcapsNum = 0,
        .permsNum = 1,
        .aclsNum = 0,
        .dcaps = nullptr,
        .perms = perms,
        .acls = nullptr,
        .processName = "ZeroBranch10Test",
        .aplStr = "system_basic",
    };
    uint64_t tokenId = GetAccessTokenId(&infoInstance);
    SetSelfTokenID(tokenId);
    auto result = Security::AccessToken::AccessTokenKit::ReloadNativeTokenInfo();
    EXPECT_EQ(result, Security::AccessToken::RET_SUCCESS);
}
    
void ZeroBranch10Test::TearDownTestCase()
{
    sleep(WAIT_TIME);
}

using namespace Security::AccessToken;
using Security::AccessToken::AccessTokenID;

inline HapInfoParams testInfoParams = {
    .bundleName = "ZeroBranch10Test",
    .userID = 1,
    .instIndex = 0,
    .appIDDesc = "test",
    .isSystemApp = true,
};

inline PermissionDef testPermSetTelephonyStateDef = {
    .permissionName = "ohos.permission.SET_TELEPHONY_STATE",
    .bundleName = "ZeroBranch10Test",
    .grantMode = 1, // SYSTEM_GRANT
    .label = "label",
    .labelId = 1,
    .description = "Test call manager",
    .descriptionId = 1,
    .availableLevel = APL_SYSTEM_BASIC,
};

inline PermissionStateFull testSetTelephonyState = {
    .grantFlags = { 2 }, // PERMISSION_USER_SET
    .grantStatus = { PermissionState::PERMISSION_GRANTED },
    .isGeneral = true,
    .permissionName = "ohos.permission.SET_TELEPHONY_STATE",
    .resDeviceID = { "local" },
};

inline HapPolicyParams testPolicyParams = {
    .apl = APL_SYSTEM_BASIC,
    .domain = "test.domain",
    .permList = { testPermSetTelephonyStateDef },
    .permStateList = { testSetTelephonyState },
};

class AccessToken {
public:
    AccessToken()
    {
        currentID_ = GetSelfTokenID();
        AccessTokenIDEx tokenIdEx = AccessTokenKit::AllocHapToken(testInfoParams, testPolicyParams);
        accessID_ = tokenIdEx.tokenIdExStruct.tokenID;
        SetSelfTokenID(tokenIdEx.tokenIDEx);
    }
    ~AccessToken()
    {
        AccessTokenKit::DeleteToken(accessID_);
        SetSelfTokenID(currentID_);
    }
private:
    AccessTokenID currentID_ = 0;
    AccessTokenID accessID_ = 0;
};

/**
 * @tc.number   Telephony_NearlinkCallClient_001
 * @tc.name     test NearlinkCallClient
 * @tc.desc     Function test
 */
HWTEST_F(ZeroBranch10Test, Telephony_NearlinkCallClient_001, TestSize.Level1)
{
    NearlinkCallClient &client = DelayedRefSingleton<NearlinkCallClient>::GetInstance();
    client.UnInit();
    EXPECT_EQ(client.RegisterCallBack(nullptr), TELEPHONY_ERR_UNINIT);
    EXPECT_EQ(client.UnRegisterCallBack(), TELEPHONY_ERR_UNINIT);
    EXPECT_EQ(client.AnswerCall(), TELEPHONY_ERR_UNINIT);
    EXPECT_EQ(client.RejectCall(), TELEPHONY_ERR_UNINIT);
    EXPECT_EQ(client.HangUpCall(), TELEPHONY_ERR_UNINIT);
    EXPECT_EQ(client.GetCallState(), TELEPHONY_ERR_UNINIT);
    EXPECT_TRUE(client.GetCurrentCallList(0).empty());
    EXPECT_EQ(client.AddAudioDevice("", ""), TELEPHONY_ERR_UNINIT);
    EXPECT_EQ(client.RemoveAudioDevice(""), TELEPHONY_ERR_UNINIT);
    EXPECT_EQ(client.ResetNearlinkDeviceList(), TELEPHONY_ERR_UNINIT);
    client.Init();
    EXPECT_NE(client.RegisterCallBack(nullptr), TELEPHONY_ERR_UNINIT);
    EXPECT_NE(client.UnRegisterCallBack(), TELEPHONY_ERR_UNINIT);
    EXPECT_NE(client.AnswerCall(), TELEPHONY_ERR_UNINIT);
    EXPECT_NE(client.RejectCall(), TELEPHONY_ERR_UNINIT);
    EXPECT_NE(client.HangUpCall(), TELEPHONY_ERR_UNINIT);
    EXPECT_NE(client.GetCallState(), TELEPHONY_ERR_UNINIT);
    EXPECT_TRUE(client.GetCurrentCallList(0).empty());
    EXPECT_NE(client.AddAudioDevice("", ""), TELEPHONY_ERR_UNINIT);
    EXPECT_NE(client.RemoveAudioDevice(""), TELEPHONY_ERR_UNINIT);
    EXPECT_NE(client.ResetNearlinkDeviceList(), TELEPHONY_ERR_UNINIT);
    client.UnInit();
}

/**
 * @tc.number   Telephony_BluetoothCallClient_001
 * @tc.name     test BluetoothCallClient
 * @tc.desc     Function test
 */
HWTEST_F(ZeroBranch10Test, Telephony_BluetoothCallClient_001, TestSize.Level1)
{
    BluetoothCallClient &client = DelayedRefSingleton<BluetoothCallClient>::GetInstance();
    EXPECT_EQ(client.AddAudioDevice("", "", AudioDeviceType::DEVICE_BLUETOOTH_HEARING_AID), TELEPHONY_ERR_UNINIT);
    EXPECT_EQ(client.RemoveAudioDevice("", AudioDeviceType::DEVICE_BLUETOOTH_HEARING_AID), TELEPHONY_ERR_UNINIT);
    EXPECT_EQ(client.ResetHearingAidDeviceList(), TELEPHONY_ERR_UNINIT);
}

/**
 * @tc.number   Telephony_BluetoothCallService_001
 * @tc.name     test BluetoothCallService
 * @tc.desc     Function test
 */
HWTEST_F(ZeroBranch10Test, Telephony_BluetoothCallService_001, TestSize.Level1)
{
    AccessToken accessToken;
    auto bluetoothCallService = std::make_shared<BluetoothCallService>();
    MessageParcel data;
    MessageParcel reply;
    int32_t result = bluetoothCallService->OnAddAudioDeviceList(data, reply);
    EXPECT_EQ(result, TELEPHONY_ERR_ARGUMENT_INVALID);
    result = bluetoothCallService->OnRemoveAudioDeviceList(data, reply);
    EXPECT_EQ(result, TELEPHONY_ERR_ARGUMENT_INVALID);
    result = bluetoothCallService->OnResetNearlinkDeviceList(data, reply);
    EXPECT_NE(result, TELEPHONY_ERR_PERMISSION_ERR);
    result = bluetoothCallService->OnResetBtHearingAidDeviceList(data, reply);
    EXPECT_NE(result, TELEPHONY_ERR_PERMISSION_ERR);
    data.WriteString("addr");
    data.WriteInt32(static_cast<int32_t>(AudioDeviceType::DEVICE_NEARLINK));
    data.WriteString("name");
    result = bluetoothCallService->OnAddAudioDeviceList(data, reply);
    EXPECT_NE(result, TELEPHONY_ERR_ARGUMENT_INVALID);
    MessageParcel data1;
    data1.WriteString("addr");
    data1.WriteInt32(static_cast<int32_t>(AudioDeviceType::DEVICE_NEARLINK));
    result = bluetoothCallService->OnRemoveAudioDeviceList(data1, reply);
    EXPECT_NE(result, TELEPHONY_ERR_ARGUMENT_INVALID);
    result = bluetoothCallService->AddAudioDeviceList("addr",
        static_cast<int32_t>(AudioDeviceType::DEVICE_BLUETOOTH_HEARING_AID), "name");
    EXPECT_NE(result, TELEPHONY_ERR_ARGUMENT_INVALID);
    result = bluetoothCallService->AddAudioDeviceList("addr",
        static_cast<int32_t>(AudioDeviceType::DEVICE_EARPIECE), "name");
    EXPECT_EQ(result, TELEPHONY_ERR_ARGUMENT_INVALID);
    result = bluetoothCallService->RemoveAudioDeviceList("addr",
        static_cast<int32_t>(AudioDeviceType::DEVICE_BLUETOOTH_HEARING_AID));
    EXPECT_NE(result, TELEPHONY_ERR_ARGUMENT_INVALID);
    result = bluetoothCallService->RemoveAudioDeviceList("addr",
        static_cast<int32_t>(AudioDeviceType::DEVICE_EARPIECE));
    EXPECT_EQ(result, TELEPHONY_ERR_ARGUMENT_INVALID);
}

class MockCallManagerCallback : public CallManagerCallback {
public:
    explicit MockCallManagerCallback(int32_t value) : value_(value) {}
    int32_t OnCallDetailsChange(const CallAttributeInfo &info)
    {
        return value_;
    }

    int32_t OnMeeTimeDetailsChange(const CallAttributeInfo &info)
    {
        return value_;
    }

    int32_t OnCallEventChange(const CallEventInfo &info)
    {
        return value_;
    }

    int32_t OnCallDisconnectedCause(const DisconnectedDetails &details)
    {
        return value_;
    }

    int32_t OnReportAsyncResults(CallResultReportId reportId, AppExecFwk::PacMap &resultInfo)
    {
        return value_;
    }

    int32_t OnOttCallRequest(OttCallRequestId requestId, AppExecFwk::PacMap &info)
    {
        return value_;
    }

    int32_t OnReportMmiCodeResult(const MmiCodeInfo &info)
    {
        return value_;
    }

    int32_t OnReportAudioDeviceChange(const AudioDeviceInfo &info)
    {
        return value_;
    }

    int32_t OnReportPostDialDelay(const std::string &str)
    {
        return value_;
    }

    int32_t OnUpdateImsCallModeChange(const CallMediaModeInfo &imsCallModeInfo)
    {
        return value_;
    }

    int32_t OnCallSessionEventChange(const CallSessionEvent &callSessionEventOptions)
    {
        return value_;
    }

    int32_t OnPeerDimensionsChange(const PeerDimensionsDetail &peerDimensionsDetail)
    {
        return value_;
    }

    int32_t OnCallDataUsageChange(const int64_t dataUsage)
    {
        return value_;
    }

    int32_t OnUpdateCameraCapabilities(const CameraCapabilities &cameraCapabilities)
    {
        return value_;
    }

    int32_t OnPhoneStateChange(int32_t numActive, int32_t numHeld, int32_t callState,
        const std::string &number)
    {
        return value_;
    }
private:
    int32_t value_;
};

/**
 * @tc.number   Telephony_NearlinkCallReport_001
 * @tc.name     test NearlinkCall Report
 * @tc.desc     Function test
 */
HWTEST_F(ZeroBranch10Test, Telephony_NearlinkCallReport_001, TestSize.Level1)
{
    NearlinkCallClient &client = DelayedRefSingleton<NearlinkCallClient>::GetInstance();
    client.Init();
    EXPECT_NE(client.RegisterCallBack(std::make_unique<MockCallManagerCallback>(0)), TELEPHONY_ERR_UNINIT);
    EXPECT_NE(client.RegisterCallBack(std::make_unique<MockCallManagerCallback>(-1)), TELEPHONY_ERR_UNINIT);
    sptr<CallAbilityCallback> callback1 = new (std::nothrow) CallAbilityCallback();
    ASSERT_NE(callback1, nullptr);
    callback1->SetProcessCallback(std::make_unique<MockCallManagerCallback>(0));
    sptr<CallAbilityCallback> callback2 = new (std::nothrow) CallAbilityCallback();
    ASSERT_NE(callback2, nullptr);
    callback2->SetProcessCallback(std::make_unique<MockCallManagerCallback>(-1));
    DelayedSingleton<CallAbilityReportProxy>::GetInstance()->callbackPtrList_.emplace_back(callback1);
    DelayedSingleton<CallAbilityReportProxy>::GetInstance()->callbackPtrList_.emplace_back(callback2);
    DelayedSingleton<CallAbilityReportProxy>::GetInstance()->callbackPtrList_.emplace_back(nullptr);
    EXPECT_NE(DelayedSingleton<CallAbilityReportProxy>::GetInstance()->ReportPhoneStateChange(0, 0, 0, ""), 0);
    client.UnInit();
}

/**
 * @tc.number   Telephony_AudioControlManager_001
 * @tc.name     test AudioControlManager
 * @tc.desc     Function test
 */
HWTEST_F(ZeroBranch10Test, Telephony_AudioControlManager_001, TestSize.Level0)
{
    DialParaInfo info;
    sptr<CallBase> call = new IMSCall(info);
    call->SetCallType(CallType::TYPE_IMS);
    call->SetTelCallState(TelCallState::CALL_STATUS_DIALING);
    call->SetOriginalCallType(0);
    CallObjectManager::AddOneCallObject(call);
    AudioDeviceType initDeviceType = AudioDeviceType::DEVICE_NEARLINK;
    AudioDevice device;
    device.deviceType = AudioDeviceType::DEVICE_DISABLE;
    auto audioControlManager = DelayedSingleton<AudioControlManager>::GetInstance();
    audioControlManager->CheckTypeAndSetAudioDevice(call, VideoStateType::TYPE_VIDEO,
        VideoStateType::TYPE_VOICE, initDeviceType, device);
    audioControlManager->CheckTypeAndSetAudioDevice(call, VideoStateType::TYPE_VOICE,
        VideoStateType::TYPE_VIDEO, initDeviceType, device);
    call->SetTelCallState(TelCallState::CALL_STATUS_INCOMING);
    audioControlManager->CheckTypeAndSetAudioDevice(call, VideoStateType::TYPE_VIDEO,
        VideoStateType::TYPE_VOICE, initDeviceType, device);
    audioControlManager->CheckTypeAndSetAudioDevice(call, VideoStateType::TYPE_VOICE,
        VideoStateType::TYPE_VIDEO, initDeviceType, device);
    call->SetTelCallState(TelCallState::CALL_STATUS_ACTIVE);
    audioControlManager->CheckTypeAndSetAudioDevice(call, VideoStateType::TYPE_VIDEO,
        VideoStateType::TYPE_VOICE, initDeviceType, device);
    audioControlManager->CheckTypeAndSetAudioDevice(call, VideoStateType::TYPE_VOICE,
        VideoStateType::TYPE_VIDEO, initDeviceType, device);
    EXPECT_EQ(device.deviceType, AudioDeviceType::DEVICE_NEARLINK);
    initDeviceType = AudioDeviceType::DEVICE_BLUETOOTH_HEARING_AID;
    audioControlManager->CheckTypeAndSetAudioDevice(call, VideoStateType::TYPE_VOICE,
        VideoStateType::TYPE_VIDEO, initDeviceType, device);
    call->SetCallRunningState(CallRunningState::CALL_RUNNING_STATE_DIALING);
    call->SetVideoStateType(VideoStateType::TYPE_VOICE);
    audioControlManager->UpdateDeviceTypeForVideoDialing();
    call->SetVideoStateType(VideoStateType::TYPE_VIDEO);
    audioControlManager->UpdateDeviceTypeForVideoDialing();
    device.deviceType = AudioDeviceType::DEVICE_BLUETOOTH_HEARING_AID;
    audioControlManager->SetAudioDevice(device, false);
    device.deviceType = AudioDeviceType::DEVICE_EARPIECE;
    audioControlManager->SetAudioDevice(device, false);
    CallObjectManager::DeleteOneCallObject(call);
    EXPECT_TRUE(audioControlManager->IsExternalAudioDevice(AudioDeviceType::DEVICE_BLUETOOTH_HEARING_AID));
}

/**
 * @tc.number   Telephony_AudioControlManager_002
 * @tc.name     test AudioControlManager
 * @tc.desc     Function test
 */
HWTEST_F(ZeroBranch10Test, Telephony_AudioControlManager_002, TestSize.Level0)
{
    AudioDevice device;
    device.deviceType = AudioDeviceType::DEVICE_NEARLINK;
    auto audioControlManager = DelayedSingleton<AudioControlManager>::GetInstance();
    EXPECT_EQ(audioControlManager->HandleWirelessAudioDevice(device),
        CALL_ERR_AUDIO_SET_AUDIO_DEVICE_FAILED);
    device.deviceType = AudioDeviceType::DEVICE_BLUETOOTH_SCO;
    EXPECT_EQ(audioControlManager->HandleWirelessAudioDevice(device),
        CALL_ERR_AUDIO_SET_AUDIO_DEVICE_FAILED);
    device.deviceType = AudioDeviceType::DEVICE_BLUETOOTH_HEARING_AID;
    EXPECT_EQ(audioControlManager->HandleWirelessAudioDevice(device),
        CALL_ERR_AUDIO_SET_AUDIO_DEVICE_FAILED);
    strcpy_s(device.address, kMaxAddressLen, "1");
    device.deviceType = AudioDeviceType::DEVICE_NEARLINK;
    EXPECT_EQ(audioControlManager->HandleWirelessAudioDevice(device),
        CALL_ERR_AUDIO_SET_AUDIO_DEVICE_FAILED);
    device.deviceType = AudioDeviceType::DEVICE_BLUETOOTH_SCO;
    EXPECT_EQ(audioControlManager->HandleWirelessAudioDevice(device),
        CALL_ERR_AUDIO_SET_AUDIO_DEVICE_FAILED);
    device.deviceType = AudioDeviceType::DEVICE_BLUETOOTH_HEARING_AID;
    EXPECT_EQ(audioControlManager->HandleWirelessAudioDevice(device),
        CALL_ERR_AUDIO_SET_AUDIO_DEVICE_FAILED);
    audioControlManager->audioInterruptState_ = AudioInterruptState::INTERRUPT_STATE_ACTIVATED;
    EXPECT_NE(audioControlManager->GetInitAudioDeviceType(), AudioDeviceType::DEVICE_NEARLINK);
}

/**
 * @tc.number   Telephony_AudioDeviceManager_001
 * @tc.name     test AudioDeviceManager
 * @tc.desc     Function test
 */
HWTEST_F(ZeroBranch10Test, Telephony_AudioDeviceManager_001, TestSize.Level0)
{
    auto audioDeviceManager = DelayedSingleton<AudioDeviceManager>::GetInstance();
    AudioDevice device;
    device.deviceType = AudioDeviceType::DEVICE_BLUETOOTH_SCO;
    audioDeviceManager->info_.audioDeviceList.clear();
    audioDeviceManager->info_.audioDeviceList.push_back(device);
    audioDeviceManager->audioDeviceType_ = AudioDeviceType::DEVICE_BLUETOOTH_SCO;
    audioDeviceManager->ResetNearlinkAudioDevicesList();
    audioDeviceManager->ResetBtHearingAidDeviceList();
    device.deviceType = AudioDeviceType::DEVICE_NEARLINK;
    audioDeviceManager->info_.audioDeviceList.push_back(device);
    audioDeviceManager->audioDeviceType_ = AudioDeviceType::DEVICE_NEARLINK;
    audioDeviceManager->ResetNearlinkAudioDevicesList();
    EXPECT_EQ(audioDeviceManager->info_.audioDeviceList.size(), 1);
    audioDeviceManager->ReportAudioDeviceChange(device);
    device.deviceType = AudioDeviceType::DEVICE_BLUETOOTH_HEARING_AID;
    audioDeviceManager->info_.audioDeviceList.push_back(device);
    audioDeviceManager->audioDeviceType_ = AudioDeviceType::DEVICE_BLUETOOTH_HEARING_AID;
    audioDeviceManager->ResetBtHearingAidDeviceList();
    EXPECT_EQ(audioDeviceManager->info_.audioDeviceList.size(), 1);
    EXPECT_FALSE(audioDeviceManager->SwitchDevice(AudioDeviceType::DEVICE_NEARLINK));
    audioDeviceManager->info_.currentAudioDevice.deviceType = AudioDeviceType::DEVICE_BLUETOOTH_SCO;
    device.deviceType = AudioDeviceType::DEVICE_NEARLINK;
    audioDeviceManager->audioDeviceType_ = AudioDeviceType::DEVICE_NEARLINK;
    audioDeviceManager->ReportAudioDeviceChange(device);
    std::string address = "1";
    std::string deviceName = "1";
    audioDeviceManager->UpdateNearlinkDevice(address, deviceName);
    audioDeviceManager->UpdateBtDevice(address, deviceName);
    audioDeviceManager->UpdateBtHearingAidDevice(address, deviceName);
    address = "";
    audioDeviceManager->UpdateNearlinkDevice(address, deviceName);
    audioDeviceManager->UpdateBtDevice(address, deviceName);
    audioDeviceManager->UpdateBtHearingAidDevice(address, deviceName);
    address = "1";
    deviceName = "";
    audioDeviceManager->UpdateNearlinkDevice(address, deviceName);
    audioDeviceManager->UpdateBtDevice(address, deviceName);
    audioDeviceManager->UpdateBtHearingAidDevice(address, deviceName);
    EXPECT_EQ(audioDeviceManager->info_.currentAudioDevice.deviceType, AudioDeviceType::DEVICE_NEARLINK);
    EXPECT_TRUE(audioDeviceManager->ConvertAddress().empty());
    audioDeviceManager->info_.currentAudioDevice.deviceType = AudioDeviceType::DEVICE_BLUETOOTH_HEARING_AID;
    EXPECT_TRUE(audioDeviceManager->ConvertAddress().empty());
    EXPECT_FALSE(audioDeviceManager->SwitchDevice(AudioDeviceType::DEVICE_BLUETOOTH_HEARING_AID));
}

/**
 * @tc.number   Telephony_AudioDeviceManager_002
 * @tc.name     test AudioDeviceManager
 * @tc.desc     Function test
 */
HWTEST_F(ZeroBranch10Test, Telephony_AudioDeviceManager_002, TestSize.Level0)
{
    auto audioDeviceManager = DelayedSingleton<AudioDeviceManager>::GetInstance();
    AudioDevice deviceHearingAid = {
        .deviceType = AudioDeviceType::DEVICE_BLUETOOTH_HEARING_AID,
        .deviceName = "name",
        .address = "addr"
    };
    audioDeviceManager->info_.audioDeviceList.clear();
    audioDeviceManager->info_.audioDeviceList.push_back(deviceHearingAid);
    audioDeviceManager->UpdateBluetoothDeviceName("addr", "nameReplace");
    EXPECT_EQ(audioDeviceManager->info_.audioDeviceList.size(), 1);
    EXPECT_EQ(audioDeviceManager->info_.audioDeviceList.front().deviceName, "nameReplace");
}

/**
 * @tc.number   Telephony_AudioProxy_001
 * @tc.name     test AudioProxy
 * @tc.desc     Function test
 */
HWTEST_F(ZeroBranch10Test, Telephony_AudioProxy_001, TestSize.Level0)
{
    DelayedSingleton<AudioDeviceManager>::GetInstance()->audioDeviceType_ = AudioDeviceType::DEVICE_SPEAKER;
    std::vector<std::shared_ptr<AudioStandard::AudioDeviceDescriptor>> descs;
    std::make_shared<AudioPreferDeviceChangeCallback>()->OnPreferredOutputDeviceUpdated(descs);
    DialParaInfo info;
    sptr<CallBase> call = new IMSCall(info);
    call->SetTelCallState(TelCallState::CALL_STATUS_ACTIVE);
    call->SetCallType(CallType::TYPE_IMS);
    CallObjectManager::AddOneCallObject(call);
    std::make_shared<AudioPreferDeviceChangeCallback>()->OnPreferredOutputDeviceUpdated(descs);
    auto desc = std::make_shared<AudioStandard::AudioDeviceDescriptor>();
    desc->deviceType_ = AudioStandard::DEVICE_TYPE_BLUETOOTH_SCO;
    descs.push_back(desc);
    std::make_shared<AudioPreferDeviceChangeCallback>()->OnPreferredOutputDeviceUpdated(descs);
    EXPECT_EQ(DelayedSingleton<AudioDeviceManager>::GetInstance()->audioDeviceType_,
        AudioDeviceType::DEVICE_BLUETOOTH_SCO);
    descs.clear();
    desc->deviceType_ = AudioStandard::DEVICE_TYPE_NEARLINK;
    descs.push_back(desc);
    std::make_shared<AudioPreferDeviceChangeCallback>()->OnPreferredOutputDeviceUpdated(descs);
    EXPECT_EQ(DelayedSingleton<AudioDeviceManager>::GetInstance()->audioDeviceType_, AudioDeviceType::DEVICE_NEARLINK);
    descs.clear();
    desc->deviceType_ = AudioStandard::DEVICE_TYPE_EARPIECE;
    descs.push_back(desc);
    std::make_shared<AudioPreferDeviceChangeCallback>()->OnPreferredOutputDeviceUpdated(descs);
    EXPECT_EQ(DelayedSingleton<AudioDeviceManager>::GetInstance()->audioDeviceType_, AudioDeviceType::DEVICE_EARPIECE);
    descs.clear();
    desc->deviceType_ = AudioStandard::DEVICE_TYPE_SPEAKER;
    descs.push_back(desc);
    std::make_shared<AudioPreferDeviceChangeCallback>()->OnPreferredOutputDeviceUpdated(descs);
    EXPECT_EQ(DelayedSingleton<AudioDeviceManager>::GetInstance()->audioDeviceType_, AudioDeviceType::DEVICE_SPEAKER);
    descs.clear();
    desc->deviceType_ = AudioStandard::DEVICE_TYPE_WIRED_HEADSET;
    descs.push_back(desc);
    std::make_shared<AudioPreferDeviceChangeCallback>()->OnPreferredOutputDeviceUpdated(descs);
    EXPECT_EQ(DelayedSingleton<AudioDeviceManager>::GetInstance()->audioDeviceType_,
        AudioDeviceType::DEVICE_WIRED_HEADSET);
    descs.clear();
    desc->deviceType_ = AudioStandard::DEVICE_TYPE_WIRED_HEADPHONES;
    descs.push_back(desc);
    std::make_shared<AudioPreferDeviceChangeCallback>()->OnPreferredOutputDeviceUpdated(descs);
    EXPECT_EQ(DelayedSingleton<AudioDeviceManager>::GetInstance()->audioDeviceType_,
        AudioDeviceType::DEVICE_WIRED_HEADSET);
    descs.clear();
    desc->deviceType_ = AudioStandard::DEVICE_TYPE_USB_HEADSET;
    descs.push_back(desc);
    std::make_shared<AudioPreferDeviceChangeCallback>()->OnPreferredOutputDeviceUpdated(descs);
    EXPECT_EQ(DelayedSingleton<AudioDeviceManager>::GetInstance()->audioDeviceType_,
        AudioDeviceType::DEVICE_WIRED_HEADSET);
}

/**
 * @tc.number   Telephony_AudioProxy_002
 * @tc.name     test AudioProxy
 * @tc.desc     Function test
 */
HWTEST_F(ZeroBranch10Test, Telephony_AudioProxy_002, TestSize.Level0)
{
    DelayedSingleton<AudioDeviceManager>::GetInstance()->audioDeviceType_ = AudioDeviceType::DEVICE_SPEAKER;
    std::vector<std::shared_ptr<AudioStandard::AudioDeviceDescriptor>> descs;
    std::make_shared<AudioPreferDeviceChangeCallback>()->OnPreferredOutputDeviceUpdated(descs);
    DialParaInfo info;
    sptr<CallBase> call = new IMSCall(info);
    call->SetTelCallState(TelCallState::CALL_STATUS_ACTIVE);
    call->SetCallType(CallType::TYPE_IMS);
    CallObjectManager::AddOneCallObject(call);
    std::make_shared<AudioPreferDeviceChangeCallback>()->OnPreferredOutputDeviceUpdated(descs);
    auto desc = std::make_shared<AudioStandard::AudioDeviceDescriptor>();
    desc->deviceType_ = AudioStandard::DEVICE_TYPE_HEARING_AID;
    descs.push_back(desc);
    std::make_shared<AudioPreferDeviceChangeCallback>()->OnPreferredOutputDeviceUpdated(descs);
    EXPECT_EQ(DelayedSingleton<AudioDeviceManager>::GetInstance()->audioDeviceType_,
        AudioDeviceType::DEVICE_BLUETOOTH_HEARING_AID);
}

/**
 * @tc.number   Telephony_IncomingFlashReminder_001
 * @tc.name     test IncomingFlashReminder
 * @tc.desc     Function test
 */
HWTEST_F(ZeroBranch10Test, Telephony_IncomingFlashReminder_001, TestSize.Level1)
{
    DelayedSingleton<CallControlManager>::GetInstance()->incomingFlashReminder_ = nullptr;
    DelayedSingleton<CallControlManager>::GetInstance()->StartFlashRemind();
    DelayedSingleton<CallControlManager>::GetInstance()->StopFlashRemind();
    auto runner = AppExecFwk::EventRunner::Create("handler_incoming_flash_reminder");
    DelayedSingleton<CallControlManager>::GetInstance()->incomingFlashReminder_ =
        std::make_shared<IncomingFlashReminder>(runner, nullptr);
    DelayedSingleton<CallControlManager>::GetInstance()->StopFlashRemind();
    DelayedSingleton<CallControlManager>::GetInstance()->StartFlashRemind();
    DelayedSingleton<CallControlManager>::GetInstance()->StopFlashRemind();
    DelayedSingleton<CallControlManager>::GetInstance()->ClearFlashReminder();
    EXPECT_EQ(DelayedSingleton<CallControlManager>::GetInstance()->incomingFlashReminder_, nullptr);
}

/**
 * @tc.number   Telephony_IncomingFlashReminder_002
 * @tc.name     test IncomingFlashReminder
 * @tc.desc     Function test
 */
HWTEST_F(ZeroBranch10Test, Telephony_IncomingFlashReminder_002, TestSize.Level1)
{
    auto runner = AppExecFwk::EventRunner::Create("handler_incoming_flash_reminder");
    std::shared_ptr<IncomingFlashReminder> reminder = std::make_shared<IncomingFlashReminder>(runner, nullptr);
    reminder->isFlashRemindUsed_ = false;
    reminder->HandleStopFlashRemind();
    reminder->isFlashRemindUsed_ = true;
    reminder->HandleStopFlashRemind();
    EXPECT_FALSE(reminder->isFlashRemindUsed_);
    AppExecFwk::InnerEvent::Pointer event = AppExecFwk::InnerEvent::Get(0);
    reminder->ProcessEvent(event);
    event = AppExecFwk::InnerEvent::Get(1000001);
    reminder->ProcessEvent(event);
    event = AppExecFwk::InnerEvent::Get(1000002);
    reminder->ProcessEvent(event);
    EXPECT_TRUE(reminder->isFlashRemindUsed_);
    reminder->HandleStartFlashRemind();
    event = AppExecFwk::InnerEvent::Get(1000000);
    reminder->ProcessEvent(event);
    reminder->RemoveEvent(1000000);
    reminder->IsScreenStatusSatisfied();
    bool isTorchOk = false;
#ifdef ABILITY_CAMERA_FRAMEWORK_SUPPORT
    isTorchOk = (CameraStandard::CameraManager::GetInstance()->IsTorchSupported() &&
        CameraStandard::CameraManager::GetInstance()->GetTorchMode() != CameraStandard::TORCH_MODE_ON);
#endif
    EXPECT_EQ(reminder->IsTorchReady(), isTorchOk);
}

/**
 * @tc.number   Telephony_IncomingFlashReminder_003
 * @tc.name     test IncomingFlashReminder
 * @tc.desc     Function test
 */
HWTEST_F(ZeroBranch10Test, Telephony_IncomingFlashReminder_003, TestSize.Level1)
{
    auto runner = AppExecFwk::EventRunner::Create("handler_incoming_flash_reminder");
    DelayedSingleton<CallControlManager>::GetInstance()->incomingFlashReminder_ =
        std::make_shared<IncomingFlashReminder>(runner,
            []() {
                TELEPHONY_LOGI("clear flash reminder");
                DelayedSingleton<CallControlManager>::GetInstance()->ClearFlashReminder();
            }
        );
    DelayedSingleton<CallControlManager>::GetInstance()->incomingFlashReminder_->HandleStartFlashRemind();
    DelayedSingleton<CallControlManager>::GetInstance()->incomingFlashReminder_->isFlashRemindUsed_ = false;
    DelayedSingleton<CallControlManager>::GetInstance()->incomingFlashReminder_->HandleStopFlashRemind();
    EXPECT_EQ(DelayedSingleton<CallControlManager>::GetInstance()->incomingFlashReminder_, nullptr);
    DelayedSingleton<CallControlManager>::GetInstance()->incomingFlashReminder_ =
        std::make_shared<IncomingFlashReminder>(runner,
            []() {
                TELEPHONY_LOGI("clear flash reminder");
                DelayedSingleton<CallControlManager>::GetInstance()->ClearFlashReminder();
            }
        );
    DelayedSingleton<CallControlManager>::GetInstance()->incomingFlashReminder_->isFlashRemindUsed_ = true;
    DelayedSingleton<CallControlManager>::GetInstance()->incomingFlashReminder_->HandleStopFlashRemind();
    EXPECT_EQ(DelayedSingleton<CallControlManager>::GetInstance()->incomingFlashReminder_, nullptr);
}

/**
 * @tc.number   Telephony_MuteRinger_001
 * @tc.name     test MuteRinger
 * @tc.desc     Function test
 */
HWTEST_F(ZeroBranch10Test, Telephony_MuteRinger_001, TestSize.Level1)
{
    auto callControlManager = DelayedSingleton<CallControlManager>::GetInstance();
    auto audioControlManager = DelayedSingleton<AudioControlManager>::GetInstance();
    audioControlManager->isVideoRingVibrating_ = false;
    CallObjectManager::callObjectPtrList_.clear();
    DialParaInfo info;
    info.accountId = 0;
    sptr<CallBase> call1 = new VoIPCall(info);
    call1->SetCallType(CallType::TYPE_VOIP);
    call1->SetCallRunningState(CallRunningState::CALL_RUNNING_STATE_RINGING);
    callControlManager->VoIpCallState_ = CallStateToApp::CALL_STATE_UNKNOWN;
    CallObjectManager::AddOneCallObject(call1);
    audioControlManager->ringState_ = RingState::STOPPED;
    EXPECT_EQ(callControlManager->HasCall(), false);
    audioControlManager->MuteRinger();
    CallObjectManager::callObjectPtrList_.clear();
    audioControlManager->isVideoRingVibrating_ = true;
    EXPECT_EQ(audioControlManager->isVideoRingVibrating_, false);
    sptr<CallBase> call2 = new IMSCall(info);
    call2->SetCallType(CallType::TYPE_IMS);
    call2->SetCrsType(CRS_TYPE);
    callControlManager->VoIPCallState_ = CallStateToApp::CALL_STATE_UNKNOWN;
    CallObjectManager::AddOneCallObject(call2);
    audioControlManager->MuteRinger();
    CallObjectManager::callObjectPtrList_.clear();
    sptr<CallBase> call3 = new IMSCall(info);
    call2->SetCallType(CallType::TYPE_VOIP);
    callControlManager->VoIPCallState_ = CallStateToApp::CALL_STATE_ANSWERED;
    CallObjectManager::AddOneCallObject(call3);
    audioControlManager->MuteRinger();
}

/**
 * @tc.number   CallStateReportProxy_CallStateUpdated_001
 * @tc.name     test CallStateUpdated
 * @tc.desc     Function test
 */
HWTEST_F(ZeroBranch10Test, Telephony_MuteRinger_001, TestSize.Level1)
{
    auto callStateProxy = DelayedSingleton<CallStateReportProxy>::GetInstance();
    auto callControlManager = DelayedSingleton<CallControlManager>::GetInstance();
    DialParaInfo info;
    info.accountId = 0;
    sptr<CallBase> call1 = new VoIPCall(info);
    CallObjectManager::voipCallObjectList_.clear();
    call1->SetType(CallType::TYPE_VOIP);
    call1->SetCallRunningState(CallRunningState::CALL_RUNNING_STATE_RINGING);
    CallObjectManager::AddOneCallObject(call1);
    callStateProxy->CallStateUpdated(call1, TelCallState::CALL_STATUS_INCOMING, TelCallState::CALL_STATUS_INCOMING);
    CallAttributeInfo voipInfo;
    sptr<CallBase> calll2 = new VoIPCall(info);
    CallObjectManager::voipCallObjectList_[1] = voipInfo;
    callControlManager->VoIPCallState_ = CallStateToApp::CALL_STATE_ANSWERED;
    callStateProxy->CallStateUpdated(call1, TelCallState::CALL_STATUS_INCOMING, TelCallState::CALL_STATUS_INCOMING);
}
}