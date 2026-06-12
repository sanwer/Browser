@Echo Off
SETLOCAL ENABLEDELAYEDEXPANSION
SET BIN_DIR=%~dp0
SET CEF_URL=https://gitee.com/sanwer/libcef/releases/download/145.0.28/cef_binary_145.0.28_g51162e8_windows64.7z
SET CEF_BIN=cef_binary_145.0.28_g51162e8_windows64.7z
SET CEF_DLL="%BIN_DIR%x64\libcef.dll"

PUSHD "%BIN_DIR%"
IF NOT EXIST %CEF_DLL% (
    Echo download %BaseVersion% binary files
    IF NOT EXIST %CEF_BIN% (
        wget.exe -q --show-progress -P %BIN_DIR% %CEF_URL%
        IF %ERRORLEVEL%==1 GOTO ErrorBinary
    )
    7z.exe x -bsp1 -bso0 %CEF_BIN% -o"%BIN_DIR%x64\"
    IF %ERRORLEVEL%==1 GOTO ErrorBinary
    IF NOT EXIST %CEF_DLL% GOTO ErrorBinary
)
GOTO EndBinary

:ErrorBinary
Echo [ERROR]: There was an error in downloading binary files.
PAUSE
ENDLOCAL
EXIT /B 1

:EndBinary
POPD
ENDLOCAL