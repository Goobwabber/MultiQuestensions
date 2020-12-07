#include "PacketManager.hpp"

namespace MultiQuestensions {
	PacketManager::PacketManager(GlobalNamespace::MultiplayerSessionManager* sessionManager) {
		_sessionManager = sessionManager;
		packetSerializer = new PacketSerializer();
		_sessionManager->RegisterSerializer(GlobalNamespace::MultiplayerSessionManager_MessageType(100), packetSerializer);
	}

	void PacketManager::Send(Il2CppObject* message) { _sessionManager->Send(message); }
	void PacketManager::SendUnreliable(Il2CppObject* message) { _sessionManager->SendUnreliable(message); }
}