# Builds a .qmod file for loading with QP
if ($args.Count -eq 0) {
$ModID = "multiquestensions"
$BSHook = "2_3_0"
$VERSION = "0.1.0"
$new_VERSION = Read-Host -Prompt "Input desired mod version (leave empty for default: '$VERSION')"
$bs_hook_version = $BSHook.Replace("_", ".")
$new_BSHook = Read-Host -Prompt "Input current bs-hook version (leave empty for default: '$bs_hook_version')"
if ($new_VERSION -ne "") {
    $VERSION = $new_VERSION
}
if ($new_BSHook -ne "") {
    $BSHook = $new_BSHook.Replace(".", "_")
}
Write-Host "Compiling Mod"
& $PSScriptRoot/build.ps1 -Version $VERSION
}

# TODO: Get the below working with Github Actions variables.
if ($args[0] -eq "--package") {
    $ModID = $env:module_id
    $BSHook = $env:bs_hook
    $VERSION = $env:version
    Write-Host "Github Actions Package started"
}
# Checks if the build was successful
if ($?) {
    # Checks if any needed files are missing
    if ((Test-Path "./libs/arm64-v8a/libbeatsaber-hook_$BSHook.so", "./libs/arm64-v8a/lib$ModID.so", "./mod.json") -contains $false) {
        Write-Host "The following files are missing"
        if (!(Test-Path "./libs/arm64-v8a/libbeatsaber-hook_$BSHook.so")) {
            Write-Host "./libs/arm64-v8a/libbeatsaber-hook_$BSHook.so"
        }
        if (!(Test-Path "./libs/arm64-v8a/lib$ModID.so")) {
            Write-Host "./libs/arm64-v8a/lib$ModID.so"
        }
            if (!(Test-Path ".\mod.json")) {
            Write-Host ".\mod.json"
        }
        Write-Host "Task Failed"
        exit 1;
    }
    else {
    # If we have all files needed, go ahead and packe into qmod
        if ($args.Count -eq 0 -or $args[0] -eq "--package") {
            Write-Host "Upating mod.json"
            $json = Get-Content $PSScriptRoot/mod.json -raw | ConvertFrom-Json
            $json.version="$VERSION"
            $json.libraryFiles=@("libbeatsaber-hook_$BSHook.so")
            $json | ConvertTo-Json -depth 32| Set-Content $PSScriptRoot/mod.json

            Write-Host "Packaging QMod with ModID: $ModID Version: $VERSION and BS-Hook version: $BSHook"
            Compress-Archive -Path "./libs/arm64-v8a/lib$ModID.so", "./libs/arm64-v8a/libbeatsaber-hook_$BSHook.so", ".\mod.json" -DestinationPath "./Temp$ModID_v$VERSION.zip" -Update
            Move-Item "./Temp$ModID_v$VERSION.zip" "./$ModID_v$VERSION.qmod" -Force
        }
        Write-Host "Task Completed"
    }
}
else {
    Write-Host "Build failed, see output"
}