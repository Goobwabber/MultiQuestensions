#pragma once
#include "main.hpp"
#include "HMUI/CurvedTextMeshPro.hpp"
#include "UnityEngine/UI/Toggle.hpp"
#include "UnityEngine/RectTransform.hpp"
#include "GlobalNamespace/LobbySetupViewController.hpp"

namespace MultiQuestensions::UI {
	class LobbySetupPanel {
	private:
		//static GlobalNamespace::IMultiplayerSessionManager* sessionManager;
		//HMUI::CurvedTextMeshPro* modifierText;
		static UnityEngine::UI::Toggle* lagReducerToggle;
		//static UnityEngine::UI::Toggle* customSongsToggle;
		static bool needRefresh;

	public:
		static void AddSetupPanel(UnityEngine::RectTransform* parent, GlobalNamespace::MultiplayerSessionManager* sessionManager, GlobalNamespace::LobbySetupViewController* lobbySetupViewController);
	};
	//void SetCustomSongs(bool value);
}