#ifndef LIBS_HEADER_CAMERA_H
#define LIBS_HEADER_CAMERA_H

#include "constants.h"
#include "vec3.h"


namespace LNF
{
    /*
     Moveable camera
     */
    struct Camera
    {
        Camera()
        {}
        
        Camera(const Vec &_origin, const Vec &_up, const Vec &_lookat)
            :m_origin(_origin),
             m_camera(axisLookat(_lookat, _origin, _up))
        {}
        
        Camera(const Camera &) = default;
        Camera(Camera &&) = default;
        Camera(Camera &) = default;

        Camera &operator=(const Camera &) = default;
        Camera &operator=(Camera &&) = default;
        
        // rotate from camera/view to world coordinates
        Vec camera2world(const Vec &_view) {
            return m_camera.translateFrom(_view);
        }
        
        Vec     m_origin;
        Axis    m_camera;       // [x=left, y=up, z=lookat]
    };


};  // namespace LNF

#endif  // #ifndef LIBS_HEADER_COLOR_H
