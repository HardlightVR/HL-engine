-- We are using PCH, so you must disable PCH on the protobuf files, and enable generation on stdafx.cpp


workspace "Driver"
	configurations {"Debug", "Release"}
	platforms {"Win32", "Win64", "Win32Exe"}
	language "C++"
	
	


project "Driver" 



	
	targetdir "bin/%{cfg.buildcfg}/%{cfg.platform}"
	targetname "Driver"
	
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
		"../src/Driver/*.cpp",
		"../src/Driver/*.h",
		"../src/Driver/*.hpp",
		"../src/Consumers/*.cpp",
		"../src/Consumers/*.h",
		"../src/Driver/",
		"../src/Driver/Json/*.cpp",
		"../src/Driver/Json/*.h",
		"../src/Driver/events_impl/**.cpp",
		"../src/Driver/events_impl/**.h",

		"../src/Driver/include/**.h",
		
		-- protobuffs. Could just do something like **.pb.cc
		path.join(protobuf_def_incl_dir, "DriverCommand.pb.cc"),
		path.join(protobuf_def_incl_dir, "EffectCommand.pb.cc"),
		-- end protobufs
		path.join(shared_comms_incl_dir, "ScheduledEvent.cpp")

	}

	

	postbuildcommands {
		"{COPY} ../src/Driver/*.json %{cfg.targetdir}"
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
	filter {"platforms:Win32Exe"}
		kind "ConsoleApp"
	-- input: libprotobuf
	filter {"platforms:Win32*", "configurations:Debug"}
		libdirs {
			path.join(protobuf_win32_dir, "Debug")
		}
	filter {"platforms:Win32*", "configurations:Release"}
		libdirs {
			path.join(protobuf_win32_dir, "Release")
		}
	filter {"platforms:Win64*", "configurations:Debug"}
		libdirs {
			path.join(protobuf_win64_dir, "Debug")
		}
	filter {"platforms:Win64*", "configurations:Release"}
		libdirs {
			path.join(protobuf_win64_dir, "Release")
		}


	filter "platforms:Win32*" 
		system "Windows"
		architecture "x86"
		libdirs {
			boost_win32_dir
		}
		defines {"WIN32"}
		
	filter "platforms:Win64*"
		system "Windows"
		architecture "x86_64"
		libdirs {
			boost_win64_dir
		}
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
		"../src/Driver/include/**.h"
	}

	



	boost_win32_dir = "D:/Libraries/boost/boost_1_61_0/stage/win32/lib"
	boost_win64_dir = "D:/Libraries/boost/boost_1_61_0/stage/x64/lib"


	

	pchheader "stdafx.h"
	pchsource "../src/plugins/hardlight/stdafx.cpp"

	nsvr_core_win32_dir_debug = "C:/Users/NullSpace Team/Documents/NS_Unreal_SDK/build/bin/Debug/Win32"
	nsvr_core_win32_dir_release = "C:/Users/NullSpace Team/Documents/NS_Unreal_SDK/build/bin/Release/Win32"

	filter {"files:**.pb.cc"}
		flags {'NoPCH'}
	


	filter {"configurations:Debug or configurations:Release"}
		kind "SharedLib"
	filter {"configurations:UnitTest"}
		kind "ConsoleApp"

	
	

	filter "platforms:Win32" 
		system "Windows"
		architecture "x86"

		defines {"WIN32"}
		postbuildcommands {
			"{COPY} %{cfg.targetdir}/HardlightPlugin.dll bin/Debug/Win32Exe"		

		}
	filter "platforms:Win64"
		system "Windows"
		architecture "x86_64"
		libdirs {
			boost_win64_dir
		}
	filter "configurations:Debug or configurations:UnitTest"
		defines {"DEBUG", "_DEBUG"}
		symbols "On"
		optimize "Off"
		links {"Driver"}
		libdirs {
			boost_win32_dir,
			nsvr_core_win32_dir_debug
		}

	filter "configurations:Release"
		defines {"NDEBUG"}
		optimize "On" 
		links { "Driver"}
		libdirs {
			boost_win32_dir,
			nsvr_core_win32_dir_release
		}

	filter {"system:Windows"}
		defines {"_WINDOWS", "_USRDLL"}

	filter {"system:Windows", "configurations:Debug or configurations:UnitTest"}
		buildoptions {"-D_SCL_SECURE_NO_WARNINGS"}