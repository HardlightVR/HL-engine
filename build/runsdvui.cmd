cd /d "C:\Users\NullSpace Team\Documents\NS_Unreal_SDK\build" &msbuild "HardlightMkIII.vcxproj" /t:sdvViewer /p:configuration="Debug UnitTestWin32" /p:platform=Win32
exit %errorlevel% 