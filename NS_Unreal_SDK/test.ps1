Add-Type -AssemblyName System.Windows.Forms 

Set-ExecutionPolicy -Scope Process -ExecutionPolicy Bypass
Import-Module -Name "C:\Users\NullSpace Team\Documents\NS_Unreal_SDK\NS_Unreal_SDK\Invoke-MsBuild.psm1"



$registryPath = "HKCU:\SOFTWARE\Microsoft\VisualStudio\14.0_Config\MSBuild"
$name = "EnableOutOfProcBuild"
$value = 0

if(!(Test-Path $registryPath)) {
    New-Item -Path $registryPath -Force | Out-Null
    New-ItemProperty -Path $registryPath -Name $name -Value $value -PropertyType DWORD -Force | Out-Null
}
else {
    New-ItemProperty -Path $registryPath -Name $name -Value $value -PropertyType DWORD -Force | Out-Null
}

$devenv = "C:/Program Files (x86)/Microsoft Visual Studio 14.0/Common7/IDE/devenv.exe"
 $repo_directories = @{
        # for the Unity SDK, .unitypackage
        "unity_sdk" = "$Env:USERPROFILE\Documents\NullSpace SDK 0.1.1";

        # For driver.dll, our userland driver (ignore the bad name of NS_Unreal_SDK)
        "driver" = "$Env:USERPROFILE\Documents\NS_Unreal_SDK";

        # For our C# installer of the service
        "installer" = "$Env:USERPROFILE\Documents\Visual Studio 2015\Projects\NSVRService";

        # For our C# wrapper over our C API
        "csharp_wrapper" = "$Env:USERPROFILE\Documents\Visual Studio 2015\Projects\NSLoaderWrapper";

        # For our C API
        "plugin" = "$Env:USERPROFILE\Documents\Visual Studio 2015\Projects\NSLoader";

        # The destination repo for the Chimera SDK
        "public_chimera" = "$Env:USERPROFILE\Documents\NullSpace-Chimera-SDK";

        # AssetTool for creating haptic assets
        "asset_tool" = "$Env:USERPROFILE\Documents\Visual Studio 2015\Projects\HapticAssetTools";

        # Diagnostics tool for determining problems with suit
        "diagnostic_tool" = "$Env:USERPROFILE\Documents\Visual Studio 2015\Projects\DiagnosticsTool";

        # The UE plugin
        "unreal_plugin" = "D:\UnrealEngine-release\Engine\Plugins\Runtime\HapticSuit";


    }

$tag_friendly_names = @{

    "unity_sdk" = "Unity_SDK";
    "diagnostic_tool" = "DiagnosticTool";
    "plugin" = "Plugin";
    "installer" = "Service";
    "asset_tool" = "AssetTool";
}

$products =    @{

    "chimera" = "unity_sdk", "installer", "asset_tool";
    "installer" = @("installer");

}
# Copy all files in $file_list from the directory $source to the directory $destination
function copy_all($source, $file_list, $destination)
{
    Foreach ($file in $file_list) {
        $source_path = Join-Path $source $file
        $destination_path = Join-Path $destination $file
        Copy-Item -Path $source_path -Destination $destination_path
    }
}

# copy the directories in $dir_list recursively into $destination 
function copy_all_dirs($source, $dir_list, $destination) {
    Foreach ($dir in $dir_list) {
        $source_path = Join-Path $source $dir
        Copy-Item $source_path $destination -Recurse
    }
}


function tag([String] $git_repo, [string]$tag, [string]$tag_message) { 

    #some common things that aren't allowed in tags. Not the full list. See here
    #http://stackoverflow.com/questions/26382234/what-names-are-valid-git-tags
    $tag = ($tag -replace "\s|~|:|\^|\?|\*", '_')

    Push-Location -Path $git_repo
    if (-not $tag_msg) {
        $tag_msg = "(No message)"
    }  
    $cmd = "git tag -a `"$tag`" -m `"$tag_message`""

   Invoke-Expression $cmd 

    Pop-Location

}

$make_diagnostics_tool = { 
    param([HashTable]$dirs, [String] $destination, [HashTable] $options) 

    $essential_files = "DiagnosticsTool.exe", "NSLoader.dll", "PadToZone.json", "VertexShader.txt", "FragmentShader.txt", "Zones.json", "imgui.ini"
    
    copy_all (Join-Path $dirs["diagnostic_tool"] "Release") $essential_files $destination

}


$make_installer = {
    param([HashTable]$dirs, [String] $destination, [HashTable] $options) 


    $new_service_version = $options["service_version_bump"];
    $new_chimera_version = $options["chimera_version_bump"];


    $resx_file_path = (Join-Path $dirs["installer"] "NSVRGui\Properties\Resources.resx")
    [xml] $resx = Get-Content $resx_file_path

    if ($resx.root.data -eq $null) {
        Write-Host "Failed to read xml from the service gui's resx! Cannot bump versions automatically. You'll have to do it manually."
        Read-Host "Hit enter once you've done it [enter]";
    } else {
        $service_version = ($resx.root.data|where {$_.name -eq "ServiceVersion"})
        $distribution_version = ($resx.root.data|where {$_.name -eq "ChimeraVersion"})

        Write-Host "Bumping service $($service_version.value) --> $($new_service_version)"

        $service_version.Value = $new_service_version;

        Write-Host "Bumping chimera $($distribution_version.value) --> $($new_chimera_version)"

        $distribution_version.Value = $new_chimera_version;

        $resx.Save($resx_file_path)
        
        
    }

    $sln = (Join-Path $dirs["installer"] "NSVRService.sln") 
      
    Write-Host "Rebuilding the installer, just for you.."
    & "$devenv" $sln /Rebuild Release /project (Join-Path $dirs["installer"] "NSVRServiceSetup\NSVRServiceSetup.vdproj") | Out-Null
    Write-Host "Finished."

    $essential_files = "setup.exe", "NSVRServiceSetup.msi"
    $essential_folders = "vcredist_x86", "vcredist_x64", "DotNetFX45"

    $build_path = Join-Path $dirs["installer"] "NSVRServiceSetup\Release"

    copy_all $build_path $essential_files $destination
    copy_all_dirs $build_path $essential_folders $destination

}

$make_unitypackage= {
    param([HashTable]$dirs, [String] $destination, [HashTable] $options) 
    $unity_package_name = Read-Host "What is the name of the unitypackage? No need to include .unitypackage"
    if (-not $unity_package_name) {
        Write-Host "Invalid package name!"
    }
    $unity_package_name = "$($unity_package_name).unitypackage"
    if (-not (Test-Path (Join-Path $repo_directories["unity_sdk"] $unity_package_name))) {
        Write-Host "That package doesn't seem to exist"
    }

    $essential_files = @($unity_package_name)
    copy_all $repo_directories["unity_sdk"] $essential_files $destination

}


$make_assettool = {
    param([HashTable]$dirs, [String] $destination, [HashTable] $options) 

    $essential_files = @("HapticAssetTools.exe")
    copy_all $repo_directories["asset_tool"] $essential_files $destination

}
$build_functions = @{
    "diagnostic_tool" = $make_diagnostics_tool;
    "installer" = $make_installer;
    "unitypackage" = $make_unitypackage;
    "asset_tool" = $make_assettool;
}

function number_from_version_tag([string]$tag) {
    $index_of_v = $tag.IndexOf("_v")
    return $tag.Substring($index_of_v+1)
}

function get_git_branch([string] $repo) {
    Push-Location $repo_directories[$repo]
    $branch = iex "git rev-parse --abbrev-ref HEAD"
    Pop-Location
    return $branch;
}
function latest_tagged_git_release([string]$repo, [string] $prefix) {

   Push-Location $repo_directories[$repo]
  
  
    $commit = iex "git rev-parse HEAD"
    $cmd = "git config --global versionsort.prereleaseSuffix `"-rc`""
    iex $cmd
    $cmd = "git tag -l --sort=-version:refname `"$($prefix)_v*`""
    $result = Invoke-Expression $cmd
    if (-not $result) {
        Pop-Location
        return "(No version tag found) ($commit)"
    }
    $latest_tag = $result.Split(' ')[0]

    
    $latest_tag = "$latest_tag ($commit)"
    Pop-Location

    return $latest_tag
}

function do_build([String] $component_name, [HashTable] $options, [String] $path) {

    Write-Host "------- Packaging $component_name -------"
    $delegate = $build_functions[$component_name]

    if ($options.ContainsKey("tag_repo")) {
        tag $repo_directories[$component_name] $options["tag_repo"] $options["tag_msg"]
    }

 # create the directory where we will store the final result 
    New-Item -ItemType Directory -Force -Path $path | Out-Null
    $delegate.Invoke($repo_directories, $path, $options)

}



$chimera_wizard = {
    param([HashTable] $options)

    $chimera_version = Read-Host "You're planning a new Chimera release. What shall be the marketing version number?"

    Write-Host "Great, version >>>>> $chimera_version <<<<<`n"

    Write-Host "This release will contain the following products:`n"
    Foreach ($repo in $products["chimera"]) {
        $branch = get_git_branch $repo
        $release = "[$branch] $(latest_tagged_git_release $repo $tag_friendly_names[$repo])"
        Write-Host "$repo = $release"
      #  Write-Host (number_from_version_tag $release)
    }


    $result = Read-Host "Does this configuration look correct? [y/n]"
    if ($result -contains "y") {
        do_build "installer" $options "C:\Users\NullSpace Team\Documents\Visual Studio 2015\Projects\DiagnosticsTool\Release\TestBuild"
        do_build "unitypackage" $options "C:\Users\NullSpace Team\Documents\Visual Studio 2015\Projects\DiagnosticsTool\Release\TestBuild"
        do_build "asset_tool" $options "C:\Users\NullSpace Team\Documents\Visual Studio 2015\Projects\DiagnosticsTool\Release\TestBuild"


    } 

}
$product_wizards = @{
    "chimera" = $chimera_wizard;

}

function Main(){

    $options = @{
        "tag_repo" = "TestTag";
        "tag_msg"   = "test";
        "service_version_bump" = "0.0.1";
        "chimera_version_bump" = "0.1.4";
    }


    $product = Read-Host "Welcome to the release wizard. Which product?"

    $product_wizards[$product].Invoke($options);

    #do_build "installer" $options "C:\Users\NullSpace Team\Documents\Visual Studio 2015\Projects\DiagnosticsTool\Release\TestBuild"
   # do_build "diagnostic_tool" $options "C:\Users\NullSpace Team\Documents\Visual Studio 2015\Projects\DiagnosticsTool\Release\TestBuild"
  #  do_build "unitypackage" $options "C:\Users\NullSpace Team\Documents\Visual Studio 2015\Projects\DiagnosticsTool\Release\TestBuild"

}


Main




