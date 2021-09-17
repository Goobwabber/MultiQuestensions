#pragma once
#include "beatsaber-hook/shared/utils/hooking.hpp"
#include "GlobalNamespace/NetworkPlayerEntitlementChecker.hpp"

#include "System/Collections/Generic/Dictionary_2.hpp"
#include "System/Collections/Hashtable.hpp"
#include "System/Collections/Hashtable_ValueCollection.hpp"
#include "System/Threading/Tasks/Task_1.hpp"
#include "System/Action_1.hpp"
#include "System/Action_3.hpp"
#include "System/Action.hpp"

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
#include "GlobalNamespace/ConnectedPlayerManager.hpp"
#include "GlobalNamespace/ConnectedPlayerManager_SyncTimePacket.hpp"

#include "questui/shared/CustomTypes/Components/MainThreadScheduler.hpp"

#include "songdownloader/shared/BeatSaverAPI.hpp"

namespace MultiQuestensions {

    extern bool IsCustomLevel(const std::string& levelId);

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

    public:
        static const void Install_Hooks() {
            NetworkplayerEntitlementChecker();
        }
    };
}