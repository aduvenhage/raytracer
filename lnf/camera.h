#ifndef LIBS_HEADER_CAMERA_H
#define LIBS_HEADER_CAMERA_H

#include "constants.h"
#include "vec3.h"


namespace LNF
{
    /*
        Camera base class.
     */
    class Camera
    {
     public:
        virtual ~Camera() = default;

        // returns the camera position
        virtual Vec origin() const = 0;

        // returns camera view axis
        virtual const Axis &axis() const = 0;
        
        // returns FOV
        virtual float fov() const = 0;

        // returns camera focus distance
        virtual float focusDistance() const = 0;

        // returns camera aperture size
        virtual float aperture() const = 0;
    };


};  // namespace LNF

#endif  // #ifndef LIBS_HEADER_CAMERA_H

