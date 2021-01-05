#pragma once
#include "main.hpp"
#include "PacketSerializer.hpp"

#include "GlobalNamespace/MultiplayerSessionManager_MessageType.hpp"
#include "GlobalNamespace/ThreadStaticPacketPool_1.hpp"
#include "GlobalNamespace/PacketPool_1.hpp"

template <class T>
using PacketCallback = void (*)(T, GlobalNamespace::IConnectedPlayer*);

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
			Il2CppReflectionType* packetType = typeof(TPacket);
			if (packetType == nullptr) {
				getLogger().error("Packet Type null.");
				return;
			}
			Il2CppString* identifier = packetType->ToString();

			auto* newCallback = il2cpp_utils::MakeDelegate<CallbackAction*>(classof(CallbackAction*), &callback, *[](PacketCallback<TPacket> context, LiteNetLib::Utils::NetDataReader* reader, int size, GlobalNamespace::IConnectedPlayer* player) {
				TPacket packet = GlobalNamespace::ThreadStaticPacketPool_1<TPacket>::get_pool()->Obtain();
				if (packet == nullptr) {
					reader->SkipBytes(size);
				}
				else {
					packet->Deserialize(reader);
				}

				//(*callback)(packet, player);
			});

			packetSerializer->RegisterCallback(identifier, newCallback);
		}
	};
}