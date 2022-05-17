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
#include "MultiplayerCore/shared/Players/MpPlayerManager.hpp"
#include "MultiplayerCore/shared/Players/MpPlayerData.hpp"

#include "Environments/MQEAvatarNameTag.hpp"

using namespace MultiQuestensions;
using namespace MultiplayerCore::Networking;
using namespace GlobalNamespace;
using namespace MultiQuestensions::Environments;

// std::map<std::string, SafePtr<Extensions::ExtendedPlayer>> _extendedPlayers;
std::map<std::string, SafePtr<Players::MpexPlayerData>> _mpexPlayerData;
//System::Collections::Generic::Dictionary_2<Il2CppString*, Extensions::ExtendedPlayer*>* extendedPlayers;
//SafePtr<System::Collections::Generic::Dictionary_2<std::string, Extensions::ExtendedPlayer*>*> extendedPlayersSPTR;
//SafePtr<Extensions::ExtendedPlayer*> localExtendedPlayerSPTR;
// SafePtr<Extensions::ExtendedPlayer> localExtendedPlayer;
SafePtr<Players::MpexPlayerData> localMpexPlayerData;
IPlatformUserModel* platformUserModel;


MultiplayerCore::event_handler<GlobalNamespace::IConnectedPlayer*> _PlayerConnectedHandler = MultiplayerCore::event_handler<GlobalNamespace::IConnectedPlayer*>(HandlePlayerDisconnected(player));
MultiplayerCore::event_handler<GlobalNamespace::IConnectedPlayer*> _PlayerDisconnectedHandler = MultiplayerCore::event_handler<GlobalNamespace::IConnectedPlayer*>(HandlePlayerDisconnected(player));
MultiplayerCore::event_handler<GlobalNamespace::IConnectedPlayer*> _DisconnectedHandler = MultiplayerCore::event_handler<GlobalNamespace::DisconnectedReason*>(HandleDisconnect(reason));

MultiplayerCore::event<GlobalNamespace::IConnectedPlayer*, MultiQuestensions::Players::MpexPlayerData*> MultiQuestensions::Players::MpexPlayerManager::RecievedMpExPlayerData;

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
    getLogger().debug("HandleMpexData");
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
    getLogger().info("Calling event 'RecievedMpExPlayerData'");
    MultiQuestensions::Players::MpexPlayerManager::RecievedMpExPlayerData(player, packet);
}

void HandlePlayerConnected(IConnectedPlayer* player) {
    try {
        getLogger().debug("MQE HandlePlayerConnected");
        if (player) {
            const std::string userId = player->get_userId();
            getLogger().info("MQE Player '%s' joined", userId.c_str());
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
    if (player) {
        const std::string userId = to_utf8(csstrtostr(player->get_userId()));
        if (_mpexPlayerData.contains(userId)) {
            getLogger().info("Reseting platform lights for Player '%s'", userId.c_str());
            SetPlayerPlaceColor(player, UnityEngine::Color::get_black(), true);
            _mpexPlayerData.erase(userId);
        }
    }
}

void HandleDisconnect(DisconnectedReason reason) {
    getLogger().info("Clearing MPEX player data");
    _mpexPlayerData.clear();
    MultiplayerCore::Players::MpPlayerManager::playerConnectedEvent -= _PlayerConnectedHandler;
    MultiplayerCore::Players::MpPlayerManager::playerDisconnectedEvent -= _PlayerDisconnectedHandler;
    MultiplayerCore::Players::MpPlayerManager::disconnectedEvent -= _DisconnectedHandler;
}

MAKE_HOOK_MATCH(SessionManagerStart, &MultiplayerSessionManager::Start, void, MultiplayerSessionManager* self) {

    sessionManager = self;
    SessionManagerStart(sessionManager);
}

MAKE_HOOK_MATCH(SessionManager_StartSession, &MultiplayerSessionManager::StartSession, void, MultiplayerSessionManager* self, MultiplayerSessionManager_SessionType sessionType, ConnectedPlayerManager* connectedPlayerManager) {
    SessionManager_StartSession(self, sessionType, connectedPlayerManager);

    localMpexPlayerData = Players::MpexPlayerData::New_ctor();
    localMpexPlayerData->Color = config.getPlayerColor();

    MultiplayerCore::Networking::MpPacketSerializer::RegisterCallbackStatic<MultiQuestensions::Players::MpexPlayerData*>(HandleMpexData);



    MultiplayerCore::Players::MpPlayerManager::playerConnectedEvent += _PlayerConnectedHandler;
    MultiplayerCore::Players::MpPlayerManager::playerDisconnectedEvent += _PlayerDisconnectedHandler;
    MultiplayerCore::Players::MpPlayerManager::disconnectedEvent += _DisconnectedHandler;
}


void MultiQuestensions::Hooks::SessionManagerAndExtendedPlayerHooks() {
    INSTALL_HOOK(getLogger(), SessionManagerStart);
    INSTALL_HOOK(getLogger(), SessionManager_StartSession);
}