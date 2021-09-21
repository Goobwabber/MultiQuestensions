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
			List<MultiplayerPlayerResultsData*>* newResultsData = Enumerable::ToList<MultiplayerPlayerResultsData*>(Enumerable::Take<MultiplayerPlayerResultsData*>(reinterpret_cast<IEnumerable_1<MultiplayerPlayerResultsData*>*>(resultsData), 5));
			MultiplayerResultsPyramidPatch(self, (IReadOnlyList_1<MultiplayerPlayerResultsData*>*)newResultsData, badgeStartTransform, badgeMidTransform);
		}
		catch (const std::runtime_error& e) {
			getLogger().critical("%s", e.what());
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
			getLogger().critical("%s", e.what());
			IntroAnimationPatch(self, maxDesiredIntroAnimationDuration, onCompleted);
		}
	}

	MAKE_HOOK_MATCH(CalculatePlayerIndexSequencePatch, &MultiplayerIntroAnimationController::CalculatePlayerIndexSequence, Queue_1<int>*, MultiplayerIntroAnimationController* self, IReadOnlyList_1<IConnectedPlayer*>* allActivePlayer) {
		try {
			List<IConnectedPlayer*>* listActivePlayers = Enumerable::ToList<IConnectedPlayer*>(reinterpret_cast<IEnumerable_1<IConnectedPlayer*>*>(allActivePlayer));
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
			List<IConnectedPlayer*>* selectedActivePlayers = Enumerable::ToList<IConnectedPlayer*>(Enumerable::Take<IConnectedPlayer*>(Enumerable::Skip<IConnectedPlayer*>(reinterpret_cast<IEnumerable_1<IConnectedPlayer*>*>(listActivePlayers), (targetIterations - 1) * 4), 4));

			// Add back local player if not null
			if (targetIterations == 1 && localPlayer != nullptr) {
				selectedActivePlayers->Add(localPlayer);
			}

			// Call method with new list of players
			CalculatePlayerIndexSequencePatch(self, (IReadOnlyList_1<IConnectedPlayer*>*)selectedActivePlayers);
		}
		catch (const std::runtime_error& e) {
			getLogger().critical("%s", e.what());
			CalculatePlayerIndexSequencePatch(self, allActivePlayer);
		}
	}

	void Hooks::MaxPlayerHooks() {
		INSTALL_HOOK(getLogger(), MultiplayerResultsPyramidPatch);
		INSTALL_HOOK(getLogger(), IntroAnimationPatch);
		INSTALL_HOOK(getLogger(), CalculatePlayerIndexSequencePatch);
	}
}