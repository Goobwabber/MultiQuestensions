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

    System::Action_3<::Il2CppString*, ::Il2CppString*, EntitlementsStatus>* entitlementAction;
#pragma endregion

    // Subscribe this method to 'menuRpcManager.setIsEntitledToLevelEvent' when on NetworkPlayerEntitlementChecker.Start, unsub on destroy
    static void HandleEntitlementReceived(Il2CppString* userId, Il2CppString* levelId, EntitlementsStatus entitlement) {
        std::string cUserId = to_utf8(csstrtostr(userId)).c_str();
        std::string cLevelId = to_utf8(csstrtostr(levelId)).c_str();
        //entitlementDictionary.emplace(std::make_pair(cUserId,  cLevelId), entitlement.value);
        //if (!entitlementDictionary.count(cUserId)) {
        //    entitlementDictionary[cUserId] = std::map<std::string, int>;
        //}
        entitlementDictionary[cUserId][cLevelId] = entitlement.value;
        if (lobbyGameStateController != nullptr && lobbyGameStateController->get_state() == MultiplayerLobbyState::GameStarting) {
            lobbyGameStateController->HandleMultiplayerLevelLoaderCountdownFinished(loadingPreviewBeatmapLevel, loadingBeatmapDifficulty, loadingBeatmapCharacteristic, loadingDifficultyBeatmap, loadingGameplayModifiers);
        }
    }

#pragma region Hooks
    MAKE_HOOK_MATCH(NetworkPlayerEntitlementChecker_GetEntitlementStatus, &NetworkPlayerEntitlementChecker::GetEntitlementStatus, Task_1<EntitlementsStatus>*, NetworkPlayerEntitlementChecker* self, Il2CppString* levelIdCS) {
        std::string levelId = to_utf8(csstrtostr(levelIdCS));
        getLogger().info("NetworkPlayerEntitlementChecker_GetEntitlementStatus: %s", levelId.c_str());
        if (IsCustomLevel(levelId)) {
            if (HasSong(levelId)) {
                return Task_1<EntitlementsStatus>::New_ctor(EntitlementsStatus::Ok);
            }
            else {
                auto task = Task_1<EntitlementsStatus>::New_ctor();
                BeatSaver::API::GetBeatmapByHashAsync(GetHash(levelId),
                    [task](std::optional<BeatSaver::Beatmap> beatmap) {
                        QuestUI::MainThreadScheduler::Schedule(
                            [task, beatmap] {
                                if (beatmap.has_value()) {
                                    task->TrySetResult(EntitlementsStatus::NotDownloaded);
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
        entitlementAction = il2cpp_utils::MakeDelegate<System::Action_3<::Il2CppString*, ::Il2CppString*, EntitlementsStatus>*>(classof(System::Action_3<::Il2CppString*, ::Il2CppString*, EntitlementsStatus>*), (std::function<void(Il2CppString*, Il2CppString*, EntitlementsStatus)>) [&](Il2CppString* userId, Il2CppString* beatmapId, EntitlementsStatus status) {
            HandleEntitlementReceived(userId, beatmapId, status);
            });
        self->rpcManager->add_setIsEntitledToLevelEvent(entitlementAction);
        NetworkPlayerEntitlementChecker_Start(self);
    }

    MAKE_HOOK_MATCH(NetworkPlayerEntitlementChecker_OnDestroy, &NetworkPlayerEntitlementChecker::OnDestroy, void, NetworkPlayerEntitlementChecker* self) {
        if (entitlementAction)
            self->rpcManager->remove_setIsEntitledToLevelEvent(entitlementAction);
        NetworkPlayerEntitlementChecker_OnDestroy(self);
    }
#pragma endregion

    void Hooks::NetworkplayerEntitlementChecker() {
        INSTALL_HOOK(getLogger(), NetworkPlayerEntitlementChecker_GetEntitlementStatus);
        INSTALL_HOOK(getLogger(), NetworkPlayerEntitlementChecker_Start);
        INSTALL_HOOK(getLogger(), NetworkPlayerEntitlementChecker_OnDestroy);
    }
}