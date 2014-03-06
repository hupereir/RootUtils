// $Id: Postscript.cxx,v 1.4 2010/08/27 22:07:51 hpereira Exp $

#include "Postscript.h"

#include <iostream>
#include <assert.h>
#include <TPaveText.h>

ClassImp( Postscript);

//____________________________________________________
bool Postscript::Open( TString file, int type, double range_x, double range_y )
{
  if( fState == OPEN )
  {
    std::cout << "Postscript::open - already opened" << std::endl;
    return false;
  }

  std::cout << "Postscript::open - file: " << file << " type: " << type << std::endl;
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

  std::cout << "Postscript::close - file: " << fFile << std::endl;
  if( fState == CLOSED )
  {
    std::cout << "Postscript::close - not opened" << std::endl;
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
void Postscript::TitlePage( TCanvas* cv, TString title )
{
  std::cout << "Postscript::TitlePage - title = " << title  << std::endl;
  assert( fState == OPEN );
  NewPage();
  fPostscript->On();
  fPostscript->SetTextSize(0.025);
  fPostscript->SetTextAlign(21);
  fPostscript->TextNDC(0.5, 0.5, title );
  fPostscript->Off();
  return;
}

//________________________________________________________________________________
void Postscript::Title( TString title )
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
void Postscript::NewPage( TString title )
{
  assert( fState == OPEN );
  fPostscript->On();
  fPostscript->NewPage();
  fPostscript->Off();

  // draw title
  if( title.Length() > 0 ) Title( title );

}
