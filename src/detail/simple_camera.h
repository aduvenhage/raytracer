#ifndef DETAIL_SIMPLE_CAMERA_H
#define DETAIL_SIMPLE_CAMERA_H


#include "base/camera.h"
#include "core/constants.h"
#include "core/vec3.h"


namespace DETAIL
{
    /*
        Simple camera with origin, up and lookat point.
     */
    class SimpleCamera  : public BASE::Camera
    {
     public:
        SimpleCamera(const CORE::Vec &_origin, const CORE::Vec &_up, const CORE::Vec &_lookat, float _fFov, float _fAperture, float _fFocusDist)
            :m_axis(axisLookat(_lookat, _origin, _up)),
             m_fFov(_fFov),
             m_fAperture(_fAperture),
             m_fFocusDist(_fFocusDist)
        {}
        
        // returns the camera position
        virtual const CORE::Vec &origin() const override {
            return m_axis.m_origin;
        }
        
        // returns the camera forward (z) axis
        virtual const CORE::Vec &z() const override {
            return m_axis.m_z;
        }
        
        // returns the camera up (y) axis
        virtual const CORE::Vec &y() const override {
            return m_axis.m_y;
        }
        
        // returns the camera left (x) axis
        virtual const CORE::Vec &x() const override {
            return m_axis.m_x;
        }

        // returns camera view axis
        virtual const CORE::Axis &axis() const override {
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
        CORE::Axis  m_axis;
        float       m_fFov;
        float       m_fAperture;
        float       m_fFocusDist;
    };
    

};  // namespace DETAIL

#endif  // #ifndef DETAIL_SIMPLE_CAMERA_H

