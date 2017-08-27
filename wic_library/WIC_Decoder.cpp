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
#include "Utilities/ConfigFile.h"

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

    ConfigFile config;
    istringstream stream( ToString( ext ) );
    string line;   

    while ( getline( stream, line, ',' ) )
    {
        wstring extension = ToWideString( ToLower( line ).c_str( ) );
                       
        if ( ( static_cast<UINT>( extension.length( ) ) > 1 ) &&
             ( extension[ 0 ] == '.' ) )
        {          
            wstring ext = extension.erase( 0, 1 );
            if ( !config.IsExtensionExcluded( ext ) )
            {
                extensions.push_back( ext ); // Add extension without leading dot.
            }
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
    memset( pBuffer, NULL, sizeof(char) * ( size + 1 ) );
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
    HRESULT hr = S_OK;
    
    CoInitialize( nullptr );
    {
        CComPtr<IWICImagingFactory> pFactory;
        CComPtr<IWICBitmapDecoder> pDecoder;
        CComPtr<IWICBitmapFrameDecode> pFrame;
        CComPtr<IWICFormatConverter> pWicFormatConverter;

        if ( hr == S_OK )
        {
            hr = CoCreateInstance( CLSID_WICImagingFactory, NULL, CLSCTX_INPROC_SERVER, IID_IWICImagingFactory, (LPVOID*)&pFactory );
            if ( hr != S_OK ) OutputDebugString( L"[WIC_Plugin] CoCreateInstance( CLSID_WICImagingFactor ) Failed!" );
        }

        if ( hr == S_OK )
        {
            hr = pFactory->CreateDecoderFromFilename( ToWideString( filename ).c_str( ), NULL, GENERIC_READ, WICDecodeMetadataCacheOnDemand, &pDecoder );
            if ( hr != S_OK ) OutputDebugString( L"[WIC_Plugin] IWICImagingFactory->CreateDecoderFromFilename( ) Failed!" );
        }

        if ( hr == S_OK )
        {
            hr = pDecoder->GetFrameCount( frameCount );
            if ( hr != S_OK ) OutputDebugString( L"[WIC_Plugin] IWICBitmapDecoder->GetFrameCount( ) Failed!" );
        }

        if ( hr == S_OK )
        {
            hr = pDecoder->GetFrame( frameIndex, &pFrame );
            if ( hr != S_OK ) OutputDebugString( L"[WIC_Plugin] IWICBitmapDecoder->GetFrame( ) Failed!" );
        }

        if ( hr == S_OK )
        {
            hr = pFrame->GetSize( frameWidth, frameHeight );
            if ( hr != S_OK ) OutputDebugString( L"[WIC_Plugin] IWICBitmapFrameDecode->GetSize( ) Failed!" );
        }

        if ( hr == S_OK )
        {
            hr = pFactory->CreateFormatConverter( &pWicFormatConverter );
            if ( hr != S_OK ) OutputDebugString( L"[WIC_Plugin] IWICImagingFactory->CreateFormatConverter( ) Failed!" );
        }
 
        if ( hr == S_OK )
        {
            hr = pWicFormatConverter->Initialize( pFrame,                       // Input bitmap to convert
                                                  GUID_WICPixelFormat24bppRGB,  // Destination pixel format
                                                  WICBitmapDitherTypeNone,      // Specified dither pattern
                                                  nullptr,                      // Specify a particular palette 
                                                  0.f,                          // Alpha threshold
                                                  WICBitmapPaletteTypeCustom ); // Palette translation type
            if ( hr != S_OK ) OutputDebugString( L"[WIC_Plugin] IWICFormatConverter->Initialize( ) Failed!" );
        }

        if ( hr == S_OK )
        {
            const UINT bytesPerPixel = 3;
            UINT stride = *frameWidth * bytesPerPixel;
            *frameBufferSize = *frameWidth * *frameHeight * bytesPerPixel;

            *frameBuffer = malloc( *frameBufferSize );
            hr = pWicFormatConverter->CopyPixels( NULL, stride, *frameBufferSize, static_cast<BYTE*>( *frameBuffer ) );
            if ( hr != S_OK ) OutputDebugString( L"[WIC_Plugin] IWICFormatConverter->CopyPixels( ) Failed!" );
        }

        // Note: the WIC COM pointers should be released before 'CoUninitialize( )' is called.
    }
    CoUninitialize( );

    return ( hr == S_OK ) ? 0 : -1;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int GetDisplayName( char* name, const int bufferSize )
{
    if ( name == nullptr ) return -1;
    
    ConfigFile config;
    wstring displayName;
    if ( config.GetDisplayName( displayName ) )
    {
        if ( displayName.length( ) >= bufferSize )
        {
            return -1;
        }
        
        sprintf_s( name, bufferSize, "%s\0", ToString( displayName ).c_str( ) );
        return 0;
    }

    return -1;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////