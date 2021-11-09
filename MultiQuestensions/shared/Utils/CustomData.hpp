#pragma once
#include <string>
#include "GlobalNamespace/CustomPreviewBeatmapLevel.hpp"
//#include "songdownloader/shared/Types/TypeMacros.hpp"
//
//DECLARE_JSON_CLASS(MultiQuestensions::Utils, Difficulty,
//    GETTER_VALUE(std::vector<std::string>, Requirements);
//)
//
//DECLARE_JSON_CLASS(MultiQuestensions::Utils, Difficulty,
//    GETTER_VALUE(std::vector<MultiQuestensions::Utils::CustomData>, CustomData);
//)
//
//DECLARE_JSON_CLASS(MultiQuestensions::Utils, DifficultySet,
//    GETTER_VALUE(std::vector<MultiQuestensions::Utils::Difficulty>, Versions);
//)
//
//DECLARE_JSON_CLASS(MultiQuestensions::Utils, LevelInfo,
//    GETTER_VALUE(std::vector<MultiQuestensions::Utils::DifficultySet>, DiffSets);
//)

namespace MultiQuestensions::Utils {
    inline bool CheckRequirements(std::optional<GlobalNamespace::CustomPreviewBeatmapLevel*> beatmapOpt) {
        if (beatmapOpt)
        {
            std::string infodatPath = to_utf8(csstrtostr(beatmapOpt.value()->get_customLevelPath())) + "/Info.dat";
            rapidjson::Document document;
            document.Parse(readfile(infodatPath));
            if ((!document.HasParseError() || document.IsObject()) && document.HasMember("")) {

            }
        }
    return false;
    }
}
