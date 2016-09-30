#ifndef PdfDocument_h
#define PdfDocument_h

#include <TVirtualPad.h>
#include <TString.h>

class PdfDocument
{

  public:

  //* constructor
  PdfDocument( TString );

  //* destructor
  virtual ~PdfDocument( void );

  //* add pad
  void Add( TVirtualPad* );

  private:

  //* filename
  TString fFilename;

  //* true if first page
  Bool_t fFirst;

  //* root dictionary
  ClassDef( PdfDocument, 0 );

};

#endif
