# MultiQuestensions [![Build](https://github.com/Goobwabber/MultiQuestensions/workflows/NDK%20Build/badge.svg)](https://github.com/Goobwabber/MultiQuestensions/actions?query=workflow%3A%22NDK+Build%22)
Quest Port of PCs MultiplayerExtensions

## Features
- Allows custom levels to be selected in private lobbies and Quickplay Customs.
- Attempts to download missing songs from Beat Saver.
- Shows a Download and Players Ready Indicator
- Shows Platform Specific Icons and Custom Colors
- Has a Download History for Multiplayer in the GameplaySetupMenu
- Auto-Delete Toggle that automatically delete songs that where downloaded in Multiplayer after playing them.
- Lag Reducer Toggle that hides notes/bombs and walls from other players. (Specifically useful when playing ME maps)
- Loosen Avatar Pose Restrictions, you can freely walk over to others if you so desire
- 10 player lobbies

## Contributing
Anyone can contribute if you want to, it would be greatly appreciated.

### Building
Visual Studio 2019 with the Android C++ dev kit is recommended.

The project will not build without your NDK directory set. You can set it using a `ndkbuild.props` file in the project root.
**Example ndkbuild.props File:**
```xml
<?xml version="1.0" encoding="utf-8"?>
<Project ToolsVersion="Current" xmlns="http://schemas.microsoft.com/developer/msbuild/2003">
  <ImportGroup Label="PropertySheets" />
  <PropertyGroup Label="UserMacros">
    <NdkPath>Full\Path\To\NDK</NdkPath>
  </PropertyGroup>
  <PropertyGroup />
  <ItemDefinitionGroup />
  <ItemGroup>
    <BuildMacro Include="NdkPath">
      <Value>${NdkPath}</Value>
    </BuildMacro>
  </ItemGroup>
</Project>
```
