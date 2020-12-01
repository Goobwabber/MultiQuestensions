#include "beatmaps.hpp"



Il2CppString* LevelIdToHash(Il2CppString* levelId) {
	if (Il2CppString::IsNullOrWhiteSpace(levelId)) {
		return nullptr;
	}
	Array<Il2CppString*>* ary = levelId->Split('_', ' ');
	Il2CppString* hash = nullptr;
	if (ary->Length() > 2) {
		hash = ary->values[2];
	}
	if (hash != nullptr && hash->get_Length() == 40) {
		return hash;
	}
	return nullptr;
}



// PreviewBeatmapPacket
std::string PreviewBeatmapPacket::get_identifier() {
	return "MultiplayerExtensions.Beatmaps.PreviewBeatmapPacket";
}

void PreviewBeatmapPacket::Serialize(LiteNetLib::Utils::NetDataWriter* writer) {
	writer->Put(levelId);
	writer->Put(levelKey);
	writer->Put(songName);
	writer->Put(songSubName);
	writer->Put(songAuthorName);
	writer->Put(levelAuthorName);
	writer->Put(beatsPerMinute);
	writer->Put(songDuration);

	writer->Put(isDownloadable);

	writer->Put(characteristic);
	writer->Put(difficulty.value);

	if (coverImage != nullptr) {
		writer->PutBytesWithLength(coverImage);
	}
	else {
		Array<uint8_t> emptyByteArray = Array<uint8_t>();
		writer->PutBytesWithLength(&emptyByteArray);
	}
}

void PreviewBeatmapPacket::Deserialize(LiteNetLib::Utils::NetDataReader* reader) {
	levelId = reader->GetString();
	levelKey = reader->GetString();
	songName = reader->GetString();
	songSubName = reader->GetString();
	songAuthorName = reader->GetString();
	levelAuthorName = reader->GetString();
	beatsPerMinute = reader->GetFloat();
	songDuration = reader->GetFloat();

	isDownloadable = reader->GetBool();

	characteristic = reader->GetString();
	difficulty = GlobalNamespace::BeatmapDifficulty(reader->GetUInt());

	coverImage = reader->GetBytesWithLength();
}

void PreviewBeatmapPacket::Release() {
	GlobalNamespace::ThreadStaticPacketPool_1<PreviewBeatmapPacket*>::get_pool()->Release(this);
}



// PreviewBeatmapStub
PreviewBeatmapStub::PreviewBeatmapStub(GlobalNamespace::IPreviewBeatmapLevel* preview) {
	levelID = preview->get_levelID();
	levelHash = LevelIdToHash(levelID);
	isDownloaded = true;

	songName = preview->get_songName();
	songSubName = preview->get_songSubName();
	songAuthorName = preview->get_songAuthorName();
	levelAuthorName = preview->get_levelAuthorName();

	beatsPerMinute = preview->get_beatsPerMinute();
	songDuration = preview->get_songDuration();

	_audioGetter = preview->GetPreviewAudioClipAsync(System::Threading::CancellationToken::get_None());
	_coverGetter = preview->GetCoverImageAsync(System::Threading::CancellationToken::get_None());
	_rawCoverGetter = _coverGetter->ContinueWith<Array<uint8_t>*>(il2cpp_utils::MakeFunc<System::Func_2<System::Threading::Tasks::Task_1<UnityEngine::Sprite*>*, Array<uint8_t>*>*>(
		*[](System::Threading::Tasks::Task_1<UnityEngine::Sprite*>* spriteTask)->Array<uint8_t>*{
			UnityEngine::Sprite* sprite = spriteTask->get_Result();
			Unity::Collections::NativeArray_1<uint8_t> rawCover = sprite->get_texture()->GetRawTextureData<uint8_t>();
			return reinterpret_cast<Array<uint8_t>*>(&rawCover);
		}
	));
}

PreviewBeatmapStub::PreviewBeatmapStub(PreviewBeatmapPacket* packet) {
	levelID = packet->levelId;
	levelHash = LevelIdToHash(levelID);
	levelKey = packet->levelKey;
	isDownloaded = false;

	songName = packet->songName;
	songSubName = packet->songSubName;
	songAuthorName = packet->songAuthorName;
	levelAuthorName = packet->levelAuthorName;

	beatsPerMinute = packet->beatsPerMinute;
	songDuration = packet->songDuration;

	isDownloadable = packet->isDownloadable;

	_rawCoverGetter = System::Threading::Tasks::Task_1<Array<uint8_t>*>::New_ctor(packet->coverImage);
	_coverGetter = _rawCoverGetter->ContinueWith<UnityEngine::Sprite*>(il2cpp_utils::MakeFunc<System::Func_2<System::Threading::Tasks::Task_1<Array<uint8_t>*>*, UnityEngine::Sprite*>*>(
		*[](System::Threading::Tasks::Task_1<Array<uint8_t>*>* rawCoverTask)->UnityEngine::Sprite* {
		Array<uint8_t>* rawCover = rawCoverTask->get_Result();
			UnityEngine::Texture2D* texture;
			if (rawCover == nullptr || rawCover->Length() == 0) {
				texture = UnityEngine::Texture2D::get_whiteTexture();
			}
			else {
				// in the future make this actually load bytes
				texture = UnityEngine::Texture2D::get_whiteTexture();
			}
			return UnityEngine::Sprite::Create(texture, UnityEngine::Rect(0, 0, 2, 2), UnityEngine::Vector2(0, 0), 100, 0, UnityEngine::SpriteMeshType::_get_FullRect(), UnityEngine::Vector4(2, 2, 2, 2), false);
		}
	));
}

PreviewBeatmapPacket* PreviewBeatmapStub::GetPacket(Il2CppString* characteristic, GlobalNamespace::BeatmapDifficulty difficulty) {
	PreviewBeatmapPacket* packet = new PreviewBeatmapPacket();

	packet->levelId = levelID;
	packet->levelKey = levelKey;

	packet->songName = songName;
	packet->songSubName = songSubName;
	packet->songAuthorName = songAuthorName;
	packet->levelAuthorName = levelAuthorName;

	packet->beatsPerMinute = beatsPerMinute;
	packet->isDownloadable = isDownloadable;

	_rawCoverGetter->Wait();
	packet->coverImage = _rawCoverGetter->get_Result();

	packet->characteristic = characteristic;
	packet->difficulty = difficulty;

	return packet;
}

System::Threading::Tasks::Task_1<Array<uint8_t>*>* PreviewBeatmapStub::GetRawCoverAsync(System::Threading::CancellationToken cancellationToken) {
	return _rawCoverGetter;
}

System::Threading::Tasks::Task_1<UnityEngine::Sprite*>* PreviewBeatmapStub::GetCoverImageAsync(System::Threading::CancellationToken cancellationToken) {
	return _coverGetter;
}

System::Threading::Tasks::Task_1<UnityEngine::AudioClip*>* PreviewBeatmapStub::GetPreviewAudioClipAsync(System::Threading::CancellationToken cancellationToken) {
	return _audioGetter;
}