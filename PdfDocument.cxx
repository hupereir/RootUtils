#include "PdfDocument.h"

#include <TCanvas.h>

//________________________________________________________
PdfDocument::PdfDocument( TString filename ):
  fFilename( filename ),
  fFirst( kTRUE )
{}

//________________________________________________________
PdfDocument::~PdfDocument( void )
{
  if( fFirst ) return;
  if( fFilename.IsNull() ) return;
  TCanvas().SaveAs( Form( "%s)", fFilename.Data() ) );
}

//________________________________________________________
void PdfDocument::Add( TVirtualPad* pad )
{

  if( fFilename.IsNull() ) return;
  if( fFirst )
  {

    pad->SaveAs( Form( "%s(", fFilename.Data() ) );
    fFirst = kFALSE;

  } else {

    pad->SaveAs( Form( "%s", fFilename.Data() ) );

  }

}
