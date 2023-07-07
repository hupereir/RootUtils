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
  ~PdfDocument( void );

  //* set filename
  void SetFilename( const TString& filename )
  {
    fFilename = filename; 
    fFirst = true;
  }
  
  //* add pad
  void Add( TVirtualPad* );

  //* get filename
  const TString& Filename() const 
  { return m_filename; }
  
  private:

  //* filename
  TString m_filename;

  //* true if first page
  Bool_t m_first = true;

  //* root dictionary
  ClassDef( PdfDocument, 0 );

};

#endif
