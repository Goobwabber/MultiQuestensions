#include "Beatmaps/PreviewBeatmapPacket.hpp"
#include "custom-types/shared/register.hpp"

DEFINE_TYPE(MultiplayerExtensions::Beatmaps, PreviewBeatmapPacket);

namespace MultiplayerExtensions::Beatmaps {

	void PreviewBeatmapPacket::New_ctor() {}

	void PreviewBeatmapPacket::FromPreview(MultiQuestensions::Beatmaps::PreviewBeatmapStub* preview, Il2CppString* characteristic, GlobalNamespace::BeatmapDifficulty difficulty) {
		getLogger().debug("PreviewBeatmapPacket::FromPreview");

		levelId = preview->levelID;
		levelHash = preview->levelHash;
		songName = preview->songName;
		songSubName = preview->songSubName;
		songAuthorName = preview->songAuthorName;
		levelAuthorName = preview->levelAuthorName;
		beatsPerMinute = preview->beatsPerMinute;
		songDuration = preview->songDuration;

		characteristic = characteristic;
		difficulty = difficulty;
	}

	void PreviewBeatmapPacket::Release() {
		getLogger().debug("Running Release");
		GlobalNamespace::ThreadStaticPacketPool_1<PreviewBeatmapPacket*>::get_pool()->Release(this);
	}

	void PreviewBeatmapPacket::Serialize(LiteNetLib::Utils::NetDataWriter* writer) {
		getLogger().debug("Starting Serialize");

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
		getLogger().debug("Starting Deserialize");

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
