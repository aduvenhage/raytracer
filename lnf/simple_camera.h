#ifndef LIBS_HEADER_SIMPLE_CAMERA_H
#define LIBS_HEADER_SIMPLE_CAMERA_H


#include "camera.h"
#include "constants.h"
#include "vec3.h"


namespace LNF
{
    /*
        Simple camera with origin, up and lookat point.
     */
    class SimpleCamera  : public Camera
    {
     public:
        SimpleCamera(const Vec &_origin, const Vec &_up, const Vec &_lookat, float _fFov, float _fAperture, float _fFocusDist)
            :m_axis(axisLookat(_lookat, _origin, _up)),
             m_fFov(_fFov),
             m_fAperture(_fAperture),
             m_fFocusDist(_fFocusDist)
        {}
        
        // returns the camera position
        virtual Vec origin() const override {
            return m_axis.m_origin;
        }
        
        // returns camera view axis
        virtual const Axis &axis() const override {
            return m_axis;
        }
        
        // returns camera focus distance
        virtual float focusDistance() const override {
            return m_fFocusDist;
        }
        
        // returns camera aperture size
        virtual float aperture() const override {
            return m_fAperture;
        }

        // returns FOV
        virtual float fov() const override {
            return m_fFov;
        }

     private:
        Axis    m_axis;
        float   m_fFov;
        float   m_fAperture;
        float   m_fFocusDist;
    };
    

};  // namespace LNF

#endif  // #ifndef LIBS_HEADER_SIMPLE_CAMERA_H

