rem  Change the path to the actual location of gem-fits-shell executable and Resources
cd ./GemFits-app/bin

rem Force Qt to use the platform plugin bundled with this app (GemFits-app\plugins\platforms).
rem Without this, a QT_QPA_PLATFORM_PLUGIN_PATH or QT_PLUGIN_PATH already set on this machine
rem (e.g. left over from an unrelated Anaconda/PyQt/Qt Creator install) silently overrides Qt's
rem normal bundled-app lookup, and gem-fits-shell.exe aborts immediately with:
rem   qt.qpa.plugin: Could not find the Qt platform plugin "windows" in ""
set "QT_QPA_PLATFORM_PLUGIN_PATH=%CD%\..\plugins\platforms"
set "QT_PLUGIN_PATH=%CD%\..\plugins"

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
rem Shortcuts target runshell.bat itself (not gem-fits-shell.exe directly) so that every
rem launch - not just the very first one - goes through the QT_QPA_PLATFORM_PLUGIN_PATH fix
rem above. Pointing a shortcut straight at the exe would skip that fix and could reproduce
rem the "Could not find the Qt platform plugin" crash on machines with a conflicting Qt env var.
set "targetPath=%scriptPath%runshell.bat"
set "workingDir=%scriptPath%"
set "exePath=%scriptPath%GemFits-app\bin\gem-fits-shell.exe"
set "iconPath=%scriptPath%GemFits-app\Resources\gemsfits.ico"
set "startMenuShortcut=%APPDATA%\Microsoft\Windows\Start Menu\Programs\gems-fits-shell.lnk"
set "desktopShortcut=%USERPROFILE%\Desktop\gems-fits-shell.lnk"

REM Check if required files exist
IF NOT EXIST "%exePath%" (
    echo ERROR: Executable not found: "%exePath%"
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
echo oLink.WorkingDirectory = "%workingDir%" >> CreateShortcut.vbs
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
echo oLink.WorkingDirectory = "%workingDir%" >> CreateShortcut.vbs
echo oLink.IconLocation = "%iconPath%" >> CreateShortcut.vbs
echo oLink.Save >> CreateShortcut.vbs
cscript //nologo CreateShortcut.vbs
del CreateShortcut.vbs
echo Shortcut created successfully on Desktop.

endlocal

