#include "main.hpp"
#include "Hooks/Hooks.hpp"
#include "GlobalNamespace/MultiplayerConnectedPlayerBeatmapObjectEventManager.hpp"
#include "Config.hpp"
using namespace GlobalNamespace;

namespace MultiQuestensions {

    MAKE_HOOK_MATCH(MultiplayerConnectedPlayerBeatmapObjectEventManager_Start, &MultiplayerConnectedPlayerBeatmapObjectEventManager::Start, void, MultiplayerConnectedPlayerBeatmapObjectEventManager* self) {
        //getLogger().debug("MultiplayerConnectedPlayerBeatmapObjectEventManager_Start, checking if lag reducer is enabled");
        // getLogger().debug("ZenMode: '%s'", self->dyn__sceneSetupData()->dyn_gameplayModifiers()->dyn__zenMode() ? "true" : "false");
        if (!config.getLagReducer())
            MultiplayerConnectedPlayerBeatmapObjectEventManager_Start(self);
    }


    void Hooks::InstallerHooks() {
        INSTALL_HOOK(getLogger(), MultiplayerConnectedPlayerBeatmapObjectEventManager_Start);
    }

}