#ifndef Utils_h
#define Utils_h

// $Id: Utils.h,v 1.47 2010/09/15 02:27:25 hpereira Exp $

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

    //! get average for a number measurements
    static double GetMean( double* values, int n )
    {
        std::list< double > value_list;
        for( int i=0; i<n; i++ ) value_list.push_back( values[i] );
        return GetMean( value_list );
    }

    //! get rms for a number measurements
    static double GetRMS( double* values, int n )
    {
        std::list< double > value_list;
        for( int i=0; i<n; i++ ) value_list.push_back( values[i] );
        return GetRMS( value_list );
    }

    //! get average for a number measurements
    static double GetMean( std::list<double> values );

    //! get rms for a number measurements
    static double GetRMS( std::list<double> values );

    //! dump histogram
    static void DumpHistogram( TH1 *h );

    //! dump function parameters
    static void DumpFunctionParameters( TF1 *f );

    /*! \brief
    generate a random number following probability distribution given by
    histogram
    */
    static double GetRandom( TH1* h );

    /*! \brief
    generate a flat random number between 0 and max
    */
    static double GetRandom( double max )
    { return GetRandom( 0, max ); }

    /*! \brief
    generate a flat random number between min and max
    */
    static double GetRandom( double min, double max );

    /*! \brief
    generate a random number following probability distribution given by
    histogram
    */
    static double GetRandom( TF1* f, double x_min, double x_max );

    //! draw normalized histogram
    static void DrawNormalized( TTree* tree, const char* name, const char* var, const TCut& cut="", const char* opt = "" );

    //! get ideal chisquare tree for a given number of freedoms
    static TTree* GetChisquareTree(
        int ndf,
        int nevents );


    /*!
    get ideal chisquare for a given number of freedoms.
    Sums ndf+1 normalized gaussian distributions and get the chisquare
    corresponding to the average
    */
    static double GetChisquare( int ndf );

    //! fills histogram from tree return histogram
    static TH1* TreeToHisto(
        TTree *tree,
        const char* name,
        const char* var,
        TCut cut="",
        bool autoH = true );

    //! fills histogram from tree return histogram
    static TH2* TreeToHisto2D(
        TTree *tree,
        const char* name,
        const char* var,
        TCut cut="",
        bool autoH = true );

    //! Convert an histogram into a TGraph
    static TGraphErrors* HistoToTGraph( TH1* h, bool zero_sup = false );

    //! returns number of entries (debugged)
    static double GetEntries( TH1* h );

    //! scale histogram x axis
    static TH1* ScaleAxis( TH1*h, double scale );

    //! returns Integrated histogram
    static TH1* Integrate( TH1* h, bool normalize = false );

    /*!
    returns histogram Integrate between to axis values
    linearly correct from bin effects
    */
    static double Integrate( TH1* h, double xmin, double xmax );

    //! create a new histogram safely (delete histograms with same name before)
    static TH1* NewTH1(
        const char* name,
        const char* title,
        int bin = 100,
        double min = 0,
        double max = 1 );

    //! create a new histogram safely (delete histograms with same name before)
    static TH1* NewTH1(
        const char* name,
        const char* title,
        int bin,
        double *x	);

    //! create a new 2D histogram safely (delete histograms with same name before)
    static TH2* NewTH2(
        const char* name,
        const char* title,
        int binx = 100,
        double minx = 0,
        double maxx = 1,
        int biny = 100,
        double miny = 0,
        double maxy = 1 );

    //! create a new clone histogram safely (delete histograms with same name before)
    static TH1* NewClone(
        const char* name,
        const char* title,
        TH1* parent )
    { return NewClone( name, title, parent, false ); }

    //! create a new clone histogram safely (delete histograms with same name before)
    static TH1* NewClone(
        const char* name,
        const char* title,
        TH1* parent,
        bool reset );

    //! create a new clone histogram safely (delete histograms with same name before)
    static TH2* NewClone2D(
        const char* name,
        const char* title,
        TH2* parent )
    { return NewClone2D( name, title, parent, false ); }

    //! create a new clone histogram safely (delete histograms with same name before)
    static TH2* NewClone2D(
        const char* name,
        const char* title,
        TH2* parent,
        bool reset );

    //! create TF1
    /*! before creating the TF1, a matching TObject is looked after and deleted if found */
    static TF1* NewTF1(
        const char* name,
        double (*function)(double*, double*),
        const double& min, const double& max,
        const int& n_par );

    //! subtract 2 histograms bin/bin; stores the result in an third histo; returns diff of number of entries
    static int HDiff(TH1* h1, TH1* h2, TH1* h3);

    //! subtract histogram and function bin/bin; stores the result in an third histo; returns diff of number of entries
    static int HDiff(TH1* h1, TF1* f, TH1* h3, double min, double max);

    //! divides 2 histograms bin/bin; stores the result in an third histo; returns ratio of the number of entries in the 2 histo
    enum ErrorMode
    {
        EFF,
        STD
    };

    //! divides 2 histograms bin/bin; stores the result in an third histo; returns ratio of the number of entries in the 2 histo
    static double HDiv(TH1* h_found, TH1* h_ref, TH1* h3, int error_mode = EFF );

    //! divides 2 histograms bin/bin; stores the result in an third histo; returns ratio of the number of entries in the 2 histo
    static double HDiv2D(TH2* h_found, TH2* h_ref, TH2* h3, int error_mode = EFF);

    //! divides 2 histograms bin/bin whith specified range; stores the result in an third histo; returns ratio of the number of entries in the 2 histo
    static double HDiv(TH1* h_found, TH1* h_ref, TH1* h3, unsigned int i1, unsigned int i2, int error_mode = EFF);

    //! divides too TGraphs point/point; stores the result in 3 TGraph
    static TGraphErrors* TGDiv( TGraphErrors* tg_found, TGraphErrors* tg_ref );

    #ifndef __CINT__

    /*! \brief
    generate two random numbers following probability distribution given by
    histogram
    */
    static std::pair<double,double> GetRandom2D( TH2* h );

    //! fill a string until its size reached required size
    static std::string Fill( unsigned int size, const std::string& value )
    {
        return( value.size()>=size) ?
            value :
            value+std::string(size-value.size(), ' ');
    }

    //! display a text, formed, to a stream
    static void Form( std::ostream &out, const char* format, ... );
    #endif

    protected:

    #ifndef __CINT__

    //! merge all ntuples from a file into output
    static void Merge( std::vector<std::string> files, const std::string& output_file, const std::string& selection );

    #endif



    ClassDef(Utils,0)
};

#endif
