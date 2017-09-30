@Echo Off
SET OLDPATH=%PATH%
SET PLATFORM=x86
SET DEVPATH64=C:\Program Files (x86)\Microsoft Visual Studio 10.0\Common7\IDE;
SET DEVPATH86=C:\Program Files\Microsoft Visual Studio 10.0\Common7\IDE;
SET PATH=%PATH%;%DEVPATH86%;%DEVPATH64%
SET Action="Prep"
CHOICE /C PBCE /N /D P /T 3 /M "Build(B) Clean(C) Continue:"
IF %ERRORLEVEL% LEQ 1 (SET Action="Prep" && GOTO Start)
IF %ERRORLEVEL% LEQ 2 (SET Action="Build" && GOTO Start)
IF %ERRORLEVEL% LEQ 3 (SET Action="Clean" && GOTO Start)
IF %ERRORLEVEL% LEQ 4 (SET Action="END" && GOTO Start)


:Start
Echo %Action%
IF %Action%=="Clean" GOTO Clean
IF %Action%=="END" GOTO END


:Prep
Echo Packed Skin.zip
IF EXIST "%CD%\Browser\Skin.zip" del /F /Q /S "%CD%\Browser\Skin.zip"
pushd "%CD%\Browser\Skin\"
..\..\Bin\7z.exe a ..\Skin.zip *
popd popd
Echo.

IF NOT EXIST "%CD%\Bin\cef.pak" (
pushd "%CD%\Bin\"
IF NOT EXIST cef_binary_3.2623.1401.gb90a3be.zip (
Rem wget https://github.com/sanwer/libcef/releases/download/latest/cef_binary_3.2623.1401.gb90a3be.zip
wget http://files.git.oschina.net/group1/M00/01/E7/PaAvDFm7e8CAYvlMBd6DJ5NhPGc663.zip?token=206a44f4482dcf8e9a288468c79a9107&ts=1506732571&attname=cef_binary_3.2623.1401.gb90a3be.zip
IF "%errorlevel%"=="1" GOTO error
)
7z.exe x cef_binary_3.2623.1401.gb90a3be.zip
popd popd
Echo.
)
IF NOT EXIST "%CD%\Bin\cef.pak" GOTO error


IF %Action%=="Prep" GOTO END
IF %Action%=="Build" GOTO Build


:Build
Echo Build Release Version
DEVENV Browser.sln /build "Release|Win32"
IF "%ERRORLEVEL%"=="1" GOTO Error
Echo Build Release Version End
GOTO Package


:Package
Echo Package
pushd "%CD%\Bin\"
IF EXIST Browser.zip DEL /F /Q /S Browser.zip
7z.exe a Browser.zip Browser.exe locales PepperFlash cef.pak cef_100_percent.pak cef_extensions.pak d3dcompiler_43.dll d3dcompiler_47.dll icudtl.dat libcef.dll libEGL.dll libGLESv2.dll natives_blob.bin DuiLib.dll
popd popd
GOTO End


:Clean
Echo Clean Temp File
DEL /F /Q /S /A *.sdf *.user *.ilk *.ipch
DEL /F /Q /S /A .\Bin\Browser.exe .\Bin\Browser.pdb .\Bin\Browser_d.exe .\Bin\Browser_d.pdb .\Bin\Browser.zip
DEL /F /Q /S /A .\Bin\DuiLib.exe .\Bin\DuiLib.pdb .\Bin\DuiLib_d.dll .\Bin\DuiLib_d.pdb
IF EXIST .\Bin\Debug RD /S /Q .\Bin\Debug
IF EXIST .\Bin\Release RD /S /Q .\Bin\Release
IF EXIST .\Bin\x64 RD /S /Q .\Bin\x64
IF EXIST .\Lib RD /S /Q .\Lib
IF EXIST .\ipch RD /S /Q .\ipch
IF EXIST .vs RD /S /Q .vs
IF EXIST .\Browser\Skin.zip DEL /F /Q /S .\Browser\Skin.zip
SET PATH=%OLDPATH%
exit /B 0

:Error
SET PATH=%OLDPATH%
Echo [ERROR]: There was an error building the component
PAUSE
EXIT /B 1

:End
SET PATH=%OLDPATH%
