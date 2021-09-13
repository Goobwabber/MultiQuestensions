#include "main.hpp"
#include "UI/HostLobbySetupPanel.hpp"
#include "questui/shared/BeatSaberUI.hpp"

namespace MultiQuestensions::UI {

	UnityEngine::UI::Toggle* HostLobbySetupPanel::customSongsToggle;

	void SetCustomSongs(bool value) {
		getConfig().config["customsongs"].SetBool(value);
		customSongsEnabled = value;
	}

	void HostLobbySetupPanel::AddSetupPanel(UnityEngine::RectTransform* parent, GlobalNamespace::MultiplayerSessionManager* sessionManager) {
		customSongsToggle = QuestUI::BeatSaberUI::CreateToggle(parent, "Custom Songs", getConfig().config["customsongs"].GetBool(), SetCustomSongs);
		QuestUI::BeatSaberUI::AddHoverHint(customSongsToggle->get_gameObject(), "Toggles custom songs for all players");
	}
}