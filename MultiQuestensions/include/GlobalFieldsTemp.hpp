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
}

extern MultiQuestensions::Extensions::ExtendedPlayer* localExtendedPlayer;