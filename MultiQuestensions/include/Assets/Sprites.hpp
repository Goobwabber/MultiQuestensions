#pragma once
#include "questui/shared/BeatSaberUI.hpp"

namespace MultiQuestensions {
    class Sprites {
        static std::string Oculus64;
        static std::string Steam64;
    
        static UnityEngine::Sprite* Oculus64Icon;
        static UnityEngine::Sprite* Steam64Icon;
    public:
        static UnityEngine::Sprite* IconOculus64();
        static UnityEngine::Sprite* IconSteam64();
    };
}