#ifndef Stream_h
#define Stream_h

// $Id: Streamer.h,v 1.47 2010/09/15 02:27:25 hpereira Exp $

#include <TROOT.h>
#include <TCut.h>
#include <TH1.h>
#include <TObject.h>

#include <iostream>
#include <vector>
#include <string>

class Stream:public TObject
{

  public:

  //* constructor (doing nothing, needed for root)
  Stream():TObject()
  {}

  //* string replacement
  static std::string ReplaceAll( const std::string& in, const std::string& c0, const std::string& c1 );

  //* print vector
  static void PrintVector( std::ostream& out, TString name, const Double_t* values, Int_t size,  TString format = "%f" );

  //* print vector
  static void PrintVector( TString name, const std::vector<Double_t> values, TString format = "%f" )
  {
    if( !values.empty() )
    { PrintVector( std::cout, name, &values[0], values.size(), format ); }
  }

  //* print vector
  static void PrintVector( TString name, const Double_t* values, Int_t size, TString format = "%f" )
  { PrintVector( std::cout, name, values, size, format ); }

  //* print vector
  static void PrintIntVector( TString name, const Int_t* values, Int_t size, TString format = "%i" )
  { PrintIntVector( std::cout, name, values, size, format ); }

  //* print vector
  static void PrintIntVector( std::ostream& out, TString name, const Int_t* values, Int_t size, TString format = "%i" );

  //* print cuts, formated
  static void PrintCuts( const TCut& cut );

  //* print histogram axis
  static void PrintAxis( TH1* );

  //* print vector
  template<typename T>
  static void PrintVector( TString type, TString name, const std::vector<T> values, TString format = "%f" )
  {
    if( !values.empty() )
    { PrintVector( std::cout, type, name, &values[0], values.size(), format ); }
  }

  //* print vector
  template<typename T>
  static void PrintVector( TString type, TString name, const T* values, Int_t size, TString format = "%f" )
  { PrintVector( std::cout, type, name, values, size, format ); }

  //* print vector
  template<typename T>
  static void PrintVector( std::ostream& out, TString type, TString name, const T* values, Int_t size, TString format = "%f" )
  {
    out << "std::array<" << type << "," << size << "> " << name << " = {{";
    char value[20];
    for( Int_t i=0; i<size; ++i )
    {
      sprintf( value, format.Data(), values[i] );
      out << value;
      if( i != size-1 ) out << ", ";
      else out << "}};";
    }

    out << std::endl;
  }

  ClassDef(Stream,0)

};

#endif
