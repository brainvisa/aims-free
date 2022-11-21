CMAKE_MINIMUM_REQUIRED(VERSION 3.10)

#UseDoxygen trips over a policy change in cmake 3, suppress the warning
CMAKE_POLICY(VERSION 3.10)
#the suggested version-type policy command doesn't shut this warning up, so set it manually
IF (POLICY CMP0045)
    CMAKE_POLICY(SET CMP0045 OLD)
ENDIF (POLICY CMP0045)


SET(CIFTILIB_VERSION 1.5)

IF ( NO_BV_MAKER )
    CMAKE_POLICY( SET CMP0048 NEW )
    PROJECT( CiftiLib VERSION ${CIFTILIB_VERSION})
else()
    PROJECT(CiftiLib)
endif()

SET(CIFTI_DEFINITIONS)

#MSVC seems like the only compiler that chokes on -W -Wall
# IF (NOT MSVC)
#     SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -W -Wall")
# ENDIF (NOT MSVC)

SET(IGNORE_QT FALSE CACHE BOOL "don't try to use QT")

SET(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake/Modules/" "${CMAKE_CURRENT_SOURCE_DIR}/cmake/Modules/UseDoxygen/")

INCLUDE(UseDoxygen)

#QT
IF (NOT IGNORE_QT)
    # here in Aims, Qt detection has already been done (and may be qt5)
#     FIND_PACKAGE(Qt4 4.8.0 QUIET)
    IF( ${DESIRED_QT_VERSION} EQUAL 4 AND QT_FOUND )
#     IF (QT_FOUND)
        SET(QT_DONT_USE_QTGUI TRUE)
        ADD_DEFINITIONS(-DCIFTILIB_USE_QT)
#         INCLUDE(${QT_USE_FILE})
        SET(LIBS ${LIBS} ${QT_LIBRARIES})
        #for pkg-config file
        SET(CIFTILIB_PKGCONFIG_REQUIRES_LINE "Requires: QtCore >= 4.8.0 QtXml")
        SET(CIFTILIB_PKGCONFIG_DEFINE "-DCIFTILIB_USE_QT")
    ELSEIF ( ${DESIRED_QT_VERSION} EQUAL 5 AND QT_FOUND )
#         FIND_PACKAGE(Qt5Core)
        IF (Qt5Core_FOUND)
            SET(QT_FOUND TRUE)
            INCLUDE_DIRECTORIES(${Qt5Core_INCLUDE_DIRS})
            SET(LIBS ${LIBS} Qt5::Core)
            #whatever that means
            ADD_DEFINITIONS(-DCIFTILIB_USE_QT)
            SET(CIFTILIB_PKGCONFIG_REQUIRES_LINE "Requires: Qt5Core Qt5Xml")
            SET(CIFTILIB_PKGCONFIG_DEFINE "-DCIFTILIB_USE_QT")
        ENDIF (Qt5Core_FOUND)
    ELSEIF ( ${DESIRED_QT_VERSION} EQUAL 6 AND QT_FOUND )
#         FIND_PACKAGE(Qt5Core)
        IF (Qt6Core_FOUND)
            SET(QT_FOUND TRUE)
            INCLUDE_DIRECTORIES(${Qt5Core_INCLUDE_DIRS}
                                ${Qt5Core5Compat_INCLUDE_DIRS})
            SET(LIBS ${LIBS} Qt6::Core Qt6::Core5Compat)
            #whatever that means
            ADD_DEFINITIONS(-DCIFTILIB_USE_QT)
            SET(CIFTILIB_PKGCONFIG_REQUIRES_LINE "Requires: Qt6Core Qt6Xml")
            SET(CIFTILIB_PKGCONFIG_DEFINE "-DCIFTILIB_USE_QT")
        ENDIF ()
    ENDIF ()
ENDIF (NOT IGNORE_QT)

#alternative to QT xml, string
IF (NOT QT_FOUND)
    FIND_PACKAGE(libxml++ 2.17.0 REQUIRED)
    INCLUDE_DIRECTORIES(${libxml++_INCLUDE_DIRS})
    SET(LIBS ${LIBS} ${libxml++_LIBRARIES})
    ADD_DEFINITIONS(-DCIFTILIB_USE_XMLPP)
    #for pkg-config file
    SET(CIFTILIB_PKGCONFIG_REQUIRES_LINE "Requires: libxml++-2.6 >= 2.17.0")
    SET(CIFTILIB_PKGCONFIG_DEFINE "-DCIFTILIB_USE_XMLPP")
ENDIF (NOT QT_FOUND)

SET(CIFTI_DEFINITIONS ${CIFTILIB_PKGCONFIG_DEFINE})

#boost, including filesystem if not using QT
IF (NOT QT_FOUND)
    FIND_PACKAGE(Boost REQUIRED COMPONENTS filesystem system)
ELSE (NOT QT_FOUND)
    FIND_PACKAGE(Boost REQUIRED)
ENDIF (NOT QT_FOUND)
INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIRS})
SET(LIBS ${LIBS} ${Boost_LIBRARIES})
#boost quirks
IF (Boost_VERSION LESS 104400)
    #absolute() was added in 1.44.0, with filesystem v3
    ADD_DEFINITIONS(-DCIFTILIB_BOOST_NO_FSV3)
    LIST(APPEND CIFTI_DEFINITIONS "-DCIFTILIB_BOOST_NO_FSV3")
ENDIF (Boost_VERSION LESS 104400)
IF (Boost_VERSION LESS 104800)
    #canonical() was added in 1.48.0
    ADD_DEFINITIONS(-DCIFTILIB_BOOST_NO_CANONICAL)
    LIST(APPEND CIFTI_DEFINITIONS "-DCIFTILIB_BOOST_NO_CANONICAL")
ENDIF (Boost_VERSION LESS 104800)
IF (Boost_VERSION LESS 105600)
    #try_lexical_cast was added in 1.56.0
    ADD_DEFINITIONS(-DCIFTILIB_BOOST_NO_TRY_LEXICAL)
    LIST(APPEND CIFTI_DEFINITIONS "-DCIFTILIB_BOOST_NO_TRY_LEXICAL")
ENDIF (Boost_VERSION LESS 105600)


#zlib, useful for volume reading
FIND_PACKAGE(ZLIB)
IF (ZLIB_FOUND)
    INCLUDE_DIRECTORIES(${ZLIB_INCLUDE_DIRS})
    SET(LIBS ${LIBS} ${ZLIB_LIBRARIES})
    ADD_DEFINITIONS("-DCIFTILIB_HAVE_ZLIB")
    LIST(APPEND CIFTI_DEFINITIONS "-DCIFTILIB_HAVE_ZLIB")
ENDIF (ZLIB_FOUND)
#OS X has some weirdness in its zlib, so let the preprocessor know
IF (APPLE)
    ADD_DEFINITIONS(-DCIFTILIB_OS_MACOSX)
    LIST(APPEND CIFTI_DEFINITIONS "-DCIFTILIB_OS_MACOSX")
ENDIF (APPLE)

#openmp provides a fast mutex implementation, faster than QT (and probably faster than glibmm)
# brainvisa: skip OpenMP for now to avoid having to package it
# FIND_PACKAGE(OpenMP)
# IF (OPENMP_FOUND)
#     SET(CMAKE_CXX_FLAGS "${OpenMP_CXX_FLAGS} ${CMAKE_CXX_FLAGS}")
# ENDIF (OPENMP_FOUND)

ENABLE_TESTING()

#the library source, doesn't contain build targets
ADD_SUBDIRECTORY(src)

#example directory, has build targets and tests
# ADD_SUBDIRECTORY(example)

macro(append_subdir_files variable dirname)
    get_directory_property(holder DIRECTORY ${dirname} DEFINITION ${variable})
    foreach(depfile ${holder})
        list(APPEND ${variable} "${dirname}/${depfile}")
    endforeach()
endmacro()

#get the sources and install info
append_subdir_files(SOURCES src)
append_subdir_files(HEADERS src)
append_subdir_files(PUBLIC_HEADERS src)
append_subdir_files(PRIVATE_DIRS src)

ADD_LIBRARY(Cifti SHARED
${SOURCES}
${HEADERS}
)

#one way to get qt5's new compiler flag restrictions into the build - does it have other consequences?
TARGET_LINK_LIBRARIES(Cifti ${LIBS})

#NOTE: soversion set to 0 because ABI compatibility was not designed into the interface
#soversion defines what symlinks are created, version defines what to put on the end of the actual library file
SET_TARGET_PROPERTIES(Cifti
PROPERTIES
OUTPUT_NAME Cifti
SOVERSION 0
VERSION ${CIFTILIB_VERSION}
)

INCLUDE_DIRECTORIES(
${CMAKE_CURRENT_SOURCE_DIR}/src
)

#install dirs
IF(NOT CMAKE_VERSION VERSION_LESS "3.4")
  # GNUInstallDirs appeared in cmake 3.4
  INCLUDE(GNUInstallDirs)
ENDIF()

#pkg-config
CONFIGURE_FILE(CiftiLib.pc.in CiftiLib.pc @ONLY)
# INSTALL(FILES ${CMAKE_CURRENT_BINARY_DIR}/CiftiLib.pc DESTINATION ${CMAKE_INSTALL_LIBDIR}/pkgconfig)

#install
# INSTALL(TARGETS Cifti DESTINATION ${CMAKE_INSTALL_LIBDIR})
# INSTALL(FILES ${PUBLIC_HEADERS} DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/CiftiLib)
# INSTALL(DIRECTORY ${PRIVATE_DIRS} DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/CiftiLib FILES_MATCHING PATTERN "*.h")

# for brainvisa-cmake
SET(CIFTI_LIBS Cifti ${LIBS} PARENT_SCOPE)
SET(CIFTI_DEFINITIONS ${CIFTI_DEFINITIONS} PARENT_SCOPE)
SET(CIFTI_INCLUDES "${CMAKE_CURRENT_SOURCE_DIR}/src"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/Cifti"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/Nifti"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/Common" PARENT_SCOPE)
BRAINVISA_INSTALL(FILES ${CMAKE_CURRENT_BINARY_DIR}/CiftiLib.pc
    DESTINATION lib/pkgconfig COMPONENT aims-free-dev)
BRAINVISA_INSTALL(TARGETS Cifti DESTINATION lib COMPONENT aims-free)
BRAINVISA_INSTALL(FILES ${PUBLIC_HEADERS} DESTINATION include/CiftiLib
    COMPONENT aims-free-dev)
BRAINVISA_INSTALL_DIRECTORY(${PRIVATE_DIRS} DESTINATION include/CiftiLib
    FILES_MATCHING PATTERN "*.h" COMPONENT aims-free-dev)

