#pragma once
#include "custom-types/shared/macros.hpp"
#include "GlobalNamespace/IConnectedPlayer.hpp"
#include "UnityEngine/Color.hpp"
#include "GlobalNamespace/MultiplayerAvatarData.hpp"
#include "GlobalNamespace/DisconnectedReason.hpp"

#include "LiteNetLib/Utils/NetDataReader.hpp"
#include "LiteNetLib/Utils/NetDataWriter.hpp"
#include "LiteNetLib/Utils/INetSerializable.hpp"
#include "GlobalNamespace/ThreadStaticPacketPool_1.hpp"
#include "GlobalNamespace/PacketPool_1.hpp"
#include "GlobalNamespace/IPoolablePacket.hpp"

#include <optional>

namespace MultiQuestensions::Extensions {
    enum class Platform
    {
        Unknown = 0,
        Steam = 1,
        OculusPC = 2,
        OculusQuest = 3,
        PS4 = 4
    };
}

#define EPInterfaces { classof(GlobalNamespace::IConnectedPlayer*) }
___DECLARE_TYPE_WRAPPER_INHERITANCE(MultiQuestensions::Extensions, ExtendedPlayer, Il2CppTypeEnum::IL2CPP_TYPE_CLASS,
    Il2CppObject, "MultiplayerExtensions.Extensions",
    EPInterfaces, 0, nullptr,

    DECLARE_INSTANCE_FIELD(GlobalNamespace::IConnectedPlayer*, _connectedPlayer);

    /// <summary>
    /// Platform User ID
    /// </summary>
    DECLARE_INSTANCE_FIELD(Il2CppString*, platformID);
    DECLARE_INSTANCE_METHOD(Il2CppString*, get_platformID);

    /// <summary>
    /// MultiplayerExtensions version reported by BSIPA.
    /// </summary>
    DECLARE_INSTANCE_FIELD(Il2CppString*, mpexVersion);
    DECLARE_INSTANCE_METHOD(Il2CppString*, get_mpexVersion);

    /// <summary>
    /// Player's color set in the plugin config.
    /// </summary>
    DECLARE_INSTANCE_FIELD(UnityEngine::Color, playerColor);

    DECLARE_INSTANCE_FIELD(Il2CppString*, isPartyOwner);
    DECLARE_INSTANCE_FIELD(Il2CppString*, hasRecommendBeatmapPermission);
    DECLARE_INSTANCE_FIELD(Il2CppString*, hasRecommendModifiersPermission);
    DECLARE_INSTANCE_FIELD(Il2CppString*, hasKickVotePermission);

    DECLARE_CTOR(ExtendedPlayer_ctor, GlobalNamespace::IConnectedPlayer* player, Il2CppString* platformID, int platform, UnityEngine::Color playerColor, Il2CppString* mpexVersion);

    DECLARE_INSTANCE_METHOD(bool, get_isMe);
    DECLARE_INSTANCE_METHOD(Il2CppString*, get_userId);
    DECLARE_INSTANCE_METHOD(Il2CppString*, get_userName);
    DECLARE_INSTANCE_METHOD(float, get_currentLatency);
    DECLARE_INSTANCE_METHOD(bool, get_isConnected);
    DECLARE_INSTANCE_METHOD(bool, get_isConnectionOwner);
    DECLARE_INSTANCE_METHOD(float, get_offsetSyncTime);
    DECLARE_INSTANCE_METHOD(int, get_sortIndex);
    DECLARE_INSTANCE_METHOD(bool, get_isKicked);
    DECLARE_INSTANCE_METHOD(GlobalNamespace::MultiplayerAvatarData, get_multiplayerAvatarData);
    DECLARE_INSTANCE_METHOD(GlobalNamespace::DisconnectedReason, get_disconnectedReason);
    DECLARE_INSTANCE_METHOD(bool, HasState, Il2CppString* state);

public:
    static const constexpr UnityEngine::Color DefaultColor{ 0.031f, 0.752f, 1.0f, 1.0f };

    /// <summary>
    /// Platform
    /// </summary>
        Platform platform = Platform::OculusQuest;
        Platform get_platform();

    static ExtendedPlayer* CS_ctor(GlobalNamespace::IConnectedPlayer* player);
    static ExtendedPlayer* CS_ctor(GlobalNamespace::IConnectedPlayer* player, Il2CppString* platformID, Platform platform, UnityEngine::Color playerColor);
    static ExtendedPlayer* CS_ctor(GlobalNamespace::IConnectedPlayer* player, Il2CppString* platformID, Platform platform, Il2CppString* mpexVersion, UnityEngine::Color playerColor);

    constexpr const UnityEngine::Color& get_playerColor() const {
        return playerColor;
    }

    constexpr bool HasState_CPP(std::string& state) const {
        return _connectedPlayer->HasState(il2cpp_utils::newcsstr(state));
    }

    constexpr ExtendedPlayer* get_self() {
        return this;
    }

)
#undef EPInterfaces

#define EPPInterfaces { classof(LiteNetLib::Utils::INetSerializable*), classof(GlobalNamespace::IPoolablePacket*) }
___DECLARE_TYPE_WRAPPER_INHERITANCE(MultiQuestensions::Extensions, ExtendedPlayerPacket, Il2CppTypeEnum::IL2CPP_TYPE_CLASS,
    Il2CppObject, "MultiplayerExtensions.Extensions",
    EPPInterfaces, 0, nullptr,
    DECLARE_CTOR(New);

    // Player and platform information
    DECLARE_INSTANCE_FIELD(Il2CppString*, platformID);
    DECLARE_INSTANCE_FIELD(Il2CppString*, mpexVersion);
    DECLARE_INSTANCE_FIELD(UnityEngine::Color, playerColor);

    DECLARE_OVERRIDE_METHOD(void, Serialize, il2cpp_utils::FindMethodUnsafe(classof(LiteNetLib::Utils::INetSerializable*), "Serialize", 1), LiteNetLib::Utils::NetDataWriter* writer);
    DECLARE_OVERRIDE_METHOD(void, Deserialize, il2cpp_utils::FindMethodUnsafe(classof(LiteNetLib::Utils::INetSerializable*), "Deserialize", 1), LiteNetLib::Utils::NetDataReader* reader);
    DECLARE_OVERRIDE_METHOD(void, Release, il2cpp_utils::FindMethodUnsafe(classof(GlobalNamespace::IPoolablePacket*), "Release", 0));

    public:
        Platform platform;

        static ExtendedPlayerPacket* Init(Il2CppString* platformID, Platform platform, UnityEngine::Color playerColor);
)
#undef EPPInterfaces