#ifndef LIBS_HEADER_MATERIAL_H
#define LIBS_HEADER_MATERIAL_H

#include "color.h"
#include "uv.h"


namespace LNF
{
    /* Diffuse material/texture base class */
    class Material
    {
     public:
        virtual ~Material() = default;
        
        /* Returns the diffuse color at the given surface position */
        virtual Color color(const Uv &_uv) const = 0;
        
        /* Returns material property [0..1] = [diffuse .. mirror] */
        virtual double reflection() = 0;
        
        /* Returns material property [0..1 = [opaque .. clear] */
        virtual double transparancy() = 0;
        
        /* Returns material property */
        virtual double indexOfRefraction() = 0;

    };

};  // namespace LNF


#endif  // #ifndef LIBS_HEADER_MATERIAL_H

