Param (
[Parameter(Mandatory=$false)][Switch]$clean,
[Parameter(HelpMessage="The version the mod should be compiled with")][string]$Version,
[Parameter(Mandatory=$false, HelpMessage="To create a release build")][Alias("publish")][Switch]$release,
[Parameter(Mandatory=$false, HelpMessage="To create a github actions build, assumes specific Environment variables are set")][Alias("github-build")][Switch]$actions,
[Parameter(HelpMessage="The MpEx protocol version the mod should be compiled with")][Alias("MpEx-Protocol")][string]$MpEx_Protocol
)
$QPMpackage = "./qpm.json"
$qpmjson = Get-Content $QPMpackage -Raw | ConvertFrom-Json
if (-not $Version) {
    $VERSION = $qpmjson.info.version
} else {
    $VERSION = $Version
}
if ($release -ne $true -and -not $VERSION.Contains('-')) {
    $VERSION += "-Dev"
}

if ($env:version -eq "") {
    & qpm-rust package edit --version $VERSION
}

if ((Test-Path "./extern/includes/beatsaber-hook/src/inline-hook/And64InlineHook.cpp", "./extern/includes/beatsaber-hook/src/inline-hook/inlineHook.c", "./extern/includes/beatsaber-hook/src/inline-hook/relocate.c") -contains $false) {
    Write-Host "Critical: Missing inline-hook"
    if (!(Test-Path "./extern/includes/beatsaber-hook/src/inline-hook/And64InlineHook.cpp")) {
        Write-Host "./extern/includes/beatsaber-hook/src/inline-hook/And64InlineHook.cpp"
    }
    if (!(Test-Path "./extern/includes/beatsaber-hook/src/inline-hook/inlineHook.c")) {
        Write-Host "./extern/includes/beatsaber-hook/src/inline-hook/inlineHook.c"
    }
        if (!(Test-Path "./extern/includes/beatsaber-hook/inline-hook/src/relocate.c")) {
        Write-Host "./extern/includes/beatsaber-hook/src/inline-hook/relocate.c"
    }
    Write-Host "Task Failed"
    exit 1;
}

Write-Output "Building MultiQuestensions Version: $VERSION with MpEx Protocol Version: $MpEx_Protocol"

if ($clean.IsPresent)
{
    if (Test-Path -Path "build")
    {
        remove-item build -R
    }
}

if (($clean.IsPresent) -or (-not (Test-Path -Path "build")))
{
    (new-item -Path build -ItemType Directory) | Out-Null
}

Get-ChildItem

Set-Location build
Get-ChildItem
& cmake -G "Ninja" -DCMAKE_BUILD_TYPE="RelWithDebInfo" -DMPEX_PROTOCOL="$MpEx_Protocol" ../
& cmake --build . -j 6
$ExitCode = $LastExitCode
Set-Location ..
exit $ExitCode