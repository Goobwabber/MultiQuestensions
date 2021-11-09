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
        self->dyn__sceneSetupData()->dyn_gameplayModifiers()->dyn__zenMode() |= getConfig().config["LagReducer"].GetBool();
        getLogger().debug("[Prefix] ZenMode is set to '%s'", self->dyn__sceneSetupData()->dyn_gameplayModifiers()->dyn__zenMode() ? "true" : "false");
        MultiplayerConnectedPlayerInstaller_InstallBindings(self);
        getLogger().debug("[Postfix] ZenMode is set to '%s'", self->dyn__sceneSetupData()->dyn_gameplayModifiers()->dyn__zenMode() ? "true" : "false");
    }

    void Hooks::InstallerHooks() {
        INSTALL_HOOK(getLogger(), MultiplayerConnectedPlayerInstaller_InstallBindings);
    }

}