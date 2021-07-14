#include "Beatmaps/PreviewBeatmapPacket.hpp"
#include "custom-types/shared/register.hpp"

DEFINE_TYPE(MultiplayerExtensions::Beatmaps, PreviewBeatmapPacket);

namespace MultiplayerExtensions::Beatmaps {

	void PreviewBeatmapPacket::New_ctor() {}

	void PreviewBeatmapPacket::Release() {
		getLogger().debug("PreviewBeatmapPacket::Release");
		GlobalNamespace::ThreadStaticPacketPool_1<PreviewBeatmapPacket*>::get_pool()->Release(this);
	}

	void PreviewBeatmapPacket::Serialize(LiteNetLib::Utils::NetDataWriter* writer) {
		getLogger().debug("PreviewBeatmapPacket::Serialize");

		writer->Put(levelId);
		writer->Put(levelHash);
		writer->Put(songName);
		writer->Put(songSubName);
		writer->Put(songAuthorName);
		writer->Put(levelAuthorName);
		writer->Put(beatsPerMinute);
		writer->Put(songDuration);

		writer->Put(characteristic);
		writer->Put(difficulty);
	}

	void PreviewBeatmapPacket::Deserialize(LiteNetLib::Utils::NetDataReader* reader) {
		getLogger().debug("PreviewBeatmapPacket::Deserialize");

		levelId = reader->GetString();
		levelHash = reader->GetString();
		songName = reader->GetString();
		songSubName = reader->GetString();
		songAuthorName = reader->GetString();
		levelAuthorName = reader->GetString();
		beatsPerMinute = reader->GetFloat();
		songDuration = reader->GetFloat();

		characteristic = reader->GetString();
		difficulty = reader->GetUInt();
	}
}
