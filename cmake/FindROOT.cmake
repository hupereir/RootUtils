# - Find ROOT instalation
# This module tries to find the ROOT installation on your system.
# It tries to find the root-config script which gives you all the needed information.
# If the system variable ROOTSYS is set this is straight forward.
# If not the module uses the pathes given in ROOT_CONFIG_SEARCHPATH.
# If you need an other path you should add this path to this varaible.
# The root-config script is then used to detect basically everything else.
# This module defines a number of key variables and macros.

message(STATUS "Looking for Root...")

set(ROOT_CONFIG_SEARCHPATH
	${SIMPATH}/tools/root/bin
	$ENV{ROOTSYS}/bin
)

set(ROOT_DEFINITIONS "")

set(ROOT_INSTALLED_VERSION_TOO_OLD FALSE)

set(ROOT_CONFIG_EXECUTABLE ROOT_CONFIG_EXECUTABLE-NOTFOUND)

find_program(ROOT_CONFIG_EXECUTABLE NAMES root-config PATHS
	 ${ROOT_CONFIG_SEARCHPATH}
	 NO_DEFAULT_PATH)

if(${ROOT_CONFIG_EXECUTABLE} MATCHES "ROOT_CONFIG_EXECUTABLE-NOTFOUND")
	message( FATAL_ERROR "ROOT not installed in the searchpath and ROOTSYS is not set. Please
 set ROOTSYS or add the path to your ROOT installation in the Macro FindROOT.cmake in the
 subdirectory cmake/modules.")
else()
	string(REGEX REPLACE "(^.*)/bin/root-config" "\\1" test ${ROOT_CONFIG_EXECUTABLE})
	set( ENV{ROOTSYS} ${test})
	set( ROOTSYS ${test})
endif()

if(ROOT_CONFIG_EXECUTABLE)

	set(ROOT_FOUND FALSE)

	exec_program(${ROOT_CONFIG_EXECUTABLE} ARGS "--version" OUTPUT_VARIABLE ROOTVERSION)

	message(STATUS "Looking for Root... - found $ENV{ROOTSYS}/bin/root")
	message(STATUS "Looking for Root... - version ${ROOTVERSION} ")

	# we need at least version 5.00/00
	if(NOT ROOT_MIN_VERSION)
		set(ROOT_MIN_VERSION "5.00/00")
	endif()

	# now parse the parts of the user given version string into variables
	string(REGEX REPLACE "^([0-9]+)\\.[0-9][0-9]+\\/[0-9][0-9]+" "\\1" req_root_major_vers "${ROOT_MIN_VERSION}")
	string(REGEX REPLACE "^[0-9]+\\.([0-9][0-9])+\\/[0-9][0-9]+.*" "\\1" req_root_minor_vers "${ROOT_MIN_VERSION}")
	string(REGEX REPLACE "^[0-9]+\\.[0-9][0-9]+\\/([0-9][0-9]+)" "\\1" req_root_patch_vers "${ROOT_MIN_VERSION}")

	# and now the version string given by qmake
	string(REGEX REPLACE "^([0-9]+)\\.[0-9][0-9]+\\/[0-9][0-9]+.*" "\\1" found_root_major_vers "${ROOTVERSION}")
	string(REGEX REPLACE "^[0-9]+\\.([0-9][0-9])+\\/[0-9][0-9]+.*" "\\1" found_root_minor_vers "${ROOTVERSION}")
	string(REGEX REPLACE "^[0-9]+\\.[0-9][0-9]+\\/([0-9][0-9]+).*" "\\1" found_root_patch_vers "${ROOTVERSION}")

	if(found_root_major_vers LESS 5)
		message( FATAL_ERROR "Invalid ROOT version \"${ROOTERSION}\", at least major version 4 is required, e.g. \"5.00/00\"")
	endif()

	# compute an overall version number which can be compared at once
	math(EXPR req_vers "${req_root_major_vers}*10000 + ${req_root_minor_vers}*100 + ${req_root_patch_vers}")
	math(EXPR found_vers "${found_root_major_vers}*10000 + ${found_root_minor_vers}*100 + ${found_root_patch_vers}")

	if(found_vers LESS req_vers)
		set(ROOT_FOUND FALSE)
		set(ROOT_INSTALLED_VERSION_TOO_OLD TRUE)
	else()
		set(ROOT_FOUND TRUE)
	endif()

endif()


if(ROOT_FOUND)

	# ask root-config for the library dir
	# Set ROOT_LIBRARY_DIR

	exec_program( ${ROOT_CONFIG_EXECUTABLE}
		ARGS "--libdir"
		OUTPUT_VARIABLE ROOT_LIBRARY_DIR_TMP )

	if(EXISTS "${ROOT_LIBRARY_DIR_TMP}")
		set(ROOT_LIBRARY_DIR ${ROOT_LIBRARY_DIR_TMP} )
	else()
		message("Warning: ROOT_CONFIG_EXECUTABLE reported ${ROOT_LIBRARY_DIR_TMP} as library path,")
		message("Warning: but ${ROOT_LIBRARY_DIR_TMP} does NOT exist, ROOT must NOT be installed correctly.")
	endif()

	# ask root-config for the binary dir
	exec_program(${ROOT_CONFIG_EXECUTABLE}
		ARGS "--bindir"
		OUTPUT_VARIABLE root_bins )
	set(ROOT_BINARY_DIR ${root_bins})

	# ask root-config for the include dir
	exec_program( ${ROOT_CONFIG_EXECUTABLE}
		ARGS "--incdir"
		OUTPUT_VARIABLE root_headers )
	set(ROOT_INCLUDE_DIR ${root_headers})

	# ask root-config for the library varaibles
	exec_program( ${ROOT_CONFIG_EXECUTABLE}
		ARGS "--glibs"
		OUTPUT_VARIABLE root_flags )

	set(ROOT_LIBRARIES ${root_flags})

	# Make variables changeble to the advanced user
	mark_as_advanced( ROOT_LIBRARY_DIR ROOT_INCLUDE_DIR ROOT_DEFINITIONS)

	# Set ROOT_INCLUDES
	set( ROOT_INCLUDES ${ROOT_INCLUDE_DIR})

	set(LD_LIBRARY_PATH ${LD_LIBRARY_PATH} ${ROOT_LIBRARY_DIR})

	#######################################
	#
	#       Check the executables of ROOT
	#          ( rootcint )
	#
	#######################################

	find_program(ROOT_CINT_EXECUTABLE
		NAMES rootcint
		PATHS ${ROOT_BINARY_DIR}
		NO_DEFAULT_PATH
		)

endif()

###########################################
#
#       Macros for building ROOT dictionary
#
###########################################
macro(ADD_ROOT_DICTIONARIES _sources )

	get_property(INC_DIRS DIRECTORY PROPERTY INCLUDE_DIRECTORIES)

	foreach (_current_FILE ${ARGN})

		# generate associated header
		string( REPLACE "LinkDef" "" _current_HEADER "${_current_FILE}")
		string( REPLACE "LinkDef.h" "Dict.cxx" _current_DICT "${_current_FILE}")

  # generate dictionary
		root_generate_dictionary( "${_current_HEADER}" "${_current_FILE}" "${_current_DICT}" "${INC_DIRS}" )

		# append generated sources
		list(APPEND ${_sources} ${_current_DICT})

	endforeach()

endmacro()

###########################################
#
#       Macros for building ROOT dictionary
#
###########################################
macro(ROOT_GENERATE_DICTIONARY INFILES LINKDEF_FILE OUTFILE INCLUDE_DIRS_IN)

	set(INCLUDE_DIRS)

	foreach(_current_FILE ${INCLUDE_DIRS_IN})
		set(INCLUDE_DIRS ${INCLUDE_DIRS} -I${_current_FILE})
	endforeach()

	string(REGEX REPLACE "^(.*)\\.(.*)$" "\\1.h" bla "${OUTFILE}")
	set(OUTFILES ${OUTFILE} ${bla})

	if(CMAKE_SYSTEM_NAME MATCHES Linux)

	add_custom_command(OUTPUT ${OUTFILES}
			 COMMAND LD_LIBRARY_PATH=${ROOT_LIBRARY_DIR} ROOTSYS=${ROOTSYS} ${ROOT_CINT_EXECUTABLE}
			 ARGS -f ${OUTFILE} -c -DHAVE_CONFIG_H ${INCLUDE_DIRS} ${INFILES} ${LINKDEF_FILE} DEPENDS ${INFILES} ${LINKDEF_FILE})

	else()

		if(CMAKE_SYSTEM_NAME MATCHES Darwin)

			add_custom_command(OUTPUT ${OUTFILES}
			 COMMAND DYLD_LIBRARY_PATH=${ROOT_LIBRARY_DIR} ROOTSYS=${ROOTSYS} ${ROOT_CINT_EXECUTABLE}
			 ARGS -f ${OUTFILE} -c -DHAVE_CONFIG_H ${INCLUDE_DIRS} ${INFILES} ${LINKDEF_FILE} DEPENDS ${INFILES} ${LINKDEF_FILE})

  endif()
	endif()

endmacro()
