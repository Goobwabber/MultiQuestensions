#pragma once

// Include the modloader header, which allows us to tell the modloader which mod this is, and the version etc.
#include "extern/modloader/shared/modloader.hpp"

// beatsaber-hook is a modding framework that lets us call functions and fetch field values from in the game
// It also allows creating objects, configuration, and importantly, hooking methods to modify their values
#include "extern/beatsaber-hook/shared/utils/logging.hpp"
#include "extern/beatsaber-hook/shared/config/config-utils.hpp"
#include "extern/beatsaber-hook/shared/utils/il2cpp-functions.hpp"
#include "extern/beatsaber-hook/shared/utils/il2cpp-utils.hpp"
#include "extern/codegen/include/LiteNetLib/Utils/INetSerializable.hpp"

// Codegen includes
#include "extern/codegen/include/GlobalNamespace/MultiplayerSessionManager.hpp"
#include "extern/codegen/include/GlobalNamespace/IMenuRpcManager.hpp"
#include "extern/codegen/include/GlobalNamespace/LobbyPlayersDataModel.hpp"
#include "extern/codegen/include/GlobalNamespace/BeatmapLevelsModel.hpp"
#include "extern/codegen/include/GlobalNamespace/IConnectedPlayer.hpp"

#include "extern/codegen/include/GlobalNamespace/BeatmapIdentifierNetSerializable.hpp"
#include "extern/codegen/include/GlobalNamespace/IPreviewBeatmapLevel.hpp"
#include "extern/codegen/include/GlobalNamespace/BeatmapDifficulty.hpp"
#include "extern/codegen/include/GlobalNamespace/BeatmapCharacteristicSO.hpp"
#include "extern/codegen/include/GlobalNamespace/BeatmapCharacteristicCollectionSO.hpp"

// Cpp includes
#include <map>
#include <set>
#include <string>

// Define these functions here so that we can easily read configuration and log information from other files
Configuration& getConfig();
const Logger& getLogger();