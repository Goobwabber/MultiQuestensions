#pragma once
#include "main.hpp"
#include "HMUI/CurvedTextMeshPro.hpp"
#include "UnityEngine/UI/Toggle.hpp"
#include "UnityEngine/RectTransform.hpp"

namespace MultiQuestensions::UI {
	class HostLobbySetupPanel {
	private:
		//static GlobalNamespace::IMultiplayerSessionManager* sessionManager;
		//HMUI::CurvedTextMeshPro* modifierText;
		static UnityEngine::UI::Toggle* customSongsToggle;

	public:
		static void AddSetupPanel(UnityEngine::RectTransform* parent, GlobalNamespace::MultiplayerSessionManager* sessionManager);
	};
	void SetCustomSongs(bool value);
}