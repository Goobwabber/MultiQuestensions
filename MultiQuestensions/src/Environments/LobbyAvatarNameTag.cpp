#include "Environments/LobbyAvatarNameTag.hpp"
#include "Assets/Sprites.hpp"
#include "UnityEngine/CanvasRenderer.hpp"
#include "UnityEngine/RectTransform.hpp"
#include "UnityEngine/UI/HorizontalLayoutGroup.hpp"
#include "GlobalNamespace/ConnectedPlayerName.hpp"
#include "UnityEngine/GameObject.hpp"
using MultiQuestensions::Extensions::ExtendedPlayer;
using namespace GlobalNamespace;
using namespace UnityEngine::UI;
using namespace UnityEngine;
using namespace HMUI;

DEFINE_TYPE(MultiQuestensions::Environments, LobbyAvatarNameTag);

namespace MultiQuestensions::Environments {
    static Il2CppString* BG() {
        static auto* bg = il2cpp_utils::newcsstr<il2cpp_utils::CreationType::Manual>("BG");
        return bg;
    }

    static Il2CppString* Name() {
        static auto* name = il2cpp_utils::newcsstr<il2cpp_utils::CreationType::Manual>("BG");
        return name;
    }

    static Il2CppString* Player() {
        static auto* player = il2cpp_utils::newcsstr<il2cpp_utils::CreationType::Manual>("Player");
        return player;
    }


    void LobbyAvatarNameTag::New() {
        _enabled = false;
        _playerInfo = nullptr;
        _playerIcons.clear();
    }

    void LobbyAvatarNameTag::Awake() {
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
            GameObject::Destroy(nativeNameScript);
        _nameText->set_text(Player());
    }

    void LobbyAvatarNameTag::OnEnable()
    {
        _enabled = true;

        if (_playerInfo != nullptr)
            SetPlayerInfo(_playerInfo);
    }

    #pragma region Set Player Info
    void LobbyAvatarNameTag::SetPlayerInfo(IConnectedPlayer* player)
    {
        if (il2cpp_utils::AssignableFrom<MultiQuestensions::Extensions::ExtendedPlayer*>(reinterpret_cast<Il2CppObject*>(player)->klass))
            SetExtendedPlayerInfo(reinterpret_cast<MultiQuestensions::Extensions::ExtendedPlayer*>(player));
        else
            SetSimplePlayerInfo(player);
    }

    void LobbyAvatarNameTag::SetExtendedPlayerInfo(ExtendedPlayer* extendedPlayer)
    {
        _playerInfo = extendedPlayer;

        if (!_enabled)
            return;

        _nameText->set_text(extendedPlayer->get_userName());
        _nameText->set_color(extendedPlayer->get_playerColor());

        switch (extendedPlayer->get_platform())
        {
        case Extensions::Platform::Steam:
            SetIcon(PlayerIconSlot::Platform, Sprites::IconSteam64());
            break;
        case Extensions::Platform::OculusQuest:
        case Extensions::Platform::OculusPC:
            SetIcon(PlayerIconSlot::Platform, Sprites::IconOculus64());
            break;
        default:
            RemoveIcon(PlayerIconSlot::Platform);
            break;
        }
    }

    void LobbyAvatarNameTag::SetSimplePlayerInfo(IConnectedPlayer* simplePlayer)
    {
        _playerInfo = simplePlayer;

        if (!_enabled)
            return;

        _nameText->set_text(simplePlayer->get_userName());
        _nameText->set_color(Color::get_white());

        RemoveIcon(PlayerIconSlot::Platform);
    }
    #pragma endregion

    #pragma region Set Icons
    void LobbyAvatarNameTag::SetIcon(PlayerIconSlot slot, Sprite* sprite)
    {
        if (!_enabled)
            return;

        HMUI::ImageView* imageView;
        if (!_playerIcons[slot])
        {
            auto iconObj = GameObject::New_ctor(il2cpp_utils::newcsstr(string_format("MpExPlayerIcon(%d)", (int)slot)));
            iconObj->get_transform()->SetParent(_bg->get_transform(), false);
            iconObj->get_transform()->SetSiblingIndex((int)slot);
            iconObj->set_layer(5);

            iconObj->AddComponent<CanvasRenderer*>();

            imageView = iconObj->AddComponent<ImageView*>();
            imageView->set_maskable(true);
            imageView->set_fillCenter(true);
            imageView->set_preserveAspect(true);
            imageView->set_material(_bg->get_material()); // No Glow Billboard material
            _playerIcons[slot] = imageView;

            auto rectTransform = iconObj->GetComponent<RectTransform*>();
            rectTransform->set_localScale({ 3.2f, 3.2f });
        }
        else imageView = _playerIcons[slot];

        imageView->set_sprite(sprite);

        _nameText->get_transform()->SetSiblingIndex(999);
    }

    void LobbyAvatarNameTag::RemoveIcon(PlayerIconSlot slot)
    {
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