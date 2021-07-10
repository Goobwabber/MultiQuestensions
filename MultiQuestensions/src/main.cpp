#include "main.hpp"
#include "Beatmaps/PreviewBeatmapPacket.hpp"
#include "Beatmaps/PreviewBeatmapStub.hpp"
#include "Packets/PacketManager.hpp"
#include "beatsaber-hook/shared/utils/il2cpp-utils.hpp"
#include "custom-types/shared/register.hpp"
#include "beatsaber-hook/shared/utils/hooking.hpp"
using namespace GlobalNamespace;

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


static void HandlePreviewBeatmapPacket(MultiplayerExtensions::Beatmaps::PreviewBeatmapPacket* packet, IConnectedPlayer* player) {
    IPreviewBeatmapLevel* localPreview = lobbyPlayersDataModel->beatmapLevelsModel->GetLevelPreviewForLevelId(packet->levelId);

    MultiQuestensions::Beatmaps::PreviewBeatmapStub* preview;

    if (localPreview == nullptr) {
        preview = CRASH_UNLESS(il2cpp_utils::New<MultiQuestensions::Beatmaps::PreviewBeatmapStub*>(packet));
    }
    else {
        preview = CRASH_UNLESS(il2cpp_utils::New<MultiQuestensions::Beatmaps::PreviewBeatmapStub*>(localPreview));
    }

    if (player->get_isConnectionOwner()) {
        sessionManager->SetLocalPlayerState(il2cpp_utils::newcsstr(beatmapDownloadedState), preview->isDownloaded);
    }

    BeatmapCharacteristicSO* characteristic = lobbyPlayersDataModel->beatmapCharacteristicCollection->GetBeatmapCharacteristicBySerializedName(packet->characteristic);
    lobbyPlayersDataModel->SetPlayerBeatmapLevel(player->get_userId(), reinterpret_cast<IPreviewBeatmapLevel*>(preview), GlobalNamespace::BeatmapDifficulty(packet->difficulty), characteristic);
}


MAKE_HOOK_MATCH(SessionManagerStart, &MultiplayerSessionManager::Start, void, MultiplayerSessionManager* self) {

    sessionManager = self;
    SessionManagerStart(sessionManager);
    packetManager = new MultiQuestensions::PacketManager(sessionManager);
    
    self->SetLocalPlayerState(il2cpp_utils::newcsstr(moddedState), true);
    self->SetLocalPlayerState(il2cpp_utils::newcsstr(questState), true);
    self->SetLocalPlayerState(il2cpp_utils::newcsstr(customSongsState), getConfig().config["customsongs"].GetBool());
    self->SetLocalPlayerState(il2cpp_utils::newcsstr(enforceModsState), getConfig().config["enforcemods"].GetBool());

    packetManager->RegisterCallback<MultiplayerExtensions::Beatmaps::PreviewBeatmapPacket*>(HandlePreviewBeatmapPacket);
}



// LobbyPlayersDataModel Activate
MAKE_HOOK_MATCH(LobbyPlayersActivate, &LobbyPlayersDataModel::Activate, void, LobbyPlayersDataModel* self) {
    lobbyPlayersDataModel = self;
    LobbyPlayersActivate(lobbyPlayersDataModel);
}

// LobbyPlayersDataModel SetLocalPlayerBeatmapLevel
MAKE_HOOK_MATCH(LobbyPlayersSetLocalBeatmap, &LobbyPlayersDataModel::SetLocalPlayerBeatmapLevel, void, LobbyPlayersDataModel* self, Il2CppString* levelId, BeatmapDifficulty beatmapDifficulty, BeatmapCharacteristicSO* characteristic) {
    IPreviewBeatmapLevel* localPreview = self->beatmapLevelsModel->GetLevelPreviewForLevelId(levelId);
    if (localPreview != nullptr) {
        getLogger().info("Local user selected song '%s'.", to_utf8(csstrtostr(levelId)).data());
        MultiQuestensions::Beatmaps::PreviewBeatmapStub* preview = CRASH_UNLESS(il2cpp_utils::New<MultiQuestensions::Beatmaps::PreviewBeatmapStub*>(localPreview));

        if (preview->levelHash != nullptr) {
            if (self->get_localUserId() == self->get_hostUserId()) {
                sessionManager->SetLocalPlayerState(il2cpp_utils::newcsstr(beatmapDownloadedState), true);
            }

            packetManager->Send(reinterpret_cast<LiteNetLib::Utils::INetSerializable*>(preview->GetPacket(characteristic->get_serializedName(), beatmapDifficulty)));
            self->menuRpcManager->SelectBeatmap(BeatmapIdentifierNetSerializable::New_ctor(levelId, characteristic->get_serializedName(), beatmapDifficulty));
            self->SetPlayerBeatmapLevel(self->get_localUserId(), reinterpret_cast<IPreviewBeatmapLevel*>(preview), beatmapDifficulty, characteristic);
            return;
        }
    }
    LobbyPlayersSetLocalBeatmap(self, levelId, beatmapDifficulty, characteristic);
}

// LobbyPlayersDataModel HandleMenuRpcManagerSelectedBeatmap (DONT REMOVE THIS, without it a player's selected map will be cleared)
MAKE_HOOK_MATCH(LobbyPlayersSelectedBeatmap, &LobbyPlayersDataModel::HandleMenuRpcManagerSelectedBeatmap, void, LobbyPlayersDataModel* self, Il2CppString* userId, BeatmapIdentifierNetSerializable* beatmapId) {
    IPreviewBeatmapLevel* localPreview = self->beatmapLevelsModel->GetLevelPreviewForLevelId(beatmapId->levelID);
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
    
    custom_types::Register::AutoRegister();
    //custom_types::TypeRegistrator;
    //custom_types::Register::ExplicitRegister(&MultiQuestensions::PacketSerializer, &MultiplayerExtensions::Beatmaps::PreviewBeatmapPacket, &MultiQuestensions::Beatmaps::PreviewBeatmapStub);
    //CRASH_UNLESS(custom_types::Register::RegisterType<MultiQuestensions::PacketSerializer>());
    //CRASH_UNLESS(custom_types::Register::RegisterType<MultiplayerExtensions::Beatmaps::PreviewBeatmapPacket>());
    //CRASH_UNLESS(custom_types::Register::RegisterType<MultiQuestensions::Beatmaps::PreviewBeatmapStub>());

    getLogger().info("Installing hooks...");
    INSTALL_HOOK(getLogger(), SessionManagerStart);
    INSTALL_HOOK(getLogger(), LobbyPlayersActivate);
    INSTALL_HOOK(getLogger(), LobbyPlayersSetLocalBeatmap);
    INSTALL_HOOK(getLogger(), LobbyPlayersSelectedBeatmap);

    //INSTALL_HOOK_OFFSETLESS(getLogger(), SessionManagerStart, il2cpp_utils::FindMethodUnsafe("", "MultiplayerSessionManager", "Start", 0));
    //INSTALL_HOOK_OFFSETLESS(getLogger(), LobbyPlayersActivate, il2cpp_utils::FindMethodUnsafe("", "LobbyPlayersDataModel", "Activate", 0));
    //INSTALL_HOOK_OFFSETLESS(getLogger(), LobbyPlayersSetLocalBeatmap, il2cpp_utils::FindMethodUnsafe("", "LobbyPlayersDataModel", "SetLocalPlayerBeatmapLevel", 3));
    //INSTALL_HOOK_OFFSETLESS(getLogger(), LobbyPlayersSelectedBeatmap, il2cpp_utils::FindMethodUnsafe("", "LobbyPlayersDataModel", "HandleMenuRpcManagerSelectedBeatmap", 2));

    

    getLogger().info("Installed all hooks!");
}