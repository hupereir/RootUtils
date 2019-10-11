cmake_minimum_required(VERSION 2.8.4 FATAL_ERROR)
project(RootUtil CXX)

######################
# module path
set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/CMake")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")

######
# GSL
include( FindPkgConfig )
pkg_check_modules( GSL REQUIRED gsl )
link_directories( ${GSL_LIBRARY_DIRS} )
include_directories( ${GSL_INCLUDE_DIRS} )

######
# ROOT
find_package(ROOT REQUIRED)

######################
# include directories
include_directories(SYSTEM ${PROJECT_SOURCE_DIR} ${ROOT_INCLUDE_DIR} )

######################
# base
set( libbase_SOURCES
  ChisquareFitter.cxx
  Color.cxx
  Debug.cxx
  Draw.cxx
  FileManager.cxx
  FitUtils.cxx
  Grid.cxx
  LikelihoodFitter.cxx
  PdfDocument.cxx
  Stream.cxx
  Table.cxx
  TH2Fit.cxx
  Utils.cxx
 )


add_root_dictionaries( libbase_SOURCES
  ROOT_MACROLinkDef.h
  ColorLinkDef.h
  DebugLinkDef.h
  DrawLinkDef.h
  FileManagerLinkDef.h
  FitUtilsLinkDef.h
  GridLinkDef.h
  PdfDocumentLinkDef.h
  StreamLinkDef.h
  TableLinkDef.h
  TH2FitLinkDef.h
  UtilsLinkDef.h
)

add_library( RootUtilBase SHARED ${libbase_SOURCES} )
target_link_libraries( RootUtilBase ${ROOT_LIBRARIES} ${GSL_LIBRARIES})
install( TARGETS RootUtilBase DESTINATION ${CMAKE_INSTALL_PREFIX}/lib )

######################
# ranlib
set( librandlib_SOURCES RandLib.cxx )

add_root_dictionaries( librandlib_SOURCES
  RandLibLinkDef.h
)

add_library( RandLib SHARED ${librandlib_SOURCES} )
target_link_libraries( RandLib ${ROOT_LIBRARIES})

### install
install( TARGETS RandLib DESTINATION ${CMAKE_INSTALL_PREFIX}/lib )
