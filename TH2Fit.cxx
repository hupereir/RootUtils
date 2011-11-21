// $Id: TH2Fit.cxx,v 1.4 2007/09/12 10:16:09 hpereira Exp $
#include "TH2Fit.h"
#include <TH2.h>
#include <TF1.h>
#include <TMinuit.h>
#include <cmath>

/*!
	\file TH2Fit.cxx
	\brief	 to perform minuit fit of a 2D histogram using any x vs y function.
	\author Hugo Pereira
	\version $Revision: 1.4 $
	\date $Date: 2007/09/12 10:16:09 $
*/


//__________________________________
TH2* TH2Fit::fHStatic = 0;
TF1* TH2Fit::fFStatic = 0;
unsigned int TH2Fit::fNParametersStatic = 0;

// Rem: xMax<xMin means that histogram range is used
double TH2Fit::fXMin = 1;
double TH2Fit::fXMax = -1;


//___________________________
TH2Fit::TH2Fit( TF1* f, const unsigned int nP ):
	f_( f ), fNParameters( nP )
{
	// initialise minuit
	gMinuit = new TMinuit( fNParameters );
	gMinuit->mninit(5,6,7);

	// set starting parameters
	double flag = -1;
	int error;
	gMinuit->mnexcm("SET PRINT", &flag ,1,error); // disable all printouts
	for( unsigned int iP=0; iP<fNParameters; iP++ ) {
		char* pName = new char[15];
		sprintf(pName,"parameter_%i",iP );
		gMinuit->mnparm(iP,pName, f_->GetParameter( iP ), 1, 0, 0, error);
		if (error) std::cout << "TH2Fit::TH2Fit - ERROR:Troubles defining parameter" << iP << std::endl;
		SafeDelete( pName );
	}
	return;
}

//__________________________________________________________________
int TH2Fit::ExecMinuitCommand( const char* command, double* pList, int size )
{
	int error;
	gMinuit->mnexcm( command, pList, size, error);
	return error;
}

//__________________________________________________________________
bool TH2Fit::Fit( TH2* h, const double xMin, const double xMax )
{

	// copy function and histogram into static vars
	fFStatic = f_;
	fNParametersStatic = fNParameters;
	fHStatic = h;

	// set limits
	if( xMax < xMin ) {
		std::cout << "TH2Fit::fit - INFO: Fit range is histogram range.\n";
		fXMin =	fHStatic->GetXaxis()->GetBinCenter( 1 );
		fXMax =	fHStatic->GetXaxis()->GetBinCenter( fHStatic->GetNbinsX() );
	} else {
		fXMin = xMin;
		fXMax = xMax;
	}

	// set Minuit minimisation function
	gMinuit->SetFCN(Fcn);

	// Do the fit
	int error;
	double flag0 = 0, flag1 = 1, flag3 = 3;
	gMinuit->mnexcm("CALL FCN",	&flag1 ,1,error);
	gMinuit->mnexcm("SET NOGradient", &flag0 ,1,error);
	gMinuit->mnexcm("MIGRAD",	 &flag0 ,0,error);
	gMinuit->mnexcm("CALL FCN", &flag3 ,1,error);

	return true;
}

//___________________________________________________________
bool TH2Fit::FitInverted( TH2* h, const double yMin, const double yMax	)
{

	// copy function and histogram into static vars
	fFStatic	= f_;
	fNParametersStatic = fNParameters;
	fHStatic = h;

	// set limits
	if( yMax < yMin ) {
		std::cout << "TH2Fit::FitInverted - INFO: Fit range is histogram range.\n";
		fXMin =	fHStatic->GetXaxis()->GetBinCenter( 1 );
		fXMax =	fHStatic->GetXaxis()->GetBinCenter( fHStatic->GetNbinsX() );
	} else {
		fXMin = yMin;
		fXMax = yMax;
	}

	// set Minuit minimisation function
	gMinuit->SetFCN(FcnInv);

	// Do the fit
	int error;
	double flag0 = 0, flag1 = 1, flag3 = 3;
	gMinuit->mnexcm("CALL FCN",	&flag1 ,1,error);
	gMinuit->mnexcm("SET PRINT", &flag0 ,1,error);
	gMinuit->mnexcm("SET NOGradient", &flag0 ,1,error);
	gMinuit->mnexcm("MIGRAD",	 &flag0 ,0,error);
	gMinuit->mnexcm("CALL FCN", &flag3 ,1,error);
	return true;
}

//_____________________________________________________
void TH2Fit::Fcn( int& npar, double *gin, double &res, double *par, int flag )
{
	// Get Histogram and related infos
	if( !fHStatic ) {
		std::cout << "TH2Fit::Fcn - FATAL: histogram not set" << std::endl;
		return;
	}
	unsigned int nX = fHStatic->GetNbinsX();
	unsigned int nY = fHStatic->GetNbinsY();

	// Get Function and set parameters
	if( !fFStatic ) {
		std::cout << "TH2Fit::Fcn - FATAL: function not set" << std::endl;
		return;
	}

	for( unsigned int iP = 0; iP < fNParametersStatic; iP++ )
	fFStatic->SetParameter( iP, par[iP] );

	// Scan all bins, calculate result
	double nEnt = 0;
	res = 0;
	for( unsigned int iX = 1; iX <= nX; iX++ )
	for( unsigned int iY = 1; iY <= nY; iY++ ) {
		double x = fHStatic->GetXaxis()->GetBinCenter( iX );
		double y = fHStatic->GetYaxis()->GetBinCenter( iY );
		double n = fHStatic->GetBinContent( iX, iY );
		if( x >= fXMin && x < fXMax ) {
			res+= n* pow( y - fFStatic->Eval( x ), 2 );
			nEnt+=n;
		}
	}
	res/=nEnt;

	return;
}

//_____________________________________________________
void TH2Fit::FcnInv( int& npar, double *gin, double &res, double *par, int flag )
{
	// Get Histogram and related infos
	if( !fHStatic ) {
		std::cout << "TH2Fit::FcnInv - FATAL: histogram not set" << std::endl;
		return;
	}
	unsigned int nX = fHStatic->GetNbinsX();
	unsigned int nY = fHStatic->GetNbinsY();

	// Get Function and set parameters
	if( !fFStatic ) {
		std::cout << "TH2Fit::FcnInv - FATAL: function not set" << std::endl;
		return;
	}

	for( unsigned int iP = 0; iP < fNParametersStatic; iP++ )
	fFStatic->SetParameter( iP, par[iP] );

	// Scan all bins, calculate result
	double nEnt = 0;
	res = 0;
	for( unsigned int iX = 1; iX <= nX; iX++ )
	for( unsigned int iY = 1; iY <= nY; iY++ ) {
		double x = fHStatic->GetXaxis()->GetBinCenter( iX );
		double y = fHStatic->GetYaxis()->GetBinCenter( iY );
		double n = fHStatic->GetBinContent( iX, iY );

		// warning fXMin and xMax here apply to y!
		if( y >= fXMin && y < fXMax ) {
			res+= n* pow( x - fFStatic->Eval( y, 0, 0 ), 2 );
			nEnt+=n;
		}
	}
	res/=nEnt;
	return;
}
