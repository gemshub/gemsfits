@echo off
setlocal EnableDelayedExpansion 

set ROOT_DIR=%cd%
set LOCALINSTALL=C:/usr/local
set PROGFILES=%ProgramFiles%
if not "%ProgramFiles(x86)%" == "" set PROGFILES=%ProgramFiles(x86)%

REM Check if Visual Studio 2017 comunity is installed
set MSVCDIR="%PROGFILES%\Microsoft Visual Studio\2017\Community"
set VCVARSALLPATH="%PROGFILES%\Microsoft Visual Studio\2017\Community\VC\Auxiliary\Build\vcvarsall.bat"        
if exist %MSVCDIR% (
  if exist %VCVARSALLPATH% (
   	set COMPILER_VER="2017"
        set COMPILER_VER_NAME="Visual Studio 15 2017"
        echo Using Visual Studio 2017 Community
	goto setup_env
  )
)

REM Check if Visual Studio 2019 comunity is installed
set MSVCDIR="%PROGFILES%\Microsoft Visual Studio\2019\Community"
set VCVARSALLPATH="%PROGFILES%\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvarsall.bat"        
if exist %MSVCDIR% (
  if exist %VCVARSALLPATH% (
   	set COMPILER_VER="2019"
        set COMPILER_VER_NAME="Visual Studio 16 2019"
        echo Using Visual Studio 2019 Community
	goto setup_env
  )
)


echo No compiler : Microsoft Visual Studio 2017 or 2019 Community is not installed.
goto end

:setup_env

echo "%MSVCDIR%\VC\Auxiliary\Build\vcvarsall.bat"
call %MSVCDIR%\VC\Auxiliary\Build\vcvarsall.bat x64

mkdir tmp_all
cd tmp_all

echo
echo ******                		 ******
echo ****** Compiling NLopt      ******
echo ******                		 ******
echo

echo Get NLopt from git...
git clone https://github.com/stevengj/nlopt.git
cd nlopt

echo "Configuring..."
REM cmake -G%COMPILER_VER_NAME% -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_FLAGS=-fPIC -DCMAKE_INSTALL_PREFIX=%LOCALINSTALL% .. -S . -B build
cmake -G%COMPILER_VER_NAME%  -DCMAKE_INSTALL_PREFIX=%LOCALINSTALL% .. -S . -B build
echo "Building..."
cmake --build build  --config Release --target install
cd ..

echo
echo ******                		 ******
echo ****** Compiling ejdb      ******
echo ******                		 ******
echo

echo Get ejdb from git...
git clone https://github.com/Softmotions/ejdb.git -b  v1.2.12
cd ejdb

echo "Configuring..."
cmake -G%COMPILER_VER_NAME%  -DCMAKE_INSTALL_PREFIX=%LOCALINSTALL% .. -S . -B build
echo "Building..."
cmake --build build  --config Release --target install
cd ..


echo
echo ******                		 ******
echo ****** Compiling yaml-cpp      ******
echo ******                		 ******
echo

echo Get yaml-cpp from git...
git clone https://github.com/jbeder/yaml-cpp.git
cd yaml-cpp

echo "Configuring..."
cmake -G%COMPILER_VER_NAME% -DYAML_CPP_BUILD_TOOLS=OFF -DYAML_CPP_BUILD_CONTRIB=OFF -DCMAKE_INSTALL_PREFIX=%LOCALINSTALL% .. -S . -B build
echo "Building..."
cmake --build build  --config Release --target install
cd ..\..


REM Housekeeping
rd /s /q tmp_all

:end

