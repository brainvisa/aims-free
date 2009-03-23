cmake_minimum_required( VERSION 2.6 )

find_package( soma-infra REQUIRED )

SOMA_PROJECT()

find_package( Sigc++2 REQUIRED )
SOMA_ADD_RUNTIME_DEPENDENCY( THIRDPARTY Sigc++2 REQUIRED )
find_package( LibXml2 REQUIRED )
SOMA_ADD_RUNTIME_DEPENDENCY( THIRDPARTY LibXml2 REQUIRED )
find_package( Boost REQUIRED )
SOMA_ADD_RUNTIME_DEPENDENCY( THIRDPARTY Boost REQUIRED )
find_package( Doxygen )
find_package( JPEG )
if( JPEG_FOUND )
  SOMA_ADD_RUNTIME_DEPENDENCY( THIRDPARTY JPEG )
endif( JPEG_FOUND )
find_package( DCMTK2 )
if( DCMTK2_FOUND )
  SOMA_ADD_RUNTIME_DEPENDENCY( THIRDPARTY DCMTK2 )
endif( DCMTK2_FOUND )
find_package( MINC )
if( MINC_FOUND )
  SOMA_ADD_RUNTIME_DEPENDENCY( THIRDPARTY MINC )
endif( MINC_FOUND )
find_package( TIFF )
if( TIFF_FOUND )
  SOMA_ADD_RUNTIME_DEPENDENCY( THIRDPARTY TIFF )
endif( TIFF_FOUND )
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


add_subdirectory( cartobase )
add_subdirectory( cartodata )
add_subdirectory( graph )
add_subdirectory( ecat )
add_subdirectory( ecat+ )
add_subdirectory( vidaIO )
add_subdirectory( aimsdata )
add_subdirectory( aimsalgo )
if( PYTHONLIBS_FOUND AND NUMPY_FOUND AND SIP_FOUND )
  SOMA_ADD_RUNTIME_DEPENDENCY( THIRDPARTY PythonInterp )
  SOMA_ADD_RUNTIME_DEPENDENCY( THIRDPARTY Numpy )
  SOMA_ADD_RUNTIME_DEPENDENCY( THIRDPARTY SIP )
  set( AIMS_RELATIVE_SIP_DIRECTORY
       "share/sip/${PROJECT_NAME}-${${PROJECT_NAME}_VERSION_MAJOR}.${${PROJECT_NAME}_VERSION_MINOR}" )
  add_subdirectory( pyaims )
else( PYTHONLIBS_FOUND AND NUMPY_FOUND AND SIP_FOUND )
  message( "WARNING: Not building pyaims and pyaimsalgo due to missing dependency:\n  PYTHONLIBS_FOUND=${PYTHONLIBS_FOUND}\n  NUMPY_FOUND=${NUMPY_FOUND}\n  SIP_FOUND=${SIP_FOUND}" )
endif( PYTHONLIBS_FOUND AND NUMPY_FOUND AND SIP_FOUND )

set( AIMS_INSTALL_DIR "${CMAKE_INSTALL_PREFIX}" )
set( AIMS_BINARY_DIR "${CMAKE_BINARY_DIR}" )

SOMA_CREATE_CMAKE_CONFIG_FILES()

