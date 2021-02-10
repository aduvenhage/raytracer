#ifndef LIBS_HEADER_MARCHED_NODE_H
#define LIBS_HEADER_MARCHED_NODE_H

#include "constants.h"
#include "primitive.h"
#include "signed_distance_functions.h"
#include "trace.h"
#include "uv.h"
#include "vec3.h"


namespace LNF
{
    /* Axis aligned box shape class -- fixed at origin [0, 0, 0]; base class for raymarched nodes */
    class MarchedBox        : public Primitive
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
        virtual bool hit(Intersect &_hit, RandomGen &) const override {
            auto bi = aaboxIntersect(m_bounds, _hit.m_ray.m_origin, _hit.m_ray.m_invDirection);
            if (bi.m_intersect == true) {
                // try to hit surface inside (using raymarching)
                bool is_hit = check_marched_hit(_hit,
                                                m_iMaxSamples,
                                                bi.m_tmax,
                                                [this](const Vec &_p){return this->sdfSurface(_p);});

                if ( (is_hit == true) &&
                     ((_hit.m_fPositionOnRay >= _hit.m_ray.m_fMinDist) && (_hit.m_fPositionOnRay <= _hit.m_ray.m_fMaxDist)) )
                {
                    return true;
                }
            }
            
            return false;
        }
        
        /* Completes the node intersect properties. */
        virtual Intersect &intersect(Intersect &_hit) const override {
            _hit.m_normal = surfaceNormal(_hit.m_position);
            _hit.m_uv = surfaceUv(_hit.m_position);
            return _hit;
        }
                
        /* returns bounds for shape */
        virtual const Bounds &bounds() const override {
            return  m_bounds;
        }
        
     protected:
        // surface signed distance function
        virtual float sdfSurface(const Vec &_p) const = 0;
        
        // get normal from surface function
        virtual Vec surfaceNormal(const Vec &_p) const {
            return LNF::surfaceNormal(_p, [this](const Vec &_x){return this->sdfSurface(_x);});
        }
            
        // calc surface UV
        virtual Uv surfaceUv(const Vec &_p) const {
            return getSphericalUv(_p, _p.size());
        }

     private:
        Axis                   m_axis;
        Bounds                 m_bounds;
        const Material         *m_pMaterial;
        int                    m_iMaxSamples;
    };


    // simple marched sphere
    class MarchedSphere        : public MarchedBox
    {
     public:
        MarchedSphere(const Vec &_size, const Material *_pMaterial, float _fWaveRatio, int _iMaxSamples)
            :MarchedBox(_size, _pMaterial, _iMaxSamples),
             m_fSize(_size.size() * 0.5),
             m_fWaveRatio(_fWaveRatio)
        {}

        MarchedSphere(float _fSize, const Material *_pMaterial, float _fWaveRatio, int _iMaxSamples)
            :MarchedBox(_fSize, _pMaterial, _iMaxSamples),
             m_fSize(_fSize * 0.5),
             m_fWaveRatio(_fWaveRatio)
        {}

     protected:
        // surface signed distance function
        virtual float sdfSurface(const Vec &_p) const override {
            return sdfSphereDeformed(_p, m_fSize * (1 - m_fWaveRatio), m_fSize * m_fWaveRatio);
        }

     private:
        float   m_fSize;
        float   m_fWaveRatio;
    };

};  // namespace LNF


#endif  // #ifndef LIBS_HEADER_MARCHED_NODE_H

