// $Id: Utils.h,v 1.47 2010/09/15 02:27:25 hpereira Exp $
#ifndef Utils_h
#define Utils_h

/*!
\file Utils.h
\brief some root utilities to handle histograms safely
\author	Hugo Pereira
\version $Revision: 1.47 $
\date $Date: 2010/09/15 02:27:25 $
*/

#include <TROOT.h>
#include <TObject.h>
#include <TCut.h>
#include <TCanvas.h>
#include <TVirtualPad.h>
#include <cstdarg>
#include <list>
#include <iostream>

#ifndef __CINT__
#include <map>
#include <vector>
#include <sstream>
#include <string>
#endif

//#include <algorithm>
class TChain;
class TTree;
class TF1;
class TH1;
class TH2;
class TText;
class TGraphErrors;

/*!
\class Utils
\brief some root utilities to handle histograms safely
*/
class Utils:public TObject
{

    public:

    //! constructor (doing nothing, needed for root)
    Utils():TObject()
    {}

    //! string replacement
    static std::string replace_all( const std::string& in, const std::string& c0, const std::string& c1 );

    //! print vector
    static void print_vector(
        const char* name,
        const double* values,
        const int& size,
        const char* format = "%f" )
    { print_vector( std::cout, name, values, size, format ); }

    //! print vector
    static void print_vector(
        std::ostream& out,
        const char* name,
        const double* values,
        const int& size,
        const char* format = "%f" );

    //! print vector
    static void print_int_vector(
        const char* name,
        const int* values,
        const int& size,
        const char* format = "%i" )
    { print_int_vector( std::cout, name, values, size, format ); }

    //! print vector
    static void print_int_vector(
        std::ostream& out,
        const char* name,
        const int* values,
        const int& size,
        const char* format = "%i" );

    //! print vector
    static void print_vector_2d(
        std::ostream& out,
        const char* name,
        const double* values,
        const int& size_1,
        const int& size_2,
        const char* format = "%f" );

    //! print vector
    static void print_vector_2d(
        const char* name,
        const double* values,
        const int& size_1,
        const int& size_2,
        const char* format = "%f" )
    { print_vector_2d( std::cout, name, values, size_1, size_2, format ); }

    //! print vector
    static void print_int_vector_2d(
        const char* name,
        const int* values,
        const int& size_1,
        const int& size_2,
        const char* format = "%i" )
    { print_int_vector_2d( std::cout, name, values, size_1, size_2, format ); }

    //! print vector
    static void print_int_vector_2d(
        std::ostream& out,
        const char* name,
        const int* values,
        const int& size_1,
        const int& size_2,
        const char* format = "%i" );

    //! print vector
    static void print_vector_3d(
        std::ostream& out,
        const char* name,
        const double* values,
        const int& size_1,
        const int& size_2,
        const int& size_3,
        const char* format = "%f" );

    //! print vector
    static void print_vector_3d(
        const char* name,
        const double* values,
        const int& size_1,
        const int& size_2,
        const int& size_3,
        const char* format = "%f" )
    { print_vector_3d( std::cout, name, values, size_1, size_2, size_3, format ); }

    //! print vector
    static void print_vector_4d(
        const char* name,
        const double* values,
        const int& size_1,
        const int& size_2,
        const int& size_3,
        const int& size_4,
        const char* format = "%f" )
    { print_vector_4d( std::cout, name, values, size_1, size_2, size_3, size_4, format ); }

    //! print vector
    static void print_vector_4d(
        std::ostream& out,
        const char* name,
        const double* values,
        const int& size_1,
        const int& size_2,
        const int& size_3,
        const int& size_4,
        const char* format = "%f" );

    //! print cuts, formated
    static void print_cuts( const TCut& cut );

    //! get average for a number measurements
    static double get_mean( double* values, int n )
    {
        std::list< double > value_list;
        for( int i=0; i<n; i++ ) value_list.push_back( values[i] );
        return get_mean( value_list );
    }

    //! get rms for a number measurements
    static double get_rms( double* values, int n )
    {
        std::list< double > value_list;
        for( int i=0; i<n; i++ ) value_list.push_back( values[i] );
        return get_rms( value_list );
    }

    //! get average for a number measurements
    static double get_mean( std::list<double> values );

    //! get rms for a number measurements
    static double get_rms( std::list<double> values );

    //! dump histogram
    static void dump_histogram( TH1 *h );

    //! dump function parameters
    static void dump_function_parameters( TF1 *f );

    /*! \brief
    generate a random number following probability distribution given by
    histogram
    */
    static double get_random( TH1* h );

    /*! \brief
    generate a flat random number between 0 and max
    */
    static double get_random( double max )
    { return get_random( 0, max ); }

    /*! \brief
    generate a flat random number between min and max
    */
    static double get_random( double min, double max );

    /*! \brief
    generate a random number following probability distribution given by
    histogram
    */
    static double get_random( TF1* f, double x_min, double x_max );

    //! draw normalized histogram
    static void draw_normalized( TTree* tree, const char* name, const char* var, const TCut& cut="", const char* opt = "" );

    //! get ideal chisquare tree for a given number of freedoms
    static TTree* get_chisquare_tree(
        int ndf,
        int nevents );


    /*!
    get ideal chisquare for a given number of freedoms.
    Sums ndf+1 normalized gaussian distributions and get the chisquare
    corresponding to the average
    */
    static double get_chisquare( int ndf );

    //! fills histogram from tree return histogram
    static TH1* tree_to_histo(
        TTree *tree,
        const char* name,
        const char* var,
        TCut cut="",
        bool autoH = true );

    //! fills histogram from tree return histogram
    static TH2* tree_to_histo_2d(
        TTree *tree,
        const char* name,
        const char* var,
        TCut cut="",
        bool autoH = true );

    //! convert an histogram into a TGraph
    static TGraphErrors* histo_to_tgraph( TH1* h, bool zero_sup = false );

    //! returns number of entries (debugged)
    static double get_entries( TH1* h );

    //! scale histogram x axis
    static TH1* scale_axis( TH1*h, double scale );

    //! returns integrated histogram
    static TH1* integrate( TH1* h, bool normalize = false );

    /*!
    returns histogram integral between to axis values
    linearly correct from bin effects
    */
    static double integral( TH1* h, double xmin, double xmax );

    //! create a new histogram safely (delete histograms with same name before)
    static TH1* new_th1(
        const char* name,
        const char* title,
        int bin = 100,
        double min = 0,
        double max = 1 );

    //! create a new histogram safely (delete histograms with same name before)
    static TH1* new_th1(
        const char* name,
        const char* title,
        int bin,
        double *x	);

    //! create a new 2D histogram safely (delete histograms with same name before)
    static TH2* new_th2(
        const char* name,
        const char* title,
        int binx = 100,
        double minx = 0,
        double maxx = 1,
        int biny = 100,
        double miny = 0,
        double maxy = 1 );

    //! create a new clone histogram safely (delete histograms with same name before)
    static TH1* new_clone(
        const char* name,
        const char* title,
        TH1* parent )
    { return new_clone( name, title, parent, false ); }

    //! create a new clone histogram safely (delete histograms with same name before)
    static TH1* new_clone(
        const char* name,
        const char* title,
        TH1* parent,
        bool reset );

    //! create a new clone histogram safely (delete histograms with same name before)
    static TH2* new_clone_2d(
        const char* name,
        const char* title,
        TH2* parent )
    { return new_clone_2d( name, title, parent, false ); }

    //! create a new clone histogram safely (delete histograms with same name before)
    static TH2* new_clone_2d(
        const char* name,
        const char* title,
        TH2* parent,
        bool reset );

    //! create TF1
    /*! before creating the TF1, a matching TObject is looked after and deleted if found */
    static TF1* new_tf1(
        const char* name,
        double (*function)(double*, double*),
        const double& min, const double& max,
        const int& n_par );

    //! subtract 2 histograms bin/bin; stores the result in an third histo; returns diff of number of entries
    static int h_diff(TH1* h1, TH1* h2, TH1* h3);

    //! subtract histogram and function bin/bin; stores the result in an third histo; returns diff of number of entries
    static int h_diff(TH1* h1, TF1* f, TH1* h3, double min, double max);

    //! divides 2 histograms bin/bin; stores the result in an third histo; returns ratio of the number of entries in the 2 histo
    enum ErrorMode
    {
        EFF,
        STD
    };

    //! divides 2 histograms bin/bin; stores the result in an third histo; returns ratio of the number of entries in the 2 histo
    static double h_div(TH1* h_found, TH1* h_ref, TH1* h3, int error_mode = EFF );

    //! divides 2 histograms bin/bin; stores the result in an third histo; returns ratio of the number of entries in the 2 histo
    static double h_div_2d(TH2* h_found, TH2* h_ref, TH2* h3, int error_mode = EFF);

    //! divides 2 histograms bin/bin whith specified range; stores the result in an third histo; returns ratio of the number of entries in the 2 histo
    static double h_div(TH1* h_found, TH1* h_ref, TH1* h3, unsigned int i1, unsigned int i2, int error_mode = EFF);

    //! divides too TGraphs point/point; stores the result in 3 TGraph
    static TGraphErrors* tg_div( TGraphErrors* tg_found, TGraphErrors* tg_ref );

    #ifndef __CINT__

    //! returns a null terminated string where all occurences of c1 is replaced with c2
    static std::string convert( const std::string& in, const std::string& c1, const std::string& c2 );

    /*! \brief
    generate two random numbers following probability distribution given by
    histogram
    */
    static std::pair<double,double> get_random_2d( TH2* h );

    //! fill a string until its size reached required size
    static std::string fill( unsigned int size, const std::string& value )
    {
        return( value.size()>=size) ?
            value :
            value+std::string(size-value.size(), ' ');
    }

    //! display a text, formed, to a stream
    static void form( std::ostream &out, const char* format, ... );
    #endif

    private:

    #ifndef __CINT__

    //! merge all ntuples from a file into output
    static void _merge( std::vector<std::string> files, const std::string& output_file, const std::string& selection );

    //! print vector
    template<typename T>
        static void _print_vector(
        std::ostream& out,
        const char* type,
        const char* name,
        const T* values,
        const int& size,
        const char* format = "%f" )
    {
        out << type << " " << name << "[] = {";
        char value[20];
        for( int i=0; i<size; i++ )
        {
            sprintf( value, format, values[i] );
            out << value;
            if( i != size-1 ) out << ", ";
            else out << "};";
        }

        out << std::endl;
    }

    //! print vector
    template<typename T>
        static void _print_vector_2d(
        std::ostream& out,
        const char* type,
        const char* name,
        const T* values,
        const int& size_1,
        const int& size_2,
        const char* format = "%f" )
    {
        out << type << " " << name << "[" << size_1 << "][" << size_2 << "] = {";
        if( size_1 > 1 ) out << std::endl;
        char value[20];
        for( int i=0; i<size_1; i++ )
        {
            out << "  {";
            for( int j=0; j<size_2; j++ )
            {
                sprintf( value, format, (double) values[i*size_2+j] );
                out << value;
                if( j != size_2-1 ) out << ", ";
                else out << "}";
            }
            if( i != size_1-1 ) out << "," << std::endl;
            else out << " };" << std::endl;
        }

        return;
    }


    //! print vector 3D
    template<typename T> static void _print_vector_3d(
        std::ostream& out,
        const char* type,
        const char* name,
        const T* values,
        const int& size_1,
        const int& size_2,
        const int& size_3,
        const char* format = "%f" )
    {
        out << type << " " << name << "[" << size_1 << "][" << size_2 << "][" << size_3 << "] = {" << std::endl;
        char value[20];
        for( int i=0; i<size_1; i++ )
        {
            out << "  {";
            for( int j=0; j<size_2; j++ )
            {
                out << "  {";
                for( int k=0; k<size_3; k++ )
                {
                    sprintf( value, format, (double) values[(i*size_2+j)*size_3+k] );
                    out << value;
                    if( k != size_3-1 ) out << ", ";
                    else out << "}";
                }
                if( j != size_2-1 ) out << ", ";
                else out << "}";
            }
            if( i != size_1-1 ) out << "," << std::endl;
            else out << " };" << std::endl;
        }

        return;
    }


    //! print vector 4D
    template<typename T> static void _print_vector_4d(
        std::ostream& out,
        const char* type,
        const char* name,
        const T* values,
        const int& size_1,
        const int& size_2,
        const int& size_3,
        const int& size_4,
        const char* format = "%f" )
    {
        out << type << " " << name << "[" << size_1 << "][" << size_2 << "][" << size_3 << "][" << size_4 << "] = {" << std::endl;
        char value[20];
        for( int i=0; i<size_1; i++ )
        {
            out << "  {" << std::endl;
            for( int j=0; j<size_2; j++ )
            {
                out << "    { ";
                for( int k=0; k<size_3; k++ )
                {
                    out << "{";
                    for( int l=0; l<size_4; l++ )
                    {
                        sprintf( value, format, (double) values[((i*size_2+j)*size_3+k)*size_4+l] );
                        out << value;
                        if( l != size_4-1 ) out << ", ";
                        else out << " }";
                    }

                    if( k != size_3-1 ) out << ", ";
                    else out << "    }";
                }
                if( j != size_2-1 ) out << ", " << std::endl;
                else out << std::endl << "  }";
            }
            if( i != size_1-1 ) out << "," << std::endl;
            else out << std::endl << "};" << std::endl;
        }

        return;
    }


    #endif



    ClassDef(Utils,0)
};

#endif
