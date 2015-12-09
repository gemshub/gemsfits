echo off
echo Setting up environment for Qt usage - see qtenv2.bat in mingw48 bin subfolder
set PATH=C:\Qt\Qt5.5.0\Tools\mingw492_32\bin;C:\Qt\Qt5.5.0\Tools\mingw492_32\bin;%PATH%
windeployqt %1
