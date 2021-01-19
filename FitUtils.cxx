// $Id: FitUtils.cxx,v 1.1 2008/07/31 16:52:30 hpereira Exp $

#include "FitUtils.h"

#include "ROOT_MACRO.h"
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
double FitUtils::Gaus( double x, double mean, double sigma )
{ return TMath::Exp( -0.5*ROOT_MACRO::SQUARE( (x-mean)/sigma ) ); }

//_______________________________________________________________________________
double FitUtils::GausIntegrated( double x, double mean, double sigma )
{ return 1/(sigma*TMath::Sqrt(2.0*TMath::Pi()))*TMath::Exp( -0.5*ROOT_MACRO::SQUARE( (x-mean)/sigma ) ); }

//_______________________________________________________________________________
double FitUtils::GausIntegrated( double *x, double *par)
{ return par[0]*GausIntegrated( x[0], par[1], par[2] ); }

//_______________________________________________________________________________
double FitUtils::GausIntegratedExp( double *x, double *par)
{
  double g = par[0]*GausIntegrated( x[0], par[1], par[2] );
  double e = par[3]*TMath::Exp( -(x[0]-par[1])/par[4] );
  return g+e;
}

//_______________________________________________________________________________
double FitUtils::GausGausIntegratedExp( double *x, double *par)
{
  double g1 = par[0]*GausIntegrated( x[0], par[1], par[2] );
  double g2 = par[3]*GausIntegrated( x[0], par[4], par[5] );
  double e = par[6]*TMath::Exp( -1*(x[0])/par[7] );
  return g1+g2+e;
}

//_______________________________________________________________________________
double FitUtils::GausGausIntegrated( double *x, double *par)
{
  double g1 = par[0]*GausIntegrated( x[0], par[1], par[2] );
  double g2 = par[3]*GausIntegrated( x[0], par[4], par[5] );
  return g1+g2;
}

//_______________________________________________________________________________
double FitUtils::Exp( double *x, double *par)
{ return par[0]*TMath::Exp( -par[1]*x[0] ); }

//____________________________________________
double FitUtils::CrystalBall0( double *x, double *par )
{ return par[0]*CrystalBall( x[0], par[1], par[2], par[3], par[4] ); }

//____________________________________________
double FitUtils::CrystalBall( double *x, double *par )
{

  // get normalized Crystal ball
  double result = CrystalBall( x[0], par[1], par[2], par[3], par[4] );

  // get integral
  double integral = CrystalBallIntegral( par[2], par[3], par[4] );

  // return scaled Crystalball so that par[0] corresponds to integral
  return par[0] * result / integral;

}

//____________________________________________
double FitUtils::CrystalBall( double x, double mean, double sigma, double alpha, double n )
{

  double t = (x-mean)/sigma;
  if( alpha < 0 ) t *= -1.0;

  alpha = fabs( alpha );
  if( t >= -alpha ) return TMath::Exp( -ROOT_MACRO::SQUARE( t )/2 );
  else {

    double a = TMath::Power( n/alpha, n )*TMath::Exp( -ROOT_MACRO::SQUARE( alpha )/2 );
    double b = n/alpha - alpha;
    return a/TMath::Power( b - t, n );

  }

}

//____________________________________________
double FitUtils::CrystalBall2( double *x, double *par )
{

  // get normalized Crystal ball
  double result = CrystalBall2( x[0], par[1], par[2], par[3], par[4], par[5], par[6] );

  // get integral
  double integral = CrystalBall2Integral( par[2], par[3], par[4], par[5], par[6] );

  // return scaled Crystalball so that par[0] corresponds to integral
  return par[0] * result/integral;

}

//____________________________________________
double FitUtils::CrystalBall2( double x, double mean, double sigma, double alpha1, double n1, double alpha2, double n2 )
{

  double t = (x-mean)/sigma;
  if( t < -alpha1 )
  {
    double a = TMath::Power( n1/alpha1, n1 )*TMath::Exp( -ROOT_MACRO::SQUARE( alpha1 )/2 );
    double b = n1/alpha1 - alpha1;
    return a/TMath::Power( b - t, n1 );

  } else if( t > alpha2 ) {

    double a = TMath::Power( n2/alpha2, n2 )*TMath::Exp( -ROOT_MACRO::SQUARE( alpha2 )/2 );
    double b = n2/alpha2 - alpha2;
    return a/TMath::Power( b + t, n2 );

  } else return TMath::Exp( -ROOT_MACRO::SQUARE( t )/2 );

}

//____________________________________________
double FitUtils::CrystalBallIntegral( double sigma, double alpha, double n )
{
  // get corresponding integral
  alpha = fabs( alpha );
  return sigma*(
    n/(alpha*(n-1))*TMath::Exp( -ROOT_MACRO::SQUARE( alpha )/2 ) +
    TMath::Sqrt( TMath::Pi()/2 )*TMath::Erfc( -alpha/TMath::Sqrt(2) ) );

}

//____________________________________________
std::array<double,2> FitUtils::CrystalBallFractions( double alpha, double n )
{
  // get corresponding integral
  alpha = fabs( alpha );
  const double left = n/(alpha*(n-1))*TMath::Exp( -ROOT_MACRO::SQUARE( alpha )/2 );
  const double core = TMath::Sqrt( TMath::Pi()/2 )*TMath::Erfc( -alpha/TMath::Sqrt(2) );
  const double sum = left + core;
  return {{ left/sum, core/sum }};
}

//____________________________________________
double FitUtils::CrystalBall2Integral( double sigma, double alpha1, double n1, double alpha2, double n2 )
{
  // get corresponding integral
  alpha1 = fabs( alpha1 );
  alpha2 = fabs( alpha2 );
  return sigma*(
    n1/(alpha1*(n1-1))*TMath::Exp( -ROOT_MACRO::SQUARE( alpha1 )/2 ) +
    n2/(alpha2*(n2-1))*TMath::Exp( -ROOT_MACRO::SQUARE( alpha2 )/2 ) +
    TMath::Sqrt( TMath::Pi()/2)*TMath::Erfc( -alpha1/TMath::Sqrt(2) ) -
    TMath::Sqrt( TMath::Pi()/2)*TMath::Erfc( alpha2/TMath::Sqrt(2) ) );

}

//____________________________________________
std::array<double, 3> FitUtils::CrystalBall2Fractions(double alpha1, double n1, double alpha2, double n2 )
{
  // get corresponding integral
  alpha1 = fabs( alpha1 );
  alpha2 = fabs( alpha2 );
  
  const double left = n1/(alpha1*(n1-1))*TMath::Exp( -ROOT_MACRO::SQUARE( alpha1 )/2 );
  const double right = n2/(alpha2*(n2-1))*TMath::Exp( -ROOT_MACRO::SQUARE( alpha2 )/2 );
  const double core = TMath::Sqrt( TMath::Pi()/2)*TMath::Erfc( -alpha1/TMath::Sqrt(2) ) - TMath::Sqrt( TMath::Pi()/2)*TMath::Erfc( alpha2/TMath::Sqrt(2) );
  const double sum = left + right + core;
  return {{ left/sum, core/sum, right/sum }};
  
}

//____________________________________________
double FitUtils::VWG( double* x, double* par )
{ return par[0]*VWG( x[0], par[1], par[2], par[3] ); }

//____________________________________________
double FitUtils::VWG( double x, double mean, double sigma, double slope )
{
  const double sigmaTmp = sigma+slope*((x-mean)/mean);
  return TMath::Exp( -0.5*ROOT_MACRO::SQUARE( (x-mean)/sigmaTmp ) );
}

//____________________________________________
double FitUtils::VWG2( double* x, double* par )
{ return par[0]*VWG2( x[0], par[1], par[2], par[3], par[4] ); }

//____________________________________________
double FitUtils::VWG2( double x, double mean, double sigma, double slope, double slopeQuad )
{
  const double sigmaTmp = sigma+slope*((x-mean)/mean) + slopeQuad*ROOT_MACRO::SQUARE((x-mean)/mean);
  return TMath::Exp( -0.5*ROOT_MACRO::SQUARE( (x-mean)/sigmaTmp ) );
}

//____________________________________________
double FitUtils::Na60Old( double* x, double* par )
{
  double tail1[3] = { par[3], par[4], par[5] };
  double tail2[3] = { par[6], par[7], par[8] };
  return par[0]*Na60Old( x[0], par[1], par[2], tail1, tail2, par[9], par[10] );
}

//____________________________________________
// there are 3 parameters for each tail
double FitUtils::Na60Old(
  double mass,
  double mean, double sigma,
  double* tail1,
  double* tail2,
  double massRatio1, double massRatio2
  )
{
  const double mass1 = massRatio1*mean;
  const double mass2 = massRatio2*mean;

  double sigmaTmp;
  if( mass < mass1 ) sigmaTmp = sigma*( 1.0 + TMath::Power( tail1[0]*(mass1-mass), tail1[1]-tail1[2]*TMath::Sqrt(mass1-mass)));
  else if( mass >= mass1 && mass < mass2 ) sigmaTmp = sigma;
  else if( mass >= mass2 ) sigmaTmp = sigma*( 1.0 + TMath::Power( tail2[0]*(mass-mass2), tail2[1]-tail2[2]*TMath::Sqrt(mass-mass2)));

  return TMath::Exp( -0.5*ROOT_MACRO::SQUARE( (mass-mean)/sigmaTmp ) );
}

//____________________________________________
double FitUtils::Na60New( double* x, double* par )
{
  double tail1[3] = { par[3], par[4], par[5] };
  double tail2[3] = { par[6], par[7], par[8] };
  return par[0]*Na60New( x[0], par[1], par[2], tail1, tail2, par[9], par[10] );
}

//____________________________________________
// there are 3 parameters for each tail
double FitUtils::Na60New(
  double mass,
  double mean, double sigma,
  double* tail1,
  double* tail2,
  double alpha1, double alpha2
  )
{

  const double t = (mass-mean)/sigma;

  double sigmaRatio;
  if( t < alpha1 ) sigmaRatio = ( 1.0 + TMath::Power( tail1[0]*(alpha1-t), tail1[1]-tail1[2]*TMath::Sqrt(alpha1 - t) ) );
  else if( t >= alpha1 && t < alpha2 ) sigmaRatio = 1;
  else if( t >= alpha2 ) sigmaRatio = ( 1.0 + TMath::Power( tail2[0]*(t-alpha2), tail2[1]-tail2[2]*TMath::Sqrt(t - alpha2) ) );

  return TMath::Exp( -ROOT_MACRO::SQUARE( t/sigmaRatio )/2 );

}
