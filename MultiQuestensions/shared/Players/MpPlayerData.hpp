#pragma once
#include "custom-types/shared/macros.hpp"
#include "GlobalNamespace/IConnectedPlayer.hpp"
#include "MultiplayerCore/shared/Networking/Abstractions/MpPacket.hpp"

namespace MultiQuestensions::Players {
enum class Platform
    {
        Unknown = 0,
        Steam = 1,
        OculusPC = 2,
        OculusQuest = 3,
        PS4 = 4
    };
}

DECLARE_CLASS_CUSTOM_DLL(MultiQuestensions::Players, MpPlayerData, MultiplayerCore::Networking::Abstractions::MpPacket, "MultiQuestensions.Players",

    DECLARE_CTOR(New);

    // Player and platform information
    DECLARE_INSTANCE_FIELD(StringW, platformId);

    DECLARE_OVERRIDE_METHOD(void, Serialize, il2cpp_utils::FindMethodUnsafe(classof(MultiplayerCore::Networking::Abstractions::MpPacket*), "Serialize", 1), LiteNetLib::Utils::NetDataWriter* writer);
    DECLARE_OVERRIDE_METHOD(void, Deserialize, il2cpp_utils::FindMethodUnsafe(classof(MultiplayerCore::Networking::Abstractions::MpPacket*), "Deserialize", 1), LiteNetLib::Utils::NetDataReader* reader);

    public:
        Platform platform;

        static MpPlayerData* Init(StringW platformID, Platform platform);
)
//#undef MPDInterfaces