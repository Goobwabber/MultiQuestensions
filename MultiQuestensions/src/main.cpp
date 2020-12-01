#include "main.hpp"
#include "beatmaps.hpp"
#include "packets.hpp"

static ModInfo modInfo; // Stores the ID and version of our mod, and is sent to the modloader upon startup

// Loads the config from disk using our modInfo, then returns it for use
Configuration& getConfig() {
    static Configuration config(modInfo);
    config.Load();
    return config;
}

// Returns a logger, useful for printing debug messages
const Logger& getLogger() {
    static const Logger logger(modInfo);
    return logger;
}



// Plugin setup stuff
static GlobalNamespace::MultiplayerSessionManager* sessionManager;
static GlobalNamespace::LobbyPlayersDataModel* lobbyPlayersDataModel;
static PacketManager* packetManager;

static Il2CppString* moddedState = createcsstr("modded", il2cpp_utils::StringType::Manual);
static Il2CppString* questState = createcsstr("platformquest", il2cpp_utils::StringType::Manual);
static Il2CppString* customSongsState = createcsstr("customsongs", il2cpp_utils::StringType::Manual);
static Il2CppString* enforceModsState = createcsstr("enforcemods", il2cpp_utils::StringType::Manual);

static Il2CppString* beatmapDownloadedState = createcsstr("beatmap_downloaded", il2cpp_utils::StringType::Manual);


static void HandlePreviewBeatmapPacket(PreviewBeatmapPacket* packet, GlobalNamespace::IConnectedPlayer* player) {
    GlobalNamespace::IPreviewBeatmapLevel* localPreview = lobbyPlayersDataModel->beatmapLevelsModel->GetLevelPreviewForLevelId(packet->levelId);
    PreviewBeatmapStub* preview;

    if (localPreview != nullptr) {
        preview = new PreviewBeatmapStub(localPreview);
    }
    else {
        preview = new PreviewBeatmapStub(packet);
    }

    if (player->get_isConnectionOwner()) {
        sessionManager->SetLocalPlayerState(beatmapDownloadedState, preview->isDownloaded);
    }

    GlobalNamespace::BeatmapCharacteristicSO* characteristic = lobbyPlayersDataModel->beatmapCharacteristicCollection->GetBeatmapCharacteristicBySerializedName(packet->characteristic);
    lobbyPlayersDataModel->SetPlayerBeatmapLevel(player->get_userId(), preview, packet->difficulty, characteristic);
}


MAKE_HOOK_OFFSETLESS(SessionManagerStart, void, GlobalNamespace::MultiplayerSessionManager* self) {
    SessionManagerStart(self);

    sessionManager = self;
    packetManager = new PacketManager(self);

    self->SetLocalPlayerState(moddedState, true);
    self->SetLocalPlayerState(questState, true);
    self->SetLocalPlayerState(customSongsState, getConfig().config["customsongs"].GetBool());
    self->SetLocalPlayerState(enforceModsState, getConfig().config["enforcemods"].GetBool());

    packetManager->RegisterCallback(HandlePreviewBeatmapPacket);
}



// LobbyPlayersDataModel Activate
MAKE_HOOK_OFFSETLESS(LobbyPlayersActivate, void, GlobalNamespace::LobbyPlayersDataModel* self) {
    LobbyPlayersActivate(self);
    lobbyPlayersDataModel = self;
}

// LobbyPlayersDataModel SetLocalPlayerBeatmapLevel
MAKE_HOOK_OFFSETLESS(LobbyPlayersSetLocalBeatmap, void, GlobalNamespace::LobbyPlayersDataModel* self, Il2CppString* levelId, GlobalNamespace::BeatmapDifficulty beatmapDifficulty, GlobalNamespace::BeatmapCharacteristicSO* characteristic) {
    GlobalNamespace::IPreviewBeatmapLevel* localPreview = self->beatmapLevelsModel->GetLevelPreviewForLevelId(levelId);
    if (localPreview != nullptr) {
        getLogger().info("Local user selected song '", levelId, "'.");
        PreviewBeatmapStub preview = PreviewBeatmapStub(localPreview);

        if (preview.levelHash != nullptr) {
            if (self->get_localUserId() == self->get_hostUserId()) {
                sessionManager->SetLocalPlayerState(beatmapDownloadedState, true);
            }

            packetManager->Send(preview.GetPacket(characteristic->get_serializedName(), beatmapDifficulty));
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

    getLogger().info("Installing hooks...");
    // Install our hooks (none defined yet)
    getLogger().info("Installed all hooks!");
}