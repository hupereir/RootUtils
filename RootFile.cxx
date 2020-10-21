#include "RootFile.h"

#include <TFile.h>

#include <iostream>

//________________________________________________________
RootFile::RootFile( const TString& filename, const TString& option ):
  fFilename( filename ),
  fOption( option )
{}

//________________________________________________________
RootFile::~RootFile( void )
{ Close(); }

//________________________________________________________
void RootFile::Close()
{
  if( fClosed ) return;

  if( fObjects.empty() ) return;
  if( fFilename.IsNull() ) return;
  std::unique_ptr<TFile> tfile( TFile::Open( fFilename, fOption ) );
  if( !tfile ) return;

  std::cout << "RootFile::Close - writing objects to " << fFilename << std::endl;

  tfile->cd();
  for( auto&& o:fObjects ) { if(o) o->Write( o->GetName(), TObject::kOverwrite); }
  tfile->Close();

  fObjects.clear();
  fClosed == true;
}

//________________________________________________________
void RootFile::Add( TObject* object )
{ fObjects.push_back( object ); }
