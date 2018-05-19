Compilation and Execution Instructions
======================================
 * Run 'make run' in the terminal to compile and execute

Scene
=======
 * The five walls are diffuse surfaces
 * The first sphere is specular
 * The second and third are refractive
 * The number of photons shot into the scene and stored in the map can be varied by editing lines 10 and 11 in model.h
 * To vary refractive index edit line 295 in model.cpp
 * To add new spheres edit Scene::init()

Usage 
=====
 * Use the arrow keys to move camera up, down, left and right
 * Use the '+' and '-' keys to move camera in and out
	'+' zooms in
	'-' zooms out
 * Use 'l' to move the light source left
 * Use 'r' to move the light source right
 * Use 'i' to move the light source in
 * Use 'o' to move the light source out
 * Use ESC to quit.

External Libraries
==================
 * Fast Trackball

Future Work
===========
 * Integrate with a ray tracer

