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
    std::string lineStream( line_buffer );
    for( std::list< ConversionPair >::const_iterator iter = fConversions.begin(); iter != fConversions.end(); iter++ )
    { lineStream = Stream::ReplaceAll( lineStream, iter->first.Data(), iter->second.Data() ); }

    Line line;
    line.Parse( lineStream.c_str() );
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
  if( !(fFlags & SkipHeader ) )
  {
    out << "\\begin{tabular}{";

    Bool_t first = kTRUE;
    for( int column = 0; column < fColumns.size(); column++ )
    {
      if( fColumns[column]->GetType() & ColumnBase::HasHeader )
      {
        if(!first ) out << "|";
        first = kFALSE;
        out << fColumns[column]->GetAlignment();
      }
    }

    out << "}" << std::endl;

    for( int column = 0; column < fColumns.size(); column++ )
    {
      if( column != 0 && (fColumns[column]->GetType() & ColumnBase::HasHeader) ) out << " & " << fColumns[column]->GetName();
      else if( column == 0 && (fColumns[column]->GetType() & ColumnBase::HasHeader) ) out << fColumns[column]->GetName();
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
      std::string valueString( fColumns[column]->GetString(line) );

      // convert exp to latex format
      while( valueString.find( "e-0" ) != std::string::npos ) valueString = Stream::ReplaceAll( valueString, "e-0", "e-" );
      while( valueString.find( "e0" ) != std::string::npos ) valueString = Stream::ReplaceAll( valueString, "e0", "e" );
      if( valueString.find( "e" ) != std::string::npos ) valueString = Stream::ReplaceAll( valueString, "e", "\\;10^{" )+"}";

      // print column
      if( column == 0 ) out << "$";
      else if( fColumns[column]->GetType() & ColumnBase::HasHeader ) out << "$ & $";
      if( fColumns[column]->GetType() == ColumnBase::IntervalBegin ) out << "[";
      if( fColumns[column]->GetType() == ColumnBase::IntervalEnd ) out << " , ";
      if( fColumns[column]->GetType() == ColumnBase::Error ) out << " \\pm ";
      if( fColumns[column]->GetType() == ColumnBase::ErrorPlus ) out << "^{+";
      if( fColumns[column]->GetType() == ColumnBase::ErrorMinus ) out << "_{-";
      if( fColumns[column]->GetType() == ColumnBase::ErrorRel ) out << "\\;(";
      out << valueString;
      if( fColumns[column]->GetType() == ColumnBase::ErrorPlus ) out << "}";
      if( fColumns[column]->GetType() == ColumnBase::ErrorMinus ) out << "}";
      if( fColumns[column]->GetType() == ColumnBase::IntervalEnd ) out << "]";
      if( fColumns[column]->GetType() == ColumnBase::ErrorRel ) out << "\\%)";

    }
    out << "$ \\\\" << std::endl;

    if( fHorizontalLines.find( line+1 ) != fHorizontalLines.end() )
    { out << "\\hline" << std::endl; }

  }

  if( !(fFlags & SkipTrailer ) )
  {
    out << "\\end{tabular}" << std::endl;
    out << std::endl;
  }

}

//_________________________________________________________________
void Table::PrintText( std::ostream& out, int firstLine, int nLines ) const
{


  // dump header
  if( !(fFlags & SkipHeader) )
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
      std::string valueString( fColumns[column]->GetString(line) );

      // convert exp to latex format
      while( valueString.find( "e-0" ) != std::string::npos ) valueString = Stream::ReplaceAll( valueString, "e-0", "e-" );
      while( valueString.find( "e0" ) != std::string::npos ) valueString = Stream::ReplaceAll( valueString, "e0", "e" );
      if( valueString.find( "e" ) != std::string::npos )
        valueString = Stream::ReplaceAll( valueString, "e", " 10^" );

      // print column
      else if( fColumns[column]->GetType() & ColumnBase::HasHeader ) out << "   ";
      if( fColumns[column]->GetType() == ColumnBase::IntervalBegin ) out << "[";
      if( fColumns[column]->GetType() == ColumnBase::IntervalEnd ) out << " , ";
      if( fColumns[column]->GetType() == ColumnBase::Error ) out << " +/- ";
      if( fColumns[column]->GetType() == ColumnBase::ErrorPlus ) out << " +";
      if( fColumns[column]->GetType() == ColumnBase::ErrorMinus ) out << " -";
      out << valueString;
      if( fColumns[column]->GetType() == ColumnBase::IntervalEnd ) out << "]";

    }
    if( !( fFlags & SkipTrailer ) ) { out << std::endl; }
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
      out << "const Double_t " << fColumns[column]->GetName() << " = " <<  fColumns[column]->GetString(firstLine) << ";" << std::endl;
    } else {
      out << "const Double_t " << fColumns[column]->GetName() << "[" << nLinesMax-firstLine << "] = {";
      for( int line = firstLine; line < nLinesMax; line++ )
      {
        if( line != firstLine ) out << ", ";
        out << fColumns[column]->GetString(line);
      }
      out << "}; " << std::endl;
    }
  }

}


//_________________________________________________________________
void Table::PrintHep( std::ostream& out, int firstLine, int nLines ) const
{

  if( !(fFlags & SkipHeader) )
  { out << "*dataset:" << std::endl; }

  // get min number of entries in the columns
  int nLinesMax = (*std::min_element( fColumns.begin(), fColumns.end(), SizeLessFTor() ))->Size();
  if( nLines ) nLinesMax = std::min( nLinesMax, nLines+firstLine );

  // dump values
  for( int line = firstLine; line < nLinesMax; line++ )
  {

    bool foundSyst( false );
    bool hasSyst( false );
    bool hasStat( false );
    for( int column = 0; column < fColumns.size(); column++ )
    {
      std::string valueString( fColumns[column]->GetString(line) );

      // formats
      if( fColumns[column]->GetType() & ColumnBase::ErrorSyst && !hasSyst )
      {
        foundSyst = true;
        out << " (DSYS=";
      }

      if( fColumns[column]->GetType() & ColumnBase::ErrorPlus )
      {
        if( fColumns[column]->GetType() & ColumnBase::ErrorSyst && hasSyst ) out << "; DSYS=";
        out << " +";
      }
      else if( fColumns[column]->GetType() & ColumnBase::ErrorMinus ) out << " , -";
      else if( fColumns[column]->GetType() & ColumnBase::Error )
      {
        if( fColumns[column]->GetType() & ColumnBase::ErrorStat ) out << " +- ";
        else if( fColumns[column]->GetType() & ColumnBase::ErrorSyst && hasSyst ) out << "; DSYS=";
      }
      else if( fColumns[column]->GetType() & ColumnBase::IntervalEnd ) out << " TO ";
      else if( column > 0 ) out << "; ";

      // value
      out << valueString;

      // trailer
      if( (fColumns[column]->GetType() & (ColumnBase::Error|ColumnBase::ErrorMinus) ) &&
        ( fColumns[column]->GetType() & ColumnBase::ErrorSyst ) &&
        !fColumns[column]->GetName().IsNull() )
      { out << ":" << fColumns[column]->GetName(); }

      if( foundSyst ) hasSyst = true;

    }

    if( hasSyst ) out << ");" << std::endl;
    else out << ";" << std::endl;

  }

  if( !(fFlags & SkipTrailer) )
  { out << "*dataend:" << std::endl << std::endl; }

}
