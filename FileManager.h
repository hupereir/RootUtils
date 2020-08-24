// $Id: FileManager.h,v 1.26 2011/07/01 15:15:05 hpereira Exp $
#ifndef FileManager_h
#define FileManager_h

#include "ROOT_MACRO.h"

#include <iostream>
#include <set>
#include <vector>
#include <string>
#include <TCut.h>
#include <TObject.h>

#ifndef __CINT__
#include "Projection.h"
#endif
/*!
\file FileManager.h
\brief handles list of files for merging
*/

class TChain;
class TTree;
class TH1;
class TH2;
class TDirectory;

//* handles list of files for merging
//class FileManager: virtual public TObject
class FileManager: public TObject
{

    public:

    //* constructor
    FileManager( TString selection = TString() ):
        fVerbosity( ROOT_MACRO::NONE )
    { AddFiles( selection ); }

    //* clear selection
    void Clear( void )
    { fFiles.clear(); }

    //* load an entire directory
    void AddDirectory( TString directory );

    //* add files to existing list
    bool AddFiles( TString selection );

    //* add files from list
    void AddList( TString fileList );

    //* remove files from existing list
    void RemoveFiles( TString selection );

    //* remove files from existing list
    void RemoveList( TString file_list );

    //* remove invalid files from list
    bool RemoveInvalidFiles() const
    { return CheckFiles( true ); }

    //* remove invalid files from list
    bool RemoveInvalidFiles( const TString& name, int entries) const
    { return CheckTree( name, entries, true ); }

    //* check if root files are valid
    /*! possibly dumps the bad fines in output */
    bool CheckFiles( bool remove_invalid = false ) const;

    //* check if tree is valid for each file. Dump its entries
    bool CheckTree( const TString&, int = 0, bool remove_invalid = false ) const;

    //*check all the trees in a file.
    bool CheckAllTrees( void ) const;

    //* returns true if file list is empty
    bool Empty( void ) const
    {
        if( fFiles.size() ) return false;
        std::cout << "FileManager::CheckFiles - no files loaded" << std::endl;
        return true;
    }

    // make version name of a file
    static TString MakeVersion( TString );

    /*!
    make a backup copy of all loaded files adding _i trailer, where i is the
    first found integer so that files do not collide
    */
    void MakeBackup( void ) const;

    //* Merge TChain from files
    TChain* GetChain( TString key ) const;

    //* filter Tree
    //TTree* filter_tree( TString key, const TCut& cut );

    //* project chain from files into histogram
    /*
    note this method do not Merge the trees but opens them one after the
    other, project the tree, sums the result. This leads to smaller memory
    consumption
    */
    TH1* TreeToHisto(
        TString treename,
        TString name,
        TString var,
        TCut cut ) const;

    #ifndef __CINT__
    //* project chain from files into histograms define by projections
    void TreeToHisto( const TString& treename, ProjectionList& projection_list ) const;
    #endif

    //* Merge histogram from files
    TH1* GetHistogram( TString key ) const;

    //* list
    TList* GetList( TString key ) const;

    //* Get histogram from TList
    TH1* GetHistogramFromList( TString key, TString list ) const;

    //* Merge all histograms found in file_selection into output file
    /*! keeps TDirectory structure of the input files into output file */
    void Merge( TString = "out.root", TString selection="" ) const;

    //* write files
    void DumpFiles( void ) const;

    //* shortcut for set of files
    typedef std::set< TString > FileSet;
    typedef std::vector< TString > FileList;

    //* get number of files
    unsigned int GetNFiles( void ) const
    { return fFiles.size(); }

    //* get list of files
    const FileSet& GetFiles( void ) const
    { return fFiles; }

    //* verbosity
    ROOT_MACRO::Verbosity GetVerbosity( void ) const
    { return fVerbosity; }

    //* verbosity
    void SetVerbosity( ROOT_MACRO::Verbosity value )
    { fVerbosity = value; }

    //*@name utility functions
    //@{

    //* file size
    static int FileSize( TString file );

    //@}

    protected:

    //* recursive merging of TDirectories
    void MergeRecursive(TDirectory *root,TDirectory* node, const TString& selection) const;

    private:

    //* list of input files
    FileSet fFiles;

    //* verbosity
    ROOT_MACRO::Verbosity fVerbosity;

    //* root dictionary
    ClassDef( FileManager, 0 );

};


#endif
