@echo off
setlocal EnableDelayedExpansion

REM -------------------------
REM Simple BAT -> EXE wrapper using IEXPRESS
REM Usage: bat-to-exe.bat input.bat output.exe
REM -------------------------

if "%~1"=="" (
  echo Usage: %~nx0 ^<input.bat^> ^<output.exe^>
  echo Example: %~nx0 myscript.bat myscript.exe
  goto :eof
)

if "%~2"=="" (
  echo You must provide an output EXE name.
  echo Example: %~nx0 myscript.bat myscript.exe
  goto :eof
)

set "INPUT=%~1"
set "OUTPUT=%~2"

if not exist "%INPUT%" (
  echo [ERROR] Input file not found: %INPUT%
  goto :eof
)

REM Normalize paths
set "INPUT_FULL=%~f1"
set "OUTPUT_FULL=%~dp0%OUTPUT%"

REM Create working temp folder
set "WORKDIR=%TEMP%\bat2exe_%RANDOM%"
mkdir "%WORKDIR%" >nul 2>&1
if errorlevel 1 (
  echo [ERROR] Cannot create temp folder: %WORKDIR%
  goto :cleanup
)

REM Copy the input batch into the temp folder
copy /Y "%INPUT_FULL%" "%WORKDIR%\%~nx1" >nul
if errorlevel 1 (
  echo [ERROR] Failed to copy input file to temp folder.
  goto :cleanup
)

REM Build a small SED file for IEXPRESS
set "SEDFILE=%WORKDIR%\pack.sed"
(
  echo [Version]
  echo Class=IEXPRESS
  echo
  echo [Options]
  echo PackagePurpose=InstallApp
  echo ShowInstallProgramWindow=1
  echo HideExtractAnimation=0
  echo UseLongFileName=1
  echo InsideCompressed=1
  echo CAB_FixedSize=0
  echo RebootMode=NoRestart
  echo InstallPrompt=
  echo DisplayName=BAT to EXE wrapper
  echo FriendlyName=BATtoEXE
  echo TargetName=%OUTPUT_FULL%
  echo SetupLang=EN
  echo
  echo [Strings]
  echo ; Files section follows
  echo
  echo [SourceFiles]
  echo SourceFiles0=%WORKDIR%
  echo
  echo [SourceFiles0]
  echo %~nx1=
  echo
  echo [InstallFiles]
  echo ; Run the batch file after extraction
  echo InstallProgram=%~nx1
) > "%SEDFILE%"

REM Run IEXPRESS to build the EXE
echo Building EXE via IEXPRESS...
where /Q iexpress.exe >nul 2>&1
if errorlevel 1 (
  echo [ERROR] IEXPRESS not found on this system. This tool is normally included on Windows.
  echo You can run IEXPRESS manually (Start -> Run -> iexpress) and point it to the batch file.
  goto :manual
)

REM IEXPRESS command: /N <sedfile> should run the sed non-interactively
iexpress /N "%SEDFILE%"
if errorlevel 1 (
  echo.
  echo [WARN] IEXPRESS build returned an error or failed.
  echo Try running IEXPRESS interactively with the same SED file:
  echo    iexpress "%SEDFILE%"
  goto :manual
)

REM Check result
if exist "%OUTPUT_FULL%" (
  echo [SUCCESS] Created: %OUTPUT_FULL%
  echo Note: This EXE is a self-extractor that will extract the batch to a temp folder and run it.
  goto :cleanup
) else (
  echo [ERROR] Output EXE not found at: %OUTPUT_FULL%
  goto :manual
)

:manual
echo.
echo --- Manual fallback instructions ---
echo 1) Open Start and run "iexpress" (iexpress.exe).
echo 2) Choose "Create new Self Extraction Directive file".
echo 3) Use "Extract files and run an installation command".
echo 4) Add your batch file, set the "Install Program" to the batch's filename.
echo 5) Set the target name to the desired output EXE and finish.
echo Alternatively, use third-party tools like "Bat To Exe Converter" or 7-Zip SFX.
echo ------------------------------------
goto :cleanup

:cleanup
REM remove temp folder (comment this out while testing if you want to inspect)
REM use rd /s /q to delete directory
if exist "%WORKDIR%" rd /s /q "%WORKDIR%" >nul 2>&1

endlocal
