pacman -S mingw-w64-x86_64-gcc cmake ninja --noconfirm --needed
mkdir build
cd build
cmake -G Ninja ..
ninja
