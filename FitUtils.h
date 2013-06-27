// $Id: FitUtils.h,v 1.1 2008/07/31 16:52:31 hpereira Exp $
#ifndef FitUtils_h
#define FitUtils_h

/*!
	\file	 Fit.h
	\brief some usefull functions for fits
	\author Hugo Pereira
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

  class FitUtils: public TObject
  {

    public:

    //! shortcut for fit function
    typedef Double_t (*FitFunction) (double*, double*);

    //! embeded fit
    static void Fit( TH1*, const char* ffName, FitFunction, Double_t min, Double_t max, int n_par, Double_t *pars = 0 );

    //! normalized gauss
    static Double_t Gaus( Double_t x, Double_t mean, Double_t sigma );

    //! gaussian, using integral for first parameter
    static Double_t GausIntegrated( Double_t *x, Double_t *par );

    //! 1 gaussian, using integral for first parameter + exp
    static Double_t GausIntegratedExp( Double_t *x, Double_t *par );

    //! 2 parameters exponential
    static Double_t Exp( Double_t *x, Double_t *par );

    //! 2 gaussian, using integral for first parameter + exp
    static Double_t GausGausIntegratedExp( Double_t *x, Double_t *par );

    //! 2 gaussian (integ,ave,sigma)+exp
    static Double_t GausGausIntegrated( Double_t *x, Double_t *par );

    //! Crystall ball fit, using Amplitude for first parameter
    static Double_t CrystallBall0( Double_t *x, Double_t *par );

    //! Crystal ball, using integral for first parameter
    static Double_t CrystallBall( Double_t *x, Double_t *par );

    //! Crystal ball
    static Double_t CrystallBall( Double_t x, Double_t mean, Double_t sigma, Double_t alpha, Double_t n );

    //! Crystal ball integral
    static Double_t CrystallBallIntegral( Double_t sigma, Double_t alpha, Double_t n );

    //! Crystal ball (with tails on both sides), using integral for first parameter
    static Double_t CrystallBall2( Double_t *x, Double_t *par );

    //! Crystal ball
    static Double_t CrystallBall2(
      Double_t x, Double_t mean, Double_t sigma,
      Double_t alpha, Double_t n,
      Double_t alpha2, Double_t n2 );

    //! Crystal ball
    static Double_t CrystallBall2Integral(
      Double_t sigma,
      Double_t alpha, Double_t n,
      Double_t alpha2, Double_t n2 );

    //! variable width gaussian
    static Double_t VWG( double* x, double* par );

    //! variable width gaussian
    static Double_t VWG( Double_t x, Double_t mean, Double_t sigma, Double_t slope );

    //! Na60 function
    static Double_t Na60( double*, double* par );

    //! Na60 fit function
    //! tails have 3 parameters each
    static Double_t Na60(
      Double_t mass,
      Double_t mean, Double_t sigma,
      double* tail1,
      double* tail2,
      Double_t massRatio1, Double_t massRatio2
      );

    //! Na60 function
    static Double_t Na60Old( double*, double* par );

    //! Na60 fit function
    //! tails have 3 parameters each
    static Double_t Na60Old(
      Double_t mass,
      Double_t mean, Double_t sigma,
      double* tail1,
      double* tail2,
      Double_t massRatio1, Double_t massRatio2
      );

    //! Na60 function
    static Double_t Na60New( double*, double* par );

    //! Na60 fit function
    //! tails have 3 parameters each
    static Double_t Na60New(
      Double_t mass,
      Double_t mean, Double_t sigma,
      double* tail1,
      double* tail2,
      Double_t alpha1, Double_t alpha2
      );

    ClassDef(FitUtils,0)
  };

};

#endif
