#pragma once
#include "main.hpp"
#include "PacketSerializer.hpp"

template <class T>
using PacketCallback = void (*)(T, GlobalNamespace::IConnectedPlayer*);

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
		void RegisterCallback(PacketCallback<TPacket> callback) {
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

			CallbackAction* newCallback = il2cpp_utils::MakeAction<CallbackAction*>(*[](LiteNetLib::Utils::NetDataReader* reader, int size, GlobalNamespace::IConnectedPlayer* player) {
				//(*callback)(deserealize(reader, size), player);
			});

			packetSerializer->RegisterCallback(identifier, newCallback);
		}
	};
}