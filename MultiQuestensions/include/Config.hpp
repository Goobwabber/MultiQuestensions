#pragma once
#include "UnityEngine/Color.hpp"

namespace MultiQuestensions {
    struct Config {
        static constexpr UnityEngine::Color DefaultPlayerColor = UnityEngine::Color(0.031f, 0.752f, 1.0f, 1.0f);

        bool Hologram = true;
        bool LagReducer = false;
        bool MissLighting = true;
        UnityEngine::Color PlayerColor = DefaultPlayerColor;
        UnityEngine::Color MissColor = UnityEngine::Color(1, 0, 0, 1);
    };

    extern Config config;
}