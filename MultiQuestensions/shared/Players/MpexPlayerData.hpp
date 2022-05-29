#pragma once
#include "custom-types/shared/macros.hpp"
#include "GlobalNamespace/IConnectedPlayer.hpp"
#include "MultiplayerCore/shared/Networking/Abstractions/MpPacket.hpp"
#include "UnityEngine/Color.hpp"

DECLARE_CLASS_CUSTOM_DLL(MultiQuestensions::Players, MpexPlayerData, MultiplayerCore::Networking::Abstractions::MpPacket, "MultiplayerExtensions.Players",

    DECLARE_DEFAULT_CTOR();

    /// <summary>
    /// Player's color set in the mod config.
    /// </summary>
    DECLARE_INSTANCE_FIELD(UnityEngine::Color, Color);

    DECLARE_OVERRIDE_METHOD(void, Serialize, il2cpp_utils::FindMethodUnsafe(classof(MultiplayerCore::Networking::Abstractions::MpPacket*), "Serialize", 1), LiteNetLib::Utils::NetDataWriter* writer);
    DECLARE_OVERRIDE_METHOD(void, Deserialize, il2cpp_utils::FindMethodUnsafe(classof(MultiplayerCore::Networking::Abstractions::MpPacket*), "Deserialize", 1), LiteNetLib::Utils::NetDataReader* reader);
)