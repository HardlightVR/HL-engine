Param(
	[switch]$debug,
	[switch]$release
)

 $projects = @{
 	"plugin" = "C:\Users\NullSpace Team\Documents\Visual Studio 2015\Projects\NSLoader\build\bin";
 	"wrapper" = "C:\Users\NullSpace Team\Documents\Visual Studio 2015\Projects\NSLoaderWrapper\build\bin";
 	"unity" = "C:\Users\NullSpace Team\Documents\NullSpace SDK 0.1.1\Assets\NullSpace SDK\Plugins";
 }
$project_dll_names = @{
	"plugin" = "NSLoader.dll";
	"wrapper" = "NSLoaderWrapper.dll", "NSLoader.dll"
	"unity" = "NSLoader.dll", "NSLoaderWrapper.dll"
}


 $projects_build_dirs = @{
 	"plugin" = @{
 		"debug" = @{
 			"win32" = "Debug\Win32";
 			"win64" = "Debug\Win64";
 		};
 		"release" = @{
 			"win32" = "Release\Win32";
 			"win64" = "Release\Win64";
 		};
 	};
 	"wrapper" = @{
 		"debug" = @{
 			"win32" = "Debug\Win32";
 			"win64" = "Debug\Win64";
 		};
 		"release" = @{
 			"win32" = "Release\Win32";
 			"win64" = "Release\Win64";
 		};
 	};
 	"unity" = @{
 		"debug" = @{
 			"win32" = "x86";
 			"win64" = "x86_64";
 		};
 		"release" = @{
 			"win32" = "x86";
 			"win64" = "x86_64";
 		};
 	};


 }


function copy_dlls_from_to($projectA, $projectB, $configuration) {


	$B_root_path = $projects[$projectB];
	$A_root_path = $projects[$projectA];



	$A_win32_dir = Join-Path $A_root_path $projects_build_dirs[$projectA][$configuration]["win32"];
	$A_win64_dir = Join-Path $A_root_path $projects_build_dirs[$projectA][$configuration]["win64"];
	
	$B_win32_dir = Join-Path $B_root_path $projects_build_dirs[$projectB][$configuration]["win32"];
	$B_win64_dir = Join-Path $B_root_path $projects_build_dirs[$projectB][$configuration]["win64"];

	#Write-Host "$($projectA) root path: $($A_root_path)";
	#Write-Host "$($projectA) win32: $($A_win32_dir)";
	#Write-Host "$($projectA) win64: $($A_win64_dir)";

	
#	Write-Host "$($projectB) root path: $($B_root_path)";
#	Write-Host "$($projectB) win32: $($B_win32_dir)";
#	Write-Host "$($projectB) win64: $($B_win64_dir)";

	Foreach ($dll in $project_dll_names[$projectA]) {

		$a_win32_dll = Join-Path $A_win32_dir $dll;
		$b_win32_dll = Join-Path $B_win32_dir $dll;

		$a_win64_dll = Join-Path $A_win64_dir $dll;
		$b_win64_dll = Join-Path $B_win64_dir $dll;

		Write-Host "Copying $($a_win32_dll) to $($b_win32_dll)";

		Write-Host "Copying $($a_win64_dll) to $($b_win64_dll)";
		Copy-Item -Path $a_win32_dll -Destination $b_win32_dll
		Copy-Item -Path $a_win64_dll -Destination $b_win64_dll

	}



}

if ($debug) {

	copy_dlls_from_to "plugin" "wrapper" "debug"
	copy_dlls_from_to "wrapper" "unity" "debug"
	#Write-Host "Copying from loader to wrapper.."
	#copy_debug_to_wrapper
	#Write-Host "Copying from wrapper to unity.."
	#copy_debug_to_unity
	#Write-Host "Done!"
}

if ($release) {

	copy_dlls_from_to "plugin" "wrapper" "release"
	copy_dlls_from_to "wrapper" "unity" "release"
	#Write-Host "Copying from loader to wrapper.."
	#copy_release_to_wrapper
	#Write-Host "Copying from wrapper to unity.."
	#copy_release_to_unity
	#Write-Host "Done!"
}
