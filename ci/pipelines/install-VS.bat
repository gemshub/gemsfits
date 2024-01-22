call activate gemsfits

call "C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvars64.bat"

conda list
echo "Configuring..."
cmake -G"Visual Studio 16 2019" -A x64 -S . -B build
echo "Building..."
cmake --build build --target install
