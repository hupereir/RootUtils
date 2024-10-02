// $Id: FileManager.cxx,v 1.29 2011/07/01 15:15:05 hpereira Exp $

#include "Debug.h"
#include "FileManager.h"
#include "Utils.h"

#include <TChain.h>
#include <TFile.h>
#include <TH1.h>
#include <TH2.h>
#include <THnBase.h>
#include <TKey.h>
#include <TTree.h>
#include <TDirectory.h>
#include <TKey.h>
#include <TSystem.h>

#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <memory>
#include <sstream>
#include <unistd.h>
#include <sys/stat.h>

//_________________________________________________
//! root dictionary
ClassImp( FileManager );

//_________________________________________________
void FileManager::AddDirectory( TString directory )
{
  if( !(directory && strlen( directory ) ) ) return;

  TString command = TString("ls -1 ") + directory;
  FILE *tmp = popen( command.Data(), "r" );
  char line[512];
  while( fgets( line, 512, tmp ) ){

    char *name = new char[512];
    sscanf( line, "%s", name );

    TString name_str( name );
    if( !name_str.Length() ) continue;
    name_str = TString(directory)+"/"+name_str;

    if( access( name_str.Data(), R_OK ) ){
      std::cout << "FileManager::AddDirectory - cannot access file \"" << name_str << "\"." << std::endl;
      continue;
    }

    fFiles.insert( name_str );
    delete[] name;
  }
  pclose( tmp );
}

//_________________________________________________
bool FileManager::AddFiles( TString selection )
{
  std::cout << "FileManager::AddFiles - selection: " << selection << std::endl;
  if( !selection.Length() ) return false;

  // check if alien files
  if( std::string( selection.Data() ).rfind( "alien://", 0 ) == 0 )
  {
    fFiles.insert( selection );
    return true;
  }

  bool added = false;
  TString command = TString("ls -1 ") + selection;
  
  FILE *tmp = popen( command.Data(), "r" );
  char line[512];
  while( fgets( line, 512, tmp ) )
  {

    char *name = new char[512];
    sscanf( line, "%s", name );
    TString name_str( name );
    if( !name_str.Length() ) continue;
    if( access( name_str.Data(), R_OK ) ){
      std::cout << "FileManager::AddFiles - INFO: cannot access file \"" << name_str << "\"." << std::endl;
      continue;
    }

    added = true;
    fFiles.insert( name_str );
    delete[] name;

  }
  pclose( tmp );

  return added;

}

//_________________________________________________
void FileManager::AddList( TString selection )
{
  if( !(selection && strlen( selection ) ) ) return;

  std::ifstream in( selection );
  if( !in )
  {
    std::cout << "FileManager::AddList - invalid file " << selection << std::endl;
    return;
  }

  while( !(in.rdstate()&std::ios::failbit ) )
  {

    std::string line;
    std::getline( in, line );
    if( line.empty() ) continue;
    if( line.substr(0,2) == "//" )
    {
      std::cout << "FileManager::AddList skipping " << line << std::endl;
      continue;
    }

    std::istringstream lineStream( line.c_str() );
    TString file;
    lineStream >> file;
    if( lineStream.rdstate() & std::ios::failbit ) continue;
    fFiles.insert( file );
  }
  return;
}

//_________________________________________________
void FileManager::RemoveFiles( TString selection )
{
  if( !(selection && strlen( selection ) ) ) return;

  TString command = TString("ls -1 ") + selection;
  FILE *tmp = popen( command.Data(), "r" );
  char line[512];
  while( fgets( line, 512, tmp ) ){
    char *name = new char[512];
    sscanf( line, "%s", name );
    TString name_str( name );
    if( !name_str.Length() ) continue;
    if( access( name_str.Data(), R_OK ) ) continue;

    if( fFiles.find( name_str ) != fFiles.end() ) {
      std::cout << "FileManager::RemoveFiles - removing " << name_str << std::endl;
      fFiles.erase( name_str );
    }

    delete[] name;
  }

  pclose( tmp );
}

//_________________________________________________
void FileManager::RemoveList( TString selection )
{
  if( !(selection && strlen( selection ) ) ) return;

  std::ifstream in( selection );
  if( !in ) {
    std::cout << "FileManager::AddList - invalid file " << selection << std::endl;
    return;
  }

  char line[512];
  while( !(in.rdstate()&std::ios::failbit ) )
  {

    in.getline( line, 512, '\n' );
    if( !strlen(line) ) continue;
    if( strncmp( line, "//", 2 ) == 0 ) continue;

    std::istringstream lineStream( line );
    TString file;
    lineStream >> file;
    if( lineStream.rdstate() & std::ios::failbit ) continue;

    if( fFiles.find( file ) != fFiles.end() ) {
      std::cout << "FileManager::RemoveList - removing " << file << std::endl;
      fFiles.erase( file );
    }

  }
  return;
}

//_________________________________________________
bool FileManager::CheckFiles( bool remove_invalid ) const
{

  FileSet badFiles;
  FileSet goodFiles;

  unsigned int total( fFiles.size() );
  unsigned int current( 0 );

  // loop over files
  for( const auto& filename:fFiles )
  {
    ++current;
    std::unique_ptr<TFile> f(TFile::Open( filename ));
    if( !( f && f->IsOpen() && f->GetSeekKeys() > 0 ) )
    {
      std::cout
        << "FileManager::CheckFiles - "
        << current << "/" << total
        << " " << filename << " looks corrupted"
        << std::endl;
      badFiles.insert( filename );
    } else {
      std::cout << "FileManager::CheckFiles - "
        << current << "/" << total
        << " " << filename << " looks good" << std::endl;
      goodFiles.insert( filename );
    }
  }

  for( const auto& filename:badFiles )
  {
    std::cout << filename << std::endl;
    if(remove_invalid) gSystem->Unlink( filename );
  }

  std::cout << std::endl;
  std::cout
    << "FileManager::CheckFiles -"
    << " total files: " << fFiles.size()
    << " valid files: " << goodFiles.size()
    << " invalid files: " << badFiles.size()
    << std::endl;

  return badFiles.empty();

}

//_________________________________________________
bool FileManager::CheckTree(const TString& key, int refEntries, bool remove_invalid) const
{

  // loop over files
  double totalEntries( 0 );

  using FileMap = std::map<TString, double>;

  FileMap goodFiles;
  FileSet badFiles;

  unsigned int total( fFiles.size() );
  unsigned int current( 0 );

  for(const auto& filename:fFiles)
  {
    ++current;
    // std::unique_ptr<TFile> f(TFile::Open(filename));
    std::unique_ptr<TFile> f(TFile::Open(filename));
    if( !( f && f->IsOpen() && f->GetSeekKeys() > 0 ) || f->IsZombie() )
    {
      std::cout
        << "FileManager::CheckTree - "
        << current << "/" << total
        << " " << filename << " looks corrupted" << std::endl;
      badFiles.insert( filename );
      continue;
    }

    // try retrieve tree
    auto tree = static_cast<TTree*>(f->Get(key));
    if( !tree )
    {
      std::cout
        << "FileManager::CheckTree - "
        << current << "/" << total
        << " " << filename << ": " << key << " not found." << std::endl;
      badFiles.insert( filename );
    } else {
      auto entries( tree->GetEntries() );
      goodFiles[filename] = entries;
      totalEntries += entries;

      std::cout
        << "FileManager::CheckTree - "
        << current << "/" << total
        << " " << filename << ": " << entries << " entries." << std::endl;

      if( refEntries > 0 && entries != refEntries )
      {
        std::cout << "FileManager::CheckTree - number of entries do not match reference." << std::endl;
        badFiles.insert( filename );
      }
    }
  }

  std::cout << std::endl;
  for( const auto& pair:goodFiles )
  { std::cout << pair.first << ": " << pair.second << " entries" << std::endl; }

  if( !goodFiles.empty() ) std::cout << std::endl;
  for( const auto& filename:badFiles )
  {
    std::cout << filename << " is corrupted" << std::endl;
    if(remove_invalid) gSystem->Unlink(filename);
  }

  std::cout << std::endl;
  std::cout
    << "FileManager::CheckTree -"
    << " total files: " << fFiles.size()
    << " valid files: " << goodFiles.size()
    << " invalid files: " << badFiles.size()
    << " total entries: " << totalEntries
    << std::endl;

  return badFiles.empty();

}

//_________________________________________________
bool FileManager::CheckAllTrees( void ) const
{
  FileSet badFiles;
  for( const auto& filename:fFiles )
  {
    std::unique_ptr<TFile> f(TFile::Open( filename ));
    TKey* key= nullptr;
    TIter treeIter( f->GetListOfKeys() );
    while( ( key = (TKey*) treeIter() ) )
    {
      std::cout << key->GetName() << std::endl;
      if( TString( key->GetClassName() ) != "TTree" ) continue;

      std::cout << "FileManager::CheckAllTrees checking tree " << key->GetName() << std::endl;
      if( CheckTree( TString( key->GetName() ) ) )
      { badFiles.insert(filename); }
    }
  }
  return badFiles.empty();
}

//_________________________________________________
TString FileManager::MakeVersion( TString input )
{
  if( !input ) return TString();

  unsigned int ver(0);
  TString backup( input );
  while( !access( backup.Data(), R_OK ) )
  {
    ver++;
    std::ostringstream what;
    what << input << "." << ver;
    backup = what.str();
  }

  return backup;

}

//_________________________________________________
void FileManager::MakeBackup( void ) const
{

  // loop over files
  for( const auto& filename:fFiles )
  {
    if( access( filename.Data(), R_OK ) )
    {
      std::cout << "FileManager::MakeBackup - file " << filename << " not found. Unchanged." << std::endl;
      continue;
    }

    TString backup;
    unsigned int ver=0;

    do
    {
      std::ostringstream what;
      what << filename << "." << ver;
      backup = what.str();
      ver++;
    } while  ( !access( backup.Data(), R_OK ) );

    std::ostringstream what;
    what << "cp " << filename << " " << backup;
    system( what.str().c_str() );

    std::cout << "FileManager::MakeBackup - " << filename << " -> " << backup <<std::endl;

  }

}

//_________________________________________________
TChain* FileManager::GetChain( TString key ) const
{

  if( !(key && strlen( key ) ) ) return nullptr;
  if( Empty() ) return nullptr;

  int valid_files = 0;

  // check files
  TChain *out = nullptr;
  for( const auto& filename:fFiles )
  {

    // open TFile
    std::unique_ptr<TFile> f( TFile::Open(filename) );

    if( !( f && f->IsOpen() ) )
    {
      std::cout << "FileManager::GetChain - troubles with TFile \"" << filename << "\"." << std::endl;
      continue;
    }

    // load tree
    auto tree = static_cast<TTree*>( f->Get( key ) );
    if( !tree ) {
      std::cout << "FileManager::GetChain - Unable to load chain \"" << key << "\" in \"" << filename << "\"." << std::endl;
      continue;
    }

    if( !out ) {

      // create tree
      auto title(tree->GetTitle());
      out = new TChain( key, title );
    }

    // dump file
    if( fVerbosity >= ROOT_MACRO::SOME )
    { std::cout << "FileManager::GetChain - loading \"" << filename << "\"." << std::endl; }

    // add TFile to chain
    out->Add( filename );
    ++valid_files;

  }

  std::cout << "FileManager::GetChain - valid files: " << valid_files << std::endl;

  return out;
}

//_________________________________________________
TH1* FileManager::TreeToHisto(
  TString treename,
  TString hName,
  TString var,
  TCut cut ) const
{
  // check tree name
  if( !(treename && strlen( treename ) ) ) return nullptr;


  // check files (do nothing so far)
  if( Empty() ) return nullptr;

  // check if histogram with requested name exists
  auto h = static_cast<TH1*>(gROOT->FindObject(hName));
  if( !h ) {
    std::cout << "FileManager::TreeToHisto - fatal: cannot find predefined histogram \"" << hName << "\" ." << std::endl;
    return nullptr;
  }

  // clone histogram into template
  TString tmpName( TString(hName)+"_tmp" );

  // loop over TFiles
  unsigned int count(0);
  unsigned int total(fFiles.size());
  for( const auto& filename:fFiles )
  {

    // dump file
    if( fVerbosity >= ROOT_MACRO::NONE )
      std::cout << "FileManager::TreeToHisto - loading \"" << filename << "\" (" << ++count << "/" << total << ")." << std::endl;

    // open TFile
    std::unique_ptr<TFile> f(TFile::Open(filename));
    if( !( f && f->IsOpen() ) )
    {
      std::cout << "FileManager::TreeToHisto - troubles with TFile \"" << filename << "\"." << std::endl;
      continue;
    }

    // try load tree
    auto tree = static_cast<TTree*>(f->Get(treename));
    if( tree )
    {
      // project tree to histogram
      auto hTmp( Utils::NewClone( tmpName.Data(), tmpName.Data(), h ) );
      tree->Project( tmpName, var, cut );
      h->Add( hTmp );
    } else {
      std::cout << "FileManager::TreeToHisto - Unable to load chain \"" << treename << "\"." << std::endl;
    }
  }

  return h;

}

// //_________________________________________________
// void FileManager::TreeToHisto( const TString& treename, ProjectionList& projection_list ) const
// {
//
//   // check files (do nothing so far)
//   if( Empty() ) return;
//
//
//   // check if histogram with requested name exists
//   for( ProjectionList::iterator iter = projection_list.begin(); iter != projection_list.end(); iter++ )
//     if( !iter->fH )
//   {
//     std::cout << "FileManager::TreeToHisto - fatal: cannot find predefined histogram \"" << iter->fHName << "\" ." << std::endl;
//     return;
//   }
//
//   // loop over TFiles
//   unsigned int count(0);
//   unsigned int total(fFiles.size());
//   for(const auto& filename:fFiles)
//   {
//
//     // open TFile
//     std::unique_ptr<TFile> f(TFile::Open(filename));
//     if( !( f && f->IsOpen() ) )
//     {
//       std::cout << "FileManager::TreeToHisto - troubles with TFile \"" << filename << "\"." << std::endl;
//       continue;
//     }
//
//     // try load tree
//     TTree * tree = (TTree*) f->Get( treename.Data() );
//     if( !tree )
//     {
//       std::cout << "FileManager::TreeToHisto - Unable to load chain \"" << treename << "\"." << std::endl;
//       continue;
//     }
//
//     // dump file
//     if( fVerbosity >= ROOT_MACRO::NONE )
//     { std::cout << "FileManager::TreeToHisto - loading \"" << filename << "\" (" << ++count << "/" << total << ")." << std::endl; }
//
//     // project tree to histogram
//     for( ProjectionList::iterator iter = projection_list.begin(); iter != projection_list.end(); iter++ )
//     {
//       TString tmpName( iter->fHName + "_tmp" );
//       TH1* hTmp( Utils::NewClone( tmpName.Data(), tmpName.Data(), iter->fH ) );
//       tree->Project( tmpName.Data(), iter->fVarName.Data(), iter->fCut );
//       iter->fH->Add( hTmp );
//     }
//   }
//
//   return;
//
// }
//
// //_________________________________________________
// TList* FileManager::GetList( TString key ) const
// {
//
//   // create output list
//   TList* outputList = new TList();
//   outputList->SetName( key );
//
//   for(const auto& filename:fFiles)
//   {
//
//     // open TFile
//     std::unique_ptr<TFile> f(TFile::Open(filename));
//     if(!(f&&f->IsOpen()))
//     {
//       std::cout << "FileManager::GetList - troubles with TFile \"" << filename << "\"." << std::endl;
//       continue;
//     }
//
//     auto inputList = dynamic_cast<TList*>(f->Get(key));
//     if( !inputList )
//     {
//       std::cout << "FileManager::GetList - load list from \"" << filename << "\" failed." << std::endl;
//       continue;
//     }
//
//     inputList->SetOwner();
//
//     // iterate
//     TIter inputIter( inputList );
//     TObject* inputObject = 0x0;
//     while(inputObject = inputIter.Next())
//     {
//
//       // try cast to histogram
//       auto inputHistogram = dynamic_cast<TH1*>( inputObject );
//       if( inputHistogram )
//       {
//         // check if already in output list
//         auto outputHistogram = static_cast<TH1*>(outputList->FindObject(inputHistogram->GetName()));
//         if( outputHistogram ) outputHistogram->Add( inputHistogram );
//         else {
//           std::cout << "FileManager::GetList - adding histogram " << inputHistogram->GetName() << " to list " << key << std::endl;
//           gROOT->cd();
//           outputList->Add( inputHistogram->Clone() );
//         }
//         continue;
//
//       }
//
//       // try cast to THnBase
//       THnBase* inputThn = dynamic_cast<THnBase*>( inputObject );
//       if( inputThn )
//       {
//         // check if already in output list
//         THnBase* outputThn = (THnBase*) outputList->FindObject( inputThn->GetName() );
//         if( outputThn ) outputThn->Add( inputThn );
//         else {
//
//           std::cout << "FileManager::GetList - adding THn " << inputThn->GetName() << " to list " << key << std::endl;
//           gROOT->cd();
//           outputList->Add( inputThn->Clone() );
//
//         }
//
//       }
//
//     }
//
//     // cleanup
//     delete inputList;
//   }
//
//   return outputList;
// }

//_________________________________________________
TH1* FileManager::GetHistogram( TString key ) const
{

  if( !(key && strlen( key ) ) ) return nullptr;
  if( Empty() ) return nullptr;

  // check if histogram with requested name exists
  ROOT_MACRO::Delete<TH1>( key );
  TH1* out(nullptr);

  for( const auto& filename:fFiles)
  {

    // open TFile
    std::unique_ptr<TFile> f(TFile::Open(filename));
    if( !( f && f->IsOpen() ) )
    {
      std::cout << "FileManager::GetHistogram - troubles with TFile \"" << filename << "\"." << std::endl;
      continue;
    }

    auto h = dynamic_cast<TH1*>( f->Get( key ) );
    if( !h )
    {
      std::cout << "FileManager::GetHistogram - load histogram " << key << " from \"" << filename << "\" failed." << std::endl;
      continue;
    }

    // dump file
    if( fVerbosity >= ROOT_MACRO::SOME )
    { std::cout << "FileManager::GetHistogram - loading \"" << filename << "\"." << std::endl; }

    if( !out )
    {

      gROOT->cd();
      out = static_cast<TH1*>(h->Clone());

    } else out->Add( h );
  }

  return out;

}

//_________________________________________________
TH1* FileManager::GetHistogramFromList( TString key, TString listKey ) const
{

  if( !(key && strlen( key ) ) ) return nullptr;
  if( Empty() ) return nullptr;

  // check if histogram with requested name exists
  ROOT_MACRO::Delete<TH1>( key );
  TH1* out( nullptr );
  for(const auto& filename:fFiles)
  {

    // open TFile
    std::unique_ptr<TFile> f(TFile::Open(filename));
    if( !( f && f->IsOpen() ) )
    {
      std::cout << "FileManager::GetHistogramFromList - troubles with TFile \"" << filename << "\"." << std::endl;
      continue;
    }

    auto list = dynamic_cast<TList*>(f->Get(listKey));
    if( !list )
    {
      std::cout << "FileManager::GetHistogramFromList - load list from \"" << filename << "\" failed." << std::endl;
      continue;
    }

    list->SetOwner();

    // histogram
    auto h = dynamic_cast<TH1*>( list->FindObject( key ) );
    if( !h )
    {
      std::cout << "FileManager::GetHistogramFromList - load histogram from \"" << filename << "\" failed." << std::endl;
      continue;
    }

    // dump file
    // if( fVerbosity >= ROOT_MACRO::SOME )
    { std::cout << "FileManager::GetHistogram - loading \"" << filename << "\"." << std::endl; }

    if( !out ) {
      gROOT->cd();
      out = (TH1*)h->Clone();
    } else out->Add( h );

    // delete h;
    delete list;
    f->Close();
  }

  return out;

}

//_________________________________________________
void FileManager::Merge( TString output, TString selection ) const
{

  if( Empty() ) return;

  // create output TFile
  std::unique_ptr<TFile> out( TFile::Open(output, "RECREATE"));
  std::unique_ptr<TFile> in( TFile::Open(*fFiles.begin()));
  if( !( in && in->IsOpen() ) )
  {
    std::cout << "FileManager::MergeRecursive - troubles with TFile \"" << *fFiles.begin() << "\"." << std::endl;
    return;
  }

  MergeRecursive( in.get(), out.get(), selection );
  out->Write();
  if( fVerbosity >= ROOT_MACRO::SOME ) std::cout << "FileManager::MergeRecursive - done" << std::endl;

}

//_________________________________________________
void FileManager::DumpFiles() const
{
  std::cout << "FileManager::DumpFiles";
  if( fFiles.size() == 1 ) std::cout << " - " << *fFiles.begin() << std::endl;
  else
  {
    std::cout << std::endl;
    for(const auto& filename:fFiles)
    { std::cout << " " << filename << std::endl; }
  }
}

//_________________________________________________
int FileManager::FileSize( TString file )
{
  struct stat status;
  if( stat( file, &status ) ) return 0;
  return status.st_size;
}

//_________________________________________________
void FileManager::MergeRecursive(
  TDirectory *input,
  TDirectory *output, const TString& selection ) const
{

  if( fVerbosity >= ROOT_MACRO::MAX )
  {
    std::cout << std::endl;
    std::cout << "FileManager::MergeRecursive - input:" << std::endl;
    input->ls();
  }

  //We create an iterator to loop on all objects(keys) of first file
  TList* keyList( input->GetListOfKeys() );
  for( auto key = static_cast<TKey*>(keyList->First()); key; key = static_cast<TKey*>(keyList->After(key)) )
  {

    // check if key is to be saved
    if( selection.Length() && !selection.Contains( key->GetName() ) ) continue;

    input->cd();
    auto *obj = key->ReadObj();

    if(obj->IsA()->InheritsFrom("TH1"))
    {

      //case of TH1 or TProfile
      std::cout << "FileManager::MergeRecursive - histogram " << obj->GetName() << std::endl;
      auto sum = static_cast<TH1*>(obj);

      // loop over files except the first
      auto iter = fFiles.begin();
      ++iter;
      for( ;iter != fFiles.end(); ++iter )
      {

        // store base path
        TString base_path( output->GetPath() );
        size_t pos( base_path.First(":") );
        base_path = base_path( pos+2, base_path.Length()-pos-2 );

        // process file
        std::unique_ptr<TFile> f( TFile::Open(*iter) );
        if( !( f && f->IsOpen() ) ) continue;

        f->cd( base_path.Data() );
        std::unique_ptr<TH1> h(static_cast<TH1*>(gDirectory->Get(key->GetName())));
        if( h )
        {
          sum->Add(h.get());
        } else {
          std::cout << "FileManager::MergeRecursive - cannot find histogram " << key->GetName() << " in file " << *iter << std::endl;
        }

      }

    } else if( obj->IsA()->InheritsFrom("TDirectory") ) {

      std::cout << "FileManager::MergeRecursive - directory " << obj->GetName() << std::endl;
      output->cd();
      TDirectory *dest = output->mkdir(obj->GetName(),obj->GetTitle());
      dest->cd();
      TObject *objSave = obj;
      TKey    *keySave = key;
      MergeRecursive( (TDirectory*)obj, dest, selection );
      obj = objSave;
      key = keySave;

    }

    // write node object, modified or not into fnew
    if (obj && key ) {
      output->cd();
      obj->Write(key->GetName());
      delete obj;
    }

  }

}

