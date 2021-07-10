#pragma once
#include "main.hpp"
#include "Callback.hpp"
#include "PacketSerializer.hpp"

#include "future"
#include "GlobalNamespace/MultiplayerSessionManager_MessageType.hpp"
#include "GlobalNamespace/PacketPool_1.hpp"

namespace MultiQuestensions {
	class PacketManager {
	private:
		GlobalNamespace::MultiplayerSessionManager* _sessionManager;
		PacketSerializer* packetSerializer;

	public:
		PacketManager(GlobalNamespace::MultiplayerSessionManager* sessionManager);
		void Send(LiteNetLib::Utils::INetSerializable* message);
		void SendUnreliable(LiteNetLib::Utils::INetSerializable* message);

		template <class TPacket>
		void RegisterCallback(PacketCallback<TPacket> callback) {
			Il2CppReflectionType* packetType = csTypeOf(TPacket);
			if (packetType == nullptr) {
				getLogger().info("Packet Type null.");
				return;
			}

			Il2CppString* identifier = packetType->ToString()->Replace(il2cpp_utils::newcsstr((std::string)"::"), il2cpp_utils::newcsstr((std::string)"."));
			getLogger().info("%s", to_utf8(csstrtostr(identifier)).data());

			CallbackWrapper<TPacket> newCallback = CallbackWrapper<TPacket>(callback);

			if (identifier == nullptr) {
				getLogger().error("Cannot register callback: Identifier null.");
				return;
			} else {
				packetSerializer->RegisterCallback(identifier, &newCallback);
			}
		}
	};
}