call activate gemsfits

call "C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvars64.bat"

echo "Configuring..."
cmake -G"Visual Studio 16 2019" -A x64 -S . -B build  -DUSE_OLD_EJDB=ON
echo "Building..."
cmake --build build --target install
