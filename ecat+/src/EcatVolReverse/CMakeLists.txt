BRAINVISA_ADD_EXECUTABLE( EcatVolReverse EcatVolReverse.c )
target_link_libraries( EcatVolReverse ecat+ m )
BRAINVISA_INSTALL( TARGETS EcatVolReverse
              DESTINATION bin
              COMPONENT ${PROJECT_NAME} )
