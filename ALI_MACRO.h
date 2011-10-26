// $Id: ALI_MACRO.h,v 1.7 2006/12/27 14:31:34 hugo Exp $
#ifndef ALI_MACRO_H
#define ALI_MACRO_H

/*!
  \file ALI_MACRO.h
  \brief typedefs and enumerations
*/

#include <string>
#include <TObject.h>
#include <TROOT.h>
#include <sstream>

//! typedefs and enumerations
class ALI_MACRO
{

  public:

  //! arms
  enum {
    SouthArm,
    NorthArm
  };

  //! arms (bitwise)
  enum Arm {
    SOUTH = 1<<0,
    NORTH = 1<<1,
    BOTH = SOUTH | NORTH };

  //! verbosity
  enum Verbosity { NONE, SOME, ALOT, MAX };

  //! number of arms
  enum { MAX_ARM = 2 };

  //! number of stations
  enum { MAX_STATION = 3 };

  //! number of gaps/station
  enum { MAX_GAP = 3 };

  //! number of cathode/gap
  enum { MAX_CATHODE = 2 };

  //! returns uniq id associated to arm, station, gap and cathode
  static int get_index( int station, int gap, int cathode )
  {
    return
      cathode + MAX_CATHODE*(
      gap + MAX_GAP*(
      station ) );
  }


  //! square a number of a given type
  static double SQUARE(const double& x)
  { return x*x; }

  #ifndef __CINT__
  //! try find object of given type/name; delete it if found.
  template< typename T >
  static void safe_delete( const char* name )
  {
    if( !name ) return;
    T* h = (T*) gROOT->FindObject( name );
    if( h ) SafeDelete( h );
    return;
  }

  /*!
    make backup copy of a TObject. Namely rename it, apending _#i to the name
    with i the first index of a non existing object of the same type
  */
  template< typename T >
  static T* backup( T* obj, const std::string& tag = "" )
  {

    if( !obj ) return 0;
    std::string name( (const char*) obj->GetName() );
    std::string new_name( name );
    int index( 0 );
    do {

      std::ostringstream what;
      what << name << tag << "_" << index;
      new_name = what.str();
      index ++;

    } while( (T*) gROOT->FindObject( new_name.c_str() ) );

    return (T*) obj->Clone( new_name.c_str() );

  }

  #endif

};


#endif
