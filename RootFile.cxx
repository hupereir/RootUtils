#include "RootFile.h"

#include <TFile.h>

#include <iostream>

//________________________________________________________
RootFile::RootFile( TString filename ):
  fFilename( filename )
{}

//________________________________________________________
RootFile::~RootFile( void )
{
  if( fObjects.empty() ) return;
  if( fFilename.IsNull() ) return;
  std::unique_ptr<TFile> tfile( TFile::Open( fFilename, "RECREATE" ) );
  if( !tfile ) return;
  
  std::cout << "RootFile::~RootFile - writing objects to " << fFilename << std::endl;
  
  tfile->cd();
  for( auto&& o:fObjects ) { o->Write(); }
  tfile->Close();
}

//________________________________________________________
void RootFile::Add( TObject* object )
{ fObjects.push_back( object ); }
