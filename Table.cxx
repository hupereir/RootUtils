// $Id: Table.cxx,v 1.21 2010/09/15 02:27:24 hpereira Exp $

/*!
\file Table.cxx
\brief data formating
*/

#include <algorithm>
#include <fstream>
#include <iostream>
#include <sstream>

#include "Table.h"
#include "Utils.h"

using namespace std;

//_________________________________________________
//! root dictionary
ClassImp( ColumnBase );
ClassImp( Table );

//_________________________________________________________________
void Line::parse( const std::string& line_buffering )
{
    
    clear();
    
    istringstream in( line_buffering.c_str() );
    while( !(in.rdstate() & ios::failbit ) )
    {
        string value;
        in >> value;
        if( !(in.rdstate() & ios::failbit ) ) push_back( value );
    }
    
    cout << "Table::Line::parse - " << size() << " values read" << endl;
}

//_________________________________________________________________
void Table::add_conversion( const char* c1, const char* c2 )
{ if( c1 && c2 ) _conversions.push_back( conversion_pair( string( c1 ), string( c2 ) ) ); }

//_________________________________________________________________
void Table::load( const char* filename )
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
    _columns.clear();
    
    // store parsed lines
    vector< Line > lines;
    
    char line_buffer[1024];
    while( in.getline( line_buffer, 1024, '\n' ) )
    {
        
        if( !strlen( line_buffer ) ) continue;
        if( strncmp( line_buffer, "//", 2 ) == 0 ) continue;
        
        // add conversions
        string line_str( line_buffer );
        for( list< conversion_pair >::const_iterator iter = _conversions.begin(); iter != _conversions.end(); iter++ )
            line_str = Utils::replace_all( line_str, iter->first, iter->second );
        
        Line line;
        line.parse( line_str.c_str() );
        if( !line.empty() ) lines.push_back( line );
        
    }
    
    cout << "Table::load - " << lines.size() << " lines read" << endl;
    if( lines.empty() ) return;
    
    // retrieve number of columns
    unsigned int n_columns = min_element(  lines.begin(), lines.end(), SizeLessFTor() )->size();
    cout << "Table::load - number of columns: " << n_columns << endl;
    
    // fill columns
    for( unsigned int i_column=0; i_column< n_columns; i_column++ )
    {
        
        // check type
        ColumnBase* column = 0;
        if( lines[0].is<double>(i_column) ) column = new ColumnDouble();
        else column = new ColumnString();
        for( unsigned int i_line=0; i_line < lines.size(); i_line++ )
            column->add_value( lines[i_line][i_column] );
        
        _columns.push_back( column );
        
    }
    
    return;
    
}

//_________________________________________________________________
void Table::clear_conversions( void )
{ _conversions.clear(); }

//_________________________________________________________________
unsigned int Table::get_n_lines( void ) const
{ return (*std::min_element( _columns.begin(), _columns.end(), SizeLessFTor() ))->size(); }

//_________________________________________________________________
double* Table::get_double_array( const unsigned int& column, const unsigned int& first_line, const unsigned int& n_lines ) const
{
    
    // check column index
    if( !_check_column( column ) ) return 0;
    
    // try cast column
    ColumnDouble *column_double = static_cast<ColumnDouble*>(_columns[column]);
    if( !column_double )
    {
        cout << "Table::get_column_array - cannot cast column " << column << endl;
        return 0;
    }
    
    return column_double->get_array( first_line, n_lines );
}

//_________________________________________________________________
void Table::print_latex( ostream& out, const unsigned int& first_line, const unsigned int& n_lines ) const
{
    
    // dump begin of table
    if( !(_flags & SKIP_HEADER ) )
    {
        out << "\\begin{tabular}{";
        for( unsigned int column = 0; column < _columns.size(); column++ )
        {
            if( _columns[column]->type() & ColumnBase::HAS_HEADER ) {
                out << _columns[column]->alignment();
                if( column < _columns.size()-1 ) out << "|";
            }
        }
        
        out << "}" << endl;
        
        for( unsigned int column = 0; column < _columns.size(); column++ )
        {
            if( column != 0 && (_columns[column]->type() & ColumnBase::HAS_HEADER) ) out << " & " << _columns[column]->name();
            else if( column == 0 && (_columns[column]->type() & ColumnBase::HAS_HEADER) ) out << _columns[column]->name();
        }
        
        out << "\\\\" << endl;
    }
    
    out << "\\hline" << endl;
    
    // get min number of entries in the columns
    unsigned int n_lines_max = (*std::min_element( _columns.begin(), _columns.end(), SizeLessFTor() ))->size();
    if( n_lines ) n_lines_max = min( n_lines_max, n_lines+first_line );
    
    // dump values
    for( unsigned int line = first_line; line < n_lines_max; line++ )
    {
        for( unsigned int column = 0; column < _columns.size(); column++ )
        {
            string value_string( _columns[column]->get_string(line) );
            
            // convert exp to latex format
            while( value_string.find( "e-0" ) != string::npos ) value_string = Utils::convert( value_string, "e-0", "e-" );
            while( value_string.find( "e0" ) != string::npos ) value_string = Utils::convert( value_string, "e0", "e" );
            if( value_string.find( "e" ) != string::npos ) value_string = Utils::convert( value_string, "e", "\\;10^{" )+"}";
            
            // print column
            if( column == 0 ) out << "$";
            else if( _columns[column]->type() & ColumnBase::HAS_HEADER ) out << "$ & $";
            if( _columns[column]->type() == ColumnBase::INTERVAL_BEGIN ) out << "[";
            if( _columns[column]->type() == ColumnBase::INTERVAL_END ) out << " , ";
            if( _columns[column]->type() == ColumnBase::ERROR ) out << " \\pm ";
            if( _columns[column]->type() == ColumnBase::ERROR_PLUS ) out << "^{+";
            if( _columns[column]->type() == ColumnBase::ERROR_MINUS ) out << "_{-";
            out << value_string;
            if( _columns[column]->type() == ColumnBase::ERROR_PLUS ) out << "}";
            if( _columns[column]->type() == ColumnBase::ERROR_MINUS ) out << "}";
            if( _columns[column]->type() == ColumnBase::INTERVAL_END ) out << "]";
            
        }
        out << "$ \\\\" << endl;

        if( _horizontal_lines.find( line+1 ) != _horizontal_lines.end() )
        { out << "\\hline" << endl; }
    
    }

    if( !(_flags & SKIP_TRAILER ) )
    {
        out << "\\end{tabular}" << endl;
        out << endl;
    }

}

//_________________________________________________________________
void Table::print_text( ostream& out, const unsigned int& first_line, const unsigned int& n_lines ) const
{


    // dump header
    if( !(_flags & SKIP_HEADER) )
    {
        for( unsigned int column = 0; column < _columns.size(); column++ )
            if( column != 0 ) out << "   " << _columns[column]->name();
        else out << _columns[column]->name();

        out << endl;
    }

    // get min number of entries in the columns
    unsigned int n_lines_max = (*std::min_element( _columns.begin(), _columns.end(), SizeLessFTor() ))->size();
    if( n_lines ) n_lines_max = min( n_lines_max, n_lines+first_line );

    // dump values
    for( unsigned int line = first_line; line < n_lines_max; line++ )
    {
        for( unsigned int column = 0; column < _columns.size(); column++ )
        {
            string value_string( _columns[column]->get_string(line) );

            // convert exp to latex format
            while( value_string.find( "e-0" ) != string::npos ) value_string = Utils::convert( value_string, "e-0", "e-" );
            while( value_string.find( "e0" ) != string::npos ) value_string = Utils::convert( value_string, "e0", "e" );
            if( value_string.find( "e" ) != string::npos )
                value_string = Utils::convert( value_string, "e", " 10^" );

            // print column
            else if( _columns[column]->type() & ColumnBase::HAS_HEADER ) out << "   ";
            if( _columns[column]->type() == ColumnBase::INTERVAL_BEGIN ) out << "[";
            if( _columns[column]->type() == ColumnBase::INTERVAL_END ) out << " , ";
            if( _columns[column]->type() == ColumnBase::ERROR ) out << " +/- ";
            if( _columns[column]->type() == ColumnBase::ERROR_PLUS ) out << " +";
            if( _columns[column]->type() == ColumnBase::ERROR_MINUS ) out << " -";
            out << value_string;
            if( _columns[column]->type() == ColumnBase::INTERVAL_END ) out << "]";

        }
        if( !( _flags & SKIP_TRAILER ) ) { out << endl; }
    }

}

//_________________________________________________________________
void Table::print_c( ostream& out, const unsigned int& first_line, const unsigned int& n_lines ) const
{

    // dump values
    for( unsigned int column = 0; column < _columns.size(); column++ )
    {

        // get min number of entries in the columns
        unsigned int n_lines_max = (*std::min_element( _columns.begin(), _columns.end(), SizeLessFTor() ))->size();
        if( n_lines ) n_lines_max = min( n_lines_max, n_lines+first_line );

        if( n_lines_max == 1 )
        {
            out << "double " << _columns[column]->name() << " = " <<  _columns[column]->get_string(first_line) << ";" << endl;
        } else {
            out << "double " << _columns[column]->name() << "[" << n_lines_max-first_line << "] = {";
            for( unsigned int line = first_line; line < n_lines_max; line++ )
            {
                if( line != first_line ) out << ", ";
                out << _columns[column]->get_string(line);
            }
            out << "}; " << endl;
        }
    }
}
