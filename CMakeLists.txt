cmake_minimum_required(VERSION 2.8.4 FATAL_ERROR)
project(AliUtil CXX)

######################
# module path
set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake")

######
# ROOT
find_package(ROOT REQUIRED)

######################
# include directories
include_directories(SYSTEM ${PROJECT_SOURCE_DIR} ${ROOT_INCLUDE_DIR})

############
# postscript

set( libpostscript_SOURCES
  Postscript.cxx
)

add_root_dictionaries( libpostscript_SOURCES
  PostscriptLinkDef.h
)

add_library( AliUtilPostscript SHARED ${libpostscript_SOURCES} )
target_link_libraries( AliUtilPostscript ${ROOT_LIBRARIES})

############
# base

set( libbase_SOURCES
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


add_root_dictionaries( libbase_SOURCES
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

add_library( AliUtilBase SHARED ${libbase_SOURCES} )
target_link_libraries( AliUtilBase ${ROOT_LIBRARIES})

### install
install( TARGETS AliUtilPostscript AliUtilBase DESTINATION ${CMAKE_install_PREFIX}/lib )
