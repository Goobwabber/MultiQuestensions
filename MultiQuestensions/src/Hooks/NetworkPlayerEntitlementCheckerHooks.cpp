#include "main.hpp"
#include "Hooks/Hooks.hpp"
#include "Utils/CustomData.hpp"
#include "CS_DataStore.hpp"
using namespace GlobalNamespace;
using namespace System::Threading::Tasks;

std::string missingLevelText;

namespace MultiQuestensions {

#pragma region Fields
    std::map<std::string, std::map<std::string, int>> entitlementDictionary;

    //static std::multimap<std::string, std::string, int> entitlementDictionary;

    GlobalNamespace::IPreviewBeatmapLevel* loadingPreviewBeatmapLevel;
    GlobalNamespace::BeatmapDifficulty loadingBeatmapDifficulty;
    GlobalNamespace::BeatmapCharacteristicSO* loadingBeatmapCharacteristic;
    GlobalNamespace::IDifficultyBeatmap* loadingDifficultyBeatmap;
    GlobalNamespace::GameplayModifiers* loadingGameplayModifiers;

    //System::Action_3<::Il2CppString*, ::Il2CppString*, EntitlementsStatus>* entitlementAction;
#pragma endregion

    // For debugging purposes
    const char* entitlementText(EntitlementsStatus entitlement) {
        switch (entitlement.value) {
        case EntitlementsStatus::Unknown:
            return "Unknown";
        case EntitlementsStatus::NotOwned:
            return "NotOwned";
        case EntitlementsStatus::NotDownloaded:
            return "NotDownloaded";
        case EntitlementsStatus::Ok:
            return "Ok";
        }
        return "";
    }

    // Subscribe this method to 'menuRpcManager.setIsEntitledToLevelEvent' when on NetworkPlayerEntitlementChecker.Start, unsub on destroy
    static void HandleEntitlementReceived(Il2CppString* userId, Il2CppString* levelId, EntitlementsStatus entitlement) {
        std::string cUserId = to_utf8(csstrtostr(userId));
        std::string cLevelId = to_utf8(csstrtostr(levelId));

        getLogger().debug("[HandleEntitlementReceived] Received Entitlement from user '%s' for level '%s' with status '%s'",
            cUserId.c_str(),
            cLevelId.c_str(),
            entitlementText(entitlement)
            );

        if (entitlement == EntitlementsStatus::NotOwned && MultiQuestensions::Utils::HasRequirement(RuntimeSongLoader::API::GetLevelById(cLevelId))) {
            IConnectedPlayer* player = sessionManager->GetPlayerByUserId(userId);
            if (player) {
                if (!player->HasState(getMEStateStr()))
                    missingLevelText = "One or more players are missing the following Requirement: Mapping Extensions";
                else if (!player->HasState(getNEStateStr()))
                    missingLevelText = "One or more players are missing the following Requirement: Noodle Extensions";
                else if (!player->HasState(getChromaStateStr()))
                    missingLevelText = "One or more players are missing the following Requirement: Chroma";
            }
            else {
                missingLevelText = "Error Checking Requirement: Player not found";
            }
        }
        entitlementDictionary[cUserId][cLevelId] = entitlement.value;
        if (lobbyGameStateController != nullptr && lobbyGameStateController->get_state() == MultiplayerLobbyState::GameStarting) {
            DataStore* instance = DataStore::get_Instance();
            if (instance && instance->loadingPreviewBeatmapLevel && instance->loadingBeatmapCharacteristic && instance->loadingDifficultyBeatmap && instance->loadingGameplayModifiers && instance->loadingBeatmapDifficulty) {
                getLogger().debug("[HandleEntitlementReceived] GameStarting, running 'HandleMultiplayerLevelLoaderCountdownFinished'");
                lobbyGameStateController->HandleMultiplayerLevelLoaderCountdownFinished(instance->loadingPreviewBeatmapLevel, instance->loadingBeatmapDifficulty.value(), instance->loadingBeatmapCharacteristic, instance->loadingDifficultyBeatmap, instance->loadingGameplayModifiers);
                return;
            }
            else if (lobbyGameStateController->dyn__multiplayerLevelLoader()) {
                getLogger().debug("[HandleEntitlementReceived] GameStarting, DataStore empty trying to run 'HandleMultiplayerLevelLoaderCountdownFinished' with lvlLoader Data");
                MultiplayerLevelLoader* lvlLoader = lobbyGameStateController->dyn__multiplayerLevelLoader();
                if (lvlLoader->dyn__previewBeatmapLevel() && lvlLoader->dyn__beatmapId() && lvlLoader->dyn__beatmapCharacteristic() && lvlLoader->dyn__difficultyBeatmap() && lvlLoader->dyn__gameplayModifiers()) {
                    lobbyGameStateController->HandleMultiplayerLevelLoaderCountdownFinished(lvlLoader->dyn__previewBeatmapLevel(), lvlLoader->dyn__beatmapId()->get_difficulty(), lvlLoader->dyn__beatmapCharacteristic(), lvlLoader->dyn__difficultyBeatmap(), lvlLoader->dyn__gameplayModifiers());
                    return;
                }
            }
            getLogger().error("[HandleEntitlementReceived] GameStarting but level data not available");
            //getLogger().debug("[HandleEntitlementReceived] checking pointers: loadingPreviewBeatmapLevel='%p', loadingBeatmapDifficulty set to '%d', loadingBeatmapCharacteristic='%p', loadingDifficultyBeatmap='%p', loadingGameplayModifiers='%p'", loadingPreviewBeatmapLevel, static_cast<int>(loadingBeatmapDifficulty), loadingBeatmapCharacteristic, loadingDifficultyBeatmap, loadingGameplayModifiers);
        }
    }

#pragma region Hooks
    MAKE_HOOK_MATCH(NetworkPlayerEntitlementChecker_GetEntitlementStatus, &NetworkPlayerEntitlementChecker::GetEntitlementStatus, Task_1<EntitlementsStatus>*, NetworkPlayerEntitlementChecker* self, Il2CppString* levelIdCS) {
        missingLevelText.clear();
        std::string levelId = to_utf8(csstrtostr(levelIdCS));
        getLogger().info("NetworkPlayerEntitlementChecker_GetEntitlementStatus: %s", levelId.c_str());
        if (IsCustomLevel(levelId)) {
            if (HasSong(levelId)) {
                //std::optional<GlobalNamespace::CustomPreviewBeatmapLevel*> level = RuntimeSongLoader::API::GetLevelById(levelId);
                if (MultiQuestensions::Utils::HasRequirement(RuntimeSongLoader::API::GetLevelById(levelId)))
                    return Task_1<EntitlementsStatus>::New_ctor(EntitlementsStatus::Ok);
                else return Task_1<EntitlementsStatus>::New_ctor(EntitlementsStatus::NotOwned);
            }
            else {
                auto task = Task_1<EntitlementsStatus>::New_ctor();
                BeatSaver::API::GetBeatmapByHashAsync(GetHash(levelId),
                    [task, levelId](std::optional<BeatSaver::Beatmap> beatmaps) {
                        QuestUI::MainThreadScheduler::Schedule(
                            [task, beatmaps, levelId] {
                                if (beatmaps.has_value()) {
                                    // TODO: Server side check, possibly something better mod side as well, this would just prevent downloading
                                    // Possibly taking a look at this https://github.com/BSMGPink/PinkCore/blob/master/include/Utils/RequirementUtils.hpp
                                    //for (auto& beatmap : beatmaps->GetVersions()) {
                                    auto& beatmap = beatmaps->GetVersions().front();
                                    std::string mapHash = beatmap.GetHash();
                                    std::transform(mapHash.begin(), mapHash.end(), mapHash.begin(), toupper);
                                    if (mapHash == GetHash(levelId)) {
                                        for (auto& diff : beatmap.GetDiffs()) {
                                            if (diff.GetChroma() && !AllPlayersHaveChroma() && Modloader::getMods().find("Chroma") != Modloader::getMods().end()) {
                                                task->TrySetResult(EntitlementsStatus::NotOwned);
                                                getLogger().warning("Map contains Chroma difficulty and Chroma is installed, returning NotOwned as Chroma currently causes issues in Multiplayer");
                                                missingLevelText = "Chroma Requirement block, please uninstall Chroma";
                                                return;
                                            }
                                            else if (diff.GetNE() && !AllPlayersHaveNE() && Modloader::getMods().find("NoodleExtensions") == Modloader::getMods().end()) {
                                                task->TrySetResult(EntitlementsStatus::NotOwned);
                                                getLogger().warning("Map contains NE difficulty but NoodleExtensions doesn't seem to be installed, returning NotOwned");
                                                missingLevelText = "You or another Player is Missing the following Requirement: Noodle Extensions";
                                                return;
                                            }
                                            else if (diff.GetME() && !AllPlayersHaveME() && Modloader::getMods().find("MappingExtensions") == Modloader::getMods().end()) {
                                                task->TrySetResult(EntitlementsStatus::NotOwned);
                                                getLogger().warning("Map contains ME difficulty but MappingExtensions doesn't seem to be installed, returning NotOwned");
                                                missingLevelText = "You or another Player is Missing the following Requirement: Mapping Extensions";
                                                return;
                                            }
                                        }
                                        task->TrySetResult(EntitlementsStatus::NotDownloaded);
                                        return;
                                    }
                                    else {
                                        getLogger().error("Hash returned by BeatSaver doesn't match requested hash: Expected: '%s' got '%s'", GetHash(levelId).c_str(), mapHash.c_str());
                                        missingLevelText = string_format("Hash returned by BeatSaver doesn't match requested hash: Expected: '%s' got '%s'", GetHash(levelId).c_str(), mapHash.c_str());
                                    }
                                    //}
                                    task->TrySetResult(EntitlementsStatus::NotOwned);
                                }
                                else {
                                    task->TrySetResult(EntitlementsStatus::NotOwned);
                                }
                            }
                        );
                    }
                );
                return task;
            }
        }
        else {
            return NetworkPlayerEntitlementChecker_GetEntitlementStatus(self, levelIdCS);
        }
    }

    MAKE_HOOK_MATCH(NetworkPlayerEntitlementChecker_Start, &NetworkPlayerEntitlementChecker::Start, void, NetworkPlayerEntitlementChecker* self) {
        //entitlementAction = il2cpp_utils::MakeDelegate<System::Action_3<::Il2CppString*, ::Il2CppString*, EntitlementsStatus>*>(classof(System::Action_3<::Il2CppString*, ::Il2CppString*, EntitlementsStatus>*), (std::function<void(Il2CppString*, Il2CppString*, EntitlementsStatus)>) [&](Il2CppString* userId, Il2CppString* beatmapId, EntitlementsStatus status) {
        //    HandleEntitlementReceived(userId, beatmapId, status);
        //    });
        //self->rpcManager->add_setIsEntitledToLevelEvent(entitlementAction);
        self->dyn__rpcManager()->add_setIsEntitledToLevelEvent(
        il2cpp_utils::MakeDelegate<System::Action_3<::Il2CppString*, ::Il2CppString*, EntitlementsStatus>*>(classof(System::Action_3<::Il2CppString*, ::Il2CppString*, EntitlementsStatus>*), (std::function<void(Il2CppString*, Il2CppString*, EntitlementsStatus)>) [&](Il2CppString* userId, Il2CppString* beatmapId, EntitlementsStatus status) {
            HandleEntitlementReceived(userId, beatmapId, status);
            }));
        NetworkPlayerEntitlementChecker_Start(self);
    }

    // Causes crash when being called
    //MAKE_HOOK_MATCH(NetworkPlayerEntitlementChecker_OnDestroy, &NetworkPlayerEntitlementChecker::OnDestroy, void, NetworkPlayerEntitlementChecker* self) {
    //    if (entitlementAction)
    //        self->rpcManager->remove_setIsEntitledToLevelEvent(entitlementAction);
    //    NetworkPlayerEntitlementChecker_OnDestroy(self);
    //}
#pragma endregion

    void Hooks::NetworkplayerEntitlementChecker() {
        INSTALL_HOOK_ORIG(getLogger(), NetworkPlayerEntitlementChecker_GetEntitlementStatus);
        INSTALL_HOOK(getLogger(), NetworkPlayerEntitlementChecker_Start);
        //INSTALL_HOOK(getLogger(), NetworkPlayerEntitlementChecker_OnDestroy);
    }
}