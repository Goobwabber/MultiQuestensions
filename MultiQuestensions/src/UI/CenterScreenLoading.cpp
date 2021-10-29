#include "main.hpp"
#include "GlobalFields.hpp"
#include "UI/CenterScreenLoading.hpp"
#include "UnityEngine/GameObject.hpp"
#include "UnityEngine/Resources.hpp"
#include "UnityEngine/Transform.hpp"
#include "UnityEngine/RectOffset.hpp"
#include "UnityEngine/UI/LayoutElement.hpp"
using namespace GlobalNamespace;
using namespace UnityEngine;
using namespace UnityEngine::UI;

#include "questui/shared/BeatSaberUI.hpp"

DEFINE_TYPE(MultiQuestensions::UI, CenterScreenLoading);

namespace MultiQuestensions::UI{

    CenterScreenLoading* CenterScreenLoading::instance;

    void CenterScreenLoading::Awake() {
        getLogger().debug("CenterScreenLoading::Awake");
        instance = this;

        UnityEngine::UI::VerticalLayoutGroup* vertical = QuestUI::BeatSaberUI::CreateVerticalLayoutGroup(get_transform());
        vertical->get_rectTransform()->set_sizeDelta({ 60, 60 });
        vertical->get_rectTransform()->set_anchoredPosition({ 0.0f, -30.0f });
        auto layout = vertical->get_gameObject()->AddComponent<LayoutElement*>();
        //layout->set_preferredHeight(GameListSizeDelta.y);
        layout->set_minWidth(60);
        //vertical->set_padding(UnityEngine::RectOffset::New_ctor(0, 0, 30, 10));

        GameObject* existingLoadingControl = Resources::FindObjectsOfTypeAll<LoadingControl*>()->values[0]->get_gameObject();
        GameObject* loadingControlGameObject = UnityEngine::GameObject::Instantiate(existingLoadingControl, vertical->get_transform());
        loadingControl = loadingControlGameObject->GetComponent<LoadingControl*>();
        loadingControl->Hide();
    }

    void CenterScreenLoading::OnDisable() {
        getLogger().debug("CenterScreenLoading::OnDisable");
        if (loadingControl) {
            loadingControl->Hide();
        }
    }

    void CenterScreenLoading::ShowLoading() {
        getLogger().debug("CenterScreenLoading::ShowLoading, players ready (%d of %d)", 
            playersReady + 1, sessionManager ? sessionManager->get_connectedPlayerCount() + 1 : 1);
        if (loadingControl) {
            loadingControl->ShowLoading(il2cpp_utils::newcsstr(string_format("%d of %d players ready...",
                playersReady + 1, sessionManager ? sessionManager->get_connectedPlayerCount() + 1 : 1)));
        }
    }

    void CenterScreenLoading::ShowDownloadingProgress(float downloadingProgress) {
        //getLogger().debug("CenterScreenLoading::ShowDownloadingProgress: %f2", downloadingProgress);
        if (loadingControl) {
            static Il2CppString* string = il2cpp_utils::newcsstr<il2cpp_utils::CreationType::Manual>("Downloading...");
            loadingControl->ShowDownloadingProgress(string, downloadingProgress);
        }
    }

    void CenterScreenLoading::HideLoading() {
        getLogger().debug("CenterScreenLoading::HideLoading");
        if (loadingControl) {
            loadingControl->Hide();
        }
    }

    void CenterScreenLoading::UpdatePlayersReady(int playerReady) {
        if (instance) {
            instance->playersReady = playerReady;
            //instance->dyn__loadingText()->SetText(il2cpp_utils::newcsstr());
            if (instance->loadingControl)
                instance->ShowLoading();
        }
    }

}