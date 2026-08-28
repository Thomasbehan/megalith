# Auto-detect the host project and Engine Version for the primary build
$UProjectFile = Get-ChildItem -Path $ProjectDir -Filter "*.uproject" | Select-Object -First 1
if (-not $UProjectFile) {
    Write-Host "
  [FAIL] No .uproject found in $ProjectDir. The script must be run from inside a host project's Plugins\Monolith\Scripts folder." -ForegroundColor Red
    exit 1
}
$HostProjectName = $UProjectFile.BaseName
$UProjectData = Get-Content $UProjectFile.FullName -Raw | ConvertFrom-Json
$HostEngineAssoc = $UProjectData.EngineAssociation
if (-not $HostEngineAssoc) {
    Write-Host "
  [FAIL] No EngineAssociation found in $($UProjectFile.Name)." -ForegroundColor Red
    exit 1
}

$HostEngineRoot = $null
$RegPath = "HKLM:\SOFTWARE\EpicGames\Unreal Engine\$HostEngineAssoc"
if (Test-Path $RegPath) {
    $HostEngineRoot = (Get-ItemProperty $RegPath).InstalledDirectory
} else {
    $Fallbacks = @("C:\Program Files\Epic Games\UE_$HostEngineAssoc", "C:\Program Files (x86)\Epic Games\UE_$HostEngineAssoc", "C:\Program Files (x86)\UE_$HostEngineAssoc")
    foreach ($path in $Fallbacks) { if (Test-Path $path) { $HostEngineRoot = $path; break } }
}
if (-not $HostEngineRoot) {
    Write-Host "
  [FAIL] Could not locate Unreal Engine $HostEngineAssoc installation." -ForegroundColor Red
    exit 1
}
$HostUBTPath = Join-Path $HostEngineRoot "Engine\Binaries\DotNET\UnrealBuildTool\UnrealBuildTool.exe"

$EngineMatrix = @(
    [PSCustomObject]@{
        Tag        = "UE$HostEngineAssoc"                             # asset/marker engine tag
        UBT        = $HostUBTPath
        Target     = "${HostProjectName}Editor"
        ProjectDir = $ProjectDir                            # this Monolith dev-master
        UProject   = $UProjectFile.FullName
        PluginDir  = $PluginDir                             # this repo's Plugins/Monolith
        Zip        = (Join-Path $ProjectDir "Monolith-v$Version-UE$HostEngineAssoc.zip")
        IsLegacy   = $true                                  # this zip seeds the legacy bridge
    },
    [PSCustomObject]@{
        Tag        = "UE5.8"
        UBT        = 'C:\Program Files (x86)\UE_5.8\Engine\Binaries\DotNET\UnrealBuildTool\UnrealBuildTool.exe'
        Target     = "FIVEPOINT8Editor"
        ProjectDir = 'D:\Unreal Projects\FIVEPOINT8'
        UProject   = 'D:\Unreal Projects\FIVEPOINT8\FIVEPOINT8.uproject'
        PluginDir  = 'D:\Unreal Projects\FIVEPOINT8\Plugins\Monolith'  # the UE5.8 clone (Binaries source)
        Zip        = (Join-Path $ProjectDir "Monolith-v$Version-UE5.8.zip")
        IsLegacy   = $false
    }
)

if ($SkipUE58 -or -not (Test-Path 'D:\Unreal Projects\FIVEPOINT8')) {
    $EngineMatrix = $EngineMatrix | Where-Object { $_.Tag -ne "UE5.8" }
}
