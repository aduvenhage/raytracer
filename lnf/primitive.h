#ifndef LIBS_HEADER_NODE_H
#define LIBS_HEADER_NODE_H

#include "constants.h"
#include "intersect.h"
#include "material.h"
#include "ray.h"
#include "resource.h"
#include "vec3.h"

#include <memory>
#include <array>
#include <memory>
#include <atomic>
#include <mutex>


namespace LNF
{
    /*
        Scene Primitive
        API could be accessed by multiple worker threads concurrently.
     */
    class Primitive      : public Resource
    {
     public:
        /* Returns the material used for rendering, etc. */
        virtual const Material *material() const = 0;
        
        /* Quick node hit check (populates at least critical Intersect properties) */
        virtual bool hit(Intersect &_hit, RandomGen &_randomGen) const = 0;
        
        /* Completes the Intersect properties. */
        virtual Intersect &intersect(Intersect &_hit) const = 0;
        
        /* returns bounds for shape */
        virtual const Bounds &bounds() const = 0;
    };
    

    /*
        Transform and reference of a primitive.        
        API could be accessed by multiple worker threads concurrently.
    */
    class PrimitiveInstance
    {
     public:
        PrimitiveInstance()
            :m_bOwner(false),
             m_bDirtyBounds(true)
        {}
        
        PrimitiveInstance(const Primitive *_pTarget, const Axis &_axis)
            :m_pTarget(_pTarget),
             m_axis(_axis),
             m_bOwner(false),
             m_bDirtyBounds(true)
        {}
        
        PrimitiveInstance(std::unique_ptr<Primitive> &&_pTarget, const Axis &_axis)
            :m_pTarget(_pTarget.release()),
             m_axis(_axis),
             m_bOwner(true),
             m_bDirtyBounds(true)
        {}
        
        virtual ~PrimitiveInstance() {
            if (m_bOwner == true) {
                delete m_pTarget;
            }
        }
        
        /* Returns the material used for rendering, etc. */
        virtual const Material *material() const {
            return m_pTarget->material();
        }
        
        /* Quick node hit check (populates at least node and time properties of intercept) */
        virtual bool hit(Intersect &_hit, RandomGen &_randomGen) const {
            // check AA bounding volume first
            if (aaboxIntersectCheck(bounds(), _hit.m_viewRay) == true)
            {
                // transform ray for primitive hit
                _hit.m_priRay = transformRayTo(_hit.m_viewRay, m_axis);
                
                // check hit
                bool bHit = m_pTarget->hit(_hit, _randomGen);
                if (bHit == true) {
                    _hit.m_pPrimitive = this;
                    return true;
                }
            }
    
            return false;
        }

        /* Completes the Intersect properties. */
        virtual Intersect &intersect(Intersect &_hit) const {
            return m_pTarget->intersect(_hit);
        }
        
        /* tranform ray back to view space */
        virtual Ray transformRayFrom(const Ray &_ray) const {
            return LNF::transformRayFrom(_ray, m_axis);
        }

        /* move instance */
        virtual void move(const Vec &_origin) {
            m_axis.m_origin = _origin;
        }
        
        /*
         Rotates instance (creates an axis set with the given orientation).
         alpha - angle around Z axis
         beta  - angle around Y axis
         gamma - angle around X axis
         */
        void rotateEulerZYX(float _fAlpha, float _fBeta, float _fGamma) {
            m_axis = axisEulerZYX(_fAlpha, _fBeta, _fGamma, m_axis.m_origin);
        }
        
        /* return axis aligned bounding volume */
        const Bounds &bounds() const {
            if (m_bDirtyBounds == true) {
                std::lock_guard<std::mutex> lock(m_mutex);
                
                if (m_bDirtyBounds == true) {
                    m_bounds = rotateBounds(m_pTarget->bounds(), m_axis);
                    m_bDirtyBounds = false;
                }
            }
            
            return m_bounds;
        }
        
     private:
        const Primitive             *m_pTarget;
        Axis                        m_axis;
        bool                        m_bOwner;
        
        mutable std::mutex          m_mutex;
        mutable Bounds              m_bounds;
        mutable std::atomic<bool>   m_bDirtyBounds;
    };
    


};  // namespace LNF


#endif  // #ifndef LIBS_HEADER_NODE_H

