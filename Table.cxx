// $Id: Table.cxx,v 1.21 2010/09/15 02:27:24 hpereira Exp $

/*!
\file Table.cxx
\brief data formating
*/

#include "Table.h"
#include "Stream.h"
#include "Utils.h"

#include <algorithm>
#include <fstream>
#include <iostream>
#include <sstream>

//_________________________________________________
//! root dictionary
ClassImp( ColumnBase );
ClassImp( Table );

//_________________________________________________________________
void Line::Parse( const TString& line_buffering )
{

  clear();

  std::istringstream in( line_buffering.Data() );
  while( !(in.rdstate() & std::ios::failbit ) )
  {
    std::string value;
    in >> value;
    if( !(in.rdstate() & std::ios::failbit ) ) push_back( value );
  }

  std::cout << "Table::Line::Parse - " << size() << " values read" << std::endl;
}

//_________________________________________________________________
void Table::AddConversion( const char* c1, const char* c2 )
{ if( c1 && c2 ) fConversions.push_back( ConversionPair( std::string( c1 ), std::string( c2 ) ) ); }

//_________________________________________________________________
void Table::Load( const char* filename )
{
  if( !filename ) {
    std::cout << "Table::load - empty string." << std::endl;
    return;
  }

  std::ifstream in( filename );
  if( !in ) {
    std::cout << "Table::load - invalid file: " << filename << std::endl;
    return;
  }

  // clear columns
  fColumns.clear();

  // store Parsed lines
  std::vector< Line > lines;

  char line_buffer[1024];
  while( in.getline( line_buffer, 1024, '\n' ) )
  {

    if( !strlen( line_buffer ) ) continue;
    if( strncmp( line_buffer, "//", 2 ) == 0 ) continue;

    // add conversions
    std::string line_str( line_buffer );
    for( std::list< ConversionPair >::const_iterator iter = fConversions.begin(); iter != fConversions.end(); iter++ )
    { line_str = Stream::ReplaceAll( line_str, iter->first.Data(), iter->second.Data() ); }

    Line line;
    line.Parse( line_str.c_str() );
    if( !line.empty() ) lines.push_back( line );

  }

  std::cout << "Table::load - " << lines.size() << " lines read" << std::endl;
  if( lines.empty() ) return;

  // retrieve number of columns
  int nfColumns = min_element(  lines.begin(), lines.end(), SizeLessFTor() )->size();
  std::cout << "Table::load - number of columns: " << nfColumns << std::endl;

  // fill columns
  for( int i_column=0; i_column< nfColumns; i_column++ )
  {

    // check type
    ColumnBase* column = 0;
    if( lines[0].Is<double>(i_column) ) column = new ColumnDouble();
    else column = new ColumnString();
    for( int i_line=0; i_line < lines.size(); i_line++ )
      column->AddValue( lines[i_line][i_column] );

    fColumns.push_back( column );

  }

  return;

}

//_________________________________________________________________
void Table::ClearConversions( void )
{ fConversions.clear(); }

//_________________________________________________________________
int Table::GetNLines( void ) const
{ return (*std::min_element( fColumns.begin(), fColumns.end(), SizeLessFTor() ))->Size(); }

//_________________________________________________________________
double* Table::GetDoubleArray( int column, int firstLine, int nLines ) const
{

  // check column index
  if( !CheckColumn( column ) ) return 0;

  // try cast column
  ColumnDouble *column_double = static_cast<ColumnDouble*>(fColumns[column]);
  if( !column_double )
  {
    std::cout << "Table::get_column_array - cannot cast column " << column << std::endl;
    return 0;
  }

  return column_double->GetArray( firstLine, nLines );
}

//_________________________________________________________________
void Table::PrintLatex( std::ostream& out, int firstLine, int nLines ) const
{

  // dump begin of table
  if( !(fFlags & SKIP_HEADER ) )
  {
    out << "\\begin{tabular}{";
    for( int column = 0; column < fColumns.size(); column++ )
    {
      if( fColumns[column]->GetType() & ColumnBase::HAS_HEADER ) {
        out << fColumns[column]->GetAlignment();
        if( column < fColumns.size()-1 ) out << "|";
      }
    }

    out << "}" << std::endl;

    for( int column = 0; column < fColumns.size(); column++ )
    {
      if( column != 0 && (fColumns[column]->GetType() & ColumnBase::HAS_HEADER) ) out << " & " << fColumns[column]->GetName();
      else if( column == 0 && (fColumns[column]->GetType() & ColumnBase::HAS_HEADER) ) out << fColumns[column]->GetName();
    }

    out << "\\\\" << std::endl;
  }

  out << "\\hline" << std::endl;

  // get min number of entries in the columns
  int nLinesMax = (*std::min_element( fColumns.begin(), fColumns.end(), SizeLessFTor() ))->Size();
  if( nLines ) nLinesMax = std::min( nLinesMax, nLines+firstLine );

  // dump values
  for( int line = firstLine; line < nLinesMax; line++ )
  {
    for( int column = 0; column < fColumns.size(); column++ )
    {
      std::string value_string( fColumns[column]->GetString(line) );

      // convert exp to latex format
      while( value_string.find( "e-0" ) != std::string::npos ) value_string = Stream::ReplaceAll( value_string, "e-0", "e-" );
      while( value_string.find( "e0" ) != std::string::npos ) value_string = Stream::ReplaceAll( value_string, "e0", "e" );
      if( value_string.find( "e" ) != std::string::npos ) value_string = Stream::ReplaceAll( value_string, "e", "\\;10^{" )+"}";

      // print column
      if( column == 0 ) out << "$";
      else if( fColumns[column]->GetType() & ColumnBase::HAS_HEADER ) out << "$ & $";
      if( fColumns[column]->GetType() == ColumnBase::INTERVAL_BEGIN ) out << "[";
      if( fColumns[column]->GetType() == ColumnBase::INTERVAL_END ) out << " , ";
      if( fColumns[column]->GetType() == ColumnBase::ERROR ) out << " \\pm ";
      if( fColumns[column]->GetType() == ColumnBase::ERROR_PLUS ) out << "^{+";
      if( fColumns[column]->GetType() == ColumnBase::ERROR_MINUS ) out << "_{-";
      out << value_string;
      if( fColumns[column]->GetType() == ColumnBase::ERROR_PLUS ) out << "}";
      if( fColumns[column]->GetType() == ColumnBase::ERROR_MINUS ) out << "}";
      if( fColumns[column]->GetType() == ColumnBase::INTERVAL_END ) out << "]";

    }
    out << "$ \\\\" << std::endl;

    if( fHorizontalLines.find( line+1 ) != fHorizontalLines.end() )
    { out << "\\hline" << std::endl; }

  }

  if( !(fFlags & SKIP_TRAILER ) )
  {
    out << "\\end{tabular}" << std::endl;
    out << std::endl;
  }

}

//_________________________________________________________________
void Table::PrintText( std::ostream& out, int firstLine, int nLines ) const
{


  // dump header
  if( !(fFlags & SKIP_HEADER) )
  {
    for( int column = 0; column < fColumns.size(); column++ )
      if( column != 0 ) out << "   " << fColumns[column]->GetName();
    else out << fColumns[column]->GetName();

    out << std::endl;
  }

  // get min number of entries in the columns
  int nLinesMax = (*std::min_element( fColumns.begin(), fColumns.end(), SizeLessFTor() ))->Size();
  if( nLines ) nLinesMax = std::min( nLinesMax, nLines+firstLine );

  // dump values
  for( int line = firstLine; line < nLinesMax; line++ )
  {
    for( int column = 0; column < fColumns.size(); column++ )
    {
      std::string value_string( fColumns[column]->GetString(line) );

      // convert exp to latex format
      while( value_string.find( "e-0" ) != std::string::npos ) value_string = Stream::ReplaceAll( value_string, "e-0", "e-" );
      while( value_string.find( "e0" ) != std::string::npos ) value_string = Stream::ReplaceAll( value_string, "e0", "e" );
      if( value_string.find( "e" ) != std::string::npos )
        value_string = Stream::ReplaceAll( value_string, "e", " 10^" );

      // print column
      else if( fColumns[column]->GetType() & ColumnBase::HAS_HEADER ) out << "   ";
      if( fColumns[column]->GetType() == ColumnBase::INTERVAL_BEGIN ) out << "[";
      if( fColumns[column]->GetType() == ColumnBase::INTERVAL_END ) out << " , ";
      if( fColumns[column]->GetType() == ColumnBase::ERROR ) out << " +/- ";
      if( fColumns[column]->GetType() == ColumnBase::ERROR_PLUS ) out << " +";
      if( fColumns[column]->GetType() == ColumnBase::ERROR_MINUS ) out << " -";
      out << value_string;
      if( fColumns[column]->GetType() == ColumnBase::INTERVAL_END ) out << "]";

    }
    if( !( fFlags & SKIP_TRAILER ) ) { out << std::endl; }
  }

}

//_________________________________________________________________
void Table::PrintC( std::ostream& out, int firstLine, int nLines ) const
{

  // dump values
  for( int column = 0; column < fColumns.size(); column++ )
  {

    // get min number of entries in the columns
    int nLinesMax = (*std::min_element( fColumns.begin(), fColumns.end(), SizeLessFTor() ))->Size();
    if( nLines ) nLinesMax = std::min( nLinesMax, nLines+firstLine );

    if( nLinesMax == 1 )
    {
      out << "double " << fColumns[column]->GetName() << " = " <<  fColumns[column]->GetString(firstLine) << ";" << std::endl;
    } else {
      out << "double " << fColumns[column]->GetName() << "[" << nLinesMax-firstLine << "] = {";
      for( int line = firstLine; line < nLinesMax; line++ )
      {
        if( line != firstLine ) out << ", ";
        out << fColumns[column]->GetString(line);
      }
      out << "}; " << std::endl;
    }
  }
}
