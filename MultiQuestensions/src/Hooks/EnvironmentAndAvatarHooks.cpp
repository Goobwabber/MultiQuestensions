#include "main.hpp"
#include "Hooks/Hooks.hpp"
#include "GlobalFields.hpp"
#include "multiplayer-core/shared/GlobalFields.hpp"
#include "Hooks/EnvironmentAndAvatarHooks.hpp"
#include "Hooks/SessionManagerAndExtendedPlayerHooks.hpp"
#include "Environments/MQEAvatarPlaceLighting.hpp"
#include "Environments/MQEAvatarNameTag.hpp"
#include "Players/MpexPlayerManager.hpp"
#include "Config.hpp"
#include "multiplayer-core/shared/Players/MpPlayerManager.hpp"
#include "multiplayer-core/shared/Utils/EnumUtils.hpp"

#include "GlobalNamespace/MultiplayerLobbyController.hpp"
#include "GlobalNamespace/LightWithIdMonoBehaviour.hpp"
#include "GlobalNamespace/MultiplayerLobbyAvatarManager.hpp"
#include "GlobalNamespace/MultiplayerLobbyAvatarPlace.hpp"
#include "GlobalNamespace/MultiplayerPlayerPlacement.hpp"
#include "GlobalNamespace/MultiplayerPlayerLayout.hpp"
#include "GlobalNamespace/ILobbyStateDataModel.hpp"
#include "GlobalNamespace/MultiplayerLobbyAvatarPlaceManager.hpp"
#include "GlobalNamespace/MenuEnvironmentManager.hpp"
#include "GlobalNamespace/MultiplayerLobbyCenterStageManager.hpp"
#include "GlobalNamespace/MultiplayerLobbyAvatarManager.hpp"
#include "GlobalNamespace/IConnectedPlayer.hpp"
#include "GlobalNamespace/ConnectedPlayerManager_ConnectedPlayer.hpp"
#include "GlobalNamespace/GameplayServerConfiguration.hpp"
#include "GlobalNamespace/MultiplayerGameplayAnimator.hpp"
#include "GlobalNamespace/MultiplayerController.hpp"
#include "GlobalNamespace/SimpleColorSO.hpp"
#include "GlobalNamespace/ConnectedPlayerHelpers.hpp"
#include "GlobalNamespace/LightsAnimator.hpp"
#include "GlobalNamespace/EaseType.hpp"

#include "Tweening/Tween_1.hpp"
#include "Tweening/ColorTween.hpp"

#include "System/Collections/Generic/List_1.hpp"

#include "UnityEngine/Resources.hpp"
#include "UnityEngine/Transform.hpp"

using namespace GlobalNamespace;
using namespace UnityEngine;
using namespace MultiQuestensions::Environments;
using namespace System::Collections::Generic;

namespace MultiQuestensions {

#pragma region Fields
    std::vector<MQEAvatarPlaceLighting*> avatarPlaces;

    float innerCircleRadius;
    float minOuterCircleRadius;
    float angleBetweenPlayersWithEvenAdjustment;
    float outerCircleRadius;

    bool initialized;

    MultiplayerLobbyAvatarManager* _avatarManager;

    Dictionary_2<StringW, MultiplayerLobbyAvatarController*>* _refPlayerIdToAvatarMap;
#pragma endregion

#pragma region LobbyAvatarPlaceLigting
    MQEAvatarPlaceLighting* GetConnectedPlayerPlace(IConnectedPlayer* player)
    {
        //getLogger().debug("GetConnectedPlayerPlace, lighting");
        int sortIndex = MultiplayerCore::_multiplayerSessionManager->get_localPlayer()->get_sortIndex();
        //getLogger().debug("GetConnectedPlayerPlace sortIndex %d, angleBetweenPlayersWithEvenAdjustment %f", sortIndex, angleBetweenPlayersWithEvenAdjustment);
        //static auto* sortIndexMethod = THROW_UNLESS(il2cpp_utils::FindMethodUnsafe(classof(IConnectedPlayer*), "get_sortIndex", 0));
        //int playerSortIndex = il2cpp_utils::RunMethodThrow<int>(player, sortIndexMethod);
        float outerCirclePositionAngleForPlayer = MultiplayerPlayerPlacement::GetOuterCirclePositionAngleForPlayer(player->get_sortIndex(), sortIndex, angleBetweenPlayersWithEvenAdjustment);
        //getLogger().debug("GetConnectedPlayerPlace outerCirclePositionAngleForPlayer %f", outerCirclePositionAngleForPlayer);
        Vector3 playerWorldPosition = MultiplayerPlayerPlacement::GetPlayerWorldPosition(outerCircleRadius, outerCirclePositionAngleForPlayer, MultiplayerPlayerLayout::Circle);
        for (auto* place : avatarPlaces) {
            if (place->get_transform()->get_position() == playerWorldPosition && place->get_isActiveAndEnabled()) return place;
        }
        //getLogger().debug("GetConnectedPlayerPlace, lighting is NULLptr");
        return nullptr;
        //return Array::Find(_avatarPlaces, place => place.transform.position == playerWorldPosition && place.isActiveAndEnabled);
    }

    void SetPlayerPlaceColor(IConnectedPlayer* player, const Color& color, bool priority)
    {
        //getLogger().debug("SetPlayerPlaceColor");
        if (!initialized) return;

        MQEAvatarPlaceLighting* place = GetConnectedPlayerPlace(player);

        if (place == nullptr){
            //getLogger().debug("Place was null, returning");
            return;
        }

        // getLogger().debug("SetPlayerPlaceColor player %s userId %s", static_cast<std::string>(player->get_userName()).c_str(), static_cast<std::string>(player->get_userId()).c_str());

        // getLogger().debug("SetPlayerPlaceColor new TargetColor %f, %f, %f, %f", color.r, color.g, color.b, color.a);

        // getLogger().debug("SetPlayerPlaceColor current TargetColor %f, %f, %f, %f", place->targetColor.r, place->targetColor.g, place->targetColor.b, place->targetColor.a);

        if (!priority && place->targetColor != Color::get_black() && place->targetColor != Config::DefaultPlayerColor) {
            //getLogger().debug("Default color, returning");
            // getLogger().debug("SetPlayerPlaceColor not priority and TargetColor check is priority '%s' check is black '%s', check is DefaultPlayerColor '%s' check full eval '%s'", !priority ?  "true" : "false", place->targetColor != Color::get_black() ? "true" : "false", place->targetColor != Config::DefaultPlayerColor ? "true" : "false", (!priority && place->targetColor != Color::get_black() && place->targetColor != Config::DefaultPlayerColor) ? "true" : "false");
            
            // getLogger().debug("SetPlayerPlaceColor TargetColor %f, %f, %f, %f", place->targetColor.r, place->targetColor.g, place->targetColor.b, place->targetColor.a);
            
            // getLogger().debug("SetPlayerPlaceColor black %f, %f, %f, %f", Color::get_black().r, Color::get_black().g, Color::get_black().b, Color::get_black().a);
            
            // getLogger().debug("SetPlayerPlaceColor DefaultPlayerColor %f, %f, %f, %f", Config::DefaultPlayerColor.r, Config::DefaultPlayerColor.g, Config::DefaultPlayerColor.b, Config::DefaultPlayerColor.a);
            // Priority colors are always set; non-priority colors can only override default black/blue
            return;
        }

        //getLogger().debug("SetPlayerPlaceColor setting color");
        place->SetColor(color, false);
    }

    void SetAllPlayerPlaceColors(Color color, bool immediate = false)
    {
        //getLogger().debug("SetAllPlayerPlaceColors");
        for (MQEAvatarPlaceLighting* place : avatarPlaces)
        {
            place->SetColor(color, immediate);
        }
    }

    static void SetDefaultPlayerPlaceColors()
    {
        SetAllPlayerPlaceColors(Color::get_black(), true);
        //getLogger().debug("SetDefaultPlayerPlaceColors, set local player color");
        SetPlayerPlaceColor(MultiplayerCore::_multiplayerSessionManager->get_localPlayer(), config.getPlayerColor(), true);
        using System::Collections::Generic::List_1;
        for (int i = 0; i < reinterpret_cast<List_1<IConnectedPlayer*>*>(MultiplayerCore::_multiplayerSessionManager->get_connectedPlayers())->get_Count(); i++) {
            auto player = MultiplayerCore::_multiplayerSessionManager->get_connectedPlayers()->get_Item(i);
            std::string userId = player->get_userId();
            // If user is us, we skip
            if (userId == static_cast<std::string>(MultiplayerCore::_multiplayerSessionManager->get_localPlayer()->get_userId())) continue;
            if (_mpexPlayerData.contains(userId)) {
                SetPlayerPlaceColor(player, _mpexPlayerData.at(userId)->Color, true);
            } else {
                SetPlayerPlaceColor(player, Config::DefaultPlayerColor, true);
            }
        }
    }

    void HandleLobbyEnvironmentLoaded(ILobbyStateDataModel* _lobbyStateDataModel, MenuEnvironmentManager* _menuEnvironmentManager, MultiplayerLobbyAvatarPlaceManager* _placeManager, MultiplayerLobbyCenterStageManager* _stageManager) {
        initialized = false;
        getLogger().debug("HandleLobbyEnvironmentLoaded start");
        auto nativeAvatarPlaces = Resources::FindObjectsOfTypeAll<MultiplayerLobbyAvatarPlace*>();
        for (int i = 0; i < nativeAvatarPlaces.Length(); i++)
        {
            auto nativeAvatarPlace = nativeAvatarPlaces[i];

            auto avatarPlace = nativeAvatarPlace->GetComponent<MQEAvatarPlaceLighting*>();
            if (avatarPlace == nullptr)
                avatarPlace = nativeAvatarPlace->get_gameObject()->AddComponent<MQEAvatarPlaceLighting*>();

            avatarPlaces.push_back(avatarPlace);
        }

        innerCircleRadius = _placeManager->innerCircleRadius;
        minOuterCircleRadius = _placeManager->minOuterCircleRadius;

        angleBetweenPlayersWithEvenAdjustment = MultiplayerPlayerPlacement::GetAngleBetweenPlayersWithEvenAdjustment(_lobbyStateDataModel->get_configuration().maxPlayerCount, MultiplayerPlayerLayout::Circle);
        outerCircleRadius = fmax(MultiplayerPlayerPlacement::GetOuterCircleRadius(angleBetweenPlayersWithEvenAdjustment, innerCircleRadius), minOuterCircleRadius);

        initialized = true;
        SetDefaultPlayerPlaceColors();
        getLogger().debug("HandleLobbyEnvironmentLoaded Finished");
    }

    MAKE_HOOK_MATCH(MultiplayerLobbyController_ActivateMultiplayerLobby, &MultiplayerLobbyController::ActivateMultiplayerLobby, void, MultiplayerLobbyController* self) {
        getLogger().debug("MultiplayerLobbyController::ActivateMultiplayerLobby");
        MultiplayerLobbyAvatarPlaceManager* _placeManager = self->multiplayerLobbyAvatarPlaceManager;
        MenuEnvironmentManager* _menuEnvironmentManager = self->menuEnvironmentManager;
        MultiplayerLobbyCenterStageManager* _stageManager = self->multiplayerLobbyCenterStageManager;
        ILobbyStateDataModel* _lobbyStateDataModel = _placeManager->lobbyStateDataModel;

        MultiplayerLobbyController_ActivateMultiplayerLobby(self);

        HandleLobbyEnvironmentLoaded(_lobbyStateDataModel, _menuEnvironmentManager, _placeManager, _stageManager);
    }

    MAKE_HOOK_MATCH(LightWithIdMonoBehaviour_RegisterLight, &LightWithIdMonoBehaviour::RegisterLight, void, LightWithIdMonoBehaviour* self) {
        if ((self && self->get_transform() && self->get_transform()->get_parent() && self->get_transform()->get_parent()->get_name()->Contains("LobbyAvatarPlace"))){
            if(!self->isRegistered){
                //getLogger().debug("MQE registering a MQEAvatarPlaceLighting or light that has the lobby avatar place as a parent");
                LightWithIdMonoBehaviour_RegisterLight(self);
            }
        }
        else{
            LightWithIdMonoBehaviour_RegisterLight(self);
        }
    }
#pragma endregion

#pragma region LobbyAvatarNameTag
    MultiplayerLobbyAvatarController* GetAvatarController(StringW userId)
    {
        //getLogger().debug("Start GetAvatarController: _refPlayerIdToAvatarMap");
        if (_refPlayerIdToAvatarMap == nullptr && _avatarManager)
            _refPlayerIdToAvatarMap = _avatarManager->playerIdToAvatarMap;

        //getLogger().debug("Start GetAvatarController: _refPlayerIdToAvatarMap Done");

        if (_refPlayerIdToAvatarMap != nullptr) {
            MultiplayerLobbyAvatarController* value;
            //getLogger().debug("Start GetAvatarController return MultiplayerLobbyAvatarController");
            return _refPlayerIdToAvatarMap->TryGetValue(userId, byref(value)) ? value : nullptr;
        }

        getLogger().debug("GetAvatarController return nullptr, this part should never be triggered");

        return nullptr;
    }

    GameObject* GetAvatarCaptionObject(StringW userId)
    {
        auto avatarController = GetAvatarController(userId);
        if (avatarController) return avatarController->get_transform()->Find("AvatarCaption")->get_gameObject();
        else return nullptr;
    }

    void HandleLobbyAvatarCreated(IConnectedPlayer* player) {
        auto objAvatarCaption = GetAvatarCaptionObject(player->get_userId());
        //getLogger().debug("Finding GetAvatarCaptionObject");
        if (objAvatarCaption == nullptr)
            return;

        MQEAvatarNameTag* nameTag;
        if (!objAvatarCaption->TryGetComponent<MQEAvatarNameTag*>(byref(nameTag))) {
            getLogger().debug("Adding new LobbyAvatarNameTag Component");
            nameTag = objAvatarCaption->AddComponent<MQEAvatarNameTag*>();
        }
        nameTag->SetPlayerInfo(player);
    }

    void UpdateNameTagIcons() {
        getLogger().debug("Start UpdateNameTagIcons");
        for (int i = 0; i < MultiplayerCore::_multiplayerSessionManager->connectedPlayers->get_Count(); i++) {
            auto player = MultiplayerCore::_multiplayerSessionManager->connectedPlayers->get_Item(i);
            auto objAvatarCaption = GetAvatarCaptionObject(player->get_userId());
            getLogger().debug("Finding GetAvatarCaptionObject");
            if (objAvatarCaption == nullptr) {
                //getLogger().debug("GetAvatarCaptionObject is nullptr, returning");
                continue;
            }

            //getLogger().debug("Found GetAvatarCaptionObject");
            MQEAvatarNameTag* nameTag;
            if (objAvatarCaption->TryGetComponent<MQEAvatarNameTag*>(byref(nameTag))) {
                MultiplayerCore::Players::MpPlayerData* mpPlayerData;
                if (MultiplayerCore::Players::MpPlayerManager::TryGetMpPlayerData(player->get_userId(), mpPlayerData)) {
                    nameTag->SetPlatformData(mpPlayerData);
                } else {
                    getLogger().debug("UpdateNameTagIcons: mpPlayerData is nullptr");
                }
            }
        }
    }

    MAKE_HOOK_MATCH(MultiplayerLobbyAvatarManager_AddPlayer, &MultiplayerLobbyAvatarManager::AddPlayer, void, MultiplayerLobbyAvatarManager* self, IConnectedPlayer* connectedPlayer) {
        //getLogger().debug("MultiplayerLobbyAvatarManager::AddPlayer");
        MultiplayerLobbyAvatarManager_AddPlayer(self, connectedPlayer);
        _avatarManager = self;
        HandleLobbyAvatarCreated(connectedPlayer);
    }

#pragma region Lighting hooks

    // Utilities for working with colors
    ColorSO* getColorSO(UnityEngine::Color color) {
        SimpleColorSO* colorSO = UnityEngine::ScriptableObject::CreateInstance<SimpleColorSO*>();
        colorSO->SetColor(color);
        return colorSO;
    }

    MAKE_HOOK_MATCH(MultiplayerGameplayAnimator_HandleStateChanged, &MultiplayerGameplayAnimator::HandleStateChanged, void, MultiplayerGameplayAnimator* self, MultiplayerController::State state) {
        getLogger().debug("MultiplayerGameplayAnimator::HandleStateChanged: Checking State %s", MultiplayerCore::EnumUtils::GetEnumName(state).c_str());
        getLogger().debug("MultiplayerGameplayAnimator::HandleStateChanged: Checking Player IsFailed %s", ConnectedPlayerHelpers::IsFailed(self->connectedPlayer) ? "true" : "false");

        static bool skippedFirst = false;

        if (!ConnectedPlayerHelpers::IsFailed(self->connectedPlayer)) {
            getLogger().debug("MultiplayerGameplayAnimator::HandleStateChanged: Player is not Failed");
            if (self && self->connectedPlayer && self->connectedPlayer->get_isMe()) {
                if (state == MultiplayerController::State::Gameplay)
                {
                    getLogger().debug("MultiplayerGameplayAnimator::HandleStateChanged: State is Gameplay and Player is not Failed (Me)");
                    self->activeLightsColor = getColorSO(config.getPlayerColor());
                    self->leadingLightsColor = getColorSO(config.getPlayerColor());
                    // self->failedLightsColor = getColorSO(config.getPlayerColor());
                    skippedFirst = false;
                }
                else if (state == MultiplayerController::State::Intro)
                {   
                    getLogger().debug("MultiplayerGameplayAnimator::HandleStateChanged: State is Intro and Player is not Failed (Me)");
                    // if (skippedFirst)
                    // {
                        // TODO: Figure out how to animate the lights to the new color without the old value mixing itself in
                        for(LightsAnimator* lightsAnimator : self->allLightsAnimators)
                        {
                            // lightsAnimator->tween->set_delay(1.0f);
                            // lightsAnimator->tween->fromValue = {0, 0, 0, 0};
                            // lightsAnimator->tween->toValue = config.getPlayerColor();
                            lightsAnimator->AnimateToColor(config.getPlayerColor() * 1.1, 3, EaseType::Linear);
                        }
                    // }
                    // else skippedFirst = true;

                }
            }
            else if (self && self->connectedPlayer) {
                StringW userId = self->connectedPlayer->get_userId();
                if (userId && _mpexPlayerData.contains(userId)) {
                    if (state == MultiplayerController::State::Gameplay)
                    {
                        getLogger().debug("MultiplayerGameplayAnimator::HandleStateChanged: State is Gameplay and Player is not Failed (Other Player)");
                        self->activeLightsColor = getColorSO(_mpexPlayerData.at(userId)->Color);
                        self->leadingLightsColor = getColorSO(_mpexPlayerData.at(userId)->Color);
                        // self->failedLightsColor = getColorSO(_mpexPlayerData.at(userId)->Color);
                        skippedFirst = false;
                    }
                    else if (state == MultiplayerController::State::Intro)
                    {
                        getLogger().debug("MultiplayerGameplayAnimator::HandleStateChanged: State is Intro and Player is not Failed (Other Player)");
                        // if (skippedFirst)
                        // {
                            // TODO: Figure out how to animate the lights to the new color without the old value mixing itself in
                            for(LightsAnimator* lightsAnimator : self->allLightsAnimators)
                            {
                                // lightsAnimator->tween->set_delay(1.0f);
                                // lightsAnimator->tween->fromValue = {0, 0, 0, 0};
                                // lightsAnimator->tween->toValue = _mpexPlayerData.at(userId)->Color;
                                lightsAnimator->AnimateToColor(_mpexPlayerData.at(userId)->Color * 1.1, 3, EaseType::Linear);
                            }
                        // }
                        // else skippedFirst = true;
                    }
                }
            }
        }
        MultiplayerGameplayAnimator_HandleStateChanged(self, state);
    }

#pragma endregion

    void Hooks::EnvironmentHooks() {
        INSTALL_HOOK(getLogger(), MultiplayerLobbyController_ActivateMultiplayerLobby);
        INSTALL_HOOK(getLogger(), LightWithIdMonoBehaviour_RegisterLight);
        INSTALL_HOOK(getLogger(), MultiplayerLobbyAvatarManager_AddPlayer);
        INSTALL_HOOK(getLogger(), MultiplayerGameplayAnimator_HandleStateChanged);
    }
}