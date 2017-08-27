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

#include <algorithm>

#include "ConfigFile.h"
#include "../WIC_Decoder.h"

using namespace std;

EXTERN_C IMAGE_DOS_HEADER __ImageBase;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace Gimp {
namespace Plugin {
namespace Utilities {

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

ConfigFile::ConfigFile( )
{
    wstring path( _MAX_PATH, '\0' );
    DWORD written = GetModuleFileName( (HINSTANCE) &__ImageBase, &path[0], _MAX_PATH );

    if ( written > 0 )
    {
        m_path = path.substr( 0, path.find_last_of( '.' ) ) + L".ini";
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

ConfigFile::~ConfigFile( )
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool ConfigFile::GetDisplayName( wstring& name ) const
{
    if ( m_path.length( ) == 0 ) return false;
    
    const UINT bufferSize = 4096;
    vector<WCHAR> buffer( bufferSize, '\0' );
    DWORD written = GetPrivateProfileString( L"GUI", L"name", L"WIC Supported images", buffer.data( ), bufferSize, m_path.c_str( ) );
    
    if ( ( written > 0 ) && ( written < 32 ) )
    {
        name = wstring( buffer.begin( ), buffer.begin( ) + written );
        return true;
    }

    return false;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool ConfigFile::IsExtensionExcluded( const wstring& extension ) const
{
    if ( m_path.length( ) == 0 ) return false;

    const UINT bufferSize = 4096;
    vector<WCHAR> buffer( bufferSize, '\0' );
    DWORD written = GetPrivateProfileString( L"Extensions", extension.c_str( ), L"included", buffer.data( ), bufferSize, m_path.c_str( ) );

    if ( written > 0 )
    {
        wstring value( buffer.begin( ), buffer.begin( ) + written );
        wstring valueLowercase;
        transform( value.begin( ), value.end( ), back_inserter( valueLowercase ), tolower );

        if ( valueLowercase == L"excluded" )
        {
            return true;
        }
    }

    return false;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

} // namespace Utilities
} // namespace Plugin
} // namespace Gimp

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////