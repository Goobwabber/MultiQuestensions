#include "Extensions/ExtendedPlayer.hpp"
#include "CodegenExtensions/ColorUtility.hpp"

DEFINE_TYPE(MultiQuestensions::Extensions, ExtendedPlayer)

DEFINE_TYPE(MultiQuestensions::Extensions, ExtendedPlayerPacket)

namespace MultiQuestensions::Extensions {

	static Il2CppString* const MQE_version() {
		return il2cpp_utils::newcsstr<il2cpp_utils::CreationType::Manual>(modInfo.version);
	}

#pragma region ExtendedPlayer
	void ExtendedPlayer::ExtendedPlayer_ctor(GlobalNamespace::IConnectedPlayer* player, Il2CppString* platformID, int platform, UnityEngine::Color playerColor, Il2CppString* mpexVersion) {
		getLogger().debug("Creating ExtendedPlayer");
		_connectedPlayer = player;
		if (mpexVersion) this->mpexVersion = mpexVersion;
		else this->mpexVersion = MQE_version();
		if (platformID) {
			this->platformID = platformID;
			this->platform = (Platform)platform;
			this->playerColor = playerColor;
		}
		getLogger().debug("Finished Creating ExtendedPlayer");
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
		writer->Put(Il2CppString::Concat(il2cpp_utils::newcsstr("#"), UnityEngine::ColorUtility::ToHtmlStringRGB(playerColor)));
		writer->Put((int)platform);
		getLogger().debug("Serialize ExtendedPlayerPacket done");
	}

	void ExtendedPlayerPacket::Deserialize(LiteNetLib::Utils::NetDataReader* reader) {
		getLogger().debug("ExtendedPlayerPacket::Deserialize");

		platformID = reader->GetString();
		mpexVersion = reader->GetString();
		if (!UnityEngine::ColorUtility::TryParseHtmlString(reader->GetString(), ByRef(playerColor)))
			this->playerColor = ExtendedPlayer::DefaultColor;
		if (reader->get_AvailableBytes() >= 4) // Verify this works when the platform int exists.
			this->platform = (Platform)reader->GetInt();
		else
			this->platform = Platform::Unknown;
		getLogger().debug("Deserialize ExtendedPlayerPacket done");
	}

	ExtendedPlayerPacket* ExtendedPlayerPacket::Init(Il2CppString* platformID, Platform platform, UnityEngine::Color playerColor)
	{
		this->platformID = platformID;
		this->mpexVersion = il2cpp_utils::newcsstr(modInfo.version);
		this->playerColor = playerColor;
		this->platform = platform;
		return this;
	}
#pragma endregion
}