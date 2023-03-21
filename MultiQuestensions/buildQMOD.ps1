Param(
    [Parameter(Mandatory=$false, HelpMessage="The name the output qmod file should have")][String] $qmodname="MultiQuestensions",

    [Parameter(Mandatory=$false, HelpMessage="The version of the mod")][String] $version,

    [Parameter(Mandatory=$false, HelpMessage="Switch to create a clean compilation")]
    [Alias("rebuild")]
    [Switch] $clean,

    [Parameter(Mandatory=$false, HelpMessage="Prints the help instructions")]
    [Switch] $help,

    [Parameter(Mandatory=$false, HelpMessage="Tells the script to not compile and only package the existing files")]
    [Alias("actions", "pack")]
    [Switch] $package
)

# Builds a .qmod file for loading with QP or BMBF


if ($help -eq $true) {
    Write-Output "`"BuildQmod <qmodName>`" - Copiles your mod into a `".so`" or a `".a`" library"
    Write-Output "`n-- Parameters --`n"
    Write-Output "qmodName `t The file name of your qmod"

    Write-Output "`n-- Arguments --`n"

    Write-Output "-clean `t`t Performs a clean build on both your library and the qmod"
    Write-Output "-help `t`t Prints this"
    Write-Output "-package `t Only packages existing files, without recompiling`n"

    exit
}

if ($qmodName -eq "")
{
    Write-Output "Give a proper qmod name and try again"
    exit
}

if ($package -eq $true) {
    $qmodName = "$($env:module_id)_$($env:version)"
    Write-Output "Actions: Packaging QMod $qmodName"
}
if (($args.Count -eq 0) -And $package -eq $false) {
    Write-Output "Creating QMod $qmodName"
    & $PSScriptRoot/build.ps1 -clean:$clean -version:$version

    if ($LASTEXITCODE -ne 0) {
        Write-Output "Failed to build, exiting..."
        exit $LASTEXITCODE
    }

    qpm-rust qmod build
}

Write-Output "Creating qmod from mod.json"

$mod = "./mod.json"
$modJson = Get-Content $mod -Raw | ConvertFrom-Json

$filelist = @($mod)

$cover = "./" + $modJson.coverImage
if ((-not ($cover -eq "./")) -and (Test-Path $cover))
{ 
    $filelist += ,$cover
} else {
    Write-Output "No cover Image found"
}

foreach ($mod in $modJson.modFiles)
{
    $path = "./build/" + $mod
    if (-not (Test-Path $path))
    {
        $path = "./extern/libs/" + $mod
    }
    $filelist += $path
}

foreach ($lib in $modJson.libraryFiles)
{
    $path = "./extern/libs/" + $lib
    if (-not (Test-Path $path))
    {
        $path = "./build/" + $lib
    }
    $filelist += $path
}

$zip = $qmodName + ".zip"
$qmod = $qmodName + ".qmod"

if ((-not ($clean.IsPresent)) -and (Test-Path $qmod))
{
    Write-Output "Making Clean Qmod"
    Move-Item $qmod $zip -Force
}

Compress-Archive -Path $filelist -DestinationPath $zip -Update
Move-Item $zip $qmod -Force

Write-Output "Task Completed"