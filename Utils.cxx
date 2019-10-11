// $Id: Utils.cxx,v 1.56 2010/09/15 02:27:25 hpereira Exp $

#include "ROOT_MACRO.h"
#include "Debug.h"

#if WITH_GSL
#include "GSLError.h"
#endif

#include "Stream.h"
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
#include <TProfile.h>
#include <TGraphErrors.h>
#include <TRandom.h>
#include <TMarker.h>
#include <TVirtualFitter.h>

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
Bool_t Utils::MatrixToAngles(const Double_t *rot, Double_t *angles)
{
    // Calculates the Euler angles in "x y z" notation
    // using the rotation matrix
    // Returns kFALSE in case the rotation angles can not be
    // extracted from the matrix
    if(TMath::Abs(rot[0])<1e-7 || TMath::Abs(rot[8])<1e-7)
    { return kFALSE; }

    const Double_t raddeg = TMath::RadToDeg();
    angles[0]=raddeg*TMath::ATan2(-rot[5],rot[8]);
    angles[1]=raddeg*TMath::ASin(rot[2]);
    angles[2]=raddeg*TMath::ATan2(-rot[1],rot[0]);
    return kTRUE;

}

//________________________________________________________________________
void Utils::DeleteObject( TString name )
{ return ROOT_MACRO::Delete<TObject>( name ); }

//________________________________________________________________________
Double_t Utils::GetMean( Double_t* values, Int_t n )
{
    Double_t out(0);
    for( Int_t i = 0; i < n; ++i )
    { out+=values[i]; }
    return out/n;
}

//________________________________________________________________________
Double_t Utils::GetRMS( Double_t* values, Int_t n  )
{
    Double_t out(0);
    const Double_t mean( GetMean( values, n ) );
    for( Int_t i = 0; i < n; ++i )
    { out+=ROOT_MACRO::SQUARE(values[i] - mean ); }

    return TMath::Sqrt(out/n);
}

//________________________________________________________________________
Double_t Utils::GetMean( Double_t* values, Double_t* errors, Int_t n )
{
    Double_t out(0);
    Double_t weight(0);
    for( Int_t i=0; i < n; ++i )
    {
        if( errors[i] > 0 )
        {
            out += values[i]/ROOT_MACRO::SQUARE( errors[i] );
            weight += 1.0/ROOT_MACRO::SQUARE( errors[i] );
        }
    }

    return out/weight;
}

//________________________________________________________________________
Double_t Utils::GetRMS( Double_t* values, Double_t* errors, Int_t n )
{
    Double_t out(0);
    Double_t weight(0);
    const Double_t mean( GetMean( values, errors, n ) );
    for( Int_t i=0; i < n; ++i )
    {

        if( errors[i] > 0 )
        {
            out += ROOT_MACRO::SQUARE( values[i] - mean )/ROOT_MACRO::SQUARE( errors[i] );
            weight += 1.0/ROOT_MACRO::SQUARE( errors[i] );
        }

    }

    return TMath::Sqrt(out/weight);
}

//________________________________________________________________________
Double_t* Utils::GetRelativeDifference( Double_t* values, Int_t n )
{

    Double_t* out = new Double_t[n];
    for( Int_t i = 0; i < n; ++i )
    {
        if( values[0] == 0 ) out[i] = 0;
        else out[i] = values[i]/values[0] - 1;
    }

    return out;

}

//________________________________________________________________________
Double_t* Utils::GetRelativeDifferenceError( Double_t* values, Double_t* errors, Int_t n )
{

    Double_t* out = new Double_t[n];
    for( Int_t i = 0; i < n; ++i )
    {
        if( values[0] == 0 ) out[i] = 0;
        else if( values[i] > 0 ) {

            Double_t ratio = values[i]/values[0];
            out[i] = ratio * TMath::Sqrt( ROOT_MACRO::SQUARE( errors[i]/values[i] ) + ROOT_MACRO::SQUARE( errors[0]/values[0] ) );

        } else out[i] = 0;
    }

    return out;

}

//________________________________________________________________________
Double_t Utils::GetEfficiency( Double_t ref, Double_t found )
{ return ref > 0 ? found/ref : 0; }

//________________________________________________________________________
Double_t Utils::GetEffError( Double_t ref, Double_t found )
{
    if( ref <= 0 ) return 0;

    Double_t eff( GetEfficiency( ref, found ) );
    if( eff > 1 ) return 0;

    return TMath::Sqrt( eff*(1.0-eff)/ref );
}

//________________________________________________________________________
void Utils::DumpHistogram( TH1* h )
{

    std::cout << "Utils::DumpHistogram - " << h->GetName() << std::endl;
    printf( "%5s %10s %10s %10s %10s %10s\n", "bin", "center", "content", "error", "sum", "error" );

    Double_t sum( 0 );
    Double_t sum_error( 0 );
    for( Int_t i=0; i<h->GetNbinsX(); i++ )
    {
        sum += h->GetBinContent(i);
        sum_error += ROOT_MACRO::SQUARE( h->GetBinError(i) );
        printf( "%5i %10f %10f %10f %10f %10f\n",
            i,
            h->GetBinCenter(i),
            h->GetBinContent(i),
            h->GetBinError(i),
            sum,
            TMath::Sqrt( sum_error ) );
    }

    return;

}

//________________________________________________________________________
void Utils::DumpFunctionParameters( TF1* f )
{

    std::cout << "Utils::DumpFunctionParameters - " << f->GetName() << std::endl;
    printf( "%5s %10s %10s %10s\n", "index", "value", "min", "max" );
    for( Int_t i=0; i < f->GetNpar(); i++ )
    {
        Double_t min(0), max(0);
        f->GetParLimits( i, min, max );
        printf( "%5i %10f %10f %10f\n", i, f->GetParameter(i), min, max );
    }

    return;
}

//________________________________________________________________________
void Utils::DrawNormalized( TTree* tree, TString name, TString var, const TCut& cut, TString opt )
{
    TH1* h = TreeToHisto( tree, name, var, cut, kTRUE );
    if( h->GetEntries() ) h->Scale( 1.0/h->GetEntries() );
    h->Draw( opt );
}

//________________________________________________________________________
TTree *Utils::GetChisquareTree( Int_t ndf, Int_t nevents )
{

    ROOT_MACRO::Delete<TTree>( "chisquare" );
    TTree *tree = new TTree( "chisquare", "chisquare" );
    static Double_t chi_square(0);

    enum { BUFFER_SIZE=32000 };
    enum { AUTO_SAVE=16000 };
    tree->Branch( "chi_square", &chi_square, "chi_square/D", BUFFER_SIZE );
    for( Int_t event=0; event < nevents; event++ )
    {
        chi_square = GetChisquare( ndf );
        tree->Fill();
    }

    return tree;

}

//________________________________________________________________________
Double_t Utils::GetChisquare( Int_t ndf )
{

    Double_t chi_square = 0;
    Double_t average(0);
    static TRandom random;
    for( Int_t i=0; i<ndf+1; i++ )
    {
        Double_t value(random.Gaus());
        chi_square += ROOT_MACRO::SQUARE( value );
        average+=value;
    }

    chi_square -= ROOT_MACRO::SQUARE( average )/(ndf+1);
    return chi_square;

}

//__________________________________________________
Double_t Utils::GetRandom( TH1* h )
{
    // initialize
    static Bool_t first( kTRUE );
    if( first ) {
        first = kFALSE;
        srand( time( 0 ) );
    }

    if( !h ) return 0;

    Double_t max( h->GetMaximum() );
    Double_t xMin( h->GetXaxis()->GetXmin() );
    Double_t xMax( h->GetXaxis()->GetXmax() );
    Debug::Str() << "Utils::GetRandom - xMin=" << xMin << "xMax=" << xMax << std::endl;

    while( 1 ) {
        Double_t out = xMin + Double_t( rand() )*(xMax-xMin)/RAND_MAX;

        Int_t bin( h->GetXaxis()->FindBin( out ) );
        Double_t value( h->GetBinContent(bin) );
        Debug::Str() << "Utils::GetRandom - max=" << max << " out=" << out << " value=" << value << std::endl;

        Double_t prob = Double_t( rand() )*max/RAND_MAX;
        if( prob < value ) return out;
    }

    //! never reached
    return 0;

}

//__________________________________________________
Double_t Utils::GetRandom( TF1* f, Double_t xMin, Double_t xMax )
{
    // initialize
    static Bool_t first( kTRUE );
    if( first ) {
        first = kFALSE;
        srand( time( 0 ) );
    }

    if( !f ) return 0;

    Double_t max( f->GetMaximum( xMin, xMax ) );
    Debug::Str() << "Utils::GetRandom - xMin=" << xMin << "xMax=" << xMax << std::endl;

    while( 1 ) {
        Double_t out = xMin + Double_t( rand() )*(xMax-xMin)/RAND_MAX;
        Double_t value( f->Eval( out ) );

        Double_t prob = Double_t( rand() )*max/RAND_MAX;
        if( prob < value ) return out;

    }

    //! never reached
    return 0;

}

//__________________________________________________
Double_t Utils::GetRandom( Double_t min, Double_t max )
{
    // initialize
    static Bool_t first( kTRUE );
    if( first ) {
        first = kFALSE;
        srand( time( 0 ) );
    }

    return (max - min)*Double_t( rand() )/RAND_MAX;
}

//__________________________________________________
std::pair<Double_t,Double_t> Utils::GetRandom2D( TH2* h )
{
    // initialize
    static Bool_t first( kTRUE );
    if( first ) {
        first = kFALSE;
        srand( time( 0 ) );
    }

    if( !h ) return std::make_pair( 0, 0 );

    while( kTRUE )
    {
        Double_t xMin( h->GetXaxis()->GetXmin() );
        Double_t xMax( h->GetXaxis()->GetXmax() );
        Double_t out_x = Double_t( rand() )*(xMax-xMin)/RAND_MAX;

        Double_t y_min( h->GetYaxis()->GetXmin() );
        Double_t y_max( h->GetYaxis()->GetXmax() );
        Double_t out_y = Double_t( rand() )*(y_max-y_min)/RAND_MAX;

        Double_t max( h->GetMaximum() );
        Int_t bin_x( h->GetXaxis()->FindBin( out_x ) );
        Int_t bin_y( h->GetYaxis()->FindBin( out_y ) );

        Double_t value( h->GetBinContent(bin_x, bin_y) );
        Double_t prob = Double_t( rand() )*max/RAND_MAX;
        if( prob < value ) return std::make_pair(out_x, out_y);
    }

    //! not reached
    return std::make_pair( 0, 0 );

}

//_________________________________________________
void UtilsForm( std::ostream &out, TString format, ... )
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
TH1* Utils::ScaleAxis( TH1* h, Double_t scale )
{

    TString title( h->GetTitle() );
    TString name( h->GetName() );
    name += "_scale";


    TAxis* axis = h->GetXaxis();
    Double_t xMin = scale*axis->GetXmin();
    Double_t xMax = scale*axis->GetXmax();
    if( xMin > xMax ) std::swap( xMin, xMax );

    TH1* hOut =	NewTH1( name.Data(), title.Data(), axis->GetNbins(), xMin, xMax );
    for( Int_t bin=0; bin < axis->GetNbins()+2; bin++ )
    {
        Double_t x = scale*axis->GetBinCenter( bin );
        Int_t dest_bin = hOut->FindBin( x );
        hOut->SetBinContent( dest_bin, h->GetBinContent( bin ) );
        hOut->SetBinError( dest_bin, h->GetBinError( bin ) );
    }
    hOut->SetEntries( h->GetEntries() );
    return hOut;

}

//__________________________________________________
TH1* Utils::Integrate( TH1* h, Bool_t normalize )
{
    TString name( h->GetName() );
    name += "_Integrated";

    TString title( h->GetTitle() );
    title += " [Integrated]";

    Double_t entries( h->GetEntries() );
    TH1* hInt( NewClone( name.Data(), title.Data(), h ) );

    // retrieve number of bins in histograms
    Int_t n_bins( h->GetNbinsX() );
    for( Int_t bin=0; bin < n_bins; bin++ ) {

        //retrieve Integrate
        Double_t y( h->Integral( 1, bin+1 ) );
        Double_t error = TMath::Sqrt( y*(1.0-(y/entries)) );
        if( normalize ) {
            y /= entries;
            error /= entries;
        }
        hInt->SetBinContent( bin+1, y );
        hInt->SetBinError( bin+1, error );

    }

    return hInt;
}

//___________________________________________
Double_t Utils::Integrate( TH1*h, Double_t xmin, Double_t xmax )
{
    // check order
    if( xmin >= xmax )
    {
        std::cout << "Utils::Integrate - invalid range" << std::endl;
        return 0;
    }

    // find bins matching var_min and var_max
    Int_t bin_min = h->GetXaxis()->FindBin( xmin );
    Int_t bin_max = h->GetXaxis()->FindBin( xmax );

    // get the Integrate
    Double_t out = h->Integral( bin_min, bin_max );

    // need to correct (linearly) from the bound bins
    Double_t low_bin_correction(
        h->GetBinContent( bin_min )*
        ( xmin - h->GetXaxis()->GetBinLowEdge( bin_min ) )/
        ( h->GetXaxis()->GetBinUpEdge( bin_min ) - h->GetXaxis()->GetBinLowEdge( bin_min ) ) );

    Double_t high_bin_correction(
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
Double_t Utils::GetEffectiveScale( TH1* h )
{
    const Int_t nBinsX = h->GetNbinsX();
    const Int_t nBinsY = h->GetNbinsY();
    const Int_t nBinsZ = h->GetNbinsZ();
    std::cout << "Utils::GetEffectiveScale - bins: " << nBinsX << ", " << nBinsY << ", " << nBinsZ << std::endl;

    Double_t sumEntries = 0;
    Double_t sumErrorSquare = 0;
    for( Int_t iX = 0; iX < nBinsX; ++iX )
        for( Int_t iY = 0; iY < nBinsY; ++iY )
        for( Int_t iZ = 0; iZ < nBinsZ; ++iZ )
    {

        const Int_t bin = h->GetBin( iX+1, iY+1, iZ+1 );
        sumEntries += h->GetBinContent( bin );
        sumErrorSquare += ROOT_MACRO::SQUARE( h->GetBinError( bin ) );

    }

    return sumEntries/sumErrorSquare;
}

//__________________________________________________
TH1* Utils::TreeToHisto(
    TTree *tree,
    TString name,
    TString var,
    TCut cut,
    Bool_t autoH )
{
    // check tree
    if( !tree )
    {
        std::cout << "Utils::TreeToHisto - tree is NULL .\n";
        return 0;
    }

    // check if histogram with requested name exists
    TH1* h = static_cast<TH1*>( gROOT->FindObject(name) );

    // if histogram autoformat requested, delete found histogram if any, give error message otherwise
    if( autoH && h ) { SafeDelete( h ); }
    else if( !(autoH || h ) )
    {
        std::cout << "Utils::TreeToHisto - fatal: cannot find predefined histogram \"" << name << "\" .\n";
        return 0;
    }

    if( autoH )
    {

        // create/fill autoformated histogram if requested
        tree->Draw( Form( "%s >> %s", var.Data(), name.Data() ), cut, "goff" );
        h = static_cast<TH1*>( gROOT->FindObject(name) );

    } else {

        // project in existing histogram otherwise
        tree->Project( name, var, cut );

    }

    if( h )
    {
        h->SetLineWidth( 2 );
        h->SetTitle( Form( "%s {%s}", var.Data(), ((TString) cut).Data() ) );
    }

    return h;

}

//__________________________________________________
TProfile* Utils::TreeToTProfile(
    TTree *tree,
    TString name,
    TString var,
    TCut cut )
{
    // check tree
    if( !tree )
    {
        std::cout << "Utils::TreeToHisto - tree is NULL .\n";
        return 0;
    }

    // check if histogram with requested name exists
    TProfile* h = static_cast<TProfile*>( gROOT->FindObject(name) );

    // if histogram autoformat requested, delete found histogram if any, give error message otherwise
    if( !h )
    {
        std::cout << "Utils::TreeToHisto - fatal: cannot find predefined histogram \"" << name << "\" .\n";
        return 0;
    }

    // create/fill autoformated histogram if requested
    tree->Draw( Form( "%s >> %s", var.Data(), name.Data() ), cut, "goff" );
    h= static_cast<TProfile*>( gROOT->FindObject(name) );

    return h;

}

//____________________________________________________________
TH1* Utils::TGraphToHistogram( TGraphErrors* tg )
{

    std::vector<Double_t> bins;

    for( Int_t i = 0; i < tg->GetN(); ++i )
    {
        Double_t x(0);
        Double_t y(0);
        tg->GetPoint( i, x, y );

        // error
        const Double_t errorXLow( tg->GetErrorXlow( i ) );
        const Double_t errorXHigh( tg->GetErrorXhigh( i ) );

        if( bins.empty() ) bins.push_back( x+errorXLow );
        else if( bins.back() !=  x+errorXLow )
        {
            std::cerr << "Utils::TGraphToHistogram - horizontal error bars are not adjacent for bin " << i << std::endl;
            if( bins.back() > x+errorXLow ) std::cerr << "Utils::TGraphToHistogram - Bins at index " << i << "are overlapping. Ignoring." << std::endl;
            else bins.push_back( x+errorXLow );
        }

        bins.push_back( x+errorXHigh );

    }

    return TGraphToHistogram( tg, bins.size(), &bins[0] );

}

//____________________________________________________________
TH1* Utils::TGraphToHistogram( TGraphErrors* tg, Int_t nBins, const Double_t* bins )
{

    const TString name = tg->GetName() ? tg->GetName():"h";
    const TString title = tg->GetTitle() ? tg->GetTitle():"";

    TH1* h = new TH1F( name, title, nBins, bins );
    for( Int_t i = 0; i < tg->GetN(); ++i )
    {

        // position
        Double_t x(0);
        Double_t y(0);
        tg->GetPoint( i, x, y );

        // error
        const Double_t errorY( tg->GetErrorY( i ) );

        // find matching bin in histogram
        const Int_t iBin = h->FindBin( x );
        if( h->GetBinContent( iBin ) != 0 )
        { std::cout << "Utils::TGraphToHistogram - something is wrong: several points match bin number " << iBin << std::endl; }

        h->SetBinContent( iBin, y );
        h->SetBinError( iBin, errorY );

    }

    return h;

}

//____________________________________________________________
void Utils::TGraphToC( TGraphErrors* tgraph, TString xLabel, TString yLabel )
{

    const Int_t nBins = tgraph->GetN();
    Stream::PrintVector( xLabel, tgraph->GetX(), nBins, "%.3f" );
    Stream::PrintVector( Form( "%sErr", xLabel.Data() ), tgraph->GetEX(), nBins, "%.3f" );

    Stream::PrintVector( yLabel, tgraph->GetY(), nBins, "%.3g" );
    Stream::PrintVector( Form( "%sErr", yLabel.Data() ), tgraph->GetEY(), nBins, "%.3g" );

}

//____________________________________________________________
TGraphErrors* Utils::HistogramToTGraph( TH1* h, Bool_t zeroSup )
{
    if( !h ) return 0;

    TGraphErrors *tg = new TGraphErrors();
    tg->SetMarkerStyle( h->GetMarkerStyle() );
    tg->SetMarkerColor( h->GetMarkerColor() );
    tg->SetMarkerSize( h->GetMarkerSize() );
    tg->SetLineColor( h->GetLineColor() );
    tg->SetLineWidth( h->GetLineWidth() );
    Int_t point( 0 );
    for( Int_t i=0; i<h->GetNbinsX(); i++ )
    {
        Double_t x = h->GetXaxis()->GetBinCenter( i+1 );
        Double_t y = Double_t( h->GetBinContent( i+1 ) );
        Double_t error = Double_t( h->GetBinError( i+1 ) );
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
    th1( TString name, TString title, Int_t bin, Double_t min, Double_t max ):
        TH1F( name, title, bin, min, max )
    { Debug::Str() << "th1::th1 - name = " << name << std::endl; }

    //! destructor
    ~th1( void )
    { Debug::Str() << "th1::~th1 - name = " << GetName() << std::endl; }

};


//____________________________________________________________
TCanvas* Utils::NewTCanvas(
    TString name, TString title,
    Int_t width, Int_t height )
{
    ROOT_MACRO::Delete<TCanvas>( name );
    return new TCanvas( name, title, width, height );
}

//____________________________________________________________
TH1* Utils::NewTH1(
    TString name,
    TString title,
    Int_t bin,
    Double_t min,
    Double_t max
    )
{
    ROOT_MACRO::Delete<TH1>( name );
    return new th1( name, title, bin, min, max );
}

//____________________________________________________________
TH1* Utils::NewTH1(
    TString name,
    TString title,
    Int_t bin,
    Double_t *x
    )
{
    ROOT_MACRO::Delete<TH1>( name );
    return Utils::NewTH1( name, title, bin, x );
}

//____________________________________________________________
TH2* Utils::NewTH2(
    TString name,
    TString title,
    Int_t binx ,
    Double_t minx,
    Double_t maxx,
    Int_t biny,
    Double_t miny,
    Double_t maxy )
{
    ROOT_MACRO::Delete<TH1>( name );
    return new TH2F( name, title, binx, minx, maxx, biny, miny, maxy );
}

//____________________________________________________________
TH3* Utils::NewTH3(
    TString name,
    TString title,
    Int_t binx ,
    Double_t minx,
    Double_t maxx,
    Int_t biny,
    Double_t miny,
    Double_t maxy,
    Int_t binz,
    Double_t minz,
    Double_t maxz )
{
    ROOT_MACRO::Delete<TH1>( name );
    return new TH3F( name, title,
        binx, minx, maxx,
        biny, miny, maxy,
        binz, minz, maxz );
}

//____________________________________________________________
TH1* Utils::NewClone(
    TString name,
    TString title,
    TH1* parent,
    Bool_t reset
    )
{

    Debug::Str() << "Utils::NewClone - " << name << std::endl;

    // check parent histogram
    if( !parent ) {
        std::cout << "Utils::NewClone - null parent.\n";
        return 0;
    }

    // check if histogram with requested name exists
    ROOT_MACRO::Delete<th1>( name );
    TH1* h = static_cast<TH1*>( parent->Clone( name ) );
    if( reset ) h->Reset();
    h->SetName(name);
    h->SetTitle(title);
    return h;
}

//____________________________________________________________
TH2* Utils::NewClone2D(
    TString name,
    TString title,
    TH2* parent,
    Bool_t reset
    )
{
    // check parent histogram
    if( !parent ) {
        std::cout << "Utils::NewClone - null parent.\n";
        return 0;
    }

    // check if histogram with requested name exists
    ROOT_MACRO::Delete<TH2>( name );
    TH2* h = (TH2*) parent->Clone();
    if( reset ) h->Reset();
    h->SetName(name);
    h->SetTitle(title);
    return h;
}

//_______________________________________________________________
TF1* Utils::NewTF1( TString name,
    Double_t (*function)(Double_t*, Double_t*),
    const Double_t& min, const Double_t& max,
    const int& n_par )
{

    ROOT_MACRO::Delete<TF1>( name );
    return new TF1( name, function, min, max, n_par );

}

//______________________________________________________
Double_t Utils::GetEntries(TH1* h)
{
    Double_t out( 0 );
    for(Int_t i = 1; i < h->GetNbinsX(); i++)
        out += h->GetBinContent(i);
    return out;

}

//______________________________________________________
Int_t Utils::SubtractHistograms(TH1* h1, TH1* h2, TH1* h3)
{

    UInt_t n1 = h1->GetNbinsX();
    UInt_t n2 = h2->GetNbinsX();
    UInt_t n3 = h3->GetNbinsX();

    Double_t sum1( 0 ), sum2( 0 );
    if(!(n1 == n2 && n2 == n3)){
        std::cout << "Utils::SubtractHistograms - Different number of bins.\n";
        std::cout << "	 " << n1 << ", " << n2 << ", " << n3 << std::endl;
        return int(h1->GetEntries()-h2->GetEntries() );
    }

    //loop over bins
    for(UInt_t i = 1; i < n1+1; i++) {

        Double_t b1= h1->GetBinContent(i); sum1 += b1;
        Double_t b2= h2->GetBinContent(i); sum2 += b2;
        h3->SetBinContent(i,b1-b2);

        Double_t e1= ROOT_MACRO::SQUARE(h1->GetBinError(i));
        Double_t e2= ROOT_MACRO::SQUARE(h2->GetBinError(i));

        h3->SetBinError( i, TMath::Sqrt( e1+e2 ) );

    }
    return int( sum1 - sum2 );
}

//______________________________________________________
Int_t Utils::SubtractHistograms(TH1* h1, TF1* f, TH1* h3, Double_t min, Double_t max)
{

    UInt_t n1 = h1->GetNbinsX();
    UInt_t n3 = h3->GetNbinsX();

    if(!(n1 == n3)){
        std::cout << "Utils::SubtractHistograms - Different number of bins.\n";
        std::cout << "	 " << n1 << ", " << n3 << std::endl;
        return 0;
    }

    Double_t sum( 0 );
    for(UInt_t i = 1; i < n1+1; i++) {

        // check bin is in range
        Double_t center( h1->GetBinCenter( i ) );
        if( !(center >= min && center <= max ) ) {
            h3->SetBinContent(i, 0 );
            h3->SetBinError( i, 0 );
            continue;
        }

        Double_t b1= h1->GetBinContent(i);
        Double_t function_value( f->Eval( center ) );

        Double_t e1= h1->GetBinError(i);

        sum+=b1-function_value;
        h3->SetBinContent(i,b1-function_value);
        h3->SetBinError( i, e1 );
    }
    return int( sum );
}

//______________________________________________________
Double_t Utils::DivideHistograms(TH1* h1, TH1* h2, TH1* h3, Int_t errorMode )
{

    UInt_t n1 = h1->GetNbinsX();
    UInt_t n2 = h2->GetNbinsX();
    UInt_t n3 = h3->GetNbinsX();

    if(!(n1 == n2 && n2 == n3)){
        std::cout << "Utils::DivideHistograms - Different number of bins.\n";
        std::cout << "	 " << n1 << ", " << n2 << ", " << n3 << std::endl;
        return 0;
    }
    return DivideHistograms( h1, h2, h3, 1, n1, errorMode );

}

//______________________________________________________
Double_t Utils::DivideHistograms(TH1* h1, TH1* h2, TH1* h3, UInt_t i1, UInt_t i2, Int_t errorMode)
{

    UInt_t n1 = h1->GetNbinsX();
    UInt_t n2 = h2->GetNbinsX();
    UInt_t n3 = h3->GetNbinsX();

    if(!(n1 == n2 && n2 == n3)){
        std::cout << "Utils::DivideHistograms - Different number of bins.\n";
        std::cout << "	 " << n1 << ", " << n2 << ", " << n3 << std::endl;
        return 0;
    }

    for(UInt_t i = 1; i < n1+1; i++)
    {

        if( i >= i1 && i< i2+1 ) {

            Double_t b1 = h1->GetBinContent(i);
            Double_t b2 = h2->GetBinContent(i);
            Double_t b3 = (b2 != 0) ? b1/b2:0;

            Double_t e3 = 0;
            if( errorMode == EFF )
            {

                e3 = (b2 != 0 ) ? TMath::Sqrt( b3*(1-b3)/b2 ):0;
                if( b1 == b2 && b1 ) e3 =	0.00001;
                if( b1 == 0 && b2 ) e3 = 0.00001;

            } else {

                if( b1 != 0 ) e3 += ROOT_MACRO::SQUARE( 1.0/TMath::Sqrt(b1) );
                if( b2 != 0 ) e3 += ROOT_MACRO::SQUARE( 1.0/TMath::Sqrt(b2) );
                e3 = b3*TMath::Sqrt(e3);
                if( e3 == 0 ) e3 = 0.00001;
            }

            h3->SetBinContent(i,b3);
            h3->SetBinError	(i,e3);

        } else {

            h3->SetBinContent(i,0.);
            h3->SetBinError	(i,0.);

        }
    }
    return ( (Double_t) h1->Integral() / (Double_t) h2->Integral() );
}

//______________________________________________________
TGraphErrors* Utils::DivideTGraphs(TGraphErrors* tg1, TGraphErrors* tg2 )
{
    if( tg1->GetN() != tg2->GetN() )
    {
        std::cout << "Utils::DivideTGraphs - Different number of points.\n";
        return 0;
    }

    TGraphErrors *tg_out = new TGraphErrors();
    Int_t point( 0 );
    for( Int_t i=0; i<tg1->GetN(); i++ )
    {
        Double_t x1(0), y1(0);
        Double_t x2(0), y2(0);
        tg1->GetPoint(i, x1, y1 );
        tg2->GetPoint(i, x2, y2 );

        if( x1 != x2 )
        {
            std::cout << "Utils::DivideTGraphs - different x. poInt_t " <<	i << "skipped" << std::endl;
            continue;
        }


        if( !y2 ) continue;
        Double_t eff( y1/y2 );

        Double_t err1( tg1->GetErrorY( i ) );
        Double_t err2( tg2->GetErrorY( i ) );
        Double_t err( TMath::Sqrt(
            ROOT_MACRO::SQUARE((1-eff)*err1)+
            ROOT_MACRO::SQUARE(eff)*(
            ROOT_MACRO::SQUARE(err2)-ROOT_MACRO::SQUARE(err1)))/y2 );

        tg_out->SetPoint( point, x1, eff );
        tg_out->SetPointError( point, 0, err );
        std::cout << "Utils::DivideTGraphs -"
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
Double_t Utils::DivideHistograms2D(TH2* h1, TH2* h2, TH2* h3, Int_t errorMode )
{

    UInt_t nx1 = h1->GetNbinsX();
    UInt_t nx2 = h2->GetNbinsX();
    UInt_t nx3 = h3->GetNbinsX();

    UInt_t ny1 = h1->GetNbinsY();
    UInt_t ny2 = h2->GetNbinsY();
    UInt_t ny3 = h3->GetNbinsY();

    if(!(nx1 == nx2 && nx2 == nx3))
    {
        std::cout << "Utils::DivideHistograms - Different number of x bins: ";
        std::cout << " " << nx1 << ", " << nx2 << ", " << nx3 << std::endl;
        return 0;
    }

    if(!(ny1 == ny2 && ny2 == ny3))
    {
        std::cout << "Utils::DivideHistograms - Different number of bins: ";
        std::cout << " " << ny1 << ", " << ny2 << ", " << ny3 << std::endl;
        return 0;
    }

    for(UInt_t i = 1; i < nx1+1; i++)
    {

        for(UInt_t j = 1; j < ny1+1; j++)
        {

            Int_t bin = h1->GetBin( i, j );
            Double_t b1 = h1->GetBinContent(bin);
            Double_t b2 = h2->GetBinContent(bin);
            Double_t b3 = (b2 != 0) ? b1/b2:0;

            Double_t e3 = 0;
            if( errorMode == EFF )
            {
                e3 = (b2 != 0 ) ? TMath::Sqrt( b3*(1-b3)/b2 ):0;
                if( b1 == b2 && b1 ) e3 =	0.00001;
            } else {

                e3 = b3*TMath::Sqrt(
                    ROOT_MACRO::SQUARE( 1.0/TMath::Sqrt(b1) )
                    + ROOT_MACRO::SQUARE( 1.0/TMath::Sqrt(b2) )
                    );
            }

            h3->SetBinContent(bin,b3);
            h3->SetBinError	(bin,e3);

        }

    }

    return ( (Double_t) h1->Integral() / (Double_t) h2->Integral() );

}
