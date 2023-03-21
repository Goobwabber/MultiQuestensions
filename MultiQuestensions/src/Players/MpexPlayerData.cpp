#include "CodegenExtensions/ColorUtility.hpp"
#include "Players/MpexPlayerData.hpp"
#include "Config.hpp"
using namespace UnityEngine;
using namespace LiteNetLib::Utils;

DEFINE_TYPE(MultiQuestensions::Players, MpexPlayerData)

namespace MultiQuestensions::Players
{
    void MpexPlayerData::Serialize(NetDataWriter* writer)
    {
        writer->Put(ColorUtility::ToHtmlStringRGB(Color));
    }

    void MpexPlayerData::Deserialize(NetDataReader* reader)
    {
        UnityEngine::Color color;
        if (!ColorUtility::TryParseHtmlString(reader->GetString(), color))
            color = Config::DefaultPlayerColor;
        Color = color;
    }
}
