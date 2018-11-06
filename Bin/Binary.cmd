@Echo Off
SET Version=%1
if "%Version%" =="" (SET Version=3.2623.1401.gb90a3be)
for /f "tokens=1-4 delims=." %%a in ("%Version%") do ((set V1=%%a)&(set V2=%%b)&(set V3=%%c)&(set V4=%%d))
IF EXIST .\Bin ( PUSHD .\Bin ) else ( PUSHD %CD% )
IF NOT EXIST libcef.dll (
  Echo Download %V1%.%V2%.%V3%.%V4% binary files
  IF NOT EXIST "cef_binary_%Version%_windows32.7z" (
    wget.exe https://github.com/sanwer/libcef/releases/download/%V1%.%V2%.%V3%/cef_binary_%Version%_windows32.7z
    IF %ERRORLEVEL%==1 GOTO ErrorBinary
  )
  7z.exe x cef_binary_%Version%_windows32.7z
  IF %ERRORLEVEL%==1 GOTO ErrorBinary
  IF NOT EXIST libcef.dll GOTO ErrorBinary
)
GOTO EndBinary

:ErrorBinary
Echo [ERROR]: There was an error in downloading binary files.
PAUSE

:EndBinary
POPD
