# Additional modules
include(FindPackageHandleStandardArgs)

if (WIN32)
	# Find include files
	find_path(
		SOIL_INCLUDE_DIR
		NAMES SOIL.h
		PATHS
		$ENV{PROGRAMFILES}/include
		${GLEW_ROOT_DIR}/include
		DOC "The directory where SOIL.h resides")

	# Find library files
	find_library(
		SOIL_LIBRARY
		NAMES libSOIL
		PATHS
		$ENV{PROGRAMFILES}/lib
		${GLEW_ROOT_DIR}/lib)

	unset(SOIL_LIBRARY_NAME)
else()
	# Find include files
	find_path(
		SOIL_INCLUDE_DIR
		NAMES SOIL.h 
		PATHS
		/usr/include
		/usr/local/include
		/sw/include
		/opt/local/include
		DOC "The directory where SOIL.h resides")

	# Find library files
	# Try to use static libraries
	find_library(
		SOIL_LIBRARY
		NAMES libSOIL.a
		PATHS
		/usr/lib64
		/usr/lib
		/usr/local/lib64
		/usr/local/lib
		/sw/lib
		/opt/local/lib
		${SOIL_ROOT_DIR}/lib
		DOC "The SOIL library")
endif()

# Handle REQUIRD argument, define *_FOUND variable
find_package_handle_standard_args(SOIL DEFAULT_MSG SOIL_INCLUDE_DIR SOIL_LIBRARY)

# Define GLFW_LIBRARIES and GLFW_INCLUDE_DIRS
if (SOIL_FOUND)
	set(SOIL_LIBRARIES ${SOIL_LIBRARY})
	set(SOIL_INCLUDE_DIRS ${SOIL_INCLUDE_DIR})
endif()

# Hide some variables
mark_as_advanced(SOIL_INCLUDE_DIR SOIL_LIBRARY)
