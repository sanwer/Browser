@Echo Off
SET OLDPATH=%PATH%
SET PLATFORM=x86
SET DEVPATH64=C:\Program Files (x86)\Microsoft Visual Studio 10.0\Common7\IDE;
SET DEVPATH86=C:\Program Files\Microsoft Visual Studio 10.0\Common7\IDE;
SET PATH=%PATH%;%DEVPATH86%;%DEVPATH64%

SET Action=Clean
CHOICE /C BCE /N /D B /T 3 /M "Build(B) Clean(C) Continue:"
IF %ERRORLEVEL% LEQ 1 (SET Action=Build && GOTO Start)
IF %ERRORLEVEL% LEQ 2 (SET Action=Clean && GOTO Start)
IF %ERRORLEVEL% LEQ 3 (SET Action=END && GOTO Start)

:Start
Echo %Action%
IF %Action%==Clean GOTO Clean
IF %Action%==END GOTO END

:Prep
Echo Packed Skin.zip
IF EXIST .\Browser\Skin.zip del /F /Q /S .\Browser\Skin.zip
pushd .\Browser\Skin\
..\..\Bin\7z.exe a ..\Skin.zip *
popd popd
Echo.

:Build
:BuildC
IF NOT EXIST "%CD%\Bin\libcef.dll" (
pushd "%CD%\Bin\"
IF NOT EXIST "cef_binary_3.3071.1649.g98725e6_windows32_client.7z" (
wget.exe https://github.com/sanwer/libcef/releases/download/3.3071.1649/cef_binary_3.3071.1649.g98725e6_windows32_client.7z
IF "%errorlevel%"=="1" GOTO error
)
7z.exe x cef_binary_3.3071.1649.g98725e6_windows32_client.7z
popd popd
Echo.
)
IF NOT EXIST .\Bin\libcef.dll GOTO error

Echo Build Release Version with CEF
IF EXIST .\Bin\Release\Browser RD /S /Q .\Bin\Release\Browser
DEVENV Browser.sln /build "Release|Win32"
IF "%ERRORLEVEL%"=="1" GOTO Error
Echo Package
pushd ".\Bin\"
IF EXIST .\Bin\Release\Browser RD /S /Q .\Bin\Release\Browser
IF EXIST Browser_CEF.zip DEL /F /Q /S Browser_CEF.zip
7z.exe a Browser_CEF.zip Browser.exe locales cef.pak cef_100_percent.pak cef_200_percent.pak cef_extensions.pak chrome_elf.dll d3dcompiler_43.dll d3dcompiler_47.dll icudtl.dat libcef.dll libEGL.dll libGLESv2.dll natives_blob.bin snapshot_blob.bin
popd popd
GOTO End


:Clean
Echo Clean Temp File
DEL /F /Q /S /A *.sdf *.user *.ilk *.ipch DuiLib*.lib libcef*.lib DuiLib*.pdb libcef*.pdb
DEL /F /Q /S /A .\Bin\Browser.exe .\Bin\Browser.pdb .\Bin\Browser_d.exe .\Bin\Browser_d.pdb
DEL /F /Q /S /A .\Bin\DuiLib.exe .\Bin\DuiLib.pdb .\Bin\DuiLib_d.dll .\Bin\DuiLib_d.pdb
IF EXIST .\Bin\Debug RD /S /Q .\Bin\Debug
IF EXIST .\Bin\Release RD /S /Q .\Bin\Release
IF EXIST .\Bin\x64 RD /S /Q .\Bin\x64
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
