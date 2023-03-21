#pragma once
#include "MpexPlayerData.hpp"
#include "multiplayer-core/shared/Utils/event.hpp"
#include "multiplayer-core/shared/Players/MpPlayerData.hpp"


namespace MultiQuestensions::Players {
    class MpexPlayerManager {
    public:


        static bool TryGetMpexPlayerData(std::string const& playerId, MultiQuestensions::Players::MpexPlayerData*& player);
        static MpexPlayerData* GetMpexPlayerData(std::string const& playerId);

        static MultiplayerCore::event<GlobalNamespace::IConnectedPlayer*, MpexPlayerData*> ReceivedMpExPlayerData;
    };
}