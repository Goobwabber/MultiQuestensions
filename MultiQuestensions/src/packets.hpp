#pragma once
#include "main.hpp"

class CustomPacket : public LiteNetLib::Utils::INetSerializable, public GlobalNamespace::IPoolablePacket {
private:
	virtual std::string get_identifier() = 0;

public:
	Il2CppString* GetIdentifier();
	virtual void Serialize(LiteNetLib::Utils::NetDataWriter* writer) = 0;
	virtual void Deserialize(LiteNetLib::Utils::NetDataReader* reader) = 0;
	virtual void Release() = 0;
};

class PacketSerializer : public GlobalNamespace::INetworkPacketSubSerializer_1<GlobalNamespace::IConnectedPlayer*> {
private:
	std::map<Il2CppString*, std::function<void(LiteNetLib::Utils::NetDataReader*, int, GlobalNamespace::IConnectedPlayer*)>> packetHandlers = {};
	std::set<Il2CppString*> registeredTypes = {};

public:
	void Serialize(LiteNetLib::Utils::NetDataWriter* writer, LiteNetLib::Utils::INetSerializable* packet);
	void Deserialize(LiteNetLib::Utils::NetDataReader* reader, int length, GlobalNamespace::IConnectedPlayer* data);
	bool HandlesType(System::Type* type);

	template<class TPacket> 
	void RegisterCallback(void (*callback)(TPacket, GlobalNamespace::IConnectedPlayer*)) {
		Il2CppString* identifier = TPacket()->GetIdentifier();
		registeredTypes.insert(identifier);

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

		packetHandlers[identifier] = [&](LiteNetLib::Utils::NetDataReader* reader, int size, GlobalNamespace::IConnectedPlayer* player) {
			(*callback)(deserealize(reader, size), player);
		};
	}
};

class PacketManager {
private:
	GlobalNamespace::MultiplayerSessionManager* _sessionManager;
	PacketSerializer packetSerializer = PacketSerializer();

public:
	PacketManager(GlobalNamespace::MultiplayerSessionManager* sessionManager);
	void Send(LiteNetLib::Utils::INetSerializable* message);
	void SendUnreliable(LiteNetLib::Utils::INetSerializable* message);

	template <class TPacket> 
	void RegisterCallback(void (*callback)(TPacket, GlobalNamespace::IConnectedPlayer*)) {
		packetSerializer.RegisterCallback((*callback));
	}
};