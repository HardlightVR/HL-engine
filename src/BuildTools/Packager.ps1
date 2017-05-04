Add-Type -AssemblyName System.Windows.Forms 


# Welcome to the NullSpace VR build tool. Hopefully this is temporary. But who knows how long it will last?



# If you need to add MS build because of a build step, go download the Invoke-MsBuild script and include it like so
# Set-ExecutionPolicy -Scope Process -ExecutionPolicy Bypass
# Import-Module -Name "C:\Users\NullSpace Team\Documents\NS_Unreal_SDK\NS_Unreal_SDK\Invoke-MsBuild.psm1"



# The location of devenv, which we use for building
$DEVENV = "C:/Program Files (x86)/Microsoft Visual Studio 14.0/Common7/IDE/devenv.exe"
 
 # Table of our different project git repo locations. This script assumes that the projects are git repos.
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


# When we want to tag something with git, we want a friendlier name than what's in the table above
# Only need to fill out what's necessary
$tag_friendly_names = @{

    "unity_sdk" = "Unity_SDK";
    "diagnostic_tool" = "DiagnosticTool";
    "plugin" = "Plugin";
    "installer" = "Service";
    "asset_tool" = "AssetTool";
    "unreal_plugin" = "UnrealPlugin"
}

# Group together some repos to make a product. This means they will be tagged with a common release number when building.
$products =    @{

    "chimera" = "unity_sdk", "installer", "asset_tool", "unreal_plugin"; 
    "installer" = @("installer");

}

# We need to edit the registry in order for external builds to work
function do_registry_hack() {
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
}

# Generate the little header that appears above release notes. Useful for figuring out what internal versions correspond to a marketing release. 
function generate_chimera_version_header($chimera_version, $service, $plugin, $unity, $asset_tool) {
    $output_str = "Chimera SDK $chimera_version`n"
    $output_str += "------------------------`n"
    $output_str += "Service = $service`n"
    $output_str += "Plugin = $plugin`n"
    $output_str += "Unity SDK = $unity`n"
    $output_str += "Asset Tool = $asset_tool`n"
    $output_str += "`n"
    return $output_str;
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

# Copy the directories in $dir_list recursively into $destination 
function copy_all_dirs($source, $dir_list, $destination) {
    Foreach ($dir in $dir_list) {
        $source_path = Join-Path $source $dir
        Copy-Item $source_path $destination -Recurse
    }
}

# Tags a git repo with string $tag and message $tag_msg
# Attempts to correct some simple mistakes e.g. including a ':' or '*' (replaces with '_')
function tag([String] $git_repo, [string]$tag, [string]$tag_msg) { 

    #some common things that aren't allowed in tags. Not the full list. See here
    #http://stackoverflow.com/questions/26382234/what-names-are-valid-git-tags
    $tag = ($tag -replace "\s|~|:|\^|\?|\*", '_')

    Push-Location -Path $git_repo
    if ([string]::IsNullOrEmpty($tag_msg)) {
        $tag_msg = "(No message)"
    }  


    $cmd = "git tag -a `"$tag`" -m `"$tag_msg`""
    Invoke-Expression $cmd 
    Pop-Location

}


####################
# Each product has a make_x function. It is responsible for making that project and pulling in dependencies, for some definition of make. 
# Right now it's mostly copying build results
# Convention: $essential_files contains a list of the essential files in the product
#             $essential_folders contains a list of the essential folder(s) necessary. If I include "whatever", then I want ALL the files in "whatever". 
####################




$make_diagnostics_tool = { 
    param([HashTable]$dirs, [String] $destination, [HashTable] $options) 

    $essential_files = "DiagnosticsTool.exe", "NSLoader.dll", "PadToZone.json", "VertexShader.txt", "FragmentShader.txt", "Zones.json", "imgui.ini"
    
    copy_all (Join-Path $dirs["diagnostic_tool"] "build/bin/Release/Win32") $essential_files $destination

}

# More complex build step
# This function bumps the version numbers for chimera and the service, then rebuilds the project. 
# Finally, it copies the output like all the other make_x's 
$make_installer = {
    param([HashTable]$dirs, [String] $destination, [HashTable] $options) 


    $new_service_version = $options["service_version_bump"];
    $new_chimera_version = $options["chimera_version_bump"];

    # Load up the resx XML from the project
    $resx_file_path = (Join-Path $dirs["installer"] "NSVRGui\Properties\Resources.resx")
    [xml] $resx = Get-Content $resx_file_path

    # If we fail to read it, then we will need the user to do the version bump themselves.
    if ($resx.root.data -eq $null) {
        Write-Host "Failed to read xml from the service gui's resx! Cannot bump versions automatically."
        Write-Host "Open up the GUI project, and navigate to the resources file. Use the helpful GUI to change the version strings."
        Write-Host "    Service version: >> $new_service_version <<"
        Write-Host "    Chimera version: >> $new_chimera_version <<"
        Read-Host "Hit enter once that's done [enter]";
    } else {

        # Read the previous versions 
        $old_service_version = ($resx.root.data|where {$_.name -eq "ServiceVersion"})
        $old_chimera_version = ($resx.root.data|where {$_.name -eq "ChimeraVersion"})

        Write-Host "Bumping service $($old_service_version.value) --> $($new_service_version)"

        $old_service_version.Value = $new_service_version;

        Write-Host "Bumping chimera $($old_chimera_version.value) --> $($new_chimera_version)"

        $old_chimera_version.Value = $new_chimera_version;

        $resx.Save($resx_file_path)
    }



    $sln = (Join-Path $dirs["installer"] "NSVRService.sln") 
      
    Write-Host "Rebuilding the installer.."
    & "$DEVENV" $sln /Rebuild Release /project (Join-Path $dirs["installer"] "NSVRServiceSetup\NSVRServiceSetup.vdproj") | Out-Null
    Write-Host "Finished."

    # Now we can finally copy everything!

    $essential_files = "setup.exe", "NSVRServiceSetup.msi"
    $essential_folders = "vcredist_x86", "vcredist_x64", "DotNetFX45"

    $build_path = Join-Path $dirs["installer"] "NSVRServiceSetup\Release"

    Copy-Item -Path (Join-Path $dirs["installer"] "release_notes.txt") -Destination $destination
    copy_all $build_path $essential_files $destination
    copy_all_dirs $build_path $essential_folders $destination

}

function get_file_size_kb($path) {
    $file = Get-Item $path
    return ($file.length/1024)
}
$make_unrealplugin = {
        param([HashTable]$dirs, [String] $destination, [HashTable] $options) 
        $plugin_name = "HapticSuit"
        $temp_path = ""

        $max_file_size = 3000 #kb, should be changed in future if files get bigger. This is simply a heuristic to catch a bad problem of releasing products with debug dlls by accident

        $needs_package = Read-Host "Did you already package the plugin?"
        if (-not ($needs_package -contains "y")) {
            $temp_build_path = "$Env:USERPROFILE\Documents\HardlightPluginTemp"
            New-Item -ItemType Directory -Force -Path $temp_build_path | Out-Null
            Write-Host "We're gonna do this together. I've made a temp folder at $temp_build_path"
            Read-Host "Go into Unreal, and package the plugin to that directory. When done, hit [enter]"
            $temp_path = $temp_build_path

        } else {
            $temp_path = Read-Host "Enter the path:"
            Write-Host "Path given: $temp_path"
        }

        Write-Host "Copying the ThirdParty directory from the engine/plugin repo into this temp folder.."
        Copy-Item -Path (Join-Path $dirs["unreal_plugin"] "ThirdParty") -Destination (Join-Path $temp_path $plugin_name) -Recurse

        Write-Host "Creating Binaries folder.."
        #New-Item -ItemType Directory -Force -Path (Join-Path $temp_path "Binaries") | Out-Null

        New-Item -ItemType Directory -Force -Path (Join-Path $temp_path "Binaries/Win32") | Out-Null
        New-Item -ItemType Directory -Force -Path (Join-Path $temp_path "Binaries/Win64") | Out-Null

        $win32_dll = [io.path]::combine($temp_path, $plugin_name, "ThirdParty/NullSpaceVR/lib/Win32/NSLoader.dll")
        $win64_dll = [io.path]::combine($temp_path, $plugin_name, "ThirdParty/NullSpaceVR/lib/Win64/NSLoader.dll")

        $win32_size = get_file_size_kb $win32_dll
        $win64_size = get_file_size_kb $win64_dll

        if ($win32_size -gt $max_file_size) {
            Write-Host "WARNING: The win32 dll is larger than 3000kb, which means it may be a DEBUG dll! Do not distribute!"
        }
        if ($win64_size -gt $max_file_size) {
            Write-Host "WARNING: The win64 dll is larger than 3000kb, which means it may be a DEBUG dll! Do not distribute!"
        }


        Copy-Item -Path $win32_dll -Destination (Join-Path $temp_path "Binaries/Win32") | Out-Null
        Copy-Item -Path $win64_dll -Destination (Join-Path $temp_path "Binaries/Win64") | Out-Null

        $essential_folders = "Binaries", $plugin_name

        New-Item -ItemType Directory -Force -Path (Join-Path $destination "Unreal Hardlight Plugin") |Out-Null
        copy_all_dirs $temp_path $essential_folders (Join-Path $destination "Unreal Hardlight Plugin")
        
        
}

$make_unitypackage= {
    param([HashTable]$dirs, [String] $destination, [HashTable] $options) 
    $unity_package_name = Read-Host "What is the name of the unitypackage? No need to include .unitypackage"
    if (-not $unity_package_name) {
        Write-Host "Invalid package name!"
    }
    $unity_package_name = "$($unity_package_name).unitypackage"
    if (-not (Test-Path (Join-Path $repo_directories["unity_sdk"] $unity_package_name))) {
        Write-Host "That package doesn't seem to exist. Not copying in the unitypackage."
    }

    $essential_files = @($unity_package_name)
    copy_all $repo_directories["unity_sdk"] $essential_files $destination

}


$make_assettool = {
    param([HashTable]$dirs, [String] $destination, [HashTable] $options) 

    $essential_files = @("HapticAssetTools.exe")
    $release_dir = Join-Path $repo_directories["asset_tool"] "Build/bin/Release/Win32"
    Write-Host "Release dir: $release_dir"
    copy_all $release_dir $essential_files $destination

}
$build_functions = @{
    "diagnostic_tool" = $make_diagnostics_tool;
    "installer" = $make_installer;
    "unitypackage" = $make_unitypackage;
    "asset_tool" = $make_assettool;
    "unreal_plugin" = $make_unrealplugin;
}

# Given a version tage SomeProduct_v0.1.3, return 0.1.3
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

# Given a bunch of tags like v0.1.2, v0.1.3, return the latest, i.e. v0.1.3 in this case
function latest_tag_name([string]$repo, [string] $prefix) {


   Push-Location $repo_directories[$repo]
  
    $cmd = "git config --global versionsort.prereleaseSuffix `"-rc`""
    iex $cmd
    $cmd = "git tag -l --sort=-version:refname `"$($prefix)_v*`""
    $result = Invoke-Expression $cmd
    if (-not $result) {
        Pop-Location
        return "(No version tag found)"
    }
    $latest_tag = $result.Split(' ')[0]

    Pop-Location

    return $latest_tag
}

function latest_tag_commit([string] $repo, [string] $prefix) {
     Push-Location $repo_directories[$repo]
  
  
    $commit = iex "git rev-parse HEAD"
    Pop-Location
    if (-not $commit) {
        return "(no commit found?!)"
    } else {
        return $commit
    }
}

# Wrapper around a make_x function. Handles tagging and creating the output directory.
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

    [string] $chimera_version = Read-Host "You're planning a new Chimera release. What shall be the marketing version number?"

    Write-Host "Great, version >>>>> $chimera_version <<<<<`n"

    Write-Host "This release will contain the following products:`n"
    Foreach ($repo in $products["chimera"]) {
        $branch = get_git_branch $repo
        $release = "[$branch] $(latest_tag_name $repo $tag_friendly_names[$repo]) ($(latest_tag_commit $repo $tag_friendly_names[$repo]))"
        Write-Host "$repo = $release"
      #  Write-Host (number_from_version_tag $release)
    }

    $location = "C:\Users\NullSpace Team\Documents\Visual Studio 2015\Projects\DiagnosticsTool\Release\TestBuild"

    $result = Read-Host "Does this configuration look correct? [y/n]"
    if ($result -contains "y") {
        $service_version =  number_from_version_tag (latest_tag_name "installer" $tag_friendly_names["installer"])
        $plugin_version = number_from_version_tag (latest_tag_name "plugin" $tag_friendly_names["plugin"])
        $unity_version = number_from_version_tag (latest_tag_name "unity_sdk" $tag_friendly_names["unity_sdk"])
        $unreal_version = number_from_version_tag (latest_tag_name "unreal_plugin" $tag_friendly_names["unreal_plugin"])

        $assettool_version = number_from_version_tag (latest_tag_name "asset_tool" $tag_friendly_names["asset_tool"])


        $options["chimera_version_bump"] = $chimera_version;
        $options["service_version_bump"] = $service_version

        # we don't do_build the asset tool because it is pulled in by the installer
        # do_build "asset_tool" $options $location 
        # perhaps do_build is bad name, but it is unclear if we should have this tool building stuff or just packaging for now.
        do_build "installer" $options $location
        do_build "unitypackage" $options $location
        do_build "unreal_plugin" $options $location

        $header_text = generate_chimera_version_header $chimera_version $service_version $plugin_version $unity_version $assettool_version
        New-Item (Join-Path $location "versions.txt") -type file -force -value $header_text | Out-Null

        Write-Host "`nFinished packaging chimera at location $location"
        $tag_all = Read-Host "Want to tag everything with the chimera release number? [y/n]"
        if ($tag_all -contains "y") {
             Foreach ($repo in $products["chimera"])  {
                $dir = $repo_directories[$repo]
                tag $dir "Chimera_v$($chimera_version)"
             }
        }

    } 

}


$diagnostics_wizard = {
      param([HashTable] $options)

      Write-Host "Just do it yourself"
}
$product_wizards = @{
    "chimera" = $chimera_wizard;
    "diagnostics" = $diagnostics_wizard;

}

function Main(){
    do_registry_hack

    $options = @{
        "tag_repo" = "TestTag";
        "tag_msg"   = "test";
       
    }


    $product = Read-Host "Welcome to the release wizard. Which product? Options: chimera"

    $product_wizards[$product].Invoke($options);

 

}


Main




