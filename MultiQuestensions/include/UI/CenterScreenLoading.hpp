#pragma once
#include "custom-types/shared/macros.hpp"
#include "UnityEngine/MonoBehaviour.hpp"
#include "GlobalNamespace/LoadingControl.hpp"
#include "GlobalNamespace/CenterStageScreenController.hpp"

DECLARE_CLASS_CODEGEN(MultiQuestensions::UI, CenterScreenLoading, UnityEngine::MonoBehaviour,
    DECLARE_INSTANCE_FIELD(GlobalNamespace::LoadingControl*, loadingControl);
    DECLARE_INSTANCE_FIELD(GlobalNamespace::CenterStageScreenController*, screenController);
    DECLARE_INSTANCE_FIELD(GlobalNamespace::LobbyGameStateController*, gameStateController);
    bool isDownloading;

    DECLARE_INSTANCE_METHOD(void, Awake);
    DECLARE_INSTANCE_METHOD(void, OnDisable);
    DECLARE_INSTANCE_METHOD(void, ShowLoading);
    // Note: downloadingProgress 1.0f = Full
    DECLARE_INSTANCE_METHOD(void, ShowDownloadingProgress, float downloadingProgress);
    //DECLARE_INSTANCE_METHOD(void, HideLoading);
    DECLARE_INSTANCE_METHOD(void, FixedUpdate);
    static CenterScreenLoading* instance;
public:
    static int playersReady;
    static CenterScreenLoading* get_Instance() { return instance; }
    static void HideLoading();
    //static void UpdatePlayersReady(int playerReady);
)
