// $Id: FileManager.h,v 1.26 2011/07/01 15:15:05 hpereira Exp $
#ifndef FileManager_h
#define FileManager_h


#include "MUTOO_MACRO.h"
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
        _verbosity( MUTOO_MACRO::NONE )
    { add_files( selection ); }

    //! clear selection
    void clear( void )
    { _files.clear(); }

    //! load an entire directory
    void add_directory( const char* directory );

    //! add files to existing list
    void add_files( const char* selection );

    //! add files from list
    void add_list( const char* file_list );

    //! remove files from existing list
    void remove_files( const char* selection );

    //! remove files from existing list
    void remove_list( const char* file_list );

    //! check if root files are valid
    /*! possibly dumps the bad fines in output */
    bool check_files( const char* output = 0 ) const;

    //! check if tree is valid for each file. Dump its entries
    bool check_tree( const char* tree_name = "", const char* output = 0 ) const;

    //!check all the trees in a file.
    bool check_all_trees( const char* output = 0 ) const;

    //! returns true if file list is empty
    bool empty( void ) const
    {
        if( _files.size() ) return false;
        std::cout << "FileManager::check_files - no files loaded" << std::endl;
        return true;
    }

    // make version name of a file
    static std::string make_version( const char* );

    /*!
    make a backup copy of all loaded files adding _i trailer, where i is the
    first found integer so that files do not collide
    */
    void make_backup( void ) const;

    //! merge TChain from files
    TChain* get_chain( const char* key ) const;

    //! filter Tree
    //TTree* filter_tree( const char* key, const TCut& cut );

    //! project chain from files into histogram
    /*
    note this method do not merge the trees but opens them one after the
    other, project the tree, sums the result. This leads to smaller memory
    consumption
    */
    TH1* tree_to_histo(
        const char* tree_name,
        const char* name,
        const char* var,
        TCut cut ) const;

    #ifndef __CINT__
    //! project chain from files into histograms define by projections
    void tree_to_histo( const std::string& tree_name, ProjectionList& projection_list ) const;
    #endif

    //! merge TH1 from files
    TH1* get_th1( const char* key ) const;

    //! merge TH2 from files
    TH2* get_th2( const char* key ) const;

    //!  merge all trees/histograms found in file_selection into output_file
    void merge( const char* output_file = "out.root", const char* selection="", const char* option = "" ) const;

    //! merge all histograms found in file_selection into output file
    /*! keeps TDirectory structure of the input files into output file */
    void merge_recursive( const char* output_file = "out.root", const char* selection="" ) const;

    //! write files
    void dump_files( void ) const;

    //! shortcut for set of files
    typedef std::set< std::string > FileSet;
    typedef std::vector< std::string > FileList;

    //! get number of files
    unsigned int get_n_files( void ) const
    { return _files.size(); }

    //! get list of files
    const FileSet& get_files( void ) const
    { return _files; }

    //! verbosity
    MUTOO_MACRO::Verbosity get_verbosity( void ) const
    { return _verbosity; }

    //! verbosity
    void set_verbosity( MUTOO_MACRO::Verbosity value )
    { _verbosity = value; }

    //!@name utility functions
    //@{

    //! file size
    static int file_size( const char* file );

    //@}

    protected:

    //! recursive merging of TDirectories
    void _merge_recursive(TDirectory *root,TDirectory* node, const std::string& selection) const;


    //! list of input files
    FileSet _files;

    //! verbosity
    MUTOO_MACRO::Verbosity _verbosity;

    //! root dictionary
    ClassDef( FileManager, 0 );

};


#endif
