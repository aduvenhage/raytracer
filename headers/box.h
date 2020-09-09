#ifndef LIBS_HEADER_BOX_H
#define LIBS_HEADER_BOX_H

#include "constants.h"
#include "shape.h"
#include "vec3.h"
#include "uv.h"


namespace LNF
{
    /* test if ray is inside box or not */
    inline double *aaboxIntersect(double _t[10], const Vec &_min, const Vec &_max, const Vec &_rayOrigin, const Vec &_rayDirection) {
        _t[1] = (_min.m_dX - _rayOrigin.m_dX) / _rayDirection.m_dX;
        _t[2] = (_max.m_dX - _rayOrigin.m_dX) / _rayDirection.m_dX;
        _t[3] = (_min.m_dY - _rayOrigin.m_dY) / _rayDirection.m_dY;
        _t[4] = (_max.m_dY - _rayOrigin.m_dY) / _rayDirection.m_dY;
        _t[5] = (_min.m_dZ - _rayOrigin.m_dZ) / _rayDirection.m_dZ;
        _t[6] = (_max.m_dZ - _rayOrigin.m_dZ) / _rayDirection.m_dZ;
        
        _t[7] = fmax(fmax(fmin(_t[1], _t[2]), fmin(_t[3], _t[4])), fmin(_t[5], _t[6]));
        _t[8] = fmin(fmin(fmax(_t[1], _t[2]), fmax(_t[3], _t[4])), fmax(_t[5], _t[6]));
        _t[9] = (_t[8] < 0 || _t[7] > _t[8]) ? -1 : _t[7];
        
        return _t;
    }


    /* simple yes/no ray box intersect */
    inline bool aaboxIntersect(const Vec &_min, const Vec &_max, const Ray &_ray) {
        double t[10];
        aaboxIntersect(t, _min, _max, _ray.m_origin, _ray.m_direction);
        return t[9] > 0.0001;
    }


    /* Axis aligned box shape class -- fixed at origin [0, 0, 0] */
    class Box        : public Shape
    {
     public:
        Box()
        {}
        
        Box(const Vec &_size, const std::shared_ptr<Material> &_pMaterial, double _dUvScale = 0.2)
            :m_vecSize(_size),
             m_vecMin(-_size * 0.5),
             m_vecMax(_size * 0.5),
             m_vecDiv(_size * 0.49999),
             m_pMaterial(std::move(_pMaterial)),
             m_dUvScale(_dUvScale)
        {}
        
        /* Returns the material used for rendering, etc. */
        const Material *material() const override {
            return m_pMaterial.get();
        }
        
        /* Returns the shape / ray intersect (calculates all hit properties). */
        virtual Intersect intersect(const Ray &_ray) const override {
            Intersect ret;
            static const Vec _a[] = {{0, 1, 0}, {-1, 0, 0}, {0, 1, 0}};
            static const Vec _b[] = {{0, 0, 1}, {0, 0, 1}, {-1, 0, 0}};

            double t[10];
            aaboxIntersect(t, m_vecMin, m_vecMax, _ray.m_origin, _ray.m_direction);
            if (t[9] > 0) {
                ret.m_pShape = this;
                ret.m_dPositionOnRay = t[9];
                ret.m_position = _ray.position(ret.m_dPositionOnRay);
                
                ret.m_normal = Vec((int)(ret.m_position.m_dX / m_vecDiv.m_dX),
                                   (int)(ret.m_position.m_dY / m_vecDiv.m_dY),
                                   (int)(ret.m_position.m_dZ / m_vecDiv.m_dZ));
                
                const int i = int(fabs(ret.m_normal.m_dY + ret.m_normal.m_dZ * 2) + 0.5);
                const auto &a = _a[i];
                const auto &b = _b[i];

                const auto p2 = ret.m_position - m_vecMin;
                ret.m_uv = Uv(a * p2 * m_dUvScale, b * p2 * m_dUvScale).wrap();
            }
            
            return ret;
        }
        
     private:
        Axis                        m_axis;
        Vec                         m_vecSize;
        Vec                         m_vecMin;
        Vec                         m_vecMax;
        Vec                         m_vecDiv;
        std::shared_ptr<Material>   m_pMaterial;
        double                      m_dUvScale;
    };


};  // namespace LNF

#endif  // #ifndef LIBS_HEADER_BOX_H

