#include "Packets/PacketManager.hpp"

namespace MultiQuestensions {
	PacketManager::PacketManager(GlobalNamespace::MultiplayerSessionManager* sessionManager) {
		_sessionManager = sessionManager;
		packetSerializer = (PacketSerializer*)il2cpp_functions::object_new(PacketSerializer::klass);
		_sessionManager->RegisterSerializer(GlobalNamespace::MultiplayerSessionManager_MessageType((uint8_t)100), packetSerializer);
	}

	void PacketManager::Send(LiteNetLib::Utils::INetSerializable* message) { _sessionManager->Send(message); }
	void PacketManager::SendUnreliable(LiteNetLib::Utils::INetSerializable* message) { _sessionManager->SendUnreliable(message); }
}