pacman -S mingw-w64-x86_64-gcc cmake ninja --noconfirm --needed
mkdir build
cd build
cmake -DCMAKE_POLICY_VERSION_MINIMUM=3.5 -DCMAKE_BUILD_TYPE=Release -G Ninja ..
ninja
