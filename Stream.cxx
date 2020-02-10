// $Id: Utils.cxx,v 1.56 2010/09/15 02:27:25 hpereira Exp $

/*!
\file Utils.cxx
\brief some root utilities to handle histograms safely
\author	Hugo Pereira
\version $Revision: 1.56 $
\date $Date: 2010/09/15 02:27:25 $
*/

#include "Stream.h"

//__________________________________________________
//* root dictionary
ClassImp( Stream );

//________________________________________________________________________
std::string Stream::ReplaceAll( const std::string& in, const std::string& c1, const std::string& c2 )
{
    if( !c1.size() ) return "";
    std::string out("");
    size_t len = in.size();
    size_t current = 0;
    size_t found=0;
    while( current < len && ( found = in.find( c1, current ) ) != std::string::npos )
    {
        out += in.substr( current, found-current ) + c2;
        current=found+c1.size();
    }

    if( current < len ) out += in.substr( current, len-current );

    return out;

}

//________________________________________________________________________
void Stream::PrintVector( std::ostream& out, TString name, const Double_t* values, Int_t size, TString format )
{ PrintVector<Double_t>( out, "const Double_t", name, values, size, format ); }

//________________________________________________________________________
void Stream::PrintIntVector( std::ostream& out, TString name, const Int_t* values, Int_t size, TString format )
{ PrintVector<int>( out, "const Int_t", name, values, size, format ); }

//________________________________________________________________________
void Stream::PrintCuts( const TCut& cut )
{

    std::cout << ReplaceAll( (const char*) cut, "&&", "&&\n" );
    std::cout << std::endl;

}

//________________________________________________________________________
void Stream::PrintAxis( TH1* h )
{
  std::vector<TAxis*> axis;
  axis.push_back( h->GetXaxis() );
  axis.push_back( h->GetYaxis() );
  axis.push_back( h->GetZaxis() );

  const TString names[] = { "x", "y", "z" };

  std::cout << "Histogram name: " << h->GetName() << std::endl;
  for( Int_t i=0; i < 3; ++i )
  {
    if( !axis[i] ) continue;
    std::cout
      << "  Axis " << names[i] << " -"
      << " nBins: " << axis[i]->GetNbins()
      << " min: " << axis[i]->GetXmin()
      << " max: " << axis[i]->GetXmax()
      << std::endl;
  }

}
