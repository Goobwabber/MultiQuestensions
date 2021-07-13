#pragma once
#include "main.hpp"
#include "Callback.hpp"
#include "PacketSerializer.hpp"

#include <future>
#include <algorithm>
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

			std::string identifier = to_utf8(csstrtostr(packetType->ToString()));
			getLogger().info("Before replace %s", identifier.c_str());
			int pos = 0;
			while ((pos = identifier.find("::")) != std::string::npos)
			{
				identifier.replace(pos, 2, ".");
				pos++;
			}
			//Il2CppString* identifier_old = packetType->ToString()->Replace(il2cpp_utils::newcsstr("::"), il2cpp_utils::newcsstr("."));
			getLogger().info("After replace %s", identifier.c_str());

			CallbackWrapper<TPacket> newCallback = CallbackWrapper<TPacket>(callback);

			if (identifier.empty()) {
				getLogger().error("Cannot register callback: Identifier null.");
				return;
			} else {
				packetSerializer->RegisterCallback(identifier, &newCallback);
			}
		}
	};
}