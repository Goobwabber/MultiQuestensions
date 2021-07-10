#include "Packets/PacketManager.hpp"
#include "Packets/PacketSerializer.hpp"
#include "beatsaber-hook/shared/utils/il2cpp-utils.hpp"
#include "UnityEngine/GameObject.hpp"

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
		//if (_sessionManager) _sessionManager->Send(message);
		//else {
		getLogger().debug("Running send");
			//_sessionManager = UnityEngine::GameObject::FindObjectOfType<GlobalNamespace::MultiplayerSessionManager*>();
		if (_sessionManager == nullptr) {
			getLogger().error("Aborting send, no sessionManager instance");
		} else if (message == nullptr) getLogger().error("Aborting send, message is nullptr");
			getLogger().debug("Got MultiplayerSessionManager");
			//_sessionManager->Send<LiteNetLib::Utils::INetSerializable*>(message);
			_sessionManager->Send(message);
			//				 ^
			// TODO: Figure out crash here
			getLogger().debug("Sent message");
		//}
	}
	void PacketManager::SendUnreliable(LiteNetLib::Utils::INetSerializable* message) { 
		//if (_sessionManager) _sessionManager->SendUnreliable(message);
		//else {
			_sessionManager = UnityEngine::GameObject::FindObjectOfType<GlobalNamespace::MultiplayerSessionManager*>();
			_sessionManager->SendUnreliable(message);
		//}
	}
}