#include "Packets/PacketManager.hpp"

namespace MultiQuestensions {
	PacketManager::PacketManager(GlobalNamespace::MultiplayerSessionManager* sessionManager) {
		_sessionManager = sessionManager;
		packetSerializer = (PacketSerializer*)il2cpp_functions::object_new(PacketSerializer::klass); 
		//packetSerializer = new PacketSerializer();
		auto messageType = GlobalNamespace::MultiplayerSessionManager_MessageType(100u);
		_sessionManager->RegisterSerializer(messageType, packetSerializer);
	}

	void PacketManager::Send(LiteNetLib::Utils::INetSerializable* message) { _sessionManager->Send(message); }
	void PacketManager::SendUnreliable(LiteNetLib::Utils::INetSerializable* message) { _sessionManager->SendUnreliable(message); }
}