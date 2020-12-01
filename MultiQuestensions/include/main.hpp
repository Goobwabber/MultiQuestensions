#pragma once

// Include the modloader header, which allows us to tell the modloader which mod this is, and the version etc.
#include "modloader/shared/modloader.hpp"

// beatsaber-hook is a modding framework that lets us call functions and fetch field values from in the game
// It also allows creating objects, configuration, and importantly, hooking methods to modify their values
#include "beatsaber-hook/shared/utils/logging.hpp"
#include "beatsaber-hook/shared/config/config-utils.hpp"
#include "beatsaber-hook/shared/utils/il2cpp-functions.hpp"
#include "beatsaber-hook/shared/utils/il2cpp-utils.hpp"

// Codegen includes
#include "GlobalNamespace/MultiplayerSessionManager.hpp"
#include "GlobalNamespace/MultiplayerSessionManager_MessageType.hpp"
#include "GlobalNamespace/IMenuRpcManager.hpp"
#include "GlobalNamespace/LobbyPlayersDataModel.hpp"
#include "GlobalNamespace/LobbyPlayerDataModel.hpp"
#include "GlobalNamespace/BeatmapLevelsModel.hpp"
#include "GlobalNamespace/IConnectedPlayer.hpp"

#include "GlobalNamespace/BeatmapIdentifierNetSerializable.hpp"
#include "GlobalNamespace/IPreviewBeatmapLevel.hpp"
#include "GlobalNamespace/BeatmapDifficulty.hpp"
#include "GlobalNamespace/BeatmapCharacteristicSO.hpp"
#include "GlobalNamespace/BeatmapCharacteristicCollectionSO.hpp"

#include "GlobalNamespace/INetworkPacketSubSerializer_1.hpp"
#include "GlobalNamespace/ThreadStaticPacketPool_1.hpp"
#include "GlobalNamespace/PacketPool_1.hpp"
#include "GlobalNamespace/IPoolablePacket.hpp"

#include "LiteNetLib/Utils/INetSerializable.hpp"
#include "LiteNetLib/Utils/NetDataReader.hpp"
#include "LiteNetLib/Utils/NetDataWriter.hpp"

#include "System/Threading/Tasks/Task_1.hpp"
#include "System/Threading/CancellationToken.hpp"
#include "System/Type.hpp"
#include "System/Func_2.hpp"

#include "UnityEngine/Sprite.hpp"
#include "UnityEngine/Rect.hpp"
#include "UnityEngine/Vector2.hpp"
#include "UnityEngine/Vector4.hpp"
#include "UnityEngine/Texture2D.hpp"
#include "UnityEngine/AudioClip.hpp"
#include "UnityEngine/SpriteMeshType.hpp"

// Cpp includes
#include "map"
#include "set"

// Define these functions here so that we can easily read configuration and log information from other files
Configuration& getConfig();
const Logger& getLogger();