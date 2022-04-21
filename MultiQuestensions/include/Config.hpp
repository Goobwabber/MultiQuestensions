#pragma once
#include "main.hpp"
#include "UnityEngine/Color.hpp"
#include "CodegenExtensions/ColorUtility.hpp"

namespace MultiQuestensions {
    Configuration& getConfig();
    Configuration& getLegacyConfig();

    struct Config {
        static constexpr UnityEngine::Color DefaultPlayerColor = UnityEngine::Color(0.031f, 0.752f, 1.0f, 1.0f);

    protected:
        bool Hologram = true;
        bool LagReducer = false;
        bool MissLighting = true;
        bool NoMetaZone = false;
        UnityEngine::Color PlayerColor = DefaultPlayerColor;
        UnityEngine::Color MissColor = UnityEngine::Color(1, 0, 0, 1);

    public:
        bool getHologram() const { return Hologram; }

        void setHologram(bool value) {
            Hologram = value;
            getConfig().config["Hologram"].SetBool(Hologram);
            getConfig().Write();
        }

        bool getLagReducer() const { return LagReducer; }

        void setLagReducer(bool value) {
            LagReducer = value;
            getConfig().config["LagReducer"].SetBool(LagReducer);
            getConfig().Write();
        }

        bool getMissLighting() const { return MissLighting; }

        void setMissLighting(bool value) {
            MissLighting = value;
            getConfig().config["MissLighting"].SetBool(MissLighting);
            getConfig().Write();
        }

        bool getNoMetaZone() const { return NoMetaZone; }

        void setNoMetaZone(bool value) {
            NoMetaZone = value;
            getConfig().config["NoMetaZone"].SetBool(NoMetaZone);
            getConfig().Write();
        }

        UnityEngine::Color getPlayerColor() const {
            return PlayerColor;
        }

        void setPlayerColor(const UnityEngine::Color& value) {
            PlayerColor = value;
            ConfigDocument& configDoc = getConfig().config;
            auto& allocator = configDoc.GetAllocator();
            configDoc["PlayerColor"].SetString(UnityEngine::ColorUtility::ToHtmlStringRGB_CPP(PlayerColor), allocator);
            getConfig().Write();
        }

        UnityEngine::Color getMissColor() const {
            return MissColor;
        }

        void setMissColor(const UnityEngine::Color& value) {
            MissColor = value;
            ConfigDocument& configDoc = getConfig().config;
            auto& allocator = configDoc.GetAllocator();
            configDoc["MissColor"].SetString(UnityEngine::ColorUtility::ToHtmlStringRGB_CPP(MissColor), allocator);
            getConfig().Write();
        }

        void Initialize() {
            getLogger().info("Checking for legacy config file...");
            // Checking legacy config file
            ConfigDocument& legacyConfig = getLegacyConfig().config;
            if (legacyConfig.IsObject() && !legacyConfig.HasMember("Converted")) {
                // Legacy Conversion
                getLogger().info("Legacy config file found. Converting...");
                if (legacyConfig.HasMember("LagReducer") && legacyConfig["LagReducer"].IsBool()) {
                    LagReducer = legacyConfig["LagReducer"].GetBool();
                }
                if (legacyConfig.HasMember("color") && legacyConfig["color"].IsString()) {
                    UnityEngine::ColorUtility::TryParseHtmlString(legacyConfig["color"].GetString(), PlayerColor);
                }
                legacyConfig.AddMember("Converted", true, legacyConfig.GetAllocator());
            } 
            // TODO: Delete config on a later version
            /*
            else if (legacyConfig.IsObject() && legacyConfig.HasMember("Converted")) {
                remove(Configuration::getConfigFilePath({"multiquestensions", modInfo.version}).c_str());
            }
            */

            getLogger().info("Reading config file...");
            ConfigDocument& configDoc = getConfig().config;
            bool parseError = false;
            if (configDoc.IsObject()) {
                (configDoc.HasMember("Hologram") && configDoc["Hologram"].IsBool()) ? (Hologram  = configDoc["Hologram"].GetBool()) : parseError = true;
                (configDoc.HasMember("LagReducer") && configDoc["LagReducer"].IsBool()) ? (LagReducer = configDoc["LagReducer"].GetBool()) : parseError = true;
                (configDoc.HasMember("MissLighting") && configDoc["MissLighting"].IsBool()) ? (MissLighting = configDoc["MissLighting"].GetBool()) : parseError = true;
                (configDoc.HasMember("NoMetaZone") && configDoc["NoMetaZone"].IsBool()) ? (NoMetaZone = configDoc["NoMetaZone"].GetBool()) : parseError = true;
                if (!(configDoc.HasMember("PlayerColor") && configDoc["PlayerColor"].IsString() &&
                    UnityEngine::ColorUtility::TryParseHtmlString(configDoc["PlayerColor"].GetString(), PlayerColor))) {
                    parseError = true;
                }
                if (!(configDoc.HasMember("MissColor") && configDoc["MissColor"].IsString() &&
                    UnityEngine::ColorUtility::TryParseHtmlString(configDoc["MissColor"].GetString(), MissColor))) {
                    parseError = true;
                }
            } else parseError = true;

            if (parseError) SaveDefaultConfig();
        }

        void SaveDefaultConfig() {
            getLogger().info("Creating config file...");
            ConfigDocument& configDoc = getConfig().config;
            configDoc.RemoveAllMembers();
            if (!configDoc.IsObject())
                configDoc.SetObject();
            auto& allocator = configDoc.GetAllocator();

            configDoc.AddMember("Hologram", Hologram, allocator);
            configDoc.AddMember("LagReducer", LagReducer, allocator);
            configDoc.AddMember("MissLighting", MissLighting, allocator);
            configDoc.AddMember("NoMetaZone", NoMetaZone, allocator);
            configDoc.AddMember("PlayerColor", UnityEngine::ColorUtility::ToHtmlStringRGB_CPP(PlayerColor), allocator);
            configDoc.AddMember("MissColor", UnityEngine::ColorUtility::ToHtmlStringRGB_CPP(MissColor), allocator);

            getConfig().Write();
            getLogger().info("Config file created.");
        }
    };

    extern MultiQuestensions::Config config;
}