// $Id: Projection.h,v 1.1 2005/11/15 02:55:57 hpereira Exp $
#ifndef ProjectionfH
#define ProjectionfH

/*!
\file Projection.h
\brief simple class to handle an histogram projection of a tree
*/

#include <string>
#include <list>
#include <TCut.h>
#include <TH1.h>

//! simple class to handle an histogram projection of a tree
class Projection
{
  public:

  //! constructor
  Projection(
    const std::string& hName,
    const std::string& varName,
    const TCut& cut ):
    fHName( hName ),
    fVarName( varName ),
    fCut( cut ),
    fH( 0 )
  {}

  //! define bins
  void SetBins( unsigned int bins, double min, double max )
  {
    if( fH  ) delete fH;
    fH = new TH1F( fHName.c_str(), fHName.c_str(), bins, min, max );
  }

  //! histogram name
  std::string fHName;

  //! variable name
  std::string fVarName;

  //! cut used to project histogram
  TCut fCut;

  //! projected histogram
  TH1 *fH;

};

typedef std::list<Projection> ProjectionList;

#endif

