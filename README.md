# lost-n-found
I have been coding in C++ for a long long long time.  I made some colourful patterns, fire and starfields in sweet 320x240 and I was hooked!!

This project is an effort to stir up some ghosts and get back to what really made me want to be a programmer in the first place.

## General
For now the project is mainly just a set of header files thrown into a single namespace.

Done:
- create JPEG image writer for seeing what I created
- generic job/worker system (multi-theading)

Todo:
- add namespace hyrarchy for different components and sub-systems
- add canvas for realtime rendering and output (look at cinder -- libcinder.org)
- add doom fire
- add starfield
- add planet (nxn body) simulation
- add 3D mesh viewer (3ds max or Q3A formats maybe)

## Fractals
When I was younger I made a nice fractal explorer application and it was so awesome.

Done:
- got the mandlebrot down with some zooming around a coordinate

Todo:
- look at higher accuracy floating point

## Raytracing
Never did raytracing before, but tried it now and I'm hooked :-)

Done:
- rays and viewport
- ray sphere interactions
- sphere UV mapping
- monte-carlo based sampling and materials
- multi-threaded rendering (through job system) using sub-images
- generic materials: diffuse, metal, glass, checkered diffuse

Todo:
- axis aligned box intersections
- add bounding volume hyrarchy to scene
- camera position and orientation
- rotated box intersections
- sphere rotations (affects UV)
- triangle mesh rendering
- de-noising




