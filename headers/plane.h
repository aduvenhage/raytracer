#ifndef LIBS_HEADER_PLANE_H
#define LIBS_HEADER_PLANE_H

#include "constants.h"
#include "shape.h"
#include "material.h"

#include <memory>


namespace LNF
{
    /* Spehere class */
    class Plane        : public Shape
    {
     public:
        Plane()
        {}
        
        Plane(const Vec &_origin, const Vec &_normal, std::unique_ptr<Material> _pMaterial)
            :m_origin(_origin),
             m_normal(_normal),
             m_pMaterial(std::move(_pMaterial))
        {
            m_e1 = crossProduct(m_normal, Vec(0.0, 0.0, 1.0));
            m_e2 = crossProduct(m_normal, m_e1);
        }
        
        /*
         Returns the point (t) on the ray where it intersects this shape.
         Returns 0.0 if there is no intersect possible.
         */
        virtual double intersect(const Ray &_ray) const {
            double denom = m_normal * _ray.m_direction;
            if (denom < -0.0000001) {
                auto vecRayPlane = m_origin - _ray.m_origin;
                double t = (vecRayPlane * m_normal) / denom;
                if (t > 0) {
                    return t;
                }
            }
            
            return 0;
        }
        
        /* Returns the shape normal vector at the given surface position. */
        virtual Vec normal(const Vec &_pos) const {
            return m_normal;
        }

        /* Returns texture coordinates on given surface position. */
        virtual Uv uv(const Vec &_pos) const {
            auto vec = _pos - m_origin;

            return Uv(m_e1 * vec * 0.02, m_e2 * vec * 0.02).wrap();
        }
        
        /* Returns the diffuse color at the given surface position */
        virtual Color color(const Uv &_uv) const {
            return m_pMaterial->color(_uv);
        }
        
        /* Returns material property [0..1] */
        virtual double reflection() {
            return m_pMaterial->reflection();
        }

        /* Returns material property [0..1] */
        virtual double transparancy() {
            return m_pMaterial->transparancy();
        }

        /* Returns material property */
        virtual double indexOfRefraction() {
            return m_pMaterial->indexOfRefraction();
        }

     private:
        Vec                         m_origin;
        Vec                         m_normal;
        Vec                         m_e1;
        Vec                         m_e2;
        std::unique_ptr<Material>   m_pMaterial;
    };


};  // namespace LNF

#endif  // #ifndef LIBS_HEADER_PLANE_H

