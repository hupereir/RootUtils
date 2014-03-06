// $Id: Draw.cxx,v 1.9 2011/07/01 15:15:05 hpereira Exp $


#include <TBox.h>
#include <TCanvas.h>
#include <TColor.h>
#include <TLine.h>
#include <TMarker.h>
#include <TPad.h>
#include <TPolyLine.h>
#include <TStyle.h>
#include <iostream>
#include <cmath>

#include "Color.h"
#include "Draw.h"

//__________________________________________________
//! root dictionary
ClassImp( Draw );

//__________________________________________________
// static members
Bool_t Draw::fDrawErrLimit = true;
Int_t Draw::fLineWidth = 2;
Int_t Draw::fBoxFillStyle = 2;
Int_t Draw::fFont = 42;
Double_t Draw::fMarkerSize = 2.0;
Double_t Draw::fSystShade = 0.4;
Double_t Draw::fDeltaXLog = 0.03;
Double_t Draw::fDeltaXLin = 0.012;


//__________________________________________________________
void Draw::SetStyle( Bool_t use_title )
{
  gStyle->SetOptStat(0);
  gStyle->SetTitleBorderSize(0);
  gStyle->SetTitleW(1);

  gStyle->SetPadLeftMargin(0.12);
  gStyle->SetPadRightMargin( 0.02 );
  gStyle->SetPadBottomMargin(0.13);

  if( use_title )
  {

    gStyle->SetTitleSize(0.08,"");
    gStyle->SetTitleStyle(0);
    gStyle->SetPadTopMargin(0.12);

  } else {

    gStyle->SetTitleSize(0,"");
    gStyle->SetTitleStyle(0.05);
    gStyle->SetPadTopMargin(0.02);

  }

  gStyle->SetMarkerSize( fMarkerSize );
  gStyle->SetPadTickX( 1 );
  gStyle->SetPadTickY( 1 );

  gStyle->SetTextFont( fFont );
  gStyle->SetLabelFont( fFont ,"xyz");
  gStyle->SetTitleFont( fFont ,"xyz");
  gStyle->SetStatFont( fFont );

  gStyle->SetStatFontSize(0.05);
  gStyle->SetStatX(0.97);
  gStyle->SetStatY(0.98);
  gStyle->SetStatH(0.03);
  gStyle->SetStatW(0.3);

  gStyle->SetTickLength(0.02,"y");
  gStyle->SetEndErrorSize(3);
  gStyle->SetLabelSize(0.05,"xyz");
  gStyle->SetLabelOffset(0.01,"xyz");

  gStyle->SetTitleSize(0.05, "xyz" );
  gStyle->SetTitleOffset(1.2,"y");
  gStyle->SetTitleOffset(1.2,"x");

}

//______________________________________________________
void Draw::DivideCanvas( TCanvas* cv, Int_t n, Bool_t respect_ratio )
{

    Double_t ratio( respect_ratio ? double(cv->GetWindowWidth())/ double(cv->GetWindowHeight()): 1 );
    Int_t columns = std::max( 1, int(sqrt( n*ratio )) );
    Int_t rows = std::max( 1, int(n/columns) );
    while( rows * columns < n )
    {
        columns++;
        if( rows * columns < n ) rows++;
    }

    cv->Divide( rows, columns );

}

//______________________________________________________
TLatex* Draw::PutText( Double_t x_ndc, Double_t y_ndc, const int& color, TString value)
{
    TLatex* text = new TLatex();
    text->SetNDC( true );
    text->SetTextColor(color);
    text->DrawLatex( x_ndc, y_ndc, value );
    return text;
}

//______________________________________________________
TPaveStats* Draw::ResizeStats( Double_t scale, Int_t dimension )
{

    gPad->Update();
    TPaveStats *st = (TPaveStats*)gPad->GetPrimitive("stats");
    if( !st ) {
        std::cout << "Draw::ResizeStats - cannot access PaveStats.\n";
        return 0;
    }

    // resize font
    Double_t fontSize( st->GetTextSize() );
    fontSize *= scale;
    st->SetTextSize( fontSize );

    // retrieve pave positions:
    Coord_t x1 = st->GetX1NDC();
    Coord_t x2 = st->GetX2NDC();
    Coord_t y1 = st->GetY1NDC();
    Coord_t y2 = st->GetY2NDC();

    x1 -= scale*( x2-x1 );
    y1 -= scale*( y2-y1 );
    if( dimension & WIDTH ) {
        st->SetX1NDC(x1);
        st->SetX2NDC(x2);
    }

    if( dimension & HEIGHT ) {
        st->SetY1NDC(y1);
        st->SetY2NDC(y2);
    }

    st->Draw();
    gPad->Update();
    return st;

}

//______________________________________________________
TPaveStats* Draw::UpdateStats(
    Int_t direction,
    Int_t color,
    TString newName,
    Int_t scale,
    Int_t option )
{

    gPad->Update();
    TPaveStats *st = (TPaveStats*)gPad->GetPrimitive("stats");
    if( !st ) {
        std::cout << "Draw::UpdateStats - cannot access PaveStats.\n";
        return 0;
    }

    if( newName.Length() > 0 ) st->SetName( newName );

    if( direction != NONE )
    {
        Coord_t x1 = st->GetX1NDC();
        Coord_t x2 = st->GetX2NDC();
        Coord_t y1 = st->GetY1NDC();
        Coord_t y2 = st->GetY2NDC();
        Coord_t xOffset = x2-x1;
        Coord_t yOffset = y2-y1;

        if( direction & DOWN )
        {
            y2 -= scale*yOffset;
            y1 -= scale*yOffset;
        }
        else if( direction & UP )
        {
            y1 += scale*yOffset;
            y2 += scale*yOffset;
        }

        if( direction & LEFT )
        {
            x2 -= scale*xOffset;
            x1 -= scale*xOffset;
        }
        else if( direction & RIGHT )
        {
            x1 += scale*xOffset;
            x2 += scale*xOffset;
        }

        st->SetX1NDC(x1);
        st->SetY1NDC(y1);
        st->SetX2NDC(x2);
        st->SetY2NDC(y2);
    }

    if( color > 0 ) {
        st->SetLineColor( color );
        st->SetTextColor( color );
    }

    if( option & TRANSPARENT )
    {
        st->SetFillStyle( 0 );
        st->SetFillColor( 0 );
    }

    st->Draw();
    gPad->Update();
    return st;

}

//______________________________________________________
void Draw::UpdatePaveSize(
    TPaveText* pave,
    Int_t direction
    )
{

    Int_t nLines( pave->GetListOfLines()->GetSize() );
    if( !nLines ) return;

    // resize height
    if( (direction & UP) || (direction & DOWN) )
    {

        Double_t lineHeight( pave->GetTextSize() );
        if( lineHeight <= 0 ) return;

        if( direction & UP )
        {

            pave->SetY2( pave->GetY1() + nLines*lineHeight );

        } else if( direction & DOWN ) {

            pave->SetY1( pave->GetY2() - nLines*lineHeight );
        }

    }

    if( (direction & LEFT) || (direction & RIGHT ) )
    {
        UInt_t nCharMax( 0 );
        Double_t charWidth( pave->GetTextSize() );
        Double_t ratio( 0 );
        for( Int_t line = 0; line < nLines; line++ )
        {
            TText *text( pave->GetLine( line ) );
            UInt_t n_char( strlen( text->GetTitle() ) );
            if( n_char > nCharMax )
            {
                nCharMax = n_char;
                UInt_t w(0), h(0);
                text->GetTextExtent( w, h, text->GetTitle() );
                ratio = double(w)/double(h);
            }
        }

        if( direction & LEFT )
        {
            pave->SetX1( pave->GetX2() - ratio*charWidth );

        } else if( direction & RIGHT ) {

            pave->SetX2( pave->GetX1() + ratio*charWidth );

        }

    }

    pave->Draw();
    gPad->Update();
    return;

}

//______________________________________________________
void Draw::ResizeAxis( TH1* h, Double_t size, Int_t flag, Bool_t update )
{

    if( !h ) return;
    if( flag & X ) {

        if( flag & LABEL ) h->GetXaxis()->SetLabelSize( size );
        if( flag & TITLE ) h->GetXaxis()->SetTitleSize( size );

    }

    if( flag & Y ) {

        if( flag & LABEL ) h->GetYaxis()->SetLabelSize( size );
        if( flag & TITLE ) h->GetYaxis()->SetTitleSize( size );

    }

    if( update ) gPad->Update();

    return;

}


//__________________________________________________________
TMarker* Draw::DrawPoint(
    Double_t x, Double_t y,
    Double_t xErr, Double_t yErr,
    Int_t symbol, Int_t color,
    Int_t flag )
{
    Bool_t draw_err_limit_x =fDrawErrLimit;
    Bool_t draw_err_limit_y =fDrawErrLimit;

    printf("x= %10g +/- %10g y= %10g +/- %10g\n",	x, xErr, y, yErr );

    // canvas limits
    Double_t uxmin = gPad->GetUxmin();
    Double_t uxmax = gPad->GetUxmax();
    Double_t uymin = gPad->GetUymin();
    Double_t uymax = gPad->GetUymax();

    // error bar dimensions
    Double_t deltaX = (flag&LOG_X) ? 0.05:0.013 * ( uxmax - uxmin );
    Double_t deltaY = (flag&LOG_Y) ? 0.01:0.013 * ( uymin - uymax );

    TLine *line = new TLine();
    line->SetLineColor( color );
    line->SetLineWidth( fLineWidth );

    // error along x
    if( xErr ) {
        Double_t x_min = x-xErr;
        Double_t x_max = x+xErr;

        if( flag&LOG_X )
        {
            if( x_min <=0 || log10(x_min) < uxmin ) { x_min = exp( uxmin*log(10.0) ); draw_err_limit_x = false; }
            if( log10(x_max) > uxmax ) { x_max = exp( uxmax*log(10.0) ); draw_err_limit_x = false; }
        } else {
            if( x_min < uxmin ) { x_min = uxmin; draw_err_limit_x = false; }
            if( x_max > uxmax ) { x_max = uxmax; draw_err_limit_x = false; }
        }

        line->DrawLine( x_min, y, x_max,  y );

        if( draw_err_limit_x )
        {
            if( flag & LOG_X )
            {
                line->DrawLine( x_min, y*(1-deltaY), x_min,  y*(1+deltaY) );
                line->DrawLine( x_max, y*(1-deltaY), x_max,  y*(1+deltaY) );
            } else {
                line->DrawLine( x_min, y-deltaY, x_min,  y+deltaY );
                line->DrawLine( x_max, y-deltaY, x_max,  y+deltaY );
            }
        }
    }

    // error along y
    if( yErr )
    {

        Double_t yMin = y-yErr;
        Double_t yMax = y+yErr;

        if( flag&LOG_Y )
        {
            if( yMin <=0 || log10(yMin) < uymin ) { yMin = exp( uymin*log(10.0) ); draw_err_limit_y = false; }
            if( log10(yMax) > uymax ) { yMax = exp( uymax*log(10.0) ); draw_err_limit_y = false; }
        } else {
            if( yMin < uymin ) { yMin = uymin; draw_err_limit_y = false; }
            if( yMax > uymax ) { yMax = uymax; draw_err_limit_y = false; }
        }

        line->DrawLine( x, yMin, x,  yMax );

        if( draw_err_limit_y )
        {
            if( flag & LOG_X )
            {
                line->DrawLine( x*(1-deltaX), yMin, x*(1+deltaX),  yMin );
                line->DrawLine( x*(1-deltaX), yMax, x*(1+deltaX),  yMax );
            } else {
                line->DrawLine( x-deltaX, yMin, x+deltaX,  yMin );
                line->DrawLine( x-deltaX, yMax, x+deltaX,  yMax );
            }
        }
    }

    // draw symbol
    TMarker *marker = new TMarker();
    if( symbol>= 24 )
    {
        /*
        if marker is open symbol,
        first draw full symboll white,
        then open symbol on top
        */
        marker->SetMarkerColor(0);
        marker->SetMarkerStyle( symbol - 4 );
        marker->DrawMarker(x,y);

        marker->SetMarkerColor( color );
        marker->SetMarkerStyle( symbol );
        marker->DrawMarker( x, y );

    } else {

        marker->SetMarkerColor( color );
        marker->SetMarkerStyle( symbol );
        marker->DrawMarker( x, y );

    }

    return marker;
}

//__________________________________________________________
void Draw::DrawPoint( TGraphErrors *graph, Int_t flag )
{

    Int_t symbol( graph->GetMarkerStyle() );
    Int_t color( graph->GetMarkerColor() );
    for( Int_t i=0; i<graph->GetN(); i++ )
    {
        Double_t x, y;
        graph->GetPoint( i, x, y  );
        Double_t xErr = graph->GetErrorX(i);
        Double_t yErr = graph->GetErrorY(i);
        DrawPoint( x, y, xErr, yErr, symbol, color, flag );
    }
}

//__________________________________________________________
void Draw::DrawSystBracket(
    Double_t x, Double_t y,
    Double_t yErr,
    Int_t color, Int_t flag )
{
    // check error
    if( yErr <= 0 ) {
        std::cout << "Draw::DrawSystBracket - invalid y error" << std::endl;
        return;
    }

    // canvas limits
    Double_t uxmin = gPad->GetUxmin();
    Double_t uxmax = gPad->GetUxmax();
    Double_t uymin = gPad->GetUymin();
    Double_t uymax = gPad->GetUymax();

    // bracket limits
    Double_t deltaX = (flag&LOG_X) ? 0.07:0.015 * ( uxmax - uxmin );
    Double_t deltaY = (flag&LOG_Y) ? 0.07:0.01 * ( uymax - uymin );

    // error limits
    Double_t yMin = y-yErr;
    Double_t yMax = y+yErr;

    TPolyLine* pline = new TPolyLine();
    pline->SetLineColor( color );
    pline->SetLineWidth( fLineWidth );

    static Double_t px[4] = {0};
    static Double_t py[4] = {0};

    // lower bracket
    Bool_t draw_lower = (flag&LOG_Y) ? (log10(yMin) >= uymin):(yMin>=uymin);
    if( draw_lower ) {
        if( flag&LOG_Y )
        {
            py[0] = log10((1+deltaY)*(yMin));
            py[1] = log10(yMin);
            py[2] = log10(yMin);
            py[3] = log10((1+deltaY)*(yMin));
        } else {
            py[0] = yMin+deltaY;
            py[1] = yMin;
            py[2] = yMin;
            py[3] = yMin+deltaY;
        }

        if( flag&LOG_X )
        {
            px[0] = log10((1-deltaX)*x);
            px[1] = log10((1-deltaX)*x);
            px[2] = log10((1+deltaX)*x);
            px[3] = log10((1+deltaX)*x);
        } else {
            px[0] = x-deltaX;
            px[1] = x-deltaX;
            px[2] = x+deltaX;
            px[3] = x+deltaX;
        }
        pline->DrawPolyLine( 4, px, py);
    }

    // upper bracket
    Bool_t draw_upper = (flag&LOG_Y) ? (log10(yMax) <= uymax):(yMax<=uymax);
    if( draw_upper ) {
        if( flag&LOG_Y )
        {
            py[0] = log10((1-deltaY)*(y+yErr));
            py[1] = log10(y+yErr);
            py[2] = log10(y+yErr);
            py[3] = log10((1-deltaY)*(y+yErr));
        } else {
            py[0] = y+yErr-deltaY;
            py[1] = y+yErr;
            py[2] = y+yErr;
            py[3] = y+yErr-deltaY;
        }
        if( flag&LOG_X )
        {
            px[0] = log10((1-deltaX)*x);
            px[1] = log10((1-deltaX)*x);
            px[2] = log10((1+deltaX)*x);
            px[3] = log10((1+deltaX)*x);
        } else {
            px[0] = x-deltaX;
            px[1] = x-deltaX;
            px[2] = x+deltaX;
            px[3] = x+deltaX;
        }
        pline->DrawPolyLine( 4, px, py);
    }

}

//__________________________________________________________
void Draw::DrawSystBracket( TGraphErrors *graph, Int_t flag )
{

    Int_t color( graph->GetMarkerColor() );
    for( Int_t i=0; i<graph->GetN(); i++ )
    {
        Double_t x, y;
        graph->GetPoint( i, x, y  );
        Double_t yErr = graph->GetErrorY(i);
        DrawSystBracket( x, y, yErr, color, flag );
    }
}

//__________________________________________________________
void Draw::DrawSystBox(
    Double_t x, Double_t y,
    Double_t yErr,
    Int_t color, Int_t flag )
{
    // check error
    if( yErr <= 0 ) {
        std::cout << "Draw::DrawSystBox - invalid y error" << std::endl;
        return;
    }

    // canvas limits
    Double_t uxmin = gPad->GetUxmin();
    Double_t uxmax = gPad->GetUxmax();
    Double_t uymin = gPad->GetUymin();
    Double_t uymax = gPad->GetUymax();

    // Relative flag
    if( (flag&RELATIVE) ) yErr = fabs( y*yErr );

    // error limits
    Double_t yMin = std::max<double>( (flag&LOG_Y) ? exp( log(10.0)*uymin ):uymin , y-yErr );
    Double_t yMax = std::min<double>( (flag&LOG_Y) ? exp( log(10.0)*uymax ):uymax , y+yErr );

    // bracket limits
    Double_t deltaX = (flag&LOG_X) ? fDeltaXLog:fDeltaXLin * ( uxmax - uxmin );

    // box limits
    Double_t x0 = (flag&LOG_X) ?  log10((1-deltaX)*x):x-deltaX;
    Double_t x1 = (flag&LOG_X) ?  log10((1+deltaX)*x):x+deltaX;
    Double_t y0 = (flag&LOG_Y) ? log10(yMin):yMin;
    Double_t y1 = (flag&LOG_Y) ? log10(yMax):yMax;

    TBox *box = new TBox(x0, y0, x1, y1 );
    color = Color(color).Merge( 0, 0.4 );

    box->SetFillColor( color );
    box->SetLineColor( color );
    if( fBoxFillStyle ) box->SetFillStyle( fBoxFillStyle );
    box->Draw();
}

//__________________________________________________________
void Draw::DrawSystBox( TGraphErrors *graph, Int_t flag )
{

    Int_t color( graph->GetMarkerColor() );
    for( Int_t i=0; i<graph->GetN(); i++ )
    {
        Double_t x, y;
        graph->GetPoint( i, x, y  );
        Double_t yErr = graph->GetErrorY(i);
        DrawSystBox( x, y, yErr, color, flag );
    }
}

//___________________________________________________________
void Draw::DrawSystGlobal(  TGraph* tg, Double_t err_rel, Int_t flag )
{

    const Int_t n_points = tg->GetN();

    TPolyLine* pline = new TPolyLine(2*n_points+1);
    pline->SetLineColor( tg->GetLineColor() );
    pline->SetLineWidth( tg->GetLineWidth() );

    for( Int_t i=0; i<n_points; i++ )
    {
        Double_t x = 0;
        Double_t y = 0;
        tg->GetPoint( i, x, y );
        Double_t y_up =  y*(1+err_rel);
        Double_t y_down =  y*(1-err_rel);

        if( flag & LOG_X ) x=log10(x);
        if( flag & LOG_Y ) {
            y_up = log10(y_up);
            y_down = log10( y_down );
        }

        pline->SetPoint( i, x, y_up );
        pline->SetPoint( 2*n_points - i - 1, x, y_down );
        if( i == 0 ) pline->SetPoint( 2*n_points, x, y_up );
    }

    pline->Draw();

}

//___________________________________________________________
void Draw::DrawBox(
    Double_t x, Double_t y,
    Double_t deltaX, Double_t deltaY,
    Int_t color, Int_t flag )
{
    printf( "global= %10g\n", deltaY );

    Double_t x0 = (flag&LOG_X)? log10(x)+deltaX:x+deltaX;
    Double_t x1 = (flag&LOG_X)? log10(x)-deltaX:x-deltaX;

    Double_t y0 = (flag&LOG_Y)? log10(y+deltaY):y+deltaY;
    Double_t y1 = (flag&LOG_Y)? log10(y-deltaY):y-deltaY;

    TBox *box = new TBox( x0, y0, x1, y1 );
    if( fBoxFillStyle ) box->SetFillStyle( fBoxFillStyle );
    box->SetFillColor( color );
    box->SetLineColor( color );
    box->Draw();

}

//__________________________________________________
TGraphErrors* Draw::DrawMarkers( Int_t n, Double_t* x, Double_t* y, Double_t* xErr, Double_t* yErr, Int_t symbol, Int_t color )
{
  TGraphErrors* tge = new TGraphErrors();
  for( Int_t i=0; i<n; ++i )
  {
    tge->SetPoint( i, x[i], y[i] );
    tge->SetPointError( i, xErr[i], yErr[i]  );

  }

  tge->SetMarkerStyle( symbol );
  tge->SetMarkerColor( color );
  tge->SetLineWidth( fLineWidth );
  tge->SetLineColor( color );
  tge->Draw( "P" );
  return tge;

}

//__________________________________________________
TGraphErrors* Draw::DrawSystematics( Int_t n, Double_t* x, Double_t* y, Double_t* yErr, Int_t color )
{

  Double_t uxmin = gPad->GetUxmin();
  Double_t uxmax = gPad->GetUxmax();
  Double_t deltaX = fDeltaXLin*( uxmax - uxmin );

  TGraphErrors* tge = new TGraphErrors();
  for( Int_t i=0; i<n; ++i )
  {
    tge->SetPoint( i, x[i], y[i] );
    tge->SetPointError( i, deltaX, yErr[i]  );
  }

  tge->SetLineWidth( fLineWidth );
  tge->SetLineColor( color );
  tge->SetFillStyle( 0 );
  tge->Draw( "5" );
  return tge;

}


//__________________________________________________
TGraphErrors* Draw::DrawBand( Int_t n, Double_t* x, Double_t* yMin, Double_t* yMax, Int_t color )
{
  TGraphErrors* tge = new TGraphErrors();
  for( Int_t i=0; i<n; ++i )
  {
    Double_t y = 0.5*(yMin[i]+yMax[i]);
    Double_t yErr = 0.5*fabs( yMin[i]-yMax[i] );
    tge->SetPoint( i, x[i], y );
    tge->SetPointError( i, 0, yErr  );

  }

  tge->SetLineWidth( fLineWidth );
  tge->SetFillStyle( 3363 );
  tge->SetFillColor( color );
  tge->SetLineColor( color );
  tge->Draw( "3" );

  TGraph* tgLow = new TGraph( n, x, yMin );
  tgLow->SetLineWidth( fLineWidth );
  tgLow->SetLineColor( color );
  tgLow->Draw( "L" );

  TGraph* tgHigh = new TGraph( n, x, yMax );
  tgHigh->SetLineWidth( fLineWidth );
  tgHigh->SetLineColor( color );
  tgHigh->Draw( "L" );

  return tge;

}
