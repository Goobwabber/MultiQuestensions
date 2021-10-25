#include "main.hpp"
#include "Hooks/Hooks.hpp"
#include "GlobalFields.hpp"

#include "GlobalNamespace/MasterServerQuickPlaySetupData_QuickPlaySongPacksOverride_PredefinedPack.hpp"
#include "GlobalNamespace/MasterServerQuickPlaySetupData_QuickPlaySongPacksOverride_LocalizedCustomPack.hpp"
#include "GlobalNamespace/MasterServerQuickPlaySetupData_QuickPlaySongPacksOverride_LocalizedCustomPackName.hpp"
#include "GlobalNamespace/MasterServerQuickPlaySetupData_QuickPlaySongPacksOverride.hpp"
#include "GlobalNamespace/MasterServerQuickPlaySetupData.hpp"

#include "GlobalNamespace/SongPackMaskModelSO.hpp"

#include "GlobalNamespace/MultiplayerModeSelectionFlowCoordinator.hpp"
#include "GlobalNamespace/JoinQuickplayViewController.hpp"
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

namespace MultiQuestensions {
    // Add our custom Packs
    MAKE_HOOK_MATCH(QuickPlaySongPacksDropdown_LazyInit, &QuickPlaySongPacksDropdown::LazyInit, void, QuickPlaySongPacksDropdown* self) {
        if (!self->dyn__initialized()) {
            if (self->dyn__quickPlaySongPacksOverride() == nullptr) self->dyn__quickPlaySongPacksOverride() = GlobalNamespace::MasterServerQuickPlaySetupData::QuickPlaySongPacksOverride::New_ctor();
            self->dyn__quickPlaySongPacksOverride()->dyn_localizedCustomPacks() = System::Collections::Generic::List_1<GlobalNamespace::MasterServerQuickPlaySetupData::QuickPlaySongPacksOverride::LocalizedCustomPack*>::New_ctor();

            if (self->dyn__quickPlaySongPacksOverride()->dyn_predefinedPackIds() == nullptr) self->dyn__quickPlaySongPacksOverride()->dyn_predefinedPackIds() =
                System::Collections::Generic::List_1<GlobalNamespace::MasterServerQuickPlaySetupData::QuickPlaySongPacksOverride::PredefinedPack*>::New_ctor();

            //BUILT_IN_LEVEL_PACKS
            MSQSD_QPSPO_PredefinedPack* builtin = MSQSD_QPSPO_PredefinedPack::New_ctor();
            builtin->dyn_order() = 1;
            builtin->dyn_packId() = il2cpp_utils::newcsstr("BUILT_IN_LEVEL_PACKS");

            //ALL_LEVEL_PACKS
            MSQSD_QPSPO_PredefinedPack* all = MSQSD_QPSPO_PredefinedPack::New_ctor();
            all->dyn_order() = 3;
            all->dyn_packId() = il2cpp_utils::newcsstr("ALL_LEVEL_PACKS");

            self->dyn__quickPlaySongPacksOverride()->dyn_predefinedPackIds()->Add(builtin);
            self->dyn__quickPlaySongPacksOverride()->dyn_predefinedPackIds()->Add(all);


            MSQSD_QPSPO_LocalizedCustomPack* custom = MSQSD_QPSPO_LocalizedCustomPack::New_ctor();
            custom->dyn_order() = 2;
            //newPack->dyn_order() = self->dyn__quickPlaySongPacksOverride()->dyn_localizedCustomPacks()->get_Count() + 1;
            custom->dyn_serializedName() = il2cpp_utils::newcsstr("custom_levelpack_CustomLevels");

            MSQD_QPSPO_LocalizedCustomPackName* custom_packName_Default;
            custom_packName_Default = MSQD_QPSPO_LocalizedCustomPackName::New_ctor();
            custom_packName_Default->dyn_packName() = il2cpp_utils::newcsstr("All + Custom Levels");

            Polyglot::Language currentLang = Polyglot::Localization::get_Instance()->get_SelectedLanguage();
            custom_packName_Default->dyn_language() = Polyglot::LanguageExtensions::ToSerializedName(currentLang);

            custom->dyn_localizedNames() = Array<GlobalNamespace::MasterServerQuickPlaySetupData::QuickPlaySongPacksOverride::LocalizedCustomPackName*>::New(
                { custom_packName_Default }
            );
            custom->dyn_packIds()->Add(il2cpp_utils::newcsstr("custom_levelpack_CustomLevels"));
            //custom->dyn_packIds()->Add(self->dyn__songPackMaskModel()->ToSerializedName(SongPackMask::get_all()));

            //getLogger().debug("SongPackMask All serializedName: %s", to_utf8(csstrtostr(self->dyn__songPackMaskModel()->ToSerializedName(SongPackMask::get_all()))).c_str());

            self->dyn__quickPlaySongPacksOverride()->dyn_localizedCustomPacks()->Add(custom);

            //MSQSD_QPSPO_LocalizedCustomPack* test = MSQSD_QPSPO_LocalizedCustomPack::New_ctor();
            //test->dyn_order() = 4;
            ////newPack->dyn_order() = self->dyn__quickPlaySongPacksOverride()->dyn_localizedCustomPacks()->get_Count() + 1;
            //test->dyn_serializedName() = il2cpp_utils::newcsstr("test");
            //MSQD_QPSPO_LocalizedCustomPackName* test_packName_En = MSQD_QPSPO_LocalizedCustomPackName::New_ctor();
            //test_packName_En->dyn_language() = il2cpp_utils::newcsstr("en");
            //test_packName_En->dyn_packName() = il2cpp_utils::newcsstr("Test");

            //MSQD_QPSPO_LocalizedCustomPackName* test_packName_De = MSQD_QPSPO_LocalizedCustomPackName::New_ctor();
            //test_packName_De->dyn_language() = il2cpp_utils::newcsstr("de");
            //test_packName_De->dyn_packName() = il2cpp_utils::newcsstr("Test");

            //test->dyn_localizedNames() = Array<GlobalNamespace::MasterServerQuickPlaySetupData::QuickPlaySongPacksOverride::LocalizedCustomPackName*>::New(
            //    { test_packName_En, test_packName_De }
            //);
            //test->dyn_packIds()->Add(il2cpp_utils::newcsstr("OstVol1"));
            //test->dyn_packIds()->Add(il2cpp_utils::newcsstr("OstVol4"));
            //test->dyn_packIds()->Add(il2cpp_utils::newcsstr("PanicAtTheDisco"));



            //::Array<GlobalNamespace::IBeatmapLevelPack*>* ostAndExtraCollection = self->dyn__songPackMaskModel()->dyn__ostAndExtrasCollection()->get_beatmapLevelPacks();
            //for (int i = 0; i < ostAndExtraCollection->Length(); i++) {
            //    getLogger().debug("ostAndExtra Pack: '%s'", to_utf8(csstrtostr(ostAndExtraCollection->get(i)->get_packID())).c_str());
            //}

            //::Array<GlobalNamespace::IBeatmapLevelPack*>* dlcCollection = self->dyn__songPackMaskModel()->dyn__dlcCollection()->get_beatmapLevelPacks();
            //for (int i = 0; i < dlcCollection->Length(); i++) {
            //    getLogger().debug("dlc Pack: '%s'", to_utf8(csstrtostr(dlcCollection->get(i)->get_packID())).c_str());
            //}

            //self->dyn__quickPlaySongPacksOverride()->dyn_localizedCustomPacks()->Add(test);

        }

        //for (int i = 0; i < self->dyn__songPackMaskModel()->dyn__defaultSongPackMaskItems()->get_Count(); i++) {
        //    ::Il2CppString* pack = self->dyn__songPackMaskModel()->dyn__defaultSongPackMaskItems()->get_Item(i);
        //    getLogger().debug("defaultSongPackMaskItems name: %s", to_utf8(csstrtostr(pack)).c_str()/*, pack->dyn_order()*/);
        //    //for (int j = 0; j < pack->dyn_packIds()->get_Count(); j++) {
        //    //    getLogger().debug("packId: %s", to_utf8(csstrtostr(pack->dyn_packIds()->get_Item(j))).c_str());
        //    //}
        //}

       
        //for (int i = 0; i < self->dyn__songPackMaskModel()->dyn__defaultSongPackMaskItems()->get_Count(); i++) {
        //    ::Il2CppString* pack = self->dyn__songPackMaskModel()->dyn__defaultSongPackMaskItems()->get_Item(i);
        //    getLogger().debug("defaultSongPackMaskItems serializedName: %s", to_utf8(csstrtostr(pack)).c_str()/*, pack->dyn_order()*/);
        //    //for (int j = 0; j < pack->dyn_packIds()->get_Count(); j++) {
        //    //    getLogger().debug("packId: %s", to_utf8(csstrtostr(pack->dyn_packIds()->get_Item(j))).c_str());
        //    //}
        //}

        QuickPlaySongPacksDropdown_LazyInit(self);

        //for (int i = 0; i < self->dyn__quickPlaySongPacksOverride()->dyn_localizedCustomPacks()->get_Count(); i++) {
        //    MSQSD_QPSPO_LocalizedCustomPack* pack = self->dyn__quickPlaySongPacksOverride()->dyn_localizedCustomPacks()->get_Item(i);
        //    getLogger().debug("LocalizedPack serializedName: %s, order: %d", to_utf8(csstrtostr(pack->dyn_serializedName())).c_str(), pack->dyn_order());
        //    for (int j = 0; j < pack->dyn_packIds()->get_Count(); j++) {
        //        getLogger().debug("packId: %s", to_utf8(csstrtostr(pack->dyn_packIds()->get_Item(j))).c_str());
        //    }
        //}

    }

    MAKE_HOOK_MATCH(MultiplayerModeSelectionFlowCoordinator_HandleJoinQuickPlayViewControllerDidFinish, &MultiplayerModeSelectionFlowCoordinator::HandleJoinQuickPlayViewControllerDidFinish, void, MultiplayerModeSelectionFlowCoordinator* self, bool success) {
        
        if (success && 
            to_utf8(csstrtostr(self->dyn__joinQuickPlayViewController()->dyn__multiplayerModeSettings()->dyn_quickPlaySongPackMaskSerializedName())) == "custom_levelpack_CustomLevels") {
            self->dyn__simpleDialogPromptViewController()->Init(
                il2cpp_utils::newcsstr("Custom Song Quickplay"),
                il2cpp_utils::newcsstr("<color=#ff0000>This category includes songs of varying difficulty.\nIt may be more enjoyable to play in a private lobby with friends."),
                il2cpp_utils::newcsstr("Continue"),
                il2cpp_utils::newcsstr("Cancel"),
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

    void Hooks::QuickplayHooks() {
        INSTALL_HOOK(getLogger(), QuickPlaySongPacksDropdown_LazyInit);
        INSTALL_HOOK(getLogger(), MultiplayerModeSelectionFlowCoordinator_HandleJoinQuickPlayViewControllerDidFinish);
    }
}

