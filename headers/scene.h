
#ifndef LIBS_HEADER_SCENE_H
#define LIBS_HEADER_SCENE_H

#include "constants.h"
#include "intersect.h"
#include "ray.h"
#include "shape.h"

#include <algorithm>
#include <limits>
#include <random>


namespace LNF
{

    /* Scene (word objects) */
    class Scene
    {
     public:
        virtual ~Scene() = default;

        /*
           Checks for an intersect with a scene object.
           Could be accessed by multiple worker threads concurrently.
         */
        virtual Intersect hit(const Ray &_ray) const = 0;
        
        /*
         Checks for the background color (miss handler).
         Could be accessed by multiple worker threads concurrently.
         */
        virtual Color missColor(const Ray &_ray) const = 0;
        
        /*
         Add a new shape to the scene.
         The scene is expected to be thread-safe.
         */
        virtual void addShape(std::unique_ptr<Shape> &&_pShape) = 0;
    };
    

};  // namespace LNF


#endif  // #ifndef LIBS_HEADER_SCENE_H

