#ifndef UTILS_MANDLEBROT_H
#define UTILS_MANDLEBROT_H

#include <vector>


namespace UTILS
{
    /* Returns mandlebrot escape iteration count or 0 if it reached max iterations. */
    inline int mandlebrot(double _fCx, double _fCy, unsigned int _uMaxIterations)
    {
        double fZx = 0;
        double fZy = 0;
        unsigned int i = 0;

        while (i < _uMaxIterations)
        {
            // the 'z = z*z + c' and '|z|' calculations are combined to speed things up
            double fZxx = fZx * fZx;
            double fZyy = fZy * fZy;

            fZy = 2 * fZx * fZy + _fCy;
            fZx = fZxx - fZyy + _fCx;

            if ((fZxx + fZyy) >= 4) {
                return i;
            }

            i++;
        }

        return 0;
    }
    
    /*
        Mandlebrot fractal rendering.
        Use `setView(...) to set position and zoom level.
        Use `render(...)` and `writeToJpeg(...)` to render fractal and write to disk.
        For manually rendering fractal using normalised coordinates (x, y = [0..1, 0..1]),
        create object with `width,height = 1, 1` and then use `value(...)` to find iteration count.
     */
    class MandleBrot
    {
     public:
        /* _iWidth and _iHeight is size of output image. See `renderToJpeg`. */
        MandleBrot(int _iWidth, int _iHeight)
            :m_iWidth(_iWidth),
             m_iHeight(_iHeight),
             m_iMaxIterations(0),
             m_fPosX(0),
             m_fPosY(0),
             m_fZoom(0),
             m_fScale(0)
        {
            // default view
            setView(0.0f, 0.0f, 0.2f, 50);
        }
        
        int width() const {return m_iWidth;}
        int height() const {return m_iHeight;}
        int bytesPerPixel() const {return 3;}
        int max_iterations() const {return m_iMaxIterations;}

        /* Set position and zoom level, using Mandlebrot values. */
        void setView(double _fCx, double _fCy, double _fZoom, int _iMaxIterations)
        {
            m_fZoom = _fZoom;
            if (m_iWidth < m_iHeight) {
                m_fScale = 1.0 / m_iWidth / m_fZoom;
            }
            else {
                m_fScale = 1.0 / m_iHeight / m_fZoom;
            }
            
            m_iMaxIterations = _iMaxIterations;
            m_fPosX = _fCx;
            m_fPosY = _fCy;
        }
        
        /* Returns mandlebrot escape iteration count or 0 if it reached max iterations.
           _fPixelX and _fPixelY are the image pixel positions, but allows for special case
           where image size is 1 x 1 (i.e. pixel coordinates normalised [0..1]).
         */
        int value(double _fPixelX, double _fPixelY) const
        {
            return mandlebrot(_fPixelX * m_fScale + m_fPosX,
                              _fPixelY * m_fScale + m_fPosY,
                              m_iMaxIterations);
        }
         
     private:
        int                         m_iWidth;
        int                         m_iHeight;
        int                         m_iMaxIterations;
        double                      m_fPosX;
        double                      m_fPosY;
        double                      m_fZoom;
        double                      m_fScale;
    };


};  // namespace UTILS

#endif  // #ifndef UTILS_MANDLEBROT_H

