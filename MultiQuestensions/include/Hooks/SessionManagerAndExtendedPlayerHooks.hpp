#pragma once
#include "Players/MpexPlayerData.hpp"
#include "GlobalNamespace/IConnectedPlayer.hpp"
#include "GlobalNamespace/DisconnectedReason.hpp"

extern SafePtr<MultiQuestensions::Players::MpexPlayerData> localMpexPlayerData;
extern std::unordered_map<std::string, SafePtr<MultiQuestensions::Players::MpexPlayerData>> _mpexPlayerData;

extern void HandlePlayerConnected(GlobalNamespace::IConnectedPlayer* player);
extern void HandlePlayerDisconnected(GlobalNamespace::IConnectedPlayer* player);
extern void HandleDisconnect(GlobalNamespace::DisconnectedReason reason);
