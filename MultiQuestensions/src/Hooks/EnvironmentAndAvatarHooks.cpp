#include "main.hpp"
#include "Hooks/Hooks.hpp"
#include "GlobalFieldsTemp.hpp"
#include "Environments/LobbyAvatarPlaceLighting.hpp"
#include "Environments/LobbyAvatarNameTag.hpp"

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

#include "System/Collections/Generic/List_1.hpp"

#include "UnityEngine/Resources.hpp"
#include "UnityEngine/Transform.hpp"

using namespace GlobalNamespace;
using namespace UnityEngine;
using namespace MultiQuestensions::Environments;
using namespace MultiQuestensions::Extensions;
using namespace System::Collections::Generic;

namespace MultiQuestensions {

#pragma region Fields
	ILobbyStateDataModel* _lobbyStateDataModel;
	MenuEnvironmentManager* _menuEnvironmentManager;
	MultiplayerLobbyAvatarPlaceManager* _placeManager;
	MultiplayerLobbyCenterStageManager* _stageManager;

	std::vector<LobbyAvatarPlaceLighting*> avatarPlaces;

	float innerCircleRadius;
	float minOuterCircleRadius;
	float angleBetweenPlayersWithEvenAdjustment;
	float outerCircleRadius;

	bool initialized;

	MultiplayerLobbyAvatarManager* _avatarManager;

	Dictionary_2<Il2CppString*, MultiplayerLobbyAvatarController*>* _refPlayerIdToAvatarMap;
#pragma endregion

#pragma region LobbyAvatarPlaceLigting
	LobbyAvatarPlaceLighting* GetConnectedPlayerPlace(IConnectedPlayer* player)
	{
		getLogger().debug("GetConnectedPlayerPlace");
		int sortIndex = _lobbyStateDataModel->get_localPlayer()->get_sortIndex();
		getLogger().debug("GetConnectedPlayerPlace sortIndex %d, angleBetweenPlayersWithEvenAdjustment %f", sortIndex, angleBetweenPlayersWithEvenAdjustment);
		//static auto* sortIndexMethod = THROW_UNLESS(il2cpp_utils::FindMethodUnsafe(classof(IConnectedPlayer*), "get_sortIndex", 0));
		//int playerSortIndex = il2cpp_utils::RunMethodThrow<int>(player, sortIndexMethod);
		float outerCirclePositionAngleForPlayer = MultiplayerPlayerPlacement::GetOuterCirclePositionAngleForPlayer(player->get_sortIndex(), sortIndex, angleBetweenPlayersWithEvenAdjustment);
		getLogger().debug("GetConnectedPlayerPlace outerCirclePositionAngleForPlayer %f", outerCirclePositionAngleForPlayer);
		Vector3 playerWorldPosition = MultiplayerPlayerPlacement::GetPlayerWorldPosition(outerCircleRadius, outerCirclePositionAngleForPlayer, MultiplayerPlayerLayout::Circle);
		for (auto* place : avatarPlaces) {
			if (place->get_transform()->get_position() == playerWorldPosition && place->get_isActiveAndEnabled()) return place;
		}
		return nullptr;
		//return Array::Find(_avatarPlaces, place => place.transform.position == playerWorldPosition && place.isActiveAndEnabled);
	}

	void SetPlayerPlaceColor(IConnectedPlayer* player, const Color& color, bool priority)
	{
		if (!initialized) return;
		getLogger().debug("SetPlayerPlaceColor");

		LobbyAvatarPlaceLighting* place = GetConnectedPlayerPlace(player);

		if (place == nullptr)
			return;

		if (!priority && place->TargetColor != Color::get_black() && place->TargetColor != ExtendedPlayer::DefaultColor)
			// Priority colors are always set; non-priority colors can only override default black/blue
			return;

		place->SetColor(color, false);
	}

	void SetAllPlayerPlaceColors(Color color, bool immediate = false)
	{
		for (LobbyAvatarPlaceLighting* place : avatarPlaces)
		{
			place->SetColor(color, immediate);
		}
	}

	static void SetDefaultPlayerPlaceColors()
	{
		SetAllPlayerPlaceColors(Color::get_black(), true);
		SetPlayerPlaceColor(sessionManager->get_localPlayer(), localExtendedPlayer->get_playerColor(), true);
		using System::Collections::Generic::List_1;
		for (int i = 0; i < reinterpret_cast<List_1<IConnectedPlayer*>*>(sessionManager->get_connectedPlayers())->get_Count(); i++) {
			auto player = sessionManager->get_connectedPlayers()->get_Item(i);
			SetPlayerPlaceColor(player, ExtendedPlayer::DefaultColor, false);
		}

		for (auto& [key, extendedPlayer] : _extendedPlayers) {
			SetPlayerPlaceColor(reinterpret_cast<IConnectedPlayer*>(extendedPlayer), extendedPlayer->get_playerColor(), true);
		}
	}

	void HandleLobbyEnvironmentLoaded() {
		initialized = false;
		getLogger().debug("HandleLobbyEnvironmentLoaded Started");
		auto* nativeAvatarPlaces = Resources::FindObjectsOfTypeAll<MultiplayerLobbyAvatarPlace*>();
		for (int i = 0; i < nativeAvatarPlaces->Length(); i++)
		{
			auto* nativeAvatarPlace = nativeAvatarPlaces->values[i];

			auto avatarPlace = nativeAvatarPlace->GetComponent<LobbyAvatarPlaceLighting*>();
			if (avatarPlace == nullptr)
				avatarPlace = nativeAvatarPlace->get_gameObject()->AddComponent<LobbyAvatarPlaceLighting*>();

			avatarPlaces.push_back(avatarPlace);
		}

		innerCircleRadius = _placeManager->dyn__innerCircleRadius();
		minOuterCircleRadius = _placeManager->dyn__minOuterCircleRadius();
		getLogger().debug("innerCircleRadius %f, minOuterCircleRadius %f", innerCircleRadius, minOuterCircleRadius);
		angleBetweenPlayersWithEvenAdjustment = MultiplayerPlayerPlacement::GetAngleBetweenPlayersWithEvenAdjustment(_lobbyStateDataModel->get_maxPartySize(), MultiplayerPlayerLayout::Circle);
		outerCircleRadius = fmax(MultiplayerPlayerPlacement::GetOuterCircleRadius(angleBetweenPlayersWithEvenAdjustment, innerCircleRadius), minOuterCircleRadius);
		getLogger().debug("angleBetweenPlayersWithEvenAdjustment %f, outerCircleRadius %f", angleBetweenPlayersWithEvenAdjustment, outerCircleRadius);

		bool buildingsEnabled = (sessionManager->dyn__maxPlayerCount() <= 18);
		auto* Construction_tr = _menuEnvironmentManager->get_transform()->Find(il2cpp_utils::newcsstr("Construction"));
		if (Construction_tr && Construction_tr->get_gameObject()) {
			Construction_tr->get_gameObject()->SetActive(buildingsEnabled);
		}

		auto* Construction_1_tr = _menuEnvironmentManager->get_transform()->Find(il2cpp_utils::newcsstr("Construction (1)"));
		if (Construction_1_tr && Construction_1_tr->get_gameObject()) {
			Construction_1_tr->get_gameObject()->SetActive(buildingsEnabled);
		}

		float centerScreenScale = outerCircleRadius / minOuterCircleRadius;
		_stageManager->get_transform()->set_localScale({ centerScreenScale, centerScreenScale, centerScreenScale });

		initialized = true;
		SetDefaultPlayerPlaceColors();
		getLogger().debug("HandleLobbyEnvironmentLoaded Finished");
	}

    MAKE_HOOK_MATCH(MultiplayerLobbyController_ActivateMultiplayerLobby, &MultiplayerLobbyController::ActivateMultiplayerLobby, void, MultiplayerLobbyController* self) {
		if (!_placeManager) _placeManager = Resources::FindObjectsOfTypeAll<MultiplayerLobbyAvatarPlaceManager*>()->values[0];
		if (!_menuEnvironmentManager) _menuEnvironmentManager = Resources::FindObjectsOfTypeAll<MenuEnvironmentManager*>()->values[0];
		if (!_stageManager) _stageManager = Resources::FindObjectsOfTypeAll<MultiplayerLobbyCenterStageManager*>()->values[0];
		if (!_lobbyStateDataModel)  _lobbyStateDataModel = _placeManager->lobbyStateDataModel;

        self->dyn__innerCircleRadius() = 1;
        self->dyn__minOuterCircleRadius() = 4.4f;
        MultiplayerLobbyController_ActivateMultiplayerLobby(self);

		HandleLobbyEnvironmentLoaded();
    }

    MAKE_HOOK_MATCH(LightWithIdMonoBehaviour_RegisterLight, &LightWithIdMonoBehaviour::RegisterLight, void, LightWithIdMonoBehaviour* self) {
		if (!(self->get_transform()->get_parent() != nullptr && self->get_transform()->get_parent()->get_name()->Contains(il2cpp_utils::newcsstr("LobbyAvatarPlace")))) LightWithIdMonoBehaviour_RegisterLight(self);
	}

#pragma endregion

#pragma region LobbyAvatarNameTag
	MultiplayerLobbyAvatarController* GetAvatarController(Il2CppString* userId)
	{
		getLogger().debug("Start GetAvatarController: _refPlayerIdToAvatarMap");
		if (_refPlayerIdToAvatarMap == nullptr && _avatarManager)
			_refPlayerIdToAvatarMap = _avatarManager->dyn__playerIdToAvatarMap();

		getLogger().debug("Start GetAvatarController: _refPlayerIdToAvatarMap Done");

		if (_refPlayerIdToAvatarMap != nullptr) {
			MultiplayerLobbyAvatarController* value;
			getLogger().debug("Start GetAvatarController return MultiplayerLobbyAvatarController");
			return _refPlayerIdToAvatarMap->TryGetValue(userId, ByRef(value)) ? value : nullptr;
		}

		getLogger().debug("Start GetAvatarController return nullptr");

		return nullptr;
	}

	GameObject* GetAvatarCaptionObject(Il2CppString* userId)
	{
		auto avatarController = GetAvatarController(userId);
		if (avatarController) return avatarController->get_transform()->Find(il2cpp_utils::newcsstr("AvatarCaption"))->get_gameObject();
		else return nullptr;
	}

	void CreateOrUpdateNameTag(IConnectedPlayer* player)
	{
		getLogger().debug("Start CreateOrUpdateNameTag: GetAvatarCaptionObject");
		auto objAvatarCaption = GetAvatarCaptionObject(player->get_userId());
		if (objAvatarCaption == nullptr)
			return;

		getLogger().debug("Found GetAvatarCaptionObject");
		LobbyAvatarNameTag* nameTag;
		if (!objAvatarCaption->TryGetComponent(ByRef(nameTag)))
			nameTag = objAvatarCaption->AddComponent<LobbyAvatarNameTag*>();

		getLogger().debug("SetPlayerInfo");
		nameTag->SetPlayerInfo(player);
	}

	void HandleLobbyAvatarCreated(IConnectedPlayer* player) {
		const std::string userId = to_utf8(csstrtostr(player->get_userId()));
		if (_extendedPlayers.contains(userId))
			player = reinterpret_cast<IConnectedPlayer*>(_extendedPlayers.at(userId));
		CreateOrUpdateNameTag(player);
	}

    MAKE_HOOK_MATCH(MultiplayerLobbyAvatarManager_AddPlayer, &MultiplayerLobbyAvatarManager::AddPlayer, void, MultiplayerLobbyAvatarManager* self, IConnectedPlayer* connectedPlayer) {
        MultiplayerLobbyAvatarManager_AddPlayer(self, connectedPlayer);
        // TODO: Raise Event for when Lobby Avatar has been created
		if (!_avatarManager) _avatarManager = self;
		HandleLobbyAvatarCreated(connectedPlayer);
    }

#pragma endregion

    void Hooks::EnvironmentHooks() {
        INSTALL_HOOK(getLogger(), MultiplayerLobbyController_ActivateMultiplayerLobby);
        INSTALL_HOOK(getLogger(), LightWithIdMonoBehaviour_RegisterLight);
        INSTALL_HOOK(getLogger(), MultiplayerLobbyAvatarManager_AddPlayer);
    }
}