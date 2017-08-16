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

#include <windows.h>

#include <algorithm>
#include <cctype>
#include <functional>

#include "StringUtilities.h"

using namespace std;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace Gimp {
namespace Plugin {
namespace Utilities {

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

wstring ToHexString( const UINT32 number )
{
    wchar_t buffer[16];
    swprintf( buffer, 16, L"%08X", number );

    wstring text;
    text.assign(buffer, 8);
    return text;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

string ToLower( const string& s )
{
    string lower;
    transform( s.begin( ), s.end( ), back_inserter( lower ), tolower );
    return lower;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

std::wstring ToWideString( const char* s )
{
    string temp( s );
    return ToWideString( temp );
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

wstring ToWideString( const string& s )
{
    wstring value( s.begin( ), s.end( ) );
    return value;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool IsEqual( const string& s1, const string& s2 )
{
    const string l1 = ToLower( s1 );
    const string l2 = ToLower( s2 );

    return ( l1.compare( l2 ) == 0 );
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

string ToString( const wstring& s )
{
    string value( s.begin( ), s.end( ) );
    return value;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// See: http://stackoverflow.com/questions/216823/whats-the-best-way-to-trim-stdstring

void Trim( string& s )
{
    TrimLeft( s );
    TrimRight( s );
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void TrimLeft( string& s )
{
    s.erase( s.begin( ), find_if( s.begin( ), s.end( ), not1( ptr_fun<int, int>( isspace ) ) ) );
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void TrimRight( string& s )
{
    s.erase( find_if( s.rbegin( ), s.rend( ), not1( ptr_fun<int, int>( isspace ) ) ).base( ), s.end( ) );
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void RemoveLeadingChar( string& s, const char c )
{
    size_t size = s.size( );
    if ( size <= 0 ) return;

    if ( s[ 0 ] == c )
    {
        s.erase( 0, 1 );
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void RemoveTrailingChar( string& s, const char c )
{
    size_t size = s.size( );
    if ( size <= 0 ) return;

    if ( s[ size - 1 ] == c )
    {
        s.erase( s.end( ) - 1 );
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

string ReplaceCharacter( const string s, const char source, const char target )
{
    string output = s;
    replace( output.begin( ), output.end( ), source, target );
    return output;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

UINT Split( const string& s, const char seperator, vector<string>& list )
{
    list.clear( );    
    string::size_type i = 0;
    string::size_type j = s.find( seperator );

    while ( j != string::npos )
    {
        string item = s.substr( i, j - i );
        if ( item.length( ) > 0 ) list.push_back( item );

        i = ++j;
        j = s.find( seperator, j );

        if ( j == string::npos )
        {
            item = s.substr( i, s.length( ) );
            if ( item.length( ) > 0 ) list.push_back( item );
        }
    }

    return static_cast<UINT>( list.size( ) );
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void AddToWideCharVector( vector<WCHAR>& wchars, const wstring& s )
{
    UINT count = static_cast<UINT>( s.size( ) );
    for ( UINT i = 0 ; i < count ; i++ )
    {
        wchars.push_back( s[ i ] );
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool ExistsInList( const vector<wstring>& list, const wstring& value )
{
    UINT count = static_cast<UINT>( list.size( ) );
 
    for ( UINT i = 0 ; i < count ; i++ )
    {
        if ( list[ i ].compare( value ) == 0 )
        {
            return true;
        }
    }
           
    return false;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void RemoveDuplicates( vector<wstring>& list )
{
    vector<wstring> tmp;
    UINT count = static_cast<UINT>( list.size( ) );
 
    for ( UINT i = 0 ; i < count ; i++ )
    {
        if ( !ExistsInList( tmp, list[ i ] ) )
        {
            tmp.push_back( list[ i ] );
        }
    }
 
    list = tmp;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int StringToInt( const string& s )
{
    return atoi( s.c_str(  ) );
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

} // namespace Utilities
} // namespace Plugin
} // namespace Gimp

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////