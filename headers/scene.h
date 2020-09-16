
#ifndef LIBS_HEADER_SCENE_H
#define LIBS_HEADER_SCENE_H

#include "constants.h"
#include "intersect.h"
#include "ray.h"
#include "node.h"

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
         Add a new node to the scene.
         The scene is expected to be thread-safe.
         */
        virtual void addNode(std::unique_ptr<Node> &&_pNode) = 0;
    };
    

};  // namespace LNF


#endif  // #ifndef LIBS_HEADER_SCENE_H

