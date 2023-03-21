Param (
[Parameter(Mandatory=$false)][Switch]$clean,
[Parameter(HelpMessage="The version the mod should be compiled with")][string]$Version,
[Parameter(HelpMessage="The MpEx protocol version the mod should be compiled with")][Alias("MpEx-Protocol")][string]$MpEx_Protocol
)
Write-Host $Version
if ($env:VERSION) {
$Version = $env:VERSION
}
if (!($Version)) {
$Version = "1.0.0"
}
if ($env:MPEX_PROTOCOL) {
$MpEx_Protocol = $env:MPEX_PROTOCOL
}
if (!($MpEx_Protocol)) {
$MpEx_Protocol = "1.0.0"
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

Write-Output "Building MultiQuestensions Version: $Version with MpEx Protocol Version: $MpEx_Protocol"

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

& qpm-rust package edit --version $Version

Set-Location build
& cmake -G "Ninja" -DCMAKE_BUILD_TYPE="RelWithDebInfo" -DMPEX_PROTOCOL="$MpEx_Protocol" ../
& cmake --build . -j 6
$ExitCode = $LastExitCode
Set-Location ..
exit $ExitCode