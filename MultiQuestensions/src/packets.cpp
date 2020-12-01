#include "packets.hpp"



// CustomPacket
Il2CppString* CustomPacket::GetIdentifier() {
	std::string id = get_identifier();
	return createcsstr(id, il2cpp_utils::StringType::Temporary);
}



// PacketSerializer
void PacketSerializer::Serialize(LiteNetLib::Utils::NetDataWriter* writer, LiteNetLib::Utils::INetSerializable* packet) {
	CustomPacket* cPacket = il2cpp_utils::down_cast<CustomPacket, LiteNetLib::Utils::INetSerializable>(packet);
	writer->Put(cPacket->GetIdentifier());
	cPacket->Serialize(writer);
}

void PacketSerializer::Deserialize(LiteNetLib::Utils::NetDataReader* reader, int length, GlobalNamespace::IConnectedPlayer* data) {
	Il2CppString* packetType = reader->GetString();
	if (packetHandlers.find(packetType) != packetHandlers.end()) {
		packetHandlers[packetType](reader, length, data);
	}
}

bool PacketSerializer::HandlesType(System::Type* type) {
	return registeredTypes.find(type->ToString()) != registeredTypes.end();
}



// PacketManager
PacketManager::PacketManager(GlobalNamespace::MultiplayerSessionManager* sessionManager) {
	_sessionManager = sessionManager;
	_sessionManager->RegisterSerializer(GlobalNamespace::MultiplayerSessionManager_MessageType(100), &packetSerializer);
}

void PacketManager::Send(LiteNetLib::Utils::INetSerializable* message) { _sessionManager->Send(message); }
void PacketManager::SendUnreliable(LiteNetLib::Utils::INetSerializable* message) { _sessionManager->Send(message); }