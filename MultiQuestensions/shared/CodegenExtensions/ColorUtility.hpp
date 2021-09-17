#pragma once
#include "main.hpp"
#include "beatsaber-hook/shared/utils/byref.hpp"

namespace UnityEngine {
    // Forward declaration of UnityEngine.Color
    class Color;

    class ColorUtility {
        ColorUtility() = delete;
    public:
        static inline bool TryParseHtmlString(Il2CppString* htmlString, ByRef<Color> color) {
            using TryParseHtmlString = function_ptr_t<bool, Il2CppString*, ByRef<Color>>;
            static TryParseHtmlString tryParseHtmlString = reinterpret_cast<TryParseHtmlString>(il2cpp_functions::resolve_icall("UnityEngine.ColorUtility::TryParseHtmlString"));
            return tryParseHtmlString(htmlString, color);
        }

        static inline Il2CppString* ToHtmlStringRGB(Color color) {
            using ToHtmlStringRGB = function_ptr_t<Il2CppString*, Color>;
            static ToHtmlStringRGB toHtmlStringRBG = reinterpret_cast<ToHtmlStringRGB>(il2cpp_functions::resolve_icall("UnityEngine.ColorUtility::ToHtmlStringRGB"));
            return toHtmlStringRBG(color);
        }
    };
}