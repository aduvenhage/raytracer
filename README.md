[ --> About Me <-- ](https://aduvenhage.github.io/)

# Raytracing
Never did raytracing before, but tried it now and I'm hooked :-)

## Gallery
Some images I have made so far:<br>
<p float="left">
  <img src="gallery/raytracing/raytraced_4096.jpeg" alt="Clean raytracing (no monte carlo)" width="300"/>
  <img src="gallery/raytracing/raytraced_bulb2.jpeg" alt="Mandlebulb (raymarched)" width="300"/>
  <img src="gallery/raytracing/raymarched_nougat2.jpeg" alt="Raymarched goodness" width="300"/>
  <img src="gallery/raytracing/raytraced_bvh.jpeg" alt="Tracing many object fast" width="300"/>
  <img src="gallery/raytracing/raytraced_mist.jpeg" alt="Volume scattering" width="300"/>
  <img src="gallery/raytracing/raytraced_smog2.jpeg" alt="Volume scattering" width="300"/>
  <img src="gallery/raytracing/raytraced_nougat3.jpeg" alt="Raymarched goodness" width="300"/>
  <img src="gallery/raytracing/raytraced_spheres.jpeg" alt="Refraction" width="300"/>
  <img src="gallery/raytracing/raytraced_mandlebug.jpeg" alt="Refraction" width="300"/>
  <img src="gallery/raytracing/raytraced_subsurface_scatter.jpeg" alt="Refraction" width="300"/>
  <img src="gallery/raytracing/raytraced_bulb_field.jpeg" alt="Many bulb materials" width="300"/>
  <img src="gallery/raytracing/raytraced_default.jpeg" alt="Just some shapes" width="300"/>
  <img src="gallery/raytracing/raytraced_fog3.jpeg" alt="More fog" width="300"/>
  <img src="gallery/raytracing/raytraced_cornell.jpeg" alt="Cornell box" width="300"/>
  <img src="gallery/raytracing/raytraced_fractalbox.jpeg" alt="Cornell box" width="300"/>
</p>

## Features
List of implemented features:s
* Basic Features
  * rays and viewport
  * camera position and orientation
  * depth-of-field
  * gamma correction (fixed 2.0 correction)

* Primitives and Meshes
  * spheres, boxes, planes
  * sphere UV mapping
  * triangle mesh rendering
  * volumes / fog
  * raymarched objects
  * generic materials: diffuse, metal, glass, checkered diffuse
  * procedural materials: fractals, etc.
  * textured objects (texture images)

* Rendering and optimisations
  * monte-carlo based sampling and materials
  * russian roulette early exit
  * multi-threaded rendering
  * rotated and translated objects, and re-used objects (instancing)
  * axis aligned box intersections
  * bounding volume hyrarchy hit optimisations for scene objects
  * bounding volume hyrarchy hit optimisations for triangles within a mesh
  * optional memory manager (optimised small object allocations) to ensure objects are located close together in main memory

* Advanced Features
  * cloud runner (tested on AWS and DigitalOcean)
  * procedural/textured lights


Todo:
* triangle mesh loading (see https://github.com/assimp/assimp)
* procedural content using noise (https://github.com/Auburn/FastNoiseLite)
* replace axis-math with matrix math
* add shadow rays
* optisation through importance sampling (for lights)
* data based optimisations
* de-noising
* z-buffer and rasterised debug views (like viewing BVH volumes)
* support larger scenarios rendering multiple frames or multiple scenes per cloud VM (specific to cloud runner)
* optimize cloud VM promisioning (cloud-local images, smaller images, faster building, etc.)


## Feature blog
Ramblings on cool features and implementation details.  And, actually trying to write something down makes me think of improvements.

- [Rendering with Qt](pages/qt.md)
- [Building with CMAKE](pages/cmake.md)
- [Running in the cloud with docker-machine](pages/docker-machine.md)
- [Raytracing basics](pages/raytracing.md)
- [Random number generation](pages/random.md)
- [Job system](pages/jobs.md)
- [Debugging and compiler settings](pages/compiler_settings.md)

- [Awesome C++] [TODO]
- [Profiling and optimisations](pages/profiling.md) [WIP]
- [Advanced Raytracing](pages/raytracing_advanced.md) [WIP]
- [Hit optimisations using a Bounding Volume Hyrarchy](pages/bvh.md) [WIP]
- [Coordinates, Axis & Matrices](pages/axis.md) [WIP]
- [Camera Model](pages/camera.md) [WIP]


## Resources
- <https://www.realtimerendering.com/intersections.html>
- <https://www.realtimerendering.com/raytracinggems/>
- <https://raytracing.github.io/>
- <https://www.scratchapixel.com/>
- <https://github.com/aras-p/ToyPathTracer>




