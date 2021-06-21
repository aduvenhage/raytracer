
#ifndef LIBS_HEADER_VEC3_H
#define LIBS_HEADER_VEC3_H

#include <array>
#include "constants.h"


namespace LNF
{
    /*
        X-Y-Z cartesian coordinate class.
     */
    struct Vec
    {
     public:
        Vec() noexcept
            :m_v{}
        {}
        
        Vec(const Vec &) noexcept = default;
        Vec(Vec &&) noexcept = default;
        //Vec(Vec &) noexcept = default;

        constexpr Vec(float _fX, float _fY, float _fZ) noexcept
            :m_v{_fX, _fY, _fZ}
        {}
        
        ~Vec() noexcept = default;
        
        float x() const {return m_v[0];}
        float &x() {return m_v[0];}
        float y() const {return m_v[1];}
        float &y() {return m_v[1];}
        float z() const {return m_v[2];}
        float &z() {return m_v[2];}
        
        Vec &operator=(const Vec &_vec) noexcept = default;
        Vec &operator=(Vec &&_vec) noexcept = default;

        float sizeSqr() const {
            return m_v[0] * m_v[0] + m_v[1] * m_v[1] + m_v[2] * m_v[2];
        }
        
        float size() const {
            return sqrt(sizeSqr());
        }
        
        Vec operator+(const Vec &_vec) const {
            return Vec(m_v[0] + _vec.x(),
                       m_v[1] + _vec.y(),
                       m_v[2] + _vec.z());
        }
        
        Vec operator-(const Vec &_vec) const {
            return Vec(m_v[0] - _vec.x(),
                       m_v[1] - _vec.y(),
                       m_v[2] - _vec.z());
        }
        
        Vec operator-() const {
            return inverse();
        }
        
        Vec &operator-=(const Vec &_vec) {
            m_v[0] -= _vec.x();
            m_v[1] -= _vec.y();
            m_v[2] -= _vec.z();
            return *this;
        }
        
        Vec &operator+=(const Vec &_vec) {
            m_v[0] += _vec.x();
            m_v[1] += _vec.y();
            m_v[2] += _vec.z();
            return *this;
        }
        
        // dot product
        float operator*(const Vec &_vec) const {
            return m_v[0] * _vec.x() +
                   m_v[1] * _vec.y() +
                   m_v[2] * _vec.z() ;
        }

        Vec operator*(float _fScale) const {
            return Vec(m_v[0] * _fScale,
                       m_v[1] * _fScale,
                       m_v[2] * _fScale);
        }
        
        Vec operator/(float _fScale) const {
            return Vec(m_v[0] / _fScale,
                       m_v[1] / _fScale,
                       m_v[2] / _fScale);
        }
        
        Vec &operator*=(float _fScale) {
            m_v[0] *= _fScale;
            m_v[1] *= _fScale;
            m_v[2] *= _fScale;
            return *this;
        }
        
        Vec &operator/=(float _fScale) {
            m_v[0] /= _fScale;
            m_v[1] /= _fScale;
            m_v[2] /= _fScale;
            return *this;
        }

        Vec normalized() const {
            float r = size();
            return Vec(m_v[0] / r,
                       m_v[1] / r,
                       m_v[2] / r);
        }
        
        Vec inverse() const {
            return Vec(-m_v[0], -m_v[1], -m_v[2]);
        }
        
        // per element abs() -- not the same as size
        Vec abs() const {
            return Vec(fabs(m_v[0]), fabs(m_v[1]), fabs(m_v[2]));
        }
        
        Vec xy() const {return Vec(m_v[0], m_v[1], 0);}
        Vec xz() const {return Vec(m_v[0], 0, m_v[2]);}
        Vec yz() const {return Vec(0, m_v[1], m_v[2]);}
        
        Vec scale(float _dX, float _dY, float _dZ) {
            return Vec(m_v[0] * _dX, m_v[1] * _dY, m_v[2] * _dZ);
        }
        
        float m_v[3];
    };
    
    
    inline Vec boxVec(float _fSize) {
        return Vec(_fSize, _fSize, _fSize);
    }

    inline Vec operator*(float _fScale, const Vec &_vec) {
        return Vec(_vec.x() * _fScale,
                   _vec.y() * _fScale,
                   _vec.z() * _fScale);
    }


    inline Vec operator/(float _numerator, const Vec &_vec) {
        return Vec(_numerator / _vec.x(),
                   _numerator / _vec.y(),
                   _numerator / _vec.z());
    }


    inline Vec crossProduct(const Vec &_a, const Vec &_b) {
        return Vec(_a.y() * _b.z() - _a.z() * _b.y(),
                   _a.z() * _b.x() - _a.x() * _b.z(),
                   _a.x() * _b.y() - _a.y() * _b.x());
    }

    
    inline Vec perElementScale(const Vec &_v, const Vec &_scale) {
        return Vec(_v.x() * _scale.x(),
                   _v.y() * _scale.y(),
                   _v.z() * _scale.z());
    }


    inline Vec perElementScale(const Vec &_v, const Vec &_origin, const Vec &_scale) {
        return Vec((_v.x() - _origin.x()) * _scale.x(),
                   (_v.y() - _origin.y()) * _scale.y(),
                   (_v.z() - _origin.z()) * _scale.z());
    }

    inline Vec perElementMax(const Vec &_vec1, const Vec &_vec2) {
        return Vec(fmax(_vec1.x(), _vec2.x()),
                   fmax(_vec1.y(), _vec2.y()),
                   fmax(_vec1.z(), _vec2.z()));
    }


    inline Vec perElementMin(const Vec &_vec1, const Vec &_vec2) {
        return Vec(fmin(_vec1.x(), _vec2.x()),
                   fmin(_vec1.y(), _vec2.y()),
                   fmin(_vec1.z(), _vec2.z()));
    }

    inline float minElement(const Vec &_vec) {
        return fmin(fmin(_vec.x(), _vec.y()), _vec.z());
    }

    inline float maxElement(const Vec &_vec) {
        return fmax(fmax(_vec.x(), _vec.y()), _vec.z());
    }

    inline Vec mod(const Vec &_p, float _d) {
        return Vec(
            _p.x() - _d * floor(_p.x() / _d),
            _p.y() - _d * floor(_p.y() / _d),
            _p.z() - _d * floor(_p.z() / _d)
        );
    }


    /*
     3 unit vector axis set
     (sort of like a matrix)
     */
    struct Axis
    {
        Axis() noexcept
            :m_fScale(1.0f)
        {}

        Axis(const Axis &) noexcept = default;
        Axis(Axis &&) noexcept = default;
        //Axis(Axis &) noexcept = default;
        
        template <typename VX, typename VY, typename VZ, typename P>
        Axis(VX &&_vx, VY &&_vy, VZ &&_vz, P &&_origin, float _fScale) noexcept
            :m_x(std::forward<VX>(_vx)),
             m_y(std::forward<VY>(_vy)),
             m_z(std::forward<VZ>(_vz)),
             m_origin(_origin),
             m_fScale(_fScale)
        {}

        Axis &operator=(const Axis &) noexcept = default;
        Axis &operator=(Axis &&) noexcept = default;
        
        Vec rotateTo(const Vec &_vec) const {
            return Vec(_vec * m_x, _vec * m_y, _vec * m_z);
        }
                
        Vec transformTo(const Vec &_vec) const {
            auto v = _vec - m_origin;
            return Vec(v * m_x, v * m_y, v * m_z) / m_fScale;
        }
                
        Vec rotateFrom(const Vec &_vec) const {
            return _vec.x() * m_x + _vec.y() * m_y + _vec.z() * m_z;
        }
        
        Vec transformFrom(const Vec &_vec) const {
            return (_vec.x() * m_x + _vec.y() * m_y + _vec.z() * m_z) * m_fScale + m_origin;
        }
        
        Vec     m_x;
        Vec     m_y;
        Vec     m_z;
        Vec     m_origin;
        float   m_fScale;
    };
    
    
    /*
     Min/Max bounds
     */
    struct Bounds
    {
        Bounds() noexcept = default;
        Bounds(const Bounds &) noexcept = default;
        Bounds(Bounds &&) noexcept = default;
        //Bounds(Bounds &) noexcept = default;
        
        template <typename VX, typename VY>
        Bounds(VX &&_min, VY &&_max) noexcept
            :m_min(std::forward<VX>(_min)),
             m_max(std::forward<VY>(_max))
        {}
        
        Bounds &operator=(const Bounds &) noexcept = default;
        Bounds &operator=(Bounds &&) noexcept = default;
        
        double area() const {
            Vec dist = m_max - m_min;            
            return 2 * dist.x() * dist.x() +
                   2 * dist.y() * dist.y() +
                   2 * dist.z() * dist.z();
        }
        
        double volume() const {
            Vec dist = m_max - m_min;
            return dist.x() * dist.y() * dist.z();
        }
        
        Vec size() const {
            return m_max - m_min;
        }
        
        Vec     m_min;
        Vec     m_max;
    };
    
    
    // split box on longest axis
    std::pair<Bounds, Bounds> splitBox(const Bounds &_bounds) {
        static constexpr float epsilon = 0.0001f;
        
        Vec dist = _bounds.m_max - _bounds.m_min;
        if ( (dist.x() + epsilon > dist.y()) && (dist.x() + epsilon >= dist.z()) ) {
            dist.x() *= 0.5;
        }
        else if ( (dist.y() + epsilon >= dist.x()) && (dist.y() + epsilon >= dist.z()) ) {
            dist.y() *= 0.5;
        }
        else {
            dist.z() *= 0.5;
        }
        
        return std::make_pair(Bounds(_bounds.m_min, _bounds.m_min + dist), Bounds(_bounds.m_max - dist, _bounds.m_max));
    }

    
    // combine bounds into one
    Bounds combineBoxes(const Bounds &_left, Bounds &_right) {
        return Bounds(perElementMin(_left.m_min, _right.m_min),
                      perElementMax(_left.m_max, _right.m_max));
    }


    // combine bounds into one
    Bounds combineBoxes(std::vector<Bounds> &_bounds) {
        Bounds bounds;
        
        if (_bounds.empty() == false) {
            bounds = _bounds[0];

            for (auto &b : _bounds) {
                bounds = combineBoxes(bounds, b);
            }
        }

        return bounds;
    }


    // finds min/max bounds from input points
    template <typename container_type>
    Bounds findBounds(const container_type &_points) {
        Bounds bounds;
        
        if (_points.empty() == false) {
            bounds.m_min = _points[0];
            bounds.m_max = _points[0];

            for (auto &p : _points) {
                bounds.m_min = perElementMin(bounds.m_min, p);
                bounds.m_max = perElementMax(bounds.m_max, p);
            }
        }

        return bounds;
    }

    // construct cuboid from bounds
    inline std::array<Vec, 8> boundsCuboid(const Bounds &_bounds) {
        return {
            Vec{_bounds.m_min.x(), _bounds.m_min.y(), _bounds.m_min.z()},
            Vec{_bounds.m_max.x(), _bounds.m_min.y(), _bounds.m_min.z()},
            Vec{_bounds.m_max.x(), _bounds.m_min.y(), _bounds.m_max.z()},
            Vec{_bounds.m_min.x(), _bounds.m_min.y(), _bounds.m_max.z()},
            Vec{_bounds.m_min.x(), _bounds.m_max.y(), _bounds.m_min.z()},
            Vec{_bounds.m_max.x(), _bounds.m_max.y(), _bounds.m_min.z()},
            Vec{_bounds.m_max.x(), _bounds.m_max.y(), _bounds.m_max.z()},
            Vec{_bounds.m_min.x(), _bounds.m_max.y(), _bounds.m_max.z()}
        };
    }


    // rotate axis aligned box and create a new axis aligned box
    inline Bounds rotateBounds(const Bounds &_bounds, const Axis &_axis) {
        auto cuboid = boundsCuboid(_bounds);
        for (auto &p : cuboid) {
            p = _axis.transformFrom(p);
        }
        
        return findBounds(cuboid);
    }


    // ray-box intersection (_invDir = 1 / ray_direction)
    inline bool aaboxIntersectCheck(const Bounds &_box, const Vec &_origin, const Vec &_invDir) {
        auto t1 = perElementScale(_box.m_min, _origin, _invDir);
        auto t2 = perElementScale(_box.m_max, _origin, _invDir);
        
        auto tmn = perElementMin(t1, t2);
        auto tmx = perElementMax(t1, t2);
        
        auto tmin = maxElement(tmn);
        auto tmax = minElement(tmx);
        
        return tmin < tmax;
    }


    // aaboxIntersect return
    struct AABoxItersect
    {
        AABoxItersect() noexcept
            :m_tmin(0),
             m_tmax(0),
             m_intersect(false),
             m_inside(false)
        {}
        
        float   m_tmin;
        float   m_tmax;
        bool    m_intersect;
        bool    m_inside;
    };


    // ray-box intersection (_invDir = 1 / ray_direction)
    inline AABoxItersect aaboxIntersect(const Bounds &_box, const Vec &_origin, const Vec &_invDir) {
        AABoxItersect ret;
        auto t1 = perElementScale(_box.m_min, _origin, _invDir);
        auto t2 = perElementScale(_box.m_max, _origin, _invDir);
        
        auto tmn = perElementMin(t1, t2);
        auto tmx = perElementMax(t1, t2);
        
        ret.m_tmin = maxElement(tmn);
        ret.m_tmax = minElement(tmx);
        ret.m_intersect = ret.m_tmin < ret.m_tmax;
        ret.m_inside = ret.m_intersect && (ret.m_tmin < 0) && (ret.m_tmax > 0);

        return ret;
    }
    
    
    // box-box intersection (checks whether A intersects with B)
    inline bool aaboxIntersectCheck(const Bounds &_boxA, const Bounds &_boxB) {
        return (_boxA.m_min.x() <= _boxB.m_max.x()) && (_boxA.m_max.x() >= _boxB.m_min.x()) &&
               (_boxA.m_min.y() <= _boxB.m_max.y()) && (_boxA.m_max.y() >= _boxB.m_min.y()) &&
               (_boxA.m_min.z() <= _boxB.m_max.z()) && (_boxA.m_max.z() >= _boxB.m_min.z());
    }


    // returns a vector within the unit cube (-1..1, -1..1, -1..1)
    inline Vec randomUnitCube(RandomGen &_randomGen) {
        std::uniform_real_distribution<float> dist(-1.0f, 1.0f);
        return Vec(dist(_randomGen),
                   dist(_randomGen),
                   dist(_randomGen));
    }

    
    // returns a vector within the unit disc (-1..1, -1..1, 0)
    inline Vec randomUnitSquare(RandomGen &_randomGen) {
        std::uniform_real_distribution<float> dist(-1.0f, 1.0f);
        return Vec(dist(_randomGen),
                   dist(_randomGen),
                   0);
    }

    
    // returns a vector within the unit sphere (radius of 1)
    inline Vec randomUnitSphere(RandomGen &_randomGen) {
        Vec ret = randomUnitCube(_randomGen);
        
        while (ret.sizeSqr() > 1) {
            ret = randomUnitCube(_randomGen);
        }
        
        return ret;
    }


    // returns a vector within the unit disc (y/x plane, radius of 1)
    inline Vec randomUnitDisc(RandomGen &_randomGen) {
        Vec ret = randomUnitSquare(_randomGen);
        
        while (ret.sizeSqr() > 1) {
            ret = randomUnitSquare(_randomGen);
        }
        
        return ret;
    }


    // get normal from surface function
    template <typename sdf_func>
    Vec surfaceNormal(const Vec &_p, const sdf_func &_sdf) {
        const float e = 0.0001f;
        return Vec(_sdf(_p + Vec{e, 0, 0}) - _sdf(_p - Vec{e, 0, 0}),
                   _sdf(_p + Vec{0, e, 0}) - _sdf(_p - Vec{0, e, 0}),
                   _sdf(_p + Vec{0, 0, e}) - _sdf(_p - Vec{0, 0, e})).normalized();
    }


    /* Creates the default axis */
    inline Axis axisIdentity() {
        return {
            Vec{1.0f, 0.0f, 0.0f},
            Vec{0.0f, 1.0f, 0.0f},
            Vec{0.0f, 0.0f, 1.0f},
            Vec{0.0f, 0.0f, 0.0f},
            1.0f
        };
    }

    /* Creates the default axis */
    template <typename P>
    inline Axis axisTranslation(P &&_origin, float _fScale = 1.0f) {
        return {
            Vec{1.0f, 0.0f, 0.0f},
            Vec{0.0f, 1.0f, 0.0f},
            Vec{0.0f, 0.0f, 1.0f},
            _origin,
            _fScale
        };
    }

    /*
     Creates an axis set with the given orientation.
     alpha - angle around Z axis
     beta  - angle around Y axis
     gamma - angle around X axis
     */
    template <typename P>
    Axis axisEulerZYX(float _fAlpha, float _fBeta, float _fGamma, P &&_origin, float _fScale = 1.0f) {
        const float ca = cos(_fAlpha);
        const float sa = sin(_fAlpha);
        const float cb = cos(_fBeta);
        const float sb = sin(_fBeta);
        const float cg = cos(_fGamma);
        const float sg = sin(_fGamma);

        return {
            Vec{ca*cb, sa*cb, -sb},
            Vec{ca*sb*sg - sa*cg, sa*sb*sg + ca*cg, cb*sg},
            Vec{ca*sb*cg + sa*sg, sa*sb*cg - ca*sg, cb*cg},
            _origin,
            _fScale
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
            _origin,
            1.0f
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

