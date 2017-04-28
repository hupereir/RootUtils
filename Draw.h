// $Id: Draw.h,v 1.8 2011/07/01 15:15:05 hpereira Exp $
#ifndef Draw_h
#define Draw_h

#include <TCanvas.h>
#include <TH1.h>
#include <TGraph.h>
#include <TGraphErrors.h>
#include <TLine.h>
#include <TMarker.h>
#include <TPaveStats.h>
#include <TLatex.h>
#include <TString.h>

class Draw:public TObject
{

  public:

  //* constructor (doing nothing, needed for root)
  Draw():TObject()
  {}

  //* log_scale enumeration
  enum {
    LOG_X=1<<0,
    LOG_Y=1<<1,
    RELATIVE=1<<2
  };

  //* direction enumeration for moving TPaveStats
  enum direction { NONE=0, LEFT=1, RIGHT=2, UP=4, DOWN=8 };

  //* dimension enumeration for resizing TPaveStats
  enum dimension { WIDTH=1, HEIGHT=2 };

  //* options for TPaveStatUpdate
  enum option { TRANSPARENT=1 };

  //* flag for axis text size modification
  enum AxisFlag
  {
    LABEL = 1<<0,
    TITLE = 1<<1,
    X = 1<<2,
    Y = 1<<3,
    ALL = LABEL|TITLE|X|Y
  };

  // file format
  enum FileFormat
  {
    FormatPNG = 1,
    FormatPDF = 1<<1,
    FormatEPS = 1<<2,
    FormatJPG = 1<<3,
    FormatGIF = 1<<4
  };

  // divide canvas following dimensions ratio to have at least N panels
  static void SetStyle( Bool_t );

  // save canvas using multiple formats
  static void SaveCanvas( TCanvas*, TString, Int_t = FormatPDF|FormatEPS );

  // divide canvas following dimensions ratio to have at least N panels
  static void DivideCanvas( TCanvas*, int, Bool_t respectRatio = true );

  //* add text
  static TLatex* PutText( Double_t x_ndc, Double_t y_ndc, Int_t color, TString text, Double_t fontSize = 0.04 );
  static TLatex* PutText( Double_t x_ndc, Double_t y_ndc, TString text, Double_t fontSize = 0.04 )
  { return PutText( x_ndc, y_ndc, 1, text, fontSize ); }

  //* display latest drawn histogram PaveStat together with original PaveStats
  static TPaveStats* UpdateStats(
    Int_t direction = NONE,
    Int_t color = 0,
    TString newName = TString(),
    Int_t scale = 1,
    Int_t option = 0 );

  //* display latest drawn histogram PaveStat together with original PaveStats
  static void UpdatePave(
    TPaveText*,
    Int_t direction = DOWN,
    Int_t color = 1,
    Int_t scale = 1,
    Int_t option = TRANSPARENT );

  //* update TPaveText size according to it's number of lines
  static void UpdatePaveSize( TPaveText*, Int_t direction = DOWN );

  //* modify histogram axis sizes using flags and new size
  static void ResizeAxis( TH1* h, Double_t size, Int_t flag = ALL, Bool_t update = false );

  //* small bars at statistical errors limits
  static void SetDrawErrLimit( Bool_t value )
  { fDrawErrLimit = value; }

  //* box width (linear x scale)
  static void SetDeltaXLin( Double_t value )
  { fDeltaXLin = value; }

  //* box width (logarythmic x scale)
  static void SetDeltaXLog( Double_t value )
  { fDeltaXLog = value; }

  //* line color
  static void SetLineColor( Int_t value )
  { fLineColor = value; }

  //* line width
  static void SetLineWidth( Int_t value )
  { fLineWidth = value; }

  //* box fill style
  static void SetBoxFillStyle( Int_t value )
  { fBoxFillStyle = value; }

  //* systematic error box shade
  static void SetSystShade( Double_t value )
  { fSystShade = value; }

  //* draw symbol and error bar at given point
  static TMarker* DrawMarker(
    Double_t x, Double_t y,
    Double_t x_err, Double_t y_err,
    Int_t symbol = 20, Int_t color = 1, Int_t flag = 0 );

  //* draw symbol and error bar at TGraph points
  static void DrawMarkers( TGraphErrors *graph, Int_t flag = 0 );

  //* draw systematic error at given point, using brackets
  static void DrawSystBracket(
    Double_t x, Double_t y,
    Double_t y_err,
    Int_t color = 1, Int_t flag = 0 );

  //* draw systematic error at TGraph points, using brackets
  static void DrawSystBracket( TGraphErrors* graph, Int_t flag = 0 );

  //* draw systematic error at given point, using box
  static void DrawSystBox( Double_t x, Double_t y, Double_t y_err, Int_t color = 1, Int_t flag = 0 );

  //* draw systematic error at given point, using box
  static void DrawSystBox( Int_t size, Double_t* x, Double_t* y, Double_t* y_err, Int_t color = 1, Int_t flag = 0 )
  { for( Int_t i=0; i<size; i++ ) DrawSystBox( x[i], y[i], y_err[i], color, flag ); }

  //* draw shaded area on TGraph poInt_t using X errors for limits
  static void DrawSystBox( TGraphErrors* graph, Int_t flag = 0 );

  //* draw contour around TGraph error bars
  void DrawSystGlobal( TGraph* tg, Double_t err_rel, Int_t flag = 0 );

  //* draw shaded box around poInt_t using given color and style
  static void DrawBox(
    Double_t x, Double_t y,
    Double_t delta_x, Double_t delta_y,
    Int_t color = 1 , Int_t flag = 0 );

  //* draw markers
  static TGraphErrors* DrawMarkers(
    Int_t n,
    Double_t* x, Double_t* y,
    Double_t* xErr, Double_t* yErr,
    Int_t symbol = 20, Int_t color = 1 );


  //* draw markers
  static TGraphErrors* DrawMarkers(
    Int_t n,
    Double_t* x, Double_t* y,
    Double_t* yErr,
    Int_t symbol = 20, Int_t color = 1 );

  //* draw systematic boxes
  static TGraphErrors* DrawSystematics(
    Int_t n,
    Double_t* x, Double_t* y, Double_t* yErr,
    Int_t color = 1 );

  //* draw error band (for theory)
  static TGraphErrors* DrawBand(
    Int_t n,
    Double_t* x, Double_t* yLow, Double_t* yHigh,
    Int_t color = 1 );

  //* vertical line
  static TLine* VerticalLine( TVirtualPad*, Double_t );

  //* vertical line
  static TLine* HorizontalLine( TVirtualPad*, Double_t );

  private:

  //* if true, draw small bars at statistical errors limits
  static Bool_t fDrawErrLimit;

  //* default line color
  static Int_t fLineColor;

  //* default line width
  static Int_t fLineWidth;

  //* box fill style
  static Int_t fBoxFillStyle;

  //* boxes width
  static Double_t fDeltaXLog;

  //* boxes width
  static Double_t fDeltaXLin;

  //* systematic error shade
  static Double_t fSystShade;

  //* marker style
  static Double_t fMarkerSize;

  //* font
  static Int_t fFont;

  ClassDef(Draw,0)

};

#endif
