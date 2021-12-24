#pragma once

#include "core/constants.h"
#include "core/vec3.h"
#include "resource.h"


namespace BASE
{
    /*
        Camera base class.
     */
    class Camera    : public Resource
    {
     public:
        // returns the camera position
        virtual const CORE::Vec &origin() const = 0;

        // returns camera view axis
        virtual const CORE::Axis &axis() const = 0;
        
        // returns the camera forward (z) axis
        virtual const CORE::Vec &z() const = 0;
        
        // returns the camera up (y) axis
        virtual const CORE::Vec &y() const = 0;
        
        // returns the camera left (x) axis
        virtual const CORE::Vec &x() const = 0;
        
        // returns FOV
        virtual float fov() const = 0;

        // returns camera focus distance
        virtual float focusDistance() const = 0;

        // returns camera aperture size
        virtual float aperture() const = 0;
    };


};  // namespace BASE

