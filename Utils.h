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
#include <TString.h>

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
class TH3;
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

    // get euler angles from rotation matrix
    static bool MatrixToAngles(const double *rot, double *angles);

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

    //! delete object from name
    static void DeleteObject( TString );

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
    static double GetRandom( TF1* f, double xMin, double xMax );

    //! draw normalized histogram
    static void DrawNormalized( TTree* tree, TString name, TString var, const TCut& cut="", TString opt = "" );

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
        TString name,
        TString var,
        TCut cut="",
        bool autoH = true );

    //! Convert an histogram into a TGraph
    static TGraphErrors* HistoToTGraph( TH1* h, bool zeroSup = false );

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
    static TCanvas* NewTCanvas(
        TString name, TString title,
        int width, int height );

    //! create a new histogram safely (delete histograms with same name before)
    static TH1* NewTH1(
        TString name,
        TString title,
        int bin = 100,
        double min = 0,
        double max = 1 );

    //! create a new histogram safely (delete histograms with same name before)
    static TH1* NewTH1(
        TString name,
        TString title,
        int bin,
        double *x	);

    //! create a new 2D histogram safely (delete histograms with same name before)
    static TH2* NewTH2(
        TString name,
        TString title,
        int binx = 100,
        double minx = 0,
        double maxx = 1,
        int biny = 100,
        double miny = 0,
        double maxy = 1 );

    //! create a new 2D histogram safely (delete histograms with same name before)
    static TH3* NewTH3(
        TString name,
        TString title,
        int binx = 100,
        double minx = 0,
        double maxx = 1,
        int biny = 100,
        double miny = 0,
        double maxy = 1,
        int binz = 100,
        double minz = 0,
        double maxz = 1);

    //! create a new clone histogram safely (delete histograms with same name before)
    static TH1* NewClone(
        TString name,
        TString title,
        TH1* parent )
    { return NewClone( name, title, parent, false ); }

    //! create a new clone histogram safely (delete histograms with same name before)
    static TH1* NewClone(
        TString name,
        TString title,
        TH1* parent,
        bool reset );

    //! create a new clone histogram safely (delete histograms with same name before)
    static TH2* NewClone2D(
        TString name,
        TString title,
        TH2* parent )
    { return NewClone2D( name, title, parent, false ); }

    //! create a new clone histogram safely (delete histograms with same name before)
    static TH2* NewClone2D(
        TString name,
        TString title,
        TH2* parent,
        bool reset );

    //! create TF1
    /*! before creating the TF1, a matching TObject is looked after and deleted if found */
    static TF1* NewTF1(
        TString name,
        double (*function)(double*, double*),
        const double& min, const double& max,
        const int& n_par );

    //! subtract 2 histograms bin/bin; stores the result in an third histo; returns diff of number of entries
    static int SubtractHistograms(TH1* h1, TH1* h2, TH1* h3);

    //! subtract histogram and function bin/bin; stores the result in an third histo; returns diff of number of entries
    static int SubtractHistograms(TH1* h1, TF1* f, TH1* h3, double min, double max);

    //! divides 2 histograms bin/bin; stores the result in an third histo; returns ratio of the number of entries in the 2 histo
    enum ErrorMode
    {
        EFF,
        STD
    };

    //! divides 2 histograms bin/bin; stores the result in an third histo; returns ratio of the number of entries in the 2 histo
    static double DivideHistograms(TH1* h_found, TH1* h_ref, TH1* h3, int error_mode = EFF );

    //! divides 2 histograms bin/bin; stores the result in an third histo; returns ratio of the number of entries in the 2 histo
    static double DivideHistograms2D(TH2* h_found, TH2* h_ref, TH2* h3, int error_mode = EFF);

    //! divides 2 histograms bin/bin whith specified range; stores the result in an third histo; returns ratio of the number of entries in the 2 histo
    static double DivideHistograms(TH1* h_found, TH1* h_ref, TH1* h3, unsigned int i1, unsigned int i2, int error_mode = EFF);

    //! divides too TGraphs point/point; stores the result in 3 TGraph
    static TGraphErrors* DivideTGraphs( TGraphErrors* tg_found, TGraphErrors* tg_ref );

    #ifndef __CINT__

    /*! \brief
    generate two random numbers following probability distribution given by
    histogram
    */
    static std::pair<double,double> GetRandom2D( TH2* h );

    //! fill a string until its size reached required size
    static TString Fill( unsigned int size, const TString& value )
    {
        return( value.Length()>=size) ?
            value :
            value+TString(size-value.Length(), ' ');
    }

   #endif

    protected:

    #ifndef __CINT__

    //! merge all ntuples from a file into output
    static void Merge( std::vector<TString> files, const TString& output_file, const TString& selection );

    #endif



    ClassDef(Utils,0)
};

#endif
