// $Id: Utils.cxx,v 1.56 2010/09/15 02:27:25 hpereira Exp $

#include "Grid.h"

#include <TFile.h>
#include <TGrid.h>
#include <TGridResult.h>
#include <TMap.h>
#include <TObjString.h>

#include <iostream>
#include <memory>

//__________________________________________________
//* root dictionary
ClassImp( Grid );

//______________________________________________________________________________
Bool_t Grid::FileExists( TString file, TString directory )
{

  if( !( directory.IsNull() || gGrid->Cd( directory ) ) ) return kFALSE;
  { return kFALSE; }

  std::unique_ptr<TGridResult> res( gGrid->Ls(file) );
  if (!res) return kFALSE;

  TMap *map = dynamic_cast<TMap*>(res->At(0));
  if( !map ) return kFALSE;

  TObjString *objs = dynamic_cast<TObjString*>(map->GetValue("name"));
  if (!objs || !objs->GetString().Length()) return kFALSE;
  return kTRUE;

}

//_________________________________________________
Bool_t Grid::RemoveFile( TString file, TString directory )
{
  if( !FileExists( file, directory ) ) return kFALSE;
  if( !( directory.IsNull() || gGrid->Cd( directory ) ) ) return kFALSE;
  gGrid->Rm( file );
}

//______________________________________
Bool_t Grid::CopyFile( TString inputFile, TString outputDirectory )
{
  const TString source = Form( "file:%s", inputFile.Data() );
  const TString destination = Form( "alien://%s/%s", outputDirectory.Data(), inputFile.Data() );
  std::cout << source << " -> " << destination << std::endl;
  const Bool_t result = TFile::Cp( source, destination );
  if( !result ) std::cout << "copy failed" << std::endl;
  return result;

}
