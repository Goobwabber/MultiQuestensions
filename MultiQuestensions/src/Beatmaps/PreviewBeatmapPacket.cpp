#include "Beatmaps/PreviewBeatmapPacket.hpp"
#include "custom-types/shared/register.hpp"

DEFINE_TYPE(MultiQuestensions::Beatmaps, PreviewBeatmapPacket);

namespace MultiQuestensions::Beatmaps {

	//void PreviewBeatmapPacket::New_ctor() {}
	//PreviewBeatmapPacket::PreviewBeatmapPacket() {}
	void PreviewBeatmapPacket::New() {}

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

		getLogger().debug("il2cpp_functions::Class_Init: %p"
			"il2cpp_functions::type_equals: %p"
			"il2cpp_functions::value_box: %p"
			"il2cpp_functions::object_unbox: %p",
			&il2cpp_functions::Class_Init,
			&il2cpp_functions::type_equals,
			&il2cpp_functions::value_box,
			&il2cpp_functions::object_unbox);

		levelId = reader->GetString();
		getLogger().debug("levelID: %s", to_utf8(csstrtostr(levelId)).c_str());
		levelHash = reader->GetString();
		getLogger().debug("levelHash: %s", to_utf8(csstrtostr(levelHash)).c_str());
		songName = reader->GetString();
		songSubName = reader->GetString();
		songAuthorName = reader->GetString();
		levelAuthorName = reader->GetString();
		getLogger().debug("songName: %s\n songSubName: %s\n songAuthorName: %s\n levelAuthorName: %s",
			to_utf8(csstrtostr(songName)).c_str(),
			to_utf8(csstrtostr(songSubName)).c_str(),
			to_utf8(csstrtostr(songAuthorName)).c_str(),
			to_utf8(csstrtostr(levelAuthorName)).c_str());
		beatsPerMinute = reader->GetFloat();
		songDuration = reader->GetFloat();

		characteristic = reader->GetString();
		difficulty = reader->GetUInt();
		getLogger().debug("Deserialize PreviewBeatmapPacket done");
	}
}
