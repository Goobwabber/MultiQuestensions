#include "main.hpp"
#include "Beatmaps/PreviewBeatmapPacket.hpp"
#include "Beatmaps/PreviewBeatmapStub.hpp"
#include "Packets/PacketManager.hpp"
#include "beatsaber-hook/shared/utils/il2cpp-utils.hpp"
#include "custom-types/shared/register.hpp"
#include "beatsaber-hook/shared/utils/hooking.hpp"

#include "GlobalNamespace/BeatmapIdentifierNetSerializable.hpp"
#include "GlobalNamespace/MultiplayerLevelLoader.hpp"
#include "GlobalNamespace/HMTask.hpp"
#include "GlobalNamespace/StandardScoreSyncStateNetSerializable.hpp"
#include "GlobalNamespace/NetworkPlayerEntitlementChecker.hpp"
using namespace GlobalNamespace;

#include "songloader/shared/API.hpp"
#include "questui/shared/CustomTypes/Components/MainThreadScheduler.hpp"



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



// Converts a levelId to a levelHash
Il2CppString* LevelIdToHash(Il2CppString* levelId) {
    if (Il2CppString::IsNullOrWhiteSpace(levelId)) {
        return nullptr;
    }
    Array<Il2CppString*>* ary = levelId->Split('_', ' ');
    Il2CppString* hash = nullptr;
    if (ary->Length() > 2) {
        hash = ary->values[2];
    }
    return (hash != nullptr && hash->get_Length() == 40) ? hash : nullptr;
}



// Plugin setup stuff
GlobalNamespace::MultiplayerSessionManager* sessionManager;
GlobalNamespace::LobbyPlayersDataModel* lobbyPlayersDataModel;
MultiQuestensions::PacketManager* packetManager;

std::string moddedState = "modded";
std::string questState = "platformquest";
std::string customSongsState = "customsongs";
std::string enforceModsState = "enforcemods";



MultiQuestensions::Beatmaps::PreviewBeatmapStub* GetExistingPreview(Il2CppString* levelId) {
    for (int i = 0; i < lobbyPlayersDataModel->playersData->get_Values()->get_Count(); i++) {
        ILevelGameplaySetupData* playerData = reinterpret_cast<System::Collections::Generic::List_1<ILevelGameplaySetupData*>*>(lobbyPlayersDataModel->playersData->get_Values())->get_Item(i);
        if (playerData->get_beatmapLevel() != nullptr && playerData->get_beatmapLevel()->get_levelID() == levelId) {
            if (il2cpp_functions::class_is_assignable_from((Il2CppClass*)playerData->get_beatmapLevel(), classof(MultiQuestensions::Beatmaps::PreviewBeatmapStub*))) {
                return reinterpret_cast<MultiQuestensions::Beatmaps::PreviewBeatmapStub*>(playerData->get_beatmapLevel());
            }
        }
    }
    return nullptr;
}

bool AllPlayersModded() {
    bool result = true;
    for (int i = 0; i < sessionManager->connectedPlayers->get_Count(); i++) {
        result = result && sessionManager->connectedPlayers->get_Item(i)->HasState(il2cpp_utils::newcsstr(moddedState));
    }
    return result;
}



// Handles a PreviewBeatmapPacket used to transmit data about a custom song.
static void HandlePreviewBeatmapPacket(MultiQuestensions::Beatmaps::PreviewBeatmapPacket* packet, GlobalNamespace::IConnectedPlayer* player) {

    getLogger().debug("'%s' selected song '%s'", to_utf8(csstrtostr(player->get_userId())).c_str(), to_utf8(csstrtostr(packet->levelHash)).c_str());
    BeatmapCharacteristicSO* characteristic = lobbyPlayersDataModel->beatmapCharacteristicCollection->GetBeatmapCharacteristicBySerializedName(packet->characteristic);
    MultiQuestensions::Beatmaps::PreviewBeatmapStub* preview = CRASH_UNLESS(il2cpp_utils::New<MultiQuestensions::Beatmaps::PreviewBeatmapStub*>(packet));
    lobbyPlayersDataModel->SetPlayerBeatmapLevel(player->get_userId(), reinterpret_cast<IPreviewBeatmapLevel*>(preview), GlobalNamespace::BeatmapDifficulty((int)packet->difficulty), characteristic);
}


MAKE_HOOK_MATCH(SessionManagerStart, &MultiplayerSessionManager::Start, void, MultiplayerSessionManager* self) {

    sessionManager = self;
    SessionManagerStart(sessionManager);
    packetManager = new MultiQuestensions::PacketManager(sessionManager);

    getLogger().debug("customsongsState is: %d, enforceModsState is: %d", getConfig().config["customsongs"].GetBool(), getConfig().config["enforcemods"].GetBool());
    
    // TODO: Add the SetLocalPlayerState stuff to BeatTogether
    self->SetLocalPlayerState(il2cpp_utils::newcsstr(moddedState), true);
    self->SetLocalPlayerState(il2cpp_utils::newcsstr(questState), true);
    self->SetLocalPlayerState(il2cpp_utils::newcsstr(customSongsState), getConfig().config["customsongs"].GetBool());
    //self->SetLocalPlayerState(il2cpp_utils::newcsstr(customSongsState), true);
    self->SetLocalPlayerState(il2cpp_utils::newcsstr(enforceModsState), getConfig().config["enforcemods"].GetBool());

    packetManager->RegisterCallback<MultiQuestensions::Beatmaps::PreviewBeatmapPacket*>("MultiplayerExtensions.Beatmaps.PreviewBeatmapPacket", HandlePreviewBeatmapPacket);
}

// LobbyPlayersDataModel Activate
MAKE_HOOK_MATCH(LobbyPlayersActivate, &LobbyPlayersDataModel::Activate, void, LobbyPlayersDataModel* self) {
    lobbyPlayersDataModel = self;
    LobbyPlayersActivate(lobbyPlayersDataModel);
}

// LobbyPlayersDataModel SetLocalPlayerBeatmapLevel
MAKE_HOOK_MATCH(LobbyPlayersSetLocalBeatmap, &LobbyPlayersDataModel::SetLocalPlayerBeatmapLevel, void, LobbyPlayersDataModel* self, Il2CppString* levelId, BeatmapDifficulty beatmapDifficulty, BeatmapCharacteristicSO* characteristic) {
    
    Il2CppString* hash = LevelIdToHash(levelId);
    if (hash != nullptr) {
        getLogger().info("Local user selected song '%s'.", to_utf8(csstrtostr(hash)).c_str());
        MultiQuestensions::Beatmaps::PreviewBeatmapStub* preview = GetExistingPreview(levelId);
        if (preview != nullptr) {
            self->SetPlayerBeatmapLevel(self->get_localUserId(), reinterpret_cast<IPreviewBeatmapLevel*>(preview), beatmapDifficulty, characteristic);
            self->menuRpcManager->SelectBeatmap(BeatmapIdentifierNetSerializable::New_ctor(levelId, characteristic->get_serializedName(), beatmapDifficulty));
            return;
        }
        else {
            IPreviewBeatmapLevel* localIPreview = self->beatmapLevelsModel->GetLevelPreviewForLevelId(levelId);
            if (localIPreview != nullptr) {
                MultiQuestensions::Beatmaps::PreviewBeatmapStub* localPreview = CRASH_UNLESS(il2cpp_utils::New<MultiQuestensions::Beatmaps::PreviewBeatmapStub*>(hash, localIPreview));
                self->SetPlayerBeatmapLevel(self->get_localUserId(), reinterpret_cast<IPreviewBeatmapLevel*>(preview), beatmapDifficulty, characteristic);
                MultiQuestensions::Beatmaps::PreviewBeatmapPacket* packet = CRASH_UNLESS(il2cpp_utils::New<MultiQuestensions::Beatmaps::PreviewBeatmapPacket*>(localPreview));
                packetManager->Send(reinterpret_cast<LiteNetLib::Utils::INetSerializable*>(packet));
                if (!AllPlayersModded()) self->menuRpcManager->SelectBeatmap(BeatmapIdentifierNetSerializable::New_ctor(levelId, characteristic->get_serializedName(), beatmapDifficulty));
                return;
            }
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

//static bool isMissingLevel = false;
//
//// This hook makes sure to grey-out the play button so that players can't start a level that someone doesn't have.
//// This prevents crashes.
//MAKE_HOOK_MATCH(HostLobbySetupViewController_SetPlayersMissingLevelText, void, HostLobbySetupViewController* self, Il2CppString* playersMissingLevelText) {
//    getLogger().info("HostLobbySetupViewController_SetPlayersMissingLevelText");
//    if (playersMissingLevelText && !isMissingLevel) {
//        getLogger().info("Disabling start game as missing level text exists . . .");
//        isMissingLevel = true;
//        self->SetStartGameEnabled(false, HostLobbySetupViewController::CannotStartGameReason::DoNotOwnSong);
//    }
//    else if (!playersMissingLevelText && isMissingLevel) {
//        getLogger().info("Enabling start game as missing level text does not exist . . .");
//        isMissingLevel = false;
//        self->SetStartGameEnabled(true, HostLobbySetupViewController::CannotStartGameReason::None);
//    }
//
//    HostLobbySetupViewController_SetPlayersMissingLevelText(self, playersMissingLevelText);
//}
//
//// Prevent the button becoming shown when we're force disabling it, as pressing it would crash
//MAKE_HOOK_OFFSETLESS(HostLobbySetupViewController_SetStartGameEnabled, void, HostLobbySetupViewController* self, bool startGameEnabled, HostLobbySetupViewController::CannotStartGameReason cannotStartGameReason) {
//    getLogger().info(string_format("HostLobbySetupViewController_SetStartGameEnabled. Enabled: %d. Reason: %d", startGameEnabled, cannotStartGameReason));
//    if (isMissingLevel && cannotStartGameReason == HostLobbySetupViewController::CannotStartGameReason::None) {
//        getLogger().info("Game attempted to enable the play button when the level was missing, stopping it!");
//        startGameEnabled = false;
//        cannotStartGameReason = HostLobbySetupViewController::CannotStartGameReason::DoNotOwnSong;
//    }
//    HostLobbySetupViewController_SetStartGameEnabled(self, startGameEnabled, cannotStartGameReason);
//}
//
//bool IsCustomLevel(const std::string& levelId) {
//    return levelId.starts_with(RuntimeSongLoader::API::GetCustomLevelsPrefix());
//}
//
//bool HasSong(std::string levelId) {
//    for (auto& song : RuntimeSongLoader::API::GetLoadedSongs()) {
//        if (to_utf8(csstrtostr(song->levelID)) == levelId) {
//            return true;
//        }
//    }
//    return false;
//}
//
//std::string GetHash(const std::string& levelId) {
//    return levelId.substr(RuntimeSongLoader::API::GetCustomLevelsPrefix().length(), levelId.length() - RuntimeSongLoader::API::GetCustomLevelsPrefix().length());
//}
//
//MAKE_HOOK_MATCH(MultiplayerLevelLoader_LoadLevel, &MultiplayerLevelLoader::LoadLevel, void, MultiplayerLevelLoader* self, BeatmapIdentifierNetSerializable* beatmapId, GameplayModifiers* gameplayModifiers, float initialStartTime) {
//    std::string levelId = to_utf8(csstrtostr(beatmapId->levelID));
//    getLogger().info("MultiplayerLevelLoader_LoadLevel: %s", levelId.c_str());
//    if (IsCustomLevel(levelId)) {
//        if (HasSong(levelId)) {
//            MultiplayerLevelLoader_LoadLevel(self, beatmapId, gameplayModifiers, initialStartTime);
//        }
//        else {
//            BeatSaver::API::GetBeatmapByHashAsync(GetHash(levelId),
//                [self, beatmapId, gameplayModifiers, initialStartTime](std::optional<BeatSaver::Beatmap> beatmapOpt) {
//                    if (beatmapOpt.has_value()) {
//                        auto beatmap = beatmapOpt.value();
//                        auto beatmapName = beatmap.GetName();
//                        getLogger().info("Downloading map: %s", beatmapName.c_str());
//                        BeatSaver::API::DownloadBeatmapAsync(beatmap,
//                            [self, beatmapId, gameplayModifiers, initialStartTime, beatmapName](bool error) {
//                                if (error) {
//                                    getLogger().info("Failed downloading map: %s", beatmapName.c_str());
//                                }
//                                else {
//                                    getLogger().info("Downloaded map: %s", beatmapName.c_str());
//                                    QuestUI::MainThreadScheduler::Schedule(
//                                        [self, beatmapId, gameplayModifiers, initialStartTime] {
//                                            RuntimeSongLoader::API::RefreshSongs(false,
//                                                [self, beatmapId, gameplayModifiers, initialStartTime](const std::vector<GlobalNamespace::CustomPreviewBeatmapLevel*>& songs) {
//                                                    self->loaderState = MultiplayerLevelLoader::MultiplayerBeatmapLoaderState::NotLoading;
//                                                    MultiplayerLevelLoader_LoadLevel(self, beatmapId, gameplayModifiers, initialStartTime);
//                                                }
//                                            );
//                                        }
//                                    );
//                                }
//                            }
//                        );
//                    }
//                }
//            );
//        }
//    }
//    else {
//        MultiplayerLevelLoader_LoadLevel(self, beatmapId, gameplayModifiers, initialStartTime);
//    }
//}
//
//MAKE_HOOK_MATCH(NetworkPlayerEntitlementChecker_GetEntitlementStatus, &NetworkPlayerEntitlementChecker::GetEntitlementStatus, Task_1<EntitlementsStatus>*, NetworkPlayerEntitlementChecker* self, Il2CppString* levelIdCS) {
//    std::string levelId = to_utf8(csstrtostr(levelIdCS));
//    getLogger().info("NetworkPlayerEntitlementChecker_GetEntitlementStatus: %s", levelId.c_str());
//    if (IsCustomLevel(levelId)) {
//        if (HasSong(levelId)) {
//            return Task_1<EntitlementsStatus>::New_ctor(EntitlementsStatus::Ok);
//        }
//        else {
//            auto task = Task_1<EntitlementsStatus>::New_ctor();
//            BeatSaver::API::GetBeatmapByHashAsync(GetHash(levelId),
//                [task](std::optional<BeatSaver::Beatmap> beatmap) {
//                    QuestUI::MainThreadScheduler::Schedule(
//                        [task, beatmap] {
//                            if (beatmap.has_value()) {
//                                task->TrySetResult(EntitlementsStatus::NotDownloaded);
//                            }
//                            else {
//                                task->TrySetResult(EntitlementsStatus::NotOwned);
//                            }
//                        }
//                    );
//                }
//            );
//            return task;
//        }
//    }
//    else {
//        return NetworkPlayerEntitlementChecker_GetEntitlementStatus(self, levelIdCS);
//    }
//}
//
//MAKE_HOOK_MATCH(NetworkPlayerEntitlementChecker_GetPlayerLevelEntitlementsAsync, &NetworkPlayerEntitlementChecker::GetPlayerLevelEntitlementsAsync, Task_1<EntitlementsStatus>*, NetworkPlayerEntitlementChecker* self, IConnectedPlayer* player, Il2CppString* levelId, System::Threading::CancellationToken token) {
//    auto task = Task_1<EntitlementsStatus>::New_ctor();
//    HMTask::New_ctor(il2cpp_utils::MakeDelegate<System::Action*>(classof(System::Action*),
//        (std::function<void()>)[self, player, levelId, token, task] {
//            auto returnTask = NetworkPlayerEntitlementChecker_GetPlayerLevelEntitlementsAsync(self, player, levelId, token);
//            auto result = returnTask->get_Result();
//            if (result == EntitlementsStatus::NotDownloaded) {
//                result = EntitlementsStatus::Ok;
//            }
//            task->TrySetResult(result);
//        }
//    ), nullptr)->Run();
//    return task;
//}

void saveDefaultConfig() {
    getLogger().info("Creating config file...");
    ConfigDocument& config = getConfig().config;

    if (config.HasMember("customsongs")) {
        getLogger().info("Config file already exists.");
        return;
    }

    config.RemoveAllMembers();
    config.SetObject();
    auto& allocator = config.GetAllocator();

    config.AddMember("customsongs", true, allocator);
    config.AddMember("enforcemods", true, allocator);
    //config["customsongs"].SetBool(true);
    //config["enforcemods"].SetBool(true);

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
    //MultiplayerExtensions::Beatmaps::PreviewBeatmapPacket::Install();

    custom_types::Register::AutoRegister();
    //CRASH_UNLESS(custom_types::Register::RegisterType<MultiQuestensions::PacketSerializer>());
    //CRASH_UNLESS(custom_types::Register::RegisterType<MultiQuestensions::Beatmaps::PreviewBeatmapPacket>());
    //CRASH_UNLESS(custom_types::Register::RegisterType<MultiQuestensions::Beatmaps::PreviewBeatmapStub>());

    getLogger().info("Installing hooks...");
    INSTALL_HOOK(getLogger(), SessionManagerStart);
    INSTALL_HOOK(getLogger(), LobbyPlayersActivate);
    INSTALL_HOOK(getLogger(), LobbyPlayersSetLocalBeatmap);
    INSTALL_HOOK(getLogger(), LobbyPlayersSelectedBeatmap);

    if (Modloader::getMods().find("BeatTogether") != Modloader::getMods().end()) {
        getLogger().info("Hello BeatTogether!");
    }
    else getLogger().warning("BeatTogether was not found! Is Multiplayer modded?");
    //INSTALL_HOOK(getLogger(), MultiplayerLevelLoader_LoadLevel);
    //INSTALL_HOOK(getLogger(), NetworkPlayerEntitlementChecker_GetEntitlementStatus);
    //INSTALL_HOOK(getLogger(), NetworkPlayerEntitlementChecker_GetPlayerLevelEntitlementsAsync);

    //INSTALL_HOOK(getLogger(), HostLobbySetupViewController_SetPlayersMissingLevelText);
    ////INSTALL_HOOK_OFFSETLESS(getLogger(), MultiplayerLevelSelectionFlowCoordinator_Setup,
    //INSTALL_HOOK(getLogger(), HostLobbySetupViewController_SetStartGameEnabled);

    //custom_types::logVtable(classof(IRemoteProcedureCall*)->vtable);

    //custom_types::logVtable(&classof(StandardScoreSyncStateNetSerializable*)->vtable[0]);

    //getLogger().debug("vtable_count %d", classof(StandardScoreSyncStateNetSerializable*)->vtable_count);

    //il2cpp_functions::Class_Init(classof(StandardScoreSyncStateNetSerializable*));
    //for (uint16_t i = 0; i < classof(StandardScoreSyncStateNetSerializable*)->vtable_count; i++) {
    //    custom_types::logVtable(&classof(StandardScoreSyncStateNetSerializable*)->vtable[i]);
    //}

    //INSTALL_HOOK_OFFSETLESS(getLogger(), SessionManagerStart, il2cpp_utils::FindMethodUnsafe("", "MultiplayerSessionManager", "Start", 0));
    //INSTALL_HOOK_OFFSETLESS(getLogger(), LobbyPlayersActivate, il2cpp_utils::FindMethodUnsafe("", "LobbyPlayersDataModel", "Activate", 0));
    //INSTALL_HOOK_OFFSETLESS(getLogger(), LobbyPlayersSetLocalBeatmap, il2cpp_utils::FindMethodUnsafe("", "LobbyPlayersDataModel", "SetLocalPlayerBeatmapLevel", 3));
    //INSTALL_HOOK_OFFSETLESS(getLogger(), LobbyPlayersSelectedBeatmap, il2cpp_utils::FindMethodUnsafe("", "LobbyPlayersDataModel", "HandleMenuRpcManagerSelectedBeatmap", 2));

    

    getLogger().info("Installed all hooks!");
}