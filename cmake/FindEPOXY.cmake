include(FindPackageHandleStandardArgs)

if (WIN32)
	# Find include files
	find_path(
		EPOXY_INCLUDE_DIR
		NAMES epoxy/gl.h
		PATHS
		$ENV{PROGRAMFILES}/include
		${ASSIMP_ROOT_DIR}/include
		)

	# Find library files
	find_library(
		EPOXY_LIBRARY
		NAMES ${EPOXY_LIBRARY_NAME} libepoxy
		PATHS
		$ENV{PROGRAMFILES}/epoxy/lib
		${EPOXY_ROOT_DIR}/lib
		DOC "The Epoxy library")

	unset(EPOXY_LIBRARY_NAME)
else()
	# Find include files
	find_path(
		EPOXY_INCLUDE_DIR
		NAMES assimp/config.h
		PATHS
		/usr/include
		/usr/local/include
		/sw/include
		/opt/local/include
		${EPOXY_ROOT_DIR}/include
		)

	# Find library files
	# Try to use static libraries
	find_library(
		EPOXY_LIBRARY
		NAMES epoxy EPOXY libepoxy.dylib
		PATHS
		/usr/lib64
		/usr/lib
		/usr/local/lib64
		/usr/local/lib
		/sw/lib
		/opt/local/lib
		${EPOXY_ROOT_DIR}/lib
		DOC "The Epoxy library")
endif()

# Handle REQUIRD argument, define *_FOUND variable
find_package_handle_standard_args(EPOXY DEFAULT_MSG EPOXY_INCLUDE_DIR EPOXY_LIBRARY)

# Define ASSIMP_LIBRARIES and ASSIMP_INCLUDE_DIRS
if (EPOXY_FOUND)
	set(EPOXY_LIBRARIES ${EPOXY_LIBRARY})
	set(EPOXY_INCLUDE_DIRS ${EPOXY_INCLUDE_DIR})
endif()

# Hide some variables
mark_as_advanced(EPOXY_INCLUDE_DIR EPOXY_LIBRARY)
