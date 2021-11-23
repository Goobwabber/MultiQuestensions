#pragma once
#include "custom-types/shared/macros.hpp"
#include <optional>
#include "GlobalNamespace/IPreviewBeatmapLevel.hpp"
#include "GlobalNamespace/BeatmapDifficulty.hpp"
#include "GlobalNamespace/BeatmapCharacteristicSO.hpp"
#include "GlobalNamespace/IDifficultyBeatmap.hpp"
#include "GlobalNamespace/GameplayModifiers.hpp"

DECLARE_CLASS_CODEGEN(MultiQuestensions, DataStore, Il2CppObject, 

    DECLARE_CTOR(New, GlobalNamespace::IPreviewBeatmapLevel*, GlobalNamespace::BeatmapCharacteristicSO*, GlobalNamespace::IDifficultyBeatmap*, GlobalNamespace::GameplayModifiers*);
    
    DECLARE_INSTANCE_FIELD(GlobalNamespace::IPreviewBeatmapLevel*, loadingPreviewBeatmapLevel);
    DECLARE_INSTANCE_FIELD(GlobalNamespace::BeatmapCharacteristicSO*, loadingBeatmapCharacteristic);
    DECLARE_INSTANCE_FIELD(GlobalNamespace::IDifficultyBeatmap*, loadingDifficultyBeatmap);
    DECLARE_INSTANCE_FIELD(GlobalNamespace::GameplayModifiers*, loadingGameplayModifiers);

    static DataStore* instance;

public:
    std::optional<GlobalNamespace::BeatmapDifficulty> loadingBeatmapDifficulty;

    static DataStore* get_Instance();
    static DataStore* CS_Ctor(GlobalNamespace::IPreviewBeatmapLevel* previewBeatmap, GlobalNamespace::BeatmapCharacteristicSO* beatmapCharacteristic, GlobalNamespace::IDifficultyBeatmap* difficultyBeatmap, GlobalNamespace::GameplayModifiers* gameplayModifiers, GlobalNamespace::BeatmapDifficulty beatmapDifficulty);
    static void Clear();
)