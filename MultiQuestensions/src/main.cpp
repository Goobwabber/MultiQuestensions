#include "main.hpp"
#include "Beatmaps/PreviewBeatmapPacket.hpp"
#include "Beatmaps/PreviewBeatmapStub.hpp"
#include "Packets/PacketManager.hpp"
#include "UI/HostLobbySetupPanel.hpp"

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
std::string questState = "platformquest";
std::string customSongsState = "customsongs";
std::string freeModState = "freemod";
std::string hostPickState = "hostpick";

Il2CppString* getCustomSongsStateStr() {
    static auto* customSongStateStr = il2cpp_utils::newcsstr<il2cpp_utils::CreationType::Manual>("customsongs");
    return customSongStateStr;
}

Il2CppString* getFreeModStateStr() {
    static auto* freeModStateStr = il2cpp_utils::newcsstr<il2cpp_utils::CreationType::Manual>("freemod");
    return freeModStateStr;
}

Il2CppString* getHostPickStateStr() {
    static auto* hostPickStateStr = il2cpp_utils::newcsstr<il2cpp_utils::CreationType::Manual>("hostpick");
    return hostPickStateStr;
}

bool customSongsEnabled = true;

//using PD_ValueCollection = System::Collections::Generic::Dictionary_2<Il2CppString*, ILobbyPlayerDataModel*>::ValueCollection;

MultiQuestensions::Beatmaps::PreviewBeatmapStub* GetExistingPreview(Il2CppString* levelId) {
    for (int i = 0; i < sessionManager->connectedPlayers->get_Count(); i++) {
        getLogger().debug(__FILE__ " Line: %d", __LINE__);
        ILevelGameplaySetupData* playerData = reinterpret_cast<ILevelGameplaySetupData*>(lobbyPlayersDataModel->playersData->get_Item(sessionManager->connectedPlayers->get_Item(i)->get_userId()));
        getLogger().debug(__FILE__ " Line: %d", __LINE__);
        if (playerData->get_beatmapLevel() != nullptr && playerData->get_beatmapLevel()->get_levelID() == levelId) {
            getLogger().debug(__FILE__ " Line: %d", __LINE__);
            if (il2cpp_functions::class_is_assignable_from(classof(MultiQuestensions::Beatmaps::PreviewBeatmapStub*), il2cpp_functions::object_get_class(reinterpret_cast<Il2CppObject*>(playerData->get_beatmapLevel())))) {
                getLogger().debug(__FILE__ " Line: %d", __LINE__);
                return reinterpret_cast<MultiQuestensions::Beatmaps::PreviewBeatmapStub*>(playerData->get_beatmapLevel());
            }
        }
    }
    getLogger().debug("Return nullptr " __FILE__ " Line: %d", __LINE__);
    return nullptr;
}

/*
* 
* 
*     //PD_ValueCollection* lPDM = lobbyPlayersDataModel->playersData->get_Values();
    //int count = THROW_UNLESS(il2cpp_utils::GetPropertyValue<int>(lPDM, "count"));
    //                                                  System.Collections.Generic::Dictionary`2/ValueCollection<string, ILobbyPlayerDataModel>
    //const MethodInfo* getCount = il2cpp_utils::FindMethodUnsafe("System.Collections.Generic", "Dictionary`2/ValueCollection<string, ILobbyPlayerDataModel>", "get_Count", 0);
    //const MethodInfo* getCount = il2cpp_utils::FindMethodUnsafe(lPDM, "get_Count", 0);
    //Il2CppClass* ValueCollection = il2cpp_utils::GetClassFromName("System.Collections.Generic", "Dictionary`2/ValueCollection");
    //const MethodInfo* getCount = il2cpp_utils::FindMethodUnsafe(ValueCollection, "get_Count", 0);
    //int count = THROW_UNLESS(il2cpp_utils::RunMethod<int>(lPDM, getCount));

* 
* 
*/

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

//static void HandlePlayerStateChanged(GlobalNamespace::IConnectedPlayer* player) {
//    if (player->get_isConnectionOwner()) {
//        customSongsEnabled = player->HasState(il2cpp_utils::newcsstr(customSongsState));
//    }
//}

// Checks if a MPEX player has customsongs enabled or not
MAKE_HOOK_MATCH(MultiplayerSessionManager_HandlePlayerStateChanged, &MultiplayerSessionManager::HandlePlayerStateChanged, void, MultiplayerSessionManager* self, IConnectedPlayer* player) {
    MultiplayerSessionManager_HandlePlayerStateChanged(self, player);
    if (player->get_isConnectionOwner()) {
        customSongsEnabled = player->HasState(getCustomSongsStateStr());
    }
}

MAKE_HOOK_MATCH(SessionManagerStart, &MultiplayerSessionManager::Start, void, MultiplayerSessionManager* self) {

    sessionManager = self;
    SessionManagerStart(sessionManager);
    packetManager = new MultiQuestensions::PacketManager(sessionManager);

    customSongsEnabled = getConfig().config["customsongs"].GetBool();

    getLogger().debug("customsongs: %d", customSongsEnabled);
    
    self->SetLocalPlayerState(il2cpp_utils::newcsstr(moddedState), true);
    self->SetLocalPlayerState(il2cpp_utils::newcsstr(questState), true);
    self->SetLocalPlayerState(getCustomSongsStateStr(), customSongsEnabled);
    self->SetLocalPlayerState(getFreeModStateStr(), false);
    self->SetLocalPlayerState(getHostPickStateStr(), true);
    //self->SetLocalPlayerState(il2cpp_utils::newcsstr(customSongsState), true);

    //self->add_playerStateChangedEvent(il2cpp_utils::MakeAction<System::Action_1<GlobalNamespace::IConnectedPlayer*>*>(HandlePlayerStateChanged));
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
                                                                // ^ Also crash here
        if (preview != nullptr) {
            self->SetPlayerBeatmapLevel(self->get_localUserId(), reinterpret_cast<IPreviewBeatmapLevel*>(preview), beatmapDifficulty, characteristic);
            self->menuRpcManager->SetSelectedBeatmap(BeatmapIdentifierNetSerializable::New_ctor(levelId, characteristic->get_serializedName(), beatmapDifficulty));
            return;
        }
        else {
            IPreviewBeatmapLevel* localIPreview = self->beatmapLevelsModel->GetLevelPreviewForLevelId(levelId);
            if (localIPreview != nullptr) {
                try {
                    MultiQuestensions::Beatmaps::PreviewBeatmapPacket* nullPacket = nullptr;
                    MultiQuestensions::Beatmaps::PreviewBeatmapStub* previewStub = THROW_UNLESS(il2cpp_utils::New<MultiQuestensions::Beatmaps::PreviewBeatmapStub*>(hash, localIPreview, nullPacket));
                    //MultiQuestensions::Beatmaps::PreviewBeatmapStub* previewStub = MultiQuestensions::Beatmaps::PreviewBeatmapStub::New_ctor(hash, localIPreview);
                    getLogger().debug("Check 'LobbyPlayersSetLocalBeatmap' levelID: %s", to_utf8(csstrtostr(reinterpret_cast<IPreviewBeatmapLevel*>(previewStub)->get_levelID())).c_str());
                    self->SetPlayerBeatmapLevel(self->get_localUserId(), reinterpret_cast<IPreviewBeatmapLevel*>(previewStub), beatmapDifficulty, characteristic);
                    MultiQuestensions::Beatmaps::PreviewBeatmapPacket* packet = previewStub->GetPacket(characteristic->get_serializedName(), beatmapDifficulty);
                    packetManager->Send(reinterpret_cast<LiteNetLib::Utils::INetSerializable*>(packet));
                    self->menuRpcManager->SetSelectedBeatmap(BeatmapIdentifierNetSerializable::New_ctor(levelId, characteristic->get_serializedName(), beatmapDifficulty));
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

// LobbyPlayersDataModel HandleMenuRpcManagerSelectedBeatmap (DONT REMOVE THIS, without it a player's selected map will be cleared)
MAKE_HOOK_MATCH(LobbyPlayersSelectedBeatmap, &LobbyPlayersDataModel::HandleMenuRpcManagerRecommendBeatmap, void, LobbyPlayersDataModel* self, Il2CppString* userId, BeatmapIdentifierNetSerializable* beatmapId) {
    IPreviewBeatmapLevel* localPreview = self->beatmapLevelsModel->GetLevelPreviewForLevelId(beatmapId->levelID);
    if (localPreview != nullptr) {
        LobbyPlayersSelectedBeatmap(self, userId, beatmapId);
    }
}

MAKE_HOOK_MATCH(LobbySetupViewController_DidActivate, &LobbySetupViewController::DidActivate, void, LobbySetupViewController* self, bool firstActivation, bool addedToHierarchy, bool screenSystemEnabling) {
    LobbySetupViewController_DidActivate(self, firstActivation, addedToHierarchy, screenSystemEnabling);
    MultiQuestensions::UI::HostLobbySetupPanel::AddSetupPanel(self->get_rectTransform(), sessionManager);
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


//// Prevent the button becoming shown when we're force disabling it, as pressing it would crash
//MAKE_HOOK_MATCH(LobbySetupViewController_SetStartGameEnabled, &LobbySetupViewController::SetStartGameEnabled, void, LobbySetupViewController* self, bool startGameEnabled, LobbySetupViewController::CannotStartGameReason cannotStartGameReason) {
//    getLogger().info("LobbySetupViewController_SetStartGameEnabled. Enabled: %d. Reason: %d", startGameEnabled, (int)cannotStartGameReason);
//    if (isMissingLevel && cannotStartGameReason == LobbySetupViewController::CannotStartGameReason::None) {
//        getLogger().info("Game attempted to enable the play button when the level was missing, stopping it!");
//        startGameEnabled = false;
//        cannotStartGameReason = LobbySetupViewController::CannotStartGameReason::DoNotOwnSong;
//    }
//    LobbySetupViewController_SetStartGameEnabled(self, startGameEnabled, cannotStartGameReason);
//}

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

std::string GetHash(const std::string& levelId) {
    return levelId.substr(RuntimeSongLoader::API::GetCustomLevelsPrefix().length(), levelId.length() - RuntimeSongLoader::API::GetCustomLevelsPrefix().length());
}

MAKE_HOOK_MATCH(MultiplayerLevelLoader_LoadLevel, &MultiplayerLevelLoader::LoadLevel, void, MultiplayerLevelLoader* self, BeatmapIdentifierNetSerializable* beatmapId, GameplayModifiers* gameplayModifiers, float initialStartTime) {
    std::string levelId = to_utf8(csstrtostr(beatmapId->levelID));
    getLogger().info("MultiplayerLevelLoader_LoadLevel: %s", levelId.c_str());
    if (IsCustomLevel(levelId)) {
        if (HasSong(levelId)) {
            MultiplayerLevelLoader_LoadLevel(self, beatmapId, gameplayModifiers, initialStartTime);
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
                                                    MultiplayerLevelLoader_LoadLevel(self, beatmapId, gameplayModifiers, initialStartTime);
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

System::Action_3<::Il2CppString*, ::Il2CppString*, EntitlementsStatus>* entitlementAction;

MAKE_HOOK_MATCH(NetworkPlayerEntitlementChecker_Start, &NetworkPlayerEntitlementChecker::Start, void, NetworkPlayerEntitlementChecker* self) {
    entitlementAction = il2cpp_utils::MakeDelegate<System::Action_3<::Il2CppString*, ::Il2CppString*, EntitlementsStatus>*>(classof(System::Action_3<::Il2CppString*, ::Il2CppString*, EntitlementsStatus>*), (std::function<void(Il2CppString*, Il2CppString*, EntitlementsStatus)>) [&](Il2CppString* userId, Il2CppString* beatmapId, EntitlementsStatus status) {
        playerEntitlements.emplace_back(status);
        });
    self->rpcManager->add_setIsEntitledToLevelEvent(entitlementAction);
    NetworkPlayerEntitlementChecker_Start(self);
}

MAKE_HOOK_MATCH(NetworkPlayerEntitlementChecker_OnDestroy, &NetworkPlayerEntitlementChecker::OnDestroy, void, NetworkPlayerEntitlementChecker* self) {
    if (entitlementAction)
        playerEntitlements.clear();
        self->rpcManager->remove_setIsEntitledToLevelEvent(entitlementAction);
    NetworkPlayerEntitlementChecker_OnDestroy(self);
}

MAKE_HOOK_MATCH(LobbyGameStateController_HandleMultiplayerLevelLoaderCountdownFinished, &LobbyGameStateController::HandleMultiplayerLevelLoaderCountdownFinished, void, LobbyGameStateController* self, GlobalNamespace::IPreviewBeatmapLevel* previewBeatmapLevel, GlobalNamespace::BeatmapDifficulty beatmapDifficulty, GlobalNamespace::BeatmapCharacteristicSO* beatmapCharacteristic, GlobalNamespace::IDifficultyBeatmap* difficultyBeatmap, GlobalNamespace::GameplayModifiers* gameplayModifiers) {
    // TODO: I honestly forgot what I had to add in here
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
    INSTALL_HOOK(getLogger(), NetworkPlayerEntitlementChecker_Start);
    INSTALL_HOOK(getLogger(), NetworkPlayerEntitlementChecker_OnDestroy);
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
    INSTALL_HOOK(getLogger(), MultiplayerSessionManager_HandlePlayerStateChanged);

    getLogger().info("Installed all hooks!");
}