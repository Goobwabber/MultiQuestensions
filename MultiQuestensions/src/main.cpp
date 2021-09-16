#include "main.hpp"
#include "Beatmaps/PreviewBeatmapPacket.hpp"
#include "Beatmaps/PreviewBeatmapStub.hpp"
#include "Packets/PacketManager.hpp"
#include "UI/LobbySetupPanel.hpp"

#include "beatsaber-hook/shared/utils/il2cpp-utils.hpp"
#include "custom-types/shared/register.hpp"
#include "beatsaber-hook/shared/utils/hooking.hpp"

#include "System/Collections/Generic/Dictionary_2.hpp"
#include "System/Collections/Hashtable.hpp"
#include "System/Collections/Hashtable_ValueCollection.hpp"
#include "System/Threading/Tasks/Task_1.hpp"
#include "System/Action_1.hpp"
#include "System/Action.hpp"

#include "GlobalNamespace/BeatmapIdentifierNetSerializable.hpp"
#include "GlobalNamespace/MultiplayerLevelLoader.hpp"
#include "GlobalNamespace/HMTask.hpp"
#include "GlobalNamespace/StandardScoreSyncStateNetSerializable.hpp"
#include "GlobalNamespace/NetworkPlayerEntitlementChecker.hpp"
#include "GlobalNamespace/LevelSelectionNavigationController.hpp"
#include "GlobalNamespace/LobbySetupViewController.hpp"
#include "GlobalNamespace/CannotStartGameReason.hpp"
#include "GlobalNamespace/LobbyGameStateController.hpp"
#include "GlobalNamespace/ILobbyPlayerData.hpp"
#include "GlobalNamespace/GameServerPlayerTableCell.hpp"

#include "GlobalNamespace/MenuRpcManager.hpp"
#include "GlobalNamespace/ConnectedPlayerManager.hpp"
#include "GlobalNamespace/ConnectedPlayerManager_SyncTimePacket.hpp"
//#include "GlobalNamespace/LobbySetupViewController.hpp"
//#include "GlobalNamespace/LobbySetupViewController_CannotStartGameReason.hpp"
using namespace GlobalNamespace;
using namespace System::Threading::Tasks;

#include "songloader/shared/API.hpp"
#include "questui/shared/CustomTypes/Components/MainThreadScheduler.hpp"

#include "songdownloader/shared/BeatSaverAPI.hpp"

std::vector<EntitlementsStatus> playerEntitlements;

#ifndef VERSION
#warning No Version set
#define VERSION "0.1.0"
#endif

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

bool customSongsEnabled = true;

//using PD_ValueCollection = System::Collections::Generic::Dictionary_2<Il2CppString*, ILobbyPlayerDataModel*>::ValueCollection;

MultiQuestensions::Beatmaps::PreviewBeatmapStub* GetExistingPreview(Il2CppString* levelId) {
    getLogger().debug("GetExistingPreview PlayerCount: %d", sessionManager->connectedPlayers->get_Count());
    for (int i = 0; i < sessionManager->connectedPlayers->get_Count(); i++) {
        ILevelGameplaySetupData* playerData = reinterpret_cast<ILevelGameplaySetupData*>(lobbyPlayersDataModel->playersData->
            get_Item(sessionManager->connectedPlayers->get_Item(i)->get_userId()));
        if (playerData->get_beatmapLevel() != nullptr && playerData->get_beatmapLevel()->get_levelID()->Equals(levelId)) {
            getLogger().debug("GetExistingPreview type: %s", il2cpp_utils::ClassStandardName(reinterpret_cast<Il2CppObject*>(playerData->get_beatmapLevel())->klass).c_str());
            if (il2cpp_utils::AssignableFrom<MultiQuestensions::Beatmaps::PreviewBeatmapStub*>(reinterpret_cast<Il2CppObject*>(playerData->get_beatmapLevel())->klass)) {
                getLogger().debug(__FILE__ " Line: %d", __LINE__);
                return reinterpret_cast<MultiQuestensions::Beatmaps::PreviewBeatmapStub*>(playerData->get_beatmapLevel());
            }
        }
        else {
            if (playerData->get_beatmapLevel() != nullptr)
                getLogger().debug("Check expected levelId: %s, \r\nactual levelId: %s, \r\nlevelIDs match? %s", 
                    to_utf8(csstrtostr(levelId)).c_str(), 
                    to_utf8(csstrtostr(playerData->get_beatmapLevel()->get_levelID())).c_str(), 
                    playerData->get_beatmapLevel()->get_levelID()->Equals(levelId) ? "true" : "false");
            else getLogger().debug("beatmapLevel is null");
        }
    }
    getLogger().debug("GetExistingPreview return nullptr " __FILE__ " Line: %d", __LINE__);
    return nullptr;
}

bool AllPlayersModded() {    
    for (int i = 0; i < sessionManager->connectedPlayers->get_Count(); i++) {
        if (!sessionManager->connectedPlayers->get_Item(i)->HasState(il2cpp_utils::newcsstr(moddedState))) return false;
    }
    return true;
}

// Handles a PreviewBeatmapPacket used to transmit data about a custom song.
static void HandlePreviewBeatmapPacket(MultiQuestensions::Beatmaps::PreviewBeatmapPacket* packet, GlobalNamespace::IConnectedPlayer* player) {
    getLogger().debug("'%s' selected song '%s'", to_utf8(csstrtostr(player->get_userId())).c_str(), to_utf8(csstrtostr(packet->levelHash)).c_str());
    IPreviewBeatmapLevel* localPreview = lobbyPlayersDataModel->beatmapLevelsModel->GetLevelPreviewForLevelId(packet->levelId);
    MultiQuestensions::Beatmaps::PreviewBeatmapStub* preview;
    try {
        if (localPreview == nullptr) {
            Il2CppString* nullString = nullptr;
            IPreviewBeatmapLevel* nullLvl = nullptr;
            preview = THROW_UNLESS(il2cpp_utils::New<MultiQuestensions::Beatmaps::PreviewBeatmapStub*>(nullString, nullLvl, packet));
        }
        else {
            MultiQuestensions::Beatmaps::PreviewBeatmapPacket* nullpacket = nullptr;
            preview = THROW_UNLESS(il2cpp_utils::New<MultiQuestensions::Beatmaps::PreviewBeatmapStub*>(packet->levelHash, localPreview, nullpacket));
        }
        BeatmapCharacteristicSO* characteristic = lobbyPlayersDataModel->beatmapCharacteristicCollection->GetBeatmapCharacteristicBySerializedName(packet->characteristic);
        getLogger().debug("Check difficulty as unsigned int: %u", packet->difficulty);
        lobbyPlayersDataModel->SetPlayerBeatmapLevel(player->get_userId(), reinterpret_cast<IPreviewBeatmapLevel*>(preview), packet->difficulty, characteristic);
    }
    catch (const std::runtime_error& e) {
        getLogger().error("%s", e.what());
    }
    catch (...) {
        getLogger().debug("Unknown exception in HandlePreviewBeatmapPacket");
    }

}

MAKE_HOOK_MATCH(SessionManagerStart, &MultiplayerSessionManager::Start, void, MultiplayerSessionManager* self) {

    sessionManager = self;
    SessionManagerStart(sessionManager);
    packetManager = new MultiQuestensions::PacketManager(sessionManager);

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
            self->menuRpcManager->RecommendBeatmap(BeatmapIdentifierNetSerializable::New_ctor(levelId, characteristic->get_serializedName(), beatmapDifficulty));
            return;
        }
        else {
            IPreviewBeatmapLevel* localIPreview = self->beatmapLevelsModel->GetLevelPreviewForLevelId(levelId);
            if (localIPreview != nullptr) {
                try {
                    using namespace MultiQuestensions::Beatmaps;
                    PreviewBeatmapStub* previewStub = THROW_UNLESS(il2cpp_utils::New<MultiQuestensions::Beatmaps::PreviewBeatmapStub*>(hash, localIPreview, static_cast<PreviewBeatmapPacket*>(nullptr)));
                    //MultiQuestensions::Beatmaps::PreviewBeatmapStub* previewStub = MultiQuestensions::Beatmaps::PreviewBeatmapStub::New_ctor(hash, localIPreview);
                    getLogger().debug("Check 'LobbyPlayersSetLocalBeatmap' levelID: %s", to_utf8(csstrtostr(reinterpret_cast<IPreviewBeatmapLevel*>(previewStub)->get_levelID())).c_str());
                    self->SetPlayerBeatmapLevel(self->get_localUserId(), reinterpret_cast<IPreviewBeatmapLevel*>(previewStub), beatmapDifficulty, characteristic);
                    PreviewBeatmapPacket* packet = previewStub->GetPacket(characteristic->get_serializedName(), beatmapDifficulty);
                    packetManager->Send(reinterpret_cast<LiteNetLib::Utils::INetSerializable*>(packet));
                    self->menuRpcManager->RecommendBeatmap(BeatmapIdentifierNetSerializable::New_ctor(levelId, characteristic->get_serializedName(), beatmapDifficulty));
                    return;
                }
                catch (const std::runtime_error& e) {
                    getLogger().error("An Exception occured during class creation: %s", e.what());
                }
            }
        }
    }
    LobbyPlayersSetLocalBeatmap(self, levelId, beatmapDifficulty, characteristic);
}

std::string GetHash(const std::string& levelId) {
    return levelId.substr(RuntimeSongLoader::API::GetCustomLevelsPrefix().length(), levelId.length() - RuntimeSongLoader::API::GetCustomLevelsPrefix().length());
}

// LobbyPlayersDataModel HandleMenuRpcManagerSelectedBeatmap (DONT REMOVE THIS, without it a player's selected map will be cleared)
MAKE_HOOK_MATCH(LobbyPlayersSelectedBeatmap, &LobbyPlayersDataModel::HandleMenuRpcManagerRecommendBeatmap, void, LobbyPlayersDataModel* self, Il2CppString* userId, BeatmapIdentifierNetSerializable* beatmapId) {
    getLogger().debug("HandleMenuRpcManagerRecommendBeatmap: LevelID: %s", to_utf8(csstrtostr(beatmapId->get_levelID())).c_str());
    MultiQuestensions::Beatmaps::PreviewBeatmapStub* preview = GetExistingPreview(beatmapId->get_levelID());
    if (preview != nullptr) {
        getLogger().debug("HandleMenuRpcManagerRecommendBeatmap: Preview exists, SetPlayerBeatmapLevel");
        BeatmapCharacteristicSO* characteristic = self->beatmapCharacteristicCollection->GetBeatmapCharacteristicBySerializedName(beatmapId->get_beatmapCharacteristicSerializedName());
        self->SetPlayerBeatmapLevel(userId, reinterpret_cast<IPreviewBeatmapLevel*>(preview), beatmapId->get_difficulty(), characteristic);
        return;
    }
    else {
        getLogger().debug("HandleMenuRpcManagerRecommendBeatmap: Preview doesn't exist use localPreview, SetPlayerBeatmapLevel");
        IPreviewBeatmapLevel* localPreview = self->beatmapLevelsModel->GetLevelPreviewForLevelId(beatmapId->levelID);
        if (localPreview != nullptr) {
            BeatmapCharacteristicSO* characteristic = self->beatmapCharacteristicCollection->GetBeatmapCharacteristicBySerializedName(beatmapId->get_beatmapCharacteristicSerializedName());
            //self->SetPlayerBeatmapLevel(userId, localPreview, beatmapId->get_difficulty(), characteristic);
            LobbyPlayersSelectedBeatmap(self, userId, beatmapId);
        }
        else {
            // TODO: fetch beatmap data from beatsaver
            getLogger().debug("No Info found, need to fetch from BeatSaver");
            //BeatSaver::API::GetBeatmapByHashAsync(GetHash((beatmapId->levelID),
            //    [](std::optional<BeatSaver::Beatmap> beatmap) {
            //        QuestUI::MainThreadScheduler::Schedule(
            //            [beatmap] {
            //                if (beatmap.has_value()) {
            //                    self->SetPlayerBeatmapLevel(userId, bmPreview, beatmapId);
            //                // Create our own Preview or something?
            //                }
            //                else {
            //                // Well doesn't exist on BeatSaver then
            //                }
            //            }
            //        );
            //    }
            //);
        }
    }
}

MAKE_HOOK_MATCH(LobbySetupViewController_DidActivate, &LobbySetupViewController::DidActivate, void, LobbySetupViewController* self, bool firstActivation, bool addedToHierarchy, bool screenSystemEnabling) {
    LobbySetupViewController_DidActivate(self, firstActivation, addedToHierarchy, screenSystemEnabling);
    if (firstActivation)
        MultiQuestensions::UI::LobbySetupPanel::AddSetupPanel(self->get_rectTransform(), sessionManager);
}

// Show the custom levels tab in multiplayer
MAKE_HOOK_MATCH(LevelSelectionNavigationController_Setup, &LevelSelectionNavigationController::Setup, void, LevelSelectionNavigationController* self,
    SongPackMask songPackMask, BeatmapDifficultyMask allowedBeatmapDifficultyMask, Array<GlobalNamespace::BeatmapCharacteristicSO*>* notAllowedCharacteristics,
    bool hidePacksIfOneOrNone, bool hidePracticeButton, bool showPlayerStatsInDetailView, Il2CppString* actionButtonText, IBeatmapLevelPack* levelPackToBeSelectedAfterPresent,
    SelectLevelCategoryViewController::LevelCategory startLevelCategory, IPreviewBeatmapLevel* beatmapLevelToBeSelectedAfterPresent, bool enableCustomLevels) {
    getLogger().info("LevelSelectionNavigationController_Setup setting custom songs . . .");
    LevelSelectionNavigationController_Setup(self, songPackMask, allowedBeatmapDifficultyMask, notAllowedCharacteristics, hidePacksIfOneOrNone, hidePracticeButton, showPlayerStatsInDetailView,
        actionButtonText, levelPackToBeSelectedAfterPresent, startLevelCategory, beatmapLevelToBeSelectedAfterPresent, customSongsEnabled);
}

static bool isMissingLevel = false;

// This hook makes sure to grey-out the play button so that players can't start a level that someone doesn't have.
// This prevents crashes.
MAKE_HOOK_MATCH(LobbySetupViewController_SetPlayersMissingLevelText , &LobbySetupViewController::SetPlayersMissingLevelText, void, LobbySetupViewController* self, Il2CppString* playersMissingLevelText) {
    getLogger().info("LobbySetupViewController_SetPlayersMissingLevelText");
    if (playersMissingLevelText && !isMissingLevel) {
        getLogger().info("Disabling start game as missing level text exists . . .");
        isMissingLevel = true;
        self->SetStartGameEnabled(CannotStartGameReason::DoNotOwnSong);
    }
    else if (!playersMissingLevelText && isMissingLevel) {
        getLogger().info("Enabling start game as missing level text does not exist . . .");
        isMissingLevel = false;
        self->SetStartGameEnabled(CannotStartGameReason::None);
    }

    LobbySetupViewController_SetPlayersMissingLevelText(self, playersMissingLevelText);
}

// Prevent the button becoming shown when we're force disabling it, as pressing it would crash
MAKE_HOOK_MATCH(LobbySetupViewController_SetStartGameEnabled, &LobbySetupViewController::SetStartGameEnabled, void, LobbySetupViewController* self, CannotStartGameReason cannotStartGameReason) {
    getLogger().info("LobbySetupViewController_SetStartGameEnabled. Reason: %d", (int)cannotStartGameReason);
    if (isMissingLevel && cannotStartGameReason == CannotStartGameReason::None) {
        getLogger().info("Game attempted to enable the play button when the level was missing, stopping it!");
        cannotStartGameReason = CannotStartGameReason::DoNotOwnSong;
    }
    LobbySetupViewController_SetStartGameEnabled(self, cannotStartGameReason);
}

bool IsCustomLevel(const std::string& levelId) {
    return levelId.starts_with(RuntimeSongLoader::API::GetCustomLevelsPrefix());
}

bool HasSong(std::string levelId) {
    for (auto& song : RuntimeSongLoader::API::GetLoadedSongs()) {
        if (to_utf8(csstrtostr(song->levelID)) == levelId) {
            return true;
        }
    }
    return false;
}

MAKE_HOOK_MATCH(MultiplayerLevelLoader_LoadLevel, &MultiplayerLevelLoader::LoadLevel, void, MultiplayerLevelLoader* self, BeatmapIdentifierNetSerializable* beatmapId, GameplayModifiers* gameplayModifiers, float initialStartTime) {
    std::string levelId = to_utf8(csstrtostr(beatmapId->levelID));
    getLogger().info("MultiplayerLevelLoader_LoadLevel: %s", levelId.c_str());
    if (IsCustomLevel(levelId)) {
        if (HasSong(levelId)) {
            getLogger().debug("MultiplayerLevelLoader_LoadLevel, HasSong, calling original");
            MultiplayerLevelLoader_LoadLevel(self, beatmapId, gameplayModifiers, initialStartTime);
            return;
        }
        else {
            BeatSaver::API::GetBeatmapByHashAsync(GetHash(levelId),
                [self, beatmapId, gameplayModifiers, initialStartTime](std::optional<BeatSaver::Beatmap> beatmapOpt) {
                    if (beatmapOpt.has_value()) {
                        auto beatmap = beatmapOpt.value();
                        auto beatmapName = beatmap.GetName();
                        getLogger().info("Downloading map: %s", beatmapName.c_str());
                        BeatSaver::API::DownloadBeatmapAsync(beatmap,
                            [self, beatmapId, gameplayModifiers, initialStartTime, beatmapName](bool error) {
                                if (error) {
                                    getLogger().info("Failed downloading map: %s", beatmapName.c_str());
                                }
                                else {
                                    getLogger().info("Downloaded map: %s", beatmapName.c_str());
                                    QuestUI::MainThreadScheduler::Schedule(
                                        [self, beatmapId, gameplayModifiers, initialStartTime] {
                                            RuntimeSongLoader::API::RefreshSongs(false,
                                                [self, beatmapId, gameplayModifiers, initialStartTime](const std::vector<GlobalNamespace::CustomPreviewBeatmapLevel*>& songs) {
                                                    self->loaderState = MultiplayerLevelLoader::MultiplayerBeatmapLoaderState::NotLoading;
                                                    //getLogger().debug("MultiplayerLevelLoader_LoadLevel, Downloaded, calling original");
                                                    MultiplayerLevelLoader_LoadLevel(self, beatmapId, gameplayModifiers, initialStartTime);
                                                    return;
                                                }
                                            );
                                        }
                                    );
                                }
                            }
                        );
                    }
                }
            );
        }
    }
    else {
        getLogger().debug("MultiplayerLevelLoader_LoadLevel, calling original");
        MultiplayerLevelLoader_LoadLevel(self, beatmapId, gameplayModifiers, initialStartTime);
    }
}

MAKE_HOOK_MATCH(NetworkPlayerEntitlementChecker_GetEntitlementStatus, &NetworkPlayerEntitlementChecker::GetEntitlementStatus, Task_1<EntitlementsStatus>*, NetworkPlayerEntitlementChecker* self, Il2CppString* levelIdCS) {
    std::string levelId = to_utf8(csstrtostr(levelIdCS));
    getLogger().info("NetworkPlayerEntitlementChecker_GetEntitlementStatus: %s", levelId.c_str());
    if (IsCustomLevel(levelId)) {
        if (HasSong(levelId)) {
            return Task_1<EntitlementsStatus>::New_ctor(EntitlementsStatus::Ok);
        }
        else {
            auto task = Task_1<EntitlementsStatus>::New_ctor();
            BeatSaver::API::GetBeatmapByHashAsync(GetHash(levelId),
                [task](std::optional<BeatSaver::Beatmap> beatmap) {
                    QuestUI::MainThreadScheduler::Schedule(
                        [task, beatmap] {
                            if (beatmap.has_value()) {
                                task->TrySetResult(EntitlementsStatus::NotDownloaded);
                            }
                            else {
                                task->TrySetResult(EntitlementsStatus::NotOwned);
                            }
                        }
                    );
                }
            );
            return task;
        }
    }
    else {
        return NetworkPlayerEntitlementChecker_GetEntitlementStatus(self, levelIdCS);
    }
}

//System::Action_3<::Il2CppString*, ::Il2CppString*, EntitlementsStatus>* entitlementAction;
//
//MAKE_HOOK_MATCH(NetworkPlayerEntitlementChecker_Start, &NetworkPlayerEntitlementChecker::Start, void, NetworkPlayerEntitlementChecker* self) {
//    entitlementAction = il2cpp_utils::MakeDelegate<System::Action_3<::Il2CppString*, ::Il2CppString*, EntitlementsStatus>*>(classof(System::Action_3<::Il2CppString*, ::Il2CppString*, EntitlementsStatus>*), (std::function<void(Il2CppString*, Il2CppString*, EntitlementsStatus)>) [&](Il2CppString* userId, Il2CppString* beatmapId, EntitlementsStatus status) {
//        playerEntitlements.emplace_back(status);
//        });
//    self->rpcManager->add_setIsEntitledToLevelEvent(entitlementAction);
//    NetworkPlayerEntitlementChecker_Start(self);
//}
//
//MAKE_HOOK_MATCH(NetworkPlayerEntitlementChecker_OnDestroy, &NetworkPlayerEntitlementChecker::OnDestroy, void, NetworkPlayerEntitlementChecker* self) {
//    if (entitlementAction)
//        playerEntitlements.clear();
//        self->rpcManager->remove_setIsEntitledToLevelEvent(entitlementAction);
//    NetworkPlayerEntitlementChecker_OnDestroy(self);
//}

MAKE_HOOK_MATCH(LobbyGameStateController_HandleMultiplayerLevelLoaderCountdownFinished, &LobbyGameStateController::HandleMultiplayerLevelLoaderCountdownFinished, void, LobbyGameStateController* self, GlobalNamespace::IPreviewBeatmapLevel* previewBeatmapLevel, GlobalNamespace::BeatmapDifficulty beatmapDifficulty, GlobalNamespace::BeatmapCharacteristicSO* beatmapCharacteristic, GlobalNamespace::IDifficultyBeatmap* difficultyBeatmap, GlobalNamespace::GameplayModifiers* gameplayModifiers) {
    // TODO: I honestly forgot what I had to add in here
    getLogger().debug("LobbyGameStateController_HandleMultiplayerLevelLoaderCountdownFinished");
    self->menuRpcManager->SetIsEntitledToLevel(previewBeatmapLevel->get_levelID(), EntitlementsStatus::Ok);
    LobbyGameStateController_HandleMultiplayerLevelLoaderCountdownFinished(self, previewBeatmapLevel, beatmapDifficulty, beatmapCharacteristic, difficultyBeatmap, gameplayModifiers);
}

void saveDefaultConfig() {
    getLogger().info("Creating config file...");
    ConfigDocument& config = getConfig().config;

    if (config.HasMember("customsongs") && config.HasMember("freemod")) {
        getLogger().info("Config file already exists.");
        return;
    }  

    config.RemoveAllMembers();
    config.SetObject();
    auto& allocator = config.GetAllocator();

    config.AddMember("customsongs", true, allocator);
    config.AddMember("freemod", false, allocator);
    config.AddMember("hostpick", true, allocator);
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

MAKE_HOOK_MATCH(MenuRpcManager_InvokeSetCountdownEndTime, &MenuRpcManager::InvokeSetCountdownEndTime, void, MenuRpcManager* self, ::Il2CppString* userId, float newTime) {
    getLogger().debug("InvokeSetCountdownEndTime: newTime: %f", newTime);
    MenuRpcManager_InvokeSetCountdownEndTime(self, userId, newTime);
}

MAKE_HOOK_MATCH(MenuRpcManager_InvokeStartLevel, &MenuRpcManager::InvokeStartLevel, void, MenuRpcManager* self, ::Il2CppString* userId, GlobalNamespace::BeatmapIdentifierNetSerializable* beatmapId, GlobalNamespace::GameplayModifiers* gameplayModifiers, float startTime) {
    getLogger().debug("StartLevel: startTime: %f", startTime);
    MenuRpcManager_InvokeStartLevel(self, userId, beatmapId, gameplayModifiers, startTime);
}

MAKE_HOOK_MATCH(LobbyGameStateController_HandleMenuRpcManagerSetCountdownEndTime, &LobbyGameStateController::HandleMenuRpcManagerSetCountdownEndTime, void, LobbyGameStateController* self, ::Il2CppString* userId, float countdownTime) {
    getLogger().debug("SetCountdownEndTime: raw=%f, synctime=%f, r-s=%f", countdownTime, self->multiplayerSessionManager->get_syncTime(), countdownTime - self->multiplayerSessionManager->get_syncTime());
    LobbyGameStateController_HandleMenuRpcManagerSetCountdownEndTime(self, userId, countdownTime);
}

MAKE_HOOK_MATCH(ConnectedPlayerManager_HandleSyncTimePacket, &ConnectedPlayerManager::HandleSyncTimePacket, void, ConnectedPlayerManager* self, GlobalNamespace::ConnectedPlayerManager::SyncTimePacket* packet, GlobalNamespace::IConnectedPlayer* player) {
    //getLogger().debug("SetCountdownEndTime: raw=%f, synctime=%f, r-s=%f", countdownTime, self->multiplayerSessionManager->get_syncTime(), countdownTime - self->multiplayerSessionManager->get_syncTime());
    getLogger().debug("HandleSyncTimePacket: syncTime=%f", packet->syncTime);

    ConnectedPlayerManager_HandleSyncTimePacket(self, packet, player);
}

MAKE_HOOK_MATCH(GameServerPlayerTableCell_SetData, &GameServerPlayerTableCell::SetData, void, GameServerPlayerTableCell* self, IConnectedPlayer* connectedPlayer, ILobbyPlayerData* playerData, bool hasKickPermissions, bool allowSelection, System::Threading::Tasks::Task_1<AdditionalContentModel::EntitlementStatus>* getLevelEntitlementTask) {
    getLevelEntitlementTask = Task_1<AdditionalContentModel::EntitlementStatus>::New_ctor(AdditionalContentModel::EntitlementStatus::Owned);
    GameServerPlayerTableCell_SetData(self, connectedPlayer, playerData, hasKickPermissions, allowSelection, getLevelEntitlementTask);
}

// Called later on in the game loading - a good time to install function hooks
extern "C" void load() {
    il2cpp_functions::Init();

    custom_types::Register::AutoRegister();

    getLogger().info("Installing hooks...");
    INSTALL_HOOK(getLogger(), SessionManagerStart);
    INSTALL_HOOK(getLogger(), LobbyPlayersActivate);
    INSTALL_HOOK(getLogger(), LobbyPlayersSetLocalBeatmap);
    INSTALL_HOOK(getLogger(), LobbyPlayersSelectedBeatmap);

    INSTALL_HOOK(getLogger(), MultiplayerLevelLoader_LoadLevel);
    INSTALL_HOOK(getLogger(), NetworkPlayerEntitlementChecker_GetEntitlementStatus);
    //INSTALL_HOOK(getLogger(), NetworkPlayerEntitlementChecker_Start);
    //INSTALL_HOOK(getLogger(), NetworkPlayerEntitlementChecker_OnDestroy);
    //INSTALL_HOOK(getLogger(), NetworkPlayerEntitlementChecker_GetPlayerLevelEntitlementsAsync);
    if (Modloader::getMods().find("BeatTogether") != Modloader::getMods().end()) {
        getLogger().info("Hello BeatTogether!");
    }
    else getLogger().warning("BeatTogether was not found! Is Multiplayer modded?");

    INSTALL_HOOK(getLogger(), LobbyGameStateController_HandleMultiplayerLevelLoaderCountdownFinished);
    INSTALL_HOOK(getLogger(), LobbySetupViewController_SetPlayersMissingLevelText);
    INSTALL_HOOK(getLogger(), LobbySetupViewController_SetStartGameEnabled);
    INSTALL_HOOK(getLogger(), LobbySetupViewController_DidActivate);
    INSTALL_HOOK(getLogger(), LevelSelectionNavigationController_Setup);

    INSTALL_HOOK(getLogger(), MenuRpcManager_InvokeSetCountdownEndTime);
    INSTALL_HOOK(getLogger(), MenuRpcManager_InvokeStartLevel);

    INSTALL_HOOK(getLogger(), GameServerPlayerTableCell_SetData);

    INSTALL_HOOK(getLogger(), LobbyGameStateController_HandleMenuRpcManagerSetCountdownEndTime);
    INSTALL_HOOK(getLogger(), ConnectedPlayerManager_HandleSyncTimePacket);

    getLogger().info("Installed all hooks!");

    getLogger().debug("il2cpp_functions::Class_Init: %p"
        "il2cpp_functions::type_equals: %p"
        "il2cpp_functions::value_box: %p"
        "il2cpp_functions::object_unbox: %p", 
        &il2cpp_functions::Class_Init,
        &il2cpp_functions::type_equals,
        &il2cpp_functions::value_box,
        &il2cpp_functions::object_unbox);
}