#pragma once

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#include <filesystem>


namespace CORE
{
    // image data wrapper
    struct Image
    {
        int             m_iBytesPerPixel = 0;   // RGB = 3
        int             m_iWidth = 0;
        int             m_iHeight = 0;
        int             m_iQuality = 0;     // [0..100]
        unsigned char   *m_pData = nullptr;
        std::string     m_strError;
    };


    // load a image file
    Image loadImageFile(const char *_pszFilePath) {
        // TODO: report on file and other errors
        Image image;
        image.m_pData = stbi_load(_pszFilePath, &image.m_iWidth, &image.m_iHeight, &image.m_iBytesPerPixel, 3);
        return image;
    }


    // save a image file
    bool saveImageFile(const char *_pszFilePath, Image &_image) {
        // TODO: report on file and other errors
        if (_image.m_iQuality == 0) {
            _image.m_iQuality = 100;
        }

        auto path = std::filesystem::path(_pszFilePath);  // TODO: convert to lower case
        if (auto ext = path.extension(); (ext == ".jpg") || (ext == ".jpeg")) {
            stbi_write_jpg(_pszFilePath, _image.m_iWidth, _image.m_iHeight, _image.m_iBytesPerPixel, _image.m_pData, _image.m_iQuality);
        }
        else if (ext == ".png") {
            stbi_write_png(_pszFilePath, _image.m_iWidth, _image.m_iHeight, _image.m_iBytesPerPixel, _image.m_pData, _image.m_iWidth * _image.m_iBytesPerPixel);
        }
        else if (ext == ".bmp") {
            stbi_write_bmp(_pszFilePath, _image.m_iWidth, _image.m_iHeight, _image.m_iBytesPerPixel, _image.m_pData);
        }
        else if (ext == ".tga") {
            stbi_write_tga(_pszFilePath, _image.m_iWidth, _image.m_iHeight, _image.m_iBytesPerPixel, _image.m_pData);
        }
        

        return false;
    }

};


