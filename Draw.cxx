// $Id: Draw.cxx,v 1.9 2011/07/01 15:15:05 hpereira Exp $


#include <TBox.h>
#include <TCanvas.h>
#include <TColor.h>
#include <TLine.h>
#include <TMarker.h>
#include <TPad.h>
#include <TPolyLine.h>
#include <iostream>
#include <cmath>

#include "Color.h"
#include "Draw.h"

//__________________________________________________
//! root dictionary
ClassImp( Draw );

//__________________________________________________
// static members
bool Draw::_draw_err_limit = true;
int Draw::_line_width = 2;
int Draw::_box_fill_style = 2;
double Draw::_syst_shade = 0.4;
double Draw::_delta_x_log = 0.03;
double Draw::_delta_x_lin = 0.012;

//______________________________________________________
void Draw::divide_canvas( TCanvas* cv, int n )
{

    double ratio( double(cv->GetWindowWidth())/ double(cv->GetWindowHeight()) );
    int columns = std::max( 1, int(sqrt( n*ratio )) );
    int rows = std::max( 1, int(n/columns) );
    while( rows * columns < n )
    {
        columns++;
        if( rows * columns < n ) rows++;
    }

    cv->Divide( rows, columns );

}

//______________________________________________________
TLatex* Draw::put_text( const double& x_ndc, const double& y_ndc, const int& color, const char* value)
{
    TLatex* text = new TLatex();
    text->SetNDC( true );
    text->SetTextColor(color);
    text->DrawLatex( x_ndc, y_ndc, value );
    return text;
}

//______________________________________________________
TPaveStats* Draw::resize_stats( double scale, int dimension )
{

    gPad->Update();
    TPaveStats *st = (TPaveStats*)gPad->GetPrimitive("stats");
    if( !st ) {
        std::cout << "Draw::resize_stats - cannot access PaveStats.\n";
        return 0;
    }

    // resize font
    double font_size( st->GetTextSize() );
    font_size *= scale;
    st->SetTextSize( font_size );

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
TPaveStats* Draw::update_stats(
    int direction,
    int color,
    const char* new_name,
    int scale,
    int option )
{

    gPad->Update();
    TPaveStats *st = (TPaveStats*)gPad->GetPrimitive("stats");
    if( !st ) {
        std::cout << "Draw::update_stats - cannot access PaveStats.\n";
        return 0;
    }

    if( new_name ) st->SetName( new_name );

    if( direction != NONE )
    {
        Coord_t x1 = st->GetX1NDC();
        Coord_t x2 = st->GetX2NDC();
        Coord_t y1 = st->GetY1NDC();
        Coord_t y2 = st->GetY2NDC();
        Coord_t x_offset = x2-x1;
        Coord_t y_offset = y2-y1;

        if( direction & DOWN )
        {
            y2 -= scale*y_offset;
            y1 -= scale*y_offset;
        }
        else if( direction & UP )
        {
            y1 += scale*y_offset;
            y2 += scale*y_offset;
        }

        if( direction & LEFT )
        {
            x2 -= scale*x_offset;
            x1 -= scale*x_offset;
        }
        else if( direction & RIGHT )
        {
            x1 += scale*x_offset;
            x2 += scale*x_offset;
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
void Draw::update_pave_size(
    TPaveText* pave,
    int direction
    )
{

    int n_lines( pave->GetListOfLines()->GetSize() );
    if( !n_lines ) return;

    // resize height
    if( (direction & UP) || (direction & DOWN) )
    {

        double line_height( pave->GetTextSize() );
        if( direction & UP )
        {

            pave->SetY2( pave->GetY1() + n_lines*line_height );

        } else if( direction & DOWN ) {

            pave->SetY1( pave->GetY2() - n_lines*line_height );
        }

    }

    if( (direction & LEFT) || (direction & RIGHT ) )
    {
        unsigned int n_char_max( 0 );
        double char_width( pave->GetTextSize() );
        double ratio( 0 );
        for( int line = 0; line < n_lines; line++ )
        {
            TText *text( pave->GetLine( line ) );
            unsigned int n_char( strlen( text->GetTitle() ) );
            if( n_char > n_char_max )
            {
                n_char_max = n_char;
                unsigned int w(0), h(0);
                text->GetTextExtent( w, h, text->GetTitle() );
                ratio = double(w)/double(h);
            }
        }

        if( direction & LEFT )
        {
            pave->SetX1( pave->GetX2() - ratio*char_width );

        } else if( direction & RIGHT ) {

            pave->SetX2( pave->GetX1() + ratio*char_width );

        }

    }

    pave->Draw();
    gPad->Update();
    return;

}

//______________________________________________________
void Draw::resize_axis( TH1* h, double size, int flag, bool update )
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
TMarker* Draw::draw_point(
    double x, double y,
    double x_err, double y_err,
    int symbol, int color,
    int flag )
{
    bool draw_err_limit_x =_draw_err_limit;
    bool draw_err_limit_y =_draw_err_limit;

    printf("x= %10g +/- %10g y= %10g +/- %10g\n",	x, x_err, y, y_err );

    // canvas limits
    double uxmin = gPad->GetUxmin();
    double uxmax = gPad->GetUxmax();
    double uymin = gPad->GetUymin();
    double uymax = gPad->GetUymax();

    // error bar dimensions
    double delta_x = (flag&LOG_X) ? 0.05:0.013 * ( uxmax - uxmin );
    double delta_y = (flag&LOG_Y) ? 0.01:0.013 * ( uymin - uymax );

    TLine *line = new TLine();
    line->SetLineColor( color );
    line->SetLineWidth( _line_width );

    // error along x
    if( x_err ) {
        double x_min = x-x_err;
        double x_max = x+x_err;

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
                line->DrawLine( x_min, y*(1-delta_y), x_min,  y*(1+delta_y) );
                line->DrawLine( x_max, y*(1-delta_y), x_max,  y*(1+delta_y) );
            } else {
                line->DrawLine( x_min, y-delta_y, x_min,  y+delta_y );
                line->DrawLine( x_max, y-delta_y, x_max,  y+delta_y );
            }
        }
    }

    // error along y
    if( y_err )
    {

        double y_min = y-y_err;
        double y_max = y+y_err;

        if( flag&LOG_Y )
        {
            if( y_min <=0 || log10(y_min) < uymin ) { y_min = exp( uymin*log(10.0) ); draw_err_limit_y = false; }
            if( log10(y_max) > uymax ) { y_max = exp( uymax*log(10.0) ); draw_err_limit_y = false; }
        } else {
            if( y_min < uymin ) { y_min = uymin; draw_err_limit_y = false; }
            if( y_max > uymax ) { y_max = uymax; draw_err_limit_y = false; }
        }

        line->DrawLine( x, y_min, x,  y_max );

        if( draw_err_limit_y )
        {
            if( flag & LOG_X )
            {
                line->DrawLine( x*(1-delta_x), y_min, x*(1+delta_x),  y_min );
                line->DrawLine( x*(1-delta_x), y_max, x*(1+delta_x),  y_max );
            } else {
                line->DrawLine( x-delta_x, y_min, x+delta_x,  y_min );
                line->DrawLine( x-delta_x, y_max, x+delta_x,  y_max );
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
void Draw::draw_point( TGraphErrors *graph, int flag )
{

    int symbol( graph->GetMarkerStyle() );
    int color( graph->GetMarkerColor() );
    for( int i=0; i<graph->GetN(); i++ )
    {
        double x, y;
        graph->GetPoint( i, x, y  );
        double x_err = graph->GetErrorX(i);
        double y_err = graph->GetErrorY(i);
        draw_point( x, y, x_err, y_err, symbol, color, flag );
    }
}

//__________________________________________________________
void Draw::draw_syst_bracket(
    double x, double y,
    double y_err,
    int color, int flag )
{
    // check error
    if( y_err <= 0 ) {
        std::cout << "Draw::draw_syst_bracket - invalid y error" << std::endl;
        return;
    }

    // canvas limits
    double uxmin = gPad->GetUxmin();
    double uxmax = gPad->GetUxmax();
    double uymin = gPad->GetUymin();
    double uymax = gPad->GetUymax();

    // bracket limits
    double delta_x = (flag&LOG_X) ? 0.07:0.015 * ( uxmax - uxmin );
    double delta_y = (flag&LOG_Y) ? 0.07:0.01 * ( uymax - uymin );

    // error limits
    double y_min = y-y_err;
    double y_max = y+y_err;

    TPolyLine* pline = new TPolyLine();
    pline->SetLineColor( color );
    pline->SetLineWidth( _line_width );

    static double px[4] = {0};
    static double py[4] = {0};

    // lower bracket
    bool draw_lower = (flag&LOG_Y) ? (log10(y_min) >= uymin):(y_min>=uymin);
    if( draw_lower ) {
        if( flag&LOG_Y )
        {
            py[0] = log10((1+delta_y)*(y_min));
            py[1] = log10(y_min);
            py[2] = log10(y_min);
            py[3] = log10((1+delta_y)*(y_min));
        } else {
            py[0] = y_min+delta_y;
            py[1] = y_min;
            py[2] = y_min;
            py[3] = y_min+delta_y;
        }

        if( flag&LOG_X )
        {
            px[0] = log10((1-delta_x)*x);
            px[1] = log10((1-delta_x)*x);
            px[2] = log10((1+delta_x)*x);
            px[3] = log10((1+delta_x)*x);
        } else {
            px[0] = x-delta_x;
            px[1] = x-delta_x;
            px[2] = x+delta_x;
            px[3] = x+delta_x;
        }
        pline->DrawPolyLine( 4, px, py);
    }

    // upper bracket
    bool draw_upper = (flag&LOG_Y) ? (log10(y_max) <= uymax):(y_max<=uymax);
    if( draw_upper ) {
        if( flag&LOG_Y )
        {
            py[0] = log10((1-delta_y)*(y+y_err));
            py[1] = log10(y+y_err);
            py[2] = log10(y+y_err);
            py[3] = log10((1-delta_y)*(y+y_err));
        } else {
            py[0] = y+y_err-delta_y;
            py[1] = y+y_err;
            py[2] = y+y_err;
            py[3] = y+y_err-delta_y;
        }
        if( flag&LOG_X )
        {
            px[0] = log10((1-delta_x)*x);
            px[1] = log10((1-delta_x)*x);
            px[2] = log10((1+delta_x)*x);
            px[3] = log10((1+delta_x)*x);
        } else {
            px[0] = x-delta_x;
            px[1] = x-delta_x;
            px[2] = x+delta_x;
            px[3] = x+delta_x;
        }
        pline->DrawPolyLine( 4, px, py);
    }

}

//__________________________________________________________
void Draw::draw_syst_bracket( TGraphErrors *graph, int flag )
{

    int color( graph->GetMarkerColor() );
    for( int i=0; i<graph->GetN(); i++ )
    {
        double x, y;
        graph->GetPoint( i, x, y  );
        double y_err = graph->GetErrorY(i);
        draw_syst_bracket( x, y, y_err, color, flag );
    }
}

//__________________________________________________________
void Draw::draw_syst_box(
    double x, double y,
    double y_err,
    int color, int flag )
{
    // check error
    if( y_err <= 0 ) {
        std::cout << "Draw::draw_syst_box - invalid y error" << std::endl;
        return;
    }

    // canvas limits
    double uxmin = gPad->GetUxmin();
    double uxmax = gPad->GetUxmax();
    double uymin = gPad->GetUymin();
    double uymax = gPad->GetUymax();

    // Relative flag
    if( (flag&RELATIVE) ) y_err = fabs( y*y_err );

    // error limits
    double y_min = std::max<double>( (flag&LOG_Y) ? exp( log(10.0)*uymin ):uymin , y-y_err );
    double y_max = std::min<double>( (flag&LOG_Y) ? exp( log(10.0)*uymax ):uymax , y+y_err );

    // bracket limits
    double delta_x = (flag&LOG_X) ? _delta_x_log:_delta_x_lin * ( uxmax - uxmin );

    // box limits
    double x0 = (flag&LOG_X) ?  log10((1-delta_x)*x):x-delta_x;
    double x1 = (flag&LOG_X) ?  log10((1+delta_x)*x):x+delta_x;
    double y0 = (flag&LOG_Y) ? log10(y_min):y_min;
    double y1 = (flag&LOG_Y) ? log10(y_max):y_max;

    TBox *box = new TBox(x0, y0, x1, y1 );
    color = Color(color).merge( 0, 0.4 );

    box->SetFillColor( color );
    box->SetLineColor( color );
    if( _box_fill_style ) box->SetFillStyle( _box_fill_style );
    box->Draw();
}

//__________________________________________________________
void Draw::draw_syst_box( TGraphErrors *graph, int flag )
{

    int color( graph->GetMarkerColor() );
    for( int i=0; i<graph->GetN(); i++ )
    {
        double x, y;
        graph->GetPoint( i, x, y  );
        double y_err = graph->GetErrorY(i);
        draw_syst_box( x, y, y_err, color, flag );
    }
}

//___________________________________________________________
void Draw::draw_syst_global(  TGraph* tg, double err_rel, int flag )
{

    const int n_points = tg->GetN();

    TPolyLine* pline = new TPolyLine(2*n_points+1);
    pline->SetLineColor( tg->GetLineColor() );
    pline->SetLineWidth( tg->GetLineWidth() );

    for( int i=0; i<n_points; i++ )
    {
        double x = 0;
        double y = 0;
        tg->GetPoint( i, x, y );
        double y_up =  y*(1+err_rel);
        double y_down =  y*(1-err_rel);

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
void Draw::draw_box(
    double x, double y,
    double delta_x, double delta_y,
    int color, int flag )
{
    printf( "global= %10g\n", delta_y );

    double x0 = (flag&LOG_X)? log10(x)+delta_x:x+delta_x;
    double x1 = (flag&LOG_X)? log10(x)-delta_x:x-delta_x;

    double y0 = (flag&LOG_Y)? log10(y+delta_y):y+delta_y;
    double y1 = (flag&LOG_Y)? log10(y-delta_y):y-delta_y;

    TBox *box = new TBox( x0, y0, x1, y1 );
    if( _box_fill_style ) box->SetFillStyle( _box_fill_style );
    box->SetFillColor( color );
    box->SetLineColor( color );
    box->Draw();

}
