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

#ifndef DIA_CONFIG_INFO_H
#define DIA_CONFIG_INFO_H

#define DIA_SESSION_ID "SESSION_ID"
// 预制规则集
#define DIA_SELECT_RULE "SELECT_RULE"

#define DIA_DIFF_PRIVACY_ALGORITHM "DP_ALGORITHMS_TYPE"

/* 规则名称 */
// note:后续增加规则，需要考虑规则冲突，因为typelist可能会有多个
// 身份证 社保号 居住证号 纳税人识别号 驾驶证号
#define DIA_PRC "PRC"
// 护照号
#define DIA_CHINA_RESIDENT_PASSPORT "CHINA_RESIDENT_PASSPORT"
// 军官证
#define DIA_MILITARY_IDENTITY_CARD_NUMBER "MILITARY_IDENTITY_CARD_NUMBER"
// 银行卡
#define DIA_BANK_CARD_NUMBER "BANK_CARD_NUMBER"
// 邮箱
#define DIA_EMAIL "EMAIL"
//手机号
#define DIA_PHONE_NUMBER "PHONE_NUMBER"
// 车牌号
#define DIA_LICENSE_PLATE_NUMBER "LICENSE_PLATE_NUMBER"
// 港澳通行证
#define DIA_PERMIT_LAND_TO_HM "PERMIT_LAND_TO_HM"
// 台湾通行证
#define DIA_PERMIT_LAND_TO_TW "PERMIT_LAND_TO_TW"
// 回乡证
#define DIA_PERMIT_HM_TO_LAND "PERMIT_HM_TO_LAND"
// 台胞证
#define DIA_PERMIT_TW_TO_LAND "PERMIT_TW_TO_LAND"
// 出生证明
#define DIA_BIRTH_CERTIFICATE "BIRTH_CERTIFICATE"
// 中国海员证
#define DIA_SEAFARER_PASSPORT "SEAFARER_PASSPORT"
// 警官证
#define DIA_POLICE_OFFICER_CARD "POLICE_OFFICER_CARD"

#endif // DIA_CONFIG_INFO_H