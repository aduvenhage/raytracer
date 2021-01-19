
#ifndef LIBS_HEADER_SCENE_H
#define LIBS_HEADER_SCENE_H

#include "constants.h"
#include "intersect.h"
#include "ray.h"
#include "primitive.h"
#include "material.h"

#include <algorithm>
#include <limits>
#include <random>


namespace LNF
{

    /* Scene (collection of primitives, BVH, etc.) */
    class Scene
    {
     public:
        virtual ~Scene() = default;

        /*
           Checks for an intersect with a scene object.
           Could be accessed by multiple worker threads concurrently.
         */
        virtual bool hit(Intersect &_hit, RandomGen &_randomGen) const = 0;
        
        /*
         Checks for the background color (miss handler).
         Could be accessed by multiple worker threads concurrently.
         */
        virtual Color backgroundColor() const = 0;
        
        /*
         Add a new resource (material, primitive, instance) to the scene.
         May not be safe to call while worker threads are calling 'hit'/
         */
        virtual Resource *addResource(std::unique_ptr<Resource> &&_pResource) = 0;

        /*
         Add a new primitive instance to the scene.
         May not be safe to call while worker threads are calling 'hit'/
         */
        virtual PrimitiveInstance *addPrimitiveInstance(std::unique_ptr<PrimitiveInstance> &&_pInstance) = 0;

    };
    
    
    // create primitive and add to scene as a resource
    template <typename primitive_type, class... T>
    Primitive *createPrimitive(Scene *_pScene, T ... t) {
        return static_cast<Primitive*>(
            _pScene->addResource(
                std::make_unique<primitive_type>(t ...)
            )
        );
    }


    // create new primitive instance that only references the provided primitive resource
    PrimitiveInstance *createPrimitiveInstance(Scene *_pScene, const Axis &_axis, const Primitive *_pPrimitive) {
        return _pScene->addPrimitiveInstance(
            std::make_unique<PrimitiveInstance>(
                _pPrimitive,
                _axis
            )
        );
    }


    // create new primitive instance that own a primitive (no other instancing possible)
    template <typename primitive_type, class... T>
    PrimitiveInstance *createPrimitiveInstance(Scene *_pScene, const Axis &_axis, T ... t) {
        return _pScene->addPrimitiveInstance(
            std::make_unique<PrimitiveInstance>(
                std::make_unique<primitive_type>(t ...),
                _axis
            )
        );
    }


    // create material and add to scene as a resource
    template <typename material_type, class... T>
    Material *createMaterial(Scene *_pScene, T ... t) {
        return static_cast<Material*>(
            _pScene->addResource(
                std::make_unique<material_type>(t ...)
            )
        );
    }



    

};  // namespace LNF


#endif  // #ifndef LIBS_HEADER_SCENE_H

