#ifndef LIBS_HEADER_OUTPUT_IMAGE_H
#define LIBS_HEADER_OUTPUT_IMAGE_H

#include "constants.h"


namespace LNF
{
    /* Wrapper class for output image raw buffer (RGB 8:8:8) */
    class OutputImageBuffer
    {
     protected:
        const int BYTES_PER_PIXEL = 3;
        
     public:
        OutputImageBuffer(int _iWidth, int _iHeight)
            :m_iWidth(_iWidth),
             m_iHeight(_iHeight),
             m_image(_iWidth * _iHeight * BYTES_PER_PIXEL)
        {}
        
        int bytesPerPixel() const {return BYTES_PER_PIXEL;}
        int width() const {return m_iWidth;}
        int height() const {return m_iHeight;}

        size_t size() const {
            return BYTES_PER_PIXEL * m_iWidth * m_iHeight;
        }
        
        /* returns pointer to start of image data */
        unsigned char *data() {return m_image.data();}
        
        /* returns pointer to start of image data */
        const unsigned char *data() const {return m_image.data();}
        
        /* returns pointer to start of given row */
        unsigned char *row(int _iY) {
            return m_image.data() + BYTES_PER_PIXEL * m_iWidth * _iY;
        }
        
        /* returns pointer to start of given row */
        const unsigned char *row(int _iY) const {
            return m_image.data() + BYTES_PER_PIXEL * m_iWidth * _iY;
        }

     private:
        const int                   m_iWidth;
        const int                   m_iHeight;
        std::vector<unsigned char>  m_image;
    };

};  // namespace LNF


#endif  // #ifndef LIBS_HEADER_OUTPUT_IMAGE_H

