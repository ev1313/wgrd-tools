pacman -S gcc cmake ninja --noconfirm --needed
mkdir build
cd build
cmake -G Ninja ..
ninja
