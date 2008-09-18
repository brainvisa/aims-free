project( aimsdata )
SOMA_SET_PROJECT_VERSION()

SOMA_ADD_COMPONENT( aimsdata 
                    GROUP runtime
                    DESCRIPTION "aimsdata C++ library" )

SOMA_ADD_COMPONENT( aimsdata-devel 
                    GROUP devel
                    DESCRIPTION "Developpement files for aimsdata"
                    DEPENDS aimsdata )
SOMA_ADD_COMPONENT( aimsecat-devel 
                    GROUP devel
                    DESCRIPTION "Developpement files for aimsecat"
                    DEPENDS aimsdata-devel )
SOMA_ADD_COMPONENT( aimsply-devel 
                    GROUP devel
                    DESCRIPTION "Developpement files for aimsply"
                    DEPENDS aimsdata-devel )
SOMA_ADD_COMPONENT( aimsvida-devel 
                    GROUP devel
                    DESCRIPTION "Developpement files for aimsvida"
                    DEPENDS aimsdata-devel )
SOMA_ADD_COMPONENT( aimsjpeg-devel 
                    GROUP devel
                    DESCRIPTION "Developpement files for aimsjpeg"
                    DEPENDS aimsdata-devel )
SOMA_ADD_COMPONENT( aimsdicom-devel 
                    GROUP devel
                    DESCRIPTION "Developpement files for aimsdicom"
                    DEPENDS aimsdata-devel )
SOMA_ADD_COMPONENT( aimsminc-devel 
                    GROUP devel
                    DESCRIPTION "Developpement files for aimsminc"
                    DEPENDS aimsdata-devel )
SOMA_ADD_COMPONENT( aimstiff-devel 
                    GROUP devel
                    DESCRIPTION "Developpement files for aimstiff"
                    DEPENDS aimsdata-devel )
SOMA_ADD_COMPONENT( aimsnifti-devel 
                    GROUP devel
                    DESCRIPTION "Developpement files for aimsnifti"
                    DEPENDS aimsdata-devel )
add_dependencies( install-aimsdata-devel
                  install-aimsecat-devel
                  install-aimsply-devel
                  install-aimsvida-devel
                  install-aimsjpeg-devel
                  install-aimsdicom-devel
                  install-aimsminc-devel
                  install-aimstiff-devel
                  install-aimsnifti-devel )

SOMA_ADD_COMPONENT( aimsdata-html
                    GROUP html-doc
                    DESCRIPTION "HTML documentation of aimsdata" )
SOMA_ADD_COMPONENT( aimsdata-share
                    GROUP share
                    DESCRIPTION "Shared files of aimsdata" )

add_subdirectory( src )
SOMA_COPY_DIRECTORY( "${CMAKE_CURRENT_SOURCE_DIR}/shared" 
                      "share/aimsdata-${aimsdata_VERSION_MAJOR}.${aimsdata_VERSION_MINOR}"
                      aimsdata-share aimsdata-share )
