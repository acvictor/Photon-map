# Photon Map
This is a bare-bones implementation of a Photon Map using foxed functionality OpenGL. 
<p align="left">
<img src="https://github.com/acvictor/Photon-map/blob/master/Images/8.png" alt="NMPC" width="420" height="430" border="50" /></a> 
<img src="https://github.com/acvictor/Photon-map/blob/master/Images/14.png" alt="NMPC" width="420" height="430" border="50" /></a>
</p>


## Compilation and Execution Instructions
 To compile and execute run
 ```
 make run
 ```

## Scene
 * The five walls are diffuse surfaces
 * The first sphere is specular
 * The second and third are refractive
 * The number of photons shot into the scene and stored in the map can be varied by editing lines 10 and 11 in [model.h](include/model.h)
 * To vary refractive index edit line 295 in [model.cpp](source/model.cpp)
 * To add new spheres edit [Scene::init()](source/model.cpp)

## Usage 
 * Use the arrow keys to move camera up, down, left and right
 * Use the '+' and '-' keys to move camera in and out
	* '+' zooms in
	* '-' zooms out
 * Use 'l' to move the light source left
 * Use 'r' to move the light source right
 * Use 'i' to move the light source in
 * Use 'o' to move the light source out
 * Use ESC to quit.


