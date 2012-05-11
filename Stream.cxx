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
//! root dictionary
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
void Stream::PrintVector( std::ostream& out, const char* name, const double* values, const int& size, const char* format )
{ PrintVector<double>( out, "double", name, values, size, format ); }

//________________________________________________________________________
void Stream::PrintIntVector( std::ostream& out, const char* name, const int* values, const int& size, const char* format )
{ PrintVector<int>( out, "int", name, values, size, format ); }

//_________________________________
void Stream::PrintVector2D( std::ostream& out, const char* name, const double* values, const int& size_1, const int& size_2, const char* format )
{ PrintVector2D<double>( out, "double", name, values, size_1, size_2, format ); }

//_________________________________
void Stream::PrintIntVector2D( std::ostream& out, const char* name, const int* values, const int& size_1, const int& size_2, const char* format )
{ PrintVector2D<int>( out, "int", name, values, size_1, size_2, format ); }

//_________________________________
void Stream::PrintVector3D( std::ostream& out, const char* name, const double* values, const int& size_1, const int& size_2, const int& size_3,const char* format )
{ PrintVector3D<double>( out, "double", name, values, size_1, size_2, size_3, format ); }

//_________________________________
void Stream::PrintVector4D( std::ostream& out, const char* name, const double* values, const int& size_1, const int& size_2, const int& size_3,const int& size_4, const char* format )
{ PrintVector4D<double>( out, "double", name, values, size_1, size_2, size_3, size_4, format ); }

//________________________________________________________________________
void Stream::PrintCuts( const TCut& cut )
{

    std::cout << ReplaceAll( (const char*) cut, "&&", "&&\n" );
    std::cout << std::endl;

}
