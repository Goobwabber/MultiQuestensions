#include "main.hpp"
#include "Hooks/Hooks.hpp"
#include "GlobalFields.hpp"

#include "GlobalNamespace/MasterServerQuickPlaySetupData_QuickPlaySongPacksOverride_PredefinedPack.hpp"
#include "GlobalNamespace/MasterServerQuickPlaySetupData_QuickPlaySongPacksOverride_LocalizedCustomPack.hpp"
#include "GlobalNamespace/MasterServerQuickPlaySetupData_QuickPlaySongPacksOverride_LocalizedCustomPackName.hpp"
#include "GlobalNamespace/MasterServerQuickPlaySetupData_QuickPlaySongPacksOverride.hpp"
#include "GlobalNamespace/MasterServerQuickPlaySetupData.hpp"
#include "GlobalNamespace/BloomFilterUtil.hpp"

//#include "CodegenExtensions/TempBloomFilterUtil.hpp"

//#include "GlobalNamespace/MasterServerQuickPlaySetupModel.hpp"
#include "GlobalNamespace/SongPackMaskModelSO.hpp"

#include "GlobalNamespace/MultiplayerModeSelectionFlowCoordinator.hpp"
#include "GlobalNamespace/JoinQuickPlayViewController.hpp"
#include "GlobalNamespace/SimpleDialogPromptViewController.hpp"
#include "GlobalNamespace/MultiplayerModeSettings.hpp"

#include "HMUI/ViewController_AnimationDirection.hpp"
#include "HMUI/ViewController_AnimationType.hpp"

#include "Polyglot/Localization.hpp"
#include "Polyglot/LanguageExtensions.hpp"

#include "GlobalNamespace/QuickPlaySongPacksDropdown.hpp"

using namespace GlobalNamespace;

using MSQSD_QPSPO_PredefinedPack = MasterServerQuickPlaySetupData::QuickPlaySongPacksOverride::PredefinedPack;
using MSQSD_QPSPO_LocalizedCustomPack = MasterServerQuickPlaySetupData::QuickPlaySongPacksOverride::LocalizedCustomPack;
using MSQD_QPSPO_LocalizedCustomPackName = MasterServerQuickPlaySetupData::QuickPlaySongPacksOverride::LocalizedCustomPackName;

bool gotSongPackOverrides = false;

namespace MultiQuestensions {
    // Check for our custom packs
    MAKE_HOOK_MATCH(QuickPlaySongPacksDropdown_LazyInit, &QuickPlaySongPacksDropdown::LazyInit, void, QuickPlaySongPacksDropdown* self) {
        gotSongPackOverrides = (self->dyn__quickPlaySongPacksOverride() != nullptr);
        QuickPlaySongPacksDropdown_LazyInit(self);
    }

    MAKE_HOOK_MATCH(MultiplayerModeSelectionFlowCoordinator_HandleJoinQuickPlayViewControllerDidFinish, &MultiplayerModeSelectionFlowCoordinator::HandleJoinQuickPlayViewControllerDidFinish, void, MultiplayerModeSelectionFlowCoordinator* self, bool success) {
        StringW levelPackName = self->dyn__joinQuickPlayViewController()->dyn__multiplayerModeSettings()->dyn_quickPlaySongPackMaskSerializedName();
        if (success && 
            self->dyn__songPackMaskModel()->ToSongPackMask(
                levelPackName
            ).Contains(
                BloomFilterUtil::ToBloomFilter<BitMask128>(getCustomLevelSongPackMaskStr(), 2, 13))
            ) {
            self->dyn__simpleDialogPromptViewController()->Init(
                "Custom Song Quickplay",
                "<color=#EB4949>This category includes songs of varying difficulty.\nIt may be more enjoyable to play in a private lobby with friends.",
                "Continue",
                "Cancel",
                il2cpp_utils::MakeDelegate<System::Action_1<int>*>(classof(System::Action_1<int>*), (std::function<void(int)>)[self, success](int btnId) {
                    switch (btnId)
                    {
                    default:
                    case 0: // Continue
                        MultiplayerModeSelectionFlowCoordinator_HandleJoinQuickPlayViewControllerDidFinish(self, success);
                        return;
                    case 1: // Cancel
                        //self->DismissViewController(self->dyn__simpleDialogPromptViewController(), HMUI::ViewController::AnimationDirection::Vertical, nullptr, false);
                        self->ReplaceTopViewController(self->dyn__joinQuickPlayViewController(), nullptr, HMUI::ViewController::AnimationType::In, HMUI::ViewController::AnimationDirection::Vertical);
                        return;
                    }
                    }
                )
            );
            self->ReplaceTopViewController(self->dyn__simpleDialogPromptViewController(), nullptr, HMUI::ViewController::AnimationType::In, HMUI::ViewController::AnimationDirection::Vertical);
        } else MultiplayerModeSelectionFlowCoordinator_HandleJoinQuickPlayViewControllerDidFinish(self, success);
    }

    //MAKE_HOOK_MATCH(MasterServerQuickPlaySetupModel_GetQuickPlaySetupInternal, &MasterServerQuickPlaySetupModel::GetQuickPlaySetupInternal, System::Threading::Tasks::Task_1<GlobalNamespace::MasterServerQuickPlaySetupData*>*, MasterServerQuickPlaySetupModel* self) {
    //    getLogger().debug("MasterServerQuickPlaySetupModel::GetQuickPlaySetupInternal");
    //    using T1_MasterServerQuickplaySetupData = System::Threading::Tasks::Task_1<GlobalNamespace::MasterServerQuickPlaySetupData*>*;
    //    T1_MasterServerQuickplaySetupData result = MasterServerQuickPlaySetupModel_GetQuickPlaySetupInternal(self);
    //    auto action = il2cpp_utils::MakeDelegate<System::Action_1<System::Threading::Tasks::Task*>*>(classof(System::Action_1<System::Threading::Tasks::Task*>*), (std::function<void()>)[result] {
    //        MasterServerQuickPlaySetupData* data = result->get_ResultOnSuccess();
    //        if (data) {
    //            MasterServerQuickPlaySetupData::QuickPlaySongPacksOverride* overrides = data->dyn_quickPlayAvailablePacksOverride();
    //            getLogger().debug("MasterServerQuickplaySetupData: hasOverride: '%s', Check _quickPlayAvailablePacksOverride Pointer: '%p'", data->get_hasOverride() ? "true" : "false", overrides);
    //            using PredefinedPack = MasterServerQuickPlaySetupData::QuickPlaySongPacksOverride::PredefinedPack;
    //            for (int i = 0; i < overrides->dyn_predefinedPackIds()->get_Count(); i++) {
    //                PredefinedPack* pack = overrides->dyn_predefinedPackIds()->get_Item(i);
    //                getLogger().debug("predefinedPackIds: order: '%d', packId: '%s'", pack->dyn_order(), to_utf8(csstrtostr(pack->dyn_packId())).c_str());
    //            }
    //            using LocalizedCustomPack = MasterServerQuickPlaySetupData::QuickPlaySongPacksOverride::LocalizedCustomPack;
    //            for (int i = 0; i < overrides->dyn_localizedCustomPacks()->get_Count(); i++) {
    //                LocalizedCustomPack* pack = overrides->dyn_localizedCustomPacks()->get_Item(i);
    //                getLogger().debug("predefinedPackIds: order: '%d', serializedName: '%s'", pack->dyn_order(), to_utf8(csstrtostr(pack->dyn_serializedName())).c_str());
    //                for (int j = 0; j < pack->dyn_packIds()->get_Count(); j++) {
    //                    getLogger().debug("predefinedPackIds packId '%d': packId: '%s'", j, to_utf8(csstrtostr(pack->dyn_packIds()->get_Item(j))).c_str());
    //                }
    //            }
    //        }
    //        else {
    //            getLogger().debug("MasterServerQuickPlaySetupModel::GetQuickPlaySetupInternal: Task does not have Results");
    //        }
    //        }
    //    );
    //    reinterpret_cast<System::Threading::Tasks::Task*>(result)->ContinueWith(action);

    //    return result;
    //}

    void Hooks::QuickplayHooks() {
        INSTALL_HOOK(getLogger(), QuickPlaySongPacksDropdown_LazyInit);
        INSTALL_HOOK(getLogger(), MultiplayerModeSelectionFlowCoordinator_HandleJoinQuickPlayViewControllerDidFinish);
        //INSTALL_HOOK(getLogger(), MasterServerQuickPlaySetupModel_GetQuickPlaySetupInternal);
    }
}

