// $Id: FileManager.h,v 1.26 2011/07/01 15:15:05 hpereira Exp $
#ifndef FileManager_h
#define FileManager_h


#include "ALI_MACRO.h"
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

//! handles list of files for merging
//class FileManager: virtual public TObject
class FileManager: public TObject
{

    public:

    //! constructor
    FileManager( const char* selection=0 ):
        fVerbosity( ALI_MACRO::NONE )
    { AddFiles( selection ); }

    //! clear selection
    void Clear( void )
    { fFiles.clear(); }

    //! load an entire directory
    void add_directory( const char* directory );

    //! add files to existing list
    void AddFiles( const char* selection );

    //! add files from list
    void AddList( const char* file_list );

    //! remove files from existing list
    void RemoveFiles( const char* selection );

    //! remove files from existing list
    void RemoveList( const char* file_list );

    //! check if root files are valid
    /*! possibly dumps the bad fines in output */
    bool CheckFiles( const char* output = 0 ) const;

    //! check if tree is valid for each file. Dump its entries
    bool CheckTree( const char* tree_name = "", const char* output = 0 ) const;

    //!check all the trees in a file.
    bool CheckAllTrees( const char* output = 0 ) const;

    //! returns true if file list is empty
    bool Empty( void ) const
    {
        if( fFiles.size() ) return false;
        std::cout << "FileManager::CheckFiles - no files loaded" << std::endl;
        return true;
    }

    // make version name of a file
    static std::string MakeVersion( const char* );

    /*!
    make a backup copy of all loaded files adding _i trailer, where i is the
    first found integer so that files do not collide
    */
    void MakeBackup( void ) const;

    //! Merge TChain from files
    TChain* GetChain( const char* key ) const;

    //! filter Tree
    //TTree* filter_tree( const char* key, const TCut& cut );

    //! project chain from files into histogram
    /*
    note this method do not Merge the trees but opens them one after the
    other, project the tree, sums the result. This leads to smaller memory
    consumption
    */
    TH1* TreeToHisto(
        const char* tree_name,
        const char* name,
        const char* var,
        TCut cut ) const;

    #ifndef __CINT__
    //! project chain from files into histograms define by projections
    void TreeToHisto( const std::string& tree_name, ProjectionList& projection_list ) const;
    #endif

    //! Merge TH1 from files
    TH1* GetTH1( const char* key ) const;

    //! Merge TH2 from files
    TH2* GetTH2( const char* key ) const;

    //!  Merge all trees/histograms found in file_selection into output_file
    void Merge( const char* output_file = "out.root", const char* selection="", const char* option = "" ) const;

    //! Merge all histograms found in file_selection into output file
    /*! keeps TDirectory structure of the input files into output file */
    void MergeRecursive( const char* output_file = "out.root", const char* selection="" ) const;

    //! write files
    void DumpFiles( void ) const;

    //! shortcut for set of files
    typedef std::set< std::string > FileSet;
    typedef std::vector< std::string > FileList;

    //! get number of files
    unsigned int GetNFiles( void ) const
    { return fFiles.size(); }

    //! get list of files
    const FileSet& GetFiles( void ) const
    { return fFiles; }

    //! verbosity
    ALI_MACRO::Verbosity GetVerbosity( void ) const
    { return fVerbosity; }

    //! verbosity
    void SetVerbosity( ALI_MACRO::Verbosity value )
    { fVerbosity = value; }

    //!@name utility functions
    //@{

    //! file size
    static int FileSize( const char* file );

    //@}

    protected:

    //! recursive merging of TDirectories
    void _MergeRecursive(TDirectory *root,TDirectory* node, const std::string& selection) const;


    //! list of input files
    FileSet fFiles;

    //! verbosity
    ALI_MACRO::Verbosity fVerbosity;

    //! root dictionary
    ClassDef( FileManager, 0 );

};


#endif
