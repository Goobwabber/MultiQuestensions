# MultiQuestensions
There's nothing here. Go away.

## Features
none.

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