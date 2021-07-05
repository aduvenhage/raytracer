# Raytracing Basics

## A Good Start
When I started googling 'raytracing', I came across Scratchapixel's Introduction to Raytracing (https://www.scratchapixel.com/lessons/3d-basic-rendering/introduction-to-ray-tracing/how-does-it-work).  This helped me get started pretty quickly: Went from diffuse spheres to shiny metal spheres and from there to glass spheres -- very cool; quickly started with planes and patterns on them.

After that I discovered the 'Raytracing in One Weekend' series (https://raytracing.github.io/), which quickly helped me get to grips with the monte-carlo style of raytracing: Replaced light rays with, well, just more rays and got soft shadows for free; then added anti-aliasing, camera focus blur and lots more fun stuff.

## Scene, Primitives, Materials and Intercepts
My raytracer draws objects called `Primitives` located in the world, also called a `scene`.  Primitives can be spheres, boxes, volumes, triangle meshes, etc.  The ray to primitive intersection determines the shape of what you see and also provides shape properties like normal vectors, texture coordinates, etc.  Additionally `PrimitveIntances` wrap primitives and provide ray transformations, effectively allowing primitives to be rotated and placed anywhere in the scene.

While primitives determine where rays hit an object, materials determine what colour the hit returns and also how rays bounce off objects.  The material determines whether we are looking at glass, metal or some diffuse coloured or textured object.

Intercepts are the result of a ray/primitve hit.  The intercept is essentially the structure used to store the inbound as well as outbound hit products.  The raytracer and primitve hits are optimised to populate the intercept structure only partially, while still finding the closest hit, and then only calculating the complete set of intercept properties for the final hit. 

## Advanced Primitives
### Volumes
### Raymarching

## Advanced Materials

  
