/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#ifndef ANTI_FRAUD_SERVICE_CLIENT_TYPE_H
#define ANTI_FRAUD_SERVICE_CLIENT_TYPE_H

#include <string>
#include <cstdint>

#include "parcel.h"
#include "afs_detect_type.h"

namespace OHOS::AntiFraudService {
enum VoiceType {
    VOIP_CALL = 0,
    VOICE_CALL = 1,
    UNKNOWN = 2
};

enum ModelTypeBit {
    ANTIFRAUD_DETECT_TYPE_VOICE = 0b1,
    ANTIFRAUD_DETECT_TYPE_SPEECH = 0b100,
    ANTIFRAUD_DETECT_TYPE_XOIP = 0b1000
};

struct AntiFraudResult {
    int errCode;
    bool result;
    int modelVersion;
    int fraudType;
    std::string voiceText;
    float pvalue;
};

struct AntiFraudResultExt {
    /**
     * Indicates the anti-fraud result of voice semantic detection.
     * - false：no risk
     * - true：risk
     */
    bool isVoiceSemanticFraud;
    /**
     * Indicates the anti-fraud result of speech synthesis detection.
     * - false：no risk
     * - true：risk
     */
    bool isSpeechSynthesisFraud;
    /**
     * Indicates the anti-fraud result of xoip call detection.
     * - false：no risk
     * - true：risk
     */
    float speechSynthesisProb;
    /**
     * Indicates the anti-fraud result of xoip call detection.
     * - false：no risk
     * - true：risk
     */
    bool isXoipFraud;
};

using VoiceDetectionResult = AntiFraudResult;

struct SpeechSynthesisResult {
    int errCode;
    bool result;
    int modelVersion;
    float pvalue;
};

struct Rect {
    uint32_t x;
    uint32_t y;
    uint32_t width;
    uint32_t height;
};

struct VideoDetectResult {
    int errCode;
    bool result;
    int modelVersion;
    float pvalue;
    Rect rect;
    int imgFd;
};

struct VoipCallTransferResult {
    int errCode;
    bool result;
    float pvalue;
};

struct PublicSecurityResult {
    int errCode;
    bool result;
    int modelVersion;
    float pvalue;
    int fraudClass;
};

struct StartDetectionResult {
    int errCode;
    VoiceDetectionResult voiceDetectionResult;
    SpeechSynthesisResult speechSynthesisResult;
    VideoDetectResult videoDetectResult;
    VoipCallTransferResult voipCallTransferResult;
    PublicSecurityResult publicSecurityResult;
};

struct InitializeResult {
    int errCode;
};

#ifdef DEBUG_MODE
struct Filter {
    double luminance;
    int contrast;
    double area;
};
#endif

struct DetectionResult {
    Rect rect;
    float detectionProbability;
    int modelVersion;
    bool detectionResult;
    int32_t result;
#ifdef DEBUG_MODE
    struct Filter info;
#endif
};

struct WebAntiFraudResult {
    int errCode;
    bool isCheckSuccess;
    int32_t hwCode;
    std::string mappingType;
    int32_t policy;
    int32_t childModePolicy;
    std::string url;
    std::string context;
    bool semanticIsFraud;
    float semanticFraudProb;
    int32_t semanticFraudType;
    bool isNoticeXiaoyi;
    std::string brandName;
    std::string subClassName;
    bool hasIcon;
    std::vector<std::string> detectedIcons;
    bool keywordSuccess;
    std::vector<std::string> hitKeywords;
};

struct DownloadResult {
    bool needNotify;
    int size;
    int status;
    int process;
};

using ListenerResult = std::variant<AntiFraudResult, InitializeResult, DetectionResult, DownloadResult,
    WebAntiFraudResult, StartDetectionResult, PublicSecurityResult>;
}

#endif