#ifndef FitUtils_h
#define FitUtils_h

#include <TROOT.h>
#include <TObject.h>
#include <TH1.h>
#include <TF1.h>
#include <TFitResultPtr.h>

/*!
	\class   Fit
	\brief   some usefull functions for fits
*/

namespace UTILS
{

  class FitUtils: public TObject
  {

    public:

    /// fit
    static TFitResultPtr Fit( TH1*, TF1*, TString );

    //* normalized gauss
    static double Gaus( double x, double mean, double sigma );

    //* normalized gauss
    static double GausIntegrated( double x, double mean, double sigma );

    //* gaussian, using integral for first parameter
    static double GausIntegrated( double *x, double *par );

    //* 1 gaussian, using integral for first parameter + exp
    static double GausIntegratedExp( double *x, double *par );

    //* 2 parameters exponential
    static double Exp( double *x, double *par );

    //* 2 gaussian, using integral for first parameter + exp
    static double GausGausIntegratedExp( double *x, double *par );

    //* 2 gaussian (integ,ave,sigma)+exp
    static double GausGausIntegrated( double *x, double *par );

    //* Crystall ball fit, using Amplitude for first parameter
    static double CrystalBall0( double *x, double *par );

    //* Crystal ball, using integral for first parameter
    static double CrystalBall( double *x, double *par );

    //* Crystal ball
    static double CrystalBall( double x, double mean, double sigma, double alpha, double n );

    //* Crystal ball integral
    static double CrystalBallIntegral( double sigma, double alpha, double n );

    //* Crystal ball integral
    static std::array<double,2> CrystalBallFractions( double alpha, double n );

    //* Crystal ball (with tails on both sides), using integral for first parameter
    static double CrystalBall2( double *x, double *par );

    //* Crystal ball
    static double CrystalBall2(
      double x, double mean, double sigma,
      double alpha, double n,
      double alpha2, double n2 );

    //* Crystal ball
    static double CrystalBall2Integral(
      double sigma,
      double alpha, double n,
      double alpha2, double n2 );

    //* Crystal ball
    static std::array<double,3> CrystalBall2Fractions(double alpha, double n, double alpha2, double n2 );

    //* variable width gaussian
    static double VWG( double* x, double* par );

    //* variable width gaussian
    static double VWG( double x, double mean, double sigma, double slope );

    //* variable width gaussian
    static double VWG2( double* x, double* par );

    //* variable width gaussian
    static double VWG2( double x, double mean, double sigma, double slope, double slopeQuad );

    //* Na60 function
    static double Na60Old( double*, double* par );

    //* Na60 fit function
    //* tails have 3 parameters each
    static double Na60Old(
      double mass,
      double mean, double sigma,
      double* tail1,
      double* tail2,
      double massRatio1, double massRatio2
      );

    //* Na60 function
    static double Na60New( double*, double* par );

    //* Na60 fit function
    //* tails have 3 parameters each
    static double Na60New(
      double mass,
      double mean, double sigma,
      double* tail1,
      double* tail2,
      double alpha1, double alpha2
      );

    ClassDef(FitUtils,0)
  };

};

#endif
