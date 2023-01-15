# Grassland

Grassland is a universal library designed for
common usage in graphics applications.
Using Vulkan and Direct3D 12 as its backend. (Working in Progress)

## Build

![auto judge](../../actions/workflows/linux-build.yml/badge.svg)
![auto judge](../../actions/workflows/windows-build.yml/badge.svg)

1. Clone project `git clone git@github.com:LazyJazz/grassland.git`
2. Enter directory `cd grassland`
3. Pull submodules `git submodule update --init --recursive`
4. Use CMake build the project.

## Vulkan SDK Installation

### Windows

1. Download Vulkan SDK installer for Windows from [Vulkan SDK Download page](https://vulkan.lunarg.com/sdk/home),
2. Run the installer program (As administrator), and follow the guide.
3. Reboot your system.

### Ubuntu 22.04

```bash
wget -qO- https://packages.lunarg.com/lunarg-signing-key-pub.asc | sudo tee /etc/apt/trusted.gpg.d/lunarg.asc
sudo wget -qO /etc/apt/sources.list.d/lunarg-vulkan-1.3.231-jammy.list https://packages.lunarg.com/vulkan/1.3.231/lunarg-vulkan-1.3.231-jammy.list
sudo apt update
sudo apt install vulkan-sdk
```

### Ubuntu 20.04

```bash
wget -qO - https://packages.lunarg.com/lunarg-signing-key-pub.asc | sudo apt-key add -
sudo wget -qO /etc/apt/sources.list.d/lunarg-vulkan-1.3.231-focal.list https://packages.lunarg.com/vulkan/1.3.231/lunarg-vulkan-1.3.231-focal.list
sudo apt update
sudo apt install vulkan-sdk
```

### Ubuntu 18.04

```bash
wget -qO - https://packages.lunarg.com/lunarg-signing-key-pub.asc | sudo apt-key add -
sudo wget -qO /etc/apt/sources.list.d/lunarg-vulkan-1.3.224-bionic.list https://packages.lunarg.com/vulkan/1.3.224/lunarg-vulkan-1.3.224-bionic.list
sudo apt update
sudo apt install vulkan-sdk
```

### Other Linux

1. Download Vulkan SDK package for Linux from [Vulkan SDK Download page](https://vulkan.lunarg.com/sdk/home).
2. Extract files from the package and save to a proper path, e.g. `~/vulkan_sdk/`.
3. Add the following contents to the end of `/etc/profile`, you may need to do this as super user, e.g., `sudo vim /etc/profile`.
```
export VULKAN_SDK=/your_path_to_vulkan_sdk/x86_64
export PATH=$PATH:$VULKAN_SDK/bin
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$VULKAN_SDK/lib
export VK_LAYER_PATH=$VULKAN_SDK/etc/explicit_layer.d
```
4. Reboot your system.

### Other Linux

You can try something similar with the process for Ubuntu.

### macOS

This works with both Intel and Apple Silicon.

1. Download Vulkan SDK Installer image for macOS from [Vulkan SDK Download page](https://vulkan.lunarg.com/sdk/home).
2. Run the installer, and follow the guide (It may require Rosetta 2 on Apple Silicon devices, however, the generated library will be on native Apple Silicon architecture).
3. Reboot your system.
