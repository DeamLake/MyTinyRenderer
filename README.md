# MyTinyRenderer

This is a software rasterization application written by C++. 

The most of design details are based on [ssloy's tinyrenderer](https://github.com/ssloy/tinyrenderer/wiki) and [Games101](https://sites.cs.ucsb.edu/~lingqi/teaching/games101.html)

# Dependence

* glm
* windows API
# Main Features

* CMake
* Bling-Phong shading
* Perspective correct interpolation
* Back-face culling
* Movable camera
* Tangent space normal mapping
* Skybox
* Physicallly-Based Rendering
* Image_Based Lighting
# Build

## Linux

```powershell
cd /path/to/tinyrender
mkdir build
cd build
cmake ..
make
```
## Windows

```powershell
cd /path/to/tinyrender
mkdir build
cd build
cmake -G "Visual Studio 16 2019" ..
click SoftRenderer.sln && build all
```
# Controls

* Orbit: left mouse button
* Pan: right mouse button
* move: WASD
# Screenshots
## Gouraud Shading
| <img src="images/african_left.png" width="240px" height='320px'></img> | <img src="images/african_front.png" width="240px" height='320px'></img>|<img src="images/african_right.png" width="240px" height='320px'></img>|
| ------ | ------ | ------ |
| <img src="images/fuhua_left.png" width="240px" height='440px'></img> | <img src="images/fuhua_front.png" width="240px" height='440px'></img>|<img src="images/fuhua_right.png" width="300px" height='440px'></img>|

## Image_Based Lighting
| <img src="images/helmet_left.png" width="240px" height='240px'></img> | <img src="images/helmet_front.png" width="240px" height='240px'></img>|<img src="images/helmet_right.png" width="240px" height='240px'></img>|
| ------ | ------ | ------ |

<img src="images/gun_front.png" width="700px" height='320px'></img>