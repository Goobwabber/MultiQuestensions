#pragma once
#include "main.hpp"
#include "custom-types/shared/types.hpp"
#include "custom-types/shared/macros.hpp"

using CallbackAction = System::Action_3<LiteNetLib::Utils::NetDataReader*, int, GlobalNamespace::IConnectedPlayer*>;
using CallbackDictionary = System::Collections::Generic::Dictionary_2<Il2CppString*, CallbackAction*>;
using StringList = System::Collections::Generic::List_1<Il2CppString*>;

DECLARE_CLASS_CODEGEN(MultiQuestensions, PacketSerializer, GlobalNamespace::INetworkPacketSubSerializer_1<GlobalNamespace::IConnectedPlayer*>,
	DECLARE_INSTANCE_FIELD_DEFAULT(CallbackDictionary*, packetHandlers, CallbackDictionary::New_ctor());
	DECLARE_INSTANCE_FIELD_DEFAULT(StringList*, registeredTypes, StringList::New_ctor());

	DECLARE_METHOD(void, Serialize, LiteNetLib::Utils::NetDataWriter* writer, LiteNetLib::Utils::INetSerializable* packet);
	DECLARE_METHOD(void, Deserialize, LiteNetLib::Utils::NetDataReader* reader, int length, GlobalNamespace::IConnectedPlayer* data);
	DECLARE_METHOD(bool, HandlesType, System::Type* type);
	DECLARE_METHOD(void, RegisterCallback, Il2CppString* identifier, CallbackAction* callback);
		
	REGISTER_FUNCTION(PacketSerializer,
		REGISTER_FIELD(packetHandlers);
		REGISTER_FIELD(registeredTypes);

		REGISTER_METHOD(Serialize);
		REGISTER_METHOD(Deserialize);
		REGISTER_METHOD(HandlesType);
		REGISTER_METHOD(RegisterCallback);
	)
)