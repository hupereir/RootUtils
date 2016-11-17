#ifndef Table_h
#define Table_h

#include <string>
#include <vector>
#include <TObject.h>
#include <TString.h>

#include <iostream>
#include <sstream>

#include <cmath>

#ifndef __CINT__
#include <map>
#include <list>
#include <set>
#endif

//* data column base class
class ColumnBase
{
    public:

    //* column type
    enum ColumnType {

        None = 1<<0,
        Error = 1<<1,
        ErrorPlus = 1<<2,
        ErrorMinus = 1<<3,
        IntervalBegin = 1<<4,
        IntervalEnd = 1<<5,
        ErrorStat = 1<<6,
        ErrorSyst = 1<<7,
        ErrorRel = 1<<8,
        HasHeader = IntervalBegin | None
    };

    //* constructor
    ColumnBase(
        const char* name = "empty column",
        const char* format = "",
        int type = None ):
        fName( name ),
        fFormat( format ),
        fType( type ),
        fAlignment( "c" )
    {}

    //* destructor
    virtual ~ColumnBase( void )
    {}

    //*column name
    virtual const TString& GetName( void ) const
    { return fName; }

    //* column format
    virtual const TString& GetFormat( void ) const
    { return fFormat; }

    //* column type
    virtual int GetType( void ) const
    { return fType; }

    //* alignment
    virtual const TString& GetAlignment( void ) const
    { return fAlignment; }

    //* name
    virtual void SetName( const TString& name )
    { fName = name; }

    //* format
    virtual void SetFormat( const TString& format )
    { fFormat = format; }

    //* column type
    virtual void SetType( int type )
    { fType = type; }

    //* alignment
    virtual void SetAlignment( const TString& alignment )
    { fAlignment = alignment; }

    //* data size
    virtual int Size( void ) const
    {
        std::cout << "ColumnBase::size - not implemented" << std::endl;
        return 0;
    }

    //* scale values (using double)
    virtual void Scale( double value )
    {
        std::cout << "ColumnBase::scale - not implemented" << std::endl;
        return;
    }

    //* scale values (using double)
    virtual void Scale( double* value )
    {
        std::cout << "ColumnBase::scale - not implemented" << std::endl;
        return;
    }

    //* reduce column size
    virtual void Shrink( int new_size )
    {
        std::cout << "ColumnBase::shrink - not implemented" << std::endl;
        return;
    }


    //* expand column size
    virtual void Expand( int new_size )
    {
        std::cout << "ColumnBase::expand - not implemented" << std::endl;
        return;
    }

    //* print column, formated
    virtual TString GetString( int index ) const
    {

        std::cout << "ColumnBase::GetString - not implemented" << std::endl;
        return "";
    }

    //* add value to vector
    virtual bool AddValue( const TString& value )
    {

        std::cout << "ColumnBase::AddValue - not implemented" << std::endl;
        return false;
    }

    protected:

    //* column name
    TString fName;

    //* column format
    TString fFormat;

    //* column type
    int fType;

    //* alignment
    TString fAlignment;

    //* root dictionary
    ClassDef( ColumnBase, 0 );

};

#ifndef __CINT__

//* templatized column class
template<typename T> class Column: public ColumnBase
{

    public:

    //* constructor
    Column(
        const char* name = "empty column",
        const TString* values = 0,
        int size = 0,
        const char* format = "",
        int type = None ):
        ColumnBase( name, format, type )
    { for( int i=0; i<size; i++ ) AddValue( values[i] ); }

    //* destructor
    virtual ~Column( void )
    {}

    //* values
    virtual const std::vector<T>& GetValues( void ) const
    { return fValues; }

    //* values
    virtual bool AddValue( const TString& value )
    {
        T out;
        std::istringstream in( value.Data() );
        in >> out;
        if( !(in.rdstate() & std::ios::failbit) )
        {
            fValues.push_back( out );
            return true;
        } else {
            T tmp;
            fValues.push_back( tmp );
            return false;
        }
    }

    //* retrieve c array from column
    /*!
    note that a new array is created at each call
    and needs to be deleted from the calling method
    */
    virtual T* GetArray( int firstLine = 0, int nLines = 0 ) const
    {

      // get min number of entries in the columns
        int nLinesMax = ( nLines )? std::min<int>( fValues.size(), nLines+firstLine ):fValues.size();
        T* out = new T[nLinesMax-firstLine];

        for( int i = firstLine; i < nLinesMax; i++ )
        { out[i-firstLine] = fValues[i]; }

        return out;

    }

    //* return data size
    virtual int Size( void ) const
    { return fValues.size(); }

    //* shrink column size
    virtual void Shrink( int new_size )
    {
        while( fValues.size() > new_size )
            fValues.pop_back();
        return;
    }

    //* expand column size
    virtual void Expand( int new_size )
    {
        T value;
        if( !fValues.empty() ) value = fValues.back();
        for( int i= fValues.size(); i<new_size; i++ )
            fValues.push_back( value );
        return;
    }

    protected:

    //* vector values
    std::vector<T> fValues;

};

//* column of doubles
class ColumnDouble: public Column<double>
{

    public:

    //* constructor
    ColumnDouble(
        const char* name = "",
        const TString* values = 0,
        int size = 0,
        const char* format = "%f",
        int type = None ):
        Column<double>( name, values, size, format, type )
    {}

    //* constructor
    ColumnDouble(
        const char* name,
        const double* values = 0,
        int size = 0,
        const char* format = "%f",
        int type = None ):
        Column<double>( name, 0, 0, format, type )
    {
        for( int i=0; i<size; i++ )
        { fValues.push_back( values[i] ); }
    }


    //* destructor
    virtual ~ColumnDouble( void )
    {}

    //* scale all values
    virtual void Scale( double value )
    {
        for( int i=0; i<fValues.size(); i++ )
        { fValues[i]*=value; }
    }

    //* scale all values
    virtual void Scale( double* value )
    {
        for( int i=0; i<fValues.size(); i++ )
        { fValues[i]*=value[i]; }
    }

    //* print column, formated
    virtual TString GetString( int index ) const
    { return Form( GetFormat().Data(), fValues[index] ); }


};

//* column of strings
class ColumnString: public Column<TString>
{

    public:

    //* constructor
    ColumnString(
        const char* name = "",
        const TString* values = 0,
        int size = 0,
        const char* format = "%s",
        int type = None ):
        Column<TString>( name, values, size, format, type )
    {}

    //* constructor
    ColumnString(
        const char* name,
        const char* values[],
        int size = 0,
        const char* format = "%s",
        int type = None ):
        Column<TString>( name, 0, 0, format, type )
    {
        for( int i=0; i<size; i++ )
        { fValues.push_back( TString(values[i]) ); }
    }

    //* destructor
    virtual ~ColumnString( void )
    {}

    //* GetString column, formated
    virtual TString GetString( int index ) const
    { return fValues[index]; }

};

#endif

//* line objects, used to Parse tables from file
class Line: public std::vector<TString>
{
    public:

    //* constructor
    Line( void ):
        std::vector<TString>()
    {}

    //* Parse values from input line
    void Parse( const TString& line_string );

    #ifndef __CINT__

    //* return true if line is of type T
    template<typename T>
        bool Is( int i ) const
    {
        if( i>= size() ) return false;
        T out;
        std::istringstream in( (*this)[i].Data() );
        in >> out;
        return !(in.rdstate() & std::ios::failbit );
    }

    //* cast ith element to type T
    template<typename T>
        T Get( int i ) const
    {
        T out;
        if( i>= size() ) return out;
        std::istringstream in( (*this)[i] );
        in >> out;
        return out;
    }
    #endif


};

//* table class
class Table: public TObject
{

    public:

    enum Flags {
        None = 0,
        SkipHeader = 1<<0,
        SkipTrailer = 1<<1,
        SkipAll = SkipHeader|SkipTrailer
    };

    //* constructor
    Table( void ):
        fFlags( None )
    {}

    //* destructor
    ~Table( void )
    { Clear(); }

    //* used to sort column according to their size
    class SizeLessFTor
    {
        public:

        bool operator ()( const ColumnBase* first, const ColumnBase* second )
        { return first->Size() < second->Size(); }

        bool operator()( const Line& first, const Line& second )
        { return first.size() < second.size(); }
    };

    //*@name table filling
    //@{

    //* flags
    void SetFlags( int flags )
    { fFlags = flags; }

    //* add conversion pairs
    void AddConversion( const char* c1, const char* c2 );

    //* reset conversions
    void ClearConversions( void );

    //* reset columns
    void Clear( void )
    {
        for( std::vector< ColumnBase* >::iterator iter = fColumns.begin(); iter != fColumns.end(); iter++ )
        { if( *iter ) delete *iter; }
        fColumns.clear();
    }

    //* load table from a txt file
    void Load( const char* filename );

    //* Add a column
    void AddColumn(
        const char* name,
        const double* values,
        int size,
        const char* format = "%f",
        int type = ColumnBase::None )
    { fColumns.push_back( new ColumnDouble( name, values, size, format, type ) ); }


    //* Add a column
    void AddStringColumn(
        const char* name,
        const char* values[],
        int size,
        const char* format = "%f",
        int type = ColumnBase::None )
    { fColumns.push_back( new ColumnString( name, values, size, format, type ) ); }

    //* Add a column
    void AddErrorColumn(
        const char* name,
        const double* values,
        int size,
        const char* format = "%f" )
    { fColumns.push_back( new ColumnDouble( name, values, size, format, ColumnBase::Error ) ); }

    //@}


    //*@name table formatting
    //@{

    //* set column name
    void SetColumnName( int column, const char* name )
    {
        if( name && !CheckColumn( column ) ) return;
        fColumns[column]->SetName( name );
    }

    //* set column name
    void SetColumnFormat( int column, const char* format )
    {
        if( format && !CheckColumn( column ) ) return;
        fColumns[column]->SetFormat( format );
    }

    //* set column type
    void SetColumnType( int column, int type )
    {
        if( type && !CheckColumn( column ) ) return;
        fColumns[column]->SetType( type );
    }

    //* set column alignment
    void SetColumnAlignment( int column, const char* alignment )
    {
        if( alignment && !CheckColumn( column ) ) return;
        fColumns[column]->SetAlignment( alignment );
    }

    //* scale column
    void ScaleColumn( int column, double value )
    {
        if( !CheckColumn( column ) ) return;
        fColumns[column]->Scale( value );
    }

    //* scale column
    void ScaleColumn( int column, double* values )
    {
        if( !CheckColumn( column ) ) return;
        fColumns[column]->Scale( values );
    }

    //* scale last column
    void ScaleLastColumn( double value )
    { ScaleColumn( fColumns.size()-1, value ); }

    //* scale last column
    void ScaleLastColumn( double* values )
    { ScaleColumn( fColumns.size()-1, values ); }

    //* scale column (replace first by first x second)
    void MultiplyColumn( int first, int second )
    {
        if( !CheckColumn( first ) ) return;
        if( !CheckColumn( second ) ) return;
        double *value = GetDoubleArray( second );
        fColumns[first]->Scale( value );
    }

    //* expand column with its last value so that its size is the new_size
    void ExpandColumn( int column, int new_size )
    {
        if( !CheckColumn( column ) ) return;
        fColumns[column]->Expand( new_size );
    }
    //@}

    //*@name table size
    //@{

    //* get number of columns
    int GetNColumns( void ) const
    { return fColumns.size(); }

    //* get number of lines
    int GetNLines( void ) const;

    //@}

    //*@name dumpers
    //@{

    //* retrieve column double values
    double* GetDoubleArray( int column, int firstLine = 0, int nLines = 0 ) const;

    //* print table in latex format
    void PrintLatex( int firstLine = 0, int n_line = 0 ) const
    { PrintLatex( std::cout, firstLine, n_line ); }

    //* print table in latex format
    void PrintLatex( std::ostream& out, int firstLine = 0, int n_line = 0 ) const;

    //* print table in text format
    void PrintText( int firstLine = 0, int n_line = 0 ) const
    { PrintText( std::cout, firstLine, n_line ); }

    //* print table in text format
    void PrintText( std::ostream& out, int firstLine = 0, int n_line = 0 ) const;

    //* print table in c format
    void PrintC( int firstLine = 0, int n_line = 0 ) const
    { PrintC( std::cout, firstLine, n_line ); }

    //* print table in c format
    void PrintC( std::ostream& out, int firstLine = 0, int n_line = 0 ) const;

    //* print tabe in hep format
    void PrintHep( int firstLine = 0, int n_line = 0 ) const
    { PrintHep( std::cout, firstLine, n_line ); }

    //* print table in c format
    void PrintHep( std::ostream& out, int firstLine = 0, int n_line = 0 ) const;

    //@}

    protected:

    //* check column
    bool CheckColumn( int column ) const
    {
        bool out = column < fColumns.size();
        if( !out ) std::cout << "Table::CheckColumn - invalid column: " << column << std::endl;
        return out;
    }

    private:

    //* flags
    int fFlags;

    //* list of columns
    std::vector< ColumnBase* > fColumns;

    #ifndef __CINT__

    //* index of horizontal lines to be added
    std::set< int > fHorizontalLines;

    //* conversions
    typedef std::pair<TString, TString> ConversionPair;

    //* conversion
    std::list< ConversionPair > fConversions;

    #endif

    //* root dictionary
    ClassDef( Table, 0 );

};

#endif
