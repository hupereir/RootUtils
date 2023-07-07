#include "PdfDocument.h"

#include <TCanvas.h>

//________________________________________________________
PdfDocument::PdfDocument( TString filename ):
  m_filename( filename )
{}

//________________________________________________________
PdfDocument::~PdfDocument( void )
{
  if( m_first ) return;
  if( m_filename.IsNull() ) return;
  TCanvas().SaveAs( Form( "%s)", m_filename.Data() ) );
}

//________________________________________________________
void PdfDocument::Add( TVirtualPad* pad )
{

  if( m_filename.IsNull() ) return;
  if( m_first )
  {

    pad->SaveAs( Form( "%s(", m_filename.Data() ) );
    m_first = kFALSE;

  } else {

    pad->SaveAs( Form( "%s", m_filename.Data() ) );

  }

}
