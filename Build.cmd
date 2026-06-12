@Echo Off
chcp 65001 >nul
SETLOCAL ENABLEDELAYEDEXPANSION

SET PLATFORM=x64
SET CONFIG=Release
SET CEF_VERSION=145.0.28+g51162e8+chromium-145.0.7632.160

for /f "tokens=2 delims==" %%a in ('wmic OS Get localdatetime /value') do set "dt=%%a"
set "YYYY=%dt:~0,4%"
set "MM=%dt:~4,2%"
set "DD=%dt:~6,2%"
set "HH=%dt:~8,2%"
set "MI=%dt:~10,2%"
set "SS=%dt:~12,2%"

set DATE_STR=%YYYY%-%MM%-%DD%
set TIME_STR=%HH%:%MI%:%SS%
SET OUT_FILE="%~dp0Browser_%YYYY%%MM%%DD%%HH%%MI%.zip"

Echo ================================================
Echo Build.cmd - Browser Project Build Script
Echo ================================================
Echo Build Date: %DATE_STR% %TIME%
Echo CEF Version: %CEF_VERSION%
Echo Platform: %PLATFORM%
Echo Configuration: %CONFIG%
Echo ================================================
Echo.

Echo [Step 1/4] Parsing user input...
SET Action=Build
CHOICE /C BCE /N /D B /T 3 /M "Build(B) Clean(C) Continue(E):"
IF %ERRORLEVEL% EQU 1 (SET Action=Build && Echo Selected action: Build)
IF %ERRORLEVEL% EQU 2 (SET Action=Clean && Echo Selected action: Clean)
IF %ERRORLEVEL% EQU 3 (SET Action=END && Echo Selected action: Skip build)

:Start
IF %Action%==Clean GOTO Clean
IF %Action%==END GOTO End

:Prep
Echo.
Echo [Step 2/4] Preparing resources...
IF NOT EXIST .\Browser\Skin.zip (
    Echo Creating Skin.zip...
    PUSHD .\Browser\Skin\
    ..\..\Bin\7z.exe a ..\Skin.zip *
    POPD
    IF ERRORLEVEL 1 (
        Echo [ERROR] Failed to create Skin.zip
        GOTO Error
    )
    Echo Skin.zip created successfully
) ELSE (
    Echo Skin.zip already exists, skipping
)

:Build
Echo.
Echo [Step 3/4] Building solution...
Echo MSBuild path detection...
SET MSBUILD=C:\Program Files\Microsoft Visual Studio\2022\Community\MSBuild\Current\Bin\MSBuild.exe

IF EXIST "%MSBUILD%" (
    Echo Found MSBuild: %MSBUILD%
    Echo Starting build...
    "%MSBUILD%" Browser.sln /t:Build /p:Configuration=%CONFIG%;Platform=%PLATFORM% /m /v:minimal
    IF ERRORLEVEL 1 (
        Echo [ERROR] MSBuild failed with error code %ERRORLEVEL%
        GOTO Error
    )
    Echo Build completed successfully
) ELSE (
    Echo [ERROR] MSBuild not found at: %MSBUILD%
    Echo Please install Visual Studio 2022 with MSBuild
    GOTO Error
)

Echo.
Echo [Step 4/4] Post-build processing...

IF EXIST .\Bin\Binary.cmd (
    CALL .\Bin\Binary.cmd
)

IF EXIST .\Bin\Package.cmd (
    CALL .\Bin\Package.cmd %OUT_FILE%
)

Echo.
Echo ================================================
Echo Build completed successfully!
Echo Output: %OUT_FILE%
Echo ================================================
GOTO End

:Clean
Echo.
Echo [Clean] Cleaning temporary files...
DEL /F /Q /S /A *.sdf *.user *.ilk *.ipch *.suo *.opensdf 2>NUL
DEL /F /Q /S /A DuiLib*.lib libcef*.lib DuiLib*.pdb libcef*.pdb Browser*.exe Browser*.pdb 2>NUL
IF EXIST .vs RD /S /Q .vs
IF EXIST .\ipch RD /S /Q .\ipch
IF EXIST .\Bin\Debug RD /S /Q .\Bin\Debug
IF EXIST .\Bin\Release RD /S /Q .\Bin\Release
IF EXIST .\Bin\x64\Debug RD /S /Q .\Bin\x64\Debug
IF EXIST .\Bin\x64\Release RD /S /Q .\Bin\x64\Release
IF EXIST .\Bin\Browser.zip DEL /F /Q /S .\Bin\Browser.zip 2>NUL
IF EXIST .\Browser\Skin.zip DEL /F /Q /S .\Browser\Skin.zip 2>NUL
Echo Clean completed
ENDLOCAL
exit /B 0

:ErrorBinary
Echo [ERROR]: There was an error in downloading binary files.

:Error
Echo.
Echo ================================================
Echo [ERROR] Build failed at %DATE_STR% %TIME%
Echo CEF Version: %CEF_VERSION%
Echo Platform: %PLATFORM%
Echo Configuration: %CONFIG%
Echo Action: %Action%
Echo ================================================
PAUSE
ENDLOCAL
EXIT /B 1

:End
ENDLOCAL