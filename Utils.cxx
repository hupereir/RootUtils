// $Id: Utils.cxx,v 1.56 2010/09/15 02:27:25 hpereira Exp $

/*!
\file Utils.cxx
\brief some root utilities to handle histograms safely
\author	Hugo Pereira
\version $Revision: 1.56 $
\date $Date: 2010/09/15 02:27:25 $
*/

#include "MUTOO_MACRO.h"
#include "Debug.h"
#include "Utils.h"

#include <TFile.h>
#include <TKey.h>
#include <TChain.h>
#include <TTree.h>
#include <TH1.h>
#include <TH2.h>
#include <TF1.h>
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

using namespace std;

//__________________________________________________
//! root dictionary
ClassImp( Utils );

//________________________________________________________________________
string Utils::replace_all( const string& in, const string& c1, const string& c2 )
{
    if( !c1.size() ) return "";
    string out("");
    size_t len = in.size();
    size_t current = 0;
    size_t found=0;
    while( current < len && ( found = in.find( c1, current ) ) != string::npos )
    {
        out += in.substr( current, found-current ) + c2;
        current=found+c1.size();
    }

    if( current < len ) out += in.substr( current, len-current );

    return out;

}

//________________________________________________________________________
void Utils::print_vector( ostream& out, const char* name, const double* values, const int& size, const char* format )
{ _print_vector<double>( out, "double", name, values, size, format ); }

//________________________________________________________________________
void Utils::print_int_vector( ostream& out, const char* name, const int* values, const int& size, const char* format )
{ _print_vector<int>( out, "int", name, values, size, format ); }

//_________________________________
void Utils::print_vector_2d( ostream& out, const char* name, const double* values, const int& size_1, const int& size_2, const char* format )
{ _print_vector_2d<double>( out, "double", name, values, size_1, size_2, format ); }

//_________________________________
void Utils::print_int_vector_2d( ostream& out, const char* name, const int* values, const int& size_1, const int& size_2, const char* format )
{ _print_vector_2d<int>( out, "int", name, values, size_1, size_2, format ); }

//_________________________________
void Utils::print_vector_3d( ostream& out, const char* name, const double* values, const int& size_1, const int& size_2, const int& size_3,const char* format )
{ _print_vector_3d<double>( out, "double", name, values, size_1, size_2, size_3, format ); }

//_________________________________
void Utils::print_vector_4d( ostream& out, const char* name, const double* values, const int& size_1, const int& size_2, const int& size_3,const int& size_4, const char* format )
{ _print_vector_4d<double>( out, "double", name, values, size_1, size_2, size_3, size_4, format ); }

//________________________________________________________________________
void Utils::print_cuts( const TCut& cut )
{

    cout << Utils::convert( (const char*) cut, "&&", "&&\n" );
    cout << endl;

}

//________________________________________________________________________
double Utils::get_mean( list<double>values )
{
    double out(0);
    for( list<double>::iterator iter = values.begin(); iter != values.end(); iter++ )
        out+=*iter;
    return out/values.size();
}

//________________________________________________________________________
double Utils::get_rms( list<double>values )
{
    double out(0);
    double mean( get_mean( values ) );
    for( list<double>::iterator iter = values.begin(); iter != values.end(); iter++ )
        out+=MUTOO_MACRO::SQUARE(*iter - mean );
    return sqrt(out/values.size());
}

//________________________________________________________________________
void Utils::dump_histogram( TH1* h )
{

    cout << "Utils::dump_histogram - " << h->GetName() << endl;
    printf( "%5s %10s %10s %10s %10s %10s\n", "bin", "center", "content", "error", "sum", "error" );

    double sum( 0 );
    double sum_error( 0 );
    for( int i=0; i<h->GetNbinsX(); i++ )
    {
        sum += h->GetBinContent(i);
        sum_error += MUTOO_MACRO::SQUARE( h->GetBinError(i) );
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
void Utils::dump_function_parameters( TF1* f )
{

    cout << "Utils::dump_function_parameters - " << f->GetName() << endl;
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
void Utils::draw_normalized( TTree* tree, const char* name, const char* var, const TCut& cut, const char* opt )
{
    TH1* h = tree_to_histo( tree, name, var, cut, true );
    if( h->GetEntries() ) h->Scale( 1.0/h->GetEntries() );
    h->Draw( opt );
}

//________________________________________________________________________
TTree *Utils::get_chisquare_tree( int ndf, int nevents )
{

    MUTOO_MACRO::safe_delete<TTree>( "chisquare" );
    TTree *tree = new TTree( "chisquare", "chisquare" );
    static double chi_square(0);

    enum { BUFFER_SIZE=32000 };
    enum { AUTO_SAVE=16000 };
    tree->Branch( "chi_square", &chi_square, "chi_square/D", BUFFER_SIZE );
    for( int event=0; event < nevents; event++ )
    {
        chi_square = get_chisquare( ndf );
        tree->Fill();
    }

    return tree;

}

//________________________________________________________________________
double Utils::get_chisquare( int ndf )
{

    double chi_square = 0;
    double average(0);
    static TRandom random;
    for( int i=0; i<ndf+1; i++ )
    {
        double value(random.Gaus());
        chi_square += MUTOO_MACRO::SQUARE( value );
        average+=value;
    }

    chi_square -= MUTOO_MACRO::SQUARE( average )/(ndf+1);
    return chi_square;

}

//__________________________________________________
double Utils::get_random( TH1* h )
{
    // initialize
    static bool first( true );
    if( first ) {
        first = false;
        srand( time( 0 ) );
    }

    if( !h ) return 0;

    double max( h->GetMaximum() );
    double x_min( h->GetXaxis()->GetXmin() );
    double x_max( h->GetXaxis()->GetXmax() );
    Debug::str() << "Utils::get_random - x_min=" << x_min << "x_max=" << x_max << endl;

    while( 1 ) {
        double out = x_min + double( rand() )*(x_max-x_min)/RAND_MAX;

        int bin( h->GetXaxis()->FindBin( out ) );
        double value( h->GetBinContent(bin) );
        Debug::str() << "Utils::get_random - max=" << max << " out=" << out << " value=" << value << endl;

        double prob = double( rand() )*max/RAND_MAX;
        if( prob < value ) return out;
    }

    //! never reached
    return 0;

}

//__________________________________________________
double Utils::get_random( TF1* f, double x_min, double x_max )
{
    // initialize
    static bool first( true );
    if( first ) {
        first = false;
        srand( time( 0 ) );
    }

    if( !f ) return 0;

    double max( f->GetMaximum( x_min, x_max ) );
    Debug::str() << "Utils::get_random - x_min=" << x_min << "x_max=" << x_max << endl;

    while( 1 ) {
        double out = x_min + double( rand() )*(x_max-x_min)/RAND_MAX;
        double value( f->Eval( out ) );

        double prob = double( rand() )*max/RAND_MAX;
        if( prob < value ) return out;

    }

    //! never reached
    return 0;

}

//__________________________________________________
double Utils::get_random( double min, double max )
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
pair<double,double> Utils::get_random_2d( TH2* h )
{
    // initialize
    static bool first( true );
    if( first ) {
        first = false;
        srand( time( 0 ) );
    }

    if( !h ) return make_pair<double,double>( 0, 0 );

    while( 1 ) {
        double x_min( h->GetXaxis()->GetXmin() );
        double x_max( h->GetXaxis()->GetXmax() );
        double out_x = double( rand() )*(x_max-x_min)/RAND_MAX;

        double y_min( h->GetYaxis()->GetXmin() );
        double y_max( h->GetYaxis()->GetXmax() );
        double out_y = double( rand() )*(y_max-y_min)/RAND_MAX;

        double max( h->GetMaximum() );
        int bin_x( h->GetXaxis()->FindBin( out_x ) );
        int bin_y( h->GetYaxis()->FindBin( out_y ) );

        double value( h->GetBinContent(bin_x, bin_y) );
        double prob = double( rand() )*max/RAND_MAX;
        if( prob < value ) return make_pair<double,double>(out_x, out_y);
    }

    //! not reached
    return make_pair<double,double>( 0, 0 );

}

//_________________________________________________
void Utils::form( ostream &out, const char* format, ... )
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
TH1* Utils::scale_axis( TH1* h, double scale )
{

    string title( h->GetTitle() );
    string name( h->GetName() );
    name += "_scale";


    TAxis* axis = h->GetXaxis();
    double x_min = scale*axis->GetXmin();
    double x_max = scale*axis->GetXmax();
    if( x_min > x_max ) swap( x_min, x_max );

    TH1* h_out =	new_th1( name.c_str(), title.c_str(), axis->GetNbins(), x_min, x_max );
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
TH1* Utils::integrate( TH1* h, bool normalize )
{
    string name( h->GetName() );
    name += "_integrated";

    string title( h->GetTitle() );
    title += " [integrated]";

    double entries( h->GetEntries() );
    TH1* h_int( new_clone( name.c_str(), title.c_str(), h ) );

    // retrieve number of bins in histograms
    int n_bins( h->GetNbinsX() );
    for( int bin=0; bin < n_bins; bin++ ) {

        //retrieve integral
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
double Utils::integral( TH1*h, double xmin, double xmax )
{
    // check order
    if( xmin >= xmax ) {
        cout << "Utils::integral - invalid range" << endl;
        return 0;
    }

    // find bins matching var_min and var_max
    int bin_min = h->GetXaxis()->FindBin( xmin );
    int bin_max = h->GetXaxis()->FindBin( xmax );

    // get the integral
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

    Debug::str()
        << "Utils::integral -"
        << " [" << xmin << "," << xmax << "]"
        << " [" << bin_min << "," << bin_max << "]"
        << " out=" << out
        << " low_correction=" << low_bin_correction
        << " high_correction=" << high_bin_correction
        << endl;

    return out - low_bin_correction - high_bin_correction;

}

//__________________________________________________
TH1* Utils::tree_to_histo(
    TTree *tree,
    const char* name,
    const char* var,
    TCut cut,
    bool autoH )
{
    // check tree
    if( !tree ) {
        cout << "Utils::tree_to_histo - tree is NULL .\n";
        return 0;
    }

    // check if histogram with requested name exists
    TH1* h = (TH1*) gROOT->FindObject(name);

    // if histogram autoformat requested, delete found histogram if any, give error message otherwise
    if( autoH && h ) { SafeDelete( h ); }
    else if( !(autoH || h ) ) {
        cout << "Utils::tree_to_histo - fatal: cannot find predefined histogram \"" << name << "\" .\n";
        return 0;
    }

    // create/fill autoformated histogram if requested
    if( autoH ) {
        string full_var = string( var ) + ">>" + name;
        tree->Draw(full_var.c_str(), cut, "goff" );
        h= (TH1*) gROOT->FindObject(name);

        // project in existing histogram otherwise
    } else tree->Project( name, var, cut );

    if( h ) h->SetLineWidth( 2 );
    ostringstream what;
    what << var << "{" << cut << "}";
    h->SetTitle( what.str().c_str() );
    return h;

}

//__________________________________________________
TH2* Utils::tree_to_histo_2d(
    TTree *tree,
    const char* name,
    const char* var,
    TCut cut,
    bool autoH )
{

    // check tree
    if( !tree ) {
        cout << "Utils::tree_to_histo - tree is NULL .\n";
        return 0;
    }


    // check if histogram with requested name exists
    TH2* h = (TH2*) gROOT->FindObject(name);

    // if histogram autoformat requested, delete found histogram if any, give error message otherwise
    if( autoH && h ) { SafeDelete( h ); }
    else if( !(autoH || h ) ) {
        cout << "Utils::tree_to_histo - fatal: cannot find predefined histogram\"" << name << "\" .\n";
        return 0;
    }

    // create/fill autoformated histogram if requested
    if( autoH ) {
        string full_var = string( var ) + ">>" + name;
        tree->Draw(full_var.c_str(), cut, "goff" );
        h= (TH2*) gROOT->FindObject(name);

        // project in existing histogram otherwise
    } else tree->Project( name, var, cut );

    return h;
}

//____________________________________________________________
TGraphErrors* Utils::histo_to_tgraph( TH1* h, bool zero_sup )
{
    if( !h ) return 0;

    TGraphErrors *tg = new TGraphErrors();
    int point( 0 );
    for( int i=0; i<h->GetNbinsX(); i++ )
    {
        double x = h->GetXaxis()->GetBinCenter( i+1 );
        double y = double( h->GetBinContent( i+1 ) );
        double error = double( h->GetBinError( i+1 ) );
        if( zero_sup && !y ) continue;
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
   { Debug::str() << "th1::th1 - name = " << name << endl; }

//! destructor
~th1( void )
{ Debug::str() << "th1::~th1 - name = " << GetName() << endl; }

};

//____________________________________________________________
TH1* Utils::new_th1(
    const char* name,
    const char* title,
    int bin,
    double min,
    double max
    )
{
    MUTOO_MACRO::safe_delete<TH1>( name );
    return new th1( name, title, bin, min, max );
}

//____________________________________________________________
TH1* Utils::new_th1(
    const char* name,
    const char* title,
    int bin,
    double *x
    )
{
    MUTOO_MACRO::safe_delete<TH1>( name );
    return new TH1F( name, title, bin, x );
}

//____________________________________________________________
TH2* Utils::new_th2(
    const char* name,
    const char* title,
    int binx ,
    double minx,
    double maxx,
    int biny,
    double miny,
    double maxy )
{
    MUTOO_MACRO::safe_delete<TH1>( name );
    return new TH2F( name, title, binx, minx, maxx, biny, miny, maxy );
}

//____________________________________________________________
TH1* Utils::new_clone(
    const char* name,
    const char* title,
    TH1* parent,
    bool reset
    )
{

    Debug::str() << "Utils::new_clone - " << name << endl;

    // check parent histogram
    if( !parent ) {
        cout << "Utils::new_clone - null parent.\n";
        return 0;
    }

    // check if histogram with requested name exists
    MUTOO_MACRO::safe_delete<th1>( name );
    TH1* h = (TH1*) parent->Clone( name );
    if( reset ) h->Reset();
    h->SetName(name);
    h->SetTitle(title);
    return h;
}

//____________________________________________________________
TH2* Utils::new_clone_2d(
    const char* name,
    const char* title,
    TH2* parent,
    bool reset
    )
{
    // check parent histogram
    if( !parent ) {
        cout << "Utils::new_clone - null parent.\n";
        return 0;
    }

    // check if histogram with requested name exists
    MUTOO_MACRO::safe_delete<TH2>( name );
    TH2* h = (TH2*) parent->Clone();
    if( reset ) h->Reset();
    h->SetName(name);
    h->SetTitle(title);
    return h;
}

//_______________________________________________________________
TF1* Utils::new_tf1( const char* name,
    double (*function)(double*, double*),
    const double& min, const double& max,
    const int& n_par )
{

    MUTOO_MACRO::safe_delete<TF1>( name );
    return new TF1( name, function, min, max, n_par );

}

//______________________________________________________
double Utils::get_entries(TH1* h)
{
    double out( 0 );
    for(int i = 1; i < h->GetNbinsX(); i++)
        out += h->GetBinContent(i);
    return out;

}

//______________________________________________________
int Utils::h_diff(TH1* h1, TH1* h2, TH1* h3)
{

    unsigned int n1 = h1->GetNbinsX();
    unsigned int n2 = h2->GetNbinsX();
    unsigned int n3 = h3->GetNbinsX();

    double sum1( 0 ), sum2( 0 );
    if(!(n1 == n2 && n2 == n3)){
        cout << "Utils::h_diff - ERROR: Different number of bins.\n";
        cout << "	 " << n1 << ", " << n2 << ", " << n3 << endl;
        return int(h1->GetEntries()-h2->GetEntries() );
    }

    //loop over bins
    for(unsigned int i = 1; i < n1+1; i++) {

        double b1= h1->GetBinContent(i); sum1 += b1;
        double b2= h2->GetBinContent(i); sum2 += b2;
        h3->SetBinContent(i,b1-b2);

        double e1= MUTOO_MACRO::SQUARE(h1->GetBinError(i));
        double e2= MUTOO_MACRO::SQUARE(h2->GetBinError(i));

        h3->SetBinError( i, sqrt( e1+e2 ) );

    }
    return int( sum1 - sum2 );
}

//______________________________________________________
int Utils::h_diff(TH1* h1, TF1* f, TH1* h3, double min, double max)
{

    unsigned int n1 = h1->GetNbinsX();
    unsigned int n3 = h3->GetNbinsX();

    if(!(n1 == n3)){
        cout << "Utils::h_diff - ERROR: Different number of bins.\n";
        cout << "	 " << n1 << ", " << n3 << endl;
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
double Utils::h_div(TH1* h1, TH1* h2, TH1* h3, int error_mode )
{

    unsigned int n1 = h1->GetNbinsX();
    unsigned int n2 = h2->GetNbinsX();
    unsigned int n3 = h3->GetNbinsX();

    if(!(n1 == n2 && n2 == n3)){
        cout << "Utils::h_div - ERROR: Different number of bins.\n";
        cout << "	 " << n1 << ", " << n2 << ", " << n3 << endl;
        return 0;
    }
    return h_div( h1, h2, h3, 1, n1, error_mode );

}

//______________________________________________________
double Utils::h_div(TH1* h1, TH1* h2, TH1* h3, unsigned int i1, unsigned int i2, int error_mode)
{

    unsigned int n1 = h1->GetNbinsX();
    unsigned int n2 = h2->GetNbinsX();
    unsigned int n3 = h3->GetNbinsX();

    if(!(n1 == n2 && n2 == n3)){
        cout << "Utils::h_div - ERROR: Different number of bins.\n";
        cout << "	 " << n1 << ", " << n2 << ", " << n3 << endl;
        return 0;
    }

    for(unsigned int i = 1; i < n1+1; i++) {
        if( i >= i1 && i< i2+1 ) {

            double b1 = h1->GetBinContent(i);
            double b2 = h2->GetBinContent(i);
            double b3 = (b2 != 0) ? b1/b2:0;

            double e3 = 0;
            if( error_mode == EFF )
            {

                e3 = (b2 != 0 ) ? sqrt( b3*(1-b3)/b2 ):0;
                if( b1 == b2 && b1 ) e3 =	0.00001;

            } else {

                e3 = (b2 != 0) ? b3*sqrt(
                    MUTOO_MACRO::SQUARE( 1.0/sqrt(b1) )
                    + MUTOO_MACRO::SQUARE( 1.0/sqrt(b2) )
                    ):0;

                if( b1 == 0 && b2 ) e3 = 0.00001;

            }

            h3->SetBinContent(i,b3);
            h3->SetBinError	(i,e3);

        } else {

            h3->SetBinContent(i,0);
            h3->SetBinError	(i,0);

        }
    }
    return ( (double) h1->Integral() / (double) h2->Integral() );
}

//______________________________________________________
TGraphErrors* Utils::tg_div(TGraphErrors* tg1, TGraphErrors* tg2 )
{
    if( tg1->GetN() != tg2->GetN() )
    {
        cout << "Utils::tg_div - ERROR: Different number of points.\n";
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
            cout << "Utils::tg_div - different x. point " <<	i << "skipped" << endl;
            continue;
        }


        if( !y2 ) continue;
        double eff( y1/y2 );

        double err1( tg1->GetErrorY( i ) );
        double err2( tg2->GetErrorY( i ) );
        double err( sqrt(
            MUTOO_MACRO::SQUARE((1-eff)*err1)+
            MUTOO_MACRO::SQUARE(eff)*(
            MUTOO_MACRO::SQUARE(err2)-MUTOO_MACRO::SQUARE(err1)))/y2 );

        tg_out->SetPoint( point, x1, eff );
        tg_out->SetPointError( point, 0, err );
        cout << "Utils::tg_div -"
            << " y1=" << y1
            << " y2=" << y2
            << " eff=" << eff
            << " +/- " << err
            << endl;

        point++;
    }

    return tg_out;

}

//______________________________________________________
double Utils::h_div_2d(TH2* h1, TH2* h2, TH2* h3, int error_mode )
{

    unsigned int nx1 = h1->GetNbinsX();
    unsigned int nx2 = h2->GetNbinsX();
    unsigned int nx3 = h3->GetNbinsX();

    unsigned int ny1 = h1->GetNbinsY();
    unsigned int ny2 = h2->GetNbinsY();
    unsigned int ny3 = h3->GetNbinsY();

    if(!(nx1 == nx2 && nx2 == nx3)){
        cout << "Utils::h_div - ERROR: Different number of x bins: ";
        cout << " " << nx1 << ", " << nx2 << ", " << nx3 << endl;
        return 0;
    }

    if(!(ny1 == ny2 && ny2 == ny3))
    {
        cout << "Utils::h_div - ERROR: Different number of bins: ";
        cout << " " << ny1 << ", " << ny2 << ", " << ny3 << endl;
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
            if( error_mode == EFF )
            {
                e3 = (b2 != 0 ) ? sqrt( b3*(1-b3)/b2 ):0;
                if( b1 == b2 && b1 ) e3 =	0.00001;
            } else {
                e3 = b3*sqrt(
                    MUTOO_MACRO::SQUARE( 1.0/sqrt(b1) )
                    + MUTOO_MACRO::SQUARE( 1.0/sqrt(b2) )
                    );
            }
            h3->SetBinContent(bin,b3);
            h3->SetBinError	(bin,e3);

        }
    }

    return ( (double) h1->Integral() / (double) h2->Integral() );

}

//______________________________________________________________________
string Utils::convert( const string& in, const string& c1, const string& c2)
{
    if( !c1.size() ) return "";
    string out("");
    size_t len = in.size();
    size_t current = 0;
    size_t found=0;
    while( current < len && ( found=in.find( c1, current ) ) != string::npos ) {
        out += in.substr( current, found-current ) + c2;
        current=found+c1.size();
    }
    if( current < len )
    { out += in.substr( current, len-current ); }

    return out;
}
