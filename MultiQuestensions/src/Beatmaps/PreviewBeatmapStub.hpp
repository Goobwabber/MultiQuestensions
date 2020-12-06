#pragma once
#include "main.hpp"
#include "custom-types/shared/types.hpp"
#include "custom-types/shared/macros.hpp"

#include "PreviewBeatmapPacket.hpp"

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

DECLARE_CLASS_CODEGEN(MultiQuestensions::Beatmaps, PreviewBeatmapStub, GlobalNamespace::IPreviewBeatmapLevel,
	DECLARE_INSTANCE_FIELD(System::Threading::Tasks::Task_1<UnityEngine::Sprite*>*, _coverGetter);
	DECLARE_INSTANCE_FIELD(System::Threading::Tasks::Task_1<Array<uint8_t>*>*, _rawCoverGetter);
	DECLARE_INSTANCE_FIELD(System::Threading::Tasks::Task_1<UnityEngine::AudioClip*>*, _audioGetter);

	DECLARE_INSTANCE_FIELD(Il2CppString*, levelHash);
	DECLARE_INSTANCE_FIELD(Il2CppString*, levelKey);

	DECLARE_INSTANCE_FIELD(bool, isDownloaded);
	DECLARE_INSTANCE_FIELD(bool, isDownloadable);

	DECLARE_INSTANCE_FIELD(Il2CppString*, levelID);
	DECLARE_INSTANCE_FIELD(Il2CppString*, songName);
	DECLARE_INSTANCE_FIELD(Il2CppString*, songSubName);
	DECLARE_INSTANCE_FIELD(Il2CppString*, songAuthorName);
	DECLARE_INSTANCE_FIELD(Il2CppString*, levelAuthorName);
	DECLARE_INSTANCE_FIELD(float, beatsPerMinute);
	DECLARE_INSTANCE_FIELD(float, songDuration);

	DECLARE_CTOR(fromPreview, GlobalNamespace::IPreviewBeatmapLevel*);
	DECLARE_CTOR(fromPacket, MultiplayerExtensions::Beatmaps::PreviewBeatmapPacket*);
	
	DECLARE_METHOD(MultiplayerExtensions::Beatmaps::PreviewBeatmapPacket*, GetPacket, Il2CppString*, GlobalNamespace::BeatmapDifficulty);
	DECLARE_METHOD(System::Threading::Tasks::Task_1<Array<uint8_t>*>*, GetRawCoverAsync, System::Threading::CancellationToken);
	DECLARE_METHOD(System::Threading::Tasks::Task_1<UnityEngine::Sprite*>*, GetCoverImageAsync, System::Threading::CancellationToken);
	DECLARE_METHOD(System::Threading::Tasks::Task_1<UnityEngine::AudioClip*>*, GetPreviewAudioClipAsync, System::Threading::CancellationToken);

	REGISTER_FUNCTION(PreviewBeatmapStub,
		REGISTER_FIELD(levelHash);
		REGISTER_FIELD(levelKey);

		REGISTER_FIELD(isDownloaded);
		REGISTER_FIELD(isDownloadable);

		REGISTER_FIELD(levelID);
		REGISTER_FIELD(songName);
		REGISTER_FIELD(songSubName);
		REGISTER_FIELD(songAuthorName);
		REGISTER_FIELD(levelAuthorName);
		REGISTER_FIELD(beatsPerMinute);
		REGISTER_FIELD(songDuration);

		REGISTER_METHOD(fromPreview);
		REGISTER_METHOD(fromPacket);

		REGISTER_METHOD(GetPacket);
		REGISTER_METHOD(GetRawCoverImageAsync);
		REGISTER_METHOD(GetCoverImageAsync);
		REGISTER_METHOD(GetPreviewAudioClipAsync);
	)
)