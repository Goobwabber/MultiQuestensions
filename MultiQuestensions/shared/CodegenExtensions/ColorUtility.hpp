#pragma once
#include "main.hpp"
#include "beatsaber-hook/shared/utils/byref.hpp"
#include "UnityEngine/UnityString.hpp"
#include "UnityEngine/Color32.hpp"

namespace UnityEngine {
    // Forward declaration of UnityEngine.Color
    class Color;
    //// Forward declaration of UnityEngine.Color32
    //class Color32;
    //// Forward declaration of UnityEngine.UnityString
    //class UnityString;

    class ColorUtility {
        ColorUtility() = delete;
    public:
        static bool TryParseHtmlString(Il2CppString* htmlString, Color& color) {
            Color32 c;
            bool result = DoTryParseHtmlColor(htmlString, byref(c));
            color = { (float)c.r / 255.0f, (float)c.g / 255.0f, (float)c.b / 255.0f, 1 };
            return result;
        }

        static bool TryParseHtmlString(std::string htmlString, Color& color) {
            return TryParseHtmlString(il2cpp_utils::newcsstr(htmlString), color);
        }

        static bool DoTryParseHtmlColor(Il2CppString* htmlString, ByRef<Color32> color) {
            using DoTryParseHtmlColor = function_ptr_t<bool, Il2CppString*, ByRef<Color32>>;
            static DoTryParseHtmlColor doTryParseHtmlColor = reinterpret_cast<DoTryParseHtmlColor>(il2cpp_functions::resolve_icall("UnityEngine.ColorUtility::DoTryParseHtmlColor"));
            return doTryParseHtmlColor(htmlString, byref(color));
        }

        static constexpr uint8_t conv(float colorVal) {
            return std::clamp(static_cast<int>(round(colorVal * 255.0f)), 0, UINT8_MAX);
        }

        static Il2CppString* ToHtmlStringRGB(Color color) {
            Color32 color2(conv(color.r), conv(color.g), conv(color.b), 1);
            return il2cpp_utils::newcsstr(string_format("#%02X%02X%02X", color2.r, color2.g, color2.b));
        }

        static std::string ToHtmlStringRGB_CPP(Color color) {
            Color32 color2(conv(color.r), conv(color.g), conv(color.b), 1);
            return string_format("#%02X%02X%02X", color2.r, color2.g, color2.b);
        }

    };
}