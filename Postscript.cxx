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
bool Postscript::Open( const char* file, int type, double range_x, double range_y )
{
    if( fState == OPEN )
    {
        cout << "Postscript::open - already opened" << endl;
        return false;
    }

    cout << "Postscript::open - file: " << file << " type: " << type << endl;
    fPostscript = new TPostScript( file, type );
    fPostscript->Range( range_x, range_y );
    fPostscript->Off();

    fState = OPEN;
    fFile = file;
    return true;

}

//____________________________________________________
bool  Postscript::Close( void )
{

    cout << "Postscript::close - file: " << fFile << endl;
    if( fState == CLOSED )
    {
        cout << "Postscript::close - not opened" << endl;
        return false;
    }

    fPostscript->On();
    fPostscript->Close();
    delete fPostscript;
    fPostscript = 0;

    fState = CLOSED;
    return true;
}

//____________________________________________________
void Postscript::TitlePage( TCanvas* cv, const char* title )
{
    cout << "Postscript::TitlePage - title = " << title  << endl;
    assert( fState == OPEN );
    NewPage();
    fPostscript->On();
    fPostscript->SetTextSize(0.025);
    fPostscript->SetTextAlign(21);
    fPostscript->TextNDC(0.5, 1.0, title );
    fPostscript->Off();
    return;
}

//________________________________________________________________________________
void Postscript::Title( const char* title )
{

    assert( fState == OPEN );

    TPaveText *text = new TPaveText(0.2, 0.95, 0.8, 1.0, "NDC");
    text->SetBorderSize( 0 );
    text->SetLineWidth( 0 );
    text->SetFillStyle( 0 );
    text->AddText( title );

    fPostscript->On();
    text->Draw();
    fPostscript->Off();

}

//________________________________________________________________________________
void Postscript::UpdateFromCanvas( TCanvas *cv )
{
    assert( fState == OPEN );

    fPostscript->On();
    cv->cd();
    cv->Draw();
    fPostscript->Off();
}

//____________________________________________________
void Postscript::NewPage( void )
{
    assert( fState == OPEN );
    fPostscript->On();
    fPostscript->NewPage();
    fPostscript->Off();
}
