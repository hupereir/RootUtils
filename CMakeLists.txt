cmake_minimum_required(VERSION 2.8.4 FATAL_ERROR)
project(AliUtil CXX)

######################
# module path
SET(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake")

######
# ROOT
FIND_PACKAGE(ROOT REQUIRED)

######################
# include directories
INCLUDE_DIRECTORIES(SYSTEM ${PROJECT_SOURCE_DIR} ${ROOT_INCLUDE_DIR})

############
# postscript

SET( libpostscript_SOURCES
  Postscript.cxx
)

ADD_ROOT_DICTIONARIES( libpostscript_SOURCES
  PostscriptLinkDef.h
)

ADD_LIBRARY( AliUtilPostscript SHARED ${libpostscript_SOURCES} )
TARGET_LINK_LIBRARIES( AliUtilPostscript ${ROOT_LIBRARIES})

############
# base

SET( libbase_SOURCES
  Color.cxx
  Debug.cxx
  Draw.cxx
  FileManager.cxx
  FitUtils.cxx
  Stream.cxx
  Table.cxx
  TH2Fit.cxx
  Utils.cxx
 )


ADD_ROOT_DICTIONARIES( libbase_SOURCES
  ALI_MACROLinkDef.h
  ColorLinkDef.h
  DebugLinkDef.h
  DrawLinkDef.h
  FileManagerLinkDef.h
  FitUtilsLinkDef.h
  StreamLinkDef.h
  TableLinkDef.h
  TH2FitLinkDef.h
  UtilsLinkDef.h
)

ADD_LIBRARY( AliUtilBase SHARED ${libbase_SOURCES} )
TARGET_LINK_LIBRARIES( AliUtilBase ${ROOT_LIBRARIES})

### install
INSTALL( TARGETS AliUtilPostscript AliUtilBase DESTINATION ${CMAKE_INSTALL_PREFIX}/lib )
