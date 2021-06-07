# Raytracing
Never did raytracing before, but tried it now and I'm hooked :-)

## Features


## Gallery
Some images I have made so far:<br>
<img src="/gallery/raytracing/raytraced_4096.jpeg" alt="Clean raytracing (no monte carlo)" width="300"/>
<img src="/gallery/raytracing/raytraced_bulb2.jpeg" alt="Mandlebulb (raymarched)" width="300"/>


## Development
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
- raymarched materials
- triangle mesh rendering
- look at instancing where shapes can be re-used in scene
- volumes / fog

Busy:
- profile and update math operations
- color variance based per pixel rays (quick exit)

Todo:
- triangle mesh loading (see https://github.com/assimp/assimp)
- textured objects (texture images)
- textured area lights
- replace axis-math with matrix math
- optisation through importance sampling (for lights)
- de-noising

## Resources
- http://www.realtimerendering.com/intersections.html
- https://raytracing.github.io/
- https://www.scratchapixel.com/




