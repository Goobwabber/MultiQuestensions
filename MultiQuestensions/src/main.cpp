#include "main.hpp"
#include "packets.cpp"

static ModInfo modInfo; // Stores the ID and version of our mod, and is sent to the modloader upon startup

// Loads the config from disk using our modInfo, then returns it for use
Configuration& getConfig() {
    static Configuration config(modInfo);
    config.Load();
    return config;
}

// Returns a logger, useful for printing debug messages
const Logger& getLogger() {
    static const Logger logger(modInfo);
    return logger;
}

static PacketManager packetManager;

static Il2CppString* moddedState = createcsstr("modded", StringType::Manual);
static Il2CppString* customSongsState = createcsstr("customsongs", StringType::Manual);
static Il2CppString* enforceModsState = createcsstr("enforcemods", StringType::Manual);

MAKE_HOOK_OFFSETLESS(SessionManagerStart, void, GlobalNamespace::MultiplayerSessionManager* self) {
    SessionManagerStart(self);

    packetManager = PacketManager(self);

    self->SetLocalPlayerState(moddedState, true);
    self->SetLocalPlayerState(customSongsState, getConfig().config["customsongs"].GetBool());
    self->SetLocalPlayerState(enforceModsState, getConfig().config["enforcemods"].GetBool());
}


void saveDefaultConfig() {
    getLogger().info("Creating config file...");
    ConfigDocument& config = getConfig().config;

    if (config.HasMember("customsongs")) {
        getLogger().info("Config file already exists.");
        return;
    }

    config.RemoveAllMembers();
    config.SetObject();

    config["customsongs"].SetBool(true);
    config["enforcemods"].SetBool(true);

    getConfig().Write();
    getLogger().info("Config file created.");
}

// Called at the early stages of game loading
extern "C" void setup(ModInfo& info) {
    info.id = ID;
    info.version = VERSION;
    modInfo = info;

    getConfig().Load();
    saveDefaultConfig();

    getLogger().info("Completed setup!");
}

// Called later on in the game loading - a good time to install function hooks
extern "C" void load() {
    il2cpp_functions::Init();

    getLogger().info("Installing hooks...");
    // Install our hooks (none defined yet)
    getLogger().info("Installed all hooks!");
}