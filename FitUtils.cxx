// $Id: FitUtils.cxx,v 1.1 2008/07/31 16:52:30 hpereira Exp $

/*!
	\file Fit.cxx
	\brief some usefull functions for fits
	\author  Hugo Pereira
	\version $Revision: 1.1 $
	\date $Date: 2008/07/31 16:52:30 $
*/

#include <TROOT.h>
#include <TMath.h>
#include <TF1.h>
#include <cmath>

#include "FitUtils.h"
#include "ALI_MACRO.h"

using namespace UTILS;

//_______________________________________________________________________________
//! root dictionary
ClassImp(FitUtils);

//_______________________________________________________________________________
void FitUtils::Fit( TH1* h, const char* ffName, FitUtils::FitFunction Fcn, double min, double max, int n_par, double *par )
{
	TF1* f = new TF1( ffName, Fcn, min, max, n_par );
	if( par ) {
		for( int i=0; i<n_par; i++ )
			f->SetParameter( i, par[i] );
	}

	h->Fit( f, "0Q" );

	return;
}

//_______________________________________________________________________________
double FitUtils::Pol1( double *x, double *par)
{
	double u = x[0];
	double du = par[0]+par[1]*u;
	return du;
}

//_______________________________________________________________________________
double FitUtils::GausPol0( double *x, double *par)
{
	double xx = x[0];
	return par[0]*TMath::Exp( -0.5*ALI_MACRO::SQUARE( (xx-par[1])/par[2] ) )+ par[3];
}

//_______________________________________________________________________________
double FitUtils::GausGaus( double *x, double *par)
{
  return
    par[0]*TMath::Exp( -0.5*ALI_MACRO::SQUARE( (x[0]-par[1])/par[2] ) )+
    par[3]*TMath::Exp( -0.5*ALI_MACRO::SQUARE( (x[0]-par[4])/par[5] ) );
}

//_______________________________________________________________________________
double FitUtils::ErfErf( double *x, double *par)
{ return par[0]*TMath::Erf((x[0]-(par[1]-par[2]))/par[3])-par[0]*TMath::Erf((x[0]-(par[1]+par[2]))/par[3]); }

//_______________________________________________________________________________
double FitUtils::ErfErfPol0( double *x, double *par)
{
	double f = par[0]*TMath::Erf((x[0]-(par[1]-par[2]))/par[3])-par[0]*TMath::Erf((x[0]-(par[1]+par[2]))/par[3]);
	f+= par[4];
	return f;
}

//_______________________________________________________________________________
double FitUtils::ErfErf_gaus( double *x, double *par)
{
	double f = par[0]*TMath::Erf((x[0]-(par[1]-par[2]))/par[3])-par[0]*TMath::Erf((x[0]-(par[1]+par[2]))/par[3]);
	f+= par[4]*TMath::Exp( -0.5*ALI_MACRO::SQUARE( (x[0]-par[5])/par[6] ) );
	return f;
}

//_______________________________________________________________________________
double FitUtils::GausIntegrated( double *x, double *par)
{ return par[0]/(par[2]*TMath::Sqrt(2.0*TMath::Pi()))*TMath::Exp( -0.5*ALI_MACRO::SQUARE( (x[0]-par[1])/par[2] ) ); }

//_______________________________________________________________________________
double FitUtils::GausIntegratedExp( double *x, double *par)
{
  double g = par[0]/(par[2]*TMath::Sqrt(2.0*TMath::Pi()))*TMath::Exp( -0.5*ALI_MACRO::SQUARE( (x[0]-par[1])/par[2] ) );
  double e = par[3]*TMath::Exp( -(x[0]-par[1])/par[4] );
  return g+e;
}

//_______________________________________________________________________________
double FitUtils::GausGausIntegratedExp( double *x, double *par)
{
  double g1 = par[0]/(par[2]*TMath::Sqrt(2.0*TMath::Pi()))*TMath::Exp( -0.5*ALI_MACRO::SQUARE( (x[0]-par[1])/par[2] ) );
  double g2 = par[3]/(par[5]*TMath::Sqrt(2.0*TMath::Pi()))*TMath::Exp( -0.5*ALI_MACRO::SQUARE( (x[0]-par[4])/par[5] ) );
  double e = par[6]*TMath::Exp( -1*(x[0])/par[7] );
  return g1+g2+e;
}

//_______________________________________________________________________________
double FitUtils::GausGausIntegrated( double *x, double *par)
{
  double g1 = par[0]/(par[2]*TMath::Sqrt(2.0*TMath::Pi()))*TMath::Exp( -0.5*ALI_MACRO::SQUARE( (x[0]-par[1])/par[2] ) );
  double g2 = par[3]/(par[5]*TMath::Sqrt(2.0*TMath::Pi()))*TMath::Exp( -0.5*ALI_MACRO::SQUARE( (x[0]-par[4])/par[5] ) );
  return g1+g2;
}

//_______________________________________________________________________________
double FitUtils::Exp( double *x, double *par)
{ return par[0]*TMath::Exp( -par[1]*x[0] ); }

//____________________________________________
// fit an inverted landau distribution
double FitUtils::LandauInvert( double *x, double *par )
{ return par[0]*TMath::Landau( -x[0], par[1], par[2] ); }


//____________________________________________
// fit an inverted landau distribution
double FitUtils::FixedDoubleGaus( double *x, double *par )
{
  return
    par[0]*(
    (1.0-par[3])/(par[2]*TMath::Sqrt(2.0*TMath::Pi()))*TMath::Exp(-0.5*ALI_MACRO::SQUARE( (x[0] - par[1])/par[2] ) )
    	+ par[3]/(par[4]*TMath::Sqrt(2.0*TMath::Pi())) * TMath::Exp(-0.5*ALI_MACRO::SQUARE( (x[0] - par[1])/par[4] ) )
    ) + par[5]*( TMath::Exp(-(x[0]-1.7)/par[6]));

}

//____________________________________________
double FitUtils::CrystallBall0( double *x, double *par )
{ return par[0]*CrystallBall( x[0], par[1], par[2], par[3], par[4] ); }

//____________________________________________
double FitUtils::CrystallBall( double *x, double *par )
{

  // get normalized cristal ball
  double result = CrystallBall( x[0], par[1], par[2], par[3], par[4] );

  // get integral
  double integral = CrystallBallIntegral( par[2], par[3], par[4] );

  // return scaled cristalball so that par[0] corresponds to integral
  return par[0] * result / integral;

}

//____________________________________________
double FitUtils::CrystallBall( double x, double mean, double sigma, double alpha, double n )
{

  double t = (x-mean)/sigma;
  if( alpha < 0 ) t *= -1.0;

  alpha = fabs( alpha );
  if( t >= -alpha ) return TMath::Exp( -ALI_MACRO::SQUARE( t )/2 );
  else {

    double a = TMath::Power( n/alpha, n )*TMath::Exp( -ALI_MACRO::SQUARE( alpha )/2 );
    double b = n/alpha - alpha;
    return a/TMath::Power( b - t, n );

  }

}


//____________________________________________
double FitUtils::CrystallBall2( double *x, double *par )
{

  // get normalized cristal ball
  double result = CrystallBall2( x[0], par[1], par[2], par[3], par[4], par[5], par[6] );

  // get integral
  double integral = CrystallBall2Integral( par[2], par[3], par[4], par[5], par[6] );

  // return scaled cristalball so that par[0] corresponds to integral
  return par[0] * result/integral;

}

//____________________________________________
double FitUtils::CrystallBall2( double x, double mean, double sigma, double alpha1, double n1, double alpha2, double n2 )
{

  double t = (x-mean)/sigma;
  if( t < -alpha1 )
  {
    double a = TMath::Power( n1/alpha1, n1 )*TMath::Exp( -ALI_MACRO::SQUARE( alpha1 )/2 );
    double b = n1/alpha1 - alpha1;
    return a/TMath::Power( b - t, n1 );

  } else if( t > alpha2 ) {

    double a = TMath::Power( n2/alpha2, n2 )*TMath::Exp( -ALI_MACRO::SQUARE( alpha2 )/2 );
    double b = n2/alpha2 - alpha2;
    return a/TMath::Power( b + t, n2 );

  } else return TMath::Exp( -ALI_MACRO::SQUARE( t )/2 );


}

//____________________________________________
double FitUtils::CrystallBallIntegral( double sigma, double alpha, double n )
{
  // get corresponding integral
  alpha = fabs( alpha );
  return sigma*(
    n/(alpha*(n-1))*TMath::Exp( -ALI_MACRO::SQUARE( alpha )/2 ) +
    sqrt( TMath::Pi()/2)*TMath::Erfc( -alpha/sqrt(2) ));

}


//____________________________________________
double FitUtils::CrystallBall2Integral( double sigma, double alpha1, double n1, double alpha2, double n2 )
{
  // get corresponding integral
  alpha1 = fabs( alpha1 );
  alpha2 = fabs( alpha2 );
  return sigma*(
    n1/(alpha1*(n1-1))*TMath::Exp( -ALI_MACRO::SQUARE( alpha1 )/2 ) +
    n2/(alpha2*(n2-1))*TMath::Exp( -ALI_MACRO::SQUARE( alpha2 )/2 ) +
    sqrt( TMath::Pi()/2)*TMath::Erfc( -alpha1/sqrt(2) ) -
    sqrt( TMath::Pi()/2)*TMath::Erfc( alpha2/sqrt(2) ) );

}
