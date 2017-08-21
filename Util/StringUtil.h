/*
 *  Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
 */

#ifndef __UI_STRING_UTIL__
#define __UI_STRING_UTIL__

#include <FTL/Config.h>
#include <FabricCore.h>
#include <stdio.h>
#include <stdlib.h>
#include <algorithm>
#include <iterator>
#include <vector>
#include <cmath>
#include <string>
#include <iostream>
#include <sstream>
#include <fstream>
#include <streambuf>
#include <memory>
#include <assert.h>
#if defined(FTL_OS_DARWIN)
# include <CoreFoundation/CFURL.h>
#endif

#include <QString>
#include <QStringList>
#include <QRegExp>
#include <QValidator>

namespace FabricUI {
namespace Util {  

class StringUtil {
  
  public:
    StringUtil() {}

    static QStringList ProcessPathQStringForOsX(QStringList pathList) {
      QStringList pathList_;
      for(int i=0; i<pathList.size(); ++i) 
      {
        QString localFileQString = pathList[i];
      #if defined(FTL_OS_DARWIN)
          // [pzion 20150805] Work around
          // https://bugreports.qt.io/browse/QTBUG-40449
          if ( localFileQString.startsWith("/.file/id=") )
          {
            CFStringRef relCFStringRef =
              CFStringCreateWithCString(
                kCFAllocatorDefault,
                localFileQString.toUtf8().constData(),
                kCFStringEncodingUTF8
                );
            CFURLRef relCFURL =
              CFURLCreateWithFileSystemPath(
                kCFAllocatorDefault,
                relCFStringRef,
                kCFURLPOSIXPathStyle,
                false // isDirectory
                );
            CFErrorRef error = 0;
            CFURLRef absCFURL =
              CFURLCreateFilePathURL(
                kCFAllocatorDefault,
                relCFURL,
                &error
                );
            if ( !error )
            {
              static const CFIndex maxAbsPathCStrBufLen = 4096;
              char absPathCStr[maxAbsPathCStrBufLen];
              if ( CFURLGetFileSystemRepresentation(
                absCFURL,
                true, // resolveAgainstBase
                reinterpret_cast<UInt8 *>( &absPathCStr[0] ),
                maxAbsPathCStrBufLen
                ) )
              {
                localFileQString = QString( absPathCStr );
              }
            }
            CFRelease( absCFURL );
            CFRelease( relCFURL );
            CFRelease( relCFStringRef );
          }
      #endif
        pathList_.append(localFileQString);
      }
      return pathList_;
    }
};

/// Lower a string
/// \param s The string to lower
inline std::string ToLower(const std::string &s) {
  std::string str = s;
  transform(str.begin(), str.end(), str.begin(),::tolower);
  return str;
}

/// Checks if a string is a number.
/// \param s The string to check
inline bool IsNumber(const std::string& s) {
  std::string::const_iterator it = s.begin();
  while( it != s.end() && *it >= '0' && *it <= '9' ) ++it;
  return !s.empty() && it == s.end();
}

/// Removes the character within the string.
/// \param s The string to check
/// \param c The character to remove
inline std::string RemoveCharacter(const std::string& s, char c) {
  std::string str = s;
  str.erase( std::remove(str.begin(), str.end(), c), str.end());
  return str;
}

/// Removes the newline within the string.
/// \param s The string to check
inline std::string RemoveNewLine(const std::string& s) {
  std::string str = s;
  str.erase( std::remove(str.begin(), str.end(), '\n'), str.end());
  str.erase( std::remove(str.begin(), str.end(), '\r'), str.end());
  return str;
}

/// Removes the white space within the string.
/// \param s The string to check
inline std::string RemoveSpace(const std::string& s) {
  std::string str = s;
  str.erase( std::remove_if(str.begin(), str.end(), ::isspace), str.end());
  return str;
}

/// Converts a number into to string.
/// \param t The number
template <typename T> std::string ToStr(T t) { 
  std::ostringstream os; 
  os<<t; 
  return os.str(); 
} 

/// Converts a string into a number.
/// \param s The string to check
template<typename T> T ToNum(const std::string &number) {
  T value = 0;
  if(IsNumber(number)) {
    std::stringstream stream(number);
    stream >> value;
  }
  return value;
}

/// Splits a string into a vector.
/// \param s The string to split
/// \param c The delimiter tat splits the string
inline std::vector<std::string> Split(const std::string& s, char c) {

  std::vector<std::string> v;
  std::string::size_type i = 0;
  std::string::size_type j = s.find(c);

  while (j != std::string::npos) {
    v.push_back(s.substr(i, j-i));
    i = ++j;
    j = s.find(c, j);

    if (j == std::string::npos)
     v.push_back(s.substr(i, s.length()));
  }
  return v;
}

/// returns a new validator for doubles that
/// accepts dots and commas as decimal separator.
//  (this can be used instead of QDoubleValidator).
inline QRegExpValidator *newDoubleValidator(QObject *parent)
{
  return new QRegExpValidator(QRegExp("[-+]?\\d*[\\.,]?\\d+([eE][-+]?\\d+)?"), parent);
}

// converts a string into a float regardless
// whether the separator is a dot or a comma.
inline double tolerantStringToDouble(QString s)
{
  s.replace(',', '.');
  if (s.startsWith('.'))
    s = "0" + s;
  return s.toDouble();
}

} // namespace Util
} // namespace FabricUI
 
#endif // __UI_STRING_UTIL__
