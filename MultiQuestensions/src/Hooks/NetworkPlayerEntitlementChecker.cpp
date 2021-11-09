#include "main.hpp"
#include "Hooks/Hooks.hpp"
using namespace GlobalNamespace;
using namespace System::Threading::Tasks;

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
        std::string cUserId = to_utf8(csstrtostr(userId)).c_str();
        std::string cLevelId = to_utf8(csstrtostr(levelId)).c_str();

        getLogger().debug("[HandleEntitlementReceived] Received Entitlement from user '%s' for level '%s' with status '%s'",
            cUserId.c_str(),
            cLevelId.c_str(),
            entitlementText(entitlement)
            );

        entitlementDictionary[cUserId][cLevelId] = entitlement.value;
        if (lobbyGameStateController != nullptr && lobbyGameStateController->get_state() == MultiplayerLobbyState::GameStarting && loadingPreviewBeatmapLevel && loadingBeatmapCharacteristic && loadingDifficultyBeatmap && loadingGameplayModifiers) {
            getLogger().debug("[HandleEntitlementReceived] GameStarting, running 'HandleMultiplayerLevelLoaderCountdownFinished'");
            lobbyGameStateController->HandleMultiplayerLevelLoaderCountdownFinished(loadingPreviewBeatmapLevel, loadingBeatmapDifficulty, loadingBeatmapCharacteristic, loadingDifficultyBeatmap, loadingGameplayModifiers);
        }
        else if (lobbyGameStateController != nullptr && lobbyGameStateController->get_state() == MultiplayerLobbyState::GameStarting) {
            getLogger().error("[HandleEntitlementReceived] GameStarting but level data not available");
            getLogger().debug("[HandleEntitlementReceived] checking pointers: loadingPreviewBeatmapLevel='%p', loadingBeatmapDifficulty set to '%d', loadingBeatmapCharacteristic='%p', loadingDifficultyBeatmap='%p', loadingGameplayModifiers='%p'", loadingPreviewBeatmapLevel, static_cast<int>(loadingBeatmapDifficulty), loadingBeatmapCharacteristic, loadingDifficultyBeatmap, loadingGameplayModifiers);
        }
    }

#pragma region Hooks
    MAKE_HOOK_MATCH(NetworkPlayerEntitlementChecker_GetEntitlementStatus, &NetworkPlayerEntitlementChecker::GetEntitlementStatus, Task_1<EntitlementsStatus>*, NetworkPlayerEntitlementChecker* self, Il2CppString* levelIdCS) {
        std::string levelId = to_utf8(csstrtostr(levelIdCS));
        getLogger().info("NetworkPlayerEntitlementChecker_GetEntitlementStatus: %s", levelId.c_str());
        if (IsCustomLevel(levelId)) {
            if (HasSong(levelId)) {
                //std::optional<GlobalNamespace::CustomPreviewBeatmapLevel*> level = RuntimeSongLoader::API::GetLevelById(levelId);
                return Task_1<EntitlementsStatus>::New_ctor(EntitlementsStatus::Ok);
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
                                            if (diff.GetChroma() || diff.GetNE()) {
                                                task->TrySetResult(EntitlementsStatus::NotOwned);
                                                getLogger().info("Map contains Chroma or NE difficulty, returning NotOwned");
                                                return;
                                            }
                                        }
                                        task->TrySetResult(EntitlementsStatus::NotDownloaded);
                                        return;
                                    } else getLogger().error("Hash returned by BeatSaver doesn't match requested hash: Expected: '%s' got '%s'", GetHash(levelId).c_str(), mapHash.c_str());
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