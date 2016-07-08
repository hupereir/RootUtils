// $Id: Color.h,v 1.2 2005/10/12 08:44:22 pereira Exp $
#ifndef Color_h
#define Color_h

#include <TROOT.h>
#include <TObject.h>

//* color creation
class Color:public TObject
{

  public:

  //* constructor (doing nothing, needed for root)
  Color( int index ):
    TObject(),
    fIndex( index )
  {}

  //* constructor from rgb
  Color( Int_t red, Int_t green, Int_t blue );

  //* get index
  int Get( void ) const
  { return fIndex; }

  //* merge two colors (from index) return index of merged color
  int Merge( int second, double ratio ) const;

  private:

  //* color index
  int fIndex;

  ClassDef(Color,0)

};



#endif
