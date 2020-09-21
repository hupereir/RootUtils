#ifndef RootFile_h
#define RootFile_h

#include <TObject.h>
#include <TString.h>

class RootFile
{

  public:

  //* constructor
  RootFile( TString = TString() );

  //* destructor
  virtual ~RootFile( void );

  //* add pad
  void Add( TObject* );

  //* close
  void Close();
  
  private:

  //* filename
  TString fFilename;

  //* object list
  std::vector<TObject*> fObjects;

  //* true if closed
  bool fClosed = false;
  
  //* root dictionary
  ClassDef( RootFile, 0 );

};

#endif
