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
        
        /* Quick node hit check (populates at least node and time properties of intercept) */
        virtual bool hit(Intersect &_hit, RandomGen &_randomGen) const {
            // check AA bounding volume first
            if (aaboxIntersectCheck(bounds(), _hit.m_ray.m_origin, _hit.m_ray.m_invDirection) == true)
            {
                // transform ray and check target intersect
                auto br = Ray(m_axis.transformTo(_hit.m_ray.m_origin),
                              m_axis.rotateTo(_hit.m_ray.m_direction));
                
                // check target hit
                _hit.m_axis = m_axis;
                _hit.m_ray = br;
                _hit.m_pPrimitive = m_pTarget;

                return m_pTarget->hit(_hit, _randomGen);
            }
    
            return false;
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
                    const Bounds &tb = m_pTarget->bounds();
                    
                    // construct cuboid
                    std::array<Vec, 8> points = {
                        Vec{tb.m_min.x(), tb.m_min.y(), tb.m_min.z()},
                        Vec{tb.m_max.x(), tb.m_min.y(), tb.m_min.z()},
                        Vec{tb.m_max.x(), tb.m_min.y(), tb.m_max.z()},
                        Vec{tb.m_min.x(), tb.m_min.y(), tb.m_max.z()},
                        Vec{tb.m_min.x(), tb.m_max.y(), tb.m_min.z()},
                        Vec{tb.m_max.x(), tb.m_max.y(), tb.m_min.z()},
                        Vec{tb.m_max.x(), tb.m_max.y(), tb.m_max.z()},
                        Vec{tb.m_min.x(), tb.m_max.y(), tb.m_max.z()}
                    };
                    
                    // rotate and translate
                    for (auto &vec : points) {
                        vec = m_axis.transformFrom(vec);
                    }
                    
                    // find min/max bounds
                    m_bounds.m_min = points[0];
                    m_bounds.m_max = points[0];
                    
                    for (auto &vec : points) {
                        m_bounds.m_min.x() = vec.x() < m_bounds.m_min.x() ? vec.x() : m_bounds.m_min.x();
                        m_bounds.m_min.y() = vec.y() < m_bounds.m_min.y() ? vec.y() : m_bounds.m_min.y();
                        m_bounds.m_min.z() = vec.z() < m_bounds.m_min.z() ? vec.z() : m_bounds.m_min.z();
                        
                        m_bounds.m_max.x() = vec.x() > m_bounds.m_max.x() ? vec.x() : m_bounds.m_max.x();
                        m_bounds.m_max.y() = vec.y() > m_bounds.m_max.y() ? vec.y() : m_bounds.m_max.y();
                        m_bounds.m_max.z() = vec.z() > m_bounds.m_max.z() ? vec.z() : m_bounds.m_max.z();
                    }
                    
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
    
    
    /*
     Bounding volume hyrarchy nodes.
     */
    struct PrimitiveNode
    {
        PrimitiveNode()
            :m_iLevel(0)
        {}
        
        Bounds                                   m_bounds;
        std::unique_ptr<PrimitiveNode>           m_left;
        std::unique_ptr<PrimitiveNode>           m_right;
        std::vector<PrimitiveInstance*>          m_primitives;       // primitives (leaf nodes)
        int                                      m_iLevel;
    };
    
    
    
    

};  // namespace LNF


#endif  // #ifndef LIBS_HEADER_NODE_H

