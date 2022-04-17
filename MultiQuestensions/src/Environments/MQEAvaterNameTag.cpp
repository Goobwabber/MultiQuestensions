#include "main.hpp"
#include "Environments/MQEAvatarNameTag.hpp"
#include "Assets/Sprites.hpp"
#include "Hooks/SessionManagerAndExtendedPlayerHooks.hpp"
#include "UnityEngine/CanvasRenderer.hpp"
#include "UnityEngine/RectTransform.hpp"
#include "UnityEngine/UI/HorizontalLayoutGroup.hpp"
#include "GlobalNamespace/ConnectedPlayerName.hpp"
#include "UnityEngine/Object.hpp"
#include "Players/MpexPlayerManager.hpp"
#include "MultiplayerCore/shared/Players/MpPlayerData.hpp"
using namespace GlobalNamespace;
using namespace UnityEngine::UI;
using namespace UnityEngine;
using namespace HMUI;

DEFINE_TYPE(MultiQuestensions::Environments, MQEAvatarNameTag);

namespace MultiQuestensions::Environments {
    static StringW BG() {
        static ConstString bg("BG");
        return bg;
    }

    static StringW Name() {
        static ConstString name("Name");
        return name;
    }

    static StringW Player() {
        static ConstString player("Player");
        return player;
    }


    void MQEAvatarNameTag::New() {
        _enabled = false;
        _playerInfo = nullptr;
        _playerIcons.clear();
    }

    void MQEAvatarNameTag::Awake() {
        // Get references
        _bg = get_transform()->Find(BG())->GetComponent<ImageView*>();
        _nameText = get_transform()->Find(Name())->GetComponent<CurvedTextMeshPro*>();

        // Add horizontal layout to bg
        HorizontalLayoutGroup* _;
        if (!_bg->TryGetComponent<HorizontalLayoutGroup*>(ByRef(_)))
        {
            auto hLayout = _bg->get_gameObject()->AddComponent<HorizontalLayoutGroup*>();
            hLayout->set_childAlignment(TextAnchor::MiddleCenter);
            hLayout->set_childForceExpandWidth(false);
            hLayout->set_childForceExpandHeight(false);
            hLayout->set_childScaleWidth(false);
            hLayout->set_childScaleHeight(false);
            hLayout->set_spacing(4);
        }

        // Re-nest name onto bg
        _nameText->get_transform()->SetParent(_bg->get_transform(), false);

        // Take control of name tag
        ConnectedPlayerName* nativeNameScript;
        if (_nameText->TryGetComponent<ConnectedPlayerName*>(ByRef(nativeNameScript)))
            Object::Destroy(nativeNameScript);
        _nameText->set_text(Player());

        // Create Event handlers
        _platformDataEventHandler = MultiplayerCore::event_handler<IConnectedPlayer*, MultiplayerCore::Players::MpPlayerData*>(
            [this](GlobalNamespace::IConnectedPlayer* player, MultiplayerCore::Players::MpPlayerData* data) {
            if (data && player && _playerInfo && player->get_userId() == _playerInfo->get_userId())
                SetPlatformData(data);
        });
        _mpexPlayerDataEventHandler = MultiplayerCore::event_handler<IConnectedPlayer*, MultiQuestensions::Players::MpexPlayerData*>(
            [this](GlobalNamespace::IConnectedPlayer* player, MultiQuestensions::Players::MpexPlayerData* data) {
            if (_nameText && data && player && _playerInfo && player->get_userId() == _playerInfo->get_userId())
                _nameText->set_color(data->Color);
        });
    }

    void MQEAvatarNameTag::OnEnable()
    {
        _enabled = true;

        // Subscribe to events
        MultiplayerCore::Players::MpPlayerManager::PlayerConnected += _platformDataEventHandler;
        MultiQuestensions::Players::MpexPlayerManager::PlayerConnected += _mpexPlayerDataEventHandler;

        // Set player info
        if (_playerInfo != nullptr)
            SetPlayerInfo(_playerInfo);
    }

    void MQEAvatarNameTag::OnDisable()
    {
        _enabled = false;

        // Unsubscribe from events
        MultiplayerCore::Players::MpPlayerManager::PlayerConnected -= _platformDataEventHandler;
        MultiQuestensions::Players::MpexPlayerManager::PlayerConnected -= _mpexPlayerDataEventHandler;
    }

    #pragma region Set Player Info
    void MQEAvatarNameTag::SetPlayerInfo(IConnectedPlayer* player)
    {
        _playerInfo = player;

        if (!_enabled)
            return;

        _nameText->set_text(player->get_userName());
        std::string userName = static_cast<std::string>(player->get_userName());
        if (_mpexPlayerData.contains(userName))
            _nameText->set_color(_mpexPlayerData.at(userName)->Color);

        RemoveIcon(PlayerIconSlot::Platform);
        
        MultiplayerCore::Players::MpPlayerData* data;
        std::string userId = static_cast<std::string>(player->get_userId());
        if (MultiplayerCore::Players::MpPlayerManager::TryGetPlayer(userId, data))
            SetPlatformData(data);
        // if (il2cpp_utils::AssignableFrom<MultiQuestensions::Extensions::ExtendedPlayer*>(reinterpret_cast<Il2CppObject*>(player)->klass))
        //     SetExtendedPlayerInfo(reinterpret_cast<MultiQuestensions::Extensions::ExtendedPlayer*>(player));
        // else
            // SetSimplePlayerInfo(player);
    }

    void MQEAvatarNameTag::SetPlatformData(MultiplayerCore::Players::MpPlayerData* data) {
        using MultiplayerCore::Players::Platform;
        switch (data->platform)
        {
            case Platform::Steam:
                SetIcon(PlayerIconSlot::Platform, Sprites::IconSteam64());
                break;
            case Platform::OculusQuest:
                SetIcon(PlayerIconSlot::Platform, Sprites::IconMeta64());
                break;
            case Platform::OculusPC:
                SetIcon(PlayerIconSlot::Platform, Sprites::IconOculus64());
                break;
            case Platform::Unknown:
            case Platform::PS4:
            default:
                break;
        }
    }

    // void LobbyAvatarNameTag::SetExtendedPlayerInfo(ExtendedPlayer* extendedPlayer)
    // {
    //     _playerInfo = reinterpret_cast<IConnectedPlayer*>(extendedPlayer);

    //     if (!_enabled)
    //         return;

    //     _nameText->set_text(extendedPlayer->get_userName());
    //     _nameText->set_color(extendedPlayer->get_playerColor());

    //     //getLogger().debug("SetExtendedPlayerInfo platform: %d", (int)extendedPlayer->get_platform());

    //     switch (extendedPlayer->get_platform())
    //     {
    //     case Extensions::Platform::Steam:
    //         SetIcon(PlayerIconSlot::Platform, Sprites::IconSteam64());
    //         break;
    //     case Extensions::Platform::OculusQuest:
    //     case Extensions::Platform::OculusPC:
    //         SetIcon(PlayerIconSlot::Platform, Sprites::IconOculus64());
    //         break;
    //     default:
    //         RemoveIcon(PlayerIconSlot::Platform);
    //         break;
    //     }
    // }

    void MQEAvatarNameTag::SetSimplePlayerInfo(IConnectedPlayer* simplePlayer)
    {
        _playerInfo = simplePlayer;

        if (!_enabled)
            return;

        _nameText->set_text(simplePlayer->get_userName());
        _nameText->set_color(Color::get_white());

        RemoveIcon(PlayerIconSlot::Platform);
    }

    using MultiplayerCore::Players::MpPlayerData;
   void MQEAvatarNameTag::HandlePlatformData(IConnectedPlayer* player, MpPlayerData* data) {
        if (data && player && _playerInfo && player->get_userId() == _playerInfo->get_userId())
            SetPlatformData(data);
    }

    using MultiQuestensions::Players::MpexPlayerData;
    void MQEAvatarNameTag::HandleMpexData(IConnectedPlayer* player, MpexPlayerData* data) {
        if (data && player && _playerInfo && player->get_userId() == _playerInfo->get_userId())
            _nameText->set_color(data->Color);
    }
    #pragma endregion

    #pragma region Set Icons
    void MQEAvatarNameTag::SetIcon(PlayerIconSlot slot, Sprite* sprite)
    {
        getLogger().debug("SetIcon Sprite %p", sprite);

        if (!_enabled)
            return;

        HMUI::ImageView* imageView;
        if (!_playerIcons.contains(slot))
        {
            //getLogger().debug("SetIcon, create new Icon");
            auto iconObj = GameObject::New_ctor(string_format("MQEPlayerIcon(%d)", (int)slot).c_str());
            iconObj->get_transform()->SetParent(_bg->get_transform(), false);
            iconObj->get_transform()->SetSiblingIndex((int)slot);
            iconObj->set_layer(5);

            iconObj->AddComponent<CanvasRenderer*>();

            imageView = iconObj->AddComponent<ImageView*>();
            imageView->set_maskable(true);
            imageView->set_fillCenter(true);
            imageView->set_preserveAspect(true);
            imageView->set_material(_bg->get_material()); // No Glow Billboard material
            _playerIcons.insert_or_assign(slot, imageView);

            auto rectTransform = iconObj->GetComponent<RectTransform*>();
            rectTransform->set_localScale({ 3.2f, 3.2f, 0.0f });
        }
        else imageView = _playerIcons.at(slot);

        imageView->set_sprite(sprite);

        _nameText->get_transform()->SetSiblingIndex(999);
    }

    void MQEAvatarNameTag::RemoveIcon(PlayerIconSlot slot)
    {
        //getLogger().debug("RemoveIcon");

        if (!_enabled)
            return;
        if (_playerIcons.contains(slot))
        {
            HMUI::ImageView* imageView = _playerIcons[slot];
            GameObject::Destroy(imageView->get_gameObject());
            _playerIcons.erase(slot);
        }
    }
    #pragma endregion
}