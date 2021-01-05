#include "Packets/PacketManager.hpp"
#include "Packets/PacketSerializer.hpp"
#include "extern/beatsaber-hook/shared/utils/il2cpp-utils.hpp"

namespace MultiQuestensions {

	PacketManager::PacketManager(GlobalNamespace::MultiplayerSessionManager* sessionManager) {
		if (packetSerializer == nullptr) {
			_sessionManager = sessionManager;
			packetSerializer = (PacketSerializer*)il2cpp_functions::object_new(PacketSerializer::klass);
			_sessionManager->RegisterSerializer((GlobalNamespace::MultiplayerSessionManager_MessageType)100, packetSerializer);
		} else {
			getLogger().info("Packet serializer already exists");
		}
	}

	void PacketManager::Send(LiteNetLib::Utils::INetSerializable* message) { _sessionManager->Send(message); }
	void PacketManager::SendUnreliable(LiteNetLib::Utils::INetSerializable* message) { _sessionManager->SendUnreliable(message); }
}