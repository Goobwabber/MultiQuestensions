#include "Beatmaps/PreviewBeatmapPacket.hpp"
#include "custom-types/shared/register.hpp"

DEFINE_TYPE(MultiplayerExtensions::Beatmaps, PreviewBeatmapPacket);

namespace MultiplayerExtensions::Beatmaps {

	void PreviewBeatmapPacket::Release() {
		getLogger().debug("Running Release");
		GlobalNamespace::ThreadStaticPacketPool_1<PreviewBeatmapPacket*>::get_pool()->Release(this);
	}
	void PreviewBeatmapPacket::Serialize(LiteNetLib::Utils::NetDataWriter* writer) {
		getLogger().debug("Starting Serialize");
		writer->Put(levelId);
		writer->Put(songName);
		writer->Put(songSubName);
		writer->Put(songAuthorName);
		writer->Put(levelAuthorName);
		writer->Put(beatsPerMinute);
		writer->Put(songDuration);

		writer->Put(characteristic);
		writer->Put(difficulty);
		getLogger().debug("Serialize CoverImage");

		if (coverImage == nullptr) {
			Array<uint8_t> emptyByteArray = Array<uint8_t>();
			writer->PutBytesWithLength(&emptyByteArray);
		}
		else {
			writer->PutBytesWithLength(coverImage);
		}
	}

	void PreviewBeatmapPacket::Deserialize(LiteNetLib::Utils::NetDataReader* reader) {
		getLogger().debug("Starting Deserialize");
		levelId = reader->GetString();
		songName = reader->GetString();
		songSubName = reader->GetString();
		songAuthorName = reader->GetString();
		levelAuthorName = reader->GetString();
		beatsPerMinute = reader->GetFloat();
		songDuration = reader->GetFloat();

		characteristic = reader->GetString();
		difficulty = reader->GetUInt();
		getLogger().debug("Deserialize CoverImage");

		if (reader->GetBytesWithLength() == nullptr) {
			Array<uint8_t> emptyByteArray = Array<uint8_t>();
			coverImage = &emptyByteArray;
		} else {
			coverImage = reader->GetBytesWithLength();
		}
	}
}
//void PreviewBeatmapPacket::Install() {
//	//custom_types::Register::ExplicitRegister({ &__registration_instance_PreviewBeatmapPacket });
//}
