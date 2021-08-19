# Raytracing Basics

## A Good Start
When I started googling 'raytracing', I came across Scratchapixel's Introduction to Raytracing (https://www.scratchapixel.com/lessons/3d-basic-rendering/introduction-to-ray-tracing/how-does-it-work).  This helped me get started pretty quickly: Went from diffuse spheres to shiny metal spheres and from there to glass spheres -- very cool; also started with planes and patterns on them.

After that I discovered the 'Raytracing in One Weekend' series (https://raytracing.github.io/), which quickly helped me get to grips with the monte-carlo style of raytracing: Replaced light rays with, well, just more rays and got soft shadows for free; then added anti-aliasing, camera focus blur and lots more fun stuff.

## Scene, Primitives, Materials and Intercepts
My raytracer draws objects called `Primitives` located in the world, also called a `Scene`.  Primitives can be spheres, boxes, volumes, triangle meshes, etc.  The ray to primitive intersection determines the shape of what you see and also provides shape properties like normal vectors, texture coordinates, etc.  Additionally `PrimitveIntances` wrap primitives and provide ray transformations, effectively allowing primitives to be rotated and placed anywhere in the scene.

While primitives determine where rays hit an object, `materials` determine what colour the hit returns and also how rays bounce off primitives.  The material determines whether we are looking at glass, metal or some diffuse coloured or textured object.

`Intercepts` are the result of a ray/primitve hit.  The intercept is essentially the structure used to store the inbound as well as outbound hit products.  The raytracer and primitve hits are optimised to populate the intercept structure only partially, while still finding the closest hit, and then calculating the complete set of intercept properties only for the final hit.  The hit products included in the intercept are, for example: the incoming ray, the transformed ray, the surface distance (distance on ray), the surface hit position, the surface normal, UV/texture coordinates, triangle index, inside/outside shape and material hit.  Additionally it also includes properties we can use for some cool shading effects like, trace depth, surface hit iterations (for fractals, etc.), ray marched iterations, etc.

The intercept also includes which material was hit so that the raytracer can go ahead and generate the next set of rays. Currently this is done recursively (i.e. ray is generated, ray is traced and hits something, hit creates more rays off of material and then we go one level deeper.  One optimisation would be to not do this recursively, but to do tracing breadth first, sort rays according to hit material and/or direction and then do the next level of tracing on the sorted rays.  This would help quite a bit when we have many more materials, with large textures etc.

## Camera and Viewport
The camera model determines in what direction and from where we cast the initial set of rays. The viewport ties together the camera and the output canvas size.  This is used to create the initial set of rays. Normally we would create more than one ray per pixel in the view.  With multiple rays per pixel, each ray follows a slightly different path and we then sum up and average out the color to get to the color value of the pixel.

The camera is also modelled with a variable aperture and focus distance. The values are a bit fudged, but essentially for each ray we pick a random origin within a disc on the x/y plane and cast it at a virtual pixel on the viewport some distance from the camera. The size of the disc at the camera is controlled by the aperture value and the exact pixel location is varied with some sub-pixel randomness. This gives us and depth-of-field effect, controller by the aperture value, and some anti-aliasing of primitve edges.  The camera ray is transformed from screen space to world using the camera axis before it is used by the raytracer.

It the current version of the raytracer I'm also playing with pixel color stats, stopping the per pixel tracing early if the pixel color variance is low.  This is however still being tweaked, but does make the rendering a bit faster, since in most cases we do not have to do the full number of samples per pixel.
  
