include(FindPackageHandleStandardArgs)

if (WIN32)
	# Find include files
	find_path(
		ASSIMP_INCLUDE_DIR
		NAMES assimp/config.h
		PATHS
		$ENV{PROGRAMFILES}/include
		${ASSIMP_ROOT_DIR}/include
		DOC "The directory where GL/glew.h resides")

	# Use glew32s.lib for static library
	# Define additional compiler definitions
	if (ASSIMP_USE_STATIC_LIBS)
		set(ASSIMP_LIBRARY_NAME glew32s)
		set(ASSIMP_DEFINITIONS -DASSIMP_STATIC)
	else()
		set(ASSIMP_LIBRARY_NAME glew32)
	endif()

	# Find library files
	find_library(
		ASSIMP_LIBRARY
		NAMES ${ASSIMP_LIBRARY_NAME}
		PATHS
		$ENV{PROGRAMFILES}/ASSIMP/lib
		${ASSIMP_ROOT_DIR}/lib
		DOC "The ASSIMP library")

	unset(ASSIMP_LIBRARY_NAME)
else()
	# Find include files
	find_path(
		ASSIMP_INCLUDE_DIR
		NAMES assimp/config.h
		PATHS
		/usr/include
		/usr/local/include
		/sw/include
		/opt/local/include
		${ASSIMP_ROOT_DIR}/include
		DOC "The directory where GL/glew.h resides")

	# Find library files
	# Try to use static libraries
	find_library(
		ASSIMP_LIBRARY
		NAMES assimp ASSIMP
		PATHS
		/usr/lib64
		/usr/lib
		/usr/local/lib64
		/usr/local/lib
		/sw/lib
		/opt/local/lib
		${ASSIMP_ROOT_DIR}/lib
		DOC "The ASSIMP library")
endif()

# Handle REQUIRD argument, define *_FOUND variable
find_package_handle_standard_args(ASSIMP DEFAULT_MSG ASSIMP_INCLUDE_DIR ASSIMP_LIBRARY)

# Define ASSIMP_LIBRARIES and ASSIMP_INCLUDE_DIRS
if (ASSIMP_FOUND)
	set(ASSIMP_LIBRARIES ${ASSIMP_LIBRARY})
	set(ASSIMP_INCLUDE_DIRS ${ASSIMP_INCLUDE_DIR})
endif()

# Hide some variables
mark_as_advanced(ASSIMP_INCLUDE_DIR ASSIMP_LIBRARY)
