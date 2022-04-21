#include "main.hpp"
#include "Hooks/Hooks.hpp"
#include "Hooks/SessionManagerAndExtendedPlayerHooks.hpp"
#include "GlobalFields.hpp"
#include "Hooks/EnvironmentAndAvatarHooks.hpp"
#include "Players/MpexPlayerManager.hpp"
#include "Config.hpp"

#include "GlobalNamespace/IPlatformUserModel.hpp"
#include "GlobalNamespace/LocalNetworkPlayerModel.hpp"
#include "UnityEngine/Resources.hpp"
#include "GlobalNamespace/UserInfo.hpp"
#include "GlobalNamespace/MultiplayerSessionManager_SessionType.hpp"

#include "CodegenExtensions/ColorUtility.hpp"

#include "questui/shared/BeatSaberUI.hpp"
#include "questui/shared/CustomTypes/Components/MainThreadScheduler.hpp"

#include "MultiplayerCore/shared/Networking/MpPacketSerializer.hpp"
#include "MultiplayerCore/shared/Players/MpPlayerManager.hpp"
using namespace MultiQuestensions;
using namespace MultiplayerCore::Networking;
using namespace GlobalNamespace;

// std::map<std::string, SafePtr<Extensions::ExtendedPlayer>> _extendedPlayers;
std::map<std::string, SafePtr<Players::MpexPlayerData>> _mpexPlayerData;
//System::Collections::Generic::Dictionary_2<Il2CppString*, Extensions::ExtendedPlayer*>* extendedPlayers;
//SafePtr<System::Collections::Generic::Dictionary_2<std::string, Extensions::ExtendedPlayer*>*> extendedPlayersSPTR;
//SafePtr<Extensions::ExtendedPlayer*> localExtendedPlayerSPTR;
// SafePtr<Extensions::ExtendedPlayer> localExtendedPlayer;
SafePtr<Players::MpexPlayerData> localMpexPlayerData;
IPlatformUserModel* platformUserModel;

MultiplayerCore::event<GlobalNamespace::IConnectedPlayer*, MultiQuestensions::Players::MpexPlayerData*> MultiQuestensions::Players::MpexPlayerManager::PlayerConnected;

bool MultiQuestensions::Players::MpexPlayerManager::TryGetPlayer(std::string playerId, MultiQuestensions::Players::MpexPlayerData*& player) {
            if (_mpexPlayerData.find(playerId) != _mpexPlayerData.end()) {
                player = static_cast<MultiQuestensions::Players::MpexPlayerData*>(_mpexPlayerData.at(playerId));
                return true;
            }
            return false;
        }

MultiQuestensions::Players::MpexPlayerData* MultiQuestensions::Players::MpexPlayerManager::GetPlayer(std::string playerId) {
            if (_mpexPlayerData.find(playerId) != _mpexPlayerData.end()) {
                return static_cast<MultiQuestensions::Players::MpexPlayerData*>(_mpexPlayerData.at(playerId));
            }
            return nullptr;
        }

static void HandleMpexData(Players::MpexPlayerData* packet, IConnectedPlayer* player) {
    const std::string userId = to_utf8(csstrtostr(player->get_userId()));

    if (_mpexPlayerData.contains(userId)) {
        _mpexPlayerData[userId] = packet;
    }
    else {
        getLogger().info("Received 'MpexPlayerData' from '%s' with color: '%f, %f, %f, %f'", 
        userId.c_str(), 
        packet->Color.r, packet->Color.g, packet->Color.b, packet->Color.a
        );
        _mpexPlayerData.emplace(userId, packet);
    }

    SetPlayerPlaceColor(player, packet->Color, true);
    getLogger().info("Calling event 'PlayerConnected'");
    MultiQuestensions::Players::MpexPlayerManager::PlayerConnected(player, packet);
}

void HandlePlayerConnected(IConnectedPlayer* player) {
    try {
        getLogger().debug("HandlePlayerConnected");
        if (player) {
            const std::string userId = player->get_userId();
            getLogger().info("Player '%s' joined", userId.c_str());
            getLogger().debug("Sending MpexPlayerData");
            if (localMpexPlayerData)
            {
                MultiplayerCore::mpPacketSerializer->Send(localMpexPlayerData->ToSerializable());
            }
            getLogger().debug("MpexPlayerData sent");

            SetPlayerPlaceColor(player, Config::DefaultPlayerColor, false);
        }
    }
    catch (const std::runtime_error& e) {
        getLogger().error("REPORT TO ENDER: %s", e.what());
    }
}

void HandlePlayerDisconnected(IConnectedPlayer* player) {
    getLogger().info("Reseting platform lights for Player '%s'", std::string(player->get_userId()).c_str());
    SetPlayerPlaceColor(player, UnityEngine::Color::get_black(), true);
    _mpexPlayerData.erase(std::string(player->get_userId()).c_str());
}

// void HandleDisconnect(DisconnectedReason* reason) {

// }

MAKE_HOOK_MATCH(SessionManagerStart, &MultiplayerSessionManager::Start, void, MultiplayerSessionManager* self) {

    sessionManager = self;
    SessionManagerStart(sessionManager);
}

MAKE_HOOK_MATCH(SessionManager_StartSession, &MultiplayerSessionManager::StartSession, void, MultiplayerSessionManager* self, MultiplayerSessionManager_SessionType sessionType, ConnectedPlayerManager* connectedPlayerManager) {
    SessionManager_StartSession(self, sessionType, connectedPlayerManager);
    getLogger().debug("MultiplayerSessionManager.StartSession, creating localMpexPlayerData");
    //try {
        localMpexPlayerData = Players::MpexPlayerData::New_ctor();
        // localExtendedPlayer = Extensions::ExtendedPlayer::CS_ctor(self->get_localPlayer());
        //localExtendedPlayerSPTR = localExtendedPlayer;

        localMpexPlayerData->Color = config.getPlayerColor();
        // if (!UnityEngine::ColorUtility::TryParseHtmlString(il2cpp_utils::newcsstr(getConfig().config["color"].GetString()), localExtendedPlayer->playerColor))
        //     localExtendedPlayer->playerColor = UnityEngine::Color(0.031f, 0.752f, 1.0f, 1.0f);

        // static auto localNetworkPlayerModel = UnityEngine::Resources::FindObjectsOfTypeAll<LocalNetworkPlayerModel*>().get(0);
        // static auto UserInfoTask = localNetworkPlayerModel->dyn__platformUserModel()->GetUserInfo();
        // static auto action = il2cpp_utils::MakeDelegate<System::Action_1<System::Threading::Tasks::Task*>*>(classof(System::Action_1<System::Threading::Tasks::Task*>*), (std::function<void(System::Threading::Tasks::Task_1<GlobalNamespace::UserInfo*>*)>)[&](System::Threading::Tasks::Task_1<GlobalNamespace::UserInfo*>* userInfoTask) {
        //     auto userInfo = userInfoTask->get_Result();
        //     if (userInfo) {
        //         localExtendedPlayer->platformID = userInfo->dyn_platformUserId();
        //         localExtendedPlayer->platform = (Extensions::Platform)userInfo->dyn_platform().value;
        //     }
        //     else getLogger().error("Failed to get local network player!");
        //     }
        // );
        // reinterpret_cast<System::Threading::Tasks::Task*>(UserInfoTask)->ContinueWith(action);

        self->SetLocalPlayerState("modded", true);
      
        //}
    //catch (const std::runtime_error& e) {
    //    getLogger().error("%s", e.what());
    //}

    MultiplayerCore::Networking::MpPacketSerializer::RegisterCallbackStatic<MultiQuestensions::Players::MpexPlayerData*>(HandleMpexData);

    self->add_playerConnectedEvent(il2cpp_utils::MakeDelegate<System::Action_1<IConnectedPlayer*>*>(classof(System::Action_1<IConnectedPlayer*>*), static_cast<Il2CppObject*>(nullptr), HandlePlayerConnected));
    self->add_playerDisconnectedEvent(il2cpp_utils::MakeDelegate<System::Action_1<IConnectedPlayer*>*>(classof(System::Action_1<IConnectedPlayer*>*), static_cast<Il2CppObject*>(nullptr), HandlePlayerDisconnected));
    // self->add_disconnectedEvent(il2cpp_utils::MakeDelegate<System::Action_1<GlobalNamespace::DisconnectedReason>*>*>(classof(System::Action_1<GlobalNamespace::DisconnectedReason>*>*), static_cast<Il2CppObject*>(nullptr), HandleDisconnect));
}


void MultiQuestensions::Hooks::SessionManagerAndExtendedPlayerHooks() {
    INSTALL_HOOK(getLogger(), SessionManagerStart);
    INSTALL_HOOK(getLogger(), SessionManager_StartSession);
}