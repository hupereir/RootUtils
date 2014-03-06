#ifndef Utils_h
#define Utils_h

#include <TROOT.h>
#include <TObject.h>
#include <TCut.h>
#include <TCanvas.h>
#include <TVirtualPad.h>
#include <TString.h>

#include <cstdarg>
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

//! some root utilities to handle histograms safely
class Utils:public TObject
{

  public:

  //! constructor (doing nothing, needed for root)
  Utils( void ):
    TObject()
  {}

  // get euler angles from rotation matrix
  static Bool_t MatrixToAngles(const Double_t *rot, Double_t *angles);

  //! get minimum value
  static Double_t MinElement( Double_t* values, Int_t n )
  {
    Double_t out(0);
    for( Int_t i=0; i<n; ++i )
    { if( i==0 || values[i] < out ) out = values[i]; }
    return out;
  }

  //! get maximum value
  static Double_t MaxElement( Double_t* values, Int_t n )
  {
    Double_t out(0);
    for( Int_t i=0; i<n; ++i )
    { if( i==0 || values[i] > out ) out = values[i]; }
    return out;
  }

  //! get average for a number measurements
  static Double_t GetMean( Double_t*, Int_t );

  //! get rms for a number measurements
  static Double_t GetRMS( Double_t*, Int_t );

  //! get average for a number measurements
  static Double_t GetMean( Double_t*, Double_t*, Int_t );

  //! get rms for a number measurements
  static Double_t GetRMS( Double_t*, Double_t*, Int_t );

  //! get average for a number measurements
  static Double_t* GetRelativeDifference( Double_t*, Int_t );

  //! get average for a number measurements
  static Double_t* GetRelativeDifferenceError( Double_t*, Double_t*, Int_t );

    //! delete object from name
  static void DeleteObject( TString );

  //! dump histogram
  static void DumpHistogram( TH1 *h );

  //! dump function parameters
  static void DumpFunctionParameters( TF1 *f );

  /*! \brief
  generate a random number following probability distribution given by
  histogram
  */
  static Double_t GetRandom( TH1* h );

  /*! \brief
  generate a flat random number between 0 and max
  */
  static Double_t GetRandom( Double_t max )
  { return GetRandom( 0, max ); }

  /*! \brief
  generate a flat random number between min and max
  */
  static Double_t GetRandom( Double_t min, Double_t max );

  /*! \brief
  generate a random number following probability distribution given by
  histogram
  */
  static Double_t GetRandom( TF1* f, Double_t xMin, Double_t xMax );

  //! draw normalized histogram
  static void DrawNormalized( TTree* tree, TString name, TString var, const TCut& cut="", TString opt = "" );

  //! get ideal chisquare tree for a given number of freedoms
  static TTree* GetChisquareTree(
    Int_t ndf,
    Int_t nevents );


  /*!
  get ideal chisquare for a given number of freedoms.
  Sums ndf+1 normalized gaussian distributions and get the chisquare
  corresponding to the average
  */
  static Double_t GetChisquare( Int_t ndf );

  //! fills histogram from tree return histogram
  static TH1* TreeToHisto(
    TTree *tree,
    TString name,
    TString var,
    TCut cut="",
    Bool_t autoH = kTRUE );

  //! Convert an histogram into a TGraph
  static TGraphErrors* HistoToTGraph( TH1* h, Bool_t zeroSup = kFALSE );

  //! returns number of entries (debugged)
  static Double_t GetEntries( TH1* h );

  //! scale histogram x axis
  static TH1* ScaleAxis( TH1*h, Double_t scale );

  //! returns Integrated histogram
  static TH1* Integrate( TH1* h, Bool_t normalize = kFALSE );

  /*!
  returns histogram Integrate between to axis values
  linearly correct from bin effects
  */
  static Double_t Integrate( TH1* h, Double_t xmin, Double_t xmax );

  //! create a new histogram safely (delete histograms with same name before)
  static TCanvas* NewTCanvas(
    TString name, TString title,
    Int_t width, Int_t height );

  //! create a new histogram safely (delete histograms with same name before)
  static TH1* NewTH1(
    TString name,
    TString title,
    Int_t bin = 100,
    Double_t min = 0,
    Double_t max = 1 );

  //! create a new histogram safely (delete histograms with same name before)
  static TH1* NewTH1(
    TString name,
    TString title,
    Int_t bin,
    Double_t *x	);

  //! create a new 2D histogram safely (delete histograms with same name before)
  static TH2* NewTH2(
    TString name,
    TString title,
    Int_t binx = 100,
    Double_t minx = 0,
    Double_t maxx = 1,
    Int_t biny = 100,
    Double_t miny = 0,
    Double_t maxy = 1 );

  //! create a new 2D histogram safely (delete histograms with same name before)
  static TH3* NewTH3(
    TString name,
    TString title,
    Int_t binx = 100,
    Double_t minx = 0,
    Double_t maxx = 1,
    Int_t biny = 100,
    Double_t miny = 0,
    Double_t maxy = 1,
    Int_t binz = 100,
    Double_t minz = 0,
    Double_t maxz = 1);

  //! create a new clone histogram safely (delete histograms with same name before)
  static TH1* NewClone(
    TString name,
    TString title,
    TH1* parent )
  { return NewClone( name, title, parent, kFALSE ); }

  //! create a new clone histogram safely (delete histograms with same name before)
  static TH1* NewClone(
    TString name,
    TString title,
    TH1* parent,
    Bool_t reset );

  //! create a new clone histogram safely (delete histograms with same name before)
  static TH2* NewClone2D(
    TString name,
    TString title,
    TH2* parent )
  { return NewClone2D( name, title, parent, kFALSE ); }

  //! create a new clone histogram safely (delete histograms with same name before)
  static TH2* NewClone2D(
    TString name,
    TString title,
    TH2* parent,
    Bool_t reset );

  //! create TF1
  /*! before creating the TF1, a matching TObject is looked after and deleted if found */
  static TF1* NewTF1(
    TString name,
    Double_t (*function)(Double_t*, Double_t*),
    const Double_t& min, const Double_t& max,
    const int& n_par );

  //! subtract 2 histograms bin/bin; stores the result in an third histo; returns diff of number of entries
  static Int_t SubtractHistograms(TH1* h1, TH1* h2, TH1* h3);

  //! subtract histogram and function bin/bin; stores the result in an third histo; returns diff of number of entries
  static Int_t SubtractHistograms(TH1* h1, TF1* f, TH1* h3, Double_t min, Double_t max);

  //! divides 2 histograms bin/bin; stores the result in an third histo; returns ratio of the number of entries in the 2 histo
  enum ErrorMode
  {
    EFF,
    STD
  };

  //! divides 2 histograms bin/bin; stores the result in an third histo; returns ratio of the number of entries in the 2 histo
  static Double_t DivideHistograms(TH1* h_found, TH1* h_ref, TH1* h3, Int_t error_mode = EFF );

  //! divides 2 histograms bin/bin; stores the result in an third histo; returns ratio of the number of entries in the 2 histo
  static Double_t DivideHistograms2D(TH2* h_found, TH2* h_ref, TH2* h3, Int_t error_mode = EFF);

  //! divides 2 histograms bin/bin whith specified range; stores the result in an third histo; returns ratio of the number of entries in the 2 histo
  static Double_t DivideHistograms(TH1* h_found, TH1* h_ref, TH1* h3, UInt_t i1, UInt_t i2, Int_t error_mode = EFF);

  //! divides too TGraphs point/point; stores the result in 3 TGraph
  static TGraphErrors* DivideTGraphs( TGraphErrors* tg_found, TGraphErrors* tg_ref );

  #ifndef __CINT__

  /*! \brief
  generate two random numbers following probability distribution given by
  histogram
  */
  static std::pair<Double_t,Double_t> GetRandom2D( TH2* h );

  //! fill a string until its size reached required size
  static TString Fill( Int_t size, const TString& value )
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
