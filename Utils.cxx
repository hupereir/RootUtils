// $Id: Utils.cxx,v 1.56 2010/09/15 02:27:25 hpereira Exp $

/*!
\file Utils.cxx
\brief some root utilities to handle histograms safely
\author	Hugo Pereira
\version $Revision: 1.56 $
\date $Date: 2010/09/15 02:27:25 $
*/

#include "ALI_MACRO.h"
#include "Debug.h"
#include "Utils.h"

#include <TFile.h>
#include <TKey.h>
#include <TChain.h>
#include <TTree.h>
#include <TH1.h>
#include <TH2.h>
#include <TH3.h>
#include <TF1.h>
#include <TMath.h>
#include <TText.h>
#include <TPad.h>
#include <TPaveStats.h>
#include <TGraphErrors.h>
#include <TRandom.h>
#include <TMarker.h>

#include <iostream>
#include <fstream>
#include <string>
#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <unistd.h>
#include <map>

//__________________________________________________
//! root dictionary
ClassImp( Utils );

//_____________________________________________________________________________
bool Utils::MatrixToAngles(const double *rot, Double_t *angles)
{
  // Calculates the Euler angles in "x y z" notation
  // using the rotation matrix
  // Returns false in case the rotation angles can not be
  // extracted from the matrix
  if(TMath::Abs(rot[0])<1e-7 || TMath::Abs(rot[8])<1e-7)
  { return kFALSE; }

  Double_t raddeg = TMath::RadToDeg();
  angles[0]=raddeg*TMath::ATan2(-rot[5],rot[8]);
  angles[1]=raddeg*TMath::ASin(rot[2]);
  angles[2]=raddeg*TMath::ATan2(-rot[1],rot[0]);
  return kTRUE;

}

//________________________________________________________________________
void Utils::DeleteObject( const char* name )
{ return ALI_MACRO::Delete<TObject>( name ); }

//________________________________________________________________________
double Utils::GetMean( std::list<double>values )
{
    double out(0);
    for( std::list<double>::iterator iter = values.begin(); iter != values.end(); iter++ )
        out+=*iter;
    return out/values.size();
}

//________________________________________________________________________
double Utils::GetRMS( std::list<double>values )
{
    double out(0);
    const double mean( GetMean( values ) );
    for( std::list<double>::iterator iter = values.begin(); iter != values.end(); iter++ )
    { out+=ALI_MACRO::SQUARE(*iter - mean ); }

    return sqrt(out/values.size());
}

//________________________________________________________________________
void Utils::DumpHistogram( TH1* h )
{

    std::cout << "Utils::DumpHistogram - " << h->GetName() << std::endl;
    printf( "%5s %10s %10s %10s %10s %10s\n", "bin", "center", "content", "error", "sum", "error" );

    double sum( 0 );
    double sum_error( 0 );
    for( int i=0; i<h->GetNbinsX(); i++ )
    {
        sum += h->GetBinContent(i);
        sum_error += ALI_MACRO::SQUARE( h->GetBinError(i) );
        printf( "%5i %10f %10f %10f %10f %10f\n",
            i,
            h->GetBinCenter(i),
            h->GetBinContent(i),
            h->GetBinError(i),
            sum,
            sqrt( sum_error ) );
    }

    return;

}

//________________________________________________________________________
void Utils::DumpFunctionParameters( TF1* f )
{

    std::cout << "Utils::DumpFunctionParameters - " << f->GetName() << std::endl;
    printf( "%5s %10s %10s %10s\n", "index", "value", "min", "max" );
    for( int i=0; i < f->GetNpar(); i++ )
    {
        double min(0), max(0);
        f->GetParLimits( i, min, max );
        printf( "%5i %10f %10f %10f\n", i, f->GetParameter(i), min, max );
    }

    return;
}

//________________________________________________________________________
void Utils::DrawNormalized( TTree* tree, const char* name, const char* var, const TCut& cut, const char* opt )
{
    TH1* h = TreeToHisto( tree, name, var, cut, true );
    if( h->GetEntries() ) h->Scale( 1.0/h->GetEntries() );
    h->Draw( opt );
}

//________________________________________________________________________
TTree *Utils::GetChisquareTree( int ndf, int nevents )
{

    ALI_MACRO::Delete<TTree>( "chisquare" );
    TTree *tree = new TTree( "chisquare", "chisquare" );
    static double chi_square(0);

    enum { BUFFER_SIZE=32000 };
    enum { AUTO_SAVE=16000 };
    tree->Branch( "chi_square", &chi_square, "chi_square/D", BUFFER_SIZE );
    for( int event=0; event < nevents; event++ )
    {
        chi_square = GetChisquare( ndf );
        tree->Fill();
    }

    return tree;

}

//________________________________________________________________________
double Utils::GetChisquare( int ndf )
{

    double chi_square = 0;
    double average(0);
    static TRandom random;
    for( int i=0; i<ndf+1; i++ )
    {
        double value(random.Gaus());
        chi_square += ALI_MACRO::SQUARE( value );
        average+=value;
    }

    chi_square -= ALI_MACRO::SQUARE( average )/(ndf+1);
    return chi_square;

}

//__________________________________________________
double Utils::GetRandom( TH1* h )
{
    // initialize
    static bool first( true );
    if( first ) {
        first = false;
        srand( time( 0 ) );
    }

    if( !h ) return 0;

    double max( h->GetMaximum() );
    double xMin( h->GetXaxis()->GetXmin() );
    double xMax( h->GetXaxis()->GetXmax() );
    Debug::Str() << "Utils::GetRandom - xMin=" << xMin << "xMax=" << xMax << std::endl;

    while( 1 ) {
        double out = xMin + double( rand() )*(xMax-xMin)/RAND_MAX;

        int bin( h->GetXaxis()->FindBin( out ) );
        double value( h->GetBinContent(bin) );
        Debug::Str() << "Utils::GetRandom - max=" << max << " out=" << out << " value=" << value << std::endl;

        double prob = double( rand() )*max/RAND_MAX;
        if( prob < value ) return out;
    }

    //! never reached
    return 0;

}

//__________________________________________________
double Utils::GetRandom( TF1* f, double xMin, double xMax )
{
    // initialize
    static bool first( true );
    if( first ) {
        first = false;
        srand( time( 0 ) );
    }

    if( !f ) return 0;

    double max( f->GetMaximum( xMin, xMax ) );
    Debug::Str() << "Utils::GetRandom - xMin=" << xMin << "xMax=" << xMax << std::endl;

    while( 1 ) {
        double out = xMin + double( rand() )*(xMax-xMin)/RAND_MAX;
        double value( f->Eval( out ) );

        double prob = double( rand() )*max/RAND_MAX;
        if( prob < value ) return out;

    }

    //! never reached
    return 0;

}

//__________________________________________________
double Utils::GetRandom( double min, double max )
{
    // initialize
    static bool first( true );
    if( first ) {
        first = false;
        srand( time( 0 ) );
    }

    return (max - min)*double( rand() )/RAND_MAX;
}

//__________________________________________________
std::pair<double,double> Utils::GetRandom2D( TH2* h )
{
    // initialize
    static bool first( true );
    if( first ) {
        first = false;
        srand( time( 0 ) );
    }

    if( !h ) return std::make_pair<double,double>( 0, 0 );

    while( 1 ) {
        double xMin( h->GetXaxis()->GetXmin() );
        double xMax( h->GetXaxis()->GetXmax() );
        double out_x = double( rand() )*(xMax-xMin)/RAND_MAX;

        double y_min( h->GetYaxis()->GetXmin() );
        double y_max( h->GetYaxis()->GetXmax() );
        double out_y = double( rand() )*(y_max-y_min)/RAND_MAX;

        double max( h->GetMaximum() );
        int bin_x( h->GetXaxis()->FindBin( out_x ) );
        int bin_y( h->GetYaxis()->FindBin( out_y ) );

        double value( h->GetBinContent(bin_x, bin_y) );
        double prob = double( rand() )*max/RAND_MAX;
        if( prob < value ) return std::make_pair<double,double>(out_x, out_y);
    }

    //! not reached
    return std::make_pair<double,double>( 0, 0 );

}

//_________________________________________________
void Utils::Form( ostream &out, const char* format, ... )
{
    char* text = new char[2048];
    va_list p;
    va_start(p,format);
    vsprintf(text, format, p);
    va_end(p);
    out << text;
    SafeDelete( text );
    return;
}

//__________________________________________________
TH1* Utils::ScaleAxis( TH1* h, double scale )
{

    std::string title( h->GetTitle() );
    std::string name( h->GetName() );
    name += "_scale";


    TAxis* axis = h->GetXaxis();
    double xMin = scale*axis->GetXmin();
    double xMax = scale*axis->GetXmax();
    if( xMin > xMax ) std::swap( xMin, xMax );

    TH1* h_out =	NewTH1( name.c_str(), title.c_str(), axis->GetNbins(), xMin, xMax );
    for( int bin=0; bin < axis->GetNbins()+2; bin++ )
    {
        double x = scale*axis->GetBinCenter( bin );
        int dest_bin = h_out->FindBin( x );
        h_out->SetBinContent( dest_bin, h->GetBinContent( bin ) );
        h_out->SetBinError( dest_bin, h->GetBinError( bin ) );
    }
    h_out->SetEntries( h->GetEntries() );
    return h_out;

}

//__________________________________________________
TH1* Utils::Integrate( TH1* h, bool normalize )
{
    std::string name( h->GetName() );
    name += "_Integrated";

    std::string title( h->GetTitle() );
    title += " [Integrated]";

    double entries( h->GetEntries() );
    TH1* h_int( NewClone( name.c_str(), title.c_str(), h ) );

    // retrieve number of bins in histograms
    int n_bins( h->GetNbinsX() );
    for( int bin=0; bin < n_bins; bin++ ) {

        //retrieve Integrate
        double y( h->Integral( 1, bin+1 ) );
        double error = sqrt( y*(1.0-(y/entries)) );
        if( normalize ) {
            y /= entries;
            error /= entries;
        }
        h_int->SetBinContent( bin+1, y );
        h_int->SetBinError( bin+1, error );

    }

    return h_int;
}

//___________________________________________
double Utils::Integrate( TH1*h, double xmin, double xmax )
{
    // check order
    if( xmin >= xmax ) {
        std::cout << "Utils::Integrate - invalid range" << std::endl;
        return 0;
    }

    // find bins matching var_min and var_max
    int bin_min = h->GetXaxis()->FindBin( xmin );
    int bin_max = h->GetXaxis()->FindBin( xmax );

    // get the Integrate
    double out = h->Integral( bin_min, bin_max );

    // need to correct (linearly) from the bound bins
    double low_bin_correction(
        h->GetBinContent( bin_min )*
        ( xmin - h->GetXaxis()->GetBinLowEdge( bin_min ) )/
        ( h->GetXaxis()->GetBinUpEdge( bin_min ) - h->GetXaxis()->GetBinLowEdge( bin_min ) ) );

    double high_bin_correction(
        h->GetBinContent( bin_max )*
        ( h->GetXaxis()->GetBinUpEdge( bin_max ) - xmax )/
        ( h->GetXaxis()->GetBinUpEdge( bin_max ) - h->GetXaxis()->GetBinLowEdge( bin_max ) ) );

    Debug::Str()
        << "Utils::Integrate -"
        << " [" << xmin << "," << xmax << "]"
        << " [" << bin_min << "," << bin_max << "]"
        << " out=" << out
        << " low_correction=" << low_bin_correction
        << " high_correction=" << high_bin_correction
        << std::endl;

    return out - low_bin_correction - high_bin_correction;

}

//__________________________________________________
TH1* Utils::TreeToHisto(
    TTree *tree,
    const char* name,
    const char* var,
    TCut cut,
    bool autoH )
{
    // check tree
    if( !tree ) {
        std::cout << "Utils::TreeToHisto - tree is NULL .\n";
        return 0;
    }

    // check if histogram with requested name exists
    TH1* h = (TH1*) gROOT->FindObject(name);

    // if histogram autoformat requested, delete found histogram if any, give error message otherwise
    if( autoH && h ) { SafeDelete( h ); }
    else if( !(autoH || h ) ) {
        std::cout << "Utils::TreeToHisto - fatal: cannot find predefined histogram \"" << name << "\" .\n";
        return 0;
    }

    // create/fill autoformated histogram if requested
    if( autoH ) {
        std::string full_var = std::string( var ) + ">>" + name;
        tree->Draw(full_var.c_str(), cut, "goff" );
        h= (TH1*) gROOT->FindObject(name);

        // project in existing histogram otherwise
    } else tree->Project( name, var, cut );

    if( h ) h->SetLineWidth( 2 );
    std::ostringstream what;
    what << var << "{" << cut << "}";
    h->SetTitle( what.str().c_str() );
    return h;

}

//____________________________________________________________
TGraphErrors* Utils::HistoToTGraph( TH1* h, bool zeroSup )
{
    if( !h ) return 0;

    TGraphErrors *tg = new TGraphErrors();
    tg->SetMarkerStyle( h->GetMarkerStyle() );
    tg->SetMarkerColor( h->GetMarkerColor() );
    tg->SetLineColor( h->GetLineColor() );
    tg->SetLineWidth( h->GetLineWidth() );
    int point( 0 );
    for( int i=0; i<h->GetNbinsX(); i++ )
    {
        double x = h->GetXaxis()->GetBinCenter( i+1 );
        double y = double( h->GetBinContent( i+1 ) );
        double error = double( h->GetBinError( i+1 ) );
        if( zeroSup && !y ) continue;
        tg->SetPoint( point, x, y );
        tg->SetPointError( point, 0, error );
        point++;
    }

    return tg;
}

//____________________________________________________________
//! overload of TH1F to keep track of creation/deletion
class th1: public TH1F
{
    public:

    //! constructor
    th1( const char* name, const char* title, int bin, double min, double max ):
        TH1F( name, title, bin, min, max )
   { Debug::Str() << "th1::th1 - name = " << name << std::endl; }

//! destructor
~th1( void )
{ Debug::Str() << "th1::~th1 - name = " << GetName() << std::endl; }

};


//____________________________________________________________
TCanvas* Utils::NewTCanvas(
  const char* name, const char* title,
  int width, int height )
{
    ALI_MACRO::Delete<TCanvas>( name );
    return new TCanvas( name, title, width, height );
}

//____________________________________________________________
TH1* Utils::NewTH1(
    const char* name,
    const char* title,
    int bin,
    double min,
    double max
    )
{
    ALI_MACRO::Delete<TH1>( name );
    return new th1( name, title, bin, min, max );
}

//____________________________________________________________
TH1* Utils::NewTH1(
    const char* name,
    const char* title,
    int bin,
    double *x
    )
{
    ALI_MACRO::Delete<TH1>( name );
    return Utils::NewTH1( name, title, bin, x );
}

//____________________________________________________________
TH2* Utils::NewTH2(
    const char* name,
    const char* title,
    int binx ,
    double minx,
    double maxx,
    int biny,
    double miny,
    double maxy )
{
    ALI_MACRO::Delete<TH1>( name );
    return new TH2F( name, title, binx, minx, maxx, biny, miny, maxy );
}

//____________________________________________________________
TH3* Utils::NewTH3(
    const char* name,
    const char* title,
    int binx ,
    double minx,
    double maxx,
    int biny,
    double miny,
    double maxy,
    int binz,
    double minz,
    double maxz )
{
    ALI_MACRO::Delete<TH1>( name );
    return new TH3F( name, title,
      binx, minx, maxx,
      biny, miny, maxy,
      binz, minz, maxz );
}

//____________________________________________________________
TH1* Utils::NewClone(
    const char* name,
    const char* title,
    TH1* parent,
    bool reset
    )
{

    Debug::Str() << "Utils::NewClone - " << name << std::endl;

    // check parent histogram
    if( !parent ) {
        std::cout << "Utils::NewClone - null parent.\n";
        return 0;
    }

    // check if histogram with requested name exists
    ALI_MACRO::Delete<th1>( name );
    TH1* h = (TH1*) parent->Clone( name );
    if( reset ) h->Reset();
    h->SetName(name);
    h->SetTitle(title);
    return h;
}

//____________________________________________________________
TH2* Utils::NewClone2D(
    const char* name,
    const char* title,
    TH2* parent,
    bool reset
    )
{
    // check parent histogram
    if( !parent ) {
        std::cout << "Utils::NewClone - null parent.\n";
        return 0;
    }

    // check if histogram with requested name exists
    ALI_MACRO::Delete<TH2>( name );
    TH2* h = (TH2*) parent->Clone();
    if( reset ) h->Reset();
    h->SetName(name);
    h->SetTitle(title);
    return h;
}

//_______________________________________________________________
TF1* Utils::NewTF1( const char* name,
    double (*function)(double*, double*),
    const double& min, const double& max,
    const int& n_par )
{

    ALI_MACRO::Delete<TF1>( name );
    return new TF1( name, function, min, max, n_par );

}

//______________________________________________________
double Utils::GetEntries(TH1* h)
{
    double out( 0 );
    for(int i = 1; i < h->GetNbinsX(); i++)
        out += h->GetBinContent(i);
    return out;

}

//______________________________________________________
int Utils::HDiff(TH1* h1, TH1* h2, TH1* h3)
{

    unsigned int n1 = h1->GetNbinsX();
    unsigned int n2 = h2->GetNbinsX();
    unsigned int n3 = h3->GetNbinsX();

    double sum1( 0 ), sum2( 0 );
    if(!(n1 == n2 && n2 == n3)){
        std::cout << "Utils::HDiff - ERROR: Different number of bins.\n";
        std::cout << "	 " << n1 << ", " << n2 << ", " << n3 << std::endl;
        return int(h1->GetEntries()-h2->GetEntries() );
    }

    //loop over bins
    for(unsigned int i = 1; i < n1+1; i++) {

        double b1= h1->GetBinContent(i); sum1 += b1;
        double b2= h2->GetBinContent(i); sum2 += b2;
        h3->SetBinContent(i,b1-b2);

        double e1= ALI_MACRO::SQUARE(h1->GetBinError(i));
        double e2= ALI_MACRO::SQUARE(h2->GetBinError(i));

        h3->SetBinError( i, sqrt( e1+e2 ) );

    }
    return int( sum1 - sum2 );
}

//______________________________________________________
int Utils::HDiff(TH1* h1, TF1* f, TH1* h3, double min, double max)
{

    unsigned int n1 = h1->GetNbinsX();
    unsigned int n3 = h3->GetNbinsX();

    if(!(n1 == n3)){
        std::cout << "Utils::HDiff - ERROR: Different number of bins.\n";
        std::cout << "	 " << n1 << ", " << n3 << std::endl;
        return 0;
    }

    double sum( 0 );
    for(unsigned int i = 1; i < n1+1; i++) {

        // check bin is in range
        double center( h1->GetBinCenter( i ) );
        if( !(center >= min && center <= max ) ) {
            h3->SetBinContent(i, 0 );
            h3->SetBinError( i, 0 );
            continue;
        }

        double b1= h1->GetBinContent(i);
        double function_value( f->Eval( center ) );

        double e1= h1->GetBinError(i);

        sum+=b1-function_value;
        h3->SetBinContent(i,b1-function_value);
        h3->SetBinError( i, e1 );
    }
    return int( sum );
}

//______________________________________________________
double Utils::HDiv(TH1* h1, TH1* h2, TH1* h3, int errorMode )
{

    unsigned int n1 = h1->GetNbinsX();
    unsigned int n2 = h2->GetNbinsX();
    unsigned int n3 = h3->GetNbinsX();

    if(!(n1 == n2 && n2 == n3)){
        std::cout << "Utils::HDiv - ERROR: Different number of bins.\n";
        std::cout << "	 " << n1 << ", " << n2 << ", " << n3 << std::endl;
        return 0;
    }
    return HDiv( h1, h2, h3, 1, n1, errorMode );

}

//______________________________________________________
double Utils::HDiv(TH1* h1, TH1* h2, TH1* h3, unsigned int i1, unsigned int i2, int errorMode)
{

    unsigned int n1 = h1->GetNbinsX();
    unsigned int n2 = h2->GetNbinsX();
    unsigned int n3 = h3->GetNbinsX();

    if(!(n1 == n2 && n2 == n3)){
        std::cout << "Utils::HDiv - ERROR: Different number of bins.\n";
        std::cout << "	 " << n1 << ", " << n2 << ", " << n3 << std::endl;
        return 0;
    }

    for(unsigned int i = 1; i < n1+1; i++)
    {

        if( i >= i1 && i< i2+1 ) {

            double b1 = h1->GetBinContent(i);
            double b2 = h2->GetBinContent(i);
            double b3 = (b2 != 0) ? b1/b2:0;

            double e3 = 0;
            if( errorMode == EFF )
            {

                e3 = (b2 != 0 ) ? sqrt( b3*(1-b3)/b2 ):0;
                if( b1 == b2 && b1 ) e3 =	0.00001;
                if( b1 == 0 && b2 ) e3 = 0.00001;

            } else {

                e3 = (b2 != 0) ? b3*sqrt(
                    ALI_MACRO::SQUARE( 1.0/sqrt(b1) )
                    + ALI_MACRO::SQUARE( 1.0/sqrt(b2) )
                    ):0;

                if( b1 == 0 && b2 ) e3 = 0.00001;

            }

            h3->SetBinContent(i,b3);
            h3->SetBinError	(i,e3);

        } else {

            h3->SetBinContent(i,0.);
            h3->SetBinError	(i,0.);

        }
    }
    return ( (double) h1->Integral() / (double) h2->Integral() );
}

//______________________________________________________
TGraphErrors* Utils::TGDiv(TGraphErrors* tg1, TGraphErrors* tg2 )
{
    if( tg1->GetN() != tg2->GetN() )
    {
        std::cout << "Utils::TGDiv - ERROR: Different number of points.\n";
        return 0;
    }

    TGraphErrors *tg_out = new TGraphErrors();
    int point( 0 );
    for( int i=0; i<tg1->GetN(); i++ )
    {
        double x1(0), y1(0);
        double x2(0), y2(0);
        tg1->GetPoint(i, x1, y1 );
        tg2->GetPoint(i, x2, y2 );

        if( x1 != x2 )
        {
            std::cout << "Utils::TGDiv - different x. point " <<	i << "skipped" << std::endl;
            continue;
        }


        if( !y2 ) continue;
        double eff( y1/y2 );

        double err1( tg1->GetErrorY( i ) );
        double err2( tg2->GetErrorY( i ) );
        double err( sqrt(
            ALI_MACRO::SQUARE((1-eff)*err1)+
            ALI_MACRO::SQUARE(eff)*(
            ALI_MACRO::SQUARE(err2)-ALI_MACRO::SQUARE(err1)))/y2 );

        tg_out->SetPoint( point, x1, eff );
        tg_out->SetPointError( point, 0, err );
        std::cout << "Utils::TGDiv -"
            << " y1=" << y1
            << " y2=" << y2
            << " eff=" << eff
            << " +/- " << err
            << std::endl;

        point++;
    }

    return tg_out;

}

//______________________________________________________
double Utils::HDiv2D(TH2* h1, TH2* h2, TH2* h3, int errorMode )
{

    unsigned int nx1 = h1->GetNbinsX();
    unsigned int nx2 = h2->GetNbinsX();
    unsigned int nx3 = h3->GetNbinsX();

    unsigned int ny1 = h1->GetNbinsY();
    unsigned int ny2 = h2->GetNbinsY();
    unsigned int ny3 = h3->GetNbinsY();

    if(!(nx1 == nx2 && nx2 == nx3)){
        std::cout << "Utils::HDiv - ERROR: Different number of x bins: ";
        std::cout << " " << nx1 << ", " << nx2 << ", " << nx3 << std::endl;
        return 0;
    }

    if(!(ny1 == ny2 && ny2 == ny3))
    {
        std::cout << "Utils::HDiv - ERROR: Different number of bins: ";
        std::cout << " " << ny1 << ", " << ny2 << ", " << ny3 << std::endl;
        return 0;
    }

    for(unsigned int i = 1; i < nx1+1; i++)
    {
        for(unsigned int j = 1; j < ny1+1; j++)
        {

            int bin = h1->GetBin( i, j );
            double b1 = h1->GetBinContent(bin);
            double b2 = h2->GetBinContent(bin);
            double b3 = (b2 != 0) ? b1/b2:0;

            double e3 = 0;
            if( errorMode == EFF )
            {
                e3 = (b2 != 0 ) ? sqrt( b3*(1-b3)/b2 ):0;
                if( b1 == b2 && b1 ) e3 =	0.00001;
            } else {
                e3 = b3*sqrt(
                    ALI_MACRO::SQUARE( 1.0/sqrt(b1) )
                    + ALI_MACRO::SQUARE( 1.0/sqrt(b2) )
                    );
            }
            h3->SetBinContent(bin,b3);
            h3->SetBinError	(bin,e3);

        }
    }

    return ( (double) h1->Integral() / (double) h2->Integral() );

}
