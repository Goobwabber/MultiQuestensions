#include "Environments/LobbyAvatarPlaceLighting.hpp"
#include "UnityEngine/Time.hpp"
using namespace GlobalNamespace;
using namespace UnityEngine;

DEFINE_TYPE(MultiQuestensions::Environments, LobbyAvatarPlaceLighting);

namespace MultiQuestensions::Environments {
    void LobbyAvatarPlaceLighting::OnEnable() {
        auto lights = this->GetComponentsInChildren<TubeBloomPrePassLight*>();
        for (int i = 0; i < lights->Length(); i++) {
            this->lights.emplace_back(lights->get(i));
        }
    }

    void LobbyAvatarPlaceLighting::OnDisable() {
        lights.clear();
    }

    void LobbyAvatarPlaceLighting::Update() {
        Color current = GetColor();

        if (current == targetColor)
            return;

        if (IsColorVeryCloseToColor(current, targetColor))
            SetColor(targetColor);
        else
            SetColor(Color::Lerp(current, targetColor, Time::get_deltaTime() * smoothTime));
    }

    void LobbyAvatarPlaceLighting::SetColor(Color color, bool immediate) {
        targetColor = color;
        if (immediate)
        {
            SetColor(color);
        }
    }

    Color LobbyAvatarPlaceLighting::GetColor() {
        if (lights.size() > 0)
            return lights[0]->get_color();
        return Color::get_black();
    }

    bool LobbyAvatarPlaceLighting::IsColorVeryCloseToColor(Color color0, Color color1)
    {
        return fabs(color0.r - color1.r) < 0.002f && fabs(color0.g - color1.g) < 0.002f && fabs(color0.b - color1.b) < 0.002f && fabs(color0.a - color1.a) < 0.002f;
    }

    void LobbyAvatarPlaceLighting::SetColor(Color color) {
        for (TubeBloomPrePassLight* light : lights) {
            light->set_color(color);
        }
    }
}