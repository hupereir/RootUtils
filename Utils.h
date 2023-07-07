#ifndef Utils_h
#define Utils_h

#include <TCanvas.h>
#include <TCut.h>
#include <TObject.h>
#include <TROOT.h>
#include <TString.h>
#include <TTree.h>
#include <TVirtualPad.h>

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
class TF1;
class TH1;
class TH2;
class TH3;
class TText;
class TGraph;
class TGraphErrors;
class TProfile;
/// some root utilities to handle histograms safely
class Utils:public TObject
{

  public:

  /// constructor (doing nothing, needed for root)
  Utils( void ):
    TObject()
  {}

  static double GetMaximum( TGraph* );
  static double GetMaximumError( TGraphErrors* );

  // get axis bins with evenly space bins in log scale
  static std::vector<double> LogAxis( int n, double x_min, double x_max );

  /// get euler angles from rotation matrix (in degrees)
  /// copied from AliAlignObj::MatrixToAngles
  static bool MatrixToAngles(const Double_t *rot, Double_t *angles);

  /// get average for a number measurements
  static Double_t GetMean( Double_t*, Int_t );

  /// get rms for a number measurements
  static Double_t GetRMS( Double_t*, Int_t );

  /// get average for a number measurements
  static Double_t GetMean( Double_t*, Double_t*, Int_t );

  /// get rms for a number measurements
  static Double_t GetRMS( Double_t*, Double_t*, Int_t );

  /// get average for a number measurements
  static Double_t* GetRelativeDifference( Double_t*, Int_t );

  /// get average for a number measurements
  static Double_t* GetRelativeDifferenceError( Double_t*, Double_t*, Int_t );

  /// get the efficiency (ratio) from two numbers
  static Double_t GetEfficiency( Double_t reference, Double_t found );

  /// get the efficiency (ratio) errir from two numbers
  static Double_t GetEffError( Double_t reference, Double_t found );

  /// delete object from name
  static void DeleteObject( TString );

  /// dump histogram
  static void DumpHistogram( TH1 *h );

  /// dump function parameters
  static void DumpFunctionParameters( TF1 *f );

  /** \brief
  generate a random number following probability distribution given by
  histogram
  */
  static Double_t GetRandom( TH1* h );

  /** \brief
  generate a flat random number between 0 and max
  */
  static Double_t GetRandom( Double_t max )
  { return GetRandom( 0, max ); }

  /** \brief
  generate a flat random number between min and max
  */
  static Double_t GetRandom( Double_t min, Double_t max );

  /** \brief
  generate a random number following probability distribution given by
  histogram
  */
  static Double_t GetRandom( TF1* f, Double_t xMin, Double_t xMax );

  /// draw normalized histogram
  static void DrawNormalized( TTree* tree, TString name, TString var, const TCut& cut="", TString opt = "" );

  /// get ideal chisquare tree for a given number of freedoms
  static TTree* GetChisquareTree(
    Int_t ndf,
    Int_t nevents );


  /**
  get ideal chisquare for a given number of freedoms.
  Sums ndf+1 normalized gaussian distributions and get the chisquare
  corresponding to the average
  */
  static Double_t GetChisquare( Int_t ndf );

  /// fills histogram from tree return histogram
  static TH1* TreeToHisto(
    TTree *tree,
    TString name,
    TString var,
    TCut cut="",
    bool autoH = kTRUE );


  /// fills histogram from tree return histogram
  static TProfile* TreeToTProfile(
    TTree *tree,
    TString name,
    TString var,
    TCut cut = TCut() );

  /// Convert an histogram into a TGraph
  static TH1* TGraphToHistogram( TGraphErrors* tgraph );

  /// Convert an histogram into a TGraph
  static TH1* TGraphToHistogram( TGraphErrors* tgraph, Int_t nBins, const Double_t* bins );

  /// Convert an histogram into a TGraph
  static void TGraphToC( TGraphErrors* tgraph, TString xLabel, TString yLabel );

  /// Convert an histogram into a TGraph
  static TGraphErrors* HistogramToTGraph( TH1* h, bool zeroSup = kFALSE );

  /// returns number of entries (debugged)
  static Double_t GetEntries( TH1* h );

  /// scale histogram x axis
  static TH1* ScaleAxis( TH1*h, Double_t scale );

  /// print histogram axis
  static void PrintAxis( TH1* h );

  /// returns Integrated histogram
  static TH1* Integrate( TH1* h, bool normalize = kFALSE, bool inverse = kFALSE );

  /**
  returns histogram Integrate between to axis values
  linearly correct from bin effects
  */
  static Double_t Integrate( TH1* h, Double_t xmin, Double_t xmax );

  /// calculate effective entries scale factor
  static Double_t GetEffectiveScale( TH1* );

  /// create a new histogram safely (delete histograms with same name before)
  static TCanvas* NewTCanvas(
    TString name, TString title,
    Int_t width, Int_t height );

  /// create a new histogram safely (delete histograms with same name before)
  static TH1* NewTH1(
    TString name,
    TString title,
    Int_t bin = 100,
    Double_t min = 0,
    Double_t max = 1 );

  /// create a new histogram safely (delete histograms with same name before)
  static TH1* NewTH1(
    TString name,
    TString title,
    Int_t bin,
    Double_t *x	);

  /// create a new 2D histogram safely (delete histograms with same name before)
  static TH2* NewTH2(
    TString name,
    TString title,
    Int_t binx = 100,
    Double_t minx = 0,
    Double_t maxx = 1,
    Int_t biny = 100,
    Double_t miny = 0,
    Double_t maxy = 1 );

  /// create a new 2D histogram safely (delete histograms with same name before)
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

  /// create a new clone histogram safely (delete histograms with same name before)
  static TH1* NewClone(
    TString name,
    TString title,
    TH1* parent )
  { return NewClone( name, title, parent, kFALSE ); }

  /// create a new clone histogram safely (delete histograms with same name before)
  static TH1* NewClone(
    TString name,
    TString title,
    TH1* parent,
    bool reset );

  /// create a new clone histogram safely (delete histograms with same name before)
  static TH2* NewClone2D(
    TString name,
    TString title,
    TH2* parent )
  { return NewClone2D( name, title, parent, kFALSE ); }

  /// create a new clone histogram safely (delete histograms with same name before)
  static TH2* NewClone2D(
    TString name,
    TString title,
    TH2* parent,
    bool reset );

  /// create TF1
  /** before creating the TF1, a matching TObject is looked after and deleted if found */
  static TF1* NewTF1(
    TString name,
    Double_t (*function)(Double_t*, Double_t*),
    const Double_t& min, const Double_t& max,
    const int& n_par );

  /// subtract 2 histograms bin/bin; stores the result in an third histo; returns diff of number of entries
  static Int_t SubtractHistograms(TH1* h1, TH1* h2, TH1* h3);

  /// subtract histogram and function bin/bin; stores the result in an third histo; returns diff of number of entries
  static Int_t SubtractHistograms(TH1* h1, TF1* f, TH1* h3, Double_t min, Double_t max);

  /// divides 2 histograms bin/bin; stores the result in an third histo; returns ratio of the number of entries in the 2 histo
  enum ErrorMode
  {
    EFF,
    STD
  };

  /// divides 2 histograms bin/bin; stores the result in an third histo; returns ratio of the number of entries in the 2 histo
  static Double_t DivideHistograms(TH1* hFound, TH1* hRef, TH1* h3, Int_t errorMode = EFF );

  /// divides 2 histograms bin/bin; stores the result in an third histo; returns ratio of the number of entries in the 2 histo
  static Double_t DivideHistograms2D(TH2* hFound, TH2* hRef, TH2* h3, Int_t errorMode = EFF);

  /// divides 2 histograms bin/bin whith specified range; stores the result in an third histo; returns ratio of the number of entries in the 2 histo
  static Double_t DivideHistograms(TH1* hFound, TH1* hRef, TH1* h3, UInt_t i1, UInt_t i2, Int_t errorMode = EFF);

  /// divides too TGraphs point/point; stores the result in 3 TGraph
  static TGraphErrors* DivideTGraphs( TGraphErrors* tg_found, TGraphErrors* tg_ref );

  /// get integral distribution of a given histogram
  static TH1* GetIntegralHistogram( TH1*, bool inverse = true, bool include_overflow = true );

  #ifndef __CINT__

  /** \brief
  generate two random numbers following probability distribution given by
  histogram
  */
  static std::pair<Double_t,Double_t> GetRandom2D( TH2* h );

  /// fill a string until its size reached required size
  static TString Fill( Int_t size, const TString& value )
  {
    return( value.Length()>=size) ?
      value :
      value+TString(size-value.Length(), ' ');
  }

  #endif

  // max entries
  static int64_t max_entries;
  
  ClassDef(Utils,0)

};

#endif
