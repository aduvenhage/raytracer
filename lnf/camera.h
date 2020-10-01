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

        // rotate from camera/view to world coordinates
        virtual Vec rotateFrom(const Vec &_vec) const = 0;

        // transform from camera/view to world coordinates
        virtual Vec transformFrom(const Vec &_vec) const = 0;

        // returns camera focus distance
        virtual float focusDistance() const = 0;

        // returns camera aperture size
        virtual float aperture() const = 0;
    };


    /*
        Simple camera with origin, up and lookat point.
     */
    class SimpleCamera  : public Camera
    {
     public:
        SimpleCamera(const Vec &_origin, const Vec &_up, const Vec &_lookat, float _fAperture, float _fFocusDist)
            :m_axis(axisLookat(_lookat, _origin, _up)),
             m_fAperture(_fAperture),
             m_fFocusDist(_fFocusDist)
        {}
        
        // returns the camera position
        virtual Vec origin() const override {
            return m_axis.m_origin;
        }
        
        // rotate from camera/view to world coordinates
        virtual Vec rotateFrom(const Vec &_vec) const override {
            return m_axis.rotateFrom(_vec);
        }

        // transform from camera/view to world coordinates
        virtual Vec transformFrom(const Vec &_vec) const override {
            return m_axis.transformFrom(_vec);
        }

        // returns camera focus distance
        virtual float focusDistance() const override {
            return m_fFocusDist;
        }
        
        // returns camera aperture size
        virtual float aperture() const override {
            return m_fAperture;
        }
        
     protected:
        Axis    m_axis;
        float   m_fAperture;
        float   m_fFocusDist;
    };
    
    
    /*
        Camera positioned
     */


};  // namespace LNF

#endif  // #ifndef LIBS_HEADER_COLOR_H

