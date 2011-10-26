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
  static void assert_true( const bool& value, const std::string& message )
  { 
		if( !value ) 
		{
			str() << message << std::endl;
			throw std::logic_error( message ); 
		}
	}
			
  //! writes string to cout if level is lower than debug_level_
	static void str( int level, const std::string& buf )
  {
  
    if( debug_level_ < level ) return ;  
    std::cout << buf;  
    return;
  
  }
  
  //! writes string to cout if debug_level_ is bigger than 0
	static void str( const std::string& buf ) 
  { str( 1, buf ); }

  //! returns either cout or dummy stream depending of the level
	static std::ostream& str( int level = 1 )
  {
    if( debug_level_ < level ) return null_stream_;  
    return std::cout;  
  }
  
	#endif
	
  //! sets the debug level. Everything thrown of bigger level is not discarded
  static void set_debug_level( int level ) 
  { debug_level_ = level; }
  
  //! retrieves the debug level
  static int get_debug_level( void ) 
  { return debug_level_; }
  
  private:
  
  //! debug level
  static int debug_level_;  
  
  //! dummy stream to discard the text of two high debug level
  static std::ofstream null_stream_;
};

#endif
