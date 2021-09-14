#include "main.hpp"
#include "UI/HostLobbySetupPanel.hpp"
#include "questui/shared/BeatSaberUI.hpp"
#include "UnityEngine/UI/ContentSizeFitter.hpp"
#include "UnityEngine/UI/LayoutElement.hpp"
#include "UnityEngine/RectOffset.hpp"
using namespace UnityEngine::UI;

namespace MultiQuestensions::UI {

	UnityEngine::UI::Toggle* HostLobbySetupPanel::customSongsToggle;

	void SetCustomSongs(bool value) {
		getConfig().config["customsongs"].SetBool(value);
		customSongsEnabled = value;
	}

	void HostLobbySetupPanel::AddSetupPanel(UnityEngine::RectTransform* parent, GlobalNamespace::MultiplayerSessionManager* sessionManager) {
		auto vertical = QuestUI::BeatSaberUI::CreateVerticalLayoutGroup(parent);

		auto horizontal = QuestUI::BeatSaberUI::CreateHorizontalLayoutGroup(vertical->get_transform());
		horizontal->set_padding(UnityEngine::RectOffset::New_ctor(0, 0, 12, 0));

		auto vertical2 = QuestUI::BeatSaberUI::CreateVerticalLayoutGroup(horizontal->get_transform());
		auto sizeFitter = vertical2->get_gameObject()->AddComponent<ContentSizeFitter*>();
		sizeFitter->set_horizontalFit(UnityEngine::UI::ContentSizeFitter::FitMode::MinSize);
		auto layout = vertical2->get_gameObject()->AddComponent<LayoutElement*>();
		layout->set_minWidth(45);

		customSongsToggle = QuestUI::BeatSaberUI::CreateToggle(vertical2->get_transform(), "Custom Songs", getConfig().config["customsongs"].GetBool(), SetCustomSongs);
		QuestUI::BeatSaberUI::AddHoverHint(customSongsToggle->get_gameObject(), "Toggles custom songs for all players");
	}
}