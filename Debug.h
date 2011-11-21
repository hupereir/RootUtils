// $Id: Debug.h,v 1.2 2005/07/14 18:51:26 hpereira Exp $

#ifndef Debug_h
#define Debug_h

/*!
\file    Debug.h
\brief   option driven debugging facility
\author  Hugo Pereira
\version $Revision: 1.2 $
\date    $Date: 2005/07/14 18:51:26 $
*/

#include <iostream>
#include <fstream>
#include <string>
#include <stdexcept>

/*!
\class   Debug
\brief   option driven debugging class
*/

class Debug
{
  public:

  #ifndef __CINT__

  //! check pointer, throw exception if null
  static void AssertTrue( const bool& value, const std::string& message )
  {
    if( !value )
    {
      Str() << message << std::endl;
      throw std::logic_error( message );
    }
  }

  //! writes string to cout if level is lower than fDebugLevel
  static void Str( int level, const std::string& buf )
  {

    if( fDebugLevel < level ) return ;
    std::cout << buf;
    return;

  }

  //! writes string to cout if fDebugLevel is bigger than 0
  static void Str( const std::string& buf )
  { Str( 1, buf ); }

  //! returns either cout or dummy stream depending of the level
  static std::ostream& Str( int level = 1 )
  {
    if( fDebugLevel < level ) return fNullStream;
    return std::cout;
  }

  #endif

  //! sets the debug level. Everything thrown of bigger level is not discarded
  static void SetDebugLevel( int level )
  { fDebugLevel = level; }

  //! retrieves the debug level
  static int GetDebugLevel( void )
  { return fDebugLevel; }

  private:

  //! debug level
  static int fDebugLevel;

  //! dummy stream to discard the text of two high debug level
  static std::ofstream fNullStream;
};

#endif
