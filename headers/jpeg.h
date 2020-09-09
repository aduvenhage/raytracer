#ifndef LIBS_HEADER_JPEG_H
#define LIBS_HEADER_JPEG_H

#include "constants.h"

#include <stdlib.h>
#include <stdio.h>
#include <jpeglib.h>


namespace LNF
{
    /*
     Write out JPEG file to disk.
     _pImageData: raw RGB (8:8:8) image data.
     */
    int writeJpegFile(const char *_pszFilename, int _iWidth, int _iHeight, const unsigned char *_pImageData, int _iQuality)
    {
        struct jpeg_compress_struct cinfo;
        struct jpeg_error_mgr jerr;
        
        cinfo.err = jpeg_std_error(&jerr);
        jpeg_create_compress(&cinfo);
        FILE *outfile;
        
        if ((outfile = fopen(_pszFilename, "wb")) == NULL)
        {
            fprintf(stderr, "ERROR: can't open %s\n", _pszFilename);
            exit(1);
        }
        
        jpeg_stdio_dest(&cinfo, outfile);
        
        cinfo.image_width  = _iWidth;           // Image width and height in pixels.
        cinfo.image_height = _iHeight;
        cinfo.input_components = 3;         // Number of color components per pixel.
        cinfo.in_color_space = JCS_RGB;     // Colorspace of input image as RGB.
        
        jpeg_set_defaults(&cinfo);
        jpeg_set_quality(&cinfo, _iQuality, TRUE);
        
        jpeg_start_compress(&cinfo, TRUE);
        int row_stride = cinfo.image_width * 3;        // JSAMPLEs per row in image_buffer
        JSAMPROW row_pointer[1];
        
        while (cinfo.next_scanline < cinfo.image_height)
        {
          row_pointer[0] = (unsigned char*)&_pImageData[cinfo.next_scanline * row_stride];
          jpeg_write_scanlines(&cinfo, row_pointer, 1);
        }
        
        jpeg_finish_compress(&cinfo);
        jpeg_destroy_compress(&cinfo);
        fclose(outfile);

        return 0;
    }

};  // namespace LNF


#endif  // #ifndef LIBS_HEADER_JPEG_H
