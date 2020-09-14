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
            auto br = Ray(m_axis.translateTo(_ray.m_origin - m_origin),
                          m_axis.translateTo(_ray.m_direction));
            
            auto hit = m_pTarget->intersect(br);
            hit.m_normal = m_axis.translateFrom(hit.m_normal);
            hit.m_position = m_axis.translateFrom(hit.m_position) + m_origin;
            
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
        void rotateEulerZYX(double _dAlpha, double _dBeta, double _dGamma, bool _bRecalcBounds = true) {
            m_axis = axisEulerZYX(_dAlpha, _dBeta, _dGamma);
            
            if (_bRecalcBounds == true) {
                recalcBounds();
            }
        }
        
        /* calculate bounding volume */
        void recalcBounds() {
            const Bounds &tb = m_pTarget->bounds();
            
            // construct cuboid
            std::array<Vec, 8> points = {
                Vec{tb.m_min.m_dX, tb.m_min.m_dY, tb.m_min.m_dZ},
                Vec{tb.m_max.m_dX, tb.m_min.m_dY, tb.m_min.m_dZ},
                Vec{tb.m_max.m_dX, tb.m_min.m_dY, tb.m_max.m_dZ},
                Vec{tb.m_min.m_dX, tb.m_min.m_dY, tb.m_max.m_dZ},
                Vec{tb.m_min.m_dX, tb.m_max.m_dY, tb.m_min.m_dZ},
                Vec{tb.m_max.m_dX, tb.m_max.m_dY, tb.m_min.m_dZ},
                Vec{tb.m_max.m_dX, tb.m_max.m_dY, tb.m_max.m_dZ},
                Vec{tb.m_min.m_dX, tb.m_max.m_dY, tb.m_max.m_dZ}
            };
            
            // rotate and translate
            for (auto &vec : points) {
                vec = m_axis.translateFrom(vec) + m_origin;
            }
            
            // find min/max bounds
            
            
            
            
            
            
        }
        
     private:
        std::unique_ptr<Shape>  m_pTarget;
        Bounds                  m_bounds;
        Axis                    m_axis;
        Vec                     m_origin;
    };


};  // namespace LNF

#endif  // #ifndef LIBS_HEADER_SHAPE_TRANSFORM_H

