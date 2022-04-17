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
        MultiQuestensions::UI::LobbySetupPanel::AddSetupPanel(self->get_rectTransform(), sessionManager);
    }
}

MAKE_HOOK_MATCH(LobbyGameStateController_Activate, &LobbyGameStateController::Activate, void, LobbyGameStateController* self) {
    lobbyGameStateController = self;
    LobbyGameStateController_Activate(self);
}

void saveDefaultConfig() {
    getLogger().info("Creating config file...");
    ConfigDocument& configDoc = getConfig().config;

    configDoc.RemoveAllMembers();
    if (!configDoc.IsObject())
        configDoc.SetObject();
    auto& allocator = configDoc.GetAllocator();

    configDoc.AddMember("Hologram", config.Hologram, allocator);
    configDoc.AddMember("LagReducer", config.LagReducer, allocator);
    configDoc.AddMember("MissLighting", config.MissLighting, allocator);
    configDoc.AddMember("PlayerColor", ColorUtility::ToHtmlStringRGB_CPP(config.PlayerColor), allocator);
    configDoc.AddMember("MissColor", ColorUtility::ToHtmlStringRGB_CPP(config.MissColor), allocator);

    getConfig().Write();
    getLogger().info("Config file created.");
}

bool readConfig() {
    getLogger().info("Checking for legacy config file...");
    // Checking legacy config file
    ConfigDocument& legacyConfig = getLegacyConfig().config;
    if (legacyConfig.IsObject() && !legacyConfig.HasMember("Converted")) {
        // Legacy Conversion
        getLogger().info("Legacy config file found. Converting...");
        if (legacyConfig.HasMember("LagReducer") && legacyConfig["LagReducer"].IsBool()) {
            config.LagReducer = legacyConfig["LagReducer"].GetBool();
        }
        if (legacyConfig.HasMember("color") && legacyConfig["color"].IsString()) {
            ColorUtility::TryParseHtmlString(legacyConfig["color"].GetString(), config.PlayerColor);
        }
        legacyConfig.AddMember("Converted", true, legacyConfig.GetAllocator());
    } 
    // TODO: Delete config on a later version
    /*
    else if (legacyConfig.IsObject() && legacyConfig.HasMember("Converted")) {
        remove(Configuration::getConfigFilePath({"multiquestensions", modInfo.version}).c_str());
    }
    */

    getLogger().info("Reading config file...");
    ConfigDocument& configDoc = getConfig().config;
    bool parseError = false;
    if (configDoc.IsObject()) {



        (configDoc.HasMember("Hologram") && configDoc["Hologram"].IsBool()) ? (config.Hologram  = configDoc["Hologram"].GetBool()) : parseError = true;
        (configDoc.HasMember("LagReducer") && configDoc["LagReducer"].IsBool()) ? (config.LagReducer = configDoc["LagReducer"].GetBool()) : parseError = true;
        (configDoc.HasMember("MissLighting") && configDoc["MissLighting"].IsBool()) ? (config.LagReducer = configDoc["MissLighting"].GetBool()) : parseError = true;
        if (!(configDoc.HasMember("PlayerColor") && configDoc["PlayerColor"].IsString() &&
            ColorUtility::TryParseHtmlString(configDoc["PlayerColor"].GetString(), config.PlayerColor))) {
            parseError = true;
        }
        if (!(configDoc.HasMember("MissColor") && configDoc["MissColor"].IsString() &&
            ColorUtility::TryParseHtmlString(configDoc["MissColor"].GetString(), config.MissColor))) {
            parseError = true;
        }
    } else parseError = true;

    return parseError;
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

    if (readConfig()) {
        getLogger().error("Config file is invalid.");
        saveDefaultConfig();
    }

    custom_types::Register::AutoRegister();

    getLogger().info("Installing hooks...");
    Hooks::Install_Hooks();
    INSTALL_HOOK(getLogger(), LobbyPlayersActivate);

    INSTALL_HOOK(getLogger(), LobbyGameStateController_Activate);
    INSTALL_HOOK(getLogger(), LobbySetupViewController_DidActivate);
    getLogger().info("Installed all hooks!");
}