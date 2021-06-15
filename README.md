# Raytracing
Never did raytracing before, but tried it now and I'm hooked :-)

## Features


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
</p>


## Feautures
List of implemented features:
- rays and viewport
- camera position and orientation
- depth-of-field

- ray sphere interactions
- sphere UV mapping
- generic materials: diffuse, metal, glass, checkered diffuse
- raymarched materials
- triangle mesh rendering
- volumes / fog

- monte-carlo based sampling and materials
- multi-threaded rendering (through job system) using sub-images
- color variance based per pixel rays (quick exit)

- rotated and translated objects, and re-used objects (instancing)
- axis aligned box intersections
- bounding volume hyrarchy hit optimisations for scene objects
- bounding volume hyrarchy hit optimisations for triangles within a mesh


Todo:
- triangle mesh loading (see https://github.com/assimp/assimp)
- textured objects (texture images)
- textured area lights
- replace axis-math with matrix math
- optisation through importance sampling (for lights)
- de-noising
- z-buffer and rasterised debug views (like viewing BVH volumes)

## Feature blog
Ramblings on cool features and implementation details.
- [Rendering with Qt](pages/qt.md)
- [Job system](pages/jobs.md)
- [Hit optimisations using a Bounding Volume Hyrarchy](pages/bvh.md)
- [Using axis vectors not matrices](pages/axis.md)
- [Raymarching and mandlebulb](pages/raymarching.md)

## Resources
- http://www.realtimerendering.com/intersections.html
- https://raytracing.github.io/
- https://www.scratchapixel.com/
- https://github.com/aras-p/ToyPathTracer



