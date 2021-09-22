#include "main.hpp"
#include "Hooks/Hooks.hpp"
#include "Beatmaps/PreviewBeatmapPacket.hpp"
#include "Beatmaps/PreviewBeatmapStub.hpp"
#include "UI/LobbySetupPanel.hpp"

#include "GlobalNamespace/ConnectedPlayerManager.hpp"
#include "GlobalNamespace/LocalNetworkPlayerModel.hpp"
#include "GlobalNamespace/IPlatformUserModel.hpp"
#include "GlobalNamespace/UserInfo.hpp"
#include "UnityEngine/Resources.hpp"

#include "beatsaber-hook/shared/utils/il2cpp-utils.hpp"
#include "custom-types/shared/register.hpp"

#include "CodegenExtensions/ColorUtility.hpp"

//#include "GlobalNamespace/LobbySetupViewController.hpp"
//#include "GlobalNamespace/LobbySetupViewController_CannotStartGameReason.hpp"
using namespace GlobalNamespace;
using namespace System::Threading::Tasks;
using namespace MultiQuestensions;

#ifndef VERSION
#warning No Version set
#define VERSION "0.1.0"
#endif
#ifndef MPEX_PROTOCOL
#warning No MPEX_PROTOCOL set
#define MPEX_PROTOCOL "0.6.1"
#endif

ModInfo modInfo; // Stores the ID and version of our mod, and is sent to the modloader upon startup

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



namespace MultiQuestensions {
    // Plugin setup stuff
    GlobalNamespace::MultiplayerSessionManager* sessionManager;
    GlobalNamespace::LobbyPlayersDataModel* lobbyPlayersDataModel;
    MultiQuestensions::PacketManager* packetManager;
    GlobalNamespace::LobbyGameStateController* lobbyGameStateController;

    std::string moddedState = "modded";
}

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

std::map<std::string, Extensions::ExtendedPlayer*> _extendedPlayers;
Extensions::ExtendedPlayer* localExtendedPlayer;
IPlatformUserModel* platformUserModel;

static void HandleExtendedPlayerPacket(MultiQuestensions::Extensions::ExtendedPlayerPacket* packet, IConnectedPlayer* player) {
    const std::string userId = to_utf8(csstrtostr(player->get_userId()));
    if (_extendedPlayers.contains(userId)) {
        Extensions::ExtendedPlayer* extendedPlayer = _extendedPlayers[userId];
        extendedPlayer->platformID = packet->platformID;
        extendedPlayer->platform = packet->platform;
        extendedPlayer->playerColor = packet->playerColor;
        extendedPlayer->mpexVersion = packet->mpexVersion;
    }
    else {
        getLogger().info("Received 'ExtendedPlayerPacket' from '%s' with platformID: '%s'  mpexVersion: '%s'", 
            to_utf8(csstrtostr(player->get_userId())).c_str(),
            to_utf8(csstrtostr(packet->platformID)).c_str(),
            to_utf8(csstrtostr(packet->mpexVersion)).c_str()
        );
        Extensions::ExtendedPlayer* extendedPlayer;
        try {
            extendedPlayer = Extensions::ExtendedPlayer::CS_ctor(player, packet->platformID, packet->platform, packet->mpexVersion, packet->playerColor);
            if (to_utf8(csstrtostr(extendedPlayer->mpexVersion)) != MPEX_PROTOCOL)
            {
                getLogger().warning(
                    "###################################################################\r\n"
                    "Different MultiplayerExtensions version detected!\r\n"
                    "The player '%s' is using MultiplayerExtensions %s while you are using MultiQuestensions " MPEX_PROTOCOL "\r\n"
                    "For best compatibility all players should use the same version of MultiplayerExtensions.\r\n"
                    "###################################################################",
                    to_utf8(csstrtostr(player->get_userName())).c_str(),
                    to_utf8(csstrtostr(extendedPlayer->mpexVersion)).c_str()
                );
            }
        }
        catch (const std::runtime_error& e) {
            getLogger().error("Exception while trying to create ExtendedPlayer: %s", e.what());
        }
        if (extendedPlayer) {
            _extendedPlayers[userId] = extendedPlayer;
            //extendedPlayerConnectedEvent::Invoke(extendedPlayer);
        }
    }
}

void HandlePlayerConnected(GlobalNamespace::IConnectedPlayer* player) {
    getLogger().info("Player '%s' joined", to_utf8(csstrtostr(player->get_userId())).c_str());
    getLogger().debug("Sending ExtendedPlayerPacket");
    if (localExtendedPlayer->get_platformID() != nullptr)
    {
        try {
            Extensions::ExtendedPlayerPacket* localPlayerPacket = Extensions::ExtendedPlayerPacket::Init(localExtendedPlayer->get_platformID(), localExtendedPlayer->get_platform(), localExtendedPlayer->get_playerColor());
            getLogger().debug("LocalPlayer Color is, R: %f G: %f B: %f", localPlayerPacket->playerColor.r, localPlayerPacket->playerColor.g, localPlayerPacket->playerColor.b);
            packetManager->Send(reinterpret_cast<LiteNetLib::Utils::INetSerializable*>(localPlayerPacket));
        }
        catch (const std::runtime_error& e) {
            getLogger().error("%s", e.what());
        }
    }
    getLogger().debug("ExtendedPlayerPacket sent");
}

MAKE_HOOK_MATCH(SessionManagerStart, &MultiplayerSessionManager::Start, void, MultiplayerSessionManager* self) {

    sessionManager = self;
    SessionManagerStart(sessionManager);
    packetManager = new PacketManager(sessionManager);


    packetManager->RegisterCallback<Beatmaps::PreviewBeatmapPacket*>("MultiplayerExtensions.Beatmaps.PreviewBeatmapPacket", HandlePreviewBeatmapPacket);
    packetManager->RegisterCallback<Extensions::ExtendedPlayerPacket*>("MultiplayerExtensions.Extensions.ExtendedPlayerPacket", HandleExtendedPlayerPacket);
}

MAKE_HOOK_FIND_VERBOSE(SessionManager_StartSession, il2cpp_utils::FindMethodUnsafe("", "MultiplayerSessionManager", "StartSession", 1), void, MultiplayerSessionManager* self, ConnectedPlayerManager* connectedPlayerManager) {
    SessionManager_StartSession(self, connectedPlayerManager);
    getLogger().debug("MultiplayerSessionManager.StartSession, creating localExtendedPlayerPacket");
    try {
        localExtendedPlayer = Extensions::ExtendedPlayer::CS_ctor(self->get_localPlayer());

        if (!UnityEngine::ColorUtility::TryParseHtmlString(il2cpp_utils::newcsstr(getConfig().config["color"].GetString()), localExtendedPlayer->playerColor))
            localExtendedPlayer->playerColor = UnityEngine::Color(0.031f, 0.752f, 1.0f);

        static auto localNetworkPlayerModel = UnityEngine::Resources::FindObjectsOfTypeAll<LocalNetworkPlayerModel*>()->get(0);
        static auto UserInfoTask = localNetworkPlayerModel->platformUserModel->GetUserInfo();
        static auto action = il2cpp_utils::MakeDelegate<System::Action_1<System::Threading::Tasks::Task*>*>(classof(System::Action_1<System::Threading::Tasks::Task*>*), (std::function<void(System::Threading::Tasks::Task_1<GlobalNamespace::UserInfo*>*)>)[&](System::Threading::Tasks::Task_1<GlobalNamespace::UserInfo*>* userInfoTask) {
            auto userInfo = userInfoTask->get_Result();
            if (userInfo) {
                localExtendedPlayer->platformID = userInfo->platformUserId;
                localExtendedPlayer->platform = (Extensions::Platform)userInfo->platform.value;
            }
            else getLogger().error("Failed to get local network player!");
            }
        );

        reinterpret_cast<System::Threading::Tasks::Task*>(UserInfoTask)->ContinueWith(action);
    }
    catch (const std::runtime_error& e) {
        getLogger().error("%s", e.what());
    }
    // System::Action_1<GlobalNamespace::IConnectedPlayer*>*
    self->add_playerConnectedEvent(il2cpp_utils::MakeDelegate<System::Action_1<GlobalNamespace::IConnectedPlayer*>*>(classof(System::Action_1<GlobalNamespace::IConnectedPlayer*>*), static_cast<Il2CppObject*>(nullptr), HandlePlayerConnected));
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
                    self->menuRpcManager->RecommendBeatmap(BeatmapIdentifierNetSerializable::New_ctor(levelId, characteristic->get_serializedName(), beatmapDifficulty));
                    packetManager->Send(reinterpret_cast<LiteNetLib::Utils::INetSerializable*>(packet));
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

MAKE_HOOK_MATCH(MultiplayerLobbyConnectionController_CreateParty, &MultiplayerLobbyConnectionController::CreateParty, void, MultiplayerLobbyConnectionController* self, CreateServerFormData data) {
    static auto maskStr = il2cpp_utils::newcsstr<il2cpp_utils::CreationType::Manual>("custom_levelpack_CustomLevels");
    data.songPacks = SongPackMask::get_all() | SongPackMask(maskStr);
    MultiplayerLobbyConnectionController_CreateParty(self, data);
}

MAKE_HOOK_MATCH(MultiplayerLobbyConnectionController_ConnectToMatchmaking, &MultiplayerLobbyConnectionController::ConnectToMatchmaking, void, MultiplayerLobbyConnectionController* self, BeatmapDifficultyMask beatmapDifficultyMask, SongPackMask songPackMask, bool allowSongSelection) {
    static auto maskStr = il2cpp_utils::newcsstr<il2cpp_utils::CreationType::Manual>("custom_levelpack_CustomLevels");
    songPackMask = songPackMask | SongPackMask(maskStr);
    MultiplayerLobbyConnectionController_ConnectToMatchmaking(self, beatmapDifficultyMask, songPackMask, allowSongSelection);
}

// Show the custom levels tab in multiplayer
MAKE_HOOK_MATCH(LevelSelectionNavigationController_Setup, &LevelSelectionNavigationController::Setup, void, LevelSelectionNavigationController* self,
    SongPackMask songPackMask, BeatmapDifficultyMask allowedBeatmapDifficultyMask, Array<GlobalNamespace::BeatmapCharacteristicSO*>* notAllowedCharacteristics,
    bool hidePacksIfOneOrNone, bool hidePracticeButton, bool showPlayerStatsInDetailView, Il2CppString* actionButtonText, IBeatmapLevelPack* levelPackToBeSelectedAfterPresent,
    SelectLevelCategoryViewController::LevelCategory startLevelCategory, IPreviewBeatmapLevel* beatmapLevelToBeSelectedAfterPresent, bool enableCustomLevels) {
    getLogger().info("LevelSelectionNavigationController_Setup setting custom songs . . .");
    LevelSelectionNavigationController_Setup(self, songPackMask, allowedBeatmapDifficultyMask, notAllowedCharacteristics, hidePacksIfOneOrNone, hidePracticeButton, showPlayerStatsInDetailView,
        actionButtonText, levelPackToBeSelectedAfterPresent, startLevelCategory, beatmapLevelToBeSelectedAfterPresent, songPackMask.Contains(il2cpp_utils::newcsstr("custom_levelpack_CustomLevels")));
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

namespace MultiQuestensions {
    bool HasSong(std::string levelId) {
        for (auto& song : RuntimeSongLoader::API::GetLoadedSongs()) {
            if (to_utf8(csstrtostr(song->levelID)) == levelId) {
                return true;
            }
        }
        return false;
    }
}

std::vector<std::string> DownloadedSongIds;

MAKE_HOOK_MATCH(MultiplayerLevelLoader_LoadLevel, &MultiplayerLevelLoader::LoadLevel, void, MultiplayerLevelLoader* self, BeatmapIdentifierNetSerializable* beatmapId, GameplayModifiers* gameplayModifiers, float initialStartTime) {
    std::string levelId = to_utf8(csstrtostr(beatmapId->levelID));
    getLogger().info("MultiplayerLevelLoader_LoadLevel: %s", levelId.c_str());
    std::string hash = GetHash(levelId);
    if (IsCustomLevel(levelId)) {
        if (HasSong(levelId)) {
            getLogger().debug("MultiplayerLevelLoader_LoadLevel, HasSong, calling original");
            MultiplayerLevelLoader_LoadLevel(self, beatmapId, gameplayModifiers, initialStartTime);
            return;
        }
        else {
            BeatSaver::API::GetBeatmapByHashAsync(hash,
                [self, beatmapId, gameplayModifiers, initialStartTime, hash](std::optional<BeatSaver::Beatmap> beatmapOpt) {
                    if (beatmapOpt.has_value()) {
                        auto beatmap = beatmapOpt.value();
                        auto beatmapName = beatmap.GetName();
                        getLogger().info("Downloading map: %s", beatmap.GetName().c_str());
                        BeatSaver::API::DownloadBeatmapAsync(beatmap,
                            [self, beatmapId, gameplayModifiers, initialStartTime, beatmapName, hash](bool error) {
                                if (error) {
                                    getLogger().info("Failed downloading map: %s", beatmapName.c_str());
                                }
                                else {
                                    getLogger().info("Downloaded map: %s", beatmapName.c_str());
                                    DownloadedSongIds.emplace_back(hash);
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


MAKE_HOOK_MATCH(LobbyGameStateController_HandleMultiplayerLevelLoaderCountdownFinished, &LobbyGameStateController::HandleMultiplayerLevelLoaderCountdownFinished, void, LobbyGameStateController* self, GlobalNamespace::IPreviewBeatmapLevel* previewBeatmapLevel, GlobalNamespace::BeatmapDifficulty beatmapDifficulty, GlobalNamespace::BeatmapCharacteristicSO* beatmapCharacteristic, GlobalNamespace::IDifficultyBeatmap* difficultyBeatmap, GlobalNamespace::GameplayModifiers* gameplayModifiers) {
    getLogger().debug("LobbyGameStateController_HandleMultiplayerLevelLoaderCountdownFinished");
    self->menuRpcManager->SetIsEntitledToLevel(previewBeatmapLevel->get_levelID(), EntitlementsStatus::Ok);
    lobbyGameStateController = self;
    loadingPreviewBeatmapLevel = previewBeatmapLevel;
    loadingBeatmapDifficulty = beatmapDifficulty;
    loadingBeatmapCharacteristic = beatmapCharacteristic;
    loadingDifficultyBeatmap = difficultyBeatmap;
    loadingGameplayModifiers = gameplayModifiers;
    bool entitlementStatusOK = true;
    std::string LevelID = to_utf8(csstrtostr(self->startedBeatmapId->get_levelID()));
    for (int i = 0; i < sessionManager->connectedPlayers->get_Count(); i++) {
        std::string UserID =  to_utf8(csstrtostr(sessionManager->connectedPlayers->get_Item(i)->get_userId()));
        if (entitlementDictionary[UserID][LevelID] != EntitlementsStatus::Ok) entitlementStatusOK = false;
    }
    if (entitlementStatusOK) {
        lobbyGameStateController = nullptr;
        loadingPreviewBeatmapLevel = nullptr;
        //loadingBeatmapDifficulty = beatmapDifficulty;
        loadingBeatmapCharacteristic = nullptr;
        loadingDifficultyBeatmap = nullptr;
        loadingGameplayModifiers = nullptr;

        // call original method
        LobbyGameStateController_HandleMultiplayerLevelLoaderCountdownFinished(self, previewBeatmapLevel, beatmapDifficulty, beatmapCharacteristic, difficultyBeatmap, gameplayModifiers);
    }
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


void saveDefaultConfig() {
    getLogger().info("Creating config file...");
    ConfigDocument& config = getConfig().config;

    if (config.HasMember("color")) {
        getLogger().info("Config file already exists.");
        return;
    }  

    config.RemoveAllMembers();
    config.SetObject();
    auto& allocator = config.GetAllocator();

    config.AddMember("lagreducer", false, allocator);
    config.AddMember("color", "#08C0FF", allocator);

    //config.AddMember("freemod", false, allocator);
    //config.AddMember("hostpick", true, allocator);
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
    Hooks::Install_Hooks();
    INSTALL_HOOK(getLogger(), SessionManagerStart);
    INSTALL_HOOK(getLogger(), SessionManager_StartSession);
    INSTALL_HOOK(getLogger(), LobbyPlayersActivate);
    INSTALL_HOOK(getLogger(), LobbyPlayersSetLocalBeatmap);
    INSTALL_HOOK(getLogger(), LobbyPlayersSelectedBeatmap);

    INSTALL_HOOK(getLogger(), MultiplayerLevelLoader_LoadLevel);
    //INSTALL_HOOK(getLogger(), NetworkPlayerEntitlementChecker_GetPlayerLevelEntitlementsAsync);
    if (Modloader::getMods().find("BeatTogether") != Modloader::getMods().end()) {
        getLogger().info("Hello BeatTogether!");
    }
    else getLogger().warning("BeatTogether was not found! Is Multiplayer modded?");

    INSTALL_HOOK(getLogger(), LobbyGameStateController_HandleMultiplayerLevelLoaderCountdownFinished);
    INSTALL_HOOK(getLogger(), LobbySetupViewController_SetPlayersMissingLevelText);
    INSTALL_HOOK(getLogger(), LobbySetupViewController_SetStartGameEnabled);
    INSTALL_HOOK(getLogger(), LobbySetupViewController_DidActivate);

    INSTALL_HOOK(getLogger(), MultiplayerLobbyConnectionController_CreateParty);
    INSTALL_HOOK(getLogger(), MultiplayerLobbyConnectionController_ConnectToMatchmaking);

    INSTALL_HOOK(getLogger(), LevelSelectionNavigationController_Setup);

#pragma region Debug Hooks
    INSTALL_HOOK(getLogger(), MenuRpcManager_InvokeSetCountdownEndTime);
    INSTALL_HOOK(getLogger(), MenuRpcManager_InvokeStartLevel);
    INSTALL_HOOK(getLogger(), LobbyGameStateController_HandleMenuRpcManagerSetCountdownEndTime);
    INSTALL_HOOK(getLogger(), ConnectedPlayerManager_HandleSyncTimePacket);
#pragma endregion


    INSTALL_HOOK(getLogger(), GameServerPlayerTableCell_SetData);

    getLogger().info("Installed all hooks!");
}