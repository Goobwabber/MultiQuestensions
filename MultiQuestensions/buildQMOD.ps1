# Builds a .qmod file for loading with QP
if ($args.Count -eq 0) {
$ModID = "multiquestensions"
$BSHook = "2_0_3"
$VERSION = "0.1.0"
echo "Compiling Mod"
& $PSScriptRoot/build.ps1
}

# TODO: Get the below working with Github Actions variables.
if ($args[0] -eq "--package") {
    $ModID = $env:module_id
    $BSHook = $env:bs_hook
    $VERSION = $env:version

    echo "Actions: Packaging QMod with ModID: $ModID and BS-Hook version: $BSHook"
        Compress-Archive -Path "./libs/arm64-v8a/lib$ModID.so", "./libs/arm64-v8a/libbeatsaber-hook_$BSHook.so", ".\mod.json" -DestinationPath "./Temp$ModID_v$VERSION.zip" -Update
        Move-Item "./Temp$ModID_v$VERSION.zip" "./$ModID_v$VERSION.qmod" -Force
}
if ($args.Count -eq 0) {
echo "Packaging QMod with ModID: $ModID"
    Compress-Archive -Path "./libs/arm64-v8a/lib$ModID.so", "./libs/arm64-v8a/libbeatsaber-hook_$BSHook.so", ".\mod.json" -DestinationPath "./Temp$ModID_v$VERSION.zip" -Update
    Move-Item "./Temp$ModID_v$VERSION.zip" "./$ModID_v$VERSION.qmod" -Force
}
echo "Task Completed"