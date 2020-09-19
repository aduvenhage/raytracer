
#ifndef LIBS_HEADER_VEC3_H
#define LIBS_HEADER_VEC3_H

#include "constants.h"


namespace LNF
{
    /*
        X-Y-Z cartesian coordinate class.
     */
    struct Vec
    {
     public:
        Vec()
            :m_fX(0),
             m_fY(0),
             m_fZ(0)
        {}
        
        Vec(const Vec &) = default;
        Vec(Vec &&) = default;
        Vec(Vec &) = default;

        Vec(float _fX, float _fY, float _fZ)
            :m_fX(_fX),
             m_fY(_fY),
             m_fZ(_fZ)
        {}
        
        Vec &operator=(const Vec &_vec) = default;
        Vec &operator=(Vec &&_vec) = default;

        float sizeSqr() const {
            return m_fX * m_fX + m_fY * m_fY + m_fZ * m_fZ;
        }
        
        float size() const {
            return sqrt(sizeSqr());
        }
        
        Vec operator+(const Vec &_vec) const {
            return Vec(m_fX + _vec.m_fX,
                       m_fY + _vec.m_fY,
                       m_fZ + _vec.m_fZ);
        }
        
        Vec operator-(const Vec &_vec) const {
            return Vec(m_fX - _vec.m_fX,
                       m_fY - _vec.m_fY,
                       m_fZ - _vec.m_fZ);
        }
        
        Vec operator-() const {
            return inverse();
        }
        
        Vec &operator-=(const Vec &_vec) {
            m_fX -= _vec.m_fX;
            m_fY -= _vec.m_fY;
            m_fZ -= _vec.m_fZ;
            return *this;
        }
        
        Vec &operator+=(const Vec &_vec) {
            m_fX += _vec.m_fX;
            m_fY += _vec.m_fY;
            m_fZ += _vec.m_fZ;
            return *this;
        }
        
        // dot product
        float operator*(const Vec &_vec) const {
            return m_fX * _vec.m_fX +
                   m_fY * _vec.m_fY +
                   m_fZ * _vec.m_fZ ;
        }

        Vec operator*(float _fScale) const {
            return Vec(m_fX * _fScale,
                       m_fY * _fScale,
                       m_fZ * _fScale);
        }
        
        Vec operator/(float _fScale) const {
            return Vec(m_fX / _fScale,
                       m_fY / _fScale,
                       m_fZ / _fScale);
        }
        
        Vec &operator*=(float _fScale) {
            m_fX *= _fScale;
            m_fY *= _fScale;
            m_fZ *= _fScale;
            return *this;
        }
        
        Vec &operator/=(float _fScale) {
            m_fX /= _fScale;
            m_fY /= _fScale;
            m_fZ /= _fScale;
            return *this;
        }

        Vec normalized() const {
            float r = size();
            return Vec(m_fX / r,
                       m_fY / r,
                       m_fZ / r);
        }
        
        Vec inverse() const {
            return Vec(-m_fX, -m_fY, -m_fZ);
        }
        
        // per element abs() -- not the same as size
        Vec abs() const {
            return Vec(fabs(m_fX), fabs(m_fY), fabs(m_fZ));
        }
        
        float      m_fX;
        float      m_fY;
        float      m_fZ;
    };


    inline Vec operator*(float _fScale, const Vec &_vec) {
        return Vec(_vec.m_fX * _fScale,
                   _vec.m_fY * _fScale,
                   _vec.m_fZ * _fScale);
    }


    inline Vec operator/(float _numerator, const Vec &_vec) {
        return Vec(_numerator / _vec.m_fX,
                   _numerator / _vec.m_fY,
                   _numerator / _vec.m_fZ);
    }


    inline Vec crossProduct(const Vec &_a, const Vec &_b) {
        return Vec(_a.m_fY * _b.m_fZ - _a.m_fZ * _b.m_fY,
                   _a.m_fZ * _b.m_fX - _a.m_fX * _b.m_fZ,
                   _a.m_fX * _b.m_fY - _a.m_fY * _b.m_fX);
    }

    
    inline Vec perElementScale(const Vec &_vec1, const Vec &_vec2) {
        return Vec(_vec1.m_fX * _vec2.m_fX,
                   _vec1.m_fY * _vec2.m_fY,
                   _vec1.m_fZ * _vec2.m_fZ);
    }


    inline Vec perElementMax(const Vec &_vec1, const Vec &_vec2) {
        return Vec(fmax(_vec1.m_fX, _vec2.m_fX),
                   fmax(_vec1.m_fY, _vec2.m_fY),
                   fmax(_vec1.m_fZ, _vec2.m_fZ));
    }


    inline Vec perElementMin(const Vec &_vec1, const Vec &_vec2) {
        return Vec(fmin(_vec1.m_fX, _vec2.m_fX),
                   fmin(_vec1.m_fY, _vec2.m_fY),
                   fmin(_vec1.m_fZ, _vec2.m_fZ));
    }

    inline float minElement(const Vec &_vec) {
        return fmin(fmin(_vec.m_fX, _vec.m_fY), _vec.m_fZ);
    }

    inline float maxElement(const Vec &_vec) {
        return fmax(fmax(_vec.m_fX, _vec.m_fY), _vec.m_fZ);
    }



    /*
     3 unit vector axis set
     (sort of like a matrix)
     */
    struct Axis
    {
        Axis() = default;
        Axis(const Axis &) = default;
        Axis(Axis &&) = default;
        Axis(Axis &) = default;
        
        template <typename VX, typename VY, typename VZ, typename P>
        Axis(VX &&_vx, VY &&_vy, VZ &&_vz, P &&_origin)
            :m_x(std::forward<VX>(_vx)),
             m_y(std::forward<VY>(_vy)),
             m_z(std::forward<VZ>(_vz)),
             m_origin(_origin)
        {}

        Axis &operator=(const Axis &) = default;
        Axis &operator=(Axis &&) = default;
        
        Vec rotateTo(const Vec &_vec) const {
            return Vec(_vec * m_x, _vec * m_y, _vec * m_z);
        }
                
        Vec transformTo(const Vec &_vec) const {
            auto v = _vec - m_origin;
            return Vec(v * m_x, v * m_y, v * m_z);
        }
                
        Vec rotateFrom(const Vec &_vec) const {
            return _vec.m_fX * m_x + _vec.m_fY * m_y + _vec.m_fZ * m_z;
        }
        
        Vec transformFrom(const Vec &_vec) const {
            return _vec.m_fX * m_x + _vec.m_fY * m_y + _vec.m_fZ * m_z + m_origin;
        }
        
        Vec     m_x;
        Vec     m_y;
        Vec     m_z;
        Vec     m_origin;
    };
    
    
    /*
     Min/Max bounds
     */
    struct Bounds
    {
        Bounds() = default;
        Bounds(const Bounds &) = default;
        Bounds(Bounds &&) = default;
        Bounds(Bounds &) = default;
        
        template <typename VX, typename VY>
        Bounds(VX &&_min, VY &&_max)
            :m_min(std::forward<VX>(_min)),
             m_max(std::forward<VY>(_max))
        {}
        
        Bounds &operator=(const Bounds &) = default;
        Bounds &operator=(Bounds &&) = default;
        
        double area() const {
            Vec dist = m_max - m_min;            
            return 2 * dist.m_fX * dist.m_fY +
                   2 * dist.m_fX * dist.m_fZ +
                   2 * dist.m_fY * dist.m_fZ;
        }
        
        double volume() const {
            Vec dist = m_max - m_min;
            return dist.m_fX * dist.m_fY * dist.m_fZ;
        }
        
        Vec     m_min;
        Vec     m_max;
    };
    
    
    // split box on longest axis
    std::pair<Bounds, Bounds> splitBox(const Bounds &_bounds) {
        Vec dist = _bounds.m_max - _bounds.m_min;
        if ( (dist.m_fX > dist.m_fY) && (dist.m_fX > dist.m_fZ) ) {
            dist.m_fX *= 0.5;
        }
        else if ( (dist.m_fY > dist.m_fX) && (dist.m_fY > dist.m_fZ) ) {
            dist.m_fY *= 0.5;
        }
        else {
            dist.m_fZ *= 0.5;
        }
        
        return std::make_pair(Bounds(_bounds.m_min, _bounds.m_min + dist), Bounds(_bounds.m_max - dist, _bounds.m_max));
                                
    }


    // ray-box intersection (_invDir = 1 / ray_direction)
    inline bool aaboxIntersectCheck(const Bounds &_box, const Vec &_origin, const Vec &_invDir) {
        auto t1 = perElementScale(_box.m_min - _origin, _invDir);
        auto t2 = perElementScale(_box.m_max - _origin, _invDir);
        
        auto tmn = perElementMin(t1, t2);
        auto tmx = perElementMax(t1, t2);
        
        auto tmin = maxElement(tmn);
        auto tmax = minElement(tmx);
        
        return tmin < tmax;
    }

    
    // ray-box intersection (_invDir = 1 / ray_direction)
    inline std::pair<float, bool> aaboxIntersect(const Bounds &_box, const Vec &_origin, const Vec &_invDir) {
        auto t1 = perElementScale(_box.m_min - _origin, _invDir);
        auto t2 = perElementScale(_box.m_max - _origin, _invDir);
        
        auto tmn = perElementMin(t1, t2);
        auto tmx = perElementMax(t1, t2);
        
        auto tmin = maxElement(tmn);
        auto tmax = minElement(tmx);
        
        auto intersect = tmin < tmax;
        auto inside = intersect && (tmin < 0) && (tmax > 0);
        auto t = inside ? tmax : tmin;

        return std::make_pair(intersect ? t : -1.0f, inside);
    }
    
    
    // box-box intersection (checks whether A intersects with B)
    inline bool aaboxIntersectCheck(const Bounds &_boxA, const Bounds &_boxB) {
        return (_boxA.m_min.m_fX <= _boxB.m_max.m_fX) && (_boxA.m_max.m_fX >= _boxB.m_min.m_fX) &&
               (_boxA.m_min.m_fY <= _boxB.m_max.m_fY) && (_boxA.m_max.m_fY >= _boxB.m_min.m_fY) &&
               (_boxA.m_min.m_fZ <= _boxB.m_max.m_fZ) && (_boxA.m_max.m_fZ >= _boxB.m_min.m_fZ);
    }


    // returns a vector within the unit cube (-1..1, -1..1, -1..1)
    inline Vec randomUnitCube(RandomGen &_randomGen) {
        std::uniform_real_distribution<float> dist(-1.0f, 1.0f);
        return Vec(dist(_randomGen),
                   dist(_randomGen),
                   dist(_randomGen));
    }

    
    // returns a vector within the unit sphere (radius of 1)
    inline Vec randomUnitSphere(RandomGen &_randomGen) {
        Vec ret = randomUnitCube(_randomGen);
        
        while (ret.sizeSqr() > 1) {
            ret = randomUnitCube(_randomGen);
        }
        
        return ret;
    }


    /* Creates the default axis */
    inline Axis axisIdentity() {
        return {
            Vec{1.0f, 0.0f, 0.0f},
            Vec{0.0f, 1.0f, 0.0f},
            Vec{0.0f, 0.0f, 1.0f},
            Vec{0.0f, 0.0f, 0.0f}
        };
    }

    /* Creates the default axis */
    template <typename P>
    inline Axis axisTranslation(P &&_origin) {
        return {
            Vec{1.0f, 0.0f, 0.0f},
            Vec{0.0f, 1.0f, 0.0f},
            Vec{0.0f, 0.0f, 1.0f},
            _origin
        };
    }

    /*
     Creates an axis set with the given orientation.
     alpha - angle around Z axis
     beta  - angle around Y axis
     gamma - angle around X axis
     */
    template <typename P>
    Axis axisEulerZYX(float _fAlpha, float _fBeta, float _fGamma, P &&_origin) {
        const float ca = cos(_fAlpha);
        const float sa = sin(_fAlpha);
        const float cb = cos(_fBeta);
        const float sb = sin(_fBeta);
        const float cg = cos(_fGamma);
        const float sg = sin(_fGamma);

        return {
            Vec{ca*cb,            sa*cb,            -sb},
            Vec{ca*sb*sg - sa*cg, sa*sb*sg + ca*cg, cb*sg},
            Vec{ca*sb*cg + sa*sg, sa*sb*cg - ca*sg, cb*cg},
            _origin
        };
    }
    
    /*
     Creates an axis set with the given orientation.
     alpha - angle around Z axis
     beta  - angle around Y axis
     gamma - angle around X axis
     */
    inline Axis axisEulerZYX(float _fAlpha, float _fBeta, float _fGamma) {
        return axisEulerZYX(_fAlpha, _fBeta, _fGamma, Vec{0, 0, 0});
    }


    /*
     Creates a lookat at axis set.
     lookat - position camera is aiming at
     origin - position of camera
     up - local camera 'up' vector
     
     returns: [left, up, lookat]
     */
    template <typename P>
    inline Axis axisLookat(const Vec &_lookat, P &&_origin, const Vec &_up) {
        auto lookat = (_lookat - _origin).normalized();
        auto left = crossProduct(_up, lookat).normalized();
        auto up = crossProduct(lookat, left);
        
        return {
            left,
            up,
            lookat,
            _origin
        };
    }


    /*
     Creates an axis on the plane.
     returns: [e1, normal, e2]
     */
    template <typename N, typename P>
    inline Axis axisPlane(N &&_normal, P &&_origin) {
        auto e1 = crossProduct(_normal, Vec(0.0f, 0.0f, 1.0f));
        if (e1.sizeSqr() < 0.0001f) {
            e1 = crossProduct(_normal, Vec(0.0f, 1.0f, 0.0f));
        }
        
        e1 = e1.normalized();
        auto e2 = crossProduct(_normal, e1);
        
        return {
            e1,
            _normal,
            e2,
            _origin
        };
    }
    


};  // namespace LNF

#endif  // #ifndef LIBS_HEADER_VEC3_H

