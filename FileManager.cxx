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

using namespace std;

//_________________________________________________
//! root dictionary
ClassImp( FileManager );

//_________________________________________________
void FileManager::add_directory( const char* directory )
{
    if( !(directory && strlen( directory ) ) ) return;

    string command = string("ls -1 ") + directory;
    FILE *tmp = popen( command.c_str(), "r" );
    char line[512];
    while( fgets( line, 512, tmp ) ){

        char *name = new char[512];
        sscanf( line, "%s", name );

        string name_str( name );
        if( !name_str.size() ) continue;
        name_str = string(directory)+"/"+name_str;

        if( access( name_str.c_str(), R_OK ) ){
            cout << "FileManager::add_directory - cannot access file \"" << name_str << "\".\n";
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

    string command = string("ls -1 ") + selection;
    FILE *tmp = popen( command.c_str(), "r" );
    char line[512];
    while( fgets( line, 512, tmp ) ){
        char *name = new char[512];
        sscanf( line, "%s", name );
        string name_str( name );
        if( !name_str.size() ) continue;
        if( access( name_str.c_str(), R_OK ) ){
            cout << "FileManager::AddFiles - INFO: cannot access file \"" << name_str << "\".\n";
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
        cout << "FileManager::AddList - invalid file " << selection << endl;
        return;
    }

    char line[512];
    while( !(in.rdstate()&ios::failbit ) )
    {

        in.getline( line, 512, '\n' );
        if( !strlen(line) ) continue;
        if( strncmp( line, "//", 2 ) == 0 ) {
            cout << "FileManager::AddList skipping " << line << endl;
            continue;
        }

        istringstream line_stream( line );
        string file;
        line_stream >> file;
        if( line_stream.rdstate() & ios::failbit ) continue;
        fFiles.insert( file );
    }
    return;
}

//_________________________________________________
void FileManager::RemoveFiles( const char* selection )
{
    if( !(selection && strlen( selection ) ) ) return;

    string command = string("ls -1 ") + selection;
    FILE *tmp = popen( command.c_str(), "r" );
    char line[512];
    while( fgets( line, 512, tmp ) ){
        char *name = new char[512];
        sscanf( line, "%s", name );
        string name_str( name );
        if( !name_str.size() ) continue;
        if( access( name_str.c_str(), R_OK ) ) continue;

        if( fFiles.find( name_str ) != fFiles.end() ) {
            cout << "FileManager::RemoveFiles - removing " << name_str << endl;
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
        cout << "FileManager::AddList - invalid file " << selection << endl;
        return;
    }

    char line[512];
    while( !(in.rdstate()&ios::failbit ) )
    {

        in.getline( line, 512, '\n' );
        if( !strlen(line) ) continue;
        if( strncmp( line, "//", 2 ) == 0 ) continue;

        istringstream line_stream( line );
        string file;
        line_stream >> file;
        if( line_stream.rdstate() & ios::failbit ) continue;

        if( fFiles.find( file ) != fFiles.end() ) {
            cout << "FileManager::RemoveList - removing " << file << endl;
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
        TFile f( iter->c_str() );
        if( !( f.IsOpen() && f.GetSeekKeys() > 0 ) )
        {
            cout << "FileManager::CheckFiles - "
                << current << "/" << total
                << " " << *iter << " looks corrupted" << endl;
            badfFiles.insert( *iter );
        } else {

            cout << "FileManager::CheckFiles - "
                << current << "/" << total
                << " " << *iter << " looks good" << endl;

        }

    }

    if( output )
    {
        ofstream out( output );
        for( FileSet::const_iterator iter = badfFiles.begin(); iter!= badfFiles.end(); iter++ )
            out << *iter << endl;
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
        if( out ) cout << "FileManager::CheckTree - output written to file " << output << endl;
        else {
            cout << "FileManager::CheckTree - unable to write file to " << output << endl;
            return false;
        }
    }

    // loop over files
    double total_entries( 0 );

    typedef map<string, double> FileMap;

    FileMap goodfFiles;
    FileSet badfFiles;

    unsigned int total( fFiles.size() );
    unsigned int current( 0 );

    for( FileSet::const_iterator iter = fFiles.begin(); iter!= fFiles.end(); iter++ )
    {

        current++;

        TFile f( iter->c_str() );
        if( !( f.IsOpen() && f.GetSeekKeys() > 0 ) || f.IsZombie() )
        {
            cout << "FileManager::CheckTree - "
                << current << "/" << total
                << " " << *iter << " looks corrupted" << endl;
            badfFiles.insert( *iter );
            continue;
        }

        // try retrieve tree
        TTree * tree = (TTree*) f.Get( key );
        if( !tree )
        {
            cout << "FileManager::CheckTree - "
                << current << "/" << total
                << " " << *iter << ": " << key << " not found.\n";
            badfFiles.insert( *iter );
        } else {
            double entries( tree->GetEntries() );
            goodfFiles[*iter] = entries;
            total_entries += entries;
            cout << "FileManager::CheckTree - "
                << current << "/" << total
                << " " << *iter << ": " << entries << " entries.\n";
        }
    }

    cout << "FileManager::CheckTree - " << fFiles.size() << " files, total: " << total_entries << " entries.\n";

    if( output )
    {
        ofstream out( output );

        for( FileMap::const_iterator iter = goodfFiles.begin(); iter != goodfFiles.end(); iter++ )
            out << iter->first << ": " << iter->second << " entries" << endl;

        for( FileSet::const_iterator iter = badfFiles.begin(); iter!= badfFiles.end(); iter++ )
            out << *iter << " is corrupted" << endl;
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
        TFile f( iter->c_str() );
        TKey * key=0;
        TIter treeIter( f.GetListOfKeys() );
        while( ( key = (TKey * ) treeIter() ) )
        {
            cout << key->GetName() << endl;
            if ( strcmp(key->GetClassName(),"TTree" ) )
            {
                continue;
            }

            cout << "FileManager::CheckAllTrees checking tree " << key->GetName() << endl;
            if ( CheckTree(key->GetName()) ) badfFiles.insert( *iter );
        }
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
        ostringstream what;
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
        const string& filename( *iter );
        if( access( filename.c_str(), R_OK ) )
        {
            cout << "FileManager::MakeBackup - file " << filename << " not found. Unchanged." << endl;
            continue;
        }

        string backup;
        unsigned int ver=0;

        do
        {
            ostringstream what;
            what << filename << "." << ver;
            backup = what.str();
            ver++;
        } while  ( !access( backup.c_str(), R_OK ) );

        ostringstream what;
        what << "cp " << filename << " " << backup;
        system( what.str().c_str() );

        cout << "FileManager::MakeBackup - " << filename << " -> " << backup <<endl;

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
        TFile tfile( iter->c_str() );
        if( !tfile.IsOpen() ) {
            cout << "FileManager::GetChain - troubles with TFile \"" << *iter << "\".\n";
            continue;
        }

        // load tree
        TTree * tree = (TTree*) tfile.Get( key );
        if( !tree ) {
            cout << "FileManager::GetChain - Unable to load chain \"" << key << "\".\n";
            continue;
        }

        if( !out ) {

            // create tree
            string title(tree->GetTitle());
            out = new TChain( key, title.c_str() );

        }

        // dump file
        if( fVerbosity >= ALI_MACRO::SOME )
            cout << "FileManager::GetChain - loading \"" << *iter << "\".\n";

        // add TFile to chain
        out->Add( iter->c_str() );

    }

    return out;
}

//_________________________________________________
TH1* FileManager::TreeToHisto(
    const char* tree_name,
    const char* histo_name,
    const char* var,
    TCut cut ) const
{
    // check tree name
    if( !(tree_name && strlen( tree_name ) ) ) return 0;


    // check files (do nothing so far)
    if( Empty() ) return 0;

    // check if histogram with requested name exists
    TH1* h = (TH1*) gROOT->FindObject(histo_name);
    if( !h ) {
        cout << "FileManager::TreeToHisto - fatal: cannot find predefined histogram \"" << histo_name << "\" .\n";
        return 0;
    }

    // clone histogram into template
    string tmp_name( string(histo_name)+"_tmp" );

    // loop over TFiles
    unsigned int count(0);
    unsigned int total(fFiles.size());
    for( FileSet::iterator iter = fFiles.begin(); iter!= fFiles.end(); iter++ )
    {

        // dump file
        if( fVerbosity >= ALI_MACRO::NONE )
            cout << "FileManager::TreeToHisto - loading \"" << *iter << "\" (" << ++count << "/" << total << ").\n";

        // open TFile
        TFile tfile( iter->c_str() );
        if( !tfile.IsOpen() ) {
            cout << "FileManager::TreeToHisto - troubles with TFile \"" << *iter << "\".\n";
            continue;
        }

        // try load tree
        TTree * tree = (TTree*) tfile.Get( tree_name );
        if( !tree ) {
            cout << "FileManager::TreeToHisto - Unable to load chain \"" << tree_name << "\".\n";
            continue;
        }

        // project tree to histogram
        TH1* h_tmp( Utils::new_clone( tmp_name.c_str(), tmp_name.c_str(), h ) );
        tree->Project( tmp_name.c_str(), var, cut );
        h->Add( h_tmp );

    }

    return h;

}

//_________________________________________________
void FileManager::TreeToHisto( const string& tree_name, ProjectionList& projection_list ) const
{

    // check files (do nothing so far)
    if( Empty() ) return;


    // check if histogram with requested name exists
    for( ProjectionList::iterator iter = projection_list.begin(); iter != projection_list.end(); iter++ )
        if( !iter->_h )
    {
        cout << "FileManager::TreeToHisto - fatal: cannot find predefined histogram \"" << iter->_h_name << "\" .\n";
        return;
    }

    // loop over TFiles
    unsigned int count(0);
    unsigned int total(fFiles.size());
    for( FileSet::iterator iter = fFiles.begin(); iter!= fFiles.end(); iter++ )
    {

        // open TFile
        TFile tfile( iter->c_str() );
        if( !tfile.IsOpen() ) {
            cout << "FileManager::TreeToHisto - troubles with TFile \"" << *iter << "\".\n";
            continue;
        }

        // try load tree
        TTree * tree = (TTree*) tfile.Get( tree_name.c_str() );
        if( !tree ) {
            cout << "FileManager::TreeToHisto - Unable to load chain \"" << tree_name << "\".\n";
            continue;
        }

        // dump file
        if( fVerbosity >= ALI_MACRO::NONE )
            cout << "FileManager::TreeToHisto - loading \"" << *iter << "\" (" << ++count << "/" << total << ").\n";

        // project tree to histogram
        for( ProjectionList::iterator iter = projection_list.begin(); iter != projection_list.end(); iter++ )
        {
            string tmp_name( iter->_h_name + "_tmp" );
            TH1* h_tmp( Utils::new_clone( tmp_name.c_str(), tmp_name.c_str(), iter->_h ) );
            tree->Project( tmp_name.c_str(), iter->_var_name.c_str(), iter->_cut );
            iter->_h->Add( h_tmp );
        }

    }

    return;

}

//_________________________________________________
TH1* FileManager::GetTH1( const char* key ) const
{

    if( !(key && strlen( key ) ) ) return 0;
    if( Empty() ) return 0;

    // check if histogram with requested name exists
    ALI_MACRO::safe_delete<TH1>( key );
    TH1* out( 0 );

    for( FileSet::iterator iter = fFiles.begin(); iter!= fFiles.end(); iter++ )
    {

        // open TFile
        TFile tfile( iter->c_str() );
        if( !tfile.IsOpen() ) {
            cout << "FileManager::GetTH1 - troubles with TFile \"" << *iter << "\".\n";
            continue;
        }

        TObject* object( tfile.Get( key ) );
        if( !( object && object->IsA()->InheritsFrom("TH1") ) ) {
            cout << "FileManager::GetTH1 - load histogram from \"" << *iter << "\" failed.\n";
            continue;
        }

        TH1* h = (TH1*) object;

        // dump file
        if( fVerbosity >= ALI_MACRO::SOME )
            cout << "FileManager::GetTH1 - loading \"" << *iter << "\".\n";

        if( !out ) {
            gROOT->cd();
            out = (TH1*)h->Clone();
        } else out->Add( h );

    }

    return out;

}

//_________________________________________________
TH2* FileManager::GetTH2( const char* key ) const
{

    if( !(key && strlen( key ) ) ) return 0;
    if( Empty() ) return 0;

    // check if histogram with requested name exists
    ALI_MACRO::safe_delete<TH2>( key );
    TH2* out( 0 );

    for( FileSet::iterator iter = fFiles.begin(); iter!= fFiles.end(); iter++ )
    {

        // open TFile
        TFile tfile( iter->c_str() );
        if( !tfile.IsOpen() ) {
            cout << "FileManager::GetTH2 - troubles with TFile \"" << *iter << "\".\n";
            continue;
        }

        TObject* object( tfile.Get( key ) );
        if( !( object && object->IsA()->InheritsFrom("TH2") ) ) {
            cout << "FileManager::GetTH1 - load histogram from \"" << *iter << "\".\n";
            continue;
        }

        TH2* h = (TH2*) object;

        // dump file
        if( fVerbosity >= ALI_MACRO::SOME )
            cout << "FileManager::GetTH2 - loading \"" << *iter << "\".\n";

        // this probably does not work. may need a clone.
        if( !out ) {
            gROOT->cd();
            out = (TH2*) h->Clone();
        } else out->Add( h );

    }

    return out;

}

//_________________________________________________
void FileManager::Merge( const char* output, const char* selection, const char* option ) const
{

    if( Empty() ) return;

    // store list of tree names
    set<string> tree_names;
    set<string> histo_names;

    // convert selection into a string
    string selection_string( (selection&&strlen(selection)) ? selection:"" );

    // open TFiles, get keys
    for( FileSet::iterator iter = fFiles.begin(); iter != fFiles.end(); iter++ )
    {
        TFile tfile( iter->c_str() );
        if( !tfile.IsOpen() )
        {
            cout << "FileManager::Merge - troubles with TFile \"" << *iter << "\".\n";
            continue;
        }

        // loop over all keys
        TList* key_list( tfile.GetListOfKeys() );
        for( TKey* key = (TKey*) key_list->First(); key; key = (TKey*) key_list->After( key ) )
        {

            // read object
            TObject* object( key->ReadObj() );
            if( object && selection_string.size() &&
                selection_string.find( object->GetName() ) == string::npos )
            {
                cout << "FileManager::Merge - skipping " << object->GetName() << endl;
                continue;
            }

            // add to tree list
            if( object->IsA()->InheritsFrom("TTree") )
            {

                cout << "FileManager::Merge - got tree " << object->GetName() << endl;
                tree_names.insert( object->GetName() );

            } else if( object->IsA()->InheritsFrom( "TH1" ) ) {

                cout << "FileManager::Merge - got histogram " << object->GetName() << endl;
                histo_names.insert( object->GetName() );

            } else cout << "FileManager::Merge - unrecognized object type" << endl;

        }

        // for now, list of trees is retrieved from first valid TFile
        break;

    }

    // loop over tree names, copy to output TFile
    bool first( true );

    // loop over tree names, copy to output TFile
    for( set<string>::iterator iter = tree_names.begin(); iter!= tree_names.end(); iter++  )
    {

        TChain *chain( GetChain( iter->c_str() ) );
        if( chain && chain->GetEntries() )
        {

            cout << "FileManager::Merge - writing "  <<  *iter << endl;

            /*
            need to recreate the TFile every time because it gets automatically deleted
            in the method (from documentation)
            */
            chain->Merge( new TFile( output, (first) ? "RECREATE":"UPDATE" ), 0, option );
            first = false;

        } else cout << "FileManager::Merge - chain " <<  *iter << " failed.\n";

    }

    // histograms
    for( set<string>::iterator iter = histo_names.begin(); iter!= histo_names.end(); iter++  )
    {

        TH1* h( GetTH1( iter->c_str() ) );
        if( !h ) continue;

        cout << "FileManager::Merge - writing "  <<  *iter << endl;
        TFile out( output, (first) ? "RECREATE":"UPDATE" );
        first = false;
        out.cd();
        h->Write( h->GetName() );

    }

    return;

}

//_________________________________________________
void FileManager::MergeRecursive( const char* output, const char* selection ) const
{

    if( Empty() ) return;

    // create output TFile
    TFile out( output, "RECREATE" );
    TFile in( fFiles.begin()->c_str() );
    if( !in.IsOpen() )
    {
        cout << "FileManager::MergeRecursive - troubles with TFile \"" << in.GetName() << "\".\n";
        return;
    }

    _MergeRecursive( &in, &out, selection );
    out.Write();

    if( fVerbosity >= ALI_MACRO::SOME ) cout << "FileManager::MergeRecursive - done" << endl;

}

//_________________________________________________
void FileManager::DumpFiles() const
{
    cout << "FileManager::DumpFiles";
    if( fFiles.size() == 1 ) cout << " - " << *fFiles.begin() << endl;
    else
    {
        cout << endl;
        for( FileSet::iterator iter = fFiles.begin(); iter!= fFiles.end(); iter++ )
        { cout << " " << *iter << endl; }
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
void FileManager::_MergeRecursive(
    TDirectory *input,
    TDirectory *output, const std::string& selection ) const
{

    if( fVerbosity >= ALI_MACRO::MAX )
    {
        cout << endl;
        cout << "FileManager::_MergeRecursive - input:" << endl;
        input->ls();
    }

    //We create an iterator to loop on all objects(keys) of first file
    TList* key_list( input->GetListOfKeys() );
    for( TKey* key = (TKey*) key_list->First(); key; key = (TKey*) key_list->After( key ) )
    {

        // check if key is to be saved
        if( selection.size() && selection.find( key->GetName() ) == string::npos ) continue;

        input->cd();
        TObject *obj = key->ReadObj();

        if(obj->IsA()->InheritsFrom("TH1"))
        {

            //case of TH1 or TProfile
            cout << "FileManager::_MergeRecursive - histogram " << obj->GetName() << endl;
            TH1* sum = static_cast<TH1*>(obj);

            // loop over files except the first
            FileSet::iterator iter = fFiles.begin();
            iter++;
            for( ;iter != fFiles.end(); iter++ )
            {

                // store base path
                string base_path( output->GetPath() );
                size_t pos( base_path.find(":") );
                base_path = base_path.substr( pos+2, base_path.size()-pos-2 );

                // process file
                TFile f( iter->c_str() );
                if( !f.IsOpen() ) continue;

                f.cd( base_path.c_str() );
                TH1 *h = (TH1*) gDirectory->Get( key->GetName() );
                if( h )
                {
                    sum->Add( h );
                    delete h;
                } else {
                    cout << "FileManager::_MergeRecursive - cannot find histogram " << key->GetName() << " in file " << *iter << endl;
                }
            }

        } else if( obj->IsA()->InheritsFrom("TDirectory") ) {
            cout << "FileManager::_MergeRecursive - directory " << obj->GetName() << endl;
            output->cd();
            TDirectory *dest = output->mkdir(obj->GetName(),obj->GetTitle());
            dest->cd();
            TObject *obj_save = obj;
            TKey    *key_save = key;
            _MergeRecursive( (TDirectory*)obj, dest, selection );
            obj = obj_save;
            key = key_save;

        }

        // write node object, modified or not into fnew
        if (obj && key ) {
            output->cd();
            obj->Write(key->GetName());
            delete obj;
        }

    }

}
