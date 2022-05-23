#pragma once
#include "custom-types/shared/macros.hpp"
#include "Players/MpexPlayerData.hpp"
#include "MultiplayerCore/shared/Players/MpPlayerData.hpp"
#include "Players/MpexPlayerManager.hpp"
#include "UnityEngine/MonoBehaviour.hpp"
#include "HMUI/ImageView.hpp"
#include "HMUI/CurvedTextMeshPro.hpp"
#include "GlobalNamespace/IConnectedPlayer.hpp"
#include "UnityEngine/Sprite.hpp"
#include <map>

DECLARE_CLASS_CODEGEN(MultiQuestensions::Environments, MQEAvatarNameTag, UnityEngine::MonoBehaviour,
    DECLARE_INSTANCE_FIELD(GlobalNamespace::IConnectedPlayer*, _playerInfo);

    DECLARE_INSTANCE_FIELD(HMUI::ImageView*, _bg);
    DECLARE_INSTANCE_FIELD(HMUI::CurvedTextMeshPro*, _nameText);

    DECLARE_CTOR(New);

    DECLARE_INSTANCE_METHOD(void, Awake);
    DECLARE_INSTANCE_METHOD(void, OnEnable);
    DECLARE_INSTANCE_METHOD(void, OnDisable);

    DECLARE_INSTANCE_METHOD(void, SetPlayerInfo, GlobalNamespace::IConnectedPlayer* player);
    // DECLARE_INSTANCE_METHOD(void, SetExtendedPlayerInfo, MultiQuestensions::Extensions::ExtendedPlayer* extendedPlayer);
    DECLARE_INSTANCE_METHOD(void, SetPlatformData, MultiplayerCore::Players::MpPlayerData* data);
    DECLARE_INSTANCE_METHOD(void, SetSimplePlayerInfo, GlobalNamespace::IConnectedPlayer* simplePlayer);
    DECLARE_INSTANCE_METHOD(void, HandlePlatformData, GlobalNamespace::IConnectedPlayer* player, MultiplayerCore::Players::MpPlayerData* data);
    DECLARE_INSTANCE_METHOD(void, HandleMpexData, GlobalNamespace::IConnectedPlayer* player, MultiQuestensions::Players::MpexPlayerData* data);

    enum PlayerIconSlot {
        Platform = 0
    };

    bool _enabled;

    MultiplayerCore::event_handler<GlobalNamespace::IConnectedPlayer*, MultiplayerCore::Players::MpPlayerData*> _platformDataEventHandler;
    MultiplayerCore::event_handler<GlobalNamespace::IConnectedPlayer*, MultiQuestensions::Players::MpexPlayerData*> _mpexPlayerDataEventHandler;
    

    std::map<PlayerIconSlot, HMUI::ImageView*> _playerIcons;

    void SetIcon(PlayerIconSlot slot, UnityEngine::Sprite* sprite);
    void RemoveIcon(PlayerIconSlot slot);
)