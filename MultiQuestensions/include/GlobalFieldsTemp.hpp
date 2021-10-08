#pragma once
#include "main.hpp"

// MQE Includes
#include "Extensions/ExtendedPlayer.hpp"
#include "Packets/PacketManager.hpp"

#include "System/Collections/Generic/Dictionary_2.hpp"
namespace MultiQuestensions {
    // Plugin setup stuff
    extern GlobalNamespace::MultiplayerSessionManager* sessionManager;
    extern GlobalNamespace::LobbyPlayersDataModel* lobbyPlayersDataModel;
    extern GlobalNamespace::LobbyGameStateController* lobbyGameStateController;

    extern void SetPlayerPlaceColor(GlobalNamespace::IConnectedPlayer* player, const UnityEngine::Color& color, bool priority);
    extern void CreateOrUpdateNameTag(GlobalNamespace::IConnectedPlayer* player);
}

extern SafePtr<MultiQuestensions::Extensions::ExtendedPlayer> localExtendedPlayer;
extern std::map<std::string, SafePtr<MultiQuestensions::Extensions::ExtendedPlayer>> _extendedPlayers;
//extern System::Collections::Generic::Dictionary_2<std::string, MultiQuestensions::Extensions::ExtendedPlayer*> extendedPlayers;
