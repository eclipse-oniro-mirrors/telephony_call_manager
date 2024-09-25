/*
 * Copyright (C) 2024 Huawei Device Co., Ltd.
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

#ifndef FOLD_STATUS_MANAGER
#define FOLD_STATUS_MANAGER

#include "singleton.h"
#include "display_manager.h"
#include "display_info.h"

namespace OHOS {
namespace Telephony {
class FoldStatusManager {
    DECLARE_DELAYED_SINGLETON(FoldStatusManager)
public:
    void RegisterFoldableListener();
    void UnregisterFoldableListener();
    static bool IsSmallFoldDevice();
    class FoldStatusListener : public Rosen::DisplayManager::IFoldStatusListener {
        public:
            FoldStatusListener() = default;
            ~FoldStatusListener() override = default;
            DISALLOW_COPY_AND_MOVE(FoldStatusListener);

            /**
            * @param FoldStatus uint32_t; UNKNOWN = 0, EXPAND = 1, FOLDED = 2, HALF_FOLD = 3;
            */
            void OnFoldStatusChanged(Rosen::FoldStatus foldStatus) override;
    };

    Rosen::FoldStatus mOldFoldStatus;

private:
    sptr<Rosen::DisplayManager::IFoldStatusListener> foldStatusListener_;
};
} // namespace Telephony
} // namespace OHOS
#endif // TELEPHONY_DISTRIBUTED_CALL_MANAGER_H
