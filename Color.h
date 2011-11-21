// $Id: Color.h,v 1.2 2005/10/12 08:44:22 pereira Exp $
#ifndef Color_h
#define Color_h

#include <TROOT.h>
#include <TObject.h>

//! color creation
class Color:public TObject
{

  public:

  //! constructor (doing nothing, needed for root)
  Color( int index ):
    TObject(),
    fIndex( index )
  {}

  //! merge two colors (from index) return index of merged color
  int Merge( int second, double ratio );

  private:

  //! color index
  int fIndex;

  ClassDef(Color,0)

};



#endif
