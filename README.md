# Firmware
Fetch and build fx2lib
```
git submodule update --init --recursive
cd firmware/fx2lib
make
```
## Build Firmware
```
cd firmware
mkdir build && cd build
cmake ..
make
```
# Host Software
```
mkdir build && cd build
cmake ..
make
```
