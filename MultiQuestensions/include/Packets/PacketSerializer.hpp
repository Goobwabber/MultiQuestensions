#pragma once
#include "main.hpp"
#include "Callback.hpp"
#include "custom-types/shared/types.hpp"
#include "custom-types/shared/macros.hpp"

#include <vector>
#include "System/Action_3.hpp"
#include "LiteNetLib/Utils/NetDataWriter.hpp"
#include "System/Collections/Generic/Dictionary_2.hpp"
#include "System/Collections/Generic/List_1.hpp"
#include "GlobalNamespace/INetworkPacketSubSerializer_1.hpp"
#include "GlobalNamespace/IConnectedPlayer.hpp"
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

using CallbackDictionary = std::map<std::string, MultiQuestensions::CallbackBase*>;
using TypeDictionary = std::map<Il2CppReflectionType*, std::string>;

DECLARE_CLASS_INTERFACES(MultiQuestensions, PacketSerializer, "System", "Object", sizeof(Il2CppObject),
	{ classof(GlobalNamespace::INetworkPacketSubSerializer_1<GlobalNamespace::IConnectedPlayer*>*) },
	DECLARE_OVERRIDE_METHOD(void, Serialize, il2cpp_utils::FindMethodUnsafe(classof(GlobalNamespace::INetworkPacketSubSerializer_1<GlobalNamespace::IConnectedPlayer*>*), "Serialize", 2), LiteNetLib::Utils::NetDataWriter* writer, LiteNetLib::Utils::INetSerializable* packet);
	DECLARE_OVERRIDE_METHOD(void, Deserialize, il2cpp_utils::FindMethodUnsafe(classof(GlobalNamespace::INetworkPacketSubSerializer_1<GlobalNamespace::IConnectedPlayer*>*), "Deserialize", 3), LiteNetLib::Utils::NetDataReader* reader, int length, GlobalNamespace::IConnectedPlayer* data);
	DECLARE_OVERRIDE_METHOD(bool, HandlesType, il2cpp_utils::FindMethodUnsafe(classof(GlobalNamespace::INetworkPacketSubSerializer_1<GlobalNamespace::IConnectedPlayer*>*), "HandlesType", 1), Il2CppReflectionType* type);

	//DECLARE_INSTANCE_METHOD(void, Serialize, LiteNetLib::Utils::NetDataWriter* writer, LiteNetLib::Utils::INetSerializable* packet);
	//DECLARE_INSTANCE_METHOD(void, Deserialize, LiteNetLib::Utils::NetDataReader* reader, int length, GlobalNamespace::IConnectedPlayer* data);
	//DECLARE_INSTANCE_METHOD(bool, HandlesType, Il2CppReflectionType* type);

	DECLARE_CTOR(Construct);
	DECLARE_DTOR(Deconstruct);

	//REGISTER_FUNCTION(PacketSerializer,
	//	REGISTER_METHOD(Serialize);
	//	REGISTER_METHOD(Deserialize);
	//	REGISTER_METHOD(HandlesType);
	//	REGISTER_METHOD(Construct);
	//)

	private:
		CallbackDictionary packetHandlers;
		TypeDictionary registeredTypes;

	public:
		template <class TPacket>
		void RegisterCallback(std::string identifier, CallbackWrapper<TPacket>* callback) {
			registeredTypes[csTypeOf(TPacket)] = identifier;
			packetHandlers[identifier] = callback;
		}

		template <class TPacket>
		void UnregisterCallback() {
			getLogger().debug("UnregisterCallback called");

			auto it = registeredTypes.find(csTypeOf(TPacket));
			if (it != registeredTypes.end()) registeredTypes.erase(it);

			auto itr = packetHandlers.find(csTypeOf(TPacket)->ToString());
			if (itr != packetHandlers.end()) {
				delete itr->second;
				packetHandlers.erase(itr);
			}
		}

		void UnregisterCallback(std::string identifier);
)