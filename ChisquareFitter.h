#ifndef ChisquareFitter_h
#define ChisquareFitter_h

#include <TH1.h>
#include <TF1.h>

//! chisquare fitter
class ChisquareFitter
{

  public:

  /// static function used as VirtualFitter
  static void Fcn(
    int &npar,
    double* gin,
    double& out,
    double* u,
    int flag );

  /// chisquare
  static double Chisquare( TH1*, TF1* );

};

#endif
