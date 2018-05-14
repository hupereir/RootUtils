#include "LikelihoodFitter.h"

#include <TH1.h>
#include <TF1.h>
#include <TMath.h>
#include <TVirtualFitter.h>

//_______________________________________________________________________________
// new implementation
void LikelihoodFitter::Fcn(
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

        // get bin center
        x[0] = histogram->GetXaxis()->GetBinCenter( binX );
        x[1] = histogram->GetYaxis()->GetBinCenter( binY );
        x[2] = histogram->GetZaxis()->GetBinCenter( binZ );
        if( !function->IsInside(x) ) continue;

        // evaluate prediction
        TF1::RejectPoint(false);
        const double predicted( TMath::Max( function->EvalPar( x, u ), 1e-9 ) );
        if( TF1::RejectedPoint() ) continue;

        // evaluate measurement
        const int bin( histogram->GetBin( binX, binY, binZ ) );
        const double measured( histogram->GetBinContent( bin ) );

        // increment fit points and fcn
        nFitPoints++;

        // calculate log of poissonian probability to get measured, if predicted is the mean
        out -= ( measured*TMath::Log(predicted) - predicted + TMath::LnGamma( measured+1 ) );

    }

    out *= 2;
    function->SetNumberFitPoints( nFitPoints );
    return;

}
