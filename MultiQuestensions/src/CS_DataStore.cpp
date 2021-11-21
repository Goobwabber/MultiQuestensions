#include "main.hpp"
#include "CS_DataStore.hpp"

DEFINE_TYPE(MultiQuestensions, DataStore);

namespace MultiQuestensions {
    void DataStore::New(GlobalNamespace::IPreviewBeatmapLevel* previewBeatmap, GlobalNamespace::BeatmapCharacteristicSO* beatmapCharacteristic, GlobalNamespace::IDifficultyBeatmap* difficultyBeatmap, GlobalNamespace::GameplayModifiers* gameplayModifiers) {
        loadingPreviewBeatmapLevel = previewBeatmap;
        loadingBeatmapCharacteristic = beatmapCharacteristic;
        loadingDifficultyBeatmap = difficultyBeatmap;
        loadingGameplayModifiers = gameplayModifiers;
    }

    DataStore* DataStore::instance;

    DataStore* DataStore::get_Instance() {
        return instance;
    }

    DataStore* DataStore::CS_Ctor(GlobalNamespace::IPreviewBeatmapLevel* previewBeatmap, GlobalNamespace::BeatmapCharacteristicSO* beatmapCharacteristic, GlobalNamespace::IDifficultyBeatmap* difficultyBeatmap, GlobalNamespace::GameplayModifiers* gameplayModifiers, GlobalNamespace::BeatmapDifficulty beatmapDifficulty) {
        instance = THROW_UNLESS(il2cpp_utils::New<MultiQuestensions::DataStore*, il2cpp_utils::CreationType::Manual>(previewBeatmap, beatmapCharacteristic, difficultyBeatmap, gameplayModifiers));
        instance->loadingBeatmapDifficulty = beatmapDifficulty;
        return instance;
    }

    void DataStore::Clear() {
        if (instance) {
            instance->loadingPreviewBeatmapLevel = nullptr;
            instance->loadingBeatmapCharacteristic = nullptr;
            instance->loadingDifficultyBeatmap = nullptr;
            instance->loadingGameplayModifiers = nullptr;
            instance->loadingBeatmapDifficulty = std::nullopt;
        }
    }
}