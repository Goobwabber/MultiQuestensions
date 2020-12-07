#include "PacketManager.hpp"

namespace MultiQuestensions {
	PacketManager::PacketManager(GlobalNamespace::MultiplayerSessionManager* sessionManager) {
		_sessionManager = sessionManager;
		packetSerializer = new PacketSerializer();
		_sessionManager->RegisterSerializer(GlobalNamespace::MultiplayerSessionManager_MessageType(100), packetSerializer);
	}

	void PacketManager::Send(Il2CppObject* message) { _sessionManager->Send(message); }
	void PacketManager::SendUnreliable(Il2CppObject* message) { _sessionManager->SendUnreliable(message); }

	template <class TPacket>
	void PacketManager::RegisterCallback(void (*callback)(TPacket, GlobalNamespace::IConnectedPlayer*)) {
		System::Type* packetType = typeof(TPacket);
		Il2CppString* identifier = packetType->ToString();

		auto deserealize = [](LiteNetLib::Utils::NetDataReader* reader, int size) -> TPacket {
			TPacket packet = GlobalNamespace::ThreadStaticPacketPool_1<TPacket>::get_pool()->Obtain();
			if (packet == nullptr) {
				reader->SkipBytes(size);
			}
			else {
				packet->Deserialize(reader);
			}
			return packet;
		};

		CallbackAction* newCallback = il2cpp_utils::MakeAction<CallbackAction*>(*[&](LiteNetLib::Utils::NetDataReader* reader, int size, GlobalNamespace::IConnectedPlayer* player) {
			(*callback)(deserealize(reader, size), player);
		});

		packetSerializer->RegisterCallback(identifier, newCallback);
	}
}