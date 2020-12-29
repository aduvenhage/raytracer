#ifndef LIBS_HEADER_MARCHED_NODE_H
#define LIBS_HEADER_MARCHED_NODE_H

#include "constants.h"
#include "node.h"
#include "trace.h"
#include "uv.h"
#include "vec3.h"


namespace LNF
{
    /* Axis aligned box shape class -- fixed at origin [0, 0, 0]; base class for raymarched nodes */
    class MarchedBox        : public Node
    {
     public:
        MarchedBox(const Vec &_size, const Material *_pMaterial, int _iMaxSamples)
            :m_bounds(-_size * 0.5f, _size * 0.5f),
             m_pMaterial(_pMaterial),
             m_iMaxSamples(_iMaxSamples)
        {}
        
        MarchedBox(float _fSize, const Material *_pMaterial, int _iMaxSamples)
            :m_bounds(boxVec(-_fSize*0.5), boxVec(_fSize*0.5)),
             m_pMaterial(_pMaterial),
             m_iMaxSamples(_iMaxSamples)
        {}
        
        /* Returns the material used for rendering, etc. */
        const Material *material() const override {
            return m_pMaterial;
        }
        
        /* Quick node hit check (populates at least node and time properties of intercept) */
        virtual bool hit(Intersect &_hit, const Ray &_ray, RandomGen &) const override {
            auto bi = aaboxIntersect(m_bounds, _ray.m_origin, _ray.m_invDirection);
            if (bi.m_intersect == true) {
                // try to hit surface inside (using raymarching)
                // TODO: calc surface UV
                bool is_hit = check_marched_hit(_hit,
                                                _ray,
                                                m_iMaxSamples,
                                                bi.m_tmax,
                                                [this](const Vec &_p){return this->sdfSurface(_p);});

                if ( (is_hit == true) &&
                     ((_hit.m_fPositionOnRay >= _ray.m_fMinDist) && (_hit.m_fPositionOnRay <= _ray.m_fMaxDist)) )
                {
                    _hit.m_pNode = this;
                    return true;
                }
            }
            
            return false;
        }
        
        /* Completes the node intersect properties. */
        virtual Intersect &intersect(Intersect &_hit) const override {
            _hit.m_normal = surfaceNormal(_hit.m_position,
                                          [this](const Vec &_p){return this->sdfSurface(_p);});

            return _hit;
        }
                
        /* returns bounds for shape */
        virtual const Bounds &bounds() const override {
            return  m_bounds;
        }
        
     protected:
        // surface signed distance function
        virtual float sdfSurface(const Vec &_p) const = 0;
        
     private:
        Axis                   m_axis;
        Bounds                 m_bounds;
        const Material         *m_pMaterial;
        int                    m_iMaxSamples;
    };


    // squishy blob thingy
    class MarchedSwirl        : public MarchedBox
    {
     public:
        MarchedSwirl(const Vec &_size, const Material *_pMaterial, int _iMaxSamples)
            :MarchedBox(_size, _pMaterial, _iMaxSamples),
             m_fSize(_size.size() * 0.5)
        {}
        
        MarchedSwirl(float _fSize, const Material *_pMaterial, int _iMaxSamples)
            :MarchedBox(_fSize, _pMaterial, _iMaxSamples),
             m_fSize(_fSize * 0.5)
        {}
        
     protected:
        // surface signed distance function
        virtual float sdfSurface(const Vec &_p) const override {
            return (_p.size() - m_fSize * (1 + 0.2 * sin(_p.x()/2) * sin(_p.y()/2) * sin(_p.z()/2)));
        }
        
     private:
        float   m_fSize;
    };

};  // namespace LNF


#endif  // #ifndef LIBS_HEADER_MARCHED_NODE_H

