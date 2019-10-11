#include "ChisquareFitter.h"

#include "ROOT_MACRO.h"

#include <TH1.h>
#include <TF1.h>
#include <TMath.h>
#include <TVirtualFitter.h>

//_______________________________________________________________________________
void ChisquareFitter::Fcn(
int& npar,
double* gin,
double& out,
double* u,
int flag )
{

    // number of fitted points
    int nFitPoints = 0;

    // virtual fitter
    TVirtualFitter *virtualFitter( TVirtualFitter::GetFitter() );

    // histogram to be fitted
    TH1* histogram = (TH1*)virtualFitter->GetObjectFit();

    // function used for the fit
    TF1* function = (TF1*)virtualFitter->GetUserFunc();

    // store arguments and number of parameters
    double x[3];
    function->InitArgs(x,u);
    npar = function->GetNpar();

    // initialization (copied from root)
    if( flag == 2 )
    {
        for( int k=0; k<npar; k++ )
        { gin[k] = 0; }

    }

    // initialize output
    out = 0;

    // loop over all bins
    for( int binX = histogram->GetXaxis()->GetFirst(); binX <= histogram->GetXaxis()->GetLast(); ++binX )
        for( int binY = histogram->GetYaxis()->GetFirst(); binY <= histogram->GetYaxis()->GetLast(); ++binY )
        for( int binZ = histogram->GetZaxis()->GetFirst(); binZ <= histogram->GetZaxis()->GetLast(); ++binZ )
    {

        x[0] = histogram->GetXaxis()->GetBinCenter( binX );
        x[1] = histogram->GetYaxis()->GetBinCenter( binY );
        x[2] = histogram->GetZaxis()->GetBinCenter( binZ );
        if( !function->IsInside(x) ) continue;

        // evaluate prediction
        TF1::RejectPoint(false);
        const double predicted( TMath::Max( function->EvalPar( x, u ), 1e-9 ) );
        if( TF1::RejectedPoint() ) continue;

        // evaluate measurement and error
        const int bin( histogram->GetBin( binX, binY, binZ ) );
        const double measured( histogram->GetBinContent( bin ) );
        const double error( histogram->GetBinError( bin ) );
        if( error <= 0 ) continue;

        // increment fit points and chisquare
        nFitPoints++;
        out += ROOT_MACRO::SQUARE( (measured-predicted)/error );

    }

    function->SetNumberFitPoints( nFitPoints );
    return;

}

//_______________________________________________________________________________
double ChisquareFitter::Chisquare( TH1* histogram, TF1* function )
{

    double x[3];
    double out = 0;

    // loop over all bins
    for( int binX = histogram->GetXaxis()->GetFirst(); binX <= histogram->GetXaxis()->GetLast(); ++binX )
        for( int binY = histogram->GetYaxis()->GetFirst(); binY <= histogram->GetYaxis()->GetLast(); ++binY )
        for( int binZ = histogram->GetZaxis()->GetFirst(); binZ <= histogram->GetZaxis()->GetLast(); ++binZ )
    {

        x[0] = histogram->GetXaxis()->GetBinCenter( binX );
        x[1] = histogram->GetYaxis()->GetBinCenter( binY );
        x[2] = histogram->GetZaxis()->GetBinCenter( binZ );
        if( !function->IsInside(x) ) continue;

        // evaluate prediction
        TF1::RejectPoint(false);
        const double predicted( TMath::Max( function->Eval( x[0] ), 1e-9 ) );
        if( TF1::RejectedPoint() ) continue;

        // evaluate measurement and error
        const int bin( histogram->GetBin( binX, binY, binZ ) );
        const double measured( histogram->GetBinContent( bin ) );
        const double error( histogram->GetBinError( bin ) );
        if( error <= 0 ) continue;

        // increment chisquare
        out += ROOT_MACRO::SQUARE( (measured-predicted)/error );

    }

    return out;

}
