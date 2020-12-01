#pragma once
#include "main.hpp"
#include "packets.hpp"

Il2CppString* LevelIdToHash(Il2CppString*);

class PreviewBeatmapPacket : public CustomPacket {
private:
	std::string get_identifier() override;
	
public:
	Il2CppString* levelId;
	Il2CppString* levelKey;
	Il2CppString* songName;
	Il2CppString* songSubName;
	Il2CppString* songAuthorName;
	Il2CppString* levelAuthorName;
	float beatsPerMinute;
	float songDuration;

	bool isDownloadable;
	Array<uint8_t>* coverImage;

	Il2CppString* characteristic;
	GlobalNamespace::BeatmapDifficulty difficulty;

	void Serialize(LiteNetLib::Utils::NetDataWriter*) override;
	void Deserialize(LiteNetLib::Utils::NetDataReader*) override;
	void Release() override;
};

DEFINE_IL2CPP_ARG_TYPE(PreviewBeatmapPacket, "", "PreviewBeatmapPacket");

class PreviewBeatmapStub : public GlobalNamespace::IPreviewBeatmapLevel {
private:
	System::Threading::Tasks::Task_1<UnityEngine::Sprite*>* _coverGetter;
	System::Threading::Tasks::Task_1<Array<uint8_t>*>* _rawCoverGetter;
	System::Threading::Tasks::Task_1<UnityEngine::AudioClip*>* _audioGetter;

public:
	Il2CppString* levelHash;
	Il2CppString* levelKey;

	bool isDownloaded;
	bool isDownloadable = false;

	Il2CppString* levelID;
	Il2CppString* songName;
	Il2CppString* songSubName;
	Il2CppString* songAuthorName;
	Il2CppString* levelAuthorName;
	float beatsPerMinute;
	float songDuration;

	PreviewBeatmapStub(GlobalNamespace::IPreviewBeatmapLevel*);
	PreviewBeatmapStub(PreviewBeatmapPacket*);
	PreviewBeatmapPacket* GetPacket(Il2CppString*, GlobalNamespace::BeatmapDifficulty);

	System::Threading::Tasks::Task_1<Array<uint8_t>*>* GetRawCoverAsync(System::Threading::CancellationToken);
	System::Threading::Tasks::Task_1<UnityEngine::Sprite*>* GetCoverImageAsync(System::Threading::CancellationToken);
	System::Threading::Tasks::Task_1<UnityEngine::AudioClip*>* GetPreviewAudioClipAsync(System::Threading::CancellationToken);
};