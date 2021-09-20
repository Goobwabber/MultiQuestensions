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

#define PBPInterfaces { classof(LiteNetLib::Utils::INetSerializable*), classof(GlobalNamespace::IPoolablePacket*) }

___DECLARE_TYPE_WRAPPER_INHERITANCE(MultiQuestensions::Beatmaps, PreviewBeatmapPacket, Il2CppTypeEnum::IL2CPP_TYPE_CLASS, 
	Il2CppObject, "MultiQuestensions::Beatmaps", 
	PBPInterfaces, 0, nullptr,
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
)
#undef PBPInterfaces