#pragma once
#include "Extensions/ExtendedPlayer.hpp"

extern SafePtr<MultiQuestensions::Extensions::ExtendedPlayer> localExtendedPlayer;
extern std::map<std::string, SafePtr<MultiQuestensions::Extensions::ExtendedPlayer>> _extendedPlayers;

static void HandlePreviewBeatmapPacket(MultiQuestensions::Beatmaps::PreviewBeatmapPacket* packet, GlobalNamespace::IConnectedPlayer* player);