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
  template <class T>
  static void PrintVector( const std::string& name, const std::vector<T>& values, const std::string& format = "%f" )
  { PrintVector( std::cout, name, values, format ); }

  //* print vector
  template<class T>
  static void PrintVector( const std::string& type, const std::string& name, const std::vector<T>& values, const std::string& format = "%f" )
  { PrintVector( std::cout, type, name, values, format ); }

  //* print vector
  template<class T>
  static void PrintVector( std::ostream& out, const std::string& name, const std::vector<T>& values,  const std::string& format = "%f" )
  { PrintVector( std::cout, "float", name, values, format ); }

  //* print vector
  template<class T>
  static void PrintVector( std::ostream& out, const std::string& type, const std::string& name, const std::vector<T>& values,  const std::string& format = "%f" );

  //* print cuts, formated
  static void PrintCuts( const TCut& cut );

  //* print histogram axis
  static void PrintAxis( TH1* );

  ClassDef(Stream,0)

};

//___________________________________________________________________
template<class T>
void Stream::PrintVector( std::ostream& out, const std::string& type, const std::string& name, const std::vector<T>& values, const std::string& format )
{
  const auto size( values.size() );
  out << "constexpr std::array<" << type << "," << size << ">" << " " << name << " = {{";

  char value[20];
  for( Int_t i=0; i<size; ++i )
  {
    sprintf( value, format.c_str(), values[i] );
    out << value;
    if( i != size-1 ) out << ", ";
    else out << "}};";
  }

  out << std::endl;
}

#endif
