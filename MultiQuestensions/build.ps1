Param (
[Parameter(HelpMessage="The version the mod should be compiled with")][string]$Version,
[Parameter(HelpMessage="The MpEx protocol version the mod should be compiled with")][Alias("MpEx-Protocol")][string]$MpEx_Protocol
)
Write-Host $Version
$NDKPath = Get-Content $PSScriptRoot/ndkpath.txt -First 1
if ($env:VERSION) {
$Version = $env:VERSION
}
if (!($Version)) {
$Version = "0.1.2"
}
if ($env:MPEX_PROTOCOL) {
$MpEx_Protocol = $env:MPEX_PROTOCOL
}
if (!($MpEx_Protocol)) {
$MpEx_Protocol = "0.7.1"
}
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
    exit 1;
}

echo "Building MultiQuestensions Version: $Version with MpEx Protocol Version: $MpEx_Protocol"

$buildScript = "$NDKPath/build/ndk-build"
if (-not ($PSVersionTable.PSEdition -eq "Core")) {
    $buildScript += ".cmd"
}

& $buildScript NDK_PROJECT_PATH=. APP_BUILD_SCRIPT=./Android.mk NDK_APPLICATION_MK=./Application.mk VERSION=$Version MPEX_PROTOCOL=$MpEx_Protocol