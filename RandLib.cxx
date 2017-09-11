#include "RandLib.h"

#include <TDecompChol.h>
#include <iostream>

ClassImp( RandLib );


//____________________________________________________________________
void RandLib::SetParameters( Double_t* mean, Double_t* covariance, Int_t size )
{

  _generator.SetSeed(0);

  // store mean values
  _mean.ResizeTo( size, 1 );
  _mean = TMatrixD( size, 1, mean );

  // resize work array
  _work.ResizeTo( size, 1 );

  // create covariance matrix
  TMatrixDSym covMatrix( size, covariance );

  // get cholesky decomposition
  TDecompChol cholesky( covMatrix );
  if( !cholesky.Decompose() )
  {
    std::cerr << "RandLib::SetParameters - failed to perform cholesky decomposition" << std::endl;
    return;
  }

  // store matrix
  const TMatrixD u( cholesky.GetU() );
  _cholesky.ResizeTo( u );
  _cholesky.Transpose( u );

}

//____________________________________________________________________
Double_t* RandLib::Get( void )
{

  // generate nRows random numbers following normal distribution
  const Int_t nRows( _work.GetNrows() );
  for( Int_t i=0; i<nRows; ++i )
  { _work(i,0) = _generator.Gaus(); }

  _work = TMatrixD( _cholesky, TMatrixD::kMult, _work );
  _work = TMatrixD( _mean, TMatrixD::kPlus, _work );

  return _work.GetMatrixArray();

}
