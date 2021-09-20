#include "Assets/Sprites.hpp"
using UnityEngine::Sprite;
using namespace QuestUI::BeatSaberUI;

namespace MultiQuestensions {
    Sprite* Sprites::Oculus64Icon;
    Sprite* Sprites::Steam64Icon;

    Sprite* Sprites::IconOculus64() {
        if (!Oculus64Icon) Oculus64Icon = Base64ToSprite(Oculus64);
        return Oculus64Icon;
    }

    Sprite* Sprites::IconSteam64() {
        if (!Steam64Icon) Steam64Icon = Base64ToSprite(Steam64);
        return Steam64Icon;
    }
}