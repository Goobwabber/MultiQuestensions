#pragma once

// Include the modloader header, which allows us to tell the modloader which mod this is, and the version etc.
#include "modloader/shared/modloader.hpp"

// beatsaber-hook is a modding framework that lets us call functions and fetch field values from in the game
// It also allows creating objects, configuration, and importantly, hooking methods to modify their values
#include "beatsaber-hook/shared/utils/logging.hpp"
#include "beatsaber-hook/shared/config/config-utils.hpp"
#include "beatsaber-hook/shared/utils/il2cpp-functions.hpp"

// Codegen includes
#include "GlobalNamespace/MultiplayerSessionManager.hpp"
#include "GlobalNamespace/MultiplayerSessionManager_MessageType.hpp"

#include "LiteNetLib/Utils/INetSerializable.hpp"
#include "GlobalNamespace/IPoolablePacket.hpp"
#include "LiteNetLib/Utils/NetDataReader.hpp"
#include "LiteNetLib/Utils/NetDataWriter.hpp"

#include "GlobalNamespace/INetworkPacketSubSerializer_1.hpp"
#include "System/Type.hpp"
#include "GlobalNamespace/IConnectedPlayer.hpp"
#include "GlobalNamespace/ThreadStaticPacketPool_1.hpp"
#include "GlobalNamespace/PacketPool_1.hpp"

// Cpp includes
#include "map"
#include "set"

// Define these functions here so that we can easily read configuration and log information from other files
Configuration& getConfig();
const Logger& getLogger();