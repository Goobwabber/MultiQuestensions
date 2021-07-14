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

#include "GlobalNamespace/EnvironmentInfoSO.hpp"
#include "GlobalNamespace/PreviewDifficultyBeatmapSet.hpp"

namespace MultiQuestensions::Beatmaps {
	enum class DownloadableState { True, False, Unchecked };
}

DECLARE_CLASS_INTERFACES(MultiQuestensions::Beatmaps, PreviewBeatmapStub, "System", "Object", sizeof(Il2CppObject),
	{ classof(GlobalNamespace::IPreviewBeatmapLevel*) },

	DECLARE_CTOR(FromPreview, Il2CppString* levelHash, GlobalNamespace::IPreviewBeatmapLevel*);
	DECLARE_CTOR(FromPacket, MultiplayerExtensions::Beatmaps::PreviewBeatmapPacket*);

	DECLARE_INSTANCE_METHOD(MultiplayerExtensions::Beatmaps::PreviewBeatmapPacket*, GetPacket, Il2CppString*, GlobalNamespace::BeatmapDifficulty);

	DECLARE_INSTANCE_FIELD(GlobalNamespace::IPreviewBeatmapLevel*, _preview);
	DECLARE_INSTANCE_FIELD(bool, isDownloaded);
	DECLARE_INSTANCE_FIELD(bool, isDownloadable);



	DECLARE_INSTANCE_FIELD(Il2CppString*, levelID);
	DECLARE_INSTANCE_FIELD(Il2CppString*, levelHash);

	DECLARE_INSTANCE_FIELD(Il2CppString*, songName);
	DECLARE_INSTANCE_FIELD(Il2CppString*, songSubName);
	DECLARE_INSTANCE_FIELD(Il2CppString*, songAuthorName);
	DECLARE_INSTANCE_FIELD(Il2CppString*, levelAuthorName);
	DECLARE_INSTANCE_FIELD(float, beatsPerMinute);
	DECLARE_INSTANCE_FIELD(float, songDuration);

	DECLARE_INSTANCE_FIELD(float, songTimeOffset);
	DECLARE_INSTANCE_FIELD(float, previewDuration);
	DECLARE_INSTANCE_FIELD(float, previewStartTime);
	DECLARE_INSTANCE_FIELD(float, shuffle);
	DECLARE_INSTANCE_FIELD(float, shufflePeriod);
	DECLARE_INSTANCE_FIELD(GlobalNamespace::EnvironmentInfoSO*, allDirectionsEnvironmentInfo);
	DECLARE_INSTANCE_FIELD(GlobalNamespace::EnvironmentInfoSO*, environmentInfo);
	DECLARE_INSTANCE_FIELD(Array<GlobalNamespace::PreviewDifficultyBeatmapSet*>*, previewDifficultyBeatmapSets);

	DECLARE_OVERRIDE_METHOD(Il2CppString*, get_levelID, il2cpp_utils::FindMethodUnsafe(classof(GlobalNamespace::IPreviewBeatmapLevel*), "get_levelID", 0));
	DECLARE_OVERRIDE_METHOD(Il2CppString*, get_songName, il2cpp_utils::FindMethodUnsafe(classof(GlobalNamespace::IPreviewBeatmapLevel*), "get_songName", 0));
	DECLARE_OVERRIDE_METHOD(Il2CppString*, get_songSubName, il2cpp_utils::FindMethodUnsafe(classof(GlobalNamespace::IPreviewBeatmapLevel*), "get_songSubName", 0));
	DECLARE_OVERRIDE_METHOD(Il2CppString*, get_songAuthorName, il2cpp_utils::FindMethodUnsafe(classof(GlobalNamespace::IPreviewBeatmapLevel*), "get_songAuthorName", 0));
	DECLARE_OVERRIDE_METHOD(Il2CppString*, get_levelAuthorName, il2cpp_utils::FindMethodUnsafe(classof(GlobalNamespace::IPreviewBeatmapLevel*), "get_levelAuthorName", 0));
	DECLARE_OVERRIDE_METHOD(float, get_beatsPerMinute, il2cpp_utils::FindMethodUnsafe(classof(GlobalNamespace::IPreviewBeatmapLevel*), "get_beatsPerMinute", 0));
	DECLARE_OVERRIDE_METHOD(float, get_songDuration, il2cpp_utils::FindMethodUnsafe(classof(GlobalNamespace::IPreviewBeatmapLevel*), "get_songDuration", 0));
	DECLARE_OVERRIDE_METHOD(float, get_songTimeOffset, il2cpp_utils::FindMethodUnsafe(classof(GlobalNamespace::IPreviewBeatmapLevel*), "get_songTimeOffset", 0));
	DECLARE_OVERRIDE_METHOD(float, get_previewDuration, il2cpp_utils::FindMethodUnsafe(classof(GlobalNamespace::IPreviewBeatmapLevel*), "get_previewDuration", 0));
	DECLARE_OVERRIDE_METHOD(float, get_previewStartTime, il2cpp_utils::FindMethodUnsafe(classof(GlobalNamespace::IPreviewBeatmapLevel*), "get_previewStartTime", 0));
	DECLARE_OVERRIDE_METHOD(float, get_shuffle, il2cpp_utils::FindMethodUnsafe(classof(GlobalNamespace::IPreviewBeatmapLevel*), "get_shuffle", 0));
	DECLARE_OVERRIDE_METHOD(float, get_shufflePeriod, il2cpp_utils::FindMethodUnsafe(classof(GlobalNamespace::IPreviewBeatmapLevel*), "get_shufflePeriod", 0));
	DECLARE_OVERRIDE_METHOD(GlobalNamespace::EnvironmentInfoSO*, get_allDirectionsEnvironmentInfo, il2cpp_utils::FindMethodUnsafe(classof(GlobalNamespace::IPreviewBeatmapLevel*), "get_allDirectionsEnvironmentInfo", 0));
	DECLARE_OVERRIDE_METHOD(GlobalNamespace::EnvironmentInfoSO*, get_environmentInfo, il2cpp_utils::FindMethodUnsafe(classof(GlobalNamespace::IPreviewBeatmapLevel*), "get_environmentInfo", 0));
	DECLARE_OVERRIDE_METHOD(Array<GlobalNamespace::PreviewDifficultyBeatmapSet*>*, get_previewDifficultyBeatmapSets, il2cpp_utils::FindMethodUnsafe(classof(GlobalNamespace::IPreviewBeatmapLevel*), "get_previewDifficultyBeatmapSets", 0));

	DECLARE_OVERRIDE_METHOD(System::Threading::Tasks::Task_1<UnityEngine::Sprite*>*, GetCoverImageAsync, il2cpp_utils::FindMethodUnsafe(classof(GlobalNamespace::IPreviewBeatmapLevel*), "GetCoverImageAsync", 1), System::Threading::CancellationToken cancellationToken);
	DECLARE_OVERRIDE_METHOD(System::Threading::Tasks::Task_1<UnityEngine::AudioClip*>*, GetPreviewAudioClipAsync, il2cpp_utils::FindMethodUnsafe(classof(GlobalNamespace::IPreviewBeatmapLevel*), "GetPreviewAudioClipAsync", 1), System::Threading::CancellationToken cancellationToken);

	// C++ Definitions
	private:
		DownloadableState _downloadable;
)