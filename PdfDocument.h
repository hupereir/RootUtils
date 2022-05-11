#ifndef PdfDocument_h
#define PdfDocument_h

#include <TVirtualPad.h>
#include <TString.h>

class PdfDocument
{

  public:

  //* constructor
  PdfDocument( const TString& = TString() );
  
  //* destructor
  virtual ~PdfDocument( void );

  //* set filename
  void SetFilename( const TString& filename )
  {
    fFilename = filename; 
    fFirst = true;
  }
  
  //* add pad
  void Add( TVirtualPad* );

  private:

  //* filename
  TString fFilename;

  //* true if first page
  bool fFirst = true;

  //* root dictionary
  ClassDef( PdfDocument, 0 );

};

#endif
