#pragma once
//#include "main.hpp"
//#include "GlobalNamespace/MultiplayerSessionManager.hpp"
#include "GlobalNamespace/LobbyPlayersDataModel.hpp"
#include "GlobalNamespace/LobbyGameStateController.hpp"

// MQE Includes
// #include "Packets/PacketManager.hpp"

//#include "System/Collections/Generic/Dictionary_2.hpp"
namespace MultiQuestensions {
    // Plugin setup stuff
    //extern GlobalNamespace::MultiplayerSessionManager* sessionManager;
    extern GlobalNamespace::LobbyPlayersDataModel* lobbyPlayersDataModel;
    extern GlobalNamespace::LobbyGameStateController* lobbyGameStateController;
}

//extern System::Collections::Generic::Dictionary_2<std::string, MultiQuestensions::Extensions::ExtendedPlayer*> extendedPlayers;