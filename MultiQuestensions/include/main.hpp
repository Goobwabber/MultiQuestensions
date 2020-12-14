#pragma once

// Include the modloader header, which allows us to tell the modloader which mod this is, and the version etc.
#include "modloader/shared/modloader.hpp"

// beatsaber-hook is a modding framework that lets us call functions and fetch field values from in the game
// It also allows creating objects, configuration, and importantly, hooking methods to modify their values
#include "beatsaber-hook/shared/utils/logging.hpp"
#include "beatsaber-hook/shared/config/config-utils.hpp"
#include "beatsaber-hook/shared/utils/il2cpp-functions.hpp"
#include "beatsaber-hook/shared/utils/il2cpp-utils.hpp"
#include "LiteNetLib/Utils/INetSerializable.hpp"

// Codegen includes
#include "GlobalNamespace/MultiplayerSessionManager.hpp"
#include "GlobalNamespace/IMenuRpcManager.hpp"
#include "GlobalNamespace/LobbyPlayersDataModel.hpp"
#include "GlobalNamespace/BeatmapLevelsModel.hpp"
#include "GlobalNamespace/IConnectedPlayer.hpp"

#include "GlobalNamespace/BeatmapIdentifierNetSerializable.hpp"
#include "GlobalNamespace/IPreviewBeatmapLevel.hpp"
#include "GlobalNamespace/BeatmapDifficulty.hpp"
#include "GlobalNamespace/BeatmapCharacteristicSO.hpp"
#include "GlobalNamespace/BeatmapCharacteristicCollectionSO.hpp"

// Cpp includes
#include "map"
#include "set"
#include "string"

// Define these functions here so that we can easily read configuration and log information from other files
Configuration& getConfig();
const Logger& getLogger();