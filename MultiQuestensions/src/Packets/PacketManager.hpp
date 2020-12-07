#pragma once
#include "main.hpp"
#include "PacketSerializer.hpp"

namespace MultiQuestensions {
	class PacketManager {
	private:
		GlobalNamespace::MultiplayerSessionManager* _sessionManager;
		PacketSerializer* packetSerializer;

	public:
		PacketManager(GlobalNamespace::MultiplayerSessionManager* sessionManager);
		void Send(Il2CppObject* message);
		void SendUnreliable(Il2CppObject* message);

		template <class TPacket>
		void RegisterCallback(void (*callback)(TPacket, GlobalNamespace::IConnectedPlayer*));
	};
}