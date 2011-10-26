// $Id: Postscript.cxx,v 1.4 2010/08/27 22:07:51 hpereira Exp $

/*!
\file PostScript.cxx
\brief used to save plots in multipage postscript file
*/

#include <iostream>
#include <assert.h>
#include <TPaveText.h>
#include "Postscript.h"
using namespace std;

ClassImp( Postscript);

//____________________________________________________
bool Postscript::open( const char* file, int type, double range_x, double range_y )
{
    if( _state == OPEN )
    {
        cout << "Postscript::open - already opened" << endl;
        return false;
    }

    cout << "Postscript::open - file: " << file << " type: " << type << endl;
    _postscript = new TPostScript( file, type );
    _postscript->Range( range_x, range_y );
    _postscript->Off();

    _state = OPEN;
    _file = file;
    return true;

}

//____________________________________________________
bool  Postscript::close( void )
{

    cout << "Postscript::close - file: " << _file << endl;
    if( _state == CLOSED )
    {
        cout << "Postscript::close - not opened" << endl;
        return false;
    }

    _postscript->On();
    _postscript->Close();
    delete _postscript;
    _postscript = 0;

    _state = CLOSED;
    return true;
}

//____________________________________________________
void Postscript::title_page( TCanvas* cv, const char* title )
{
    cout << "Postscript::title_page - title = " << title  << endl;
    assert( _state == OPEN );
    new_page();
    _postscript->On();
    _postscript->SetTextSize(0.025);
    _postscript->SetTextAlign(21);
    _postscript->TextNDC(0.5, 1.0, title );
    _postscript->Off();
    return;
}

//________________________________________________________________________________
void Postscript::title( const char* title )
{

    assert( _state == OPEN );

    TPaveText *text = new TPaveText(0.2, 0.95, 0.8, 1.0, "NDC");
    text->SetBorderSize( 0 );
    text->SetLineWidth( 0 );
    text->SetFillStyle( 0 );
    text->AddText( title );

    _postscript->On();
    text->Draw();
    _postscript->Off();

}

//________________________________________________________________________________
void Postscript::update_from_canvas( TCanvas *cv )
{
    assert( _state == OPEN );

    _postscript->On();
    cv->cd();
    cv->Draw();
    _postscript->Off();
}

//____________________________________________________
void Postscript::new_page( void )
{
    assert( _state == OPEN );
    _postscript->On();
    _postscript->NewPage();
    _postscript->Off();
}
