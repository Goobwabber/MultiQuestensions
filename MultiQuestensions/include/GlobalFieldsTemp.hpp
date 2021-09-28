#pragma once
#include "main.hpp"

// MQE Includes
#include "Extensions/ExtendedPlayer.hpp"
#include "Packets/PacketManager.hpp"
namespace MultiQuestensions {
    // Plugin setup stuff
    extern GlobalNamespace::MultiplayerSessionManager* sessionManager;
    extern GlobalNamespace::LobbyPlayersDataModel* lobbyPlayersDataModel;
    extern GlobalNamespace::LobbyGameStateController* lobbyGameStateController;

    extern void SetPlayerPlaceColor(GlobalNamespace::IConnectedPlayer* player, const UnityEngine::Color& color, bool priority);
    extern void CreateOrUpdateNameTag(GlobalNamespace::IConnectedPlayer* player);
}

extern MultiQuestensions::Extensions::ExtendedPlayer* localExtendedPlayer;
extern std::map<std::string, MultiQuestensions::Extensions::ExtendedPlayer*> _extendedPlayers;
