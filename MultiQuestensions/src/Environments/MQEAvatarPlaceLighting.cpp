#include "main.hpp"
#include "Environments/MQEAvatarPlaceLighting.hpp"
#include "UnityEngine/Time.hpp"
#include "Config.hpp"
using namespace GlobalNamespace;
using namespace UnityEngine;

DEFINE_TYPE(MultiQuestensions::Environments, MQEAvatarPlaceLighting);

namespace MultiQuestensions::Environments {

    Logger& getLogger() {
        static Logger* myLogger = new Logger(modInfo, LoggerOptions(false, true));
        return *myLogger;
    }

    void MQEAvatarPlaceLighting::New() {
        //getLogger().debug("MQEAvatarPlaceLighting::New()");
        smoothTime = 2.0f;
        // if (targetColor != UnityEngine::Color::get_black())
            targetColor = UnityEngine::Color::get_black();
    }

    // void MQEAvatarPlaceLighting::Awake() {
    //     // auto lightsArr = this->GetComponentsInChildren<TubeBloomPrePassLight*>();
    //     // for (int i = 0; i < lightsArr.Length(); i++) {
    //     //     this->lights.emplace_back(lightsArr.get(i));
    //     // }

    //     // if (_sessionManager == nullptr || _sessionManager->get_localPlayer() == nullptr)
    //     //     return;

    //     // if (_sessionManager->get_localPlayer()->get_sortIndex() == SortIndex) {
    //     //     this->SetColor(config.PlayerColor, true);
    //     //     return;
    //     // }

    //     // for (int i = 0; i < _sessionManager->dyn__connectedPlayers()->get_Count(); i++) {
    //     //     if (_sessionManager->dyn__connectedPlayers()->get_Item(i)->get_sortIndex() == SortIndex) {
    //     //         auto player = _sessionManager->dyn__connectedPlayers()->get_Item(i);
    //     //         auto mpexPlayerData = MpexPlayerManager::GetPlayer(player->get_playerId());
    //     //         this->SetColor(mpexPlayerData ? mpexPlayerData->get_color() : Config::DefaultPlayerColor, true);
    //     //         return;
    //     //     }
    //     // }
    // }

    void MQEAvatarPlaceLighting::OnEnable() {
        //getLogger().debug("MQEAvatarPlaceLighting::OnEnable()");
        auto lightsArr = this->GetComponentsInChildren<TubeBloomPrePassLight*>();
        for (int i = 0; i < lightsArr.Length(); i++) {
            this->lights.emplace_back(lightsArr.get(i));
        }
    }

    void MQEAvatarPlaceLighting::OnDisable() {
        //getLogger().debug("MQEAvatarPlaceLighting::OnDisable()");
        lights.clear();
    }

    inline static Color Lerp(Color const& a, Color const& b, float const& t)
    {
        //getLogger().debug("Color Lerp");
        float t_clamp = std::clamp(t, 0.0f, 1.0f);
        return Color(a.r + (b.r - a.r) * t_clamp, a.g + (b.g - a.g) * t_clamp, a.b + (b.b - a.b) * t_clamp, a.a + (b.a - a.a) * t_clamp);
    }

    void MQEAvatarPlaceLighting::FixedUpdate() {
        //getLogger().debug("MQEAvatarPlaceLighting::Update()");
        Color current = GetColor();

        if (current == targetColor)
            return;

        // SetColor(targetColor);

        if (IsColorVeryCloseToColor(current, targetColor))
            SetColor(targetColor);
        else
            //SetColor(Color::Lerp(current, targetColor, Time::get_deltaTime() * smoothTime));
            SetColor(Lerp(current, targetColor, Time::get_deltaTime() * smoothTime));
    }

    void MQEAvatarPlaceLighting::SetColor(const Color& color, bool immediate) {
        //getLogger().debug("MQEAvatarPlaceLighting::SetColor(const Color& color, bool immediate)");
        targetColor = color;

        if (immediate)
        {
            SetColor(color);
        }
    }

    Color MQEAvatarPlaceLighting::GetColor() {
        //getLogger().debug("MQEAvatarPlaceLighting::GetColor()");
        if (lights.size() > 0)
            return lights[0]->get_color();
        return Color::get_black();
    }

    bool MQEAvatarPlaceLighting::IsColorVeryCloseToColor(Color color0, Color color1)
    {
        //getLogger().debug("MQEAvatarPlaceLighting::IsColorVeryCloseToColor");
        return (fabs(color0.r - color1.r) < 0.002f && fabs(color0.g - color1.g) < 0.002f && fabs(color0.b - color1.b) < 0.002f && fabs(color0.a - color1.a) < 0.002f);
    }

    void MQEAvatarPlaceLighting::SetColor(const Color& color) {
        //getLogger().debug("MQEAvatarPlaceLighting::SetColor(const Color& color)");
        for (TubeBloomPrePassLight* light : lights) {
            light->set_color(color);
            light->Refresh();
        }
    }
}