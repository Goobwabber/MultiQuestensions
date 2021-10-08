#include "main.hpp"
#include "Hooks/Hooks.hpp"
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

    // TODO: Find a fix for players disappearing when the below is run
    MAKE_HOOK_MATCH(CalculatePlayerIndexSequencePatch, &MultiplayerIntroAnimationController::CalculatePlayerIndexSequence, Queue_1<int>*, MultiplayerIntroAnimationController* self, IReadOnlyList_1<IConnectedPlayer*>* allActivePlayer) {
        getLogger().debug("Start: CalculatePlayerIndexSequencePatch");
        try {
            // 09-21 14:30:26.025  4647  4672 D QuestHook[UtilsLogger|v2.3.0]: (il2cpp_utils::FindMethod) Method 27:
            // 09-21 14:30:26.026  4647  4672 D QuestHook[UtilsLogger|v2.3.0]: (il2cpp_utils::FindMethod) static
            // System.Collections.Generic.List<TSource> ToList(System.Collections.Generic.IEnumerable<TSource> source);
            static auto* Enumerable_ToList_Generic = THROW_UNLESS(il2cpp_utils::FindMethodUnsafe(classof(Enumerable*), "ToList", 1));
            static auto* Enumerable_ToList = THROW_UNLESS(il2cpp_utils::MakeGenericMethod(Enumerable_ToList_Generic, { classof(IConnectedPlayer*) }));
            List<IConnectedPlayer*>* listActivePlayers = il2cpp_utils::RunMethodThrow<List_1<IConnectedPlayer*>*, false>(static_cast<Il2CppClass*>(nullptr), 
                Enumerable_ToList, reinterpret_cast<IEnumerable_1<IConnectedPlayer*>*>(allActivePlayer));
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

            static auto* Enumerable_Take_Generic = THROW_UNLESS(il2cpp_utils::FindMethodUnsafe(classof(Enumerable*), "Take", 2));
            static auto* Enumerable_Take = THROW_UNLESS(il2cpp_utils::MakeGenericMethod(Enumerable_Take_Generic, { classof(IConnectedPlayer*) }));
            auto* takeResult = il2cpp_utils::RunMethodThrow<IEnumerable_1<IConnectedPlayer*>*, false>(static_cast<Il2CppClass*>(nullptr),
                Enumerable_Take, skipResult, 4);

            List<IConnectedPlayer*>* selectedActivePlayers = il2cpp_utils::RunMethodThrow<List_1<IConnectedPlayer*>*, false>(static_cast<Il2CppClass*>(nullptr), 
                Enumerable_ToList, takeResult);

            //List<IConnectedPlayer*>* selectedActivePlayers = Enumerable::ToList<IConnectedPlayer*>(Enumerable::Take<IConnectedPlayer*>(Enumerable::Skip<IConnectedPlayer*>(reinterpret_cast<IEnumerable_1<IConnectedPlayer*>*>(listActivePlayers), (targetIterations - 1) * 4), 4));
            //List<IConnectedPlayer*>* selectedActivePlayers = Enumerable::ToList<IConnectedPlayer*>(Enumerable::Take<IConnectedPlayer*>(skipResult, 4));

            // Add back local player if not null
            if (targetIterations == 1 && localPlayer != nullptr) {
                selectedActivePlayers->Add(localPlayer);
            }

            getLogger().debug("Finish: CalculatePlayerIndexSequencePatch");

            // Call method with new list of players
            return CalculatePlayerIndexSequencePatch(self, reinterpret_cast<IReadOnlyList_1<IConnectedPlayer*>*>(selectedActivePlayers));
        }
        catch (const std::runtime_error& e) {
            getLogger().critical("Hook CalculatePlayerIndexSequencePatch" __FILE__ " at Line %d: %s", __LINE__, e.what());
        }
        return CalculatePlayerIndexSequencePatch(self, allActivePlayer);
    }

    void Hooks::MaxPlayerHooks() {
        INSTALL_HOOK(getLogger(), MultiplayerResultsPyramidPatch);
        INSTALL_HOOK(getLogger(), IntroAnimationPatch);
        //INSTALL_HOOK(getLogger(), CalculatePlayerIndexSequencePatch);
    }
}