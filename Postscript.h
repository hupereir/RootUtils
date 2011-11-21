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
        fPostscript( 0 ),
        fState( CLOSED )
    { Open( file, type, range_x, range_y ); }

    //! destructor
    ~Postscript( void )
    { if( fState == OPEN ) Close(); }

    //! open postscript file
    bool Open( const char* file = "plot.ps", int type = 111, double range_x = 20, double range_y = 29 );

    //! close postscript file
    bool Close( void );

    //! get TPostscript singleton
    TPostScript& Get( void ) const
    {
        assert( fPostscript );
        return *fPostscript;
    }

    //! blank page with title in center
    void TitlePage( TCanvas* cv, const char* title = "TitlePage" );

    //! update postscript file from canvas
    void UpdateFromCanvas( TCanvas *cv );

    //! new page
    void NewPage( void );

    //! title
    void Title( const char* title );

    private:

    //! postscript file name
    std::string fFile;

    //! TPostscript interface
    TPostScript* fPostscript;

    //! state
    State fState;

    ClassDef( Postscript, 0 );

};

#endif
