#include "main.hpp"
#include "Config.hpp"
#include "Environments/MQEAvatarNameTag.hpp"
#include "Assets/Sprites.hpp"
#include "Hooks/SessionManagerAndExtendedPlayerHooks.hpp"
#include "UnityEngine/CanvasRenderer.hpp"
#include "UnityEngine/RectTransform.hpp"
#include "UnityEngine/UI/HorizontalLayoutGroup.hpp"
#include "GlobalNamespace/ConnectedPlayerName.hpp"
#include "UnityEngine/Object.hpp"
#include "Players/MpexPlayerManager.hpp"
#include "Players/MpPlayerData.hpp"
#include "Utils/event.hpp"

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
        //getLogger().debug("MQEAvatarNameTag::New()");
        _enabled = false;
        _playerInfo = nullptr;
        _playerIcons.clear();
    }

    void MQEAvatarNameTag::Awake() {
        //getLogger().debug("MQEAvatarNameTag::Awake()");
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
            hLayout->set_spacing(8);
        }

        // Re-nest name onto bg
        _nameText->get_transform()->SetParent(_bg->get_transform(), false);

        // Take control of name tag
        ConnectedPlayerName* nativeNameScript;
        if (_nameText->TryGetComponent<ConnectedPlayerName*>(byref(nativeNameScript)))
            Object::Destroy(nativeNameScript);
        _nameText->set_text(Player());

        // Create Event handlers
        _platformDataEventHandler = event_handler<IConnectedPlayer*, Players::MpPlayerData*>(
            [this](GlobalNamespace::IConnectedPlayer* player, Players::MpPlayerData* data) {
                HandlePlatformData(player, data);
        });
        _mpexPlayerDataEventHandler = event_handler<IConnectedPlayer*, Players::MpexPlayerData*>(
            [this](GlobalNamespace::IConnectedPlayer* player, Players::MpexPlayerData* data) {
                HandleMpexData(player, data);
        });
    }

    void MQEAvatarNameTag::OnEnable()
    {
        //getLogger().debug("MQEAvatarNameTag::OnEnable()");
        _enabled = true;

        // Subscribe to events
        Players::MpexPlayerManager::RecievedPlayerData += _platformDataEventHandler;
        Players::MpexPlayerManager::RecievedMpExPlayerData += _mpexPlayerDataEventHandler;

        // Set player info
        if (_playerInfo != nullptr)
            SetPlayerInfo(_playerInfo);
    }

    void MQEAvatarNameTag::OnDisable()
    {
        //getLogger().debug("MQEAvatarNameTag::OnDisable()");
        _enabled = false;

        // Unsubscribe from events
        Players::MpexPlayerManager::RecievedPlayerData -= _platformDataEventHandler;
        Players::MpexPlayerManager::RecievedMpExPlayerData -= _mpexPlayerDataEventHandler;
    }

    #pragma region Set Player Info
    void MQEAvatarNameTag::SetPlayerInfo(IConnectedPlayer* player)
    {
        //getLogger().debug("MQEAvatarNameTag::SetPlayerInfo");
        _playerInfo = player;

        if (!_enabled)
            return;

        _nameText->set_text(player->get_userName());
        std::string userId = static_cast<std::string>(player->get_userId());
        if (_mpexPlayerData.contains(userId))
            _nameText->set_color(_mpexPlayerData.at(userId)->Color);

        RemoveIcon(PlayerIconSlot::Platform);
        
        Players::MpPlayerData* data;
        if (Players::MpexPlayerManager::TryGetMpPlayerData(userId, data))
            SetPlatformData(data);
    }

    void MQEAvatarNameTag::SetPlatformData(Players::MpPlayerData* data) {
        //getLogger().debug("MQEAvatarNameTag::SetPlatformData");
        using Players::Platform;
        switch (data->platform)
        {
            case Platform::Steam:
                SetIcon(PlayerIconSlot::Platform, Sprites::IconSteam64());
                break;
            case Platform::OculusQuest:
                if (!config.getNoMetaZone()) {
                    SetIcon(PlayerIconSlot::Platform, Sprites::IconMeta64());
                    break;
                }
            case Platform::OculusPC:
                SetIcon(PlayerIconSlot::Platform, Sprites::IconOculus64());
                break;
            case Platform::Unknown:
            case Platform::PS4:
            default:
                break;
        }
    }

    void MQEAvatarNameTag::SetSimplePlayerInfo(IConnectedPlayer* simplePlayer)
    {
        //getLogger().debug("SetSimplePlayerInfo");
        _playerInfo = simplePlayer;

        if (!_enabled)
            return;
        
        SetPlayerInfo(simplePlayer);
    }

    using Players::MpPlayerData;
    void MQEAvatarNameTag::HandlePlatformData(IConnectedPlayer* player, MpPlayerData* data) {
        getLogger().debug("HandlePlatformData");
        if (data && player && _playerInfo && player->get_userId() == _playerInfo->get_userId())
            SetPlatformData(data);
    }

    using Players::MpexPlayerData;
    void MQEAvatarNameTag::HandleMpexData(IConnectedPlayer* player, MpexPlayerData* data) {
        getLogger().debug("HandleMpexData");
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
            getLogger().debug("SetIcon, create new Icon");
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
        //getLogger().debug("MQEAvatarNameTag::RemoveIcon");
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