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
  enum AxisFlag {
    LABEL = 1<<0,
    TITLE = 1<<1,
    X = 1<<2,
    Y = 1<<3,
    ALL = LABEL|TITLE|X|Y
  };


  // divide canvas following dimensions ratio to have at least N panels
  static void DivideCanvas( TCanvas*, int, bool respectRatio = true );

  //! add text
  static TLatex* PutText( const double& x_ndc, const double& y_ndc, const int& color, const char* text );

  //! apply scale factor to TPaveStats object. Both to fonts and size
  static TPaveStats* ResizeStats( double scale=1, int dimension = WIDTH | HEIGHT  );

  //! display latest drawn histogram PaveStat together with original PaveStats
  static TPaveStats* UpdateStats(
    int direction = NONE,
    int color = 0,
    const char* new_name = 0,
    int scale = 1,
    int option = 0 );

  //! update TPaveText size according to it's number of lines
  static void UpdatePaveSize(
    TPaveText *text,
    int direction = DOWN );

  //! modify histogram axis sizes using flags and new size
  static void ResizeAxis( TH1* h, double size, int flag = ALL, bool update = false );

  //! small bars at statistical errors limits
  static void SetDrawErrLimit( bool value )
  { fDrawErrLimit = value; }

  //! box width (linear x scale)
  static void SetDeltaXLin( const double& value )
  { fDeltaXLin = value; }

  //! box width (logarythmic x scale)
  static void SetDeltaXLog( const double& value )
  { fDeltaXLog = value; }

  //! line width
  static void SetLineWidth( int value )
  { fLineWidth = value; }

  //! box fill style
  static void SetBoxFillStyle( int value )
  { fBoxFillStyle = value; }

  //! systematic error box shade
  static void SetSystShade( double value )
  { fSystShade = value; }

  //! draw symbol and error bar at given point
  static TMarker* DrawPoint(
      double x, double y,
      double x_err, double y_err,
      int symbol = 20, int color = 1, int flag = 0 );

  //! draw symbol and error bar at TGraph points
  static void DrawPoint( TGraphErrors *graph, int flag = 0 );

  //! draw systematic error at given point, using brackets
  static void DrawSystBracket(
      double x, double y,
      double y_err,
      int color = 1, int flag = 0 );

  //! draw systematic error at TGraph points, using brackets
  static void DrawSystBracket( TGraphErrors* graph, int flag = 0 );

  //! draw systematic error at given point, using box
  static void DrawSystBox( double x, double y, double y_err, int color = 1, int flag = 0 );

  //! draw systematic error at given point, using box
  static void DrawSystBox( int size, double* x, double* y, double* y_err, int color = 1, int flag = 0 )
  { for( int i=0; i<size; i++ ) DrawSystBox( x[i], y[i], y_err[i], color, flag ); }

  //! draw shaded area on TGraph point using X errors for limits
  static void DrawSystBox( TGraphErrors* graph, int flag = 0 );

  //! draw contour around TGraph error bars
  void DrawSystGlobal( TGraph* tg, double err_rel, int flag = 0 );

  //! draw shaded box around point using given color and style
  static void DrawBox(
      double x, double y,
      double delta_x, double delta_y,
      int color = 1 , int flag = 0 );

  private:

  //! if true, draw small bars at statistical errors limits
  static bool fDrawErrLimit;

  //! default line width
  static int fLineWidth;

  //! box fill style
  static int fBoxFillStyle;

  //! boxes width
  static double fDeltaXLog;

  //! boxes height
  static double fDeltaXLin;

  //! systematic error shade
  static double fSystShade;

  ClassDef(Draw,0)

};

#endif
