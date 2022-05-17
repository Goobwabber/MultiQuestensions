#pragma once
#include "MpexPlayerData.hpp"
#include "MultiplayerCore/shared/Utils/event.hpp"

namespace MultiQuestensions::Players {
    class MpexPlayerManager {
    public:
        //static MultiplayerCore::event_handler<GlobalNamespace::IConnectedPlayer*> _PlayerConnectedHandler;
        static MultiplayerCore::event<GlobalNamespace::IConnectedPlayer*, MpexPlayerData*> RecievedMpExPlayerData;
        static bool TryGetPlayer(std::string playerId, MultiQuestensions::Players::MpexPlayerData*& player);
        static MpexPlayerData* GetPlayer(std::string playerId);
    };
}