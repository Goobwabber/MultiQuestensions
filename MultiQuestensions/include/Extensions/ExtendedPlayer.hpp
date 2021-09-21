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

DECLARE_CLASS_CODEGEN(MultiQuestensions::Extensions, ExtendedPlayer, GlobalNamespace::IConnectedPlayer,

    static const constexpr UnityEngine::Color DefaultColor = UnityEngine::Color(0.031f, 0.752f, 1.0f);

    DECLARE_INSTANCE_FIELD(GlobalNamespace::IConnectedPlayer*, _connectedPlayer);

    /// <summary>
    /// Platform User ID
    /// </summary>
    DECLARE_INSTANCE_FIELD(Il2CppString*, platformID);
    DECLARE_INSTANCE_METHOD(Il2CppString*, get_platformID);

    /// <summary>
    /// Platform
    /// </summary>
    public:
    Platform platform = Platform::OculusQuest;
    Platform get_platform();

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

public:
    static ExtendedPlayer* CS_ctor(GlobalNamespace::IConnectedPlayer* player);
    static ExtendedPlayer* CS_ctor(GlobalNamespace::IConnectedPlayer* player, Il2CppString* platformID, Platform platform, UnityEngine::Color playerColor);
    static ExtendedPlayer* CS_ctor(GlobalNamespace::IConnectedPlayer* player, Il2CppString* platformID, Platform platform, Il2CppString* mpexVersion, UnityEngine::Color playerColor);

    constexpr const UnityEngine::Color& get_playerColor() const {
        return playerColor;
    }

    constexpr bool get_isMe() const {
        return _connectedPlayer->get_isMe();
    }

    constexpr Il2CppString* const get_userId() const {
        return _connectedPlayer->get_userId();
    }

    constexpr Il2CppString* const get_userName() const {
        return _connectedPlayer->get_userName();
    }

    constexpr float get_currentLatency() const {
        return _connectedPlayer->get_currentLatency();
    }

    constexpr bool get_isConnected() const {
        return _connectedPlayer->get_isConnected();
    }

    constexpr bool get_isConnectionOwner() const {
        return _connectedPlayer->get_isConnectionOwner();
    }

    constexpr float get_offsetSyncTime() const {
        return _connectedPlayer->get_offsetSyncTime();
    }

    constexpr int get_sortIndex() const {
        return _connectedPlayer->get_sortIndex();
    }
    
    constexpr bool get_isKicked() const {
        return _connectedPlayer->get_isKicked();
    }

    constexpr GlobalNamespace::MultiplayerAvatarData get_multiplayerAvatarData() const {
        return _connectedPlayer->get_multiplayerAvatarData();
    }

    constexpr GlobalNamespace::DisconnectedReason get_disconnectedReason() const {
        return _connectedPlayer->get_disconnectedReason();
    }

    constexpr bool HasState(std::string& state) const {
        return _connectedPlayer->HasState(il2cpp_utils::newcsstr(state));
    }

    constexpr bool HasState(Il2CppString* state) const {
        return _connectedPlayer->HasState(state);
    }
)

#define EBPInterfaces { classof(LiteNetLib::Utils::INetSerializable*), classof(GlobalNamespace::IPoolablePacket*) }
___DECLARE_TYPE_WRAPPER_INHERITANCE(MultiQuestensions::Extensions, ExtendedPlayerPacket, Il2CppTypeEnum::IL2CPP_TYPE_CLASS,
    Il2CppObject, "MultiplayerExtensions.Extensions",
    EBPInterfaces, 0, nullptr,
    DECLARE_CTOR(New);
public:
    static ExtendedPlayerPacket* Init(Il2CppString* platformID, Platform platform, UnityEngine::Color playerColor);

    // Player and platform information
    DECLARE_INSTANCE_FIELD(Il2CppString*, platformID);
    Platform platform;
    DECLARE_INSTANCE_FIELD(Il2CppString*, mpexVersion);
    DECLARE_INSTANCE_FIELD(UnityEngine::Color, playerColor);

    DECLARE_OVERRIDE_METHOD(void, Serialize, il2cpp_utils::FindMethodUnsafe(classof(LiteNetLib::Utils::INetSerializable*), "Serialize", 1), LiteNetLib::Utils::NetDataWriter* writer);
    DECLARE_OVERRIDE_METHOD(void, Deserialize, il2cpp_utils::FindMethodUnsafe(classof(LiteNetLib::Utils::INetSerializable*), "Deserialize", 1), LiteNetLib::Utils::NetDataReader* reader);
    DECLARE_OVERRIDE_METHOD(void, Release, il2cpp_utils::FindMethodUnsafe(classof(GlobalNamespace::IPoolablePacket*), "Release", 0));

)
#undef EBPInterfaces