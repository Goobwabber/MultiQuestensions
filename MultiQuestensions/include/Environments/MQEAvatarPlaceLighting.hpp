#pragma once
#include "custom-types/shared/macros.hpp"
#include "UnityEngine/MonoBehaviour.hpp"
#include "GlobalNamespace/TubeBloomPrePassLight.hpp"
#include "UnityEngine/Color.hpp"

#include "Players/MpexPlayerManager.hpp"

#include "MultiplayerCore/shared/Players/MpPlayerManager.hpp"
#include "MultiplayerCore/shared/Utils/event.hpp"

DECLARE_CLASS_CODEGEN(MultiQuestensions::Environments, MQEAvatarPlaceLighting, UnityEngine::MonoBehaviour, 
    std::vector<GlobalNamespace::TubeBloomPrePassLight*> lights;
    DECLARE_INSTANCE_FIELD(float, smoothTime);
    DECLARE_INSTANCE_FIELD_DEFAULT(UnityEngine::Color, targetColor, UnityEngine::Color::get_black());
    DECLARE_INSTANCE_FIELD_DEFAULT(UnityEngine::Color, TargetColor, targetColor);

    DECLARE_CTOR(New);

    // DECLARE_INSTANCE_METHOD(void, Awake);
    DECLARE_INSTANCE_METHOD(void, OnEnable);
    DECLARE_INSTANCE_METHOD(void, OnDisable);
    DECLARE_INSTANCE_METHOD(void, FixedUpdate);
    DECLARE_INSTANCE_METHOD(UnityEngine::Color, GetColor);
    DECLARE_INSTANCE_METHOD(bool, IsColorVeryCloseToColor, UnityEngine::Color color0, UnityEngine::Color color1);

public:
    void SetColor(const UnityEngine::Color& color, bool immediate);
private:
    void SetColor(const UnityEngine::Color& color);
)