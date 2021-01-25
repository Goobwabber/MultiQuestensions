#include "Beatmaps/PreviewBeatmapPacket.hpp"

DEFINE_CLASS(MultiplayerExtensions::Beatmaps::PreviewBeatmapPacket);

namespace MultiplayerExtensions::Beatmaps {
	void PreviewBeatmapPacket::Release() {
		GlobalNamespace::ThreadStaticPacketPool_1<PreviewBeatmapPacket*>::get_pool()->Release(this);
	}
	void PreviewBeatmapPacket::Serialize(LiteNetLib::Utils::NetDataWriter* writer) {
		writer->Put(levelId);
		writer->Put(songName);
		writer->Put(songSubName);
		writer->Put(songAuthorName);
		writer->Put(levelAuthorName);
		writer->Put(beatsPerMinute);
		writer->Put(songDuration);

		writer->Put(characteristic);
		writer->Put(difficulty);

		if (coverImage == nullptr) {
			Array<uint8_t> emptyByteArray = Array<uint8_t>();
			writer->PutBytesWithLength(&emptyByteArray);
		}
		else {
			writer->PutBytesWithLength(coverImage);
		}
	}

	void PreviewBeatmapPacket::Deserialize(LiteNetLib::Utils::NetDataReader* reader) {
		levelId = reader->GetString();
		songName = reader->GetString();
		songSubName = reader->GetString();
		songAuthorName = reader->GetString();
		levelAuthorName = reader->GetString();
		beatsPerMinute = reader->GetFloat();
		songDuration = reader->GetFloat();

		characteristic = reader->GetString();
		difficulty = reader->GetUInt();

		if (reader->GetBytesWithLength() == nullptr) {
			Array<uint8_t> emptyByteArray = Array<uint8_t>();
			coverImage = &emptyByteArray;
		} else {
			coverImage = reader->GetBytesWithLength();
		}
	}
}