#include "main.hpp"
#include "Hooks/Hooks.hpp"
#include "Hooks/SessionManagerAndExtendedPlayerHooks.hpp"
#include "UI/LobbySetupPanel.hpp"
#include "Config.hpp"

#include "CodegenExtensions/ColorUtility.hpp"

#include "GlobalNamespace/ConnectedPlayerManager.hpp"

#include "GlobalNamespace/MultiplayerLevelSelectionFlowCoordinator.hpp"

#include "beatsaber-hook/shared/utils/il2cpp-utils.hpp"
#include "custom-types/shared/register.hpp"
#include "questui/shared/QuestUI.hpp"
#include "questui/shared/BeatSaberUI.hpp"


//#include "GlobalNamespace/LobbySetupViewController.hpp"
//#include "GlobalNamespace/LobbySetupViewController_CannotStartGameReason.hpp"
using namespace GlobalNamespace;
using namespace System::Threading::Tasks;
using namespace MultiQuestensions;
using UnityEngine::ColorUtility;

#ifndef VERSION
#warning No Version set
#define VERSION "0.1.0"
#endif
#ifndef MPEX_PROTOCOL
#warning No MPEX_PROTOCOL set
#define MPEX_PROTOCOL "0.7.1"
#endif

ModInfo modInfo; // Stores the ID and version of our mod, and is sent to the modloader upon startup

namespace MultiQuestensions {
    // Loads the config from disk using our modInfo, then returns it for use
    Configuration& getConfig() {
        static Configuration config(modInfo);
        config.Load();
        return config;
    }

    // Loads the old config from disk using our modInfo, then returns it for use
    Configuration& getLegacyConfig() {
        static Configuration config({"multiquestensions", modInfo.version});
        config.Load();
        return config;
    }
}

// Returns a logger, useful for printing debug messages
Logger& getLogger() {
    static Logger* myLogger = new Logger(modInfo, LoggerOptions(false, true));
    return *myLogger;
}

Config MultiQuestensions::config;

namespace MultiQuestensions {
    // Plugin setup stuff
    GlobalNamespace::MultiplayerSessionManager* sessionManager;
    GlobalNamespace::LobbyPlayersDataModel* lobbyPlayersDataModel;
    // MultiQuestensions::PacketManager* packetManager;
    GlobalNamespace::LobbyGameStateController* lobbyGameStateController;

    LobbySetupViewController* lobbySetupView;

    bool AllPlayersModded() {
        getLogger().debug("MQE AllPlayersModded");
        for (int i = 0; i < sessionManager->dyn__connectedPlayers()->get_Count(); i++) {
            if (!sessionManager->dyn__connectedPlayers()->get_Item(i)->HasState("modded")) return false;
        }
        return true;
    }
}

// LobbyPlayersDataModel Activate
MAKE_HOOK_MATCH(LobbyPlayersActivate, &LobbyPlayersDataModel::Activate, void, LobbyPlayersDataModel* self) {
    lobbyPlayersDataModel = self;
    LobbyPlayersActivate(lobbyPlayersDataModel);
}

MAKE_HOOK_MATCH(LobbySetupViewController_DidActivate, &LobbySetupViewController::DidActivate, void, LobbySetupViewController* self, bool firstActivation, bool addedToHierarchy, bool screenSystemEnabling) {
    lobbySetupView = self;
    LobbySetupViewController_DidActivate(self, firstActivation, addedToHierarchy, screenSystemEnabling);
    if (firstActivation) {
        MultiQuestensions::UI::LobbySetupPanel::AddSetupPanel(self->get_rectTransform(), sessionManager, self);
    }
}

MAKE_HOOK_MATCH(LobbyGameStateController_Activate, &LobbyGameStateController::Activate, void, LobbyGameStateController* self) {
    lobbyGameStateController = self;
    LobbyGameStateController_Activate(self);
}

// Called at the early stages of game loading
extern "C" void setup(ModInfo& info) {
    info.id = ID;
    info.version = VERSION;
    modInfo = info;

    getLogger().info("Completed setup!");
}

// Called later on in the game loading - a good time to install function hooks
extern "C" void load() {
    il2cpp_functions::Init();

    // Initialize Config
    config.Initialize();

    // if (readConfig()) {
    //     getLogger().error("Config file is invalid.");
    //     saveDefaultConfig();
    // }

    custom_types::Register::AutoRegister();

    getLogger().info("Installing hooks...");
    Hooks::Install_Hooks();
    INSTALL_HOOK(getLogger(), LobbyPlayersActivate);

    INSTALL_HOOK(getLogger(), LobbyGameStateController_Activate);
    INSTALL_HOOK(getLogger(), LobbySetupViewController_DidActivate);
    getLogger().info("Installed all hooks!");
}