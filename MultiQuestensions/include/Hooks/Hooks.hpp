#pragma once
#include "GlobalFieldsTemp.hpp"
#include "beatsaber-hook/shared/utils/hooking.hpp"
#include "GlobalNamespace/NetworkPlayerEntitlementChecker.hpp"

#include "System/Collections/Generic/Dictionary_2.hpp"
#include "System/Collections/Generic/IReadOnlyList_1.hpp"
#include "System/Collections/Generic/IEnumerable_1.hpp"
#include "System/Collections/Generic/Queue_1.hpp"
#include "System/Collections/Hashtable.hpp"
#include "System/Collections/Hashtable_ValueCollection.hpp"
#include "System/Threading/Tasks/Task_1.hpp"
#include "System/Action_1.hpp"
#include "System/Action_3.hpp"
#include "System/Action.hpp"
#include "System/Linq/Enumerable.hpp"

#include "GlobalNamespace/BeatmapIdentifierNetSerializable.hpp"
#include "GlobalNamespace/MultiplayerLevelLoader.hpp"
#include "GlobalNamespace/HMTask.hpp"
#include "GlobalNamespace/StandardScoreSyncStateNetSerializable.hpp"
#include "GlobalNamespace/NetworkPlayerEntitlementChecker.hpp"
#include "GlobalNamespace/LevelSelectionNavigationController.hpp"
#include "GlobalNamespace/LobbySetupViewController.hpp"
#include "GlobalNamespace/CannotStartGameReason.hpp"
#include "GlobalNamespace/LobbyGameStateController.hpp"
#include "GlobalNamespace/ILobbyPlayerData.hpp"
#include "GlobalNamespace/GameServerPlayerTableCell.hpp"
#include "GlobalNamespace/MultiplayerLobbyConnectionController.hpp"
#include "GlobalNamespace/CreateServerFormData.hpp"

#include "GlobalNamespace/MenuRpcManager.hpp"
#include "GlobalNamespace/MultiplayerPlayersManager.hpp"
#include "GlobalNamespace/ConnectedPlayerManager.hpp"
#include "GlobalNamespace/ConnectedPlayerManager_SyncTimePacket.hpp"

#include "GlobalNamespace/MultiplayerPlayerResultsData.hpp"
#include "GlobalNamespace/MultiplayerResultsPyramidView.hpp"
#include "GlobalNamespace/MultiplayerIntroAnimationController.hpp"

#include "UnityEngine/GameObject.hpp"
#include "UnityEngine/Playables/PlayableDirector.hpp"
#include "UnityEngine/Playables/PlayableAsset.hpp"
#include "UnityEngine/Timeline/TimelineAsset.hpp"
#include "UnityEngine/Timeline/TrackAsset.hpp"
#include "UnityEngine/Timeline/AudioTrack.hpp"

#include "questui/shared/CustomTypes/Components/MainThreadScheduler.hpp"

#include "songdownloader/shared/BeatSaverAPI.hpp"

#include "songloader/shared/API.hpp"

namespace MultiQuestensions {

    constexpr bool IsCustomLevel(const std::string& levelId) {
        return levelId.starts_with(RuntimeSongLoader::API::GetCustomLevelsPrefix());
    }

    extern bool HasSong(std::string levelId);


    extern GlobalNamespace::LobbyGameStateController* lobbyGameStateController;

    extern std::map<std::string, std::map<std::string, int>> entitlementDictionary;

    extern GlobalNamespace::IPreviewBeatmapLevel* loadingPreviewBeatmapLevel;
    extern GlobalNamespace::BeatmapDifficulty loadingBeatmapDifficulty;
    extern GlobalNamespace::BeatmapCharacteristicSO* loadingBeatmapCharacteristic;
    extern GlobalNamespace::IDifficultyBeatmap* loadingDifficultyBeatmap;
    extern GlobalNamespace::GameplayModifiers* loadingGameplayModifiers;

    class Hooks {
        static void NetworkplayerEntitlementChecker();
        static void MaxPlayerHooks();
        static void EnvironmentHooks();

    public:
        static const void Install_Hooks() {
            NetworkplayerEntitlementChecker();
            //MaxPlayerHooks();
            EnvironmentHooks();
        }
    };
}