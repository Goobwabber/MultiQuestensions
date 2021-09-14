$NDKPath = Get-Content $PSScriptRoot/ndkpath.txt -First 1
$VERSION = $env:version

if (!$VERSION) {
    $VERSION = "0.1.0"
}

echo "Building MultiQuestensions Version: $VERSION"

$buildScript = "$NDKPath/build/ndk-build"
if (-not ($PSVersionTable.PSEdition -eq "Core")) {
    $buildScript += ".cmd"
}

& $buildScript NDK_PROJECT_PATH=. APP_BUILD_SCRIPT=./Android.mk NDK_APPLICATION_MK=./Application.mk VERSION=$VERSION