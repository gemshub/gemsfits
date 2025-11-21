rem  Change the path to the actual location of gem-fits-shell executable and Resources
cd ./GemFits-app/bin

rem 1. First launch with default location of modeling projects (usually done by the installer)
rem gem-fits-shell.exe -d > gemsfits.log

rem 2. Normal runs in default locations (retains all settings from previous session)
gem-fits-shell.exe > gemsfits.log
rem or 
rem gem-fits-shell.exe -s . -u C:\Users\<USER>\Library\GEMSFTIS > gems3.log

rem 3. New file configuration if project subfolder(s) were added/removed to/from
rem   /projects or if /projects are not in the default location
rem gem-fits-shell.exe -c -s <Path_to_Resources> -u G:\My_gemfits_Projects_Location\GEMSFITS > gemfits.log

rem 4. New \data\*.ini files - remake DOD and module dialog configurators
rem Otherwise, does the same as  gems3 -c
rem gem-fits-shell.exe -d -s . -u C:\Users\<USER>\Library\GEMSFITS > gemfits.log

rem 5. Create on desktop a shortcut

@echo off
setlocal
set "scriptPath=%~dp0"
set "targetPath=%scriptPath%GemFits-app\bin\gem-fits-shell.exe"
set "iconPath=%scriptPath%GemFits-app\Resources\gemsfits.ico"
set "startMenuShortcut=%APPDATA%\Microsoft\Windows\Start Menu\Programs\gems-fits-shell.lnk"
set "desktopShortcut=%USERPROFILE%\Desktop\gems-fits-shell.lnk"

REM Check if required files exist
IF NOT EXIST "%targetPath%" (
    echo ERROR: Executable not found: "%targetPath%"
    exit /b
)

IF NOT EXIST "%iconPath%" (
    echo ERROR: Icon file not found: "%iconPath%"
    exit /b
)

REM Create Start Menu shortcut
echo Creating Start Menu shortcut...
echo Set oWS = CreateObject("WScript.Shell") > CreateShortcut.vbs
echo Set oLink = oWS.CreateShortcut("%startMenuShortcut%") >> CreateShortcut.vbs
echo oLink.TargetPath = "%targetPath%" >> CreateShortcut.vbs
echo oLink.IconLocation = "%iconPath%" >> CreateShortcut.vbs
echo oLink.Save >> CreateShortcut.vbs
cscript //nologo CreateShortcut.vbs
del CreateShortcut.vbs
echo Shortcut created successfully in Start Menu.

REM Create Desktop shortcut
echo Creating Desktop shortcut...
echo Set oWS = CreateObject("WScript.Shell") > CreateShortcut.vbs
echo strDesktop = oWS.SpecialFolders("Desktop") >> CreateShortcut.vbs
echo Set oLink = oWS.CreateShortcut(strDesktop ^& "\gems-fits-shell.lnk") >> CreateShortcut.vbs
echo oLink.TargetPath = "%targetPath%" >> CreateShortcut.vbs
echo oLink.IconLocation = "%iconPath%" >> CreateShortcut.vbs
echo oLink.Save >> CreateShortcut.vbs
cscript //nologo CreateShortcut.vbs
del CreateShortcut.vbs
echo Shortcut created successfully on Desktop.

endlocal

