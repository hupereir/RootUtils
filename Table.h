// $Id: Table.h,v 1.15 2010/09/15 02:27:24 hpereira Exp $
#ifndef Table_h
#define Table_h

/*!
\file Table.h
\brief data formating
*/

#include <string>
#include <vector>
#include <TObject.h>
#include <iostream>
#include <sstream>

#ifndef __CINT__
#include <map>
#include <list>
#include <set>
#endif

//! data column base class
class ColumnBase
{
    public:

    //! column type
    enum ColumnType {

        NONE = 1<<0,
        ERROR = 1<<1,
        ERROR_PLUS = 1<<2,
        ERROR_MINUS = 1<<3,
        INTERVAL_BEGIN = 1<<4,
        INTERVAL_END = 1<<5,
        HAS_HEADER = INTERVAL_BEGIN | NONE
    };

    //! constructor
    ColumnBase(
        const char* name = "empty column",
        const char* format = "",
        const ColumnType& type = NONE ):
        _name( name ),
        _format( format ),
        _type( type ),
        _alignment( "l" )
    {}

    //! destructor
    virtual ~ColumnBase( void )
    {}

    //!column name
    virtual const std::string& name( void ) const
    { return _name; }

    //! column format
    virtual const std::string& format( void ) const
    { return _format; }

    //! column type
    virtual const ColumnType& type( void ) const
    { return _type; }

    //! alignment
    virtual const std::string& alignment( void ) const
    { return _alignment; }

    //! name
    virtual void set_name( const std::string& name )
    { _name = name; }

    //! format
    virtual void set_format( const std::string& format )
    { _format = format; }

    //! column type
    virtual void set_type( const ColumnType& type )
    { _type = type; }

    //! alignment
    virtual void set_alignment( const std::string& alignment )
    { _alignment = alignment; }

    //! data size
    virtual unsigned int size( void ) const
    {
        std::cout << "ColumnBase::size - not implemented" << std::endl;
        return 0;
    }

    //! scale values (using double)
    virtual void scale( const double& value )
    {
        std::cout << "ColumnBase::scale - not implemented" << std::endl;
        return;
    }

    //! scale values (using double)
    virtual void scale( double* value )
    {
        std::cout << "ColumnBase::scale - not implemented" << std::endl;
        return;
    }

    //! reduce column size
    virtual void shrink( const unsigned int& new_size )
    {
        std::cout << "ColumnBase::shrink - not implemented" << std::endl;
        return;
    }


    //! expand column size
    virtual void expand( const unsigned int& new_size )
    {
        std::cout << "ColumnBase::expand - not implemented" << std::endl;
        return;
    }

    //! print column, formated
    virtual std::string get_string( const unsigned int& index ) const
    {

        std::cout << "ColumnBase::get_string - not implemented" << std::endl;
        return "";
    }

    //! add value to vector
    virtual bool add_value( const std::string& value )
    {

        std::cout << "ColumnBase::add_value - not implemented" << std::endl;
        return false;
    }

    protected:

    //! column name
    std::string _name;

    //! column format
    std::string _format;

    //! column type
    ColumnType _type;

    //! alignment
    std::string _alignment;

    //! root dictionary
    ClassDef( ColumnBase, 0 );

};

#ifndef __CINT__

//! templatized column class
template<typename T> class Column: public ColumnBase
{

    public:

    //! constructor
    Column(
        const char* name = "empty column",
        const std::string* values = 0,
        const int& size = 0,
        const char* format = "",
        const ColumnType& type = NONE ):
        ColumnBase( name, format, type )
    { for( int i=0; i<size; i++ ) add_value( values[i] ); }

    //! destructor
    virtual ~Column( void )
    {}

    //! values
    virtual const std::vector<T>& values( void ) const
    { return _values; }

    //! values
    virtual bool add_value( const std::string& value )
    {
        T out;
        std::istringstream in( value );
        in >> out;
        if( !(in.rdstate() & std::ios::failbit) )
        {
            _values.push_back( out );
            return true;
        } else {
            T tmp;
            _values.push_back( tmp );
            return false;
        }
    }

    //! retrieve c array from column
    /*!
    note that a new array is created at each call
    and needs to be deleted from the calling method
    */
    virtual T* get_array( const unsigned int& first_line = 0, const unsigned int& n_lines = 0 ) const
    {
        // get min number of entries in the columns
        unsigned int n_lines_max = ( n_lines )? min( _values.size(), n_lines+first_line ):_values.size();
        T* out = new T[n_lines_max-first_line];
        for( unsigned int i=first_line; i<n_lines_max; i++ ) out[i-first_line] = _values[i];
        return out;

    }

    //! return data size
    virtual unsigned int size( void ) const
    { return _values.size(); }

    //! shrink column size
    virtual void shrink( const unsigned int& new_size )
    {
        while( _values.size() > new_size )
            _values.pop_back();
        return;
    }

    //! expand column size
    virtual void expand( const unsigned int& new_size )
    {
        T value;
        if( !_values.empty() ) value = _values.back();
        for( unsigned int i= _values.size(); i<new_size; i++ )
            _values.push_back( value );
        return;
    }

//     //! get_string column, formated
//     virtual std::string get_string( const unsigned int& index ) const
//     {
//
//         char out[512];
//         sprintf( out, _format.c_str(), _values[index] );
//         return std::string( out );
//     }

    protected:

    //! vector values
    std::vector<T> _values;

};

//! column of doubles
class ColumnDouble: public Column<double>
{

    public:

    //! constructor
    ColumnDouble(
        const char* name = "",
        const std::string* values = 0,
        const int& size = 0,
        const char* format = "%f",
        const ColumnType& type = NONE ):
        Column<double>( name, values, size, format, type )
    {}

    //! constructor
    ColumnDouble(
        const char* name,
        const double* values = 0,
        const int& size = 0,
        const char* format = "%f",
        const ColumnType& type = NONE ):
        Column<double>( name, 0, 0, format, type )
    {
        for( int i=0; i<size; i++ )
        { _values.push_back( values[i] ); }
    }


    //! destructor
    virtual ~ColumnDouble( void )
    {}

    //! scale all values
    virtual void scale( const double& value )
    {
        for( unsigned int i=0; i<_values.size(); i++ )
        { _values[i]*=value; }
    }

    //! scale all values
    virtual void scale( double* value )
    {
        for( unsigned int i=0; i<_values.size(); i++ )
        { _values[i]*=value[i]; }
    }

};

//! column of strings
class ColumnString: public Column<std::string>
{

    public:

    //! constructor
    ColumnString(
        const char* name = "",
        const std::string* values = 0,
        const int& size = 0,
        const char* format = "%s",
        const ColumnType& type = NONE ):
        Column<std::string>( name, values, size, format, type )
    {}

    //! constructor
    ColumnString(
        const char* name,
        const char* values[],
        const int& size = 0,
        const char* format = "%s",
        const ColumnType& type = NONE ):
        Column<std::string>( name, 0, 0, format, type )
    {
        for( int i=0; i<size; i++ )
        { _values.push_back( std::string(values[i]) ); }
    }

    //! destructor
    virtual ~ColumnString( void )
    {}

    //! get_string column, formated
    virtual std::string get_string( const unsigned int& index ) const
    { return _values[index]; }

};

#endif

//! line objects, used to parse tables from file
class Line: public std::vector<std::string>
{
    public:

    //! constructor
    Line( void ):
        std::vector<std::string>()
    {}

    //! parse values from input line
    void parse( const std::string& line_string );

    #ifndef __CINT__

    //! return true if line is of type T
    template<typename T>
        bool is( const unsigned int& i ) const
    {
        if( i>= size() ) return false;
        T out;
        std::istringstream in( (*this)[i] );
        in >> out;
        return !(in.rdstate() & std::ios::failbit );
    }

    //! cast ith element to type T
    template<typename T>
        T get( const unsigned int& i ) const
    {
        T out;
        if( i>= size() ) return out;
        std::istringstream in( (*this)[i] );
        in >> out;
        return out;
    }
    #endif


};

class Table: public TObject
{

    public:

    enum Flags {
        NONE = 0,
        SKIP_HEADER = 1<<0,
        SKIP_TRAILER = 1<<1,
        SKIP_ALL = SKIP_HEADER|SKIP_TRAILER
    };

    //! constructor
    Table( void ):
        _flags( NONE )
    {}

    //! destructor
    ~Table( void )
    { clear(); }

    //! used to sort column according to their size
    class SizeLessFTor
    {
        public:

        bool operator ()( const ColumnBase* first, const ColumnBase* second )
        { return first->size() < second->size(); }

        bool operator()( const Line& first, const Line& second )
        { return first.size() < second.size(); }
    };

    //!@name table filling
    //@{

    //! flags
    void set_flags( const unsigned int& flags )
    { _flags = flags; }

    //! add conversion pairs
    void add_conversion( const char* c1, const char* c2 );

    //! reset conversions
    void clear_conversions( void );

    //! reset columns
    void clear( void )
    {
        for( std::vector< ColumnBase* >::iterator iter = _columns.begin(); iter != _columns.end(); iter++ )
            if( *iter ) delete *iter;
        _columns.clear();
    }

    //! load table from a txt file
    void load( const char* filename );

    //! Add a column
    void add_column(
        const char* name,
        const double* values,
        const int& size,
        const char* format = "%f",
        const ColumnBase::ColumnType& type = ColumnBase::NONE )
    { _columns.push_back( new ColumnDouble( name, values, size, format, type ) ); }


    //! Add a column
    void add_string_column(
        const char* name,
        const char* values[],
        const int& size,
        const char* format = "%f",
        const ColumnBase::ColumnType& type = ColumnBase::NONE )
    { _columns.push_back( new ColumnString( name, values, size, format, type ) ); }

    //! add a line

    //! Add a column
    void add_error_column(
        const char* name,
        const double* values,
        const int& size,
        const char* format = "%f" )
    { _columns.push_back( new ColumnDouble( name, values, size, format, ColumnBase::ERROR ) ); }

    //@}


    //!@name table formatting
    //@{

    //! set column name
    void set_column_name( const unsigned int& column, const char* name )
    {
        if( name && !_check_column( column ) ) return;
        _columns[column]->set_name( name );
    }

    //! set column name
    void set_column_format( const unsigned int& column, const char* format )
    {
        if( format && !_check_column( column ) ) return;
        _columns[column]->set_format( format );
    }

    //! set column type
    void set_column_type( const unsigned int& column, const ColumnBase::ColumnType& type )
    {
        if( type && !_check_column( column ) ) return;
        _columns[column]->set_type( type );
    }

    //! set column alignment
    void set_column_alignment( const unsigned int& column, const char* alignment )
    {
        if( alignment && !_check_column( column ) ) return;
        _columns[column]->set_alignment( alignment );
    }

    //! scale column
    void scale_column( const unsigned int& column, const double& value )
    {
        if( !_check_column( column ) ) return;
        _columns[column]->scale( value );
    }

    //! scale column
    void scale_column( const unsigned int& column, double* values )
    {
        if( !_check_column( column ) ) return;
        _columns[column]->scale( values );
    }

    //! scale column (replace first by first x second)
    void multiply_column( const unsigned int& first, const unsigned int& second )
    {
        if( !_check_column( first ) ) return;
        if( !_check_column( second ) ) return;
        double *value = get_double_array( second );
        _columns[first]->scale( value );
    }

    //! expand column with its last value so that its size is the new_size
    void expand_column( const unsigned int& column, const unsigned int& new_size )
    {
        if( !_check_column( column ) ) return;
        _columns[column]->expand( new_size );
    }
    //@}

    //!@name table size
    //@{

    //! get number of columns
    unsigned int get_n_columns( void ) const
    { return _columns.size(); }

    //! get number of lines
    unsigned int get_n_lines( void ) const;

    //@}

    //!@name dumpers
    //@{

    //! retrieve column double values
    double* get_double_array( const unsigned int& column, const unsigned int& first_line = 0, const unsigned int& n_lines = 0 ) const;

    //! print table in latex format
    void print_latex( const unsigned int& first_line = 0, const unsigned int& n_line = 0 ) const
    { print_latex( std::cout, first_line, n_line ); }

    //! print table in latex format
    void print_latex( std::ostream& out, const unsigned int& first_line = 0, const unsigned int& n_line = 0 ) const;

    //! print table in text format
    void print_text( const unsigned int& first_line = 0, const unsigned int& n_line = 0 ) const
    { print_text( std::cout, first_line, n_line ); }

    //! print table in text format
    void print_text( std::ostream& out, const unsigned int& first_line = 0, const unsigned int& n_line = 0 ) const;

    //! print table in c format
    void print_c( const unsigned int& first_line = 0, const unsigned int& n_line = 0 ) const
    { print_c( std::cout, first_line, n_line ); }

    //! print table in c format
    void print_c( std::ostream& out, const unsigned int& first_line = 0, const unsigned int& n_line = 0 ) const;

    //@}

    private:

    //! check column
    bool _check_column( const unsigned int& column ) const
    {
        bool out = column < _columns.size();
        if( !out ) std::cout << "Table::_check_column - invalid column: " << column << std::endl;
        return out;
    }

    //! flags
    unsigned int _flags;

    //! list of columns
    std::vector< ColumnBase* > _columns;

    #ifndef __CINT__

    //! index of horizontal lines to be added
    std::set< int > _horizontal_lines;

    //! conversions
    typedef std::pair<std::string, std::string> conversion_pair;

    //! conversion
    std::list< conversion_pair > _conversions;

    #endif

    //! root dictionary
    ClassDef( Table, 0 );

};

#endif
