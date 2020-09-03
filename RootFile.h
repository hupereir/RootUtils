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

  private:

  //* filename
  TString fFilename;

  //* object list
  std::vector<TObject*> fObjects;
  
  //* root dictionary
  ClassDef( RootFile, 0 );

};

#endif
