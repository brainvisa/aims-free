add_executable( EcatVolMIProj
                EcatVolMIProj.c forward_proj3d_view.c )
target_link_libraries( EcatVolMIProj ecat+ m )
SOMA_INSTALL( TARGETS EcatVolMIProj
              DESTINATION bin
              COMPONENT ${PROJECT_NAME} )
