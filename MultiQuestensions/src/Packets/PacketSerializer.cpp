#include "Packets/PacketSerializer.hpp"

DEFINE_CLASS(MultiQuestensions::PacketSerializer);

namespace MultiQuestensions {
	void PacketSerializer::Construct() {
		registeredTypes = std::move(StringList());
		packetHandlers = std::move(CallbackDictionary());
	}

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
		if (packetHandlers.find(packetType) != packetHandlers.end()) {
			try {
				packetHandlers[packetType]->Invoke(reader, length, data);
			} catch (const std::exception& e) {
				getLogger().warning("An exception was thrown while processing custom packet");
				getLogger().error(e.what());
			}
		}

		int processedBytes = reader->get_Position() - prevPosition;
		reader->SkipBytes(length - processedBytes);
	}

	bool PacketSerializer::HandlesType(Il2CppReflectionType* type) {
		return std::find(registeredTypes.begin(), registeredTypes.end(), type->ToString()) != registeredTypes.end();
	}

	void PacketSerializer::RegisterCallback(Il2CppString* identifier, CallbackBase* callback) {
		registeredTypes.push_back(identifier);
		packetHandlers[identifier] = callback;
	}

	void PacketSerializer::UnregisterCallback(Il2CppString* identifier) {
		remove(registeredTypes.begin(), registeredTypes.end(), identifier);
		packetHandlers.erase(identifier);
	}
}