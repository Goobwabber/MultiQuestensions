#include "main.hpp"
#include "Hooks/Hooks.hpp"
#include "Hooks/SessionManagerAndExtendedPlayerHooks.hpp"
#include "GlobalFields.hpp"
#include "Hooks/EnvironmentAndAvatarHooks.hpp"
#include "UI/CenterScreenLoading.hpp"

#include "Beatmaps/PreviewBeatmapStub.hpp"

#include "GlobalNamespace/IPlatformUserModel.hpp"
#include "GlobalNamespace/LocalNetworkPlayerModel.hpp"
#include "UnityEngine/Resources.hpp"
#include "GlobalNamespace/UserInfo.hpp"

#include "CodegenExtensions/ColorUtility.hpp"

#include "questui/shared/BeatSaberUI.hpp"
#include "questui/shared/CustomTypes/Components/MainThreadScheduler.hpp"
#include "songdownloader/shared/BeatSaverAPI.hpp"
using namespace MultiQuestensions;
using namespace GlobalNamespace;

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
			getLogger().info("Try getting CoverImage from BeatSaver");
			std::string levelid = to_utf8(csstrtostr(preview->get_levelID()));
			BeatSaver::API::GetBeatmapByHashAsync(GetHash(levelid),
                [preview, player, packet](std::optional<BeatSaver::Beatmap> beatmap) {
                    if (beatmap.has_value()) {
                        BeatSaver::API::GetCoverImageAsync(*beatmap, [preview, player, packet](std::vector<uint8_t> bytes) {
                            QuestUI::MainThreadScheduler::Schedule([packet, player, preview, bytes] {
                                if (packet && player && preview && lobbyPlayersDataModel) {
                                    preview->coverImage = QuestUI::BeatSaberUI::VectorToSprite(bytes);
                                    BeatmapCharacteristicSO* characteristic = lobbyPlayersDataModel->beatmapCharacteristicCollection->GetBeatmapCharacteristicBySerializedName(packet->characteristic);
                                    lobbyPlayersDataModel->SetPlayerBeatmapLevel(player->get_userId(), reinterpret_cast<IPreviewBeatmapLevel*>(preview), packet->difficulty, characteristic);
                                }
                                else {
                                    getLogger().error("Error nullptr: packet='%p', player='%p', preview='%p', lobbyPlayersDataModel='%p'", packet, player, preview, lobbyPlayersDataModel);
                                }
                                }
                            );
                            }
                        );
                    }
                    else {
                        QuestUI::MainThreadScheduler::Schedule([packet, player, preview] {
                            if (packet && player && preview) {
                                BeatmapCharacteristicSO* characteristic = lobbyPlayersDataModel->beatmapCharacteristicCollection->GetBeatmapCharacteristicBySerializedName(packet->characteristic);
                                lobbyPlayersDataModel->SetPlayerBeatmapLevel(player->get_userId(), reinterpret_cast<IPreviewBeatmapLevel*>(preview), packet->difficulty, characteristic);
                            }
                            else {
                                getLogger().error("Error nullptr: packet='%p', player='%p', preview='%p', lobbyPlayersDataModel='%p'", packet, player, preview, lobbyPlayersDataModel);
                            }
                            }
                        );
                    }
                }
                );
            return;
        }
        else {
            MultiQuestensions::Beatmaps::PreviewBeatmapPacket* nullpacket = nullptr;
            preview = THROW_UNLESS(il2cpp_utils::New<MultiQuestensions::Beatmaps::PreviewBeatmapStub*>(packet->levelHash, localPreview, nullpacket));
        }
        BeatmapCharacteristicSO* characteristic = lobbyPlayersDataModel->beatmapCharacteristicCollection->GetBeatmapCharacteristicBySerializedName(packet->characteristic);
        //getLogger().debug("Check difficulty as unsigned int: %u", packet->difficulty);
        lobbyPlayersDataModel->SetPlayerBeatmapLevel(player->get_userId(), reinterpret_cast<IPreviewBeatmapLevel*>(preview), packet->difficulty, characteristic);
    }
    catch (const std::runtime_error& e) {
        getLogger().error("REPORT TO ENDER: %s", e.what());
    }
    catch (...) {
        getLogger().debug("REPORT TO ENDER: Unknown exception in HandlePreviewBeatmapPacket");
    }
}


std::map<std::string, SafePtr<Extensions::ExtendedPlayer>> _extendedPlayers;
//System::Collections::Generic::Dictionary_2<Il2CppString*, Extensions::ExtendedPlayer*>* extendedPlayers;
//SafePtr<System::Collections::Generic::Dictionary_2<std::string, Extensions::ExtendedPlayer*>*> extendedPlayersSPTR;
//SafePtr<Extensions::ExtendedPlayer*> localExtendedPlayerSPTR;
SafePtr<Extensions::ExtendedPlayer> localExtendedPlayer;
IPlatformUserModel* platformUserModel;

static void HandleExtendedPlayerPacket(MultiQuestensions::Extensions::ExtendedPlayerPacket* packet, IConnectedPlayer* player) {
    const std::string userId = to_utf8(csstrtostr(player->get_userId()));
    //if (extendedPlayers->ContainsKey(player->get_userId())) {
    if (_extendedPlayers.contains(userId)) {
        SafePtr<Extensions::ExtendedPlayer> extendedPlayer = _extendedPlayers.at(userId);
        //Extensions::ExtendedPlayer* extendedPlayer = extendedPlayers->get_Item(player->get_userId());
        extendedPlayer->_connectedPlayer = player;
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
                    "Different MultiplayerExtensions protocol detected!\r\n"
                    "The player '%s' is using MultiplayerExtensions %s while you are using MultiQuestensions " MPEX_PROTOCOL "\r\n"
                    "For best compatibility all players should use the same version of MultiplayerExtensions.\r\n"
                    "###################################################################",
                    to_utf8(csstrtostr(player->get_userName())).c_str(),
                    to_utf8(csstrtostr(extendedPlayer->mpexVersion)).c_str()
                );
            }
        }
        catch (const std::runtime_error& e) {
            getLogger().error("REPORT TO ENDER: Exception while trying to create ExtendedPlayer: %s", e.what());
        }
        if (extendedPlayer) {
            _extendedPlayers.emplace(userId, extendedPlayer);
            //extendedPlayers->Add(player->get_userId(), extendedPlayer);
            //if (!extendedPlayersSPTR) extendedPlayersSPTR = extendedPlayers;

            getLogger().debug("SetPlayerPlaceColor");
            SetPlayerPlaceColor(reinterpret_cast<IConnectedPlayer*>(extendedPlayer->get_self()), extendedPlayer->get_playerColor(), true);
            getLogger().debug("CreateOrUpdateNameTag");
            // This packet is usually received before the avatar is actually created
            CreateOrUpdateNameTag(reinterpret_cast<IConnectedPlayer*>(extendedPlayer->get_self()));
            getLogger().debug("ExtendedPlayerPacket done");

            //extendedPlayerConnectedEvent::Invoke(extendedPlayer);
        }
    }
}

void HandlePlayerConnected(IConnectedPlayer* player) {
    try {
        getLogger().debug("HandlePlayerConnected");
        if (player) {
            const std::string userId = to_utf8(csstrtostr(player->get_userId()));
            getLogger().info("Player '%s' joined", userId.c_str());
            getLogger().debug("Sending ExtendedPlayerPacket");
            if (localExtendedPlayer->get_platformID() != nullptr)
            {
                Extensions::ExtendedPlayerPacket* localPlayerPacket = Extensions::ExtendedPlayerPacket::Init(localExtendedPlayer->get_platformID(), localExtendedPlayer->get_platform(), localExtendedPlayer->get_playerColor());
                getLogger().debug("LocalPlayer Color is, R: %f G: %f B: %f", localPlayerPacket->playerColor.r, localPlayerPacket->playerColor.g, localPlayerPacket->playerColor.b);
                packetManager->Send(reinterpret_cast<LiteNetLib::Utils::INetSerializable*>(localPlayerPacket));
            }
            getLogger().debug("ExtendedPlayerPacket sent");

            SetPlayerPlaceColor(player, Extensions::ExtendedPlayer::DefaultColor, false);
        }
    }
    catch (const std::runtime_error& e) {
        getLogger().error("REPORT TO ENDER: %s", e.what());
    }
}

void HandlePlayerDisconnected(IConnectedPlayer* player) {
    getLogger().info("Player '%s' left", to_utf8(csstrtostr(player->get_userId())).c_str());
    getLogger().debug("Reseting platform lights");
    SetPlayerPlaceColor(player, UnityEngine::Color::get_black(), true);
    _extendedPlayers.erase(to_utf8(csstrtostr(player->get_userId())).c_str());
}

//void HandleDisconnect(DisconnectedReason* reason) {
//}

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
    //try {
        localExtendedPlayer = Extensions::ExtendedPlayer::CS_ctor(self->get_localPlayer());
        //localExtendedPlayerSPTR = localExtendedPlayer;

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
    //}
    //catch (const std::runtime_error& e) {
    //    getLogger().error("%s", e.what());
    //}
    self->add_playerConnectedEvent(il2cpp_utils::MakeDelegate<System::Action_1<IConnectedPlayer*>*>(classof(System::Action_1<IConnectedPlayer*>*), static_cast<Il2CppObject*>(nullptr), HandlePlayerConnected));
    self->add_playerDisconnectedEvent(il2cpp_utils::MakeDelegate<System::Action_1<IConnectedPlayer*>*>(classof(System::Action_1<IConnectedPlayer*>*), static_cast<Il2CppObject*>(nullptr), HandlePlayerDisconnected));
    //self->add_disconnectedEvent(il2cpp_utils::MakeDelegate<System::Action_1<GlobalNamespace::DisconnectedReason>*>*>(classof(System::Action_1<GlobalNamespace::DisconnectedReason>*>*), static_cast<Il2CppObject*>(nullptr), HandleDisconnect));
}


void MultiQuestensions::Hooks::SessionManagerAndExtendedPlayerHooks() {
    INSTALL_HOOK(getLogger(), SessionManagerStart);
    INSTALL_HOOK(getLogger(), SessionManager_StartSession);
}