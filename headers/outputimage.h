#ifndef LIBS_HEADER_OUTPUT_IMAGE_H
#define LIBS_HEADER_OUTPUT_IMAGE_H

#include "constants.h"


namespace LNF
{
    /* Output image base class */
    class OutputImage
    {
     public:
        virtual ~OutputImage() = default;

        virtual int bytesPerPixel() const = 0;
        virtual int width() const = 0;
        virtual int height() const = 0;
        virtual unsigned char *data() = 0;
        virtual const unsigned char *data() const = 0;
        virtual unsigned char *row(int _iY) = 0;
        virtual const unsigned char *row(int _iY) const = 0;
    };


    /* Wrapper class for output image raw buffer (RGB 8:8:8) */
    class OutputImageBuffer     : public OutputImage
    {
     protected:
        const int BYTES_PER_PIXEL = 3;
        
     public:
        OutputImageBuffer(int _iWidth, int _iHeight)
            :m_iWidth(_iWidth),
             m_iHeight(_iHeight),
             m_image(_iWidth * _iHeight * BYTES_PER_PIXEL)
        {}
        
        virtual int bytesPerPixel() const override {return BYTES_PER_PIXEL;}
        virtual int width() const override {return m_iWidth;}
        virtual int height() const override {return m_iHeight;}
        
        virtual unsigned char *data() override {return m_image.data();}
        virtual const unsigned char *data() const override {return m_image.data();}
        
        virtual unsigned char *row(int _iY) override {
            return m_image.data() + BYTES_PER_PIXEL * m_iWidth * _iY;
        }
        
        virtual const unsigned char *row(int _iY) const override {
            return m_image.data() + BYTES_PER_PIXEL * m_iWidth * _iY;
        }

     private:
        const int                   m_iWidth;
        const int                   m_iHeight;
        std::vector<unsigned char>  m_image;
    };


    /* Sub-image reference for a larger base image */
    class OutputImageBlock  : public OutputImage
    {
     public:
        OutputImageBlock(OutputImageBuffer &_buffer, int _iX, int _iY, int _iWidth, int _iHeight)
            :m_buffer(_buffer),
             m_iX(_iX),
             m_iY(_iY),
             m_iWidth(_iWidth),
             m_iHeight(_iHeight)
        {}
        
        virtual int bytesPerPixel() const override {return m_buffer.bytesPerPixel();}
        virtual int width() const override {return m_iWidth;}
        virtual int height() const override {return m_iHeight;}
        virtual unsigned char *data() override {return row(0);}
        virtual const unsigned char *data() const override {return row(0);}
        
        virtual unsigned char *row(int _iY) override {
            return m_buffer.row(_iY + m_iY) + bytesPerPixel() * m_iX;
        }
         
        virtual const unsigned char *row(int _iY) const override {
            return m_buffer.row(_iY + m_iY) + bytesPerPixel() * m_iX;
        }
        
     protected:
        OutputImageBuffer       &m_buffer;
        const int               m_iX;
        const int               m_iY;
        const int               m_iWidth;
        const int               m_iHeight;
    };


};  // namespace LNF


#endif  // #ifndef LIBS_HEADER_OUTPUT_IMAGE_H

