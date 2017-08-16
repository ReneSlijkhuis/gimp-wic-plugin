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

#pragma once

#include <string>
#include <vector>

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace Gimp {
namespace Plugin {
namespace Utilities {

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

std::wstring ToHexString( const UINT32 number );
std::string ToLower( const std::string& s );
std::wstring ToWideString( const char* s );
std::wstring ToWideString( const std::string& s );
std::string ToString( const std::wstring& s );

bool IsEqual( const std::string& s1, const std::string& s2 );

void Trim( std::string& s );
void TrimLeft( std::string& s );
void TrimRight( std::string& s );

void RemoveLeadingChar( std::string& s, const char c );
void RemoveTrailingChar( std::string& s, const char c );
std::string ReplaceCharacter( const std::string s, const char source, const char target );
UINT Split( const std::string& s, const char seperator, std::vector<std::string>& list );

void AddToWideCharVector( std::vector<WCHAR>& wchars, const std::wstring& s );
bool ExistsInList( const std::vector<std::wstring>& list, const std::wstring& value );
void RemoveDuplicates( std::vector<std::wstring>& list );

int StringToInt( const std::string& s );

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

} // namespace Utilities
} // namespace Plugin
} // namespace Gimp

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////