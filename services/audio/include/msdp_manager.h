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

#ifndef MSDP_MANAGER_H
#define MSDP_MANAGER_H

#include <cstdint>
#include <functional>
#include <map>
#include <vector>
#include <sstream>
#include "nocopyable.h"
#include "string_ex.h"
#include "parcel.h"

namespace OHOS {
namespace Telephony {
class UserStatusData : public Parcelable {
public:
    UserStatusData();
    virtual ~UserStatusData();

    uint32_t GetFeature() const;
    void SetFeature(uint32_t feature);

    std::string GetStatus() const;
    void SetStatus(const std::string &status);

    int32_t GetResult() const;
    void SetResult(int32_t result);

    int32_t GetErrorCode() const;
    void SetErrorCode(int32_t errorCode);

    std::vector<std::string> GetResultApps() const;
    void SetResultApps(const std::vector<std::string>& resultApps);

    std::string GetHpeDeviceId() const;
    void SetHpeDeviceId(const std::string &hpeDeviceId);

    int32_t GetCoordinateX() const;
    void SetCoordinateX(int32_t coordinateX);

    int32_t GetCoordinateY() const;
    void SetCoordinateY(int32_t coordinateY);

    int32_t GetPointerAction() const;
    void SetPointerAction(int32_t pointerAction);

    int32_t GetOrientation() const;
    void SetOrientation(int32_t orientation);

    virtual std::string Dump();
    virtual std::shared_ptr<UserStatusData> Unmarshalling(Parcel &parcel);
    static int32_t UnMarshallingFeature(Parcel &parcel);
    bool Marshalling(Parcel &parcel) const override;
    bool WriteTimeTunnelData(Parcel &parcel) const;

protected:
    std::string DumpBaseData();
    uint32_t feature_ { 0 };
    int32_t result_ { 0 };
    int32_t errorCode_ { 0 };
    std::string status_;
    int32_t coordinateX_ { 0 };
    int32_t coordinateY_ { 0 };
    int32_t pointerAction_ { 0 };
    int32_t orientation_ { 0 };
    virtual bool ReadFromParcel(Parcel &parcel);

private:
    std::vector<std::string> resultApps_;
    std::string hpeDeviceId_;
};
using UserStatusDataCallbackFunc = std::function<void(int32_t, std::shared_ptr<UserStatusData>)>;
} // namespace Telephony
} // namespace OHOS
#endif // MSDP_MANAGER_H