#pragma once
#include "custom-types/shared/macros.hpp"
#include "Extensions/ExtendedPlayer.hpp"
#include "UnityEngine/MonoBehaviour.hpp"
#include "HMUI/ImageView.hpp"
#include "HMUI/CurvedTextMeshPro.hpp"
#include "GlobalNamespace/IConnectedPlayer.hpp"
#include "UnityEngine/Sprite.hpp"
#include <map>

DECLARE_CLASS_CODEGEN(MultiQuestensions::Environments, LobbyAvatarNameTag, UnityEngine::MonoBehaviour,
    enum PlayerIconSlot {
        Platform = 0
    };

    bool _enabled;
    DECLARE_INSTANCE_FIELD(GlobalNamespace::IConnectedPlayer*, _playerInfo);
    std::map<PlayerIconSlot, HMUI::ImageView*> _playerIcons;

    DECLARE_INSTANCE_FIELD(HMUI::ImageView*, _bg);
    DECLARE_INSTANCE_FIELD(HMUI::CurvedTextMeshPro*, _nameText);

    DECLARE_CTOR(New);

    DECLARE_INSTANCE_METHOD(void, Awake);
    DECLARE_INSTANCE_METHOD(void, OnEnable);

    DECLARE_INSTANCE_METHOD(void, SetPlayerInfo, GlobalNamespace::IConnectedPlayer* player);
    DECLARE_INSTANCE_METHOD(void, SetExtendedPlayerInfo, MultiQuestensions::Extensions::ExtendedPlayer* extendedPlayer);
    DECLARE_INSTANCE_METHOD(void, SetSimplePlayerInfo, GlobalNamespace::IConnectedPlayer* simplePlayer);

    void SetIcon(PlayerIconSlot slot, UnityEngine::Sprite* sprite);
    void RemoveIcon(PlayerIconSlot slot);
)