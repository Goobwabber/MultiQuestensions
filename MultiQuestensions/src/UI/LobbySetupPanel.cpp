#include "main.hpp"
#include "UI/LobbySetupPanel.hpp"
#include "questui/shared/BeatSaberUI.hpp"
#include "UnityEngine/UI/ContentSizeFitter.hpp"
#include "UnityEngine/UI/LayoutElement.hpp"
#include "UnityEngine/RectOffset.hpp"
using namespace UnityEngine::UI;

namespace MultiQuestensions::UI {

	UnityEngine::UI::Toggle* LobbySetupPanel::customSongsToggle;

	void SetCustomSongs(bool value) {
		getConfig().config["customsongs"].SetBool(value);
		customSongsEnabled = value;
	}

	void LobbySetupPanel::AddSetupPanel(UnityEngine::RectTransform* parent, GlobalNamespace::MultiplayerSessionManager* sessionManager) {
		auto vertical = QuestUI::BeatSaberUI::CreateVerticalLayoutGroup(parent);

		auto horizontal = QuestUI::BeatSaberUI::CreateHorizontalLayoutGroup(vertical->get_transform());
		horizontal->set_padding(UnityEngine::RectOffset::New_ctor(100, 0, 18, 0));

		auto vertical2 = QuestUI::BeatSaberUI::CreateVerticalLayoutGroup(horizontal->get_transform());
		auto sizeFitter = vertical2->get_gameObject()->AddComponent<ContentSizeFitter*>();
		sizeFitter->set_horizontalFit(UnityEngine::UI::ContentSizeFitter::FitMode::MinSize);
		auto layout = vertical2->get_gameObject()->AddComponent<LayoutElement*>();
		layout->set_minWidth(45);

		//auto vertical3 = QuestUI::BeatSaberUI::CreateVerticalLayoutGroup(vertical2->get_transform());
		//auto sizeFitter1 = vertical3->get_gameObject()->AddComponent<ContentSizeFitter*>();
		//sizeFitter1->set_horizontalFit(UnityEngine::UI::ContentSizeFitter::FitMode::MinSize);
		//auto layout1 = vertical3->get_gameObject()->AddComponent<LayoutElement*>();
		//layout1->set_minWidth(45);

		//auto vertical4 = QuestUI::BeatSaberUI::CreateVerticalLayoutGroup(horizontal->get_transform());
		//auto sizeFitter2 = vertical4->get_gameObject()->AddComponent<ContentSizeFitter*>();
		//sizeFitter2->set_horizontalFit(UnityEngine::UI::ContentSizeFitter::FitMode::MinSize);
		//auto layout2 = vertical4->get_gameObject()->AddComponent<LayoutElement*>();
		//layout2->set_minWidth(45);

		// <toggle-setting id="LagReducerToggle" value='LagReducer' on-change='SetLagReducer' text='Lag Reducer' hover-hint='Makes multiplayer easier for computers to handle.'></toggle-setting>

		customSongsToggle = QuestUI::BeatSaberUI::CreateToggle(vertical2->get_transform(), "Custom Songs", getConfig().config["customsongs"].GetBool(), SetCustomSongs);
		QuestUI::BeatSaberUI::AddHoverHint(customSongsToggle->get_gameObject(), "Toggles custom songs for all players");
	}
}