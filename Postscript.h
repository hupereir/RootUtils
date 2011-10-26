// $Id: Postscript.h,v 1.3 2010/08/27 22:07:52 hpereira Exp $

#ifndef PostScript_h
#define PostScript_h

/*!
\file PostScript.h
\brief used to save plots in multipage postscript file
*/

#include <assert.h>
#include <string>
#include <TObject.h>
#include <TPostScript.h>
#include <TCanvas.h>

//! used to save plots in multipage postscript file
class Postscript: public TObject
{

    public:

    //! state
    enum State {
        OPEN,
        CLOSED
    };

    //! constructor
    //Postscript( const char* file = "plots.ps", int type = 111, double range_x = 20, double range_y = 29 ):
    Postscript( const char* file = "plots.ps", int type = 111, double range_x = 28, double range_y = 40 ):
        _postscript( 0 ),
        _state( CLOSED )
    { open( file, type, range_x, range_y ); }

    //! destructor
    ~Postscript( void )
    { if( _state == OPEN ) close(); }

    //! open postscript file
    bool open( const char* file = "plot.ps", int type = 111, double range_x = 20, double range_y = 29 );

    //! close postscript file
    bool close( void );

    //! get TPostscript singleton
    TPostScript& get( void ) const
    {
        assert( _postscript );
        return *_postscript;
    }

    //! blank page with title in center
    void title_page( TCanvas* cv, const char* title = "title_page" );

    //! update postscript file from canvas
    void update_from_canvas( TCanvas *cv );

    //! new page
    void new_page( void );

    //! title
    void title( const char* title );

    private:

    //! postscript file name
    std::string _file;

    //! TPostscript interface
    TPostScript* _postscript;

    //! state
    State _state;

    ClassDef( Postscript, 0 );

};

#endif
