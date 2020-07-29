
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

        // returns best intersect on scene
        virtual Intersect hit(const Ray &_ray) const = 0;
        
        // returns background color where ray hit
        virtual Color missColor(const Ray &_ray) const = 0;
    };
    

};  // namespace LNF


#endif  // #ifndef LIBS_HEADER_SCENE_H

