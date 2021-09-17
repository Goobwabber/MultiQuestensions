#pragma once
#include "main.hpp"
#include "custom-types/shared/types.hpp"
#include "custom-types/shared/macros.hpp"

#include "LiteNetLib/Utils/NetDataReader.hpp"
#include "LiteNetLib/Utils/NetDataWriter.hpp"
#include "LiteNetLib/Utils/INetSerializable.hpp"
#include "GlobalNamespace/ThreadStaticPacketPool_1.hpp"
#include "GlobalNamespace/PacketPool_1.hpp"
#include "GlobalNamespace/IPoolablePacket.hpp"
#include "System/Object.hpp"

#define PBPInterfaces { classof(LiteNetLib::Utils::INetSerializable*), classof(GlobalNamespace::IPoolablePacket*) }

___DECLARE_TYPE_WRAPPER_INHERITANCE(MultiQuestensions::Beatmaps, PreviewBeatmapPacket, Il2CppTypeEnum::IL2CPP_TYPE_CLASS, 
	Il2CppObject, "MultiQuestensions::Beatmaps", 
	PBPInterfaces, 0, nullptr,
	//DECLARE_DEFAULT_CTOR();
	//PreviewBeatmapPacket();
	//DECLARE_CTOR(New_ctor);
	DECLARE_CTOR(New);

	// Basic Song Info/Metadata
	DECLARE_INSTANCE_FIELD(Il2CppString*, levelId);
	DECLARE_INSTANCE_FIELD(Il2CppString*, levelHash);
	DECLARE_INSTANCE_FIELD(Il2CppString*, songName);
	DECLARE_INSTANCE_FIELD(Il2CppString*, songSubName);
	DECLARE_INSTANCE_FIELD(Il2CppString*, songAuthorName);
	DECLARE_INSTANCE_FIELD(Il2CppString*, levelAuthorName);
	DECLARE_INSTANCE_FIELD(float, beatsPerMinute);
	DECLARE_INSTANCE_FIELD(float, songDuration);

	// Selection Info
	DECLARE_INSTANCE_FIELD(Il2CppString*, characteristic);
	DECLARE_INSTANCE_FIELD(uint, difficulty);

	DECLARE_OVERRIDE_METHOD(void, Serialize, il2cpp_utils::FindMethodUnsafe(classof(LiteNetLib::Utils::INetSerializable*), "Serialize", 1), LiteNetLib::Utils::NetDataWriter* writer);
	DECLARE_OVERRIDE_METHOD(void, Deserialize, il2cpp_utils::FindMethodUnsafe(classof(LiteNetLib::Utils::INetSerializable*), "Deserialize", 1), LiteNetLib::Utils::NetDataReader* reader);
	DECLARE_OVERRIDE_METHOD(void, Release, il2cpp_utils::FindMethodUnsafe(classof(GlobalNamespace::IPoolablePacket*), "Release", 0));
	
	//DECLARE_INSTANCE_METHOD(void, Release);
	//DECLARE_INSTANCE_METHOD(void, Serialize, LiteNetLib::Utils::NetDataWriter*);
	//DECLARE_INSTANCE_METHOD(void, Deserialize, LiteNetLib::Utils::NetDataReader*);

	//public:
	//	static void Install();

	//REGISTER_FUNCTION(PreviewBeatmapPacket,
	//	REGISTER_FIELD(levelId);
	//	REGISTER_FIELD(songName);
	//	REGISTER_FIELD(songSubName);
	//	REGISTER_FIELD(songAuthorName);
	//	REGISTER_FIELD(levelAuthorName);
	//	REGISTER_FIELD(beatsPerMinute);
	//	REGISTER_FIELD(songDuration);

	//	REGISTER_FIELD(coverImage);

	//	REGISTER_FIELD(characteristic);
	//	REGISTER_FIELD(difficulty);

	//	REGISTER_METHOD(Release);
	//	REGISTER_METHOD(Serialize);
	//	REGISTER_METHOD(Deserialize);
	//)
)
#undef PBPInterfaces