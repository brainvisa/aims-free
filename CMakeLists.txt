cmake_minimum_required( VERSION 2.6 )
find_package( brainvisa-cmake REQUIRED )
BRAINVISA_PROJECT()

BRAINVISA_DEPENDENCY( DEV DEPENDS "${BRAINVISA_PACKAGE_NAME}" RUN "= ${${BRAINVISA_PACKAGE_NAME}_VERSION}" )

BRAINVISA_FIND_PACKAGE( soma-base REQUIRED )
math( EXPR result "${soma-base_VERSION_MINOR}+1" )
BRAINVISA_DEPENDENCY( DEV DEPENDS "soma-base" DEV ">= ${soma-base_VERSION};<< ${soma-base_VERSION_MAJOR}.${result}" )
include( "${SOMA-BASE_USE_FILE}" )

find_package( Sigc++2 REQUIRED )
BRAINVISA_DEPENDENCY( RUN DEPENDS libsigc++2 RUN 2.0-0c2a )
BRAINVISA_DEPENDENCY( DEV DEPENDS libsigc++2 DEV 2.0 )

find_package( LibXml2 REQUIRED )
BRAINVISA_DEPENDENCY( RUN DEPENDS libxml2 RUN )
BRAINVISA_DEPENDENCY( DEV DEPENDS libxml2 DEV )

find_package( Boost REQUIRED )
BRAINVISA_DEPENDENCY( DEV DEPENDS libboost DEV )

find_package( Blitz )
if( BLITZ_FOUND )
  BRAINVISA_DEPENDENCY( RUN DEPENDS blitz++ RUN )
  set( BLITZ_FIND_PACKAGE "find_package( Blitz REQUIRED )" )
endif()
BRAINVISA_DEPENDENCY( DEV RECOMMENDS blitz++ DEV )

if( NOT DESIRED_QT_VERSION )
  find_package( Qt )
endif()
if( DESIRED_QT_VERSION EQUAL 4 )
  find_package( Qt4 COMPONENTS QtCore QtGui Qt3Support QtSql )
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

find_package( DCMTK )
if( DCMTK_FOUND )
  BRAINVISA_DEPENDENCY( RUN DEPENDS dcmtk RUN )
endif()

find_package( MINC )
BRAINVISA_DEPENDENCY( DEV RECOMMENDS dcmtk DEV )
if( MINC_FOUND )
  BRAINVISA_DEPENDENCY( RUN DEPENDS libminc RUN )
endif()
BRAINVISA_DEPENDENCY( DEV RECOMMENDS libminc DEV )

find_package( TIFF )
if( TIFF_FOUND )
  BRAINVISA_DEPENDENCY( RUN DEPENDS libtiff RUN )
endif()
BRAINVISA_DEPENDENCY( DEV RECOMMENDS libtiff DEV )

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

set( AIMS_DEFINITIONS "-D_REENTRANT -DAIMS" )
if( "${CMAKE_BUILD_TYPE}" STREQUAL "Release" OR "${CMAKE_BUILD_TYPE}" STREQUAL "RelWithDebInfo" OR "${CMAKE_BUILD_TYPE}" STREQUAL "MinSizeRel" )
  set( AIMS_DEFINITIONS "${AIMS_DEFINITIONS} -DCARTO_DEBUGMODE=\"release\"" )
else( "${CMAKE_BUILD_TYPE}" STREQUAL "Release" OR "${CMAKE_BUILD_TYPE}" STREQUAL "RelWithDebInfo" OR "${CMAKE_BUILD_TYPE}" STREQUAL "MinSizeRel" )
  if( "${CMAKE_BUILD_TYPE}" STREQUAL "Debug" )
    set( AIMS_DEFINITIONS "${AIMS_DEFINITIONS} -DCARTO_DEBUGMODE=\"debug\"" )
  else( "${CMAKE_BUILD_TYPE}" STREQUAL "Debug" )
    set( AIMS_DEFINITIONS "${AIMS_DEFINITIONS} -DCARTO_DEBUGMODE=\"default\"" )
  endif( "${CMAKE_BUILD_TYPE}" STREQUAL "Debug" )
endif( "${CMAKE_BUILD_TYPE}" STREQUAL "Release" OR "${CMAKE_BUILD_TYPE}" STREQUAL "RelWithDebInfo" OR "${CMAKE_BUILD_TYPE}" STREQUAL "MinSizeRel" )
if( BLITZ_FOUND )
  set( AIMS_DEFINITIONS "${AIMS_DEFINITIONS} -DCARTO_USE_BLITZ" )
endif()
add_definitions( ${AIMS_DEFINITIONS} )

set( AIMS_INCLUDE_DIRS 
     ${Sigc++2_INCLUDE_DIRS} 
     "${LIBXML2_INCLUDE_DIR}"
     ${Boost_INCLUDE_DIRS} )
if( BLITZ_FOUND )
  set( AIMS_INCLUDE_DIRS ${AIMS_INCLUDE_DIRS} 
       "${BLITZ_INCLUDE_DIRS}" )
endif()
if( PYTHON_INCLUDE_PATH )
  set( AIMS_INCLUDE_DIRS ${AIMS_INCLUDE_DIRS} 
       "${PYTHON_INCLUDE_PATH}" )
endif()
if( NUMPY_FOUND )
  set( AIMS_INCLUDE_DIRS ${AIMS_INCLUDE_DIRS}
       "${NUMPY_INCLUDE_DIR}" )
endif( NUMPY_FOUND )
if( SIP_FOUND )
  set( AIMS_INCLUDE_DIRS ${AIMS_INCLUDE_DIRS}
       "${SIP_INCLUDE_DIR}" )
endif( SIP_FOUND )
if( JPEG_FOUND )
  set( AIMS_INCLUDE_DIRS ${AIMS_INCLUDE_DIRS}
       "${JPEG_INCLUDE_DIR}" )
endif()
if( DCMTK_FOUND )
  set( AIMS_INCLUDE_DIRS ${AIMS_INCLUDE_DIRS}
       "${DCMTK_INCLUDE_DIR}" )
endif()
if( MINC_FOUND )
  set( AIMS_INCLUDE_DIRS ${AIMS_INCLUDE_DIRS}
       "${MINC_INCLUDE_DIRS}" )
endif()
if( TIFF_FOUND )
  set( AIMS_INCLUDE_DIRS ${AIMS_INCLUDE_DIRS}
       "${TIFF_INCLUDE_DIR}" )
endif()
include_directories( ${CMAKE_BINARY_DIR}/include ${AIMS_INCLUDE_DIRS} )

set( AIMS_AIMSDATA_LIBRARIES aimsdata graph cartodata cartobase "${LIBXML2_LIBRARIES}" )
set( AIMS_PYAIMS_LIBRARIES pyaims ${AIMS_AIMSDATA_LIBRARIES} "${PYTHON_LIBRARY}" )


add_subdirectory( cartobase )
add_subdirectory( cartodata )
add_subdirectory( graph )
add_subdirectory( ecat )
add_subdirectory( ecat+ )
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

BRAINVISA_GENERATE_DOCBOOK_DOC()

BRAINVISA_CREATE_CMAKE_CONFIG_FILES()

