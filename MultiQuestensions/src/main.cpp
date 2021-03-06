#include "include/main.hpp"
#include "include/Beatmaps/PreviewBeatmapPacket.hpp"
#include "include/Beatmaps/PreviewBeatmapStub.hpp"
#include "include/Packets/PacketManager.hpp"
#include "extern/beatsaber-hook/shared/utils/il2cpp-utils.hpp"
#include "custom-types/shared/register.hpp"

static ModInfo modInfo; // Stores the ID and version of our mod, and is sent to the modloader upon startup

// Loads the config from disk using our modInfo, then returns it for use
Configuration& getConfig() {
    static Configuration config(modInfo);
    config.Load();
    return config;
}

// Returns a logger, useful for printing debug messages
Logger& getLogger() {
    static Logger* myLogger = new Logger(modInfo, LoggerOptions(false, true));
    return *myLogger;
}



// Plugin setup stuff
GlobalNamespace::MultiplayerSessionManager* sessionManager;
GlobalNamespace::LobbyPlayersDataModel* lobbyPlayersDataModel;
MultiQuestensions::PacketManager* packetManager;

std::string moddedState = "modded";
std::string questState = "platformquest";
std::string customSongsState = "customsongs";
std::string enforceModsState = "enforcemods";
std::string beatmapDownloadedState = "beatmap_downloaded";


static void HandlePreviewBeatmapPacket(MultiplayerExtensions::Beatmaps::PreviewBeatmapPacket* packet, GlobalNamespace::IConnectedPlayer* player) {
    GlobalNamespace::IPreviewBeatmapLevel* localPreview = lobbyPlayersDataModel->beatmapLevelsModel->GetLevelPreviewForLevelId(packet->levelId);
    MultiQuestensions::Beatmaps::PreviewBeatmapStub* preview = new MultiQuestensions::Beatmaps::PreviewBeatmapStub();
    //MultiQuestensions::Beatmaps::PreviewBeatmapStub* preview = (MultiQuestensions::Beatmaps::PreviewBeatmapStub*)il2cpp_functions::object_new(MultiQuestensions::Beatmaps::PreviewBeatmapStub::klass);

    if (localPreview == nullptr) {
        preview->fromPacket(packet);
    }
    else {
        preview->fromPreview(localPreview);
    }

    if (player->get_isConnectionOwner()) {
        sessionManager->SetLocalPlayerState(il2cpp_utils::createcsstr(beatmapDownloadedState), preview->isDownloaded);
    }

    GlobalNamespace::BeatmapCharacteristicSO* characteristic = lobbyPlayersDataModel->beatmapCharacteristicCollection->GetBeatmapCharacteristicBySerializedName(packet->characteristic);
    lobbyPlayersDataModel->SetPlayerBeatmapLevel(player->get_userId(), preview, GlobalNamespace::BeatmapDifficulty(packet->difficulty), characteristic);
}


MAKE_HOOK_OFFSETLESS(SessionManagerStart, void, GlobalNamespace::MultiplayerSessionManager* self) {

    sessionManager = self;
    SessionManagerStart(sessionManager);
    packetManager = new MultiQuestensions::PacketManager(sessionManager);
    
    self->SetLocalPlayerState(il2cpp_utils::createcsstr(moddedState), true);
    self->SetLocalPlayerState(il2cpp_utils::createcsstr(questState), true);
    self->SetLocalPlayerState(il2cpp_utils::createcsstr(customSongsState), getConfig().config["customsongs"].GetBool());
    self->SetLocalPlayerState(il2cpp_utils::createcsstr(enforceModsState), getConfig().config["enforcemods"].GetBool());

    //packetManager->RegisterCallback<MultiplayerExtensions::Beatmaps::PreviewBeatmapPacket*>(HandlePreviewBeatmapPacket);
}



// LobbyPlayersDataModel Activate
MAKE_HOOK_OFFSETLESS(LobbyPlayersActivate, void, GlobalNamespace::LobbyPlayersDataModel* self) {
    lobbyPlayersDataModel = self;
    LobbyPlayersActivate(lobbyPlayersDataModel);
}

// LobbyPlayersDataModel SetLocalPlayerBeatmapLevel
MAKE_HOOK_OFFSETLESS(LobbyPlayersSetLocalBeatmap, void, GlobalNamespace::LobbyPlayersDataModel* self, Il2CppString* levelId, GlobalNamespace::BeatmapDifficulty beatmapDifficulty, GlobalNamespace::BeatmapCharacteristicSO* characteristic) {
    GlobalNamespace::IPreviewBeatmapLevel* localPreview = self->beatmapLevelsModel->GetLevelPreviewForLevelId(levelId);
    if (localPreview != nullptr) {
        getLogger().info("Local user selected song '", levelId, "'.");
        MultiQuestensions::Beatmaps::PreviewBeatmapStub preview = MultiQuestensions::Beatmaps::PreviewBeatmapStub();
        preview.fromPreview(localPreview);

        if (preview.levelHash != nullptr) {
            if (self->get_localUserId() == self->get_hostUserId()) {
                sessionManager->SetLocalPlayerState(il2cpp_utils::createcsstr(beatmapDownloadedState), true);
            }

            packetManager->Send(reinterpret_cast<LiteNetLib::Utils::INetSerializable*>(preview.GetPacket(characteristic->get_serializedName(), beatmapDifficulty)));
            self->menuRpcManager->SelectBeatmap(GlobalNamespace::BeatmapIdentifierNetSerializable::New_ctor(levelId, characteristic->get_serializedName(), beatmapDifficulty));
            self->SetPlayerBeatmapLevel(self->get_localUserId(), &preview, beatmapDifficulty, characteristic);
            return;
        }
    }
    LobbyPlayersSetLocalBeatmap(self, levelId, beatmapDifficulty, characteristic);
}

// LobbyPlayersDataModel HandleMenuRpcManagerSelectedBeatmap (DONT REMOVE THIS, without it a player's selected map will be cleared)
MAKE_HOOK_OFFSETLESS(LobbyPlayersSelectedBeatmap, void, GlobalNamespace::LobbyPlayersDataModel* self, Il2CppString* userId, GlobalNamespace::BeatmapIdentifierNetSerializable* beatmapId) {
    GlobalNamespace::IPreviewBeatmapLevel* localPreview = self->beatmapLevelsModel->GetLevelPreviewForLevelId(beatmapId->levelID);
    if (localPreview != nullptr) {
        LobbyPlayersSelectedBeatmap(self, userId, beatmapId);
    }
}



void saveDefaultConfig() {
    getLogger().info("Creating config file...");
    ConfigDocument& config = getConfig().config;

    if (config.HasMember("customsongs")) {
        getLogger().info("Config file already exists.");
        return;
    }

    config.RemoveAllMembers();
    config.SetObject();

    config["customsongs"].SetBool(true);
    config["enforcemods"].SetBool(true);

    getConfig().Write();
    getLogger().info("Config file created.");
}

// Called at the early stages of game loading
extern "C" void setup(ModInfo& info) {
    info.id = ID;
    info.version = VERSION;
    modInfo = info;

    getConfig().Load();
    saveDefaultConfig();

    getLogger().info("Completed setup!");
}

// Called later on in the game loading - a good time to install function hooks
extern "C" void load() {
    il2cpp_functions::Init();
    
    CRASH_UNLESS(custom_types::Register::RegisterType<MultiQuestensions::PacketSerializer>());
    CRASH_UNLESS(custom_types::Register::RegisterType<MultiplayerExtensions::Beatmaps::PreviewBeatmapPacket>());
    CRASH_UNLESS(custom_types::Register::RegisterType<MultiQuestensions::Beatmaps::PreviewBeatmapStub>());

    getLogger().info("Installing hooks...");
    INSTALL_HOOK_OFFSETLESS(getLogger(), SessionManagerStart, il2cpp_utils::FindMethodUnsafe("", "MultiplayerSessionManager", "Start", 0));
    INSTALL_HOOK_OFFSETLESS(getLogger(), LobbyPlayersActivate, il2cpp_utils::FindMethodUnsafe("", "LobbyPlayersDataModel", "Activate", 0));
    INSTALL_HOOK_OFFSETLESS(getLogger(), LobbyPlayersSetLocalBeatmap, il2cpp_utils::FindMethodUnsafe("", "LobbyPlayersDataModel", "SetLocalPlayerBeatmapLevel", 3));
    INSTALL_HOOK_OFFSETLESS(getLogger(), LobbyPlayersSelectedBeatmap, il2cpp_utils::FindMethodUnsafe("", "LobbyPlayersDataModel", "HandleMenuRpcManagerSelectedBeatmap", 2));
    

    getLogger().info("Installed all hooks!");
}