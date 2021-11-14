#pragma once
#include <string>
#include "GlobalNamespace/CustomPreviewBeatmapLevel.hpp"

namespace MultiQuestensions::Utils {
    bool HasRequirement(const std::optional<GlobalNamespace::CustomPreviewBeatmapLevel*>& beatmapOpt);
}
