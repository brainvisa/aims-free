cmake_minimum_required( VERSION 2.6 )
find_package( brainvisa-cmake REQUIRED )
BRAINVISA_PROJECT( CXX Fortran )
enable_testing()

BRAINVISA_DEPENDENCY(RUN DEPENDS libgfortran2 RUN)

BRAINVISA_DEPENDENCY( DEV DEPENDS "${BRAINVISA_PACKAGE_NAME}" RUN "= ${${BRAINVISA_PACKAGE_NAME}_VERSION}" )

BRAINVISA_FIND_PACKAGE( soma-io REQUIRED )
include( "${SOMA-IO_USE_FILE}" )
BRAINVISA_FIND_PACKAGE( soma-base REQUIRED )
math( EXPR result "${soma-io_VERSION_MINOR}+1" )
BRAINVISA_DEPENDENCY( RUN DEPENDS "soma-io" RUN ">= ${soma-io_VERSION};<< ${soma-io_VERSION_MAJOR}.${result}" )
BRAINVISA_DEPENDENCY( DEV DEPENDS "soma-io" DEV ">= ${soma-io_VERSION};<< ${soma-io_VERSION_MAJOR}.${result}" )
math( EXPR result "${soma-base_VERSION_MINOR}+1" )
BRAINVISA_DEPENDENCY( DEV DEPENDS "soma-base" DEV ">= ${soma-base_VERSION};<< ${soma-base_VERSION_MAJOR}.${result}" )
include( "${SOMA-BASE_USE_FILE}" )
BRAINVISA_DEPENDENCY( RUN DEPENDS "soma-base" RUN ">= ${soma-base_VERSION};<< ${soma-base_VERSION_MAJOR}.${result}" )
BRAINVISA_DEPENDENCY( RUN RECOMMENDS brainvisa-share RUN ">= ${brainvisa-share_VERSION}" )


find_package( Blitz )
if( BLITZ_FOUND )
  BRAINVISA_DEPENDENCY( RUN DEPENDS blitz++ RUN )
  set( BLITZ_FIND_PACKAGE "find_package( Blitz REQUIRED )" )
  BRAINVISA_DEPENDENCY( DEV DEPENDS blitz++ DEV )
endif()

if( NOT DESIRED_QT_VERSION )
  find_package( Qt )
endif()
if( DESIRED_QT_VERSION EQUAL 4 )
  find_package( Qt4 COMPONENTS QtCore QtGui QtSql )
elseif( DESIRED_QT_VERSION EQUAL 5 )
  find_package( Qt5Core )
  find_package( Qt5Widgets )
  find_package( Qt5LinguistTools )
  find_package( Qt5Sql )
endif()
find_package( QtVersion )

find_package( Doxygen )

find_package( python )
if( PYTHON_FOUND )
  find_package( Numpy )
  find_package( SIP )
  if( PYTHON_FOUND AND NUMPY_FOUND AND SIP_FOUND )
    BRAINVISA_DEPENDENCY( RUN DEPENDS python RUN )
    BRAINVISA_DEPENDENCY( DEV DEPENDS python DEV )
    BRAINVISA_DEPENDENCY( RUN DEPENDS python-numpy RUN )
    BRAINVISA_DEPENDENCY( RUN DEPENDS python-sip4 RUN )
    BRAINVISA_DEPENDENCY( DEV DEPENDS python-sip4 DEV )
  endif()
endif()

find_package( JPEG )
if( JPEG_FOUND )
  BRAINVISA_DEPENDENCY( RUN DEPENDS libjpeg62 RUN )
endif()
BRAINVISA_DEPENDENCY( DEV RECOMMENDS libjpeg62 DEV )

find_package( TIFF )
if( TIFF_FOUND )
  BRAINVISA_DEPENDENCY( RUN DEPENDS libtiff RUN )
  BRAINVISA_DEPENDENCY( DEV RECOMMENDS libtiff DEV )
endif()

find_package( PNG12 )
if(PNG12_FOUND)
  BRAINVISA_DEPENDENCY( RUN DEPENDS libpng12 RUN )
  BRAINVISA_DEPENDENCY( DEV RECOMMENDS libpng12 DEV )
endif()

find_package( DCMTK )
if( DCMTK_FOUND )
  BRAINVISA_DEPENDENCY( RUN DEPENDS dcmtk RUN )
  BRAINVISA_DEPENDENCY( DEV RECOMMENDS dcmtk DEV )
endif()

find_package( MINC )
if( MINC_FOUND )
  BRAINVISA_DEPENDENCY( RUN DEPENDS libminc RUN )
endif()
BRAINVISA_DEPENDENCY( DEV RECOMMENDS libminc DEV )

find_package( EXPAT )
if( EXPAT_FOUND )
  BRAINVISA_DEPENDENCY( RUN DEPENDS libexpat1 RUN )
endif()
BRAINVISA_DEPENDENCY( DEV RECOMMENDS libexpat1 DEV )

find_package( ZLIB )
if( ZLIB_FOUND )
  BRAINVISA_DEPENDENCY( RUN DEPENDS zlib RUN )
endif()
BRAINVISA_DEPENDENCY( DEV RECOMMENDS zlib DEV )

find_package( LibKDTree QUIET )


if( BLITZ_FOUND )
  set( CARTO_USE_BLITZ "True" )
endif()

set( AIMS_INCLUDE_DIRS 
     ${SOMA-IO_INCLUDE_DIRS} )
if( BLITZ_FOUND )
  set( AIMS_INCLUDE_DIRS "${BLITZ_INCLUDE_DIRS}" ${AIMS_INCLUDE_DIRS} )
endif()
if( PYTHON_INCLUDE_PATH )
  set( PYAIMS_INCLUDE_DIRS "${PYTHON_INCLUDE_PATH}" )
endif()
if( NUMPY_FOUND )
  set( PYAIMS_INCLUDE_DIRS "${NUMPY_INCLUDE_DIR}" ${PYAIMS_INCLUDE_DIRS} )
endif( NUMPY_FOUND )
if( SIP_FOUND )
  set( PYAIMS_INCLUDE_DIRS "${SIP_INCLUDE_DIR}" ${PYAIMS_INCLUDE_DIRS} )
endif( SIP_FOUND )
if( JPEG_FOUND )
  set( AIMS_INCLUDE_DIRS ${AIMS_INCLUDE_DIRS}
       "${JPEG_INCLUDE_DIR}" )
endif()
if( DCMTK_FOUND )
  set( AIMS_INCLUDE_DIRS ${AIMS_INCLUDE_DIRS}
       ${DCMTK_INCLUDE_DIR} )
endif()
if( MINC_FOUND )
  set( AIMS_INCLUDE_DIRS ${AIMS_INCLUDE_DIRS}
       "${LIBMINC_INCLUDE_DIRS}" )
  if( MINC2_FOUND )
    set( AIMS_HAS_MINC2 "1" )
  endif()
endif()
if( TIFF_FOUND )
  set( AIMS_INCLUDE_DIRS ${AIMS_INCLUDE_DIRS}
       "${TIFF_INCLUDE_DIR}" )
endif()

if( LIBKDTREE_FOUND )
  set( AIMS_INCLUDE_DIRS ${AIMS_INCLUDE_DIRS} ${LIBKDTREE_INCLUDE_DIR} )
  # else it will be done in aimsdata lib cmakelist
endif()

include_directories( ${CMAKE_BINARY_DIR}/include ${AIMS_INCLUDE_DIRS} )

set( AIMS_AIMSDATA_LIBRARIES aimsdata graph cartodata cartobase soma-io "${LIBXML2_LIBRARIES}" )
set( AIMS_PYAIMS_LIBRARIES pyaims ${AIMS_AIMSDATA_LIBRARIES} "${PYTHON_LIBRARY}" )

# Copy python directory for aims tests
BRAINVISA_COPY_PYTHON_DIRECTORY( "${CMAKE_CURRENT_SOURCE_DIR}/python"
                                 ${PROJECT_NAME} )

configure_file( "${CMAKE_CURRENT_SOURCE_DIR}/cartodata/config/config.h.in" "${CMAKE_BINARY_DIR}/include/cartodata/config/config.h" @ONLY )
configure_file( "${CMAKE_CURRENT_SOURCE_DIR}/aimsdata/config/config.h.in" "${CMAKE_BINARY_DIR}/include/aims/config/config.h" @ONLY )
configure_file( "${CMAKE_CURRENT_SOURCE_DIR}/aimsdata/src/aimsminc/config/aimsminc_config.h.in" "${CMAKE_BINARY_DIR}/include/aims/config/aimsminc_config.h" @ONLY )
BRAINVISA_INSTALL( FILES "${CMAKE_BINARY_DIR}/include/cartodata/config/config.h"
                   DESTINATION "include/cartodata/config"
                   COMPONENT ${PROJECT_NAME}-dev )
BRAINVISA_INSTALL( FILES "${CMAKE_BINARY_DIR}/include/aims/config/config.h"
                   DESTINATION "include/aims/config"
                   COMPONENT ${PROJECT_NAME}-dev )
BRAINVISA_INSTALL( FILES "${CMAKE_BINARY_DIR}/include/aims/config/aimsminc_config.h"
                   DESTINATION "include/aims/config"
                   COMPONENT ${PROJECT_NAME}-dev )
add_subdirectory( cartodata )
add_subdirectory( graph )

option(BUILD_ECAT "Build the Ecat library and tools that depend on it" ON)
mark_as_advanced( BUILD_ECAT )
if( BUILD_ECAT )
  add_subdirectory( ecat )
  add_subdirectory( ecat+ )
endif( BUILD_ECAT )

if( NOT WIN32 )
  add_subdirectory( vidaIO )
endif( NOT WIN32 )
add_subdirectory( aimsdata )
add_subdirectory( aimsalgo )
if( QT_FOUND )
  add_subdirectory( aimsgui )
endif()
if( PYTHON_FOUND AND NUMPY_FOUND AND SIP_FOUND )
  set( AIMS_RELATIVE_SIP_DIRECTORY
       "share/aims-${${PROJECT_NAME}_VERSION_MAJOR}.${${PROJECT_NAME}_VERSION_MINOR}/sip" )
  add_subdirectory( pyaims )
else()
  message( "WARNING: Not building pyaims and pyaimsalgo due to missing dependency:\n  PYTHON_FOUND=${PYTHON_FOUND}\n  NUMPY_FOUND=${NUMPY_FOUND}\n  SIP_FOUND=${SIP_FOUND}" )
endif()

set( AIMS_INSTALL_DIR "${CMAKE_INSTALL_PREFIX}" )
set( AIMS_BINARY_DIR "${CMAKE_BINARY_DIR}" )

# docbook docs are now obsolete.
# BRAINVISA_GENERATE_DOCBOOK_DOC()

BRAINVISA_CREATE_CMAKE_CONFIG_FILES()

