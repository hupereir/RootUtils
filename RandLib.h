#ifndef RandLib_h
#define RandLib_h

#include <TObject.h>
#include <TMatrixD.h>
#include <TMatrixDSym.h>
#include <TRandom3.h>


class RandLib: public TObject
{

  public:

  //! constructor
  RandLib( void ):
    TObject()
    {}

  //!@name wrappers
  //@{

  //! set mean and covariance matrix
  void SetParameters( Double_t* mean, Double_t* covariance, Int_t size );

  //! generate
  Double_t* Get( void );

  //@}

  private:

  TRandom3 _generator;
  TMatrixD _mean;
  TMatrixD _cholesky;
  TMatrixD _work;

  ClassDef(RandLib,0)

};

#endif
