@Echo Off
SET OLDPATH=%PATH%
SET PLATFORM=x86
SET DEVPATH64=C:\Program Files (x86)\Microsoft Visual Studio 10.0\Common7\IDE;
SET DEVPATH86=C:\Program Files\Microsoft Visual Studio 10.0\Common7\IDE;
SET PATH=%PATH%;%DEVPATH86%;%DEVPATH64%

:Build
DEVENV Browser.sln /build "Release|Win32"
IF "%errorlevel%"=="1" GOTO error
Echo.

Echo Package
pushd "%CD%\Bin\"
IF EXIST Browser.zip DEL /F /Q /S Browser.zip
7z.exe a Browser.zip Browser.exe locales cef.pak cef_100_percent.pak cef_extensions.pak d3dcompiler_43.dll d3dcompiler_47.dll icudtl.dat libcef.dll libEGL.dll libGLESv2.dll natives_blob.bin pepflashplayer.dll
popd popd

:Clean
del /F /Q /S "%CD%\*.pdb"
SET PATH=%OLDPATH%
exit /B 0

:error
SET PATH=%OLDPATH%
Echo [ERROR]: There was an error building the component
PAUSE
EXIT /B 1
