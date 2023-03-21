#pragma once
#include "questui/shared/BeatSaberUI.hpp"

namespace MultiQuestensions {
    class Sprites {
        static std::string Oculus64;
        static std::string Steam64;
        static std::string Meta64;
    
    public:
        static UnityEngine::Sprite* IconOculus64();
        static UnityEngine::Sprite* IconSteam64();
        static UnityEngine::Sprite* IconMeta64();
    };
}