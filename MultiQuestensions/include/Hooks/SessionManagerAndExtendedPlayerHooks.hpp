#pragma once
#include "Players/MpexPlayerData.hpp"
#include "GlobalNamespace/IConnectedPlayer.hpp"
#include "GlobalNamespace/DisconnectedReason.hpp"
#include "MultiplayerCore/shared/Networking/MpPacketSerializer.hpp"
#include "MultiplayerCore/shared/Networking/MpNetworkingEvents.hpp"

extern MultiQuestensions::Players::MpexPlayerData* localMpexPlayerData;
extern std::unordered_map<std::string, MultiQuestensions::Players::MpexPlayerData*> _mpexPlayerData;

extern void HandlePlayerConnected(GlobalNamespace::IConnectedPlayer* player);
extern void HandlePlayerDisconnected(GlobalNamespace::IConnectedPlayer* player);
extern void HandleDisconnect(GlobalNamespace::DisconnectedReason reason, GlobalNamespace::IConnectedPlayer* locPlayer);

extern void HandleRegisterMpexPackets(MultiplayerCore::Networking::MpPacketSerializer* _mpPacketSerializer);