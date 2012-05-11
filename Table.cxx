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

using namespace std;

//_________________________________________________
//! root dictionary
ClassImp( ColumnBase );
ClassImp( Table );

//_________________________________________________________________
void Line::Parse( const std::string& line_buffering )
{

    clear();

    istringstream in( line_buffering.c_str() );
    while( !(in.rdstate() & ios::failbit ) )
    {
        string value;
        in >> value;
        if( !(in.rdstate() & ios::failbit ) ) push_back( value );
    }

    cout << "Table::Line::Parse - " << size() << " values read" << endl;
}

//_________________________________________________________________
void Table::AddConversion( const char* c1, const char* c2 )
{ if( c1 && c2 ) fConversions.push_back( ConversionPair( string( c1 ), string( c2 ) ) ); }

//_________________________________________________________________
void Table::Load( const char* filename )
{
    if( !filename ) {
        cout << "Table::load - empty string." << endl;
        return;
    }

    ifstream in( filename );
    if( !in ) {
        cout << "Table::load - invalid file: " << filename << endl;
        return;
    }

    // clear columns
    fColumns.clear();

    // store Parsed lines
    vector< Line > lines;

    char line_buffer[1024];
    while( in.getline( line_buffer, 1024, '\n' ) )
    {

        if( !strlen( line_buffer ) ) continue;
        if( strncmp( line_buffer, "//", 2 ) == 0 ) continue;

        // add conversions
        string line_str( line_buffer );
        for( list< ConversionPair >::const_iterator iter = fConversions.begin(); iter != fConversions.end(); iter++ )
        { line_str = Stream::ReplaceAll( line_str, iter->first, iter->second ); }

        Line line;
        line.Parse( line_str.c_str() );
        if( !line.empty() ) lines.push_back( line );

    }

    cout << "Table::load - " << lines.size() << " lines read" << endl;
    if( lines.empty() ) return;

    // retrieve number of columns
    unsigned int nfColumns = min_element(  lines.begin(), lines.end(), SizeLessFTor() )->size();
    cout << "Table::load - number of columns: " << nfColumns << endl;

    // fill columns
    for( unsigned int i_column=0; i_column< nfColumns; i_column++ )
    {

        // check type
        ColumnBase* column = 0;
        if( lines[0].Is<double>(i_column) ) column = new ColumnDouble();
        else column = new ColumnString();
        for( unsigned int i_line=0; i_line < lines.size(); i_line++ )
            column->AddValue( lines[i_line][i_column] );

        fColumns.push_back( column );

    }

    return;

}

//_________________________________________________________________
void Table::ClearConversions( void )
{ fConversions.clear(); }

//_________________________________________________________________
unsigned int Table::GetNLines( void ) const
{ return (*std::min_element( fColumns.begin(), fColumns.end(), SizeLessFTor() ))->Size(); }

//_________________________________________________________________
double* Table::GetDoubleArray( const unsigned int& column, const unsigned int& firstLine, const unsigned int& n_lines ) const
{

    // check column index
    if( !CheckColumn( column ) ) return 0;

    // try cast column
    ColumnDouble *column_double = static_cast<ColumnDouble*>(fColumns[column]);
    if( !column_double )
    {
        cout << "Table::get_column_array - cannot cast column " << column << endl;
        return 0;
    }

    return column_double->GetArray( firstLine, n_lines );
}

//_________________________________________________________________
void Table::PrintLatex( ostream& out, const unsigned int& firstLine, const unsigned int& n_lines ) const
{

    // dump begin of table
    if( !(fFlags & SKIP_HEADER ) )
    {
        out << "\\begin{tabular}{";
        for( unsigned int column = 0; column < fColumns.size(); column++ )
        {
            if( fColumns[column]->GetType() & ColumnBase::HAS_HEADER ) {
                out << fColumns[column]->GetAlignment();
                if( column < fColumns.size()-1 ) out << "|";
            }
        }

        out << "}" << endl;

        for( unsigned int column = 0; column < fColumns.size(); column++ )
        {
            if( column != 0 && (fColumns[column]->GetType() & ColumnBase::HAS_HEADER) ) out << " & " << fColumns[column]->GetName();
            else if( column == 0 && (fColumns[column]->GetType() & ColumnBase::HAS_HEADER) ) out << fColumns[column]->GetName();
        }

        out << "\\\\" << endl;
    }

    out << "\\hline" << endl;

    // get min number of entries in the columns
    unsigned int nLinesMax = (*std::min_element( fColumns.begin(), fColumns.end(), SizeLessFTor() ))->Size();
    if( n_lines ) nLinesMax = min( nLinesMax, n_lines+firstLine );

    // dump values
    for( unsigned int line = firstLine; line < nLinesMax; line++ )
    {
        for( unsigned int column = 0; column < fColumns.size(); column++ )
        {
            string value_string( fColumns[column]->GetString(line) );

            // convert exp to latex format
            while( value_string.find( "e-0" ) != string::npos ) value_string = Stream::ReplaceAll( value_string, "e-0", "e-" );
            while( value_string.find( "e0" ) != string::npos ) value_string = Stream::ReplaceAll( value_string, "e0", "e" );
            if( value_string.find( "e" ) != string::npos ) value_string = Stream::ReplaceAll( value_string, "e", "\\;10^{" )+"}";

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
        out << "$ \\\\" << endl;

        if( fHorizontalLines.find( line+1 ) != fHorizontalLines.end() )
        { out << "\\hline" << endl; }

    }

    if( !(fFlags & SKIP_TRAILER ) )
    {
        out << "\\end{tabular}" << endl;
        out << endl;
    }

}

//_________________________________________________________________
void Table::PrintText( ostream& out, const unsigned int& firstLine, const unsigned int& n_lines ) const
{


    // dump header
    if( !(fFlags & SKIP_HEADER) )
    {
        for( unsigned int column = 0; column < fColumns.size(); column++ )
            if( column != 0 ) out << "   " << fColumns[column]->GetName();
        else out << fColumns[column]->GetName();

        out << endl;
    }

    // get min number of entries in the columns
    unsigned int nLinesMax = (*std::min_element( fColumns.begin(), fColumns.end(), SizeLessFTor() ))->Size();
    if( n_lines ) nLinesMax = min( nLinesMax, n_lines+firstLine );

    // dump values
    for( unsigned int line = firstLine; line < nLinesMax; line++ )
    {
        for( unsigned int column = 0; column < fColumns.size(); column++ )
        {
            string value_string( fColumns[column]->GetString(line) );

            // convert exp to latex format
            while( value_string.find( "e-0" ) != string::npos ) value_string = Stream::ReplaceAll( value_string, "e-0", "e-" );
            while( value_string.find( "e0" ) != string::npos ) value_string = Stream::ReplaceAll( value_string, "e0", "e" );
            if( value_string.find( "e" ) != string::npos )
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
        if( !( fFlags & SKIP_TRAILER ) ) { out << endl; }
    }

}

//_________________________________________________________________
void Table::PrintC( ostream& out, const unsigned int& firstLine, const unsigned int& n_lines ) const
{

    // dump values
    for( unsigned int column = 0; column < fColumns.size(); column++ )
    {

        // get min number of entries in the columns
        unsigned int nLinesMax = (*std::min_element( fColumns.begin(), fColumns.end(), SizeLessFTor() ))->Size();
        if( n_lines ) nLinesMax = min( nLinesMax, n_lines+firstLine );

        if( nLinesMax == 1 )
        {
            out << "double " << fColumns[column]->GetName() << " = " <<  fColumns[column]->GetString(firstLine) << ";" << endl;
        } else {
            out << "double " << fColumns[column]->GetName() << "[" << nLinesMax-firstLine << "] = {";
            for( unsigned int line = firstLine; line < nLinesMax; line++ )
            {
                if( line != firstLine ) out << ", ";
                out << fColumns[column]->GetString(line);
            }
            out << "}; " << endl;
        }
    }
}
