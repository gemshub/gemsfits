echo off
echo Setting up environment for Qt usage - see qtenv2.bat in mingw48 bin subfolder
set PATH=C:\Qt\Tools\mingw1120_64\bin;C:\Qt\6.4.1\mingw_64\bin;%PATH%
windeployqt %1
