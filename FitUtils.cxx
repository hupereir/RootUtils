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
	return par[0]*std::exp( -0.5*ALI_MACRO::SQUARE( (xx-par[1])/par[2] ) )+ par[3];
}

//_______________________________________________________________________________
double FitUtils::GausGaus( double *x, double *par)
{
  return
    par[0]*std::exp( -0.5*ALI_MACRO::SQUARE( (*x-par[1])/par[2] ) )+
    par[3]*std::exp( -0.5*ALI_MACRO::SQUARE( (*x-par[4])/par[5] ) );
}

//_______________________________________________________________________________
double FitUtils::ErfErf( double *x, double *par)
{ return par[0]*TMath::Erf((*x-(par[1]-par[2]))/par[3])-par[0]*TMath::Erf((*x-(par[1]+par[2]))/par[3]); }

//_______________________________________________________________________________
double FitUtils::ErfErfPol0( double *x, double *par)
{
	double f = par[0]*TMath::Erf((*x-(par[1]-par[2]))/par[3])-par[0]*TMath::Erf((*x-(par[1]+par[2]))/par[3]);
	f+= par[4];
	return f;
}

//_______________________________________________________________________________
double FitUtils::ErfErf_gaus( double *x, double *par)
{
	double f = par[0]*TMath::Erf((*x-(par[1]-par[2]))/par[3])-par[0]*TMath::Erf((*x-(par[1]+par[2]))/par[3]);
	f+= par[4]*std::exp( -0.5*ALI_MACRO::SQUARE( (*x-par[5])/par[6] ) );
	return f;
}

//_______________________________________________________________________________
double FitUtils::GausIntegrated( double *x, double *par)
{
  return par[0]/(par[2]*sqrt(2.0*TMath::Pi()))*std::exp( -0.5*ALI_MACRO::SQUARE( (*x-par[1])/par[2] ) );
}

//_______________________________________________________________________________
double FitUtils::GausIntegratedExp( double *x, double *par)
{
  double g = par[0]/(par[2]*sqrt(2.0*TMath::Pi()))*std::exp( -0.5*ALI_MACRO::SQUARE( (*x-par[1])/par[2] ) );
  double e = par[3]*std::exp( -(*x-1.7)/par[4] );
  return g+e;
}

//_______________________________________________________________________________
double FitUtils::GausGausIntegratedExp( double *x, double *par)
{
  double g1 = par[0]/(par[2]*sqrt(2.0*TMath::Pi()))*std::exp( -0.5*ALI_MACRO::SQUARE( (*x-par[1])/par[2] ) );
  double g2 = par[3]/(par[5]*sqrt(2.0*TMath::Pi()))*std::exp( -0.5*ALI_MACRO::SQUARE( (*x-par[4])/par[5] ) );
  double e = par[6]*std::exp( -1*(*x)/par[7] );
  return g1+g2+e;
}

//_______________________________________________________________________________
double FitUtils::GausGausIntegrated( double *x, double *par)
{
  double g1 = par[0]/(par[2]*sqrt(2.0*TMath::Pi()))*std::exp( -0.5*ALI_MACRO::SQUARE( (*x-par[1])/par[2] ) );
  double g2 = par[3]/(par[5]*sqrt(2.0*TMath::Pi()))*std::exp( -0.5*ALI_MACRO::SQUARE( (*x-par[4])/par[5] ) );
  return g1+g2;
}

//_______________________________________________________________________________
double FitUtils::Exp( double *x, double *par)
{ return par[0]*std::exp( -par[1]*x[0] ); }

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
      (1.0-par[3])/(par[2]*sqrt(2.0*TMath::Pi()))*std::exp(-0.5*ALI_MACRO::SQUARE( (x[0] - par[1])/par[2] ) )
    	+ par[3]/(par[4]*sqrt(2.0*TMath::Pi())) * std::exp(-0.5*ALI_MACRO::SQUARE( (x[0] - par[1])/par[4] ) )
    ) + par[5]*( std::exp(-(x[0]-1.7)/par[6]));

}
