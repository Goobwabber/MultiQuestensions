#include "main.hpp"



class CustomPacket : public LiteNetLib::Utils::INetSerializable, public GlobalNamespace::IPoolablePacket {
private:
	virtual std::string get_identifier() = 0;

public:
	Il2CppString* GetIdentifier() {
		std::string id = get_identifier();
		return createcsstr(id, StringType::Temporary);
	}

	virtual void Serialize(LiteNetLib::Utils::NetDataWriter*) = 0;
	virtual void Deserialize(LiteNetLib::Utils::NetDataReader*) = 0;
	virtual void Release() = 0;
};



class PacketSerializer : public GlobalNamespace::INetworkPacketSubSerializer_1<GlobalNamespace::IConnectedPlayer*> {
private:
	std::map<Il2CppString*, std::function<void(LiteNetLib::Utils::NetDataReader*, int, GlobalNamespace::IConnectedPlayer*)>> packetHandlers = {};
	std::set<Il2CppString*> registeredTypes = {};

public:
	void Serialize(LiteNetLib::Utils::NetDataWriter* writer, LiteNetLib::Utils::INetSerializable* packet) {
		CustomPacket* cPacket = down_cast<CustomPacket, LiteNetLib::Utils::INetSerializable>(packet);
		writer->Put(cPacket->GetIdentifier());
		cPacket->Serialize(writer);
	}

	void Deserialize(LiteNetLib::Utils::NetDataReader* reader, int length, GlobalNamespace::IConnectedPlayer* data) {
		Il2CppString* packetType = reader->GetString();
		if (packetHandlers.find(packetType) != packetHandlers.end()) {
			packetHandlers[packetType](reader, length, data);
		}
	}
	
	bool HandlesType(System::Type* type) {
		return registeredTypes.find(type->ToString()) != registeredTypes.end();
	}

	template<class TPacket>
	void RegisterCallback(std::function<void(TPacket, GlobalNamespace::IConnectedPlayer*)> callback) {
		Il2CppString* identifier = TPacket().GetIdentifier();
		registeredTypes.insert(identifier);

		auto deserealize = [](LiteNetLib::Utils::NetDataReader* reader, int size) {
			TPacket* packet = GlobalNamespace::ThreadStaticPacketPool_1<TPacket>::get_pool()->Obtain();
			if (packet == nullptr) {
				reader->SkipBytes(size);
			}
			else {
				packet->Deserealize(reader);
			}
			return packet;
		};

		packetHandlers[identifier] = [&](LiteNetLib::Utils::NetDataReader* reader, int size, GlobalNamespace::IConnectedPlayer* player) {
			callback(deserealize(reader, size), player)
		}
	}
};



class PacketManager {
private:
	GlobalNamespace::MultiplayerSessionManager* _sessionManager;
	PacketSerializer packetSerializer = PacketSerializer();

public:
	PacketManager(GlobalNamespace::MultiplayerSessionManager* sessionManager) {
		_sessionManager = sessionManager;

		_sessionManager->RegisterSerializer(GlobalNamespace::MultiplayerSessionManager_MessageType(100), &packetSerializer);
	}

	template <class T>
	void Send(T message) {
		_sessionManager->Send(message);
	}

	template <class T>
	void SendUnreliable(T message) {
		_sessionManager->Send(message);
	}

	template <class TPacket>
	void RegisterCallback(std::function<void(TPacket, GlobalNamespace::IConnectedPlayer*)> callback) {
		packetSerializer.RegisterCallback(callback);
	}
};