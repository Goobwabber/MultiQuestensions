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
	DECLARE_INSTANCE_FIELD(StringW, levelId);
	DECLARE_INSTANCE_FIELD(StringW, levelHash);
	DECLARE_INSTANCE_FIELD(StringW, songName);
	DECLARE_INSTANCE_FIELD(StringW, songSubName);
	DECLARE_INSTANCE_FIELD(StringW, songAuthorName);
	DECLARE_INSTANCE_FIELD(StringW, levelAuthorName);
	DECLARE_INSTANCE_FIELD(float, beatsPerMinute);
	DECLARE_INSTANCE_FIELD(float, songDuration);

	// Selection Info
	DECLARE_INSTANCE_FIELD(StringW, characteristic);
	DECLARE_INSTANCE_FIELD(uint, difficulty);

	DECLARE_OVERRIDE_METHOD(void, Serialize, il2cpp_utils::FindMethodUnsafe(classof(LiteNetLib::Utils::INetSerializable*), "Serialize", 1), LiteNetLib::Utils::NetDataWriter* writer);
	DECLARE_OVERRIDE_METHOD(void, Deserialize, il2cpp_utils::FindMethodUnsafe(classof(LiteNetLib::Utils::INetSerializable*), "Deserialize", 1), LiteNetLib::Utils::NetDataReader* reader);
	DECLARE_OVERRIDE_METHOD(void, Release, il2cpp_utils::FindMethodUnsafe(classof(GlobalNamespace::IPoolablePacket*), "Release", 0));
	public:
	LiteNetLib::Utils::INetSerializable* ToINetSerializable() noexcept { return reinterpret_cast<LiteNetLib::Utils::INetSerializable*>(this); }
)
#undef PBPInterfaces