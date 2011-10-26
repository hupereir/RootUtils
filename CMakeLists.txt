cmake_minimum_required(VERSION 2.8.4 FATAL_ERROR)
project(AliUtil CXX)

######################
# environment variables
set(ALICE_ROOT $ENV{ALICE_ROOT})
set(ALICE $ENV{ALICE})

######################
# module path
set(CMAKE_MODULE_PATH "${ALICE_ROOT}/cmake")

######################
# alice specific macro
include(ALICE_CMake)

######
# ROOT
find_package(ROOT REQUIRED)

######################
# include directories
include_directories(SYSTEM ${ROOTINCDIR})

############
# postscript
set( libpostscript_SOURCES Postscript.cxx )

add_library( postscript SHARED ${libpostscript_SOURCES} )

############
# base
set( libbase_SOURCES
  Color.cxx
  Debug.cxx
  Draw.cxx
  FileManager.cxx
  FitUtils.cxx
  Table.cxx
  TH2Fit.cxx
  Utils.cxx )

add_library( base SHARED ${libbase_SOURCES} )
