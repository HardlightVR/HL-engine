param (
	[switch]$debug,
	[switch]$release
)

$loader_dll = 'NSLoader.dll'
$loader_root_dir = 'C:\Users\NullSpace Team\Documents\Visual Studio 2015\Projects\NSLoader'
$loader_x86_dir = $loader_root_dir + '\'
$loader_x64_dir = $loader_root_dir + '\x64\'

Write-Host "For Loader DLL:" `n "x86:"  $loader_x86_dir `n "x64: " $loader_x64_dir `n

$wrapper_dll = 'NSLoaderWrapper.dll'
$wrapper_root_dir = 'C:\Users\NullSpace Team\Documents\Visual Studio 2015\Projects\NSLoaderWrapper\NSLoaderWrapper'
$wrapper_x86_dir = $wrapper_root_dir + '\bin\x86\'
$wrapper_x64_dir = $wrapper_root_dir + '\bin\x64\'


Write-Host "For Wrapper DLL:" `n "x86:"  $wrapper_x86_dir `n "x64: " $wrapper_x64_dir `n


$API_root_dir = 'C:\Users\NullSpace Team\Documents\NullSpace SDK 0.1.1\Assets\NullSpace SDK\Plugins'
$API_x86_dir = $API_root_dir + '\x86\'
$API_x64_dir = $API_root_dir + '\x86_64\'

Write-Host "For Unity PLugin Folder:" `n "x86:"  $API_x86_dir `n "x64: " $API_x64_dir `n


function copy_debug_to_wrapper {
	Copy-Item -Path $loader_x86_dir"Debug\"$loader_dll -Destination $wrapper_x86_dir"Debug\"
	Copy-Item -Path $loader_x64_dir"Debug\"$loader_dll -Destination $wrapper_x64_dir"Debug\"
}

function copy_release_to_wrapper {
	Copy-Item -Path $loader_x86_dir"Release\"$loader_dll -Destination $wrapper_x86_dir"Release\"
	Copy-Item -Path $loader_x64_dir"Release\"$loader_dll -Destination $wrapper_x64_dir"Release\"

}
function copy_debug_to_unity {
	Copy-Item -Path $wrapper_x86_dir"Debug\"$wrapper_dll -Destination $API_x86_dir
	Copy-Item -Path $wrapper_x86_dir"Debug\"$loader_dll -Destination $API_x86_dir

	Copy-Item -Path $wrapper_x64_dir"Debug\"$wrapper_dll -Destination $API_x64_dir
	Copy-Item -Path $wrapper_x64_dir"Debug\"$loader_dll -Destination $API_x64_dir

}

function copy_release_to_unity {
	Copy-Item -Path $wrapper_x86_dir"Release\"$wrapper_dll -Destination $API_x86_dir
	Copy-Item -Path $wrapper_x86_dir"Release\"$loader_dll -Destination $API_x86_dir

	Copy-Item -Path $wrapper_x64_dir"Release\"$wrapper_dll -Destination $API_x64_dir
	Copy-Item -Path $wrapper_x64_dir"Release\"$loader_dll -Destination $API_x64_dir

}

if ($debug) {
	Write-Host "Copying from loader to wrapper.."
	copy_debug_to_wrapper
	Write-Host "Copying from wrapper to unity.."
	copy_debug_to_unity
	Write-Host "Done!"
}

if ($release) {
	Write-Host "Copying from loader to wrapper.."
	copy_release_to_wrapper
	Write-Host "Copying from wrapper to unity.."
	copy_release_to_unity
	Write-Host "Done!"
}
