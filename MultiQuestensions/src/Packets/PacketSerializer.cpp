#include "Packets/PacketSerializer.hpp"

DEFINE_CLASS(MultiQuestensions::PacketSerializer);

namespace MultiQuestensions {
	void PacketSerializer::Serialize(LiteNetLib::Utils::NetDataWriter* writer, LiteNetLib::Utils::INetSerializable* packet) {
		Il2CppReflectionType* packetType = il2cpp_utils::GetSystemType(il2cpp_functions::object_get_class(reinterpret_cast<Il2CppObject*>(packet)));
		
		writer->Put(packetType->ToString());
		packet->LiteNetLib_Utils_INetSerializable_Serialize(writer);
	}

	void PacketSerializer::Deserialize(LiteNetLib::Utils::NetDataReader* reader, int length, GlobalNamespace::IConnectedPlayer* data) {
		int prevPosition = reader->get_Position();
		Il2CppString* packetType = reader->GetString();
		length -= reader->get_Position() - prevPosition;
		prevPosition = reader->get_Position();
		if (packetHandlers->ContainsKey(packetType)) {
			try {
				packetHandlers->get_Item(packetType)->Invoke(reader, length, data);
			} catch (const std::exception& e) {
				getLogger().warning("An exception was thrown while processing custom packet");
				getLogger().error(e.what());
			}
		}

		int processedBytes = reader->get_Position() - prevPosition;
		reader->SkipBytes(length - processedBytes);
	}

	bool PacketSerializer::HandlesType(Il2CppReflectionType* type) {
		return registeredTypes->Contains(type->ToString());
	}

	void PacketSerializer::RegisterCallback(Il2CppString* identifier, CallbackAction* callback) {
		registeredTypes->Add(identifier);
		packetHandlers->set_Item(identifier, callback);
	}
}