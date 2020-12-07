#pragma once
#include "main.hpp"
#include "custom-types/shared/types.hpp"
#include "custom-types/shared/macros.hpp"

DECLARE_CLASS_INTERFACES(MultiplayerExtensions::Beatmaps , PreviewBeatmapPacket, "System", "Object", sizeof(Il2CppObject),
	(il2cpp_utils::GetClassFromName("LiteNetLib::Utils", "INetSerializable"),
	il2cpp_utils::GetClassFromName("", "IPoolablePacket")),

	DECLARE_INSTANCE_FIELD(Il2CppString*, levelId);
	DECLARE_INSTANCE_FIELD(Il2CppString*, levelKey);
	DECLARE_INSTANCE_FIELD(Il2CppString*, songName);
	DECLARE_INSTANCE_FIELD(Il2CppString*, songSubName);
	DECLARE_INSTANCE_FIELD(Il2CppString*, songAuthorName);
	DECLARE_INSTANCE_FIELD(Il2CppString*, levelAuthorName);
	DECLARE_INSTANCE_FIELD(float, beatsPerMinute);
	DECLARE_INSTANCE_FIELD(float, songDuration);

	DECLARE_INSTANCE_FIELD(bool, isDownloadable);
	DECLARE_INSTANCE_FIELD(Array<uint8_t>*, coverImage);

	DECLARE_INSTANCE_FIELD(Il2CppString*, characteristic);
	DECLARE_INSTANCE_FIELD(unsigned int, difficulty);

	DECLARE_OVERRIDE_METHOD(void, Serialize, il2cpp_utils::FindMethod("LiteNetLib::Utils", "INetSerializable", "Serialize"), LiteNetLib::Utils::NetDataWriter*);
	DECLARE_OVERRIDE_METHOD(void, Deserialize, il2cpp_utils::FindMethod("LiteNetLib::Utils", "INetSerializable", "Deserialize"), LiteNetLib::Utils::NetDataReader*);
	DECLARE_OVERRIDE_METHOD(void, Release, il2cpp_utils::FindMethod("", "IPoolablePacket", "Release"));

	REGISTER_FUNCTION(PreviewBeatmapPacket,
		REGISTER_FIELD(levelId);
		REGISTER_FIELD(levelKey);
		REGISTER_FIELD(songName);
		REGISTER_FIELD(songSubName);
		REGISTER_FIELD(songAuthorName);
		REGISTER_FIELD(levelAuthorName);
		REGISTER_FIELD(beatsPerMinute);
		REGISTER_FIELD(songDuration);

		REGISTER_FIELD(isDownloadable);
		REGISTER_FIELD(coverImage);

		REGISTER_FIELD(characteristic);
		REGISTER_FIELD(difficulty);

		REGISTER_METHOD(Serialize);
		REGISTER_METHOD(Deserialize);
		REGISTER_METHOD(Release);
	)
)