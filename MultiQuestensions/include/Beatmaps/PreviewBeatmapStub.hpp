#pragma once
#include "main.hpp"
#include "custom-types/shared/types.hpp"
#include "custom-types/shared/macros.hpp"

#include "PreviewBeatmapPacket.hpp"
#include "System/Threading/CancellationToken.hpp"
#include "System/Threading/Tasks/Task_1.hpp"
#include "System/Func_2.hpp"
#include "UnityEngine/Sprite.hpp"
#include "UnityEngine/AudioClip.hpp"
#include "UnityEngine/Texture2D.hpp"
#include "UnityEngine/Rect.hpp"
#include "UnityEngine/Vector2.hpp"
#include "UnityEngine/Vector4.hpp"
#include "UnityEngine/SpriteMeshType.hpp"

Il2CppString* LevelIdToHash(Il2CppString*);

DECLARE_CLASS_INTERFACES(MultiQuestensions::Beatmaps, PreviewBeatmapStub, "System", "Object", sizeof(Il2CppObject),
	classof(GlobalNamespace::IPreviewBeatmapLevel*),
	DECLARE_INSTANCE_FIELD(System::Threading::Tasks::Task_1<UnityEngine::Sprite*>*, _coverGetter);
	DECLARE_INSTANCE_FIELD(System::Threading::Tasks::Task_1<Array<uint8_t>*>*, _rawCoverGetter);
	DECLARE_INSTANCE_FIELD(System::Threading::Tasks::Task_1<UnityEngine::AudioClip*>*, _audioGetter);

	DECLARE_INSTANCE_FIELD(Il2CppString*, levelHash);

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
		REGISTER_METHOD(GetRawCoverAsync);
		REGISTER_METHOD(GetCoverImageAsync);
		REGISTER_METHOD(GetPreviewAudioClipAsync);
	)
)