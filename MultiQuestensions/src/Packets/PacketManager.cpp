#include "Packets/PacketManager.hpp"
#include "Packets/PacketSerializer.hpp"
#include "beatsaber-hook/shared/utils/il2cpp-utils.hpp"

namespace MultiQuestensions {
	PacketManager::PacketManager(GlobalNamespace::MultiplayerSessionManager* sessionManager) {
		if (packetSerializer == nullptr) {
			getLogger().info("Creating Packet serializer.");

			_sessionManager = sessionManager;
			packetSerializer = CRASH_UNLESS(il2cpp_utils::New<PacketSerializer*>());
			GlobalNamespace::MultiplayerSessionManager_MessageType messageType = (GlobalNamespace::MultiplayerSessionManager_MessageType)100;

			_sessionManager->RegisterSerializer(messageType, reinterpret_cast<GlobalNamespace::INetworkPacketSubSerializer_1<GlobalNamespace::IConnectedPlayer*>*>(packetSerializer));
		} else {
			getLogger().info("Packet serializer already exists.");
		}
	}

	void PacketManager::Send(LiteNetLib::Utils::INetSerializable* message) { _sessionManager->Send(message); }
	void PacketManager::SendUnreliable(LiteNetLib::Utils::INetSerializable* message) { _sessionManager->SendUnreliable(message); }
}