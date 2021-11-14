#include "main.hpp"
#include "Hooks/Hooks.hpp"
#include "GlobalNamespace/MultiplayerConnectedPlayerInstaller.hpp"
#include "GlobalNamespace/GameplayCoreSceneSetupData.hpp"
#include "GlobalNamespace/GameplayModifiers.hpp"
#include "GlobalNamespace/IDifficultyBeatmap.hpp"
#include "GlobalNamespace/PlayerSpecificSettings.hpp"
#include "GlobalNamespace/PracticeSettings.hpp"
#include "GlobalNamespace/EnvironmentInfoSO.hpp"
#include "GlobalNamespace/ColorScheme.hpp"
using namespace GlobalNamespace;

namespace MultiQuestensions {
    MAKE_HOOK_MATCH(MultiplayerConnectedPlayerInstaller_InstallBindings, &MultiplayerConnectedPlayerInstaller::InstallBindings, void, MultiplayerConnectedPlayerInstaller* self) {
        getLogger().debug("LagReducer set to '%s'", getConfig().config["LagReducer"].GetBool() ? "true" : "false");
        //self->dyn__sceneSetupData()->dyn_gameplayModifiers()->dyn__zenMode() |= getConfig().config["LagReducer"].GetBool();

        self->dyn__sceneSetupData() = GameplayCoreSceneSetupData::New_ctor(
            self->dyn__sceneSetupData()->difficultyBeatmap,
            self->dyn__sceneSetupData()->previewBeatmapLevel,
            self->dyn__sceneSetupData()->gameplayModifiers->CopyWith(
                System::Nullable_1<bool>(), 
                System::Nullable_1<bool>(), 
                System::Nullable_1<GlobalNamespace::GameplayModifiers::EnergyType>(), 
                System::Nullable_1<bool>(), 
                System::Nullable_1<bool>(), 
                System::Nullable_1<bool>(), 
                System::Nullable_1<GlobalNamespace::GameplayModifiers::EnabledObstacleType>(), 
                System::Nullable_1<bool>(), 
                System::Nullable_1<bool>(), 
                System::Nullable_1<bool>(), 
                System::Nullable_1<bool>(), 
                System::Nullable_1<GlobalNamespace::GameplayModifiers::SongSpeed>(), 
                System::Nullable_1<bool>(), 
                System::Nullable_1<bool>(), 
                System::Nullable_1<bool>(), 
                System::Nullable_1<bool>(self->dyn__sceneSetupData()->dyn_gameplayModifiers()->dyn__zenMode() || getConfig().config["LagReducer"].GetBool(), true),
                System::Nullable_1<bool>()
                //zenMode: Plugin.Config.LagReducer
            ),
            self->dyn__sceneSetupData()->playerSpecificSettings,
            self->dyn__sceneSetupData()->practiceSettings,
            self->dyn__sceneSetupData()->useTestNoteCutSoundEffects,
            self->dyn__sceneSetupData()->environmentInfo,
            self->dyn__sceneSetupData()->colorScheme
        );

        MultiplayerConnectedPlayerInstaller_InstallBindings(self);
    }

    void Hooks::InstallerHooks() {
        INSTALL_HOOK(getLogger(), MultiplayerConnectedPlayerInstaller_InstallBindings);
    }

}