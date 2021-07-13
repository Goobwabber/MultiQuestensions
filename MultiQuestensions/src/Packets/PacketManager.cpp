#include "Packets/PacketManager.hpp"
#include "Packets/PacketSerializer.hpp"
#include "beatsaber-hook/shared/utils/il2cpp-utils.hpp"
#include "UnityEngine/GameObject.hpp"

#include "GlobalNamespace/StandardScoreSyncStateNetSerializable.hpp"

namespace MultiQuestensions {
	PacketManager::PacketManager(GlobalNamespace::MultiplayerSessionManager* sessionManager) {
		//if (packetSerializer == nullptr) {
			getLogger().info("Creating Packet serializer.");

			_sessionManager = sessionManager;
			packetSerializer = CRASH_UNLESS(il2cpp_utils::New<PacketSerializer*>());
			GlobalNamespace::MultiplayerSessionManager_MessageType messageType = (GlobalNamespace::MultiplayerSessionManager_MessageType)100;

			_sessionManager->RegisterSerializer(messageType, reinterpret_cast<GlobalNamespace::INetworkPacketSubSerializer_1<GlobalNamespace::IConnectedPlayer*>*>(packetSerializer));
		//} else {
		//	getLogger().info("Packet serializer already exists.");
		//}
	}
	void PacketManager::Send(LiteNetLib::Utils::INetSerializable* message) { 
		getLogger().debug("Running send");
		if (_sessionManager != nullptr && message != nullptr) {
			_sessionManager->Send(message);
			getLogger().debug("Sent message");
		}
		else getLogger().error("Failed to Send message");
	}
	void PacketManager::SendUnreliable(LiteNetLib::Utils::INetSerializable* message) { 
		if (_sessionManager != nullptr && message != nullptr) {
			_sessionManager->SendUnreliable(message);
			getLogger().debug("Sent message");
		}
		else getLogger().error("Failed to SendUnreliable message");
	}
}