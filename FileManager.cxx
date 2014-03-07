// $Id: FileManager.cxx,v 1.29 2011/07/01 15:15:05 hpereira Exp $

/*!
\file FileManager.cxx
\brief handles list of files for merging
*/

#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <sstream>
#include <unistd.h>
#include <sys/stat.h>

#include <TChain.h>
#include <TFile.h>
#include <TH1.h>
#include <TH2.h>
#include <TKey.h>
#include <TTree.h>
#include <TDirectory.h>
#include <TKey.h>

#include "Debug.h"
#include "FileManager.h"
#include "Utils.h"

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
      std::cout << "FileManager::AddDirectory - cannot access file \"" << name_str << "\".\n";
      continue;
    }

    fFiles.insert( name_str );
    delete[] name;
  }
  pclose( tmp );
}

//_________________________________________________
void FileManager::AddFiles( TString selection )
{
  if( !selection.Length() ) return;

  TString filename( selection );
  if( filename.First( "alien://" ) == 0 )
  {
    fFiles.insert( filename );
    return;
  }

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
      std::cout << "FileManager::AddFiles - INFO: cannot access file \"" << name_str << "\".\n";
      continue;
    }

    fFiles.insert( name_str );
    delete[] name;

  }
  pclose( tmp );

}

//_________________________________________________
void FileManager::AddList( TString selection )
{
  if( !(selection && strlen( selection ) ) ) return;

  ifstream in( selection );
  if( !in ) {
    std::cout << "FileManager::AddList - invalid file " << selection << std::endl;
    return;
  }

  char line[512];
  while( !(in.rdstate()&std::ios::failbit ) )
  {

    in.getline( line, 512, '\n' );
    if( !strlen(line) ) continue;
    if( strncmp( line, "//", 2 ) == 0 ) {
      std::cout << "FileManager::AddList skipping " << line << std::endl;
      continue;
    }

    std::istringstream line_stream( line );
    TString file;
    line_stream >> file;
    if( line_stream.rdstate() & std::ios::failbit ) continue;
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

  ifstream in( selection );
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

    std::istringstream line_stream( line );
    TString file;
    line_stream >> file;
    if( line_stream.rdstate() & std::ios::failbit ) continue;

    if( fFiles.find( file ) != fFiles.end() ) {
      std::cout << "FileManager::RemoveList - removing " << file << std::endl;
      fFiles.erase( file );
    }

  }
  return;
}

//_________________________________________________
bool FileManager::CheckFiles( TString output ) const
{

  FileSet badFiles;

  unsigned int total( fFiles.size() );
  unsigned int current( 0 );

  // loop over files
  for( FileSet::const_iterator iter = fFiles.begin(); iter!= fFiles.end(); iter++ )
  {

    current++;
    TFile* f = TFile::Open( iter->Data() );
    if( !( f && f->IsOpen() && f->GetSeekKeys() > 0 ) )
    {

      std::cout
        << "FileManager::CheckFiles - "
        << current << "/" << total
        << " " << *iter << " looks corrupted"
        << std::endl;
      badFiles.insert( *iter );

    } else {


      std::cout << "FileManager::CheckFiles - "
        << current << "/" << total
        << " " << *iter << " looks good" << std::endl;

    }

    delete f;

  }

  if( output.Length() )
  {
    ofstream out( output );
    for( FileSet::const_iterator iter = badFiles.begin(); iter!= badFiles.end(); iter++ )
    { out << *iter << std::endl; }
    out.close();
  }

  return badFiles.empty();

}

//_________________________________________________
bool FileManager::CheckTree( TString key, Int_t refEntries, TString output) const
{

  if( output.Length() )
  {
    ofstream out( output.Data() );
    if( out ) std::cout << "FileManager::CheckTree - output written to file " << output << std::endl;
    else {
      std::cout << "FileManager::CheckTree - unable to write file to " << output << std::endl;
      return false;
    }

  }

  // loop over files
  double totalEntries( 0 );

  typedef std::map<TString, double> FileMap;

  FileMap goodfFiles;
  FileSet badFiles;

  unsigned int total( fFiles.size() );
  unsigned int current( 0 );

  for( FileSet::const_iterator iter = fFiles.begin(); iter!= fFiles.end(); iter++ )
  {

    current++;

    TFile *f = TFile::Open( iter->Data() );
    if( !( f && f->IsOpen() && f->GetSeekKeys() > 0 ) || f->IsZombie() )
    {

      std::cout
        << "FileManager::CheckTree - "
        << current << "/" << total
        << " " << *iter << " looks corrupted" << std::endl;
      badFiles.insert( *iter );
      delete f;
      continue;

    }

    // try retrieve tree
    TTree * tree = (TTree*) f->Get( key );
    if( !tree )
    {

      std::cout
        << "FileManager::CheckTree - "
        << current << "/" << total
        << " " << *iter << ": " << key << " not found.\n";
      badFiles.insert( *iter );

    } else {

      double entries( tree->GetEntries() );
      goodfFiles[*iter] = entries;
      totalEntries += entries;

      std::cout
        << "FileManager::CheckTree - "
        << current << "/" << total
        << " " << *iter << ": " << entries << " entries.\n";

      if( refEntries > 0 && entries != refEntries )
      {
        std::cout << "FileManager::CheckTree - number of entries do not match reference." << std::endl;
        badFiles.insert( *iter );
      }


    }

    delete f;

  }

  std::cout << "FileManager::CheckTree - " << fFiles.size() << " files, total: " << totalEntries << " entries.\n";

  if( output.Length() )
  {
    ofstream out( output );

    for( FileMap::const_iterator iter = goodfFiles.begin(); iter != goodfFiles.end(); iter++ )
    { out << iter->first << ": " << iter->second << " entries" << std::endl; }

    for( FileSet::const_iterator iter = badFiles.begin(); iter!= badFiles.end(); iter++ )
    { out << *iter << " is corrupted" << std::endl; }

    out.close();
  }

  return badFiles.empty();

}

//_________________________________________________
bool FileManager::CheckAllTrees( void ) const
{
  FileSet badFiles;
  for( FileSet::const_iterator iter = fFiles.begin(); iter!= fFiles.end(); iter++ )
  {
    TString value( *iter );
    TFile* f = TFile::Open( iter->Data() );
    TKey* key=0;
    TIter treeIter( f->GetListOfKeys() );
    while( ( key = (TKey*) treeIter() ) )
    {
      std::cout << key->GetName() << std::endl;
      if( TString( key->GetClassName() ) != "TTree" ) continue;

      std::cout << "FileManager::CheckAllTrees checking tree " << key->GetName() << std::endl;
      if( CheckTree( TString( key->GetName() ) ) )
      { badFiles.insert( value ); }
    }

    delete f;

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
  for( FileSet::const_iterator iter = fFiles.begin(); iter!= fFiles.end(); iter++ )
  {
    const TString& filename( *iter );
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

  if( !(key && strlen( key ) ) ) return 0;
  if( Empty() ) return 0;

  // check files
  TChain *out = 0;

  for( FileSet::iterator iter = fFiles.begin(); iter!= fFiles.end(); iter++ )
  {

    // open TFile
    TFile* f = TFile::Open( iter->Data() );

    if( !( f && f->IsOpen() ) )
    {
      std::cout << "FileManager::GetChain - troubles with TFile \"" << *iter << "\".\n";
      delete f;
      continue;
    }

    // load tree
    TTree * tree = (TTree*) f->Get( key );
    if( !tree ) {
      std::cout << "FileManager::GetChain - Unable to load chain \"" << key << "\".\n";
      delete f;
      continue;
    }

    if( !out ) {

      // create tree
      TString title(tree->GetTitle());
      out = new TChain( key, title.Data() );

    }

    // dump file
    if( fVerbosity >= ALI_MACRO::SOME )
    { std::cout << "FileManager::GetChain - loading \"" << *iter << "\".\n"; }

    // add TFile to chain
    out->Add( iter->Data() );
    delete f;

  }

  return out;
}

//_________________________________________________
TH1* FileManager::TreeToHisto(
  TString treeName,
  TString hName,
  TString var,
  TCut cut ) const
{
  // check tree name
  if( !(treeName && strlen( treeName ) ) ) return 0;


  // check files (do nothing so far)
  if( Empty() ) return 0;

  // check if histogram with requested name exists
  TH1* h = (TH1*) gROOT->FindObject(hName);
  if( !h ) {
    std::cout << "FileManager::TreeToHisto - fatal: cannot find predefined histogram \"" << hName << "\" .\n";
    return 0;
  }

  // clone histogram into template
  TString tmpName( TString(hName)+"_tmp" );

  // loop over TFiles
  unsigned int count(0);
  unsigned int total(fFiles.size());
  for( FileSet::iterator iter = fFiles.begin(); iter!= fFiles.end(); iter++ )
  {

    // dump file
    if( fVerbosity >= ALI_MACRO::NONE )
      std::cout << "FileManager::TreeToHisto - loading \"" << *iter << "\" (" << ++count << "/" << total << ").\n";

    // open TFile
    TFile* f = TFile::Open( iter->Data() );
    if( !( f && f->IsOpen() ) )
    {

      std::cout << "FileManager::TreeToHisto - troubles with TFile \"" << *iter << "\".\n";
      delete f;
      continue;

    }

    // try load tree
    TTree * tree = (TTree*) f->Get( treeName );
    if( tree )
    {

      // project tree to histogram
      TH1* hTmp( Utils::NewClone( tmpName.Data(), tmpName.Data(), h ) );
      tree->Project( tmpName.Data(), var, cut );
      h->Add( hTmp );

    } else {

      std::cout << "FileManager::TreeToHisto - Unable to load chain \"" << treeName << "\".\n";

    }

    delete f;

  }

  return h;

}

//_________________________________________________
void FileManager::TreeToHisto( const TString& treeName, ProjectionList& projection_list ) const
{

  // check files (do nothing so far)
  if( Empty() ) return;


  // check if histogram with requested name exists
  for( ProjectionList::iterator iter = projection_list.begin(); iter != projection_list.end(); iter++ )
    if( !iter->fH )
  {
    std::cout << "FileManager::TreeToHisto - fatal: cannot find predefined histogram \"" << iter->fHName << "\" .\n";
    return;
  }

  // loop over TFiles
  unsigned int count(0);
  unsigned int total(fFiles.size());
  for( FileSet::iterator iter = fFiles.begin(); iter!= fFiles.end(); iter++ )
  {

    // open TFile
    TFile* f = TFile::Open( iter->Data() );
    if( !( f && f->IsOpen() ) )
    {
      std::cout << "FileManager::TreeToHisto - troubles with TFile \"" << *iter << "\".\n";
      delete f;
      continue;
    }

    // try load tree
    TTree * tree = (TTree*) f->Get( treeName.Data() );
    if( !tree )
    {
      std::cout << "FileManager::TreeToHisto - Unable to load chain \"" << treeName << "\".\n";
      delete f;
      continue;
    }

    // dump file
    if( fVerbosity >= ALI_MACRO::NONE )
    { std::cout << "FileManager::TreeToHisto - loading \"" << *iter << "\" (" << ++count << "/" << total << ").\n"; }

    // project tree to histogram
    for( ProjectionList::iterator iter = projection_list.begin(); iter != projection_list.end(); iter++ )
    {
      TString tmpName( iter->fHName + "_tmp" );
      TH1* hTmp( Utils::NewClone( tmpName.Data(), tmpName.Data(), iter->fH ) );
      tree->Project( tmpName.Data(), iter->fVarName.Data(), iter->fCut );
      iter->fH->Add( hTmp );
    }

    delete f;

  }

  return;

}

//_________________________________________________
TH1* FileManager::GetTH1( TString key ) const
{

  if( !(key && strlen( key ) ) ) return 0;
  if( Empty() ) return 0;

  // check if histogram with requested name exists
  ALI_MACRO::Delete<TH1>( key );
  TH1* out( 0 );

  for( FileSet::iterator iter = fFiles.begin(); iter!= fFiles.end(); iter++ )
  {

    // open TFile
    TFile* f = TFile::Open( iter->Data() );
    if( !( f && f->IsOpen() ) )
    {
      std::cout << "FileManager::GetTH1 - troubles with TFile \"" << *iter << "\".\n";
      delete f;
      continue;
    }

    TObject* object( f->Get( key ) );
    if( !( object && object->IsA()->InheritsFrom("TH1") ) )
    {
      std::cout << "FileManager::GetTH1 - load histogram from \"" << *iter << "\" failed.\n";
      delete f;
      continue;
    }

    TH1* h = (TH1*) object;

    // dump file
    if( fVerbosity >= ALI_MACRO::SOME )
    { std::cout << "FileManager::GetTH1 - loading \"" << *iter << "\".\n"; }

    if( !out ) {
      gROOT->cd();
      out = (TH1*)h->Clone();
    } else out->Add( h );

    delete f;

  }

  return out;

}

//_________________________________________________
TH2* FileManager::GetTH2( TString key ) const
{

  if( !(key && strlen( key ) ) ) return 0;
  if( Empty() ) return 0;

  // check if histogram with requested name exists
  ALI_MACRO::Delete<TH2>( key );
  TH2* out( 0 );

  for( FileSet::iterator iter = fFiles.begin(); iter!= fFiles.end(); iter++ )
  {

    // open TFile
    TFile* f = TFile::Open( iter->Data() );
    if( !( f && f->IsOpen() ) )
    {
      std::cout << "FileManager::GetTH2 - troubles with TFile \"" << *iter << "\".\n";
      delete f;
      continue;
    }

    TObject* object( f->Get( key ) );
    if( !( object && object->IsA()->InheritsFrom("TH2") ) )
    {
      std::cout << "FileManager::GetTH1 - load histogram from \"" << *iter << "\".\n";
      delete f;
      continue;
    }

    TH2* h = (TH2*) object;

    // dump file
    if( fVerbosity >= ALI_MACRO::SOME )
    { std::cout << "FileManager::GetTH2 - loading \"" << *iter << "\".\n"; }

    // this probably does not work. may need a clone.
    if( !out )
    {
      gROOT->cd();
      out = (TH2*) h->Clone();

    } else out->Add( h );

    delete f;

  }

  return out;

}

//_________________________________________________
void FileManager::Merge( TString output, TString selection, TString option ) const
{

  if( Empty() ) return;

  // store list of tree names
  std::set<TString> treeNames;
  std::set<TString> hNames;

  // Convert selection into a string
  TString selection_string( (selection&&strlen(selection)) ? selection:"" );

  // open TFiles, get keys
  for( FileSet::iterator iter = fFiles.begin(); iter != fFiles.end(); iter++ )
  {

    TFile* f = TFile::Open( iter->Data() );
    if( !( f && f->IsOpen() ) )
    {
      std::cout << "FileManager::Merge - troubles with TFile \"" << *iter << "\".\n";
      continue;
    }

    // loop over all keys
    TList* keyList( f->GetListOfKeys() );
    for( TKey* key = (TKey*) keyList->First(); key; key = (TKey*) keyList->After( key ) )
    {

      // read object
      TObject* object( key->ReadObj() );
      if( object && selection_string.Length() &&
        !selection_string.Contains( object->GetName() ) )
      {
        std::cout << "FileManager::Merge - skipping " << object->GetName() << std::endl;
        continue;
      }

      // add to tree list
      if( object->IsA()->InheritsFrom("TTree") )
      {

        std::cout << "FileManager::Merge - got tree " << object->GetName() << std::endl;
        treeNames.insert( object->GetName() );

      } else if( object->IsA()->InheritsFrom( "TH1" ) ) {

        std::cout << "FileManager::Merge - got histogram " << object->GetName() << std::endl;
        hNames.insert( object->GetName() );

      } else std::cout << "FileManager::Merge - unrecognized object type" << std::endl;

    }

    // for now, list of trees is retrieved from first valid TFile
    delete f;
    break;

  }

  // loop over tree names, copy to output TFile
  bool first( true );

  // loop over tree names, copy to output TFile
  for( std::set<TString>::iterator iter = treeNames.begin(); iter!= treeNames.end(); iter++  )
  {

    TChain *chain( GetChain( iter->Data() ) );
    if( chain && chain->GetEntries() )
    {

      std::cout << "FileManager::Merge - writing "  <<  *iter << std::endl;

      /*
      need to recreate the TFile every time because it gets automatically deleted
      in the method (from documentation)
      */
      TFile* f = TFile::Open( output, (first) ? "RECREATE":"UPDATE" );
      chain->Merge( f, 0, option );
      first = false;

    } else std::cout << "FileManager::Merge - chain " <<  *iter << " failed.\n";

  }

  // histograms
  for( std::set<TString>::iterator iter = hNames.begin(); iter!= hNames.end(); iter++  )
  {

    TH1* h( GetTH1( iter->Data() ) );
    if( !h ) continue;

    std::cout << "FileManager::Merge - writing "  <<  *iter << std::endl;
    TFile* out = TFile::Open( output, (first) ? "RECREATE":"UPDATE" );
    first = false;
    out->cd();
    h->Write( h->GetName() );

    delete out;

  }

  return;

}

//_________________________________________________
void FileManager::MergeRecursive( TString output, TString selection ) const
{

  if( Empty() ) return;

  // create output TFile
  TFile* out = TFile::Open( output, "RECREATE" );
  TFile* in = TFile::Open( fFiles.begin()->Data() );
  if( !( in && in->IsOpen() ) )
  {
    std::cout << "FileManager::MergeRecursive - troubles with TFile \"" << *fFiles.begin() << "\".\n";
    delete in;
    delete out;
    return;
  }

  MergeRecursive( in, out, selection );
  out->Write();
  delete in;
  delete out;
  if( fVerbosity >= ALI_MACRO::SOME ) std::cout << "FileManager::MergeRecursive - done" << std::endl;

}

//_________________________________________________
void FileManager::DumpFiles() const
{
  std::cout << "FileManager::DumpFiles";
  if( fFiles.size() == 1 ) std::cout << " - " << *fFiles.begin() << std::endl;
  else
  {
    std::cout << std::endl;
    for( FileSet::iterator iter = fFiles.begin(); iter!= fFiles.end(); iter++ )
    { std::cout << " " << *iter << std::endl; }
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

  if( fVerbosity >= ALI_MACRO::MAX )
  {
    std::cout << std::endl;
    std::cout << "FileManager::MergeRecursive - input:" << std::endl;
    input->ls();
  }

  //We create an iterator to loop on all objects(keys) of first file
  TList* keyList( input->GetListOfKeys() );
  for( TKey* key = (TKey*) keyList->First(); key; key = (TKey*) keyList->After( key ) )
  {

    // check if key is to be saved
    if( selection.Length() && !selection.Contains( key->GetName() ) ) continue;

    input->cd();
    TObject *obj = key->ReadObj();

    if(obj->IsA()->InheritsFrom("TH1"))
    {

      //case of TH1 or TProfile
      std::cout << "FileManager::MergeRecursive - histogram " << obj->GetName() << std::endl;
      TH1* sum = static_cast<TH1*>(obj);

      // loop over files except the first
      FileSet::iterator iter = fFiles.begin();
      iter++;
      for( ;iter != fFiles.end(); iter++ )
      {

        // store base path
        TString base_path( output->GetPath() );
        size_t pos( base_path.First(":") );
        base_path = base_path( pos+2, base_path.Length()-pos-2 );

        // process file
        TFile* f = TFile::Open( iter->Data() );
        if( !( f && f->IsOpen() ) )
        {
          delete f;
          continue;
        }

        f->cd( base_path.Data() );
        TH1 *h = (TH1*) gDirectory->Get( key->GetName() );
        if( h )
        {

          sum->Add( h );
          delete h;

        } else {

          std::cout << "FileManager::MergeRecursive - cannot find histogram " << key->GetName() << " in file " << *iter << std::endl;

        }

        delete f;

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
