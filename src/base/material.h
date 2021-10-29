#ifndef BASE_MATERIAL_H
#define BASE_MATERIAL_H

#include "core/color.h"
#include "core/scattered_ray.h"
#include "intersect.h"
#include "resource.h"

#include <vector>
#include <memory>


namespace BASE
{
    /* Material/texture base class */
    class Material      : public Resource
    {
     public:
        /* Returns the scattered ray at the intersection point. */
        virtual CORE::ScatteredRay &scatter(CORE::ScatteredRay &_sc, const Intersect &_hit) const = 0;
    };
    
    
    /* Multi material (combines other material outputs) */
    class MultiMaterial : public Material
    {
     public:
        MultiMaterial() noexcept = default;
        MultiMaterial(std::unique_ptr<Material> &&_material) {
            m_materials.push_back(std::move(_material));
        }
        
        void addMaterial(std::unique_ptr<Material> &&_material) {
            m_materials.push_back(std::move(_material));
        }
        
        /* Returns the scattered ray at the intersection point. */
        virtual CORE::ScatteredRay &scatter(CORE::ScatteredRay &_sc, const Intersect &_hit) const {
            for (const auto &pMat : m_materials) {
                _sc = pMat->scatter(_sc, _hit);
            }
            
            return _sc;
        }
        
     protected:
        std::vector<std::unique_ptr<Material>>      m_materials;
    };

};  // namespace BASE


#endif  // #ifndef BASE_MATERIAL_H

