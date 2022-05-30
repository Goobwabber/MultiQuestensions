#include "main.hpp"
#include "Hooks/Hooks.hpp"
#include "Hooks/SessionManagerAndExtendedPlayerHooks.hpp"
#include "GlobalFields.hpp"
#include "Hooks/EnvironmentAndAvatarHooks.hpp"
#include "Players/MpexPlayerManager.hpp"
#include "Config.hpp"

#include "GlobalNamespace/LocalNetworkPlayerModel.hpp"
#include "UnityEngine/Resources.hpp"
#include "GlobalNamespace/UserInfo.hpp"
#include "GlobalNamespace/MultiplayerSessionManager_SessionType.hpp"
#include "GlobalNamespace/DisconnectedReason.hpp"


#include "CodegenExtensions/ColorUtility.hpp"

#include "questui/shared/BeatSaberUI.hpp"
#include "questui/shared/CustomTypes/Components/MainThreadScheduler.hpp"

#include "MultiplayerCore/shared/Networking/MpPacketSerializer.hpp"
#include "MultiplayerCore/shared/Networking/MpNetworkingEvents.hpp"

#include "MultiplayerCore/shared/CodegenExtensions/EnumUtils.hpp"
#include "Players/MpexPlayerManager.hpp"
#include "MultiplayerCore/shared/Players/MpPlayerData.hpp"
#include "MultiplayerCore/shared/Players/MpPlayerManager.hpp"
#include "MultiplayerCore/shared/GlobalFields.hpp"

using namespace MultiQuestensions;
using namespace MultiplayerCore::Networking;
using namespace GlobalNamespace;

Players::MpexPlayerData* localMpexPlayerData;
// TODO: Check the Players::MpexPlayerData* instances don't get GC'd before getting placed in their CT
std::unordered_map<std::string, Players::MpexPlayerData*> _mpexPlayerData;

MultiplayerCore::event<GlobalNamespace::IConnectedPlayer*, MultiQuestensions::Players::MpexPlayerData*> MultiQuestensions::Players::MpexPlayerManager::ReceivedMpExPlayerData;

MultiplayerCore::event_handler<MultiplayerCore::Networking::MpPacketSerializer*> _RegisterMpexPacketsHandler = MultiplayerCore::event_handler<MultiplayerCore::Networking::MpPacketSerializer*>(HandleRegisterMpexPackets);
MultiplayerCore::event_handler<MultiplayerCore::Networking::MpPacketSerializer*> _UnRegisterMpexPacketsHandler = MultiplayerCore::event_handler<MultiplayerCore::Networking::MpPacketSerializer*>(HandleUnRegisterMpexPackets);


MultiplayerCore::event_handler<GlobalNamespace::IConnectedPlayer*> _PlayerConnectedHandler = MultiplayerCore::event_handler<GlobalNamespace::IConnectedPlayer*>(HandlePlayerConnected);
MultiplayerCore::event_handler<GlobalNamespace::IConnectedPlayer*> _PlayerDisconnectedHandler = MultiplayerCore::event_handler<GlobalNamespace::IConnectedPlayer*>(HandlePlayerDisconnected);
MultiplayerCore::event_handler<GlobalNamespace::IConnectedPlayer*> _ConnectingHandler = MultiplayerCore::event_handler<GlobalNamespace::IConnectedPlayer*>(HandleConnecting);
MultiplayerCore::event_handler<GlobalNamespace::DisconnectedReason, GlobalNamespace::IConnectedPlayer*> _DisconnectedHandler = MultiplayerCore::event_handler<GlobalNamespace::DisconnectedReason, GlobalNamespace::IConnectedPlayer*>(HandleDisconnect);


bool MultiQuestensions::Players::MpexPlayerManager::TryGetMpexPlayerData(std::string const& playerId, MultiQuestensions::Players::MpexPlayerData*& player) {
    if (_mpexPlayerData.find(playerId) != _mpexPlayerData.end()) {
        player = _mpexPlayerData.at(playerId);
        return true;
    }
    return false;
}

MultiQuestensions::Players::MpexPlayerData* MultiQuestensions::Players::MpexPlayerManager::GetMpexPlayerData(std::string const& playerId) {
    if (_mpexPlayerData.find(playerId) != _mpexPlayerData.end()) {
        return _mpexPlayerData.at(playerId);
    }
    return nullptr;
}

static void HandleMpexData(Players::MpexPlayerData* packet, IConnectedPlayer* player) {
    if(player){
        std::string const& userId = player->get_userId();
        if (_mpexPlayerData.contains(userId)) {
            getLogger().info("Received 'MpexPlayerData', player already exists");
            // Explicitly free previous MpexPlayerData
            gc_free_specific( _mpexPlayerData[userId]);
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
        getLogger().info("Calling event 'ReceivedMpExPlayerData'");
        MultiQuestensions::Players::MpexPlayerManager::ReceivedMpExPlayerData(player, packet);
    }
}

void HandleRegisterMpexPackets(MultiplayerCore::Networking::MpPacketSerializer* _mpPacketSerializer) {
        // Explicitly allocate instance when packet is received, we will manually clear it
        _mpPacketSerializer->RegisterCallback<MultiQuestensions::Players::MpexPlayerData*, ::il2cpp_utils::CreationType::Manual>(HandleMpexData);
        // _mpPacketSerializer->RegisterCallback<MultiQuestensions::Players::MpexPlayerData*>(HandleMpexData);
        getLogger().debug("Callback HandleMpexData Registered");
}
void HandleUnRegisterMpexPackets(MultiplayerCore::Networking::MpPacketSerializer* _mpPacketSerializer) {
        _mpPacketSerializer->UnregisterCallback<MultiQuestensions::Players::MpexPlayerData*>();
        getLogger().debug("Callback HandleMpexData UnRegistered");
}

void HandlePlayerConnected(IConnectedPlayer* player) {
    try {
        getLogger().debug("MpEx HandlePlayerConnected");
        if (player) {
            getLogger().info("MpExPlayer '%s' joined", static_cast<std::string>(player->get_userId()).c_str());
            getLogger().debug("Sending MpexPlayerData");
            if (localMpexPlayerData)
            {
                MultiplayerCore::mpPacketSerializer->Send(localMpexPlayerData);
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
                std::string const& userId = player->get_userId();
                getLogger().info("Player '%s' left", userId.c_str());
            if (_mpexPlayerData.contains(userId)) {
                getLogger().info("Reseting platform lights for Player '%s'", userId.c_str());
                SetPlayerPlaceColor(player, UnityEngine::Color::get_black(), true);
                // Explicitly free previous MpexPlayerData
                gc_free_specific( _mpexPlayerData[userId]);
                _mpexPlayerData.erase(userId);
            }
        }
    }
    catch (const std::runtime_error& e) {
        getLogger().error("REPORT TO ENDER: %s", e.what());
    }
}


void HandleDisconnect(DisconnectedReason reason, GlobalNamespace::IConnectedPlayer* localPlayer) {
    getLogger().info("Disconnected from server reason: '%s'", MultiplayerCore::EnumUtils::GetEnumName(reason).c_str());
    getLogger().info("Clearing MPEX player data");
    // TODO: Clear each instance manually
    for (const auto & [ key, value ] : _mpexPlayerData) {
       gc_free_specific(value);
    }
    _mpexPlayerData.clear();
    getLogger().info("MQE Removing connected/disconnected/disconnect events");
    MultiplayerCore::Players::MpPlayerManager::playerConnectedEvent -= _PlayerConnectedHandler;
    MultiplayerCore::Players::MpPlayerManager::playerDisconnectedEvent -= _PlayerDisconnectedHandler;
    MultiplayerCore::Players::MpPlayerManager::disconnectedEvent -= _DisconnectedHandler;

    MultiplayerCore::Networking::MpNetworkingEvents::RegisterPackets -= _RegisterMpexPacketsHandler;
    MultiplayerCore::Networking::MpNetworkingEvents::UnRegisterPackets -= _UnRegisterMpexPacketsHandler;

    // Clear our manually created localMpexPlayerData instance
    if(localMpexPlayerData){
        gc_free_specific(localMpexPlayerData);
        localMpexPlayerData = nullptr;
    }
}

void HandleConnecting(GlobalNamespace::IConnectedPlayer* localPlayer){
    getLogger().debug("HandleConnecting, creating localMpexPlayerData and registering events");

    if(!localMpexPlayerData){
        localMpexPlayerData = Players::MpexPlayerData::New_ctor<::il2cpp_utils::CreationType::Manual>();
        localMpexPlayerData->Color = config.getPlayerColor();
    }
    MultiplayerCore::Networking::MpNetworkingEvents::RegisterPackets += _RegisterMpexPacketsHandler;
    MultiplayerCore::Networking::MpNetworkingEvents::UnRegisterPackets += _UnRegisterMpexPacketsHandler;

    MultiplayerCore::Players::MpPlayerManager::playerConnectedEvent += _PlayerConnectedHandler;
    MultiplayerCore::Players::MpPlayerManager::playerDisconnectedEvent += _PlayerDisconnectedHandler;
    MultiplayerCore::Players::MpPlayerManager::disconnectedEvent += _DisconnectedHandler;

}

void MultiQuestensions::Hooks::SessionManagerAndExtendedPlayerHooks() {

    MultiplayerCore::Players::MpPlayerManager::connectingEvent += _ConnectingHandler;
}