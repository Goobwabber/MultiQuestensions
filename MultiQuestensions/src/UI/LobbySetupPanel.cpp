#include "main.hpp"
#include "UI/LobbySetupPanel.hpp"
#include "questui/shared/BeatSaberUI.hpp"
#include "UnityEngine/UI/ContentSizeFitter.hpp"
#include "UnityEngine/UI/LayoutElement.hpp"
#include "UnityEngine/RectOffset.hpp"

#include "CodegenExtensions/ColorUtility.hpp"
#include "GlobalFields.hpp"
#include "Hooks/SessionManagerAndExtendedPlayerHooks.hpp"
#include "Hooks/EnvironmentAndAvatarHooks.hpp"
#include "songloader/shared/API.hpp"
#include "UI/DownloadedSongsGSM.hpp"
#include "Utils/SemVerChecker.hpp"
using namespace UnityEngine::UI;

namespace MultiQuestensions::UI {

	UnityEngine::UI::Toggle* LobbySetupPanel::lagReducerToggle;
	bool LobbySetupPanel::needRefresh;

	void SetLagReducer(bool value) {
		getConfig().config["LagReducer"].SetBool(value);
		getConfig().Write();
	}

	void LobbySetupPanel::AddSetupPanel(UnityEngine::RectTransform* parent, GlobalNamespace::MultiplayerSessionManager* sessionManager) {
		auto vertical = QuestUI::BeatSaberUI::CreateVerticalLayoutGroup(parent);

		auto horizontal = QuestUI::BeatSaberUI::CreateHorizontalLayoutGroup(vertical->get_transform());
		horizontal->set_padding(UnityEngine::RectOffset::New_ctor(20, 0, 18, 0)); // 100, 0, 18, 0 // For Toggle on Right Side 20, 0, 18, 0 // No Offset 0, 0, 18, 0

		auto vertical2 = QuestUI::BeatSaberUI::CreateVerticalLayoutGroup(horizontal->get_transform());
		vertical2->get_gameObject()->AddComponent<ContentSizeFitter*>()
			->set_horizontalFit(UnityEngine::UI::ContentSizeFitter::FitMode::MinSize);
		vertical2->get_gameObject()->AddComponent<LayoutElement*>()
			->set_minWidth(45);

		auto vertical3 = QuestUI::BeatSaberUI::CreateVerticalLayoutGroup(horizontal->get_transform());
		vertical3->get_gameObject()->AddComponent<ContentSizeFitter*>()
			->set_horizontalFit(UnityEngine::UI::ContentSizeFitter::FitMode::MinSize);
		vertical3->get_gameObject()->AddComponent<LayoutElement*>()->set_minWidth(20); // Default 45 // For Toggle on Right Side 20

		auto vertical4 = QuestUI::BeatSaberUI::CreateVerticalLayoutGroup(horizontal->get_transform());
		vertical4->get_gameObject()->AddComponent<ContentSizeFitter*>()
			->set_horizontalFit(UnityEngine::UI::ContentSizeFitter::FitMode::MinSize);
		vertical4->get_gameObject()->AddComponent<LayoutElement*>()
			->set_minWidth(45);

		using namespace MultiQuestensions::Utils;
		if (IsInstalled(ChromaID) && !MatchesVersion(ChromaID, ChromaVersionRange)) {
			//HMUI::ModalView* modal = QuestUI::BeatSaberUI::CreateModal(parent, { 55, 25 }, std::nullptr_t());
			//auto wrapper = QuestUI::BeatSaberUI::CreateHorizontalLayoutGroup(modal->get_transform());
			//auto container = QuestUI::BeatSaberUI::CreateVerticalLayoutGroup(wrapper->get_transform());
			//container->set_childAlignment(UnityEngine::TextAnchor::MiddleCenter);
			//QuestUI::BeatSaberUI::CreateText(modal->get_transform(), "Chroma detected!\r\nChroma may cause issues such as crashes,\r\nif you're experiencing issues like these,\r\nthen it may be best to try disabling Chroma")->set_alignment(TMPro::TextAlignmentOptions::Center);
			//modal->Show(true, true, nullptr);
			QuestUI::BeatSaberUI::CreateText(vertical2->get_transform(),
				"Chroma outdated!\r\nPlease update to the latest version of Chroma.",
				{ -40, 0 })->set_alignment(TMPro::TextAlignmentOptions::Left);
			getLogger().warning("Chroma outdated");
		}
		else getLogger().debug("Chroma not installed or version incompatible");


		// <toggle-setting id="LagReducerToggle" value='LagReducer' on-change='SetLagReducer' text='Lag Reducer' hover-hint='Makes multiplayer easier for computers to handle.'></toggle-setting>

		//QuestUI::BeatSaberUI::CreateText(vertical4->get_transform(), "THESE TOGGLES ARE JUST\r\nPLACEHOLDERS!");

		lagReducerToggle = QuestUI::BeatSaberUI::CreateToggle(vertical4->get_transform(), "Lag Reducer", getConfig().config["LagReducer"].GetBool(), SetLagReducer);
		QuestUI::BeatSaberUI::AddHoverHint(lagReducerToggle->get_gameObject(), "Makes multiplayer easier for the quest to handle.");

		UnityEngine::Color playerColor;
		UnityEngine::ColorUtility::TryParseHtmlString(getConfig().config["color"].GetString(), playerColor);

		//QuestUI::BeatSaberUI::CreateColorPickerModal(parent->get_transform(), "Player Color Selection", playerColor);

		auto colorPicker = QuestUI::BeatSaberUI::CreateColorPickerModal(parent, "Player Color Selection", playerColor,
			[&playerColor, sessionManager](UnityEngine::Color value) {
				playerColor = value;
				getConfig().config["color"].SetString(UnityEngine::ColorUtility::ToHtmlStringRGB_CPP(value), getConfig().config.GetAllocator());
				getConfig().Write();
				localExtendedPlayer->playerColor = value;
				SetPlayerPlaceColor(sessionManager->get_localPlayer(), value, true);
				Extensions::ExtendedPlayerPacket* localPlayerPacket = Extensions::ExtendedPlayerPacket::Init(localExtendedPlayer->get_platformID(), localExtendedPlayer->get_platform(), localExtendedPlayer->get_playerColor());
				getLogger().debug("LocalPlayer Color is, R: %f G: %f B: %f", localPlayerPacket->playerColor.r, localPlayerPacket->playerColor.g, localPlayerPacket->playerColor.b);
				packetManager->Send(reinterpret_cast<LiteNetLib::Utils::INetSerializable*>(localPlayerPacket));
			},
			[sessionManager] {
				SetPlayerPlaceColor(sessionManager->get_localPlayer(), localExtendedPlayer->get_playerColor(), true);
				//Extensions::ExtendedPlayerPacket* localPlayerPacket = Extensions::ExtendedPlayerPacket::Init(localExtendedPlayer->get_platformID(), localExtendedPlayer->get_platform(), localExtendedPlayer->get_playerColor());
				//getLogger().debug("LocalPlayer Color is, R: %f G: %f B: %f", localPlayerPacket->playerColor.r, localPlayerPacket->playerColor.g, localPlayerPacket->playerColor.b);
				//packetManager->Send(reinterpret_cast<LiteNetLib::Utils::INetSerializable*>(localPlayerPacket));
			},
			[sessionManager](UnityEngine::Color value) {
				SetPlayerPlaceColor(sessionManager->get_localPlayer(), value, true);
				// TODO: Uncomment when MpEx supports live platform color updates or maybe not because that might be too much packets sent
				
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

		auto autoDelete = QuestUI::BeatSaberUI::CreateToggle(vertical4->get_transform(), "Auto-Delete Songs", getConfig().config["autoDelete"].GetBool(), [](bool value) {
			getConfig().config["autoDelete"].SetBool(value);
			getConfig().Write();
			});
		QuestUI::BeatSaberUI::AddHoverHint(autoDelete->get_gameObject(), "Automatically deletes downloaded songs after playing them.");

		//auto deleteDownloadedSongs = QuestUI::BeatSaberUI::CreateUIButton(vertical2->get_transform(), "Delete Downloaded", [] {
		//	using namespace RuntimeSongLoader::API;
		//	needRefresh = false;
		//	for (auto hash : DownloadedSongIds) {
		//		auto level = GetLevelByHash(hash);
		//		if (level.has_value()) {
		//			std::string songPath = to_utf8(csstrtostr(level.value()->customLevelPath));
		//			getLogger().debug("Deleting Song: %s", songPath.c_str());
		//			DeleteSong(songPath, [&] {
		//				if (needRefresh) {
		//					RefreshSongs(false);
		//				}
		//			});
		//		}
		//	}
		//	needRefresh = true;
		//	DownloadedSongIds.clear();
		//	UI::DownloadedSongsGSM::get_Instance()->Refresh();
		//	}
		//);
		//QuestUI::BeatSaberUI::AddHoverHint(deleteDownloadedSongs->get_gameObject(), "Deletes automatically downloaded songs from all multiplayer sessions since you launched the game.");


		auto colorPickerButton = QuestUI::BeatSaberUI::CreateUIButton(vertical2->get_transform(), "Color", [colorPicker] {
			colorPicker->Show();
			}
		);
		QuestUI::BeatSaberUI::AddHoverHint(colorPickerButton->get_gameObject(), "Lets you pick your own personal platform and Name Tag color for everyone to see.");

		//customSongsToggle = QuestUI::BeatSaberUI::CreateToggle(vertical2->get_transform(), "Custom Songs", getConfig().config["customsongs"].GetBool(), SetCustomSongs);
		//QuestUI::BeatSaberUI::AddHoverHint(customSongsToggle->get_gameObject(), "Toggles custom songs for all players");
	}
}