#pragma once
#include "main.hpp"
#include "Callback.hpp"
#include "custom-types/shared/types.hpp"
#include "custom-types/shared/macros.hpp"

#include "vector"
#include "System/Action_3.hpp"
#include "LiteNetLib/Utils/NetDataWriter.hpp"
#include "System/Collections/Generic/Dictionary_2.hpp"
#include "System/Collections/Generic/List_1.hpp"
#include "GlobalNamespace/INetworkPacketSubSerializer_1.hpp"
#include "System/Type.hpp"

/*DECLARE_CLASS_INTERFACES(Il2CppNamespace, MyCustomBeatmapLevelPackCollection, "System", "Object", sizeof(Il2CppObject),
	il2cpp_utils::GetClassFromName("", "IBeatmapLevelPackCollection"),
	DECLARE_INSTANCE_FIELD(Il2CppArray*, wrappedArr);

	DECLARE_OVERRIDE_METHOD(Il2CppArray*, get_beatmapLevelPacks, il2cpp_utils::FindMethod("", "IBeatmapLevelPackCollection", "get_beatmapLevelPacks"));
	DECLARE_CTOR(ctor, Il2CppArray* originalArray);
	REGISTER_FUNCTION(MyCustomBeatmapLevelPackCollection,
		modLogger().debug("Registering MyCustomBeatmapLevelPackCollection!");
		REGISTER_METHOD(get_beatmapLevelPacks);
		REGISTER_METHOD(ctor);
		REGISTER_FIELD(wrappedArr);
	)
)*/

using CallbackDictionary = std::map<Il2CppString*, MultiQuestensions::CallbackBase*>;
using StringList = std::vector<Il2CppString*>;

DECLARE_CLASS_INTERFACES(MultiQuestensions, PacketSerializer, "System", "Object", sizeof(Il2CppObject),
	classof(GlobalNamespace::INetworkPacketSubSerializer_1<GlobalNamespace::IConnectedPlayer*>*),
	DECLARE_OVERRIDE_METHOD(void, Serialize, il2cpp_utils::FindMethod("", "IConnectedPlayer", "Serialize"), LiteNetLib::Utils::NetDataWriter* writer, LiteNetLib::Utils::INetSerializable* packet);
	DECLARE_OVERRIDE_METHOD(void, Deserialize, il2cpp_utils::FindMethod("", "IConnectedPlayer", "Deserialize"), LiteNetLib::Utils::NetDataReader* reader, int length, GlobalNamespace::IConnectedPlayer* data);
	DECLARE_OVERRIDE_METHOD(bool, HandlesType, il2cpp_utils::FindMethod("", "IConnectedPlayer", "HandlesType"), Il2CppReflectionType* type);
	
	DECLARE_CTOR(Construct);

	REGISTER_FUNCTION(PacketSerializer,
		REGISTER_METHOD(Serialize);
		REGISTER_METHOD(Deserialize);
		REGISTER_METHOD(HandlesType);
		REGISTER_METHOD(Construct);
	)

	private:
		CallbackDictionary packetHandlers;
		StringList registeredTypes;

	public:
		void RegisterCallback(Il2CppString* identifier, CallbackBase* callback);
		void UnregisterCallback(Il2CppString* identifier);
)