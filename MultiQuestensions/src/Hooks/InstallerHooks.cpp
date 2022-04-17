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
#include "GlobalNamespace/MainSettingsModelSO.hpp"
#include "Config.hpp"
using namespace GlobalNamespace;

namespace MultiQuestensions {
    MAKE_HOOK_MATCH(MultiplayerConnectedPlayerInstaller_InstallBindings, &MultiplayerConnectedPlayerInstaller::InstallBindings, void, MultiplayerConnectedPlayerInstaller* self) {
        self->dyn__sceneSetupData() = GameplayCoreSceneSetupData::New_ctor(
            self->dyn__sceneSetupData()->dyn_difficultyBeatmap(),
            self->dyn__sceneSetupData()->dyn_previewBeatmapLevel(),
            self->dyn__sceneSetupData()->dyn_gameplayModifiers()->CopyWith(
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
                System::Nullable_1<bool>(self->dyn__sceneSetupData()->dyn_gameplayModifiers()->dyn__zenMode() || config.LagReducer, true),
                System::Nullable_1<bool>()
                //zenMode: Plugin.Config.LagReducer
            ),
            self->dyn__sceneSetupData()->dyn_playerSpecificSettings(),
            self->dyn__sceneSetupData()->dyn_practiceSettings(),
            self->dyn__sceneSetupData()->dyn_useTestNoteCutSoundEffects(),
            self->dyn__sceneSetupData()->dyn_environmentInfo(),
            self->dyn__sceneSetupData()->dyn_colorScheme(),
            self->dyn__sceneSetupData()->dyn_mainSettingsModel()
        );

        MultiplayerConnectedPlayerInstaller_InstallBindings(self);
    }


    void Hooks::InstallerHooks() {
        INSTALL_HOOK(getLogger(), MultiplayerConnectedPlayerInstaller_InstallBindings);
    }

}