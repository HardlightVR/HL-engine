[CmdletBinding()]
Param (
     [Parameter(Mandatory=$True)]
    [string]$tag,
    [string]$product,
    [string]$message,
    [switch]$bundle,
    [switch]$release
)
$current_location = $(Get-Location)

function Abort() {
    Set-Location $current_location
    Exit
}
function CheckPreconditions([HashTable]$repos) {
    foreach ($repo in $repos.GetEnumerator()) {
        if (-not (Test-Path $repo.Value)) {
            Write-Host "Could not find repo '$($repo.Key)' (path = $($repo.Value))"
            Abort
        }
    }
    foreach ($repo in $repos.GetEnumerator()) {
        Set-Location -path $repo.Value
       # Write-Host "At repo $($repo.Key):"
        $result = iex "git rev-parse --abbrev-ref HEAD"
        if ($result -eq "master") {
           # Write-Host "        master is checked out!"
        }else {
            Write-Host "At repo $($repo.Key):"
            Write-Host "        Error: branch master must be checked out. Aborting."
            Abort
        }
    }
}
function GetLatestRelease([string]$selected_product, [HashTable]$groups, [HashTable]$repos) {
    $first_product = $groups[$selected_product][0]
    Write-Host "Selecting first component in group: $first_product"
    $tag = GetLatestTag $selected_product $repos[$first_product]
    Write-Host "    Latest version for $selected_product found: $tag"
    return $tag;
}

function GetLatestTag([string]$prefix, [string]$repo_path) {
    $cmd = "git tag -l --sort=-version:refname `"$($prefix)_v*`""
   # Write-Host "    Using command: $cmd"
    Set-Location $repo_path
    $result = iex $cmd
    $latest_tag = $result.Split(' ')[0]
    return $latest_tag
}

function FormatTag([string]$tag) {
    $index_of_v = $tag.IndexOf("_v")
    return $tag.Substring($index_of_v+1)
}

function Tag([HashTable]$repos, [string]$tag, [string]$tag_message) {
    $confirm = Read-Host "Are you sure you want to tag $($repos.Keys) ? (y/n)"
    if ($confirm -eq 'y') {
        foreach ($repo in $repos.GetEnumerator()) {
            Set-Location -path $repo.Value
            $cmd = "git tag -a $tag -m `"$tag_message`""
            Write-Host "On repo $($repo.Key), running command: $cmd"
            $result = iex $cmd 
            if ($result) {
                Write-Host $result
                Abort
            } 
        }
    }

}

function BumpVersion($file_path, $contents)
 {
    if (Test-Path $file_path) {
        Add-Content -Path $file_path -Value $contents
    } else {
        New-Item -Path $file_path -Value $contents
    }
 }

 function AssembleChimera([string]$service_version, [string]$unity_package_path, [string]$installer_path, [string]$public_chimera_path) {
    $confirm = Read-Host "Pull just in case someone updated the wiki. [enter]"
    $confirm = Read-Host "Make sure that the version file is updated for the installer. [enter]"
    $confirm = Read-Host "Make sure that the version info is updated within the release notes dialog in the GUI. [enter]"
    $confirm = Read-Host "Rebuild the installer and do your testing. [enter]"
    $confirm = Read-Host "Update the readme [enter]"
    $package_name = Read-Host "Build the Unity Package and put in the sdk repo root path. Write the name of the package [enter]"
    if (-not $package_name.Contains($service_version)) {
        Write-Host "It looks like the package you built is not the same version that was assembled ($service_version)! Aborting."
        Abort
    }

    $path = $unity_package_path + '/' + $package_name

    if (-not (Test-Path $path)) {
        Write-Host "Could not find the unity package at $path. Aborting."
        Abort
    }

    $chimera_unitypackage_path = ($public_chimera_path + '/' + $package_name)
    $chimera_installer_path = ($public_chimera_path + '/' + "NullSpaceVR Service Installer")
    Copy-Item $path $chimera_unitypackage_path -force
    Remove-Item -Recurse -Force $chimera_installer_path
    Copy-Item ($installer_path + '/' + "Release") $chimera_installer_path -Recurse -force
    #precondition: the version info has been built into the installer
    # 3 components needed: 
    # the installer output
    # the unity package
    # the readme
 }

function Main() {
    $release_groups = @{
        "Service" = "installer", "engine";
        "Plugin" = @("plugin");
        "Unity_SDK" = "csharp_wrapper", "unity_sdk";
        "Chimera" = "unity_sdk", "engine", "installer", "csharp_wrapper", "plugin";
    }

    $repo_directories = @{
        "unity_sdk" = "$Env:USERPROFILE\Documents\NullSpace SDK 0.1.1";
        "engine" = "$Env:USERPROFILE\Documents\NS_Unreal_SDK";
        "installer" = "$Env:USERPROFILE\Documents\Visual Studio 2015\Projects\NSVRService";
        "csharp_wrapper" = "$Env:USERPROFILE\Documents\Visual Studio 2015\Projects\NSLoaderWrapper";
        "plugin" = "$Env:USERPROFILE\Documents\Visual Studio 2015\Projects\NSLoader";
        "public_chimera" = "$Env:USERPROFILE\Documents\NullSpace-Chimera-SDK";

    }

    $messages = @{
        "Service" = "Runtime, GUI, and Engine $tag";
        "Plugin" = "Plugin $tag";
        "Unity_SDK" = "Unity SDK $tag";
        "Chimera" = "Chimera SDK $tag";
    }

    if ($product) {
        if (-not $message) {
            $message = $messages[$product]
        }

        # so we can easily use git? may not be necessary
        New-Alias -Name git -Value "$Env:ProgramFiles\Git\bin\git.exe"

        $component_repo_names = $release_groups[$product]
        $component_repos = @{}
        foreach ($repo in $component_repo_names) {
            $component_repos[$repo] = $repo_directories[$repo]
        }

        # make sure the repos are all on master, etc.
        CheckPreconditions $component_repos

        if ($release) {
            Tag $component_repos "$($product)_$($tag)" $message
        }

    }


    if ($bundle) {
        $latest_service_release = FormatTag (GetLatestRelease "Service" $release_groups $repo_directories)
        $latest_plugin_release = FormatTag (GetLatestRelease "Plugin" $release_groups $repo_directories)
        $latest_unitysdk_release = FormatTag (GetLatestRelease "Unity_SDK" $release_groups $repo_directories)

        Write-Host "Creating version string"
        Write-Host "Chimera SDK $tag"
        Write-Host "------------------------"
        Write-Host "Service = $latest_service_release"
        Write-Host "Plugin = $latest_plugin_release"
        Write-Host "Unity SDK = $latest_unitysdk_release"

        $output_str = "Chimera SDK $tag`n"
        $output_str += "------------------------`n"
        $output_str += "Service = $latest_service_release`n"
        $output_str += "Plugin = $latest_plugin_release`n"
        $output_str += "Unity SDK = $latest_unitysdk_release`n`n"
        # BumpVersion ($repo_directories["installer"] + "\versions.txt")
        AssembleChimera $latest_unitysdk_release $repo_directories["unity_sdk"] ($repo_directories["installer"] + '/' + "NSVRServiceSetup") $repo_directories["public_chimera"]
        Write-Host "`nDone."
    }

    Set-Location $current_location


}


Main






