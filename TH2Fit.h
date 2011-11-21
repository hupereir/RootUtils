// $Id: TH2Fit.h,v 1.4 2006/06/29 16:50:31 hpereira Exp $

#ifndef TH2Fit_h
#define TH2Fit_h
/*!
\file	 TH2Fit.h
\brief   to perform minuit fit of a 2D histogram using any x vs y function.
\author  Hugo Pereira
\version $Revision: 1.4 $
\date	 $Date: 2006/06/29 16:50:31 $
*/

#include <string>
#include <iostream>
#include <TROOT.h>
#include <TObject.h>
class TH2;
class TF1;
class TMinuit;

/*!
\class   TH2Fit
\brief   to perform minuit fit of a 2D histogram using any x vs y function.
*/
class TH2Fit {
  public:

  /*!
  \fn TH2Fit( TF1* f, const unsigned int nP )
  creator
  \param f is the fit function.
  \param nP the number of parameters
  */
  TH2Fit( TF1* f, const unsigned int nP );

  /*!
  \fn int ExecMinuitCommand( const char* command, double* pList, int size )
  to execute minuit commands
  */
  int ExecMinuitCommand( const char* command, double* pList, int size );

  /*!
  \fn void Fit( TH2* h, const double xMin = 1, const double xMax = -1 )
  \brief perform the fit. By default, histogram range is used.
  \param h the histogram to be fitted
  \param xMin min value for fit range
  \param xMax max value for fit range
  */
  bool Fit( TH2* h, const double xMin = 1, const double xMax = -1 );


  /*!
  \fn void FitInverted( TH2* h, const double xMin = 1, const double xMax = -1 )
  \brief perform the fit. By default, histogram range is used. The fit is inverted: x = f(y) is considered.
  \param h the histogram to be fitted
  \param xMin min value for fit range
  \param xMax max value for fit range
  */
  bool FitInverted( TH2* h, const double xMin = 1, const double xMax = -1 );


  private:

  //! minuit to be minimised function (sum (y-f(x))^2 )
  static void Fcn( int& npar, double *gin, double &res, double *par, int flag );

  //! minuit to be minimised function (sum (x-f(y))^2 )
  static void FcnInv( int& npar, double *gin, double &res, double *par, int flag );

  //! function used for the fit
  TF1* f_;

  //! number of parameters
  unsigned int fNParameters;

  //! minuit object
  TMinuit *gMinuit;

  //! the histogram to be fitted
  static TH2* fHStatic;

  //! static copy of f_
  static TF1* fFStatic;

  //! number of parameters
  static unsigned int fNParametersStatic;

  //! lower limit for the fit
  static double fXMin;

  //! upper limit for the fit
  static double fXMax;

};


#endif
