#include "main.hpp"
#include "Hooks/Hooks.hpp"
#include "GlobalFields.hpp"
#include "Hooks/EnvironmentAndAvatarHooks.hpp"
#include "Hooks/SessionManagerAndExtendedPlayerHooks.hpp"
#include "Environments/MQEAvatarPlaceLighting.hpp"
#include "Environments/MQEAvatarNameTag.hpp"
#include "Players/MpexPlayerManager.hpp"
#include "Config.hpp"

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

#include "System/Collections/Generic/List_1.hpp"

#include "UnityEngine/Resources.hpp"
#include "UnityEngine/Transform.hpp"

using namespace GlobalNamespace;
using namespace UnityEngine;
using namespace MultiQuestensions::Environments;
using namespace System::Collections::Generic;

namespace MultiQuestensions {

#pragma region Fields
    ILobbyStateDataModel* _lobbyStateDataModel;
    MenuEnvironmentManager* _menuEnvironmentManager;
    MultiplayerLobbyAvatarPlaceManager* _placeManager;
    MultiplayerLobbyCenterStageManager* _stageManager;

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
        int sortIndex = _lobbyStateDataModel->get_localPlayer()->get_sortIndex();
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
        SetPlayerPlaceColor(sessionManager->get_localPlayer(), config.getPlayerColor(), true);
        using System::Collections::Generic::List_1;
        for (int i = 0; i < reinterpret_cast<List_1<IConnectedPlayer*>*>(sessionManager->get_connectedPlayers())->get_Count(); i++) {
            auto player = sessionManager->get_connectedPlayers()->get_Item(i);
            std::string userId = player->get_userId();
            if (userId == static_cast<std::string>(sessionManager->get_localPlayer()->get_userId())) continue;
            //getLogger().debug("SetDefaultPlayerPlaceColors, set player color for userId %s", userId.c_str());
            if (_mpexPlayerData.contains(userId)) {
                //getLogger().debug("SetDefaultPlayerPlaceColors, found MpexPlayerData setting color for player %s", userId.c_str());
                SetPlayerPlaceColor(player, _mpexPlayerData.at(userId)->Color, true);
            } else {
                //getLogger().debug("SetDefaultPlayerPlaceColors, did not find MpexPlayerData setting default color for player %s", userId.c_str());
                SetPlayerPlaceColor(player, Config::DefaultPlayerColor, true);
            }
        }

        // for (auto& [key, extendedPlayer] : _mpexPlayerData) {
        //     SetPlayerPlaceColor(reinterpret_cast<IConnectedPlayer*>(extendedPlayer->get_self()), extendedPlayer->get_playerColor(), true);
        // }
    }

    void HandleLobbyEnvironmentLoaded() {
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

        innerCircleRadius = _placeManager->dyn__innerCircleRadius();
        minOuterCircleRadius = _placeManager->dyn__minOuterCircleRadius();

        angleBetweenPlayersWithEvenAdjustment = MultiplayerPlayerPlacement::GetAngleBetweenPlayersWithEvenAdjustment(_lobbyStateDataModel->get_configuration().maxPlayerCount, MultiplayerPlayerLayout::Circle);
        outerCircleRadius = fmax(MultiplayerPlayerPlacement::GetOuterCircleRadius(angleBetweenPlayersWithEvenAdjustment, innerCircleRadius), minOuterCircleRadius);

        initialized = true;
        SetDefaultPlayerPlaceColors();
        getLogger().debug("HandleLobbyEnvironmentLoaded Finished");
    }

    MAKE_HOOK_MATCH(MultiplayerLobbyController_ActivateMultiplayerLobby, &MultiplayerLobbyController::ActivateMultiplayerLobby, void, MultiplayerLobbyController* self) {
        getLogger().debug("MultiplayerLobbyController::ActivateMultiplayerLobby");
        _placeManager = Resources::FindObjectsOfTypeAll<MultiplayerLobbyAvatarPlaceManager*>()[0];
        _menuEnvironmentManager = Resources::FindObjectsOfTypeAll<MenuEnvironmentManager*>()[0];
        _stageManager = Resources::FindObjectsOfTypeAll<MultiplayerLobbyCenterStageManager*>()[0];
        _lobbyStateDataModel = _placeManager->dyn__lobbyStateDataModel();

        MultiplayerLobbyController_ActivateMultiplayerLobby(self);

        HandleLobbyEnvironmentLoaded();
    }

    MAKE_HOOK_MATCH(LightWithIdMonoBehaviour_RegisterLight, &LightWithIdMonoBehaviour::RegisterLight, void, LightWithIdMonoBehaviour* self) {
        if ((self && self->get_transform() && self->get_transform()->get_parent() && self->get_transform()->get_parent()->get_name()->Contains("LobbyAvatarPlace"))){
            if(!self->dyn__isRegistered()){
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
            _refPlayerIdToAvatarMap = _avatarManager->dyn__playerIdToAvatarMap();

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
        for (int i = 0; i < MultiplayerCore::_multiplayerSessionManager->dyn__connectedPlayers()->get_Count(); i++) {
            auto player = MultiplayerCore::_multiplayerSessionManager->dyn__connectedPlayers()->get_Item(i);
            auto objAvatarCaption = GetAvatarCaptionObject(player->get_userId());
            getLogger().debug("Finding GetAvatarCaptionObject");
            if (objAvatarCaption == nullptr) {
                //getLogger().debug("GetAvatarCaptionObject is nullptr, returning");
                continue;
            }

            //getLogger().debug("Found GetAvatarCaptionObject");
            MQEAvatarNameTag* nameTag;
            if (objAvatarCaption->TryGetComponent<MQEAvatarNameTag*>(byref(nameTag))) {
                MultiQuestensions::Players::MpPlayerData* mpPlayerData;
                if (MultiQuestensions::Players::MpexPlayerManager::TryGetMpPlayerData(player->get_userId(), mpPlayerData)) {
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

#pragma endregion

    void Hooks::EnvironmentHooks() {
        INSTALL_HOOK(getLogger(), MultiplayerLobbyController_ActivateMultiplayerLobby);
        INSTALL_HOOK(getLogger(), LightWithIdMonoBehaviour_RegisterLight);
        INSTALL_HOOK(getLogger(), MultiplayerLobbyAvatarManager_AddPlayer);
    }
}