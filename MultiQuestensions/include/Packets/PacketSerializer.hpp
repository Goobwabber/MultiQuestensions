#pragma once
#include "main.hpp"
#include "custom-types/shared/types.hpp"
#include "custom-types/shared/macros.hpp"

#include "System/Action_3.hpp"
#include "LiteNetLib/Utils/NetDataReader.hpp"
#include "LiteNetLib/Utils/NetDataWriter.hpp"
#include "System/Collections/Generic/Dictionary_2.hpp"
#include "System/Collections/Generic/List_1.hpp"
#include "GlobalNamespace/INetworkPacketSubSerializer_1.hpp"
#include "System/Type.hpp"

using CallbackAction = System::Action_3<LiteNetLib::Utils::NetDataReader*, int, GlobalNamespace::IConnectedPlayer*>;
using CallbackDictionary = System::Collections::Generic::Dictionary_2<Il2CppString*, CallbackAction*>;
using StringList = System::Collections::Generic::List_1<Il2CppString*>;

DECLARE_CLASS_CODEGEN(MultiQuestensions, PacketSerializer, GlobalNamespace::INetworkPacketSubSerializer_1<GlobalNamespace::IConnectedPlayer*>,
	DECLARE_INSTANCE_FIELD(CallbackDictionary*, packetHandlers);
	DECLARE_INSTANCE_FIELD(StringList*, registeredTypes);
	//DECLARE_INSTANCE_FIELD_DEFAULT(CallbackDictionary*, packetHandlers, CallbackDictionary::New_ctor());
	//DECLARE_INSTANCE_FIELD_DEFAULT(StringList*, registeredTypes, StringList::New_ctor());
	//DECLARE_CTOR(packetHandlers, CallbackDictionary*);
	//DECLARE_CTOR(registeredTypes, StringList*);

	DECLARE_METHOD(void, Serialize, LiteNetLib::Utils::NetDataWriter* writer, LiteNetLib::Utils::INetSerializable* packet);
	DECLARE_METHOD(void, Deserialize, LiteNetLib::Utils::NetDataReader* reader, int length, GlobalNamespace::IConnectedPlayer* data);
	DECLARE_METHOD(bool, HandlesType, Il2CppReflectionType* type);
	DECLARE_METHOD(void, RegisterCallback, Il2CppString* identifier, CallbackAction* callback);
		
	REGISTER_FUNCTION(PacketSerializer,
		//REGISTER_FIELD(packetHandlers);
		//REGISTER_FIELD(registeredTypes);

		REGISTER_METHOD(Serialize);
		REGISTER_METHOD(Deserialize);
		REGISTER_METHOD(HandlesType);
		REGISTER_METHOD(RegisterCallback);
	)
)

/*
namespace MultiQuestensions {
	class PacketSerializer {
		public:
			CallbackDictionary* packetHandlers;
			StringList* registeredTypes;
	};
}
*/