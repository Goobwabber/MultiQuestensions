#include "Packets/PacketSerializer.hpp"
#include "GlobalNamespace/BeatmapIdentifierNetSerializable.hpp"

DEFINE_TYPE(MultiQuestensions, PacketSerializer);

namespace MultiQuestensions {
	void PacketSerializer::Construct() {
		registeredTypes = std::move(TypeDictionary());
		packetHandlers = std::move(CallbackDictionary());
	}

	void PacketSerializer::Deconstruct() {
		getLogger().debug("PacketSerializer::Deconstruct");
		for (auto it = packetHandlers.begin(); it != packetHandlers.end(); it++) {
			delete it->second;
		}
		packetHandlers.clear();
	}

	void PacketSerializer::Serialize(LiteNetLib::Utils::NetDataWriter* writer, LiteNetLib::Utils::INetSerializable* packet) {
		Il2CppReflectionType* packetType = il2cpp_utils::GetSystemType(il2cpp_functions::object_get_class(reinterpret_cast<Il2CppObject*>(packet)));
		getLogger().debug("Serialize packetType is: %s", to_utf8(csstrtostr(packetType->ToString())).c_str());
		getLogger().debug("Registered types check: %s", registeredTypes[packetType].c_str());
		writer->Put(il2cpp_utils::newcsstr(registeredTypes[packetType]));
		//writer->Put(packetType->ToString());
		getLogger().debug("Writer Put");
		getLogger().debug("Nullptr checking: packet: %p, writer: %p", packet, writer);
		packet->Serialize(writer);
		getLogger().debug("Serialize Finished");
	}

	void PacketSerializer::Deserialize(LiteNetLib::Utils::NetDataReader* reader, int length, GlobalNamespace::IConnectedPlayer* data) {
		getLogger().debug("PacketSerializer::Deserialize");
		int prevPosition = reader->get_Position();
		std::string packetType = to_utf8(csstrtostr(reader->GetString()));
		getLogger().debug("packetType: %s", packetType.c_str());
		length -= reader->get_Position() - prevPosition;
		getLogger().debug("length: %d", length);
		prevPosition = reader->get_Position();
		if (packetHandlers.find(packetType) != packetHandlers.end()) {
			getLogger().debug("packetHandlers found PacketType, try Invoke");
			try {
				packetHandlers[packetType]->Invoke(reader, length, data);
			}
			catch (const std::exception& e) {
				getLogger().warning("An C++ exception was thrown while processing custom packet");
				getLogger().error("%s", e.what());
			}
			catch (...) {
				getLogger().warning("An Unknown exception was thrown while processing custom packet");
			}
		}
		int processedBytes = reader->get_Position() - prevPosition;
		reader->SkipBytes(length - processedBytes);
	}

	bool PacketSerializer::HandlesType(Il2CppReflectionType* type) {
		auto it = registeredTypes.find(type);
		if (it != registeredTypes.end()) {
			getLogger().debug("HandlesType: %s", it->second.c_str());
			return true;
		}
		return false;
	}

	void PacketSerializer::UnregisterCallback(std::string identifier) {
		getLogger().debug("UnregisterCallback called");

		for (auto it = registeredTypes.begin(); it != registeredTypes.end(); it++) {
			if (it->second == identifier) registeredTypes.erase(it);
		}

		auto itr = packetHandlers.find(identifier);
		if (itr != packetHandlers.end()) {
			delete itr->second;
			packetHandlers.erase(itr);
		}
	}
}