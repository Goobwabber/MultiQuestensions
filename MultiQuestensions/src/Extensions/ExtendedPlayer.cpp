#include "Extensions/ExtendedPlayer.hpp"
#include "CodegenExtensions/ColorUtility.hpp"

DEFINE_TYPE(MultiQuestensions::Extensions, ExtendedPlayer)

DEFINE_TYPE(MultiQuestensions::Extensions, ExtendedPlayerPacket)

namespace MultiQuestensions::Extensions {

    static Il2CppString* MQE_Protocol() {
        static Il2CppString* protocol = il2cpp_utils::newcsstr<il2cpp_utils::CreationType::Manual>(MPEX_PROTOCOL);
        return protocol;
    }

#pragma region ExtendedPlayer
    void ExtendedPlayer::ExtendedPlayer_ctor(GlobalNamespace::IConnectedPlayer* player, Il2CppString* platformID, int platform, UnityEngine::Color playerColor, Il2CppString* mpexVersion) {
        getLogger().debug("Creating ExtendedPlayer");
        _connectedPlayer = player;
        if (mpexVersion) this->mpexVersion = mpexVersion;
        else this->mpexVersion = MQE_Protocol();
        if (platformID) {
            this->platformID = platformID;
            this->platform = (Platform)platform;
            this->playerColor = playerColor;
        }
        getLogger().debug("Finished Creating ExtendedPlayer");
    }

    ExtendedPlayer* ExtendedPlayer::CS_ctor(GlobalNamespace::IConnectedPlayer* player) {
        return THROW_UNLESS(il2cpp_utils::New<ExtendedPlayer*>(player, static_cast<Il2CppString*>(nullptr), 3, DefaultColor, static_cast<Il2CppString*>(nullptr)));
    }
    ExtendedPlayer* ExtendedPlayer::CS_ctor(GlobalNamespace::IConnectedPlayer* player, Il2CppString* platformID, Platform platform, UnityEngine::Color playerColor) {
        return THROW_UNLESS(il2cpp_utils::New<ExtendedPlayer*>(player, platformID, (int)platform, playerColor, static_cast<Il2CppString*>(nullptr)));
    }
    ExtendedPlayer* ExtendedPlayer::CS_ctor(GlobalNamespace::IConnectedPlayer* player, Il2CppString* platformID, Platform platform, Il2CppString* mpexVersion, UnityEngine::Color playerColor) {
        return THROW_UNLESS(il2cpp_utils::New<ExtendedPlayer*>(player, platformID, (int)platform, playerColor, mpexVersion));
    }
#pragma endregion

#pragma region Getters
    Il2CppString* ExtendedPlayer::get_platformID() {
        return platformID;
    }
    Platform ExtendedPlayer::get_platform() {
        return platform;
    }
    Il2CppString* ExtendedPlayer::get_mpexVersion() {
        return mpexVersion;
    }

    bool ExtendedPlayer::get_isMe() {
        return _connectedPlayer->get_isMe();
    }

    Il2CppString* ExtendedPlayer::get_userId() {
        return _connectedPlayer->get_userId();
    }

    Il2CppString* ExtendedPlayer::get_userName() {
        return _connectedPlayer->get_userName();
    }

    float ExtendedPlayer::get_currentLatency() {
        return _connectedPlayer->get_currentLatency();
    }

    bool ExtendedPlayer::get_isConnected() {
        return _connectedPlayer->get_isConnected();
    }

    bool ExtendedPlayer::get_isConnectionOwner() {
        return _connectedPlayer->get_isConnectionOwner();
    }

    float ExtendedPlayer::get_offsetSyncTime() {
        return _connectedPlayer->get_offsetSyncTime();
    }

    int ExtendedPlayer::get_sortIndex() {
        return _connectedPlayer->get_sortIndex();
    }

    bool ExtendedPlayer::get_isKicked() {
        return _connectedPlayer->get_isKicked();
    }

    GlobalNamespace::MultiplayerAvatarData ExtendedPlayer::get_multiplayerAvatarData() {
        return _connectedPlayer->get_multiplayerAvatarData();
    }

    GlobalNamespace::DisconnectedReason ExtendedPlayer::get_disconnectedReason() {
        return _connectedPlayer->get_disconnectedReason();
    }

    bool ExtendedPlayer::HasState(Il2CppString* state) {
        return _connectedPlayer->HasState(state);
    }
#pragma endregion

#pragma region ExtendedPlayerPacket
    void ExtendedPlayerPacket::New() {}

    void ExtendedPlayerPacket::Release() {
        getLogger().debug("ExtendedPlayerPacket::Release");
        GlobalNamespace::ThreadStaticPacketPool_1<ExtendedPlayerPacket*>::get_pool()->Release(this);
    }

    void ExtendedPlayerPacket::Serialize(LiteNetLib::Utils::NetDataWriter* writer) {
        getLogger().debug("ExtendedPlayerPacket::Serialize");

        writer->Put(platformID);
        writer->Put(mpexVersion);
        writer->Put(UnityEngine::ColorUtility::ToHtmlStringRGB(playerColor));
        //writer->Put(Il2CppString::Concat(il2cpp_utils::newcsstr("#"), UnityEngine::ColorUtility::ToHtmlStringRGB(playerColor)));
        writer->Put((int)platform);
        getLogger().debug("Serialize ExtendedPlayerPacket done");
    }

    void ExtendedPlayerPacket::Deserialize(LiteNetLib::Utils::NetDataReader* reader) {
        getLogger().debug("ExtendedPlayerPacket::Deserialize");

        platformID = reader->GetString();
        mpexVersion = reader->GetString();
        if (!UnityEngine::ColorUtility::TryParseHtmlString(reader->GetString(), playerColor))
            this->playerColor = ExtendedPlayer::DefaultColor;
        if (reader->get_AvailableBytes() >= 4) // Verify this works when the platform int exists.
            this->platform = (Platform)reader->GetInt();
        else
            this->platform = Platform::Unknown;
        getLogger().debug("Deserialize ExtendedPlayerPacket done");
    }

    ExtendedPlayerPacket* ExtendedPlayerPacket::Init(Il2CppString* platformID, Platform platform, UnityEngine::Color playerColor)
    {
        auto extendedPlayer = THROW_UNLESS(il2cpp_utils::New<ExtendedPlayerPacket*>());
        
        extendedPlayer->platformID = platformID;
        extendedPlayer->platform = (Platform)platform;
        extendedPlayer->playerColor = playerColor;
        extendedPlayer->mpexVersion = MQE_Protocol();
        return extendedPlayer;
    }
#pragma endregion
}