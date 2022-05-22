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
#include "GlobalNamespace/DisconnectedReason.hpp"


#include "CodegenExtensions/ColorUtility.hpp"

#include "questui/shared/BeatSaberUI.hpp"
#include "questui/shared/CustomTypes/Components/MainThreadScheduler.hpp"

#include "MultiplayerCore/shared/Networking/MpPacketSerializer.hpp"
#include "MultiplayerCore/shared/CodegenExtensions/EnumUtils.hpp"
#include "Players/MpexPlayerManager.hpp"
#include "Players/MpPlayerData.hpp"

using namespace MultiQuestensions;
using namespace MultiplayerCore::Networking;
using namespace GlobalNamespace;

SafePtr<Players::MpexPlayerData> localMpexPlayerData;
std::map<std::string, SafePtr<Players::MpexPlayerData>> _mpexPlayerData;

SafePtr<MultiQuestensions::Players::MpPlayerData> localPlayer;
std::map<std::string, SafePtr<MultiQuestensions::Players::MpPlayerData>> _playerData;

IPlatformUserModel* platformUserModel;

event<GlobalNamespace::IConnectedPlayer*, MultiQuestensions::Players::MpPlayerData*> MultiQuestensions::Players::MpexPlayerManager::RecievedPlayerData;
event<GlobalNamespace::IConnectedPlayer*, MultiQuestensions::Players::MpexPlayerData*> MultiQuestensions::Players::MpexPlayerManager::RecievedMpExPlayerData;

event<GlobalNamespace::DisconnectedReason> MultiQuestensions::Players::MpexPlayerManager::disconnectedEvent;
event<GlobalNamespace::IConnectedPlayer*> MultiQuestensions::Players::MpexPlayerManager::playerConnectedEvent;
event<GlobalNamespace::IConnectedPlayer*> MultiQuestensions::Players::MpexPlayerManager::playerDisconnectedEvent;


event_handler<GlobalNamespace::IConnectedPlayer*> _PlayerConnectedHandler = MultiQuestensions::event_handler<GlobalNamespace::IConnectedPlayer*>(HandlePlayerConnected);
event_handler<GlobalNamespace::IConnectedPlayer*> _PlayerDisconnectedHandler = MultiQuestensions::event_handler<GlobalNamespace::IConnectedPlayer*>(HandlePlayerDisconnected);
event_handler<GlobalNamespace::DisconnectedReason> _DisconnectedHandler = MultiQuestensions::event_handler<GlobalNamespace::DisconnectedReason>(HandleDisconnect);


bool MultiQuestensions::Players::MpexPlayerManager::TryGetMpexPlayerData(std::string playerId, MultiQuestensions::Players::MpexPlayerData*& player) {
    if (_mpexPlayerData.find(playerId) != _mpexPlayerData.end()) {
        player = static_cast<MultiQuestensions::Players::MpexPlayerData*>(_mpexPlayerData.at(playerId));
        return true;
    }
    return false;
}

MultiQuestensions::Players::MpexPlayerData* MultiQuestensions::Players::MpexPlayerManager::GetMpexPlayerData(std::string playerId) {
    if (_mpexPlayerData.find(playerId) != _mpexPlayerData.end()) {
        return static_cast<MultiQuestensions::Players::MpexPlayerData*>(_mpexPlayerData.at(playerId));
    }
    return nullptr;
}


bool MultiQuestensions::Players::MpexPlayerManager::TryGetMpPlayerData(std::string playerId, Players::MpPlayerData*& player) {
    if (_playerData.find(playerId) != _playerData.end()) {
        player = static_cast<Players::MpPlayerData*>(_playerData.at(playerId));
        return true;
    }
    return false;
}

Players::MpPlayerData* MultiQuestensions::Players::MpexPlayerManager::GetMpPlayerData(std::string playerId) {
    if (_playerData.find(playerId) != _playerData.end()) {
        return static_cast<Players::MpPlayerData*>(_playerData.at(playerId));
    }
    return nullptr;
}



static void HandlePlayerData(Players::MpPlayerData* playerData, IConnectedPlayer* player) {
    if(player){
        const std::string userId = to_utf8(csstrtostr(player->get_userId()));
        if (_playerData.contains(userId)) {
            getLogger().debug("HandlePlayerData, player already exists");
            _playerData.at(userId) = playerData;
        }
        else {
            getLogger().info("Received new 'MpPlayerData' from '%s' with platformID: '%s' platform: '%d'",
                to_utf8(csstrtostr(player->get_userId())).c_str(),
                to_utf8(csstrtostr(playerData->platformId)).c_str(),
                (int)playerData->platform
            );
            _playerData.emplace(userId, playerData);
        }
        getLogger().debug("MpPlayerData firing event");
        MultiQuestensions::Players::MpexPlayerManager::RecievedPlayerData(player, playerData);
        getLogger().debug("MpPlayerData done");
    }
}

static void HandleMpexData(Players::MpexPlayerData* packet, IConnectedPlayer* player) {
    if(player){
        const std::string userId = to_utf8(csstrtostr(player->get_userId()));
        if (_mpexPlayerData.contains(userId)) {
            getLogger().info("Recieved 'MpexPlayerData', player already exists");
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
        getLogger().info("Calling event 'RecievedMpExPlayerData'");
        MultiQuestensions::Players::MpexPlayerManager::RecievedMpExPlayerData(player, packet);
    }
}


void HandlePlayerConnected(IConnectedPlayer* player) {
    try {
        getLogger().debug("MpEx HandlePlayerConnected");
        if (player) {
            getLogger().info("Player '%s' joined", static_cast<std::string>(player->get_userId()).c_str());
            if (localPlayer->platformId)
            {
                getLogger().debug("Sending MpPlayerData with platformID: '%s' platform: '%d'",
                    static_cast<std::string>(localPlayer->platformId).c_str(),
                    (int)localPlayer->platform
                );
                MultiplayerCore::mpPacketSerializer->Send(localPlayer);
            }
            getLogger().debug("MpPlayerData sent");

            getLogger().debug("Sending MpexPlayerData");
            if (localMpexPlayerData)
            {
                MultiplayerCore::mpPacketSerializer->Send(localMpexPlayerData->ToSerializable());
                getLogger().debug("MpexPlayerData sent");
            }

            SetPlayerPlaceColor(player, Config::DefaultPlayerColor, false);
        }
    }
    catch (const std::runtime_error& e) {
        getLogger().error("REPORT TO ENDER: %s", e.what());
    }
}

void HandlePlayerDisconnected(IConnectedPlayer* player) {
    try {
        getLogger().debug("MpEx HandlePlayerDisconnected");
        if (player) {
                const std::string userId = to_utf8(csstrtostr(player->get_userId()));
                getLogger().info("Player '%s' left", userId.c_str());
            if(_playerData.contains(userId)){
                _playerData.at(userId).~SafePtr();
                _playerData.erase(userId);
            }
            if (_mpexPlayerData.contains(userId)) {
                getLogger().info("Reseting platform lights for Player '%s'", userId.c_str());
                SetPlayerPlaceColor(player, UnityEngine::Color::get_black(), true);
                _mpexPlayerData.at(userId).~SafePtr();
                _mpexPlayerData.erase(userId);
            }
        }
    }
        catch (const std::runtime_error& e) {
        getLogger().error("REPORT TO ENDER: %s", e.what());
    }
}


void HandleDisconnect(DisconnectedReason reason) {
    getLogger().info("Disconnected from server reason: '%s'", MultiplayerCore::EnumUtils::GetEnumName(reason).c_str());
    _playerData.clear();
    getLogger().info("Clearing MPEX player data");
    _mpexPlayerData.clear();
    getLogger().info("MQE Removing connected/disconnected/disconnect events");
    Players::MpexPlayerManager::playerConnectedEvent -= _PlayerConnectedHandler;
    Players::MpexPlayerManager::playerDisconnectedEvent -= _PlayerDisconnectedHandler;
    Players::MpexPlayerManager::disconnectedEvent -= _DisconnectedHandler;
}

MAKE_HOOK_MATCH(LobbyPlayersDataModel_sHandleMultiplayerSessionManagerPlayerConnected, &LobbyPlayersDataModel::HandleMultiplayerSessionManagerPlayerConnected, void, LobbyPlayersDataModel* self, IConnectedPlayer* player) {
    getLogger().debug("LobbyPlayersDataModel_sHandleMultiplayerSessionManagerPlayerConnected");
    LobbyPlayersDataModel_sHandleMultiplayerSessionManagerPlayerConnected(self, player);
    getLogger().debug("LobbyPlayersDataModel_sHandleMultiplayerSessionManagerPlayerConnected, triggering MQE event");
    Players::MpexPlayerManager::playerConnectedEvent(player);
}

MAKE_HOOK_MATCH(LobbyPlayersDataModel_HandleMultiplayerSessionManagerPlayerDisconnected, &LobbyPlayersDataModel::HandleMultiplayerSessionManagerPlayerDisconnected, void, LobbyPlayersDataModel* self, IConnectedPlayer* player) {
    getLogger().debug("LobbyPlayersDataModel_HandleMultiplayerSessionManagerPlayerDisconnected");
    LobbyPlayersDataModel_HandleMultiplayerSessionManagerPlayerDisconnected(self, player);
    getLogger().debug("LobbyPlayersDataModel_HandleMultiplayerSessionManagerPlayerDisconnected, triggering MQE event");
    Players::MpexPlayerManager::playerDisconnectedEvent(player);
}

MAKE_HOOK_MATCH(LobbyGameStateController_HandleMultiplayerSessionManagerDisconnected, &LobbyGameStateController::HandleMultiplayerSessionManagerDisconnected, void, LobbyGameStateController* self, DisconnectedReason disconnectedReason) {
    getLogger().debug("LobbyGameStateController_HandleMultiplayerSessionManagerDisconnected");
    LobbyGameStateController_HandleMultiplayerSessionManagerDisconnected(self, disconnectedReason);
    Players::MpexPlayerManager::disconnectedEvent(disconnectedReason);
}



MAKE_HOOK_MATCH(SessionManagerStart, &MultiplayerSessionManager::Start, void, MultiplayerSessionManager* self) {

    sessionManager = self;
    SessionManagerStart(sessionManager);
}

Players::Platform getPlatform(UserInfo::Platform platform) {
    switch (platform.value) {
    case UserInfo::Platform::Oculus:
        getLogger().debug("Platform: Oculus = OculusQuest");
        return Players::Platform::OculusQuest; // If platform is Oculus, we assume the user is using Quest
    case UserInfo::Platform::PS4:
        getLogger().debug("Platform: PS4");
        return Players::Platform::PS4;
    case UserInfo::Platform::Steam:
        getLogger().debug("Platform: Steam");
        return Players::Platform::Steam;
    case UserInfo::Platform::Test:
        getLogger().debug("Platform: Test = Unknown");
        return Players::Platform::Unknown;
    default:
        try {
            getLogger().debug("Platform: %s", MultiplayerCore::EnumUtils::GetEnumName(platform).c_str());
        }
        catch (const std::runtime_error& e) {
            getLogger().error("REPORT TO ENDER: %s", e.what());
        }
        catch (const std::exception& e) {
            getLogger().error("REPORT TO ENDER: %s", e.what());
        }
        catch (...) {
            getLogger().error("REPORT TO ENDER: Unknown exception");
        }
        getLogger().debug("Platform: %d", (int)platform.value);
        return (Players::Platform)platform.value;
    }
}


MAKE_HOOK_MATCH(SessionManager_StartSession, &MultiplayerSessionManager::StartSession, void, MultiplayerSessionManager* self, MultiplayerSessionManager_SessionType sessionType, ConnectedPlayerManager* connectedPlayerManager) {
    SessionManager_StartSession(self, sessionType, connectedPlayerManager);
    getLogger().debug("MultiplayerSessionManager.StartSession, creating localPlayer");
    
    static auto localNetworkPlayerModel = UnityEngine::Resources::FindObjectsOfTypeAll<LocalNetworkPlayerModel*>().get(0);
    static auto UserInfoTask = localNetworkPlayerModel->dyn__platformUserModel()->GetUserInfo();
    static auto action = il2cpp_utils::MakeDelegate<System::Action_1<System::Threading::Tasks::Task*>*>(classof(System::Action_1<System::Threading::Tasks::Task*>*), (std::function<void(System::Threading::Tasks::Task_1<GlobalNamespace::UserInfo*>*)>)[&](System::Threading::Tasks::Task_1<GlobalNamespace::UserInfo*>* userInfoTask) {
        auto userInfo = userInfoTask->get_Result();
        if (userInfo) {
            if (!localPlayer) localPlayer = Players::MpPlayerData::Init(userInfo->dyn_platformUserId(), getPlatform(userInfo->dyn_platform()));
            else {
                localPlayer->platformId = userInfo->dyn_platformUserId();
                localPlayer->platform = getPlatform(userInfo->dyn_platform());
                getLogger().error("got local network player, it will be null after this so no repeats");
            }
        }
        else getLogger().error("Failed to get local network player!");
        }
    );
    if(action){
        reinterpret_cast<System::Threading::Tasks::Task*>(UserInfoTask)->ContinueWith(action);
        action = nullptr;
    }



    getLogger().debug("MultiplayerSessionManager.StartSession, creating localMpexPlayerData");

    if(!localMpexPlayerData){
        localMpexPlayerData = Players::MpexPlayerData::New_ctor();
        localMpexPlayerData->Color = config.getPlayerColor();
    }


    //mpPacketSerializer->RegisterCallback<Players::MpPlayerData*>(HandlePlayerData);
    MultiplayerCore::Networking::MpPacketSerializer::RegisterCallbackStatic<MultiQuestensions::Players::MpPlayerData*>(HandlePlayerData);
    getLogger().debug("Callback HandlePlayerData Registered");
    MultiplayerCore::Networking::MpPacketSerializer::RegisterCallbackStatic<MultiQuestensions::Players::MpexPlayerData*>(HandleMpexData);

    Players::MpexPlayerManager::playerConnectedEvent += _PlayerConnectedHandler;
    Players::MpexPlayerManager::playerDisconnectedEvent += _PlayerDisconnectedHandler;
    Players::MpexPlayerManager::disconnectedEvent += _DisconnectedHandler;

}



void MultiQuestensions::Hooks::SessionManagerAndExtendedPlayerHooks() {
    INSTALL_HOOK(getLogger(), SessionManagerStart);
    INSTALL_HOOK(getLogger(), SessionManager_StartSession);

    INSTALL_HOOK(getLogger(), LobbyPlayersDataModel_sHandleMultiplayerSessionManagerPlayerConnected);
    INSTALL_HOOK(getLogger(), LobbyPlayersDataModel_HandleMultiplayerSessionManagerPlayerDisconnected);
    INSTALL_HOOK(getLogger(), LobbyGameStateController_HandleMultiplayerSessionManagerDisconnected);
}