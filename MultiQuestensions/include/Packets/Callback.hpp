#pragma once
#include "main.hpp"

#include "GlobalNamespace/ThreadStaticPacketPool_1.hpp"
#include "LiteNetLib/Utils/NetDataReader.hpp"

template <class T>
using PacketCallback = void (*)(T, GlobalNamespace::IConnectedPlayer*);

namespace MultiQuestensions {
	class CallbackBase {
	public:
		virtual ~CallbackBase() {};
		virtual void Invoke(LiteNetLib::Utils::NetDataReader* reader, int size, GlobalNamespace::IConnectedPlayer* player) = 0;
	};

	template <class TPacket> class CallbackWrapper : public CallbackBase {
	private:
		PacketCallback<TPacket> action;

	public:
		CallbackWrapper(PacketCallback<TPacket> callback) {
			action = callback;
		}

		void Invoke(LiteNetLib::Utils::NetDataReader* reader, int size, GlobalNamespace::IConnectedPlayer* player) {
			TPacket packet = GlobalNamespace::ThreadStaticPacketPool_1<TPacket>::get_pool()->Obtain();
			if (packet == nullptr) {
				reader->SkipBytes(size);
			}
			else {
				packet->Deserialize(reader);
			}
			action(packet, player);
		}
	};
}