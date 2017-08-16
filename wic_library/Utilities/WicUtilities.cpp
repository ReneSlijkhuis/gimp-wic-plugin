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

#include <atlbase.h>
#include <d2d1.h>
#include <dwrite.h>
#include <string>
#include <wincodec.h>

#include "WicUtilities.h"

using namespace std;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace Gimp {
namespace Plugin {
namespace Utilities {

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Wic::Wic( )
{
    Initialize( );
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Wic::~Wic( )
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

UINT Wic::GetNumberOfCodecs( ) const
{
    return static_cast<UINT>( m_codecs.size( ) );
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool Wic::GetCodecInfo( const UINT index, CodecInfo& codecInfo ) const
{
    if ( index >= GetNumberOfCodecs( ) ) return false;
    
    codecInfo = m_codecs[ index ];
    return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool Wic::Initialize( )
{
    const wstring nameKeyCodecs = L"CLSID\\{7ED96837-96F0-4812-B211-F13C24117ED3}\\Instance";
    
    m_codecs.clear( );

    // Add the built-in Microsoft Windows imaging codec
    CodecInfo codec;
    codec.name = L"Microsoft Windows Native Imaging Codec";
    codec.classId = L"";
    codec.extensions = L".jpg,.jpeg,.tif,.tiff,.gif,.png,.bmp";
    m_codecs.push_back( codec );

    HKEY hKeyWicCodecs;
    
    if ( RegOpenKeyEx( HKEY_CLASSES_ROOT,
                       nameKeyCodecs.c_str( ),
                       NULL,
                       KEY_ENUMERATE_SUB_KEYS,
                       &hKeyWicCodecs ) != ERROR_SUCCESS )
    {
        return false;
    }

    const UINT maxKeyLength = 255;
    DWORD index = 0;
    long ret;
    
    do
    {
        TCHAR  keyName[ maxKeyLength ] = { };
        DWORD size = maxKeyLength;

        ret = RegEnumKeyEx( hKeyWicCodecs, index, keyName, &size, NULL, NULL, NULL, NULL );
        
        index++;

        if ( ret == ERROR_SUCCESS )
        {
            ReadCodecInfo( nameKeyCodecs + L"\\" + keyName );
        }
    }
    while( ret == ERROR_SUCCESS );

    RegCloseKey( hKeyWicCodecs );    
    return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool Wic::ReadCodecInfo( const wstring& keyName )
{
    HKEY hKey;    
    if ( RegOpenKeyEx( HKEY_CLASSES_ROOT,
                       keyName.c_str( ),
                       NULL,
                       KEY_QUERY_VALUE,
                       &hKey ) != ERROR_SUCCESS )
    {
        return false;
    }

    wstring friendlyName;
    if ( !ReadString( hKey, L"FriendlyName", friendlyName ) )
    {
        RegCloseKey( hKey );
        return false;
    }

    wstring ClassId;
    if ( !ReadString( hKey, L"CLSID", ClassId ) )
    {
        RegCloseKey( hKey );
        return false;
    }

    RegCloseKey( hKey );

    // Now read the codec key itself
    wstring codecKey = L"CLSID\\" + ClassId;

    if ( RegOpenKeyEx( HKEY_CLASSES_ROOT,
                       codecKey.c_str( ),
                       NULL,
                       KEY_QUERY_VALUE,
                       &hKey ) != ERROR_SUCCESS )
    {
        return false;
    }

    wstring fileExtensions;
    if ( !ReadString( hKey, L"FileExtensions", fileExtensions ) )
    {
        RegCloseKey( hKey );
        return false;
    }

    RegCloseKey( hKey );

    if ( ( friendlyName.length( ) == 0 ) ||
         ( ClassId.length( ) == 0 ) ||
         ( fileExtensions.length( ) == 0 ) )
    {
        return false;
    }

    // Store the codec info 
    CodecInfo codec;
    codec.name = friendlyName;
    codec.classId = ClassId;
    codec.extensions = fileExtensions;
    m_codecs.push_back( codec );
        
    return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool Wic::ReadString( const HKEY hKey, const wstring& value, wstring& data ) const
{
    const UINT maxValueLength = 1024;
    
    BYTE buffer[ maxValueLength ] = { };
    DWORD type = 0;
    DWORD size = maxValueLength;

    if ( RegQueryValueEx( hKey,
                          value.c_str( ),
                          NULL,
                          &type,
                          buffer,
                          &size ) != ERROR_SUCCESS )
    {
        RegCloseKey( hKey );    
        return false;
    }

    if ( type != REG_SZ )
    {        
        RegCloseKey( hKey );    
        return false;
    }

    data = wstring( reinterpret_cast<TCHAR*>( buffer ) );
    return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

} // namespace Utilities
} // namespace Plugin
} // namespace Gimp

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////