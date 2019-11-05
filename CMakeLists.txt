cmake_minimum_required(VERSION 2.8.4 FATAL_ERROR)
project(RootUtil CXX)

######################
# module path
set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/CMake")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")

######
# GSL
include( FindPkgConfig )
pkg_check_modules( GSL gsl )

if( ${GSL_FOUND} )
  link_directories( ${GSL_LIBRARY_DIRS} )
  include_directories( ${GSL_INCLUDE_DIRS} )
  add_definitions(-DWITH_GSL=1)
else()
  add_definitions(-DWITH_GSL=0)
endif()

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


######################
# base
set( libbase_HEADERS
  ROOT_MACRO.h
  ChisquareFitter.h
  Color.h
  Debug.h
  Draw.h
  FileManager.h
  FitUtils.h
  Grid.h
  LikelihoodFitter.h
  PdfDocument.h
  Projection.h
  Stream.h
  Table.h
  TH2Fit.h
  Utils.h
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
install( FILES ${libbase_HEADERS} DESTINATION ${CMAKE_INSTALL_PREFIX}/include/RootUtil )

######################
# ranlib
set( librandlib_SOURCES RandLib.cxx )
set( librandlib_HEADERS RandLib.h )

add_root_dictionaries( librandlib_SOURCES
  RandLibLinkDef.h
)

add_library( RandLib SHARED ${librandlib_SOURCES} )
target_link_libraries( RandLib ${ROOT_LIBRARIES})

### install
install( TARGETS RandLib DESTINATION ${CMAKE_INSTALL_PREFIX}/lib )
install( FILES ${librandlib_HEADERS} DESTINATION ${CMAKE_INSTALL_PREFIX}/include/RandLib )
