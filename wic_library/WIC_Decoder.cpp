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

#include "stdafx.h"
#include "pch.h"

#include <algorithm>
#include <iostream>
#include <iomanip>
#include <iterator>
#include <sstream>
#include <string>
#include <vector>

#include <atlbase.h>
#include <wincodec.h>

#include "WIC_Decoder.h"
#include "Utilities/StringUtilities.h"
#include "Utilities/WicUtilities.h"

using namespace std;
using namespace Gimp::Plugin::Utilities;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

char* GetSupportedExtensions( )
{
    Wic wic;
    wstring ext;
    vector<wstring> extensions;    

    UINT count = wic.GetNumberOfCodecs( );
    for ( UINT i = 0 ; i < count ; i++ )
    {
        CodecInfo codecInfo;
        if ( wic.GetCodecInfo( i, codecInfo ) )
        {
            ext = ext + L"," + codecInfo.extensions;
        }
    }

    istringstream stream( ToString( ext ) );
    string line;   

    while ( getline( stream, line, ',' ) )
    {
        wstring extension = ToWideString( ToLower( line ).c_str( ) );
                       
        if ( ( static_cast<UINT>( extension.length( ) ) > 1 ) &&
                ( extension[ 0 ] == '.' ) )
        {          
            extensions.push_back( extension.erase( 0, 1 ) ); // Add extension without leading dot.
        }
    }
 
    sort( extensions.begin( ), extensions.end( ) );
 
    RemoveDuplicates( extensions );

    string extensionList;
    count = static_cast<UINT>( extensions.size( ) );
    for ( UINT i = 0 ; i < count ; i++ )
    {
        extensionList = extensionList + ToString( extensions[ i ] );
        if ( i < count - 1 ) extensionList = extensionList + ",";
    }

    int size = static_cast<int>( extensionList.size( ) );
    char* pBuffer = static_cast<char*>( malloc( sizeof(char) * ( size + 1 ) ) );
    memcpy( pBuffer, extensionList.c_str( ), size );

    return pBuffer;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int GetImageData( const char* filename,
                  UINT frameIndex,
                  UINT* frameCount,
                  UINT* frameWidth,
                  UINT* frameHeight,
                  void** frameBuffer,
                  UINT* frameBufferSize )
{
    int success = -1;
    
    CoInitialize( nullptr );
    {
        CComPtr<IWICImagingFactory> pFactory;
        CComPtr<IWICBitmapDecoder> pDecoder;
        CComPtr<IWICBitmapFrameDecode> pFrame;
        CComPtr<IWICFormatConverter> pWicFormatConverter;

        if ( CoCreateInstance( CLSID_WICImagingFactory, NULL, CLSCTX_INPROC_SERVER, IID_IWICImagingFactory, (LPVOID*)&pFactory ) == S_OK )
        {
            if ( pFactory->CreateDecoderFromFilename( ToWideString( filename ).c_str( ), NULL, GENERIC_READ, WICDecodeMetadataCacheOnDemand, &pDecoder ) == S_OK )
            {
                if ( pDecoder->GetFrameCount( frameCount ) == S_OK )
                {                
                    if ( pDecoder->GetFrame( frameIndex, &pFrame ) == S_OK )
                    {
                        if ( pFrame->GetSize( frameWidth, frameHeight ) == S_OK )
                        {
                            if ( pFactory->CreateFormatConverter( &pWicFormatConverter ) == S_OK )
                            {
                                if ( pWicFormatConverter->Initialize( pFrame,                                   // Input bitmap to convert
                                                                      GUID_WICPixelFormat24bppRGB,              // Destination pixel format
                                                                      WICBitmapDitherTypeNone,                  // Specified dither pattern
                                                                      nullptr,                                  // Specify a particular palette 
                                                                      0.f,                                      // Alpha threshold
                                                                      WICBitmapPaletteTypeCustom ) == S_OK )    // Palette translation type
                                {
                                    const UINT bytesPerPixel = 3;
                                    UINT stride = *frameWidth * bytesPerPixel;
                                    *frameBufferSize = *frameWidth * *frameHeight * bytesPerPixel;
        
                                    *frameBuffer = malloc( *frameBufferSize );
                                    if ( pWicFormatConverter->CopyPixels( NULL, stride, *frameBufferSize, static_cast<BYTE*>( *frameBuffer ) ) == S_OK )
                                    {
                                        success = 0;
                                    }
                                    else
                                    {
                                        OutputDebugString( L"[WIC_Plugin] IWICFormatConverter->CopyPixels( ) Failed!" );
                                    }
                                }
                                else
                                {
                                    OutputDebugString( L"[WIC_Plugin] IWICFormatConverter->Initialize( ) Failed!" );
                                }
                            }
                            else
                            {
                                OutputDebugString( L"[WIC_Plugin] IWICImagingFactory->CreateFormatConverter( ) Failed!" );
                            }
                        }
                        else
                        {
                            OutputDebugString( L"[WIC_Plugin] IWICBitmapFrameDecode->GetSize( ) Failed!" );
                        }
                    }
                    else
                    {
                        OutputDebugString( L"[WIC_Plugin] IWICBitmapDecoder->GetFrame( ) Failed!" );
                    }
                }
                else
                {
                    OutputDebugString( L"[WIC_Plugin] IWICBitmapDecoder->GetFrameCount( ) Failed!" );
                }
            }
            else
            {
                OutputDebugString( L"[WIC_Plugin] IWICImagingFactory->CreateDecoderFromFilename( ) Failed!" );
            }
        }
        else
        {
            OutputDebugString( L"[WIC_Plugin] CoCreateInstance( CLSID_WICImagingFactor ) Failed!" );
        }

        // Note: the WIC COM pointers should be released before 'CoUninitialize( )' is called.
    }
    CoUninitialize( );

    return success;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////