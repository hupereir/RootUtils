#ifndef LikelihoodFitter_h
#define LikelihoodFitter_h

//! log likelihood fitter
class LikelihoodFitter
{

  public:

  /// static function used as VirtualFitter
  static void Fcn(
    int &npar,
    double* gin,
    double& out,
    double* u,
    int flag );

};

#endif
