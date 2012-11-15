// $Id: FitUtils.h,v 1.1 2008/07/31 16:52:31 hpereira Exp $
#ifndef FitUtils_h
#define FitUtils_h

/*!
	\file	 Fit.h
	\brief   some usefull functions for fits
	\author  Hugo Pereira
	\version $Revision: 1.1 $
	\date	 $Date: 2008/07/31 16:52:31 $
*/

#include<TROOT.h>
#include<TObject.h>
#include<TH1.h>

/*!
	\class   Fit
	\brief   some usefull functions for fits
*/

namespace UTILS
{

  class FitUtils: public TObject {
    public:

    // 	//! log likelyhood customized fitter
    // 	/*!
    // 		note that the parameters are absolutely not explained
    // 		in root documentation.
    // 	*/
    // 	static void likelihood(
    // 			int &npar,
    // 			double *gin,
    // 			double &f,
    // 			double *u,
    // 			int flag );


    //! shortcut for fit function
    typedef double (*FitFunction) (double*, double*);

    //! embeded fit
    static void Fit( TH1*, const char* ffName, FitFunction, double min, double max, int n_par, double *pars = 0 );

    //! straight line, 2 parameters
    static double Pol1( double *x, double *par );

    //! gaussian + P0, 4 parameters
    static double GausPol0( double *x, double *par );

    //! two gaussians, 6 parameters
    static double GausGaus( double *x, double *par );

    //! two Erf
    static double ErfErf( double *x, double *par );

    //! two Erf + P0 (5 pars)
    static double ErfErfPol0( double *x, double *par );

    //! two Erf(4 pars) + a gaussian (3 pars)
    static double ErfErf_gaus( double *x, double *par );

    //! gaussian (integ,ave,sigma)
    static double GausIntegrated( double *x, double *par );

    //! 1 gaussian (integ,ave,sigma)+exp
    static double GausIntegratedExp( double *x, double *par );

    //! 2 parameters exponential
    static double Exp( double *x, double *pal );

    //! 2 gaussian (integ,ave,sigma)+exp
    static double GausGausIntegratedExp( double *x, double *par );

    //! 2 gaussian (integ,ave,sigma)+exp
    static double GausGausIntegrated( double *x, double *par );

    //! landau function of (par[4]-x)
    static double LandauInvert( double *x, double *par );

    //! double gaussian + exp fit - 8 parameters
    static double FixedDoubleGaus( double *x, double *par );

    //! cristall ball fit, using Amplitude for first parameter
    static double CrystallBall0( double *x, double *par );

    //! cristal ball
    static double CrystallBall( double *x, double *par );

    //! cristal ball
    static double CrystallBall( double x, double mean, double sigma, double alpha, double n );

    //! cristal ball
    static double CrystallBallIntegral( double sigma, double alpha, double n );

    //! cristal ball
    static double CrystallBall2( double *x, double *par );

    //! cristal ball
    static double CrystallBall2(
      double x, double mean, double sigma,
      double alpha, double n,
      double alpha2, double n2 );

    //! cristal ball
    static double CrystallBall2Integral(
      double sigma,
      double alpha, double n,
      double alpha2, double n2 );

    ClassDef(FitUtils,0)
  };

};

#endif
