#include "main.hpp"
#include "Utils/CustomData.hpp"

namespace MultiQuestensions::Utils {
    bool HasRequirement(const std::optional<GlobalNamespace::CustomPreviewBeatmapLevel*>& beatmapOpt) {
        if (beatmapOpt)
        {
            std::string infodatPath = to_utf8(csstrtostr(beatmapOpt.value()->get_customLevelPath()));
            if (fileexists(infodatPath + "/Info.dat")) {
                infodatPath += "/Info.dat";
            }
            else if (fileexists(infodatPath + "/info.dat")) {
                infodatPath += "/info.dat";
            }
            else {
                getLogger().error("Could not find Info.dat or info.dat");
                return false;
            }
            rapidjson::Document document;
            document.Parse(readfile(infodatPath));
            if (!document.HasParseError() && document.IsObject() &&
                document.HasMember("_difficultyBeatmapSets") && document["_difficultyBeatmapSets"].IsArray() && !document["_difficultyBeatmapSets"].Empty()) {
                for (auto& dBS : document["_difficultyBeatmapSets"].GetArray())
                    if (dBS.HasMember("_difficultyBeatmaps") && dBS["_difficultyBeatmaps"].IsArray() && !dBS["_difficultyBeatmaps"].Empty()) {
                        for (auto& dB : dBS["_difficultyBeatmaps"].GetArray()) {
                            if (dB.HasMember("_customData") && dB["_customData"].IsObject()) {
                                if (dB["_customData"].HasMember("_requirements") && dB["_customData"]["_requirements"].IsArray() &&
                                    !dB["_customData"]["_requirements"].Empty()) {
                                    for (auto& req : dB["_customData"]["_requirements"].GetArray()) {
                                        getLogger().debug("Found requirement: %s", req.GetString());
                                        if (strcmp(req.GetString(), "Chroma") == 0 && !AllPlayersHaveChroma()) {
                                            getLogger().warning("Requires Chroma");
                                            missingLevelText = "Chroma Requirement block, please uninstall Chroma, as it currently causes crashes";
                                            return false;
                                        }
                                        else if (strcmp(req.GetString(), "Noodle Extensions") == 0 && !AllPlayersHaveNE() && Modloader::getMods().find("NoodleExtensions") == Modloader::getMods().end()) {
                                            getLogger().warning("Requires Noodle Extensions but it was not installed");
                                            missingLevelText = "You or another Player is Missing the following Requirement: Noodle Extensions";
                                            return false;
                                        }
                                        else if (strcmp(req.GetString(), "Mapping Extensions") == 0 && !AllPlayersHaveME() && Modloader::getMods().find("MappingExtensions") == Modloader::getMods().end()) {
                                            getLogger().warning("Requires Mapping Extensions but it was not installed");
                                            missingLevelText = "You or another Player is Missing the following Requirement: Mapping Extensions";
                                            return false;
                                        }
                                    }
                                }
                                if (dB["_customData"].HasMember("_suggestions") && dB["_customData"]["_suggestions"].IsArray() &&
                                    !dB["_customData"]["_suggestions"].Empty()) {
                                    for (auto& sug : dB["_customData"]["_suggestions"].GetArray()) {
                                        getLogger().debug("Found suggestion: %s", sug.GetString());
                                        if (strcmp(sug.GetString(), "Chroma") == 0 && Modloader::getMods().find("Chroma") != Modloader::getMods().end()) {
                                            getLogger().warning("Suggestion Chroma and Chroma installed, returning false, as Chroma may cause crashes");
                                            missingLevelText = "Chroma suggestion block, please uninstall Chroma, as it currently causes crashes";
                                            return false;
                                        }
                                    }
                                }
                            }
                            else getLogger().warning("No _customData or _requirements/_suggestions empty/not an array");
                        }
                    }
                    else getLogger().error("_difficultyBeatmaps not found, empty or not an array");
            }
            else getLogger().error("Failed to parse beatmap or _difficultyBeatmapSets empty/not an array");
        }
        missingLevelText.clear();
        return true;
    }
}
