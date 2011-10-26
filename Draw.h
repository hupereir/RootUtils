// $Id: Draw.h,v 1.8 2011/07/01 15:15:05 hpereira Exp $
#ifndef Draw_h
#define Draw_h

#include <TCanvas.h>
#include <TH1.h>
#include <TGraph.h>
#include <TGraphErrors.h>
#include <TMarker.h>
#include <TPaveStats.h>
#include <TLatex.h>

/*!
  \file Draw.h
  \brief some root utilities to handle histograms safely
  \author	Hugo Pereira
  \version $Revision: 1.8 $
  \date $Date: 2011/07/01 15:15:05 $
*/

/*!
  \class Draw
  \brief some graphical root utilities
*/
class Draw:public TObject {

  public:

  //! constructor (doing nothing, needed for root)
  Draw():TObject()
  {}

  //! log_scale enumeration
  enum {
    LOG_X=1<<0,
    LOG_Y=1<<1,
    RELATIVE=1<<2
  };

  //! direction enumeration for moving TPaveStats
  enum direction { NONE=0, LEFT=1, RIGHT=2, UP=4, DOWN=8 };

  //! dimension enumeration for resizing TPaveStats
  enum dimension { WIDTH=1, HEIGHT=2 };

  //! options for TPaveStatUpdate
  enum option { TRANSPARENT=1 };

  //! flag for axis text size modification
  enum axis_flag {
    LABEL = 1<<0,
    TITLE = 1<<1,
    X = 1<<2,
    Y = 1<<3,
    ALL = LABEL|TITLE|X|Y
  };


  // divide canvas following dimensions ratio to have at least N panels
  static void divide_canvas( TCanvas*, int );

  //! add text
  static TLatex* put_text( const double& x_ndc, const double& y_ndc, const int& color, const char* text );

  //! apply scale factor to TPaveStats object. Both to fonts and size
  static TPaveStats* resize_stats( double scale=1, int dimension = WIDTH | HEIGHT  );

  //! display latest drawn histogram PaveStat together with original PaveStats
  static TPaveStats* update_stats(
    int direction = NONE,
    int color = 0,
    const char* new_name = 0,
    int scale = 1,
    int option = 0 );

  //! update TPaveText size according to it's number of lines
  static void update_pave_size(
    TPaveText *text,
    int direction = DOWN );

  //! modify histogram axis sizes using flags and new size
  static void resize_axis( TH1* h, double size, int flag = ALL, bool update = false );

  //! small bars at statistical errors limits
  static void set_draw_err_limit( bool value )
  { _draw_err_limit = value; }

  //! box width (linear x scale)
  static void set_delta_x_lin( const double& value )
  { _delta_x_lin = value; }

  //! box width (logarythmic x scale)
  static void set_delta_x_log( const double& value )
  { _delta_x_log = value; }

  //! line width
  static void set_line_width( int value )
  { _line_width = value; }

  //! box fill style
  static void set_box_fill_style( int value )
  { _box_fill_style = value; }

  //! systematic error box shade
  static void set_syst_shade( double value )
  { _syst_shade = value; }

  //! draw symbol and error bar at given point
  static TMarker* draw_point(
      double x, double y,
      double x_err, double y_err,
      int symbol = 20, int color = 1, int flag = 0 );

  //! draw symbol and error bar at TGraph points
  static void draw_point( TGraphErrors *graph, int flag = 0 );

  //! draw systematic error at given point, using brackets
  static void draw_syst_bracket(
      double x, double y,
      double y_err,
      int color = 1, int flag = 0 );

  //! draw systematic error at TGraph points, using brackets
  static void draw_syst_bracket( TGraphErrors* graph, int flag = 0 );

  //! draw systematic error at given point, using box
  static void draw_syst_box( double x, double y, double y_err, int color = 1, int flag = 0 );

  //! draw systematic error at given point, using box
  static void draw_syst_box( int size, double* x, double* y, double* y_err, int color = 1, int flag = 0 )
  { for( int i=0; i<size; i++ ) draw_syst_box( x[i], y[i], y_err[i], color, flag ); }

  //! draw shaded area on TGraph point using X errors for limits
  static void draw_syst_box( TGraphErrors* graph, int flag = 0 );

  //! draw contour around TGraph error bars
  void draw_syst_global( TGraph* tg, double err_rel, int flag = 0 );

  //! draw shaded box around point using given color and style
  static void draw_box(
      double x, double y,
      double delta_x, double delta_y,
      int color = 1 , int flag = 0 );

  private:

  //! if true, draw small bars at statistical errors limits
  static bool _draw_err_limit;

  //! default line width
  static int _line_width;

  //! box fill style
  static int _box_fill_style;

  //! boxes width
  static double _delta_x_log;

  //! boxes height
  static double _delta_x_lin;

  //! systematic error shade
  static double _syst_shade;

  ClassDef(Draw,0)

};

#endif
