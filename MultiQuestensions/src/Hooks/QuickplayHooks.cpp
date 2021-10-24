#include "main.hpp"
#include "Hooks/Hooks.hpp"
#include "GlobalFields.hpp"

#include "GlobalNamespace/MasterServerQuickPlaySetupData_QuickPlaySongPacksOverride_PredefinedPack.hpp"
#include "GlobalNamespace/MasterServerQuickPlaySetupData_QuickPlaySongPacksOverride_LocalizedCustomPack.hpp"
#include "GlobalNamespace/MasterServerQuickPlaySetupData_QuickPlaySongPacksOverride_LocalizedCustomPackName.hpp"
#include "GlobalNamespace/MasterServerQuickPlaySetupData_QuickPlaySongPacksOverride.hpp"
#include "GlobalNamespace/MasterServerQuickPlaySetupData.hpp"

#include "GlobalNamespace/SongPackMaskModelSO.hpp"

#include "Polyglot/Localization.hpp"

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
            MSQD_QPSPO_LocalizedCustomPackName* custom_packName_En = MSQD_QPSPO_LocalizedCustomPackName::New_ctor();
            custom_packName_En->dyn_language() = il2cpp_utils::newcsstr("English");
            custom_packName_En->dyn_packName() = il2cpp_utils::newcsstr("All + Custom Levels");

            MSQD_QPSPO_LocalizedCustomPackName* custom_packName_De = MSQD_QPSPO_LocalizedCustomPackName::New_ctor();
            custom_packName_De->dyn_language() = il2cpp_utils::newcsstr("Deutsch");
            custom_packName_De->dyn_packName() = il2cpp_utils::newcsstr("Alle + Custom Levels");

            custom->dyn_localizedNames() = Array<GlobalNamespace::MasterServerQuickPlaySetupData::QuickPlaySongPacksOverride::LocalizedCustomPackName*>::New(
                { custom_packName_En, custom_packName_De }
            );
            custom->dyn_packIds()->Add(il2cpp_utils::newcsstr("custom_levelpack_CustomLevels"));
            //custom->dyn_packIds()->Add(self->dyn__songPackMaskModel()->ToSerializedName(SongPackMask::get_all()));

            //getLogger().debug("SongPackMask All serializedName: %s", to_utf8(csstrtostr(self->dyn__songPackMaskModel()->ToSerializedName(SongPackMask::get_all()))).c_str());

            self->dyn__quickPlaySongPacksOverride()->dyn_localizedCustomPacks()->Add(custom);
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

    void Hooks::QuickplayHooks() {
        INSTALL_HOOK(getLogger(), QuickPlaySongPacksDropdown_LazyInit);
    }
}

