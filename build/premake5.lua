-- We are using PCH, so you must disable PCH on the protobuf files, and enable generation on stdafx.cpp


-- note[0] 
-- It appears that by messing with the output directories, VS doesn't start the binary in the right place
-- when debugging. If you simply double click the Driver.exe, for example, it will find the plugins and JSON
-- configs just fine. But if you do it from the IDE, it won't. I copy the required files into ../build to solve this.
-- Better solution: obey, or figure out, the necessary directory structure to make VS happy.

workspace "Driver"
	configurations {"Debug", "Release"}
	platforms {"Win32", "Win64", "UnitTestWin32"}
	language "C++"
	
	


project "Driver" 
	
	targetdir "bin/%{cfg.buildcfg}/%{cfg.platform}"
	targetname "NSVREngine"
	
	-- dependencies
	protobuf_incl_dir = "D:/protobuf-3.0.0/cmake/build/solution/include"
	shared_comms_incl_dir = "C:/Users/NullSpace Team/Documents/NS_Unreal_SDK/src/Driver/SharedCommunication"
	boost_incl_dir = "D:/Libraries/boost/boost_1_61_0"
	protobuf_def_incl_dir = "C:/Users/NullSpace Team/Documents/NS_Unreal_SDK/src/Driver/protobuff_defs"

	disablewarnings {"4800"}


	includedirs {
		protobuf_incl_dir,
		shared_comms_incl_dir,
		boost_incl_dir,
		"../src/Driver/include",
		"../src/Driver/events_impl"
	}

	flags {
		"MultiProcessorCompile",
		"C++11"

	}
	--links {"System", "UnityEditor", "UnityEngine", "System.ServiceProcess"}

	files {
		"../src/Driver/**.cpp",
		"../src/Driver/**.h",
		"../src/Driver/**.hpp",
		"../src/Driver/protobuff_defs/**.pb.cc",
		
		path.join(shared_comms_incl_dir, "ScheduledEvent.cpp")

	}

	


	boost_win32_dir = "D:/Libraries/boost/boost_1_61_0/stage/win32/lib"
	boost_win64_dir = "D:/Libraries/boost/boost_1_61_0/stage/x64/lib"

	protobuf_win32_dir = "D:/protobuf-3.0.0/cmake/build/solution"
	protobuf_win64_dir = "D:/protobuf-3.0.0/cmake/build/solution64"
	

	pchheader "stdafx.h"
	pchsource "../src/Driver/stdafx.cpp"


	defines {"NS_DRIVER_EXPORTS", "NSVR_BUILDING_CORE", "BOOST_THREAD_USE_LIB"}

	filter {"files:**.pb.cc or files:**ScheduledEvent.cpp or files:**jsoncpp.cpp"}
		flags {'NoPCH'}
	
	filter {"platforms:Win32 or platforms:Win64"}
		kind "SharedLib"
		postbuildcommands {
			"{COPY} %{cfg.targetdir}/%{cfg.targetname}%{cfg.targetextension} bin/%{cfg.buildcfg}/UnitTestWin32",
			-- see note at top [0]
			"{COPY} %{cfg.targetdir}/%{cfg.targetname}%{cfg.targetextension} ../build"		

		}


	filter {"platforms:UnitTestWin32"}
		kind "ConsoleApp"
		postbuildcommands {
			"{COPY} ../src/Driver/*.json %{cfg.targetdir}",
			-- see note at top [0]
			"{COPY} ../src/Driver/*.json ../build/"

		}



 	
	filter {"platforms:*Win32*", "configurations:Debug"}
		libdirs {
			path.join(protobuf_win32_dir, "Debug")
		}
	filter {"platforms:*Win32*", "configurations:Release"}
		libdirs {
			path.join(protobuf_win32_dir, "Release")
		}
	


	filter "platforms:*Win32*" 
		system "Windows"
		architecture "x86"
		libdirs {
			boost_win32_dir
		}
		defines {"WIN32", "_WIN32_WINNT=0x0A00"}
		
	filter "configurations:Debug"
		defines {"DEBUG", "_DEBUG"}
		symbols "On"
		optimize "Off"
		links {"libprotobufd"}
		

	filter "configurations:Release"
		defines {"NDEBUG"}
		optimize "On" 
		links {"libprotobuf"}
		

	filter {"system:Windows"}
		defines {"_WINDOWS", "_USRDLL"}
		-- not sure if this is actually setting sdl checks. We should be sure to work without them
		buildoptions {"-sdl"}

	filter {"system:Windows", "configurations:Debug"}
		buildoptions {"-D_SCL_SECURE_NO_WARNINGS"}


project "HardlightMkIII"
	targetdir "bin/Plugins/%{cfg.buildcfg}/%{cfg.platform}"
	targetname "HardlightPlugin"
	

	--shared_comms_incl_dir = "C:/Users/NullSpace Team/Documents/NS_Unreal_SDK/src/Driver/SharedCommunication"
	boost_incl_dir = "D:/Libraries/boost/boost_1_61_0"
	--protobuf_def_incl_dir = "C:/Users/NullSpace Team/Documents/NS_Unreal_SDK/src/Driver/protobuff_defs"
	disablewarnings {"4800"}


	includedirs {
		boost_incl_dir,
		"../src/plugins/hardlight",
		"../src/Driver/include"
			}

	
	flags {
		"MultiProcessorCompile",
		"C++11"

	}

	files {
		"../src/plugins/hardlight/**.cpp",
		"../src/plugins/hardlight/**.h",
		"../src/Driver/include/**.h",
		"../src/plugins/hardlight/zone_log/**.cpp",
		"../src/plugins/hardlight/zone_log/**.h"


	}

	




	boost_win32_dir = "D:/Libraries/boost/boost_1_61_0/stage/win32/lib"
	boost_win64_dir = "D:/Libraries/boost/boost_1_61_0/stage/x64/lib"


	

	pchheader "stdafx.h"
	pchsource "../src/plugins/hardlight/stdafx.cpp"

	--nsvr_core_win32_dir_debug = "C:/Users/NullSpace Team/Documents/NS_Unreal_SDK/build/bin/Debug/Win32"
	--nsvr_core_win32_dir_release = "C:/Users/NullSpace Team/Documents/NS_Unreal_SDK/build/bin/Release/Win32"

	nsvr_core_win32_dir_debug = "../build/bin/Debug/Win32"
	nsvr_core_win32_dir_release = "../build/bin/Release/Win32"

	defines {"BOOST_THREAD_USE_LIB"}

	filter {"files:**.pb.cc"}
		flags {'NoPCH'}
	


	filter {"platforms:Win32 or platforms:Win64"}
		kind "SharedLib"

	filter {"platforms:UnitTestWin32"}
		kind "ConsoleApp"



	filter "platforms:*Win32*" 
		system "Windows"
		architecture "x86"

		defines {"WIN32", "_WIN32_WINNT=0x0A00"}
		postbuildcommands {
			"{COPY} %{cfg.targetdir}/HardlightPlugin.dll bin/Debug/UnitTestWin32",		
			-- see note at top [0]
			"{COPY} %{cfg.targetdir}/HardlightPlugin.dll ../build/"		


		}
		libdirs {
			boost_win32_dir
		}
	filter "configurations:Debug"
		defines {"DEBUG", "_DEBUG"}
		symbols "On"
		optimize "Off"
		links {"NSVREngine"}
		libdirs {
			nsvr_core_win32_dir_debug

		}

	filter "configurations:Release"
		defines {"NDEBUG"}
		optimize "On" 
		links { "NSVREngine"}
		libdirs {
			nsvr_core_win32_dir_release
		}

	filter {"system:Windows"}
		defines {"_WINDOWS", "_USRDLL"}

	filter {"system:Windows", "configurations:Debug"}
		buildoptions {"-D_SCL_SECURE_NO_WARNINGS"}







project "OpenVR"
	targetdir "bin/Plugins/%{cfg.buildcfg}/%{cfg.platform}"
	targetname "OpenVRPlugin"
	

	
	disablewarnings {"4800"}


	includedirs {
		"../src/plugins/openvr",
		"C:/Users/NullSpace Team/Documents/openvr/headers",
		"../src/Driver/include"
			}

	
	flags {
		"MultiProcessorCompile",
		"C++11"

	}

	files {
		"../src/plugins/openvr/**.cpp",
		"../src/plugins/openvr/**.h",
		"../src/Driver/include/**.h"
	}

	

	pchheader "stdafx.h"
	pchsource "../src/plugins/openvr/stdafx.cpp"

	--nsvr_core_win32_dir_debug = "C:/Users/NullSpace Team/Documents/NS_Unreal_SDK/build/bin/Debug/Win32"
	--nsvr_core_win32_dir_release = "C:/Users/NullSpace Team/Documents/NS_Unreal_SDK/build/bin/Release/Win32"

	nsvr_core_win32_dir_debug = "../build/bin/Debug/Win32"
	nsvr_core_win32_dir_release = "../build/bin/Release/Win32"

	openvr_win32_dir = "C:/Users/NullSpace Team/Documents/openvr/lib/win32"

	filter {"files:**.pb.cc"}
		flags {'NoPCH'}
	


	filter {"platforms:Win32 or platforms:Win64"}
		kind "SharedLib"

	filter {"platforms:UnitTestWin32"}
		kind "ConsoleApp"



	filter "platforms:*Win32*" 
		system "Windows"
		architecture "x86"

		defines {"WIN32", "_WIN32_WINNT=0x0A00"}
		
	filter "platforms:Win32" 
	postbuildcommands {
			"{COPY} %{cfg.targetdir}/OpenVRPlugin.dll bin/Debug/UnitTestWin32",		
			-- see note at top [0]
			"{COPY} %{cfg.targetdir}/OpenVRPlugin.dll ../build/"		


		}
	filter "configurations:Debug"
		defines {"DEBUG", "_DEBUG"}
		symbols "On"
		optimize "Off"
		links {"NSVREngine", "openvr_api"}
		libdirs {
			nsvr_core_win32_dir_debug,
			openvr_win32_dir
		}

	filter "configurations:Release"
		defines {"NDEBUG"}
		optimize "On" 
		links { "NSVREngine", "openvr_api"}
		libdirs {
			nsvr_core_win32_dir_release,
			openvr_win32_dir
		}

	filter {"system:Windows"}
		defines {"_WINDOWS", "_USRDLL"}

	filter {"system:Windows", "configurations:Debug"}
		buildoptions {"-D_SCL_SECURE_NO_WARNINGS"}