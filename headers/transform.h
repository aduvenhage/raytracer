#ifndef LIBS_HEADER_TRANSFORM_H
#define LIBS_HEADER_TRANSFORM_H

#include "constants.h"
#include "intersect.h"
#include "material.h"
#include "node.h"
#include "ray.h"
#include "vec3.h"

#include <memory>
#include <array>


namespace LNF
{

    /* Transform wrapper for raytracing node */
    class Transform        : public Node
    {
     public:
        Transform()
        {}
        
        Transform(std::unique_ptr<Node> &&_pTarget, const Axis &_axis)
            :m_pTarget(std::move(_pTarget)),
             m_axis(_axis)
        {
            recalcBounds();
        }
        
        /* Returns the material used for rendering, etc. */
        const Material *material() const override {
            return nullptr;
        }
        
        /* Quick node hit check (populates at least node and time properties of intercept) */
        virtual bool hit(Intersect &_hit, const Ray &_ray) const override {
            // check AA bounding volume first
            float t[10];
            aaboxIntersect(t, m_bounds.m_min, m_bounds.m_max, _ray.m_origin, _ray.m_direction);
            if ( (t[9] > 0) || (t[0] > 0) )
            {
                // transform ray and check target intersect
                auto br = Ray(m_axis.transformTo(_ray.m_origin), m_axis.rotateTo(_ray.m_direction));
                
                // check target hit
                _hit.m_axis = m_axis;
                return m_pTarget->hit(_hit, br);
            }
    
            return false;
        }
        
        /* Completes the node intersect properties. */
        virtual Intersect &intersect(Intersect &_hit) const override {
            m_pTarget->intersect(_hit);
            return _hit;
        }
         
        /* returns bounds for shape */
        virtual const Bounds &bounds() const override {
            return  m_bounds;
        }
        
        /* move shape */
        virtual void move(const Vec &_origin, bool _bRecalcBounds = true) {
            m_axis.m_origin = _origin;
            
            if (_bRecalcBounds == true) {
                recalcBounds();
            }
        }
        
        /*
         Creates an axis set with the given orientation.
         alpha - angle around Z axis
         beta  - angle around Y axis
         gamma - angle around X axis
         */
        void rotateEulerZYX(float _fAlpha, float _fBeta, float _fGamma, bool _bRecalcBounds = true) {
            m_axis = axisEulerZYX(_fAlpha, _fBeta, _fGamma, m_axis.m_origin);
            
            if (_bRecalcBounds == true) {
                recalcBounds();
            }
        }
        
        /* calculate bounding volume */
        void recalcBounds() {
            const Bounds &tb = m_pTarget->bounds();
            
            // construct cuboid
            std::array<Vec, 8> points = {
                Vec{tb.m_min.m_fX, tb.m_min.m_fY, tb.m_min.m_fZ},
                Vec{tb.m_max.m_fX, tb.m_min.m_fY, tb.m_min.m_fZ},
                Vec{tb.m_max.m_fX, tb.m_min.m_fY, tb.m_max.m_fZ},
                Vec{tb.m_min.m_fX, tb.m_min.m_fY, tb.m_max.m_fZ},
                Vec{tb.m_min.m_fX, tb.m_max.m_fY, tb.m_min.m_fZ},
                Vec{tb.m_max.m_fX, tb.m_max.m_fY, tb.m_min.m_fZ},
                Vec{tb.m_max.m_fX, tb.m_max.m_fY, tb.m_max.m_fZ},
                Vec{tb.m_min.m_fX, tb.m_max.m_fY, tb.m_max.m_fZ}
            };
            
            // rotate and translate
            for (auto &vec : points) {
                vec = m_axis.transformFrom(vec);
            }
            
            // find min/max bounds
            m_bounds.m_min = points[0];
            m_bounds.m_max = points[0];
            
            for (auto &vec : points) {
                m_bounds.m_min.m_fX = vec.m_fX < m_bounds.m_min.m_fX ? vec.m_fX : m_bounds.m_min.m_fX;
                m_bounds.m_min.m_fY = vec.m_fY < m_bounds.m_min.m_fY ? vec.m_fY : m_bounds.m_min.m_fY;
                m_bounds.m_min.m_fZ = vec.m_fZ < m_bounds.m_min.m_fZ ? vec.m_fZ : m_bounds.m_min.m_fZ;
                
                m_bounds.m_max.m_fX = vec.m_fX > m_bounds.m_max.m_fX ? vec.m_fX : m_bounds.m_max.m_fX;
                m_bounds.m_max.m_fY = vec.m_fY > m_bounds.m_max.m_fY ? vec.m_fY : m_bounds.m_max.m_fY;
                m_bounds.m_max.m_fZ = vec.m_fZ > m_bounds.m_max.m_fZ ? vec.m_fZ : m_bounds.m_max.m_fZ;
            }
        }
        
     private:
        std::unique_ptr<Node>  m_pTarget;
        Bounds                   m_bounds;
        Axis                     m_axis;
    };


};  // namespace LNF

#endif  // #ifndef LIBS_HEADER_TRANSFORM_H

