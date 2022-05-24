#pragma once
#include "MpexPlayerData.hpp"
#include "MultiplayerCore/shared/Utils/event.hpp"
#include "MultiplayerCore/shared/Players/MpPlayerData.hpp"


namespace MultiQuestensions::Players {
    class MpexPlayerManager {
    public:


        static bool TryGetMpexPlayerData(std::string playerId, MultiQuestensions::Players::MpexPlayerData*& player);
        static MpexPlayerData* GetMpexPlayerData(std::string playerId);

        static MultiplayerCore::event<GlobalNamespace::IConnectedPlayer*, MpexPlayerData*> ReceivedMpExPlayerData;
    };
}