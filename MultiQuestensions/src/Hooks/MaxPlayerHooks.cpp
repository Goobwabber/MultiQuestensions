#include "main.hpp"
#include "Hooks/Hooks.hpp"
//#include "UnityEngine/Debug.hpp"
#include "GlobalNamespace/MultiplayerOutroAnimationController.hpp"
#include "GlobalNamespace/CreateServerFormController.hpp"
#include "GlobalNamespace/FormattedFloatListSettingsController.hpp"

using namespace GlobalNamespace;
using namespace System;
using namespace System::Collections::Generic;
using namespace System::Linq;
using namespace UnityEngine;
using namespace UnityEngine::Playables;
using namespace UnityEngine::Timeline;

namespace MultiQuestensions {

    int targetIterations = 0;

    MAKE_HOOK_MATCH(MultiplayerResultsPyramidPatch, &MultiplayerResultsPyramidView::SetupResults, void, MultiplayerResultsPyramidView* self, IReadOnlyList_1<MultiplayerPlayerResultsData*>* resultsData, UnityEngine::Transform* badgeStartTransform, UnityEngine::Transform* badgeMidTransform) {
        try {
            static auto* Enumerable_Take_Generic = THROW_UNLESS(il2cpp_utils::FindMethodUnsafe(classof(Enumerable*), "Take", 2));
            static auto* Enumerable_Take = THROW_UNLESS(il2cpp_utils::MakeGenericMethod(Enumerable_Take_Generic, { classof(MultiplayerPlayerResultsData*) }));
            auto* takeResult = il2cpp_utils::RunMethodThrow<IEnumerable_1<MultiplayerPlayerResultsData*>*, false>(static_cast<Il2CppClass*>(nullptr),
                Enumerable_Take, reinterpret_cast<IEnumerable_1<MultiplayerPlayerResultsData*>*>(resultsData), 5);


            static auto* Enumerable_ToList_Generic = THROW_UNLESS(il2cpp_utils::FindMethodUnsafe(classof(Enumerable*), "ToList", 1));
            static auto* Enumerable_ToList = THROW_UNLESS(il2cpp_utils::MakeGenericMethod(Enumerable_ToList_Generic, { classof(MultiplayerPlayerResultsData*) }));
            List<MultiplayerPlayerResultsData*>* newResultsData = il2cpp_utils::RunMethodThrow<List_1<MultiplayerPlayerResultsData*>*, false>(static_cast<Il2CppClass*>(nullptr),
                Enumerable_ToList, takeResult);

            //List<MultiplayerPlayerResultsData*>* newResultsData = Enumerable::ToList<MultiplayerPlayerResultsData*>(Enumerable::Take<MultiplayerPlayerResultsData*>(reinterpret_cast<IEnumerable_1<MultiplayerPlayerResultsData*>*>(resultsData), 5));
            MultiplayerResultsPyramidPatch(self, (IReadOnlyList_1<MultiplayerPlayerResultsData*>*)newResultsData, badgeStartTransform, badgeMidTransform);
        }
        catch (const std::runtime_error& e) {
            getLogger().critical("Hook MultiplayerResultsPyramidPatch File " __FILE__ " at Line %d: %s", __LINE__, e.what());
            MultiplayerResultsPyramidPatch(self, resultsData, badgeStartTransform, badgeMidTransform);
        }
    }

    MAKE_HOOK_MATCH(IntroAnimationPatch, &MultiplayerIntroAnimationController::PlayIntroAnimation, void, MultiplayerIntroAnimationController* self, float maxDesiredIntroAnimationDuration, Action* onCompleted) {
        targetIterations = ((((IReadOnlyCollection_1<GlobalNamespace::IConnectedPlayer*>*)self->multiplayerPlayersManager->allActiveAtGameStartPlayers)->get_Count() - 1) / 4) + 1;
        PlayableDirector* realDirector = self->introPlayableDirector;

        try {
            // Run animation one time for each set of 4 players
            while (targetIterations > 0) {
                if (targetIterations != 1) {
                    // Create duplicated animations
                    GameObject* newPlayableGameObject = GameObject::New_ctor();
                    self->introPlayableDirector = newPlayableGameObject->AddComponent<PlayableDirector*>();

                    using SetPlayableAsset = function_ptr_t<void, Il2CppObject*, PlayableAsset*>;
                    static SetPlayableAsset setPlayableAsset = reinterpret_cast<SetPlayableAsset>(il2cpp_functions::resolve_icall("UnityEngine.Playables.PlayableDirector::SetPlayableAsset"));
                    setPlayableAsset(self->introPlayableDirector, realDirector->get_playableAsset());

                    // Mute duplicated animations except one (otherwise audio is very loud)
                    TimelineAsset* animationTimeline = reinterpret_cast<TimelineAsset*>(self->introPlayableDirector->get_playableAsset());
                    List<TrackAsset*>* outputTracks = Enumerable::ToList<TrackAsset*>(animationTimeline->GetOutputTracks());
                    for (int i = 0; i < outputTracks->get_Count(); i++) {
                        TrackAsset* currentTrack = outputTracks->get_Item(i);
                        bool isAudio = il2cpp_utils::AssignableFrom<AudioTrack*>(reinterpret_cast<Il2CppObject*>(currentTrack)->klass);
                        currentTrack->set_muted(isAudio);
                    }
                }

                self->bindingFinished = false;
                IntroAnimationPatch(self, maxDesiredIntroAnimationDuration, onCompleted);
                self->introPlayableDirector = realDirector;
                targetIterations--;
            }

            // Reset director to real director
            self->introPlayableDirector = realDirector;
        }
        catch (const std::runtime_error& e) {
            getLogger().critical("Hook IntroAnimationPatch" __FILE__ " at Line %d: %s", __LINE__, e.what());
            IntroAnimationPatch(self, maxDesiredIntroAnimationDuration, onCompleted);
        }
    }

    enum CalculatePlayerIndexSequencePatchType {
        None, BindTimeline, BindOutroTimeline
    };

    CalculatePlayerIndexSequencePatchType cpispt = None;

    MAKE_HOOK_MATCH(MultiplayerIntroAnimationController_BindTimeline, &MultiplayerIntroAnimationController::BindTimeline, void, MultiplayerIntroAnimationController* self) {
        cpispt = BindTimeline;
        MultiplayerIntroAnimationController_BindTimeline(self);
        cpispt = None;
    }

    MAKE_HOOK_MATCH(MultiplayerOutroAnimationController_BindOutroTimeline, &MultiplayerOutroAnimationController::BindOutroTimeline, void, MultiplayerOutroAnimationController* self) {
        cpispt = BindOutroTimeline;
        MultiplayerOutroAnimationController_BindOutroTimeline(self);
        cpispt = None;
    }

    MAKE_HOOK_MATCH(MultiplayerPlayersManager_get_allActiveAtGameStartPlayers, &MultiplayerPlayersManager::get_allActiveAtGameStartPlayers, IReadOnlyList_1<GlobalNamespace::IConnectedPlayer*>*, MultiplayerPlayersManager* self) {
        getLogger().debug("Start: MultiplayerPlayersManager_get_allActiveAtGameStartPlayers");
        // 09-21 14:30:26.025  4647  4672 D QuestHook[UtilsLogger|v2.3.0]: (il2cpp_utils::FindMethod) Method 27:
        // 09-21 14:30:26.026  4647  4672 D QuestHook[UtilsLogger|v2.3.0]: (il2cpp_utils::FindMethod) static
        // System.Collections.Generic.List<TSource> ToList(System.Collections.Generic.IEnumerable<TSource> source);
        static auto* Enumerable_ToList_Generic = THROW_UNLESS(il2cpp_utils::FindMethodUnsafe(classof(Enumerable*), "ToList", 1));
        static auto* Enumerable_ToList = THROW_UNLESS(il2cpp_utils::MakeGenericMethod(Enumerable_ToList_Generic, { classof(IConnectedPlayer*) }));

        static auto* Enumerable_Take_Generic = THROW_UNLESS(il2cpp_utils::FindMethodUnsafe(classof(Enumerable*), "Take", 2));
        static auto* Enumerable_Take = THROW_UNLESS(il2cpp_utils::MakeGenericMethod(Enumerable_Take_Generic, { classof(IConnectedPlayer*) }));

        if (cpispt == BindTimeline) {
            try {
                List_1<IConnectedPlayer*>* listActivePlayers = il2cpp_utils::RunMethodThrow<List_1<IConnectedPlayer*>*, false>(static_cast<Il2CppClass*>(nullptr),
                    Enumerable_ToList, reinterpret_cast<IEnumerable_1<IConnectedPlayer*>*>(self->dyn__allActiveAtGameStartPlayers()));
                //List<IConnectedPlayer*>* listActivePlayers = Enumerable::ToList<IConnectedPlayer*>(reinterpret_cast<IEnumerable_1<IConnectedPlayer*>*>(allActivePlayer));
                IConnectedPlayer* localPlayer = nullptr;

                // Check if active players contains local player and remove local player
                for (int i = 0; i < listActivePlayers->get_Count(); i++) {
                    IConnectedPlayer* currentPlayer = listActivePlayers->get_Item(i);
                    if (currentPlayer->get_isMe()) {
                        listActivePlayers->RemoveAt(i);
                        localPlayer = currentPlayer;
                    }
                }

                // Skip x amount of players and then take 4
                static auto* Enumerable_Skip_Generic = THROW_UNLESS(il2cpp_utils::FindMethodUnsafe(classof(Enumerable*), "Skip", 2));
                static auto* Enumerable_Skip = THROW_UNLESS(il2cpp_utils::MakeGenericMethod(Enumerable_Skip_Generic, { classof(IConnectedPlayer*) }));
                auto* skipResult = il2cpp_utils::RunMethodThrow<IEnumerable_1<IConnectedPlayer*>*, false>(static_cast<Il2CppClass*>(nullptr),
                    Enumerable_Skip, reinterpret_cast<IEnumerable_1<IConnectedPlayer*>*>(listActivePlayers), (targetIterations - 1) * 4);

                auto* takeResult = il2cpp_utils::RunMethodThrow<IEnumerable_1<IConnectedPlayer*>*, false>(static_cast<Il2CppClass*>(nullptr),
                    Enumerable_Take, skipResult, 4);

                List_1<IConnectedPlayer*>* selectedActivePlayers = il2cpp_utils::RunMethodThrow<List_1<IConnectedPlayer*>*, false>(static_cast<Il2CppClass*>(nullptr),
                    Enumerable_ToList, takeResult);

                //List_1<IConnectedPlayer*>* selectedActivePlayers = Enumerable::ToList<IConnectedPlayer*>(Enumerable::Take<IConnectedPlayer*>(Enumerable::Skip<IConnectedPlayer*>(reinterpret_cast<IEnumerable_1<IConnectedPlayer*>*>(listActivePlayers), (targetIterations - 1) * 4), 4));
                //List_1<IConnectedPlayer*>* selectedActivePlayers = Enumerable::ToList<IConnectedPlayer*>(Enumerable::Take<IConnectedPlayer*>(skipResult, 4));

                // Add back local player if not null
                if (targetIterations == 1 && localPlayer != nullptr) {
                    selectedActivePlayers->Add(localPlayer);
                }

                getLogger().debug("Finish: MultiplayerPlayersManager_get_allActiveAtGameStartPlayers");

                //return new list of players
                return reinterpret_cast<IReadOnlyList_1<IConnectedPlayer*>*>(selectedActivePlayers);
            }
            catch (const std::runtime_error& e) {
                getLogger().critical("Hook MultiplayerPlayersManager_get_allActiveAtGameStartPlayers" __FILE__ " at Line %d: %s", __LINE__, e.what());
            }
        }
        else if (cpispt == BindOutroTimeline) {

            auto* result = il2cpp_utils::RunMethodThrow<IEnumerable_1<IConnectedPlayer*>*, false>(static_cast<Il2CppClass*>(nullptr),
                Enumerable_Take, reinterpret_cast<List_1<IConnectedPlayer*>*>(self->dyn__allActiveAtGameStartPlayers()), 4);

            return reinterpret_cast<IReadOnlyList_1<IConnectedPlayer*>*>(il2cpp_utils::RunMethodThrow<List_1<IConnectedPlayer*>*, false>(static_cast<Il2CppClass*>(nullptr),
                Enumerable_ToList, result));
        }

        return self->dyn__allActiveAtGameStartPlayers();
    }

    MAKE_HOOK_MATCH(CreateServerFormController_get_formData, &CreateServerFormController::get_formData, CreateServerFormData, CreateServerFormController* self) {
        CreateServerFormData result = CreateServerFormController_get_formData(self);
        result.maxPlayers = (int)std::clamp(self->dyn__maxPlayersList()->value, 2.0f, 10.0f);
        return result;
        //return CreateServerFormData
        //{
        //    (int)std::clamp(self->dyn__maxPlayersList()->value, 2.0f, 10.0f),
        //    self->dyn__netDiscoverable(),
        //    BeatmapDifficultyMask::All,
        //    GameplayModifierMask::All,
        //    SongPackMask::get_all(),
        //    GameplayServerMode::Managed,
        //    SongSelectionMode::OwnerPicks,
        //    GameplayServerControlSettings::All
        //};
    }

    MAKE_HOOK_MATCH(CreateServerFormController_Setup, &CreateServerFormController::Setup, void, CreateServerFormController* self, int selectedNumberOfPlayers, bool netDiscoverable) {
        std::vector<int> rangeVec;
        static auto* Enumerable_ToArray_Generic = THROW_UNLESS(il2cpp_utils::FindMethodUnsafe(classof(Enumerable*), "ToArray", 1));
        static auto* Enumerable_ToArray = THROW_UNLESS(il2cpp_utils::MakeGenericMethod(Enumerable_ToArray_Generic, { classof(int) }));
        il2cpp_utils::RunMethodThrow<::Array<int>*, false>(static_cast<Il2CppClass*>(nullptr),
            Enumerable_ToArray, Enumerable::Range(2, 9))->copy_to(rangeVec);
        //Enumerable::ToArray(Enumerable::Range(2, 9))->copy_to(rangeVec);
        std::vector<float> resultVec(rangeVec.begin(), rangeVec.end());
        self->dyn__maxPlayersList()->dyn__values() = il2cpp_utils::vectorToArray(resultVec);
        CreateServerFormController_Setup(self, selectedNumberOfPlayers, netDiscoverable);
    }

    void Hooks::MaxPlayerHooks() {
        INSTALL_HOOK(getLogger(), MultiplayerResultsPyramidPatch);
        INSTALL_HOOK(getLogger(), IntroAnimationPatch);
        INSTALL_HOOK(getLogger(), MultiplayerIntroAnimationController_BindTimeline);
        INSTALL_HOOK(getLogger(), MultiplayerOutroAnimationController_BindOutroTimeline);

        INSTALL_HOOK_ORIG(getLogger(), MultiplayerPlayersManager_get_allActiveAtGameStartPlayers);
        INSTALL_HOOK(getLogger(), CreateServerFormController_get_formData);
        INSTALL_HOOK(getLogger(), CreateServerFormController_Setup);

    }
}