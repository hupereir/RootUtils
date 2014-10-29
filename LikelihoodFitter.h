#ifndef LikelyhoodFitter_h
#define LikelyhoodFitter_h

//! log likelyhood fitter
class LikelihoodFitter
{

    public:

    //! static function used as VirtualFitter
    static void fcn(
        int &npar,
        double* gin,
        double& out,
        double* u,
        int flag );

};

#endif
