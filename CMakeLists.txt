# This will be the main CMakeLists.txt of aims project.
# It is still work in progress, this is why there are
# several $ENV{P4} that will disappear later.

cmake_minimum_required( VERSION 2.6 )

find_package( soma-infra REQUIRED )

project( aims )

set( CPACK_PACKAGE_NAME "${PROJECT_NAME}" )
set( CPACK_PACKAGE_VENDOR "CEA - NeuroSpin" )
set( CPACK_PACKAGE_VERSION_MAJOR 4 )
set( CPACK_PACKAGE_VERSION_MINOR 0 )
set( CPACK_PACKAGE_VERSION_PATCH 0 )
set( CPACK_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME}-${CPACK_PACKAGE_VERSION_MAJOR}.${CPACK_PACKAGE_VERSION_MINOR}.${CPACK_PACKAGE_VERSION_PATCH}-linux-i686" )
set( CPACK_PACKAGE_INSTALL_DIRECTORY "aims-${CPACK_PACKAGE_VERSION_MAJOR}.${CPACK_PACKAGE_VERSION_MINOR}" )
# SET( CPACK_INSTALL_CMAKE_PROJECTS "${CMAKE_BINARY_DIR}" aims cartobase-devel toto )

set( ${PROJECT_NAME}_VERSION_MAJOR ${CPACK_PACKAGE_VERSION_MAJOR} )
set( ${PROJECT_NAME}_VERSION_MINOR ${CPACK_PACKAGE_VERSION_MINOR} )
set( ${PROJECT_NAME}_VERSION_PATCH ${CPACK_PACKAGE_VERSION_PATCH} )
set( ${PROJECT_NAME}_VERSION ${${PROJECT_NAME}_VERSION_MAJOR}.${${PROJECT_NAME}_VERSION_MINOR}.${${PROJECT_NAME}_VERSION_PATCH} )

include( CPack )

SOMA_CREATE_MAIN_COMPONENTS()

find_package( Sigc++2 REQUIRED )
find_package( LibXml2 REQUIRED )
find_package( Boost REQUIRED )
find_package( Doxygen )
find_package( JPEG )
find_package( DCMTK2 )
find_package( MINC )
find_package( TIFF )
find_package( Qt3 )
find_package( PythonInterp )
if( PYTHONINTERP_FOUND )
  find_package( PythonLibs )
  find_package( Numpy )
  find_package( SIP )
endif( PYTHONINTERP_FOUND )

if( "${CMAKE_BUILD_TYPE}" STREQUAL "Release" OR "${CMAKE_BUILD_TYPE}" STREQUAL "RelWithDebInfo" OR "${CMAKE_BUILD_TYPE}" STREQUAL "MinSizeRel" )
  set( AIMS_DEFINITIONS "-DCARTO_DEBUGMODE=\"release\"" )
else( "${CMAKE_BUILD_TYPE}" STREQUAL "Release" OR "${CMAKE_BUILD_TYPE}" STREQUAL "RelWithDebInfo" OR "${CMAKE_BUILD_TYPE}" STREQUAL "MinSizeRel" )
  if( "${CMAKE_BUILD_TYPE}" STREQUAL "Debug" )
    set( AIMS_DEFINITIONS "-DCARTO_DEBUGMODE=\"debug\"" )
  else( "${CMAKE_BUILD_TYPE}" STREQUAL "Debug" )
    set( AIMS_DEFINITIONS "-DCARTO_DEBUGMODE=\"default\"" )
  endif( "${CMAKE_BUILD_TYPE}" STREQUAL "Debug" )
endif( "${CMAKE_BUILD_TYPE}" STREQUAL "Release" OR "${CMAKE_BUILD_TYPE}" STREQUAL "RelWithDebInfo" OR "${CMAKE_BUILD_TYPE}" STREQUAL "MinSizeRel" )
add_definitions( ${AIMS_DEFINITIONS} )

set( AIMS_INCLUDE_DIRS 
     ${Sigc++2_INCLUDE_DIRS} 
     "${LIBXML2_INCLUDE_DIR}"
     ${Boost_INCLUDE_DIRS}
     ${MINC_INCLUDE_DIRS} )
if( PYTHONLIBS_FOUND )
  set( AIMS_INCLUDE_DIRS ${AIMS_INCLUDE_DIRS} 
       "${PYTHON_INCLUDE_PATH}" )
endif( PYTHONLIBS_FOUND )
if( NUMPY_FOUND )
  set( AIMS_INCLUDE_DIRS ${AIMS_INCLUDE_DIRS}
       "${NUMPY_INCLUDE_DIR}" )
endif( NUMPY_FOUND )
if( SIP_FOUND )
  set( AIMS_INCLUDE_DIRS ${AIMS_INCLUDE_DIRS}
       "${SIP_INCLUDE_DIR}" )
endif( SIP_FOUND )
include_directories( ${CMAKE_BINARY_DIR}/include ${AIMS_INCLUDE_DIRS} )


set( CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin )
set( CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib )


add_subdirectory( $ENV{P4}/cartobase-main cartobase-main )
add_subdirectory( $ENV{P4}/cartodata-main cartodata-main )
add_subdirectory( $ENV{P4}/graph-main graph-main )
add_subdirectory( $ENV{P4}/ecat-main ecat-main )
add_subdirectory( $ENV{P4}/ecat+-main ecat+-main )
add_subdirectory( $ENV{P4}/vidaIO-main vidaIO-main )
add_subdirectory( $ENV{P4}/aimsdata-main aimsdata-main )
add_subdirectory( $ENV{P4}/aimsalgo-main aimsalgo-main )
if( PYTHONLIBS_FOUND AND NUMPY_FOUND AND SIP_FOUND )
  set( AIMS_RELATIVE_SIP_DIRECTORY
       "share/sip/${PROJECT_NAME}-${${PROJECT_NAME}_VERSION_MAJOR}.${${PROJECT_NAME}_VERSION_MINOR}" )
  add_subdirectory( $ENV{P4}/pyaims-main pyaims-main )
else( PYTHONLIBS_FOUND AND NUMPY_FOUND AND SIP_FOUND )
  message( "WARNING: Not building pyaims and pyaimsalgo due to missing dependency:\n  PYTHONLIBS_FOUND=${PYTHONLIBS_FOUND}\n  NUMPY_FOUND=${NUMPY_FOUND}\n  SIP_FOUND=${SIP_FOUND}" )
endif( PYTHONLIBS_FOUND AND NUMPY_FOUND AND SIP_FOUND )

set( AIMS_INSTALL_DIR "${CMAKE_INSTALL_PREFIX}" )
set( AIMS_BINARY_DIR "${CMAKE_BINARY_DIR}" )
SOMA_CREATE_CMAKE_CONFIG_FILES()
