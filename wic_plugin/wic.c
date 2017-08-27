///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//    MIT License
//
//    Copyright(c) 2017 René Slijkhuis
//
//    Permission is hereby granted, free of charge, to any person obtaining a copy
//    of this software and associated documentation files (the "Software"), to deal
//    in the Software without restriction, including without limitation the rights
//    to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
//    copies of the Software, and to permit persons to whom the Software is
//    furnished to do so, subject to the following conditions:
//
//    The above copyright notice and this permission notice shall be included in all
//    copies or substantial portions of the Software.
//
//    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
//    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
//    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
//    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
//    SOFTWARE.
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "wic.h"

#include <libgimp/gimp.h>
#include <windows.h>

#include "WIC_Decoder.h"

typedef unsigned int UINT;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int read_wic( const char* filename )
{
    UINT frameCount = 1;
    UINT frameWidth = 0;
    UINT frameHeight = 0;
    UINT frameBufferSize = 0;
    void* frameBuffer = NULL;
    GimpPixelRgn region;    
    int imageId = -1;
    UINT imageWidth = 0;
    UINT imageHeight = 0;
    UINT addedLayers = 0;    

    for ( UINT frameIndex = 0 ; frameIndex < frameCount ; frameIndex++ )
    {
        if ( GetImageData( filename, frameIndex, &frameCount, &frameWidth, &frameHeight, &frameBuffer, &frameBufferSize ) != 0 )
        {
            return -1;
        }

        if ( frameIndex == 0 )
        {
            // Create the RGB image.
            imageId = gimp_image_new( frameWidth, frameHeight, GIMP_RGB );
            imageWidth = frameWidth;
            imageHeight = frameHeight;
        }

        if ( ( imageWidth == frameWidth ) && ( imageHeight = frameHeight ) )
        {
            // Create the new layer
            char layerName[32];
            sprintf_s( layerName, 32, "Frame %d\0", addedLayers + 1 );        
            int layerId = gimp_layer_new( imageId, layerName, frameWidth, frameHeight, GIMP_RGB_IMAGE, 100, GIMP_NORMAL_MODE );

            // Get the drawable for the layer
            GimpDrawable* pDrawable = gimp_drawable_get( layerId );

            // Get a pixel region from the layer
            gimp_pixel_rgn_init( &region, pDrawable, 0, 0, frameWidth, frameHeight, TRUE, FALSE );

            // Now FINALLY set the pixel data
            gimp_pixel_rgn_set_rect( &region, frameBuffer, 0, 0, frameWidth, frameHeight );
            free( frameBuffer );

            // We're done with the drawable
            gimp_drawable_flush( pDrawable );
            gimp_drawable_detach( pDrawable );

            // Add the layer to the image
            gimp_image_insert_layer( imageId, layerId, 0, addedLayers );

            addedLayers++;
        }
    }

    // Set the filename
    gimp_image_set_filename( imageId, filename );

    return imageId;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

char* supported_extensions( )
{
    return GetSupportedExtensions( );
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int display_name( char* name, const int buffer_size )
{
    return GetDisplayName( name, buffer_size );
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////