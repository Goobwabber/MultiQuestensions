#pragma once
#include "custom-types/shared/macros.hpp"
#include "UnityEngine/MonoBehaviour.hpp"
#include "GlobalNamespace/LoadingControl.hpp"

DECLARE_CLASS_CODEGEN(MultiQuestensions::UI, CenterScreenLoading, UnityEngine::MonoBehaviour,
    DECLARE_INSTANCE_FIELD(GlobalNamespace::LoadingControl*, loadingControl);

    DECLARE_INSTANCE_METHOD(void, Awake);
    DECLARE_INSTANCE_METHOD(void, OnDisable);
    DECLARE_INSTANCE_METHOD(void, ShowLoading);
    // Note: downloadingProgress 1.0f = Full
    DECLARE_INSTANCE_METHOD(void, ShowDownloadingProgress, float downloadingProgress);
    DECLARE_INSTANCE_METHOD(void, HideLoading);
    static CenterScreenLoading* instance;
public:
    static CenterScreenLoading* get_Instance() { return instance; }
)
