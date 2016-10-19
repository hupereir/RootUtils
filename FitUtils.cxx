// $Id: FitUtils.cxx,v 1.1 2008/07/31 16:52:30 hpereira Exp $

#include "FitUtils.h"

#include "ALI_MACRO.h"
#include "ChisquareFitter.h"
#include "LikelihoodFitter.h"

#include <TROOT.h>
#include <TMath.h>
#include <TF1.h>
#include <TVirtualFitter.h>
#include <cmath>

using namespace UTILS;

//_______________________________________________________________________________
//* root dictionary
ClassImp(FitUtils);

//_______________________________________________________________________________
void FitUtils::Fit( TH1* h, const char* ffName, FitUtils::FitFunction Fcn, Double_t min, Double_t max, int n_par, Double_t *par )
{
  TF1* f = new TF1( ffName, Fcn, min, max, n_par );
  if( par )
  {
    for( int i=0; i<n_par; i++ )
    { f->SetParameter( i, par[i] ); }
  }

  h->Fit( f, "0Q" );

  return;
}

//_______________________________________________________________________________
TFitResultPtr FitUtils::Fit( TH1* h, TF1* f, TString option )
{

  // setup virtual fitter
  if( option.Contains( "U" ) )
  {

    if( option.Contains( "L" ) )
    {

      std::cout << "FitUtils::Fit - using local Likelihood fitter" << std::endl;
      TVirtualFitter::Fitter(h)->SetFCN( LikelihoodFitter::Fcn );

    } else {

      std::cout << "FitUtils::Fit - using local chisquare fitter" << std::endl;
      TVirtualFitter::Fitter(h)->SetFCN( ChisquareFitter::Fcn );

    }

  } else if( option.Contains( "L" ) ) {

    std::cout << "FitUtils::Fit - using default Likelihood fitter" << std::endl;

  } else {

    std::cout << "FitUtils::Fit - using default chisquare fitter" << std::endl;

  }

  // fit
  TFitResultPtr result =  h->Fit( f, option );

  if( option.Contains( "U" ) )
  {

    std::cout << "FitUtils::Fit - calculating chisquare manually" << std::endl;
    f->SetChisquare( ChisquareFitter::Chisquare( h, f ) );

  }

  return result;

}


//_______________________________________________________________________________
Double_t FitUtils::Gaus( Double_t x, Double_t mean, Double_t sigma )
{ return TMath::Exp( -0.5*ALI_MACRO::SQUARE( (x-mean)/sigma ) ); }

//_______________________________________________________________________________
Double_t FitUtils::GausIntegrated( Double_t x, Double_t mean, Double_t sigma )
{ return 1/(sigma*TMath::Sqrt(2.0*TMath::Pi()))*TMath::Exp( -0.5*ALI_MACRO::SQUARE( (x-mean)/sigma ) ); }

//_______________________________________________________________________________
Double_t FitUtils::GausIntegrated( Double_t *x, Double_t *par)
{ return par[0]*GausIntegrated( x[0], par[1], par[2] ); }

//_______________________________________________________________________________
Double_t FitUtils::GausIntegratedExp( Double_t *x, Double_t *par)
{
  Double_t g = par[0]*GausIntegrated( x[0], par[1], par[2] );
  Double_t e = par[3]*TMath::Exp( -(x[0]-par[1])/par[4] );
  return g+e;
}

//_______________________________________________________________________________
Double_t FitUtils::GausGausIntegratedExp( Double_t *x, Double_t *par)
{
  Double_t g1 = par[0]*GausIntegrated( x[0], par[1], par[2] );
  Double_t g2 = par[3]*GausIntegrated( x[0], par[4], par[5] );
  Double_t e = par[6]*TMath::Exp( -1*(x[0])/par[7] );
  return g1+g2+e;
}

//_______________________________________________________________________________
Double_t FitUtils::GausGausIntegrated( Double_t *x, Double_t *par)
{
  Double_t g1 = par[0]*GausIntegrated( x[0], par[1], par[2] );
  Double_t g2 = par[3]*GausIntegrated( x[0], par[4], par[5] );
  return g1+g2;
}

//_______________________________________________________________________________
Double_t FitUtils::Exp( Double_t *x, Double_t *par)
{ return par[0]*TMath::Exp( -par[1]*x[0] ); }

//____________________________________________
Double_t FitUtils::CrystallBall0( Double_t *x, Double_t *par )
{ return par[0]*CrystallBall( x[0], par[1], par[2], par[3], par[4] ); }

//____________________________________________
Double_t FitUtils::CrystallBall( Double_t *x, Double_t *par )
{

  // get normalized Crystal ball
  Double_t result = CrystallBall( x[0], par[1], par[2], par[3], par[4] );

  // get integral
  Double_t integral = CrystallBallIntegral( par[2], par[3], par[4] );

  // return scaled Crystalball so that par[0] corresponds to integral
  return par[0] * result / integral;

}

//____________________________________________
Double_t FitUtils::CrystallBall( Double_t x, Double_t mean, Double_t sigma, Double_t alpha, Double_t n )
{

  Double_t t = (x-mean)/sigma;
  if( alpha < 0 ) t *= -1.0;

  alpha = fabs( alpha );
  if( t >= -alpha ) return TMath::Exp( -ALI_MACRO::SQUARE( t )/2 );
  else {

    Double_t a = TMath::Power( n/alpha, n )*TMath::Exp( -ALI_MACRO::SQUARE( alpha )/2 );
    Double_t b = n/alpha - alpha;
    return a/TMath::Power( b - t, n );

  }

}

//____________________________________________
Double_t FitUtils::CrystallBall2( Double_t *x, Double_t *par )
{

  // get normalized Crystal ball
  Double_t result = CrystallBall2( x[0], par[1], par[2], par[3], par[4], par[5], par[6] );

  // get integral
  Double_t integral = CrystallBall2Integral( par[2], par[3], par[4], par[5], par[6] );

  // return scaled Crystalball so that par[0] corresponds to integral
  return par[0] * result/integral;

}

//____________________________________________
Double_t FitUtils::CrystallBall2( Double_t x, Double_t mean, Double_t sigma, Double_t alpha1, Double_t n1, Double_t alpha2, Double_t n2 )
{

  Double_t t = (x-mean)/sigma;
  if( t < -alpha1 )
  {
    Double_t a = TMath::Power( n1/alpha1, n1 )*TMath::Exp( -ALI_MACRO::SQUARE( alpha1 )/2 );
    Double_t b = n1/alpha1 - alpha1;
    return a/TMath::Power( b - t, n1 );

  } else if( t > alpha2 ) {

    Double_t a = TMath::Power( n2/alpha2, n2 )*TMath::Exp( -ALI_MACRO::SQUARE( alpha2 )/2 );
    Double_t b = n2/alpha2 - alpha2;
    return a/TMath::Power( b + t, n2 );

  } else return TMath::Exp( -ALI_MACRO::SQUARE( t )/2 );

}

//____________________________________________
Double_t FitUtils::CrystallBallIntegral( Double_t sigma, Double_t alpha, Double_t n )
{
  // get corresponding integral
  alpha = fabs( alpha );
  return sigma*(
    n/(alpha*(n-1))*TMath::Exp( -ALI_MACRO::SQUARE( alpha )/2 ) +
    TMath::Sqrt( TMath::Pi()/2 )*TMath::Erfc( -alpha/TMath::Sqrt(2) ) );

}

//____________________________________________
Double_t FitUtils::CrystallBall2Integral( Double_t sigma, Double_t alpha1, Double_t n1, Double_t alpha2, Double_t n2 )
{
  // get corresponding integral
  alpha1 = fabs( alpha1 );
  alpha2 = fabs( alpha2 );
  return sigma*(
    n1/(alpha1*(n1-1))*TMath::Exp( -ALI_MACRO::SQUARE( alpha1 )/2 ) +
    n2/(alpha2*(n2-1))*TMath::Exp( -ALI_MACRO::SQUARE( alpha2 )/2 ) +
    TMath::Sqrt( TMath::Pi()/2)*TMath::Erfc( -alpha1/TMath::Sqrt(2) ) -
    TMath::Sqrt( TMath::Pi()/2)*TMath::Erfc( alpha2/TMath::Sqrt(2) ) );

}

//____________________________________________
Double_t FitUtils::VWG( Double_t* x, Double_t* par )
{ return par[0]*VWG( x[0], par[1], par[2], par[3] ); }

//____________________________________________
Double_t FitUtils::VWG( Double_t x, Double_t mean, Double_t sigma, Double_t slope )
{
  const Double_t sigmaTmp = sigma+slope*((x-mean)/mean);
  return TMath::Exp( -0.5*ALI_MACRO::SQUARE( (x-mean)/sigmaTmp ) );
}

//____________________________________________
Double_t FitUtils::Na60Old( Double_t* x, Double_t* par )
{
  Double_t tail1[3] = { par[3], par[4], par[5] };
  Double_t tail2[3] = { par[6], par[7], par[8] };
  return par[0]*Na60Old( x[0], par[1], par[2], tail1, tail2, par[9], par[10] );
}

//____________________________________________
// there are 3 parameters for each tail
Double_t FitUtils::Na60Old(
  Double_t mass,
  Double_t mean, Double_t sigma,
  Double_t* tail1,
  Double_t* tail2,
  Double_t massRatio1, Double_t massRatio2
  )
{
  const Double_t mass1 = massRatio1*mean;
  const Double_t mass2 = massRatio2*mean;

  Double_t sigmaTmp;
  if( mass < mass1 ) sigmaTmp = sigma*( 1.0 + TMath::Power( tail1[0]*(mass1-mass), tail1[1]-tail1[2]*TMath::Sqrt(mass1-mass)));
  else if( mass >= mass1 && mass < mass2 ) sigmaTmp = sigma;
  else if( mass >= mass2 ) sigmaTmp = sigma*( 1.0 + TMath::Power( tail2[0]*(mass-mass2), tail2[1]-tail2[2]*TMath::Sqrt(mass-mass2)));

  return TMath::Exp( -0.5*ALI_MACRO::SQUARE( (mass-mean)/sigmaTmp ) );
}

//____________________________________________
Double_t FitUtils::Na60New( Double_t* x, Double_t* par )
{
  Double_t tail1[3] = { par[3], par[4], par[5] };
  Double_t tail2[3] = { par[6], par[7], par[8] };
  return par[0]*Na60New( x[0], par[1], par[2], tail1, tail2, par[9], par[10] );
}

//____________________________________________
// there are 3 parameters for each tail
Double_t FitUtils::Na60New(
  Double_t mass,
  Double_t mean, Double_t sigma,
  Double_t* tail1,
  Double_t* tail2,
  Double_t alpha1, Double_t alpha2
  )
{

  const Double_t t = (mass-mean)/sigma;

  Double_t sigmaRatio;
  if( t < alpha1 ) sigmaRatio = ( 1.0 + TMath::Power( tail1[0]*(alpha1-t), tail1[1]-tail1[2]*TMath::Sqrt(alpha1 - t) ) );
  else if( t >= alpha1 && t < alpha2 ) sigmaRatio = 1;
  else if( t >= alpha2 ) sigmaRatio = ( 1.0 + TMath::Power( tail2[0]*(t-alpha2), tail2[1]-tail2[2]*TMath::Sqrt(t - alpha2) ) );

  return TMath::Exp( -ALI_MACRO::SQUARE( t/sigmaRatio )/2 );

}
