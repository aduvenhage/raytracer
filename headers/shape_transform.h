#ifndef LIBS_HEADER_SHAPE_TRANSFORM_H
#define LIBS_HEADER_SHAPE_TRANSFORM_H

#include "constants.h"
#include "intersect.h"
#include "material.h"
#include "shape.h"
#include "ray.h"
#include "vec3.h"

#include <memory>
#include <array>


namespace LNF
{

    /* Transform wrapper for shapes */
    class Transform        : public Shape
    {
     public:
        Transform()
        {}
        
        Transform(std::unique_ptr<Shape> &&_pTarget, const Axis &_axis, const Vec &_origin)
            :m_pTarget(std::move(_pTarget)),
             m_axis(_axis),
             m_origin(_origin)
        {
            recalcBounds();
        }
        
        /* Returns the material used for rendering, etc. */
        const Material *material() const override {
            return m_pTarget->material();
        }
        
        /* Returns the shape / ray intersect (calculates all hit properties). */
        virtual Intersect intersect(const Ray &_ray) const override {
            Intersect hit;

            // check AA bounding volume first
            float t[10];
            aaboxIntersect(t, m_bounds.m_min, m_bounds.m_max, _ray.m_origin, _ray.m_direction);
            if ( (t[9] > 0) || (t[0] > 0) )
            {
                // transform ray and check target intersect
                auto br = Ray(m_axis.translateTo(_ray.m_origin - m_origin),
                              m_axis.translateTo(_ray.m_direction));
                
                hit = m_pTarget->intersect(br);
                
                // transform hit back to world space
                hit.m_normal = m_axis.translateFrom(hit.m_normal);
                hit.m_position = m_axis.translateFrom(hit.m_position) + m_origin;
            }
    
            return hit;
        }
         
        /* returns bounds for shape */
        virtual const Bounds &bounds() const override {
            return  m_bounds;
        }
        
        /* move shape */
        virtual void move(const Vec &_origin, bool _bRecalcBounds = true) {
            m_origin = _origin;
            
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
            m_axis = axisEulerZYX(_fAlpha, _fBeta, _fGamma);
            
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
                vec = m_axis.translateFrom(vec) + m_origin;
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
        std::unique_ptr<Shape>  m_pTarget;
        Bounds                  m_bounds;
        Axis                    m_axis;
        Vec                     m_origin;
    };


};  // namespace LNF

#endif  // #ifndef LIBS_HEADER_SHAPE_TRANSFORM_H

