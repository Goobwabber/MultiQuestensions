#pragma once

// Include the modloader header, which allows us to tell the modloader which mod this is, and the version etc.
#include "modloader/shared/modloader.hpp"

// beatsaber-hook is a modding framework that lets us call functions and fetch field values from in the game
// It also allows creating objects, configuration, and importantly, hooking methods to modify their values
#include "beatsaber-hook/shared/utils/logging.hpp"
#include "beatsaber-hook/shared/config/config-utils.hpp"
#include "beatsaber-hook/shared/utils/il2cpp-functions.hpp"
#include "beatsaber-hook/shared/utils/il2cpp-utils.hpp"
#include "codegen/include/LiteNetLib/Utils/INetSerializable.hpp"

// Codegen includes
#include "GlobalNamespace/MultiplayerSessionManager.hpp"
#include "GlobalNamespace/IMenuRpcManager.hpp"
#include "GlobalNamespace/LobbyPlayersDataModel.hpp"
#include "GlobalNamespace/ILobbyPlayersDataModel.hpp"
//#include "GlobalNamespace/ILobbyPlayerDataModel.hpp"
#include "GlobalNamespace/ILevelGameplaySetupData.hpp"
#include "GlobalNamespace/BeatmapLevelsModel.hpp"
#include "GlobalNamespace/IConnectedPlayer.hpp"

#include "GlobalNamespace/BeatmapIdentifierNetSerializable.hpp"
#include "GlobalNamespace/IPreviewBeatmapLevel.hpp"
#include "GlobalNamespace/BeatmapDifficulty.hpp"
#include "GlobalNamespace/BeatmapCharacteristicSO.hpp"
#include "GlobalNamespace/BeatmapCharacteristicCollectionSO.hpp"
#include "GlobalNamespace/HMMainThreadDispatcher.hpp"

// Cpp includes
#include <map>
#include <set>
#include <string>



// Define these functions here so that we can easily read configuration and log information from other files
Configuration& getConfig();
Logger& getLogger();

// Utilities
Il2CppString* LevelIdToHash(Il2CppString*);

extern bool customSongsEnabled;

// Fix DECLARE_CTOR

#ifdef DECLARE_CTOR_FIX
#error "DECLARE_CTOR_FIX is already defined! Undefine it before including macros.hpp!"
#endif
// Declare a method with name that will be called on construction.
#define DECLARE_CTOR_FIX(name, ...) \
public: \
void name(__VA_ARGS__); \
template<::il2cpp_utils::CreationType creationType = ::il2cpp_utils::CreationType::Temporary, class... TArgs> \
static ___TargetType* New_ctor(TArgs&&... args) { \
    static_assert(::custom_types::Decomposer<decltype(&___TargetType::name)>::equal<TArgs...>(), "Arguments provided to New_ctor must match the constructor!"); \
    return THROW_UNLESS(il2cpp_utils::New<___TargetType*, creationType>(___TypeRegistration::klass_ptr, std::forward<TArgs>(args)...)); \
} \
___CREATE_INSTANCE_METHOD(name, ".ctor", METHOD_ATTRIBUTE_PUBLIC | METHOD_ATTRIBUTE_HIDE_BY_SIG | METHOD_ATTRIBUTE_SPECIAL_NAME | METHOD_ATTRIBUTE_RT_SPECIAL_NAME, nullptr)
