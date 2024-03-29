#include "main.hpp"
#include "Environments/LobbyAvatarPlaceLighting.hpp"
#include "UnityEngine/Time.hpp"
using namespace GlobalNamespace;
using namespace UnityEngine;

DEFINE_TYPE(MultiQuestensions::Environments, LobbyAvatarPlaceLighting);

namespace MultiQuestensions::Environments {

    void LobbyAvatarPlaceLighting::New() {
        smoothTime = 2.0f;
    }

    void LobbyAvatarPlaceLighting::OnEnable() {
        auto lightsArr = this->GetComponentsInChildren<TubeBloomPrePassLight*>();
        for (int i = 0; i < lightsArr->Length(); i++) {
            this->lights.emplace_back(lightsArr->get(i));
        }
    }

    void LobbyAvatarPlaceLighting::OnDisable() {
        lights.clear();
    }

    inline static Color Lerp(Color const& a, Color const& b, float const& t)
    {
        float t_clamp = std::clamp(t, 0.0f, 1.0f);
        return Color(a.r + (b.r - a.r) * t_clamp, a.g + (b.g - a.g) * t_clamp, a.b + (b.b - a.b) * t_clamp, a.a + (b.a - a.a) * t_clamp);
    }

    void LobbyAvatarPlaceLighting::Update() {
        Color current = GetColor();

        if (current == targetColor)
            return;

        SetColor(targetColor);

        if (IsColorVeryCloseToColor(current, targetColor))
            SetColor(targetColor);
        else
            //SetColor(Color::Lerp(current, targetColor, Time::get_deltaTime() * smoothTime));
            SetColor(Lerp(current, targetColor, Time::get_deltaTime() * smoothTime));
    }

    void LobbyAvatarPlaceLighting::SetColor(const Color& color, bool immediate) {
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
        return (fabs(color0.r - color1.r) < 0.002f && fabs(color0.g - color1.g) < 0.002f && fabs(color0.b - color1.b) < 0.002f && fabs(color0.a - color1.a) < 0.002f);
    }

    void LobbyAvatarPlaceLighting::SetColor(const Color& color) {
        for (TubeBloomPrePassLight* light : lights) {
            light->set_color(color);
            light->Refresh();
        }
    }
}