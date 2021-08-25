#ifndef BASE_CAMERA_H
#define BASE_CAMERA_H

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
        virtual ~Camera() = default;

        // returns the camera position
        virtual CORE::Vec origin() const = 0;

        // returns camera view axis
        virtual const CORE::Axis &axis() const = 0;
        
        // returns FOV
        virtual float fov() const = 0;

        // returns camera focus distance
        virtual float focusDistance() const = 0;

        // returns camera aperture size
        virtual float aperture() const = 0;
    };


};  // namespace BASE

#endif  // #ifndef BASE_CAMERA_H

