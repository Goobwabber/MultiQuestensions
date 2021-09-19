Param (
[Parameter(HelpMessage="The version the mod should be compiled with")][string]$Version
)
Write-Host $Version
$NDKPath = Get-Content $PSScriptRoot/ndkpath.txt -First 1
if (!($Version)) {
$Version = "0.1.0"
}
if ($env:VERSION) {
$Version = $env:VERSION
}
Write-Host $Version
# F:\BS-Modding\MultiQuestensions\MultiQuestensions\extern\beatsaber-hook\src\inline-hook\And64InlineHook.cpp
if ((Test-Path "./extern/beatsaber-hook/src/inline-hook/And64InlineHook.cpp", "./extern/beatsaber-hook/src/inline-hook/inlineHook.c", "./extern/beatsaber-hook/src/inline-hook/relocate.c") -contains $false) {
    Write-Host "Critical: Missing inline-hook"
    if (!(Test-Path "./extern/beatsaber-hook/src/inline-hook/And64InlineHook.cpp")) {
        Write-Host "./extern/beatsaber-hook/src/inline-hook/And64InlineHook.cpp"
    }
    if (!(Test-Path "./extern/beatsaber-hook/src/inline-hook/inlineHook.c")) {
        Write-Host "./extern/beatsaber-hook/src/inline-hook/inlineHook.c"
    }
        if (!(Test-Path "./extern/beatsaber-hook/inline-hook/src/relocate.c")) {
        Write-Host "./extern/beatsaber-hook/src/inline-hook/relocate.c"
    }
    Write-Host "Task Failed"
    break;
}

echo "Building MultiQuestensions Version: $Version"

$buildScript = "$NDKPath/build/ndk-build"
if (-not ($PSVersionTable.PSEdition -eq "Core")) {
    $buildScript += ".cmd"
}

& $buildScript NDK_PROJECT_PATH=. APP_BUILD_SCRIPT=./Android.mk NDK_APPLICATION_MK=./Application.mk VERSION=$Version