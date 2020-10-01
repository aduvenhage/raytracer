# lost-n-found
I have been coding in C++ for a long long long time.
Remember 'Second Reality' by Future Crew? That was awesome.
After seeing that I made some colourful patterns, fire and starfields in sweet 320x240 and I was hooked!!

Currently by day I'm a python web backend programmer and this project is an effort to stir up some ghosts and get back to what really made me want to be a programmer in the first place.

## General
For now the project is mainly just a set of header files thrown into a single namespace.

Done:
- create JPEG image writer for seeing what I created
- generic job/worker system (multi-theading) -- using all of my CPU now :-)
- get into the weeds with some algorithm investigations (quicksort, breadth first searches with heuristics, etc. -- algorithms_playground.h)

Busy:
- create JPEG image reader for textures, etc.

Todo:
- add namespace hyrarchy for different components and sub-systems
- add canvas for realtime rendering and output (look at cinder -- libcinder.org)
- add doom fire
- add starfield
- add planet (nxn body) simulation
- add 3D mesh viewer (3ds max or Q3A formats maybe)
- play with some physics again

## Fractals
When I was younger I made a nice fractal explorer application and it was so awesome.

Done:
- got the mandlebrot down with some zooming around a coordinate

Todo:
- look at higher accuracy floating point

## Raytracing
Never did raytracing before, but tried it now and I'm hooked :-)

Resources:
- http://www.realtimerendering.com/intersections.html
- https://raytracing.github.io/
- https://www.scratchapixel.com/

Done:
- rays and viewport
- ray sphere interactions
- sphere UV mapping
- monte-carlo based sampling and materials
- multi-threaded rendering (through job system) using sub-images
- generic materials: diffuse, metal, glass, checkered diffuse
- rotated and translated objects (instancing)
- rotated box intersections
- sphere rotations (affects UV)
- axis aligned box intersections
- camera position and orientation
- add bounding volume hyrarchy to scene
- depth-of-field

Busy:
- triangle mesh rendering

Todo:
- triangle mesh loading (see https://github.com/assimp/assimp)
- textured objects (texture images)
- textured area lights
- volumes / fog
- replace axis-math with matrix math
- optisation through importance sampling (for lights)
- de-noising




