#include "main.hpp"
#include "UI/LobbySetupPanel.hpp"
#include "questui/shared/BeatSaberUI.hpp"
#include "UnityEngine/UI/ContentSizeFitter.hpp"
#include "UnityEngine/UI/LayoutElement.hpp"
#include "UnityEngine/RectOffset.hpp"

#include "CodegenExtensions/ColorUtility.hpp"
#include "GlobalFieldsTemp.hpp"
#include "songloader/shared/API.hpp"
using namespace UnityEngine::UI;

namespace MultiQuestensions::UI {

	UnityEngine::UI::Toggle* LobbySetupPanel::lagReducerToggle;

	void SetLagReducer(bool value) {
		getConfig().config["lagreducer"].SetBool(value);
	}

	void LobbySetupPanel::AddSetupPanel(UnityEngine::RectTransform* parent, GlobalNamespace::MultiplayerSessionManager* sessionManager) {
		auto vertical = QuestUI::BeatSaberUI::CreateVerticalLayoutGroup(parent);

		auto horizontal = QuestUI::BeatSaberUI::CreateHorizontalLayoutGroup(vertical->get_transform());
		horizontal->set_padding(UnityEngine::RectOffset::New_ctor(20, 0, 18, 0)); // 100, 0, 18, 0

		auto vertical2 = QuestUI::BeatSaberUI::CreateVerticalLayoutGroup(horizontal->get_transform());
		vertical2->get_gameObject()->AddComponent<ContentSizeFitter*>()
			->set_horizontalFit(UnityEngine::UI::ContentSizeFitter::FitMode::MinSize);
		vertical2->get_gameObject()->AddComponent<LayoutElement*>()
			->set_minWidth(45);

		auto vertical3 = QuestUI::BeatSaberUI::CreateVerticalLayoutGroup(horizontal->get_transform());
		vertical3->get_gameObject()->AddComponent<ContentSizeFitter*>()
			->set_horizontalFit(UnityEngine::UI::ContentSizeFitter::FitMode::MinSize);
		vertical3->get_gameObject()->AddComponent<LayoutElement*>()->set_minWidth(30);

		auto vertical4 = QuestUI::BeatSaberUI::CreateVerticalLayoutGroup(horizontal->get_transform());
		vertical4->get_gameObject()->AddComponent<ContentSizeFitter*>()
			->set_horizontalFit(UnityEngine::UI::ContentSizeFitter::FitMode::MinSize);
		vertical4->get_gameObject()->AddComponent<LayoutElement*>()
			->set_minWidth(45);

		// <toggle-setting id="LagReducerToggle" value='LagReducer' on-change='SetLagReducer' text='Lag Reducer' hover-hint='Makes multiplayer easier for computers to handle.'></toggle-setting>

		QuestUI::BeatSaberUI::CreateText(vertical4->get_transform(), "THESE TOGGLES ARE JUST\r\nPLACEHOLDERS!");

		lagReducerToggle = QuestUI::BeatSaberUI::CreateToggle(vertical4->get_transform(), "Lag Reducer", getConfig().config["lagreducer"].GetBool(), SetLagReducer);
		QuestUI::BeatSaberUI::AddHoverHint(lagReducerToggle->get_gameObject(), "Makes multiplayer easier for the quest to handle.");

		UnityEngine::Color playerColor;
		UnityEngine::ColorUtility::TryParseHtmlString(getConfig().config["color"].GetString(), playerColor);

		//QuestUI::BeatSaberUI::CreateColorPickerModal(parent->get_transform(), "Player Color Selection", playerColor);

		auto colorPicker = QuestUI::BeatSaberUI::CreateColorPickerModal(parent, "Player Color Selection", playerColor,
			[&playerColor](UnityEngine::Color value) {
				playerColor = value;
				getConfig().config["color"].SetString(UnityEngine::ColorUtility::ToHtmlStringRGB_CPP(value), getConfig().config.GetAllocator());
				getConfig().Write();
				try {
					localExtendedPlayer->playerColor = value;
					Extensions::ExtendedPlayerPacket* localPlayerPacket = Extensions::ExtendedPlayerPacket::Init(localExtendedPlayer->get_platformID(), localExtendedPlayer->get_platform(), localExtendedPlayer->get_playerColor());
					getLogger().debug("LocalPlayer Color is, R: %f G: %f B: %f", localPlayerPacket->playerColor.r, localPlayerPacket->playerColor.g, localPlayerPacket->playerColor.b);
					packetManager->Send(reinterpret_cast<LiteNetLib::Utils::INetSerializable*>(localPlayerPacket));
				}
				catch (const std::runtime_error& e) {
					getLogger().error("%s", e.what());
				}
			},
			[] {
				try {
					Extensions::ExtendedPlayerPacket* localPlayerPacket = Extensions::ExtendedPlayerPacket::Init(localExtendedPlayer->get_platformID(), localExtendedPlayer->get_platform(), localExtendedPlayer->get_playerColor());
					getLogger().debug("LocalPlayer Color is, R: %f G: %f B: %f", localPlayerPacket->playerColor.r, localPlayerPacket->playerColor.g, localPlayerPacket->playerColor.b);
					packetManager->Send(reinterpret_cast<LiteNetLib::Utils::INetSerializable*>(localPlayerPacket));
				}
				catch (const std::runtime_error& e) {
					getLogger().error("%s", e.what());
				}

				//instance->SetColor(getModConfig().ClockColor.GetValue());
				//getModConfig().ClockColor.SetValue(lastChangedColor);
			},
			[](UnityEngine::Color value) {
				// TODO: Uncomment when MpEx supports live platform color updates
				
				//try {
				//	Extensions::ExtendedPlayerPacket* localPlayerPacket = Extensions::ExtendedPlayerPacket::Init(localExtendedPlayer->get_platformID(), localExtendedPlayer->get_platform(), value);
				//	getLogger().debug("LocalPlayer Color is, R: %f G: %f B: %f", localPlayerPacket->playerColor.r, localPlayerPacket->playerColor.g, localPlayerPacket->playerColor.b);
				//	packetManager->Send(reinterpret_cast<LiteNetLib::Utils::INetSerializable*>(localPlayerPacket));
				//}
				//catch (const std::runtime_error& e) {
				//	getLogger().error("%s", e.what());
				//}
			}
		);

		auto deleteDownloadedSongs = QuestUI::BeatSaberUI::CreateUIButton(vertical2->get_transform(), "Delete Downloaded", [] {
			using namespace RuntimeSongLoader::API;
			bool needRefresh = false;
			for (auto hash : DownloadedSongIds) {
				auto level = GetLevelByHash(hash);
				if (level.has_value()) {
					std::string songPath = to_utf8(csstrtostr(level.value()->customLevelPath));
					getLogger().debug("Deleting Song: %s", songPath.c_str());
					DeleteSong(songPath, [&needRefresh] {
						if (needRefresh) RefreshSongs(false);
						}
					);
				}
			}
			needRefresh = true;
			DownloadedSongIds.clear();
			}
		);
		QuestUI::BeatSaberUI::AddHoverHint(deleteDownloadedSongs->get_gameObject(), "Deletes all automatically downloaded songs during multiplayer sessions since game launch.");


		QuestUI::BeatSaberUI::CreateUIButton(vertical2->get_transform(), "Color", [colorPicker] {
			colorPicker->Show();
			}
		);

		//customSongsToggle = QuestUI::BeatSaberUI::CreateToggle(vertical2->get_transform(), "Custom Songs", getConfig().config["customsongs"].GetBool(), SetCustomSongs);
		//QuestUI::BeatSaberUI::AddHoverHint(customSongsToggle->get_gameObject(), "Toggles custom songs for all players");
	}
}