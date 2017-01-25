#ifndef Stream_h
#define Stream_h

// $Id: Streamer.h,v 1.47 2010/09/15 02:27:25 hpereira Exp $

#include <TObject.h>
#include <TString.h>

class Grid:public TObject
{

  public:

  /// check whether a file exists
  static Bool_t FileExists( TString, TString = TString() );

  /// check whether a file exists
  static Bool_t IsDirectory( TString, TString = TString() );

  /// remove a file if exists
  static Bool_t RemoveFile( TString, TString = TString() );

  /// copy local file remotely
  static Bool_t CopyFile( TString, TString = TString() );

  ClassDef(Grid,0)

};

#endif
