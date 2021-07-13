#include "Beatmaps/PreviewBeatmapStub.hpp"
#include "beatsaber-hook/shared/utils/il2cpp-utils.hpp"
#include "System/Threading/Tasks/TaskStatus.hpp"
#include "UnityEngine/ImageConversion.hpp"

DEFINE_TYPE(MultiQuestensions::Beatmaps, PreviewBeatmapStub);

Il2CppString* LevelIdToHash(Il2CppString* levelId) {
	if (Il2CppString::IsNullOrWhiteSpace(levelId)) {
		return nullptr;
	}
	Array<Il2CppString*>* ary = levelId->Split('_', ' ');
	Il2CppString* hash = nullptr;
	if (ary->Length() > 2) {
		hash = ary->values[2];
	}
	return (hash != nullptr && hash->get_Length() == 40) ? hash : nullptr;
}

namespace MultiQuestensions::Beatmaps {
	void PreviewBeatmapStub::fromPreview(GlobalNamespace::IPreviewBeatmapLevel* preview) {
		getLogger().debug("PreviewBeatmapStub::fromPreview");
		levelID = preview->get_levelID();
		levelHash = LevelIdToHash(levelID);
		isDownloaded = true;
		
		getLogger().debug("levelID: %s, levelHash: %s", to_utf8(csstrtostr(levelID)).c_str(), to_utf8(csstrtostr(levelHash)).c_str());
		songName = preview->get_songName();
		songSubName = preview->get_songSubName();
		songAuthorName = preview->get_songAuthorName();
		levelAuthorName = preview->get_levelAuthorName();
		getLogger().debug("songName: %s, songSubName: %s, songAuthorName: %s, levelAuthorName: %s", to_utf8(csstrtostr(songName)).c_str(), to_utf8(csstrtostr(songSubName)).c_str(), to_utf8(csstrtostr(songAuthorName)).c_str(), to_utf8(csstrtostr(levelAuthorName)).c_str());
		
		beatsPerMinute = preview->get_beatsPerMinute();
		songDuration = preview->get_songDuration();
		getLogger().debug("beatsPerMinute: %f, songDuration: %f", beatsPerMinute, songDuration);
		
		// Need this declared before getter to prevent early freeing
		static Unity::Collections::NativeArray_1<uint8_t> rawCover;

		//_audioGetter = preview->GetPreviewAudioClipAsync(System::Threading::CancellationToken::get_None());
		//_coverGetter = preview->GetCoverImageAsync(System::Threading::CancellationToken::get_None());
		//UnityEngine::Sprite* coverSprite = _coverGetter->get_ResultOnSuccess();
		//UnityEngine::Texture2D* coverTexture_temp = coverSprite->get_texture();
		//coverBytes = UnityEngine::ImageConversion::EncodeToPNG(coverTexture_temp);
		//rawCover = coverTexture_temp->GetRawTextureData<uint8_t>();
		//coverBytes = reinterpret_cast<Array<uint8_t>*>(&rawCover);
		//_rawCoverGetter = _coverGetter->ContinueWith<Array<uint8_t>*>(il2cpp_utils::MakeFunc<System::Func_2<System::Threading::Tasks::Task_1<UnityEngine::Sprite*>*, Array<uint8_t>*>*>(
		//	*[](System::Threading::Tasks::Task_1<UnityEngine::Sprite*>* spriteTask)->Array<uint8_t>*{
		//		UnityEngine::Sprite* sprite = spriteTask->get_Result();
		//		rawCover = sprite->get_texture()->GetRawTextureData<uint8_t>();
		//		return reinterpret_cast<Array<uint8_t>*>(&rawCover);
		//	}
		//));
	}

	void PreviewBeatmapStub::fromPacket(MultiplayerExtensions::Beatmaps::PreviewBeatmapPacket* packet) {
		getLogger().debug("PreviewBeatmapStub::fromPacket");
		levelID = packet->levelId;
		levelHash = LevelIdToHash(levelID);
		isDownloaded = false;

		songName = packet->songName;
		songSubName = packet->songSubName;
		songAuthorName = packet->songAuthorName;
		levelAuthorName = packet->levelAuthorName;

		beatsPerMinute = packet->beatsPerMinute;
		songDuration = packet->songDuration;

		getLogger().debug("PreviewBeatmapStub::fromPacket: _rawCoverGetter");
		_rawCoverGetter = System::Threading::Tasks::Task_1<Array<uint8_t>*>::New_ctor(packet->coverImage);
		if (_rawCoverGetter->get_Status() == 5) {
			coverBytes = _rawCoverGetter->get_Result();
			_rawCoverGetter->Dispose();
			UnityEngine::Texture2D* texture;
			if (coverBytes == nullptr || coverBytes->Length() == 0) {
				texture = UnityEngine::Texture2D::get_whiteTexture();
			}
			else {
				UnityEngine::ImageConversion::LoadImage(texture, coverBytes, false);
			}
			coverImage = UnityEngine::Sprite::Create(texture, UnityEngine::Rect(0, 0, 2, 2), UnityEngine::Vector2(0, 0), 100, 0, UnityEngine::SpriteMeshType::FullRect, UnityEngine::Vector4(2, 2, 2, 2), false);
		}

		//_coverGetter = _rawCoverGetter->ContinueWith<UnityEngine::Sprite*>(il2cpp_utils::MakeFunc<System::Func_2<System::Threading::Tasks::Task_1<Array<uint8_t>*>*, UnityEngine::Sprite*>*>(
		//	*[](System::Threading::Tasks::Task_1<Array<uint8_t>*>* rawCoverTask)->UnityEngine::Sprite* {
		//		Array<uint8_t>* rawCover = rawCoverTask->get_Result();
		//		UnityEngine::Texture2D* texture;
		//		if (rawCover == nullptr || rawCover->Length() == 0) {
		//			texture = UnityEngine::Texture2D::get_whiteTexture();
		//		}
		//		else {
		//			// in the future make this actually load bytes
		//			texture = UnityEngine::Texture2D::get_whiteTexture();
		//		}
		//		return UnityEngine::Sprite::Create(texture, UnityEngine::Rect(0, 0, 2, 2), UnityEngine::Vector2(0, 0), 100, 0, UnityEngine::SpriteMeshType::_get_FullRect(), UnityEngine::Vector4(2, 2, 2, 2), false);
		//	}
		//));
	}

	MultiplayerExtensions::Beatmaps::PreviewBeatmapPacket* PreviewBeatmapStub::GetPacket(Il2CppString* characteristic, GlobalNamespace::BeatmapDifficulty difficulty) {
		//MultiplayerExtensions::Beatmaps::PreviewBeatmapPacket* packet = (MultiplayerExtensions::Beatmaps::PreviewBeatmapPacket*)*il2cpp_utils::New(MultiplayerExtensions::Beatmaps::PreviewBeatmapPacket());
		getLogger().debug("Start PreviewBeatmapStub::GetPacket");
		MultiplayerExtensions::Beatmaps::PreviewBeatmapPacket* packet = THROW_UNLESS(il2cpp_utils::New<MultiplayerExtensions::Beatmaps::PreviewBeatmapPacket*>());
		getLogger().debug("New PreviewBeatmapPacket*");

		packet->levelId = levelID;

		packet->songName = songName;
		packet->songSubName = songSubName;
		packet->songAuthorName = songAuthorName;
		packet->levelAuthorName = levelAuthorName;

		packet->beatsPerMinute = beatsPerMinute;

		getLogger().debug("PreviewBeatmapPacket Getting Cover");

		if (_rawCoverGetter != nullptr && (int)_rawCoverGetter->get_Status() == 5) {
			//_rawCoverGetter->Wait();
			getLogger().debug("%d", (int)_rawCoverGetter->get_Status());
			packet->coverImage = _rawCoverGetter->get_ResultOnSuccess();
			//if (coverBytes != nullptr) packet->coverImage = coverBytes;
		}
		else if (coverBytes != nullptr) {
			getLogger().debug("Use coverBytes");
			packet->coverImage = coverBytes;
		}
		else getLogger().error("Could not get coverImage");

		packet->characteristic = characteristic;
		packet->difficulty = difficulty;

		getLogger().debug("BeatmapData placed");

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

#pragma region All the pain in form of getter functions

	Il2CppString* PreviewBeatmapStub::get_levelID() {
		return levelID;
	}

	Il2CppString* PreviewBeatmapStub::get_songName() {
		return songName;
	}

	Il2CppString* PreviewBeatmapStub::get_songSubName() {
		return songSubName;
	}

	Il2CppString* PreviewBeatmapStub::get_songAuthorName() {
		return songAuthorName;
	}

	Il2CppString* PreviewBeatmapStub::get_levelAuthorName() {
		return levelAuthorName;
	}

	float PreviewBeatmapStub::get_beatsPerMinute() {
		return beatsPerMinute;
	}

	float PreviewBeatmapStub::get_songDuration() {
		return songDuration;
	}

	float PreviewBeatmapStub::get_songTimeOffset() {
		return songTimeOffset;
	}

	float PreviewBeatmapStub::get_previewDuration() {
		return previewDuration;
	}

	float PreviewBeatmapStub::get_previewStartTime() {
		return previewStartTime;
	}

	float PreviewBeatmapStub::get_shuffle() {
		return shuffle;
	}

	float PreviewBeatmapStub::get_shufflePeriod() {
		return shufflePeriod;
	}

	GlobalNamespace::EnvironmentInfoSO* PreviewBeatmapStub::get_allDirectionsEnvironmentInfo() {
		return allDirectionsEnvironmentInfo;
	}

	GlobalNamespace::EnvironmentInfoSO* PreviewBeatmapStub::get_environmentInfo() {
		return environmentInfo;
	}

	Array<GlobalNamespace::PreviewDifficultyBeatmapSet*>* PreviewBeatmapStub::get_previewDifficultyBeatmapSets() {
		return previewDifficultyBeatmapSets;
	}

#pragma endregion

}