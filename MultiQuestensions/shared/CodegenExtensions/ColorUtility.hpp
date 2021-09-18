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
        static bool TryParseHtmlString(Il2CppString* htmlString, ByRef<Color> color) {
            Color32 c;
            bool result = DoTryParseHtmlColor(htmlString, ByRef(c));
            color.heldRef.r = (float)c.r;
            color.heldRef.g = (float)c.g;
            color.heldRef.b = (float)c.b;
            return result;
        }

        static bool DoTryParseHtmlColor(Il2CppString* htmlString, ByRef<Color32> color) {
            using DoTryParseHtmlColor = function_ptr_t<bool, Il2CppString*, ByRef<Color32>>;
            static DoTryParseHtmlColor doTryParseHtmlColor = reinterpret_cast<DoTryParseHtmlColor>(il2cpp_functions::resolve_icall("UnityEngine.ColorUtility::DoTryParseHtmlColor"));
            return doTryParseHtmlColor(htmlString, ByRef(color));
        }

        static Il2CppString* ToHtmlStringRGB(Color color) {
            Color32 color2 = Color32((uint8_t)std::clamp((int)round(color.r * 255.0f), 0, 255), (uint8_t)std::clamp((int)round(color.g * 255.0f), 0, 255), (uint8_t)std::clamp((int)round(color.b * 255.0f), 0, 255), 1);
            return il2cpp_utils::newcsstr(string_format("#%02x%02x%02x", color2.r, color2.g, color2.b));
            //return UnityString::Format(il2cpp_utils::newcsstr("{0:X2}{1:X2}{2:X2}"), Array<uint8_t>::New(
            //    {
            //        color2.r,
            //        color2.g,
            //        color2.b
            //    }));
        }
    };
}