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
void FileManager::AddDirectory( const char* directory )
{
    if( !(directory && strlen( directory ) ) ) return;

    std::string command = std::string("ls -1 ") + directory;
    FILE *tmp = popen( command.c_str(), "r" );
    char line[512];
    while( fgets( line, 512, tmp ) ){

        char *name = new char[512];
        sscanf( line, "%s", name );

        std::string name_str( name );
        if( !name_str.size() ) continue;
        name_str = std::string(directory)+"/"+name_str;

        if( access( name_str.c_str(), R_OK ) ){
            std::cout << "FileManager::AddDirectory - cannot access file \"" << name_str << "\".\n";
            continue;
        }

        fFiles.insert( name_str );
        delete[] name;
    }
    pclose( tmp );
}

//_________________________________________________
void FileManager::AddFiles( const char* selection )
{
    if( !(selection && strlen( selection ) ) ) return;

    std::string filename( selection );
    if( filename.find( "alien://" ) == 0 )
    {
        fFiles.insert( filename );
        return;
    }

    std::string command = std::string("ls -1 ") + selection;
    FILE *tmp = popen( command.c_str(), "r" );
    char line[512];
    while( fgets( line, 512, tmp ) )
    {

        char *name = new char[512];
        sscanf( line, "%s", name );
        std::string name_str( name );
        if( !name_str.size() ) continue;
        if( access( name_str.c_str(), R_OK ) ){
            std::cout << "FileManager::AddFiles - INFO: cannot access file \"" << name_str << "\".\n";
            continue;
        }

        fFiles.insert( name_str );
        delete[] name;

    }
    pclose( tmp );

}

//_________________________________________________
void FileManager::AddList( const char* selection )
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
        std::string file;
        line_stream >> file;
        if( line_stream.rdstate() & std::ios::failbit ) continue;
        fFiles.insert( file );
    }
    return;
}

//_________________________________________________
void FileManager::RemoveFiles( const char* selection )
{
    if( !(selection && strlen( selection ) ) ) return;

    std::string command = std::string("ls -1 ") + selection;
    FILE *tmp = popen( command.c_str(), "r" );
    char line[512];
    while( fgets( line, 512, tmp ) ){
        char *name = new char[512];
        sscanf( line, "%s", name );
        std::string name_str( name );
        if( !name_str.size() ) continue;
        if( access( name_str.c_str(), R_OK ) ) continue;

        if( fFiles.find( name_str ) != fFiles.end() ) {
            std::cout << "FileManager::RemoveFiles - removing " << name_str << std::endl;
            fFiles.erase( name_str );
        }

        delete[] name;
    }

    pclose( tmp );
}

//_________________________________________________
void FileManager::RemoveList( const char* selection )
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
        std::string file;
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
bool FileManager::CheckFiles( const char* output ) const
{

    FileSet badfFiles;

    unsigned int total( fFiles.size() );
    unsigned int current( 0 );

    // loop over files
    for( FileSet::const_iterator iter = fFiles.begin(); iter!= fFiles.end(); iter++ )
    {

        current++;
        TFile* f = TFile::Open( iter->c_str() );
        if( !( f && f->IsOpen() && f->GetSeekKeys() > 0 ) )
        {

            std::cout
                << "FileManager::CheckFiles - "
                << current << "/" << total
                << " " << *iter << " looks corrupted"
                << std::endl;
            badfFiles.insert( *iter );

        } else {


            std::cout << "FileManager::CheckFiles - "
                << current << "/" << total
                << " " << *iter << " looks good" << std::endl;

        }

        delete f;

    }

    if( output )
    {
        ofstream out( output );
        for( FileSet::const_iterator iter = badfFiles.begin(); iter!= badfFiles.end(); iter++ )
            out << *iter << std::endl;
        out.close();
    }

    return badfFiles.empty();

}

//_________________________________________________
bool FileManager::CheckTree( const char* key, const char* output) const
{

    if( output )
    {
        ofstream out( output );
        if( out ) std::cout << "FileManager::CheckTree - output written to file " << output << std::endl;
        else {
            std::cout << "FileManager::CheckTree - unable to write file to " << output << std::endl;
            return false;
        }
    }

    // loop over files
    double total_entries( 0 );

    typedef std::map<std::string, double> FileMap;

    FileMap goodfFiles;
    FileSet badfFiles;

    unsigned int total( fFiles.size() );
    unsigned int current( 0 );

    for( FileSet::const_iterator iter = fFiles.begin(); iter!= fFiles.end(); iter++ )
    {

        current++;

        TFile *f = TFile::Open( iter->c_str() );
        if( !( f && f->IsOpen() && f->GetSeekKeys() > 0 ) || f->IsZombie() )
        {

            std::cout
                << "FileManager::CheckTree - "
                << current << "/" << total
                << " " << *iter << " looks corrupted" << std::endl;
            badfFiles.insert( *iter );
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
            badfFiles.insert( *iter );

        } else {

            double entries( tree->GetEntries() );
            goodfFiles[*iter] = entries;
            total_entries += entries;
            std::cout
                << "FileManager::CheckTree - "
                << current << "/" << total
                << " " << *iter << ": " << entries << " entries.\n";

            }

            delete f;

        }

    std::cout << "FileManager::CheckTree - " << fFiles.size() << " files, total: " << total_entries << " entries.\n";

    if( output )
    {
        ofstream out( output );

        for( FileMap::const_iterator iter = goodfFiles.begin(); iter != goodfFiles.end(); iter++ )
            out << iter->first << ": " << iter->second << " entries" << std::endl;

        for( FileSet::const_iterator iter = badfFiles.begin(); iter!= badfFiles.end(); iter++ )
            out << *iter << " is corrupted" << std::endl;
        out.close();
    }

    return badfFiles.empty();

}

//_________________________________________________
bool FileManager::CheckAllTrees( const char* key) const
{
    FileSet badfFiles;
    for( FileSet::const_iterator iter = fFiles.begin(); iter!= fFiles.end(); iter++ )
    {
        TFile* f = TFile::Open( iter->c_str() );
        TKey* key=0;
        TIter treeIter( f->GetListOfKeys() );
        while( ( key = (TKey*) treeIter() ) )
        {
            std::cout << key->GetName() << std::endl;
            if( std::string( key->GetClassName() ) != "TTree" ) continue;

            std::cout << "FileManager::CheckAllTrees checking tree " << key->GetName() << std::endl;
            if ( CheckTree(key->GetName()) ) badfFiles.insert( *iter );
        }

        delete f;

    }

    return badfFiles.empty();
}

//_________________________________________________
std::string FileManager::MakeVersion( const char* input )
{
    if( !input ) return std::string();

    unsigned int ver(0);
    std::string backup( input );
    while( !access( backup.c_str(), R_OK ) )
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
        const std::string& filename( *iter );
        if( access( filename.c_str(), R_OK ) )
        {
            std::cout << "FileManager::MakeBackup - file " << filename << " not found. Unchanged." << std::endl;
            continue;
        }

        std::string backup;
        unsigned int ver=0;

        do
        {
            std::ostringstream what;
            what << filename << "." << ver;
            backup = what.str();
            ver++;
        } while  ( !access( backup.c_str(), R_OK ) );

        std::ostringstream what;
        what << "cp " << filename << " " << backup;
        system( what.str().c_str() );

        std::cout << "FileManager::MakeBackup - " << filename << " -> " << backup <<std::endl;

    }

}

//_________________________________________________
TChain* FileManager::GetChain( const char* key ) const
{

    if( !(key && strlen( key ) ) ) return 0;
    if( Empty() ) return 0;

    // check files
    TChain *out = 0;

    for( FileSet::iterator iter = fFiles.begin(); iter!= fFiles.end(); iter++ )
    {

        // open TFile
        TFile* f = TFile::Open( iter->c_str() );

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
            std::string title(tree->GetTitle());
            out = new TChain( key, title.c_str() );

        }

        // dump file
        if( fVerbosity >= ALI_MACRO::SOME )
        { std::cout << "FileManager::GetChain - loading \"" << *iter << "\".\n"; }

        // add TFile to chain
        out->Add( iter->c_str() );
        delete f;

    }

    return out;
}

//_________________________________________________
TH1* FileManager::TreeToHisto(
    const char* treeName,
    const char* hName,
    const char* var,
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
    std::string tmpName( std::string(hName)+"_tmp" );

    // loop over TFiles
    unsigned int count(0);
    unsigned int total(fFiles.size());
    for( FileSet::iterator iter = fFiles.begin(); iter!= fFiles.end(); iter++ )
    {

        // dump file
        if( fVerbosity >= ALI_MACRO::NONE )
            std::cout << "FileManager::TreeToHisto - loading \"" << *iter << "\" (" << ++count << "/" << total << ").\n";

        // open TFile
        TFile* f = TFile::Open( iter->c_str() );
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
            TH1* hTmp( Utils::NewClone( tmpName.c_str(), tmpName.c_str(), h ) );
            tree->Project( tmpName.c_str(), var, cut );
            h->Add( hTmp );

        } else {

            std::cout << "FileManager::TreeToHisto - Unable to load chain \"" << treeName << "\".\n";

        }

        delete f;

    }

    return h;

}

//_________________________________________________
void FileManager::TreeToHisto( const std::string& treeName, ProjectionList& projection_list ) const
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
        TFile* f = TFile::Open( iter->c_str() );
        if( !( f && f->IsOpen() ) )
        {
            std::cout << "FileManager::TreeToHisto - troubles with TFile \"" << *iter << "\".\n";
            delete f;
            continue;
        }

        // try load tree
        TTree * tree = (TTree*) f->Get( treeName.c_str() );
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
            std::string tmpName( iter->fHName + "_tmp" );
            TH1* hTmp( Utils::NewClone( tmpName.c_str(), tmpName.c_str(), iter->fH ) );
            tree->Project( tmpName.c_str(), iter->fVarName.c_str(), iter->fCut );
            iter->fH->Add( hTmp );
        }

        delete f;

    }

    return;

}

//_________________________________________________
TH1* FileManager::GetTH1( const char* key ) const
{

    if( !(key && strlen( key ) ) ) return 0;
    if( Empty() ) return 0;

    // check if histogram with requested name exists
    ALI_MACRO::Delete<TH1>( key );
    TH1* out( 0 );

    for( FileSet::iterator iter = fFiles.begin(); iter!= fFiles.end(); iter++ )
    {

        // open TFile
        TFile* f = TFile::Open( iter->c_str() );
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
TH2* FileManager::GetTH2( const char* key ) const
{

    if( !(key && strlen( key ) ) ) return 0;
    if( Empty() ) return 0;

    // check if histogram with requested name exists
    ALI_MACRO::Delete<TH2>( key );
    TH2* out( 0 );

    for( FileSet::iterator iter = fFiles.begin(); iter!= fFiles.end(); iter++ )
    {

        // open TFile
        TFile* f = TFile::Open( iter->c_str() );
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
void FileManager::Merge( const char* output, const char* selection, const char* option ) const
{

    if( Empty() ) return;

    // store list of tree names
    std::set<std::string> treeNames;
    std::set<std::string> hNames;

    // Convert selection into a string
    std::string selection_string( (selection&&strlen(selection)) ? selection:"" );

    // open TFiles, get keys
    for( FileSet::iterator iter = fFiles.begin(); iter != fFiles.end(); iter++ )
    {

        TFile* f = TFile::Open( iter->c_str() );
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
            if( object && selection_string.size() &&
                selection_string.find( object->GetName() ) == std::string::npos )
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
    for( std::set<std::string>::iterator iter = treeNames.begin(); iter!= treeNames.end(); iter++  )
    {

        TChain *chain( GetChain( iter->c_str() ) );
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
    for( std::set<std::string>::iterator iter = hNames.begin(); iter!= hNames.end(); iter++  )
    {

        TH1* h( GetTH1( iter->c_str() ) );
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
void FileManager::MergeRecursive( const char* output, const char* selection ) const
{

    if( Empty() ) return;

    // create output TFile
    TFile* out = TFile::Open( output, "RECREATE" );
    TFile* in = TFile::Open( fFiles.begin()->c_str() );
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
int FileManager::FileSize( const char* file )
{
    struct stat status;
    if( stat( file, &status ) ) return 0;
    return status.st_size;
}

//_________________________________________________
void FileManager::MergeRecursive(
    TDirectory *input,
    TDirectory *output, const std::string& selection ) const
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
        if( selection.size() && selection.find( key->GetName() ) == std::string::npos ) continue;

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
                std::string base_path( output->GetPath() );
                size_t pos( base_path.find(":") );
                base_path = base_path.substr( pos+2, base_path.size()-pos-2 );

                // process file
                TFile* f = TFile::Open( iter->c_str() );
                if( !( f && f->IsOpen() ) )
                {
                    delete f;
                    continue;
                }

                f->cd( base_path.c_str() );
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
