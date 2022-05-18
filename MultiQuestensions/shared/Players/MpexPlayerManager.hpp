#pragma once
#include "MpexPlayerData.hpp"
#include "MpPlayerData.hpp"
#include "Utils/event.hpp"

namespace MultiQuestensions::Players {
    class MpexPlayerManager {
    public:


        static bool TryGetMpexPlayerData(std::string playerId, MultiQuestensions::Players::MpexPlayerData*& player);
        static MpexPlayerData* GetMpexPlayerData(std::string playerId);

        static bool TryGetMpPlayerData(std::string playerId, MultiQuestensions::Players::MpPlayerData*& player);
        static MultiQuestensions::Players::MpPlayerData* GetMpPlayerData(std::string playerId);

        static event<GlobalNamespace::IConnectedPlayer*, MpPlayerData*> RecievedPlayerData;
        static event<GlobalNamespace::IConnectedPlayer*, MpexPlayerData*> RecievedMpExPlayerData;

        //C++ equivalent to basegame events
        static event<GlobalNamespace::IConnectedPlayer*> playerConnectedEvent;
        static event<GlobalNamespace::IConnectedPlayer*> playerDisconnectedEvent;
        static event<GlobalNamespace::DisconnectedReason> disconnectedEvent;



    };
}