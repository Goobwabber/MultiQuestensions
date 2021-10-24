#include "main.hpp"
#include "Environments/LobbyAvatarPlaceLighting.hpp"
#include "UnityEngine/Time.hpp"
using namespace GlobalNamespace;
using namespace UnityEngine;

DEFINE_TYPE(MultiQuestensions::Environments, LobbyAvatarPlaceLighting);

namespace MultiQuestensions::Environments {

    void LobbyAvatarPlaceLighting::New() {}

    void LobbyAvatarPlaceLighting::OnEnable() {
        auto lightsArr = this->GetComponentsInChildren<TubeBloomPrePassLight*>();
        for (int i = 0; i < lightsArr->Length(); i++) {
            this->lights.emplace_back(lightsArr->get(i));
        }
    }

    void LobbyAvatarPlaceLighting::OnDisable() {
        lights.clear();
    }

    void LobbyAvatarPlaceLighting::Update() {
        Color current = GetColor();

        if (current == targetColor)
            return;

        SetColor(targetColor);

        // TODO: Figure out why the below causes color to be infinitely set to 0
        //if (IsColorVeryCloseToColor(current, targetColor))
        //    SetColor(targetColor);
        //else
        //    SetColor(Color::Lerp(current, targetColor, Time::get_deltaTime() * smoothTime));
    }

    void LobbyAvatarPlaceLighting::SetColor(const Color& color, bool immediate) {
        //getLogger().debug("SetColor 2 args, R: %f G: %f B: %f", color.r, color.g, color.b);
        targetColor = color;
        //getLogger().debug("SetColor 2 new targetColor, R: %f G: %f B: %f", targetColor.r, targetColor.g, targetColor.b);

        //Color lerpColor = Color::Lerp(GetColor(), targetColor, Time::get_deltaTime() * smoothTime);
        //getLogger().debug("SetColor lerp, R: %f G: %f B: %f", lerpColor.r, lerpColor.g, lerpColor.b);

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
        getLogger().debug("SetColor, R: %f G: %f B: %f", color.r, color.g, color.b);
        for (TubeBloomPrePassLight* light : lights) {
            light->set_color(color);
            light->Refresh();
        }
    }
}