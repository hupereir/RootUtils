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

  //! verbosity
  enum Verbosity { NONE, SOME, ALOT, MAX };

  //! square a number of a given type
  static double SQUARE(const double& x)
  { return x*x; }

  #ifndef __CINT__
  //! try find object of given type/name; delete it if found.
  template< typename T >
  static void Delete( const char* name )
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
  static T* backup( T* obj, const TString& tag = "" )
  {

    if( !obj ) return 0;
    TString name( obj->GetName() );
    TString newName( name );
    int index( 0 );
    do {

      newName = Form( "%s%s_%i", name.Data(), tag.Data(), index );
      index ++;

    } while( (T*) gROOT->FindObject( newName ) );

    return (T*) obj->Clone( newName );

  }

  #endif

};


#endif
