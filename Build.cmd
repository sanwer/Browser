@Echo Off
SET OLDPATH=%PATH%
SET DEVPATH64=C:\Program Files (x86)\Microsoft Visual Studio 10.0\Common7\IDE;
SET DEVPATH86=C:\Program Files\Microsoft Visual Studio 10.0\Common7\IDE;
SET PATH=%PATH%;%DEVPATH86%;%DEVPATH64%

SET Action=Clean
CHOICE /C BCE /N /D B /T 3 /M "Build(B) Clean(C) Continue:"
IF %ERRORLEVEL% LEQ 1 (SET Action=Build && GOTO Start)
IF %ERRORLEVEL% LEQ 2 (SET Action=Clean && GOTO Start)
IF %ERRORLEVEL% LEQ 3 (SET Action=END && GOTO Start)

:Start
IF %Action%==Clean GOTO Clean
IF %Action%==END GOTO END

:Prep
IF NOT EXIST .\Browser\Skin.zip (
Echo Packed Skin.zip
PUSHD .\Browser\Skin\
..\..\Bin\7z.exe a ..\Skin.zip *
POPD
)
IF "%ERRORLEVEL%"=="1" GOTO Error

FOR /f "tokens=1,2,3" %%i IN ('findstr /L "CEF_VERSION " %CD%\..\libcef\include\cef_version.h') DO (
IF "%%i"=="#define" (IF "%%j"=="CEF_VERSION" (set Version=%%k))
)
IF "%Version%" =="" (SET Version=3.3440.1805) ELSE ( set "Version=%Version:"=%" )

:Build
Echo Build Browser with libcef %Version%
DEVENV Browser.sln /build "Release|Win32"
IF "%ERRORLEVEL%"=="1" GOTO Error

CALL .\Bin\Binary.cmd %Version%
CALL .\Bin\Package.cmd
GOTO End

:Clean
Echo Clean Temp File
DEL /F /Q /S /A *.sdf *.user *.ilk *.ipch *.suo *.opensdf
DEL /F /Q /S /A DuiLib*.lib libcef*.lib DuiLib*.pdb libcef*.pdb Browser*.exe Browser*.pdb
IF EXIST .vs RD /S /Q .vs
IF EXIST .\ipch RD /S /Q .\ipch
IF EXIST .\Bin\Debug RD /S /Q .\Bin\Debug
IF EXIST .\Bin\Release RD /S /Q .\Bin\Release
IF EXIST .\Bin\x64\Debug RD /S /Q .\Bin\x64\Debug
IF EXIST .\Bin\x64\Release RD /S /Q .\Bin\x64\Release
IF EXIST .\Bin\Browser.zip DEL /F /Q /S .\Bin\Browser.zip
IF EXIST .\Browser\Skin.zip DEL /F /Q /S .\Browser\Skin.zip
SET PATH=%OLDPATH%
exit /B 0

:Error
SET PATH=%OLDPATH%
Echo [ERROR]: There was an error in the processing.
PAUSE
EXIT /B 1

:End
SET PATH=%OLDPATH%
