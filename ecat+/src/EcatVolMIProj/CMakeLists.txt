BRAINVISA_ADD_EXECUTABLE( EcatVolMIProj
                          EcatVolMIProj.c forward_proj3d_view.c )
target_link_libraries( EcatVolMIProj ecat+ m )
BRAINVISA_INSTALL( TARGETS EcatVolMIProj
              DESTINATION bin
              COMPONENT ${PROJECT_NAME} )
