// $Id: Color.cxx,v 1.3 2005/10/13 22:12:38 hugo Exp $

#include "Color.h"
#include <iostream>
#include <TColor.h>

using namespace std;

//_____________________________________
ClassImp( Color )

//_____________________________________
int Color::Merge( int secondfIndex, double ratio )
{
	if( ratio < 0 ) ratio = 0;
	if( ratio > 1 ) ratio = 1;

	TColor first( fIndex, 0, 0, 0 );
	TColor second( secondfIndex, 0, 0, 0 );

	double red = ratio*first.GetRed() + (1.0-ratio)*second.GetRed();
	double green = ratio*first.GetGreen() + (1.0-ratio)*second.GetGreen();
	double blue = ratio*first.GetBlue() + (1.0-ratio)*second.GetBlue();
	return TColor::GetColor( Float_t(red), Float_t(green), Float_t(blue) );
}
