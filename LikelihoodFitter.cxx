#include "LikelihoodFitter.h"

#include <TH1.h>
#include <TF1.h>
#include <TVirtualFitter.h>

#include <cmath>

//_______________________________________________________________________________
void LikelihoodFitter::fcn(
    int& npar,
    double* gin,
    double& out,
    double* u,
    int flag )
{

    // number of fitted points
    int nFitPoints = 0;

    // virtual fitter
    TVirtualFitter *fitter( TVirtualFitter::GetFitter() );

    // histogram to be fitted
    TH1* histogram = (TH1*)hFitter->GetObjectFit();

    // function used for the fit
    TF1* function = (TF1*)hFitter->GetUserFunc();

    // store arguments and number of parameters
    double x[3];
    f1->InitArgs(x,u);
    npar = f1->GetNpar();

    // initialization (copied from root)
    if( flag == 2 )
    {
      for( int k=0; k<npar; k++ )
      { gin[k] = 0; }

    }

    // initialize output
    out = 0;

    // loop over all bins
    for( int binX = hFitter->GetXfirst(); binX <= hFitter->GetXLast(); ++binX )
    for( int binY = hFitter->GetYfirst(); binY <= hFitter->GetYLast(); ++binY )
    for( int binZ = hFitter->GetZfirst(); binY <= hFitter->GetZLast(); ++binZ )
    {

      x[0] = hFit->GetXAxis()->GetBinCenter( binX );
      x[1] = hFit->GetYAxis()->GetBinCenter( binY );
      x[2] = hFit->GetZAxis()->GetBinCenter( binZ );
      if( !function->IsInside(x) ) continue;

      TF1::RejectPoint(false);
      const int bin = histogram->GetBin( binX, binY, binZ );
      const double measured( histogram->GetBinContent( bin );
      const double predicted = std::max( function->EvalPar( x, u ), 1e-9 );

      if( TF1::RejectedPoint() ) continue;

      nFitPoints++;

      // calculate log of poissonian probability to get measured, if predicted is the mean
      out -= ( measured*std::log(predicted) - predicted + TMath::LnGamma( measured+1 ) );

    }

    out *= 2;
    function->SetNumberFitPoints( nFitPoints );
    return;

}
