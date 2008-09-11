if( soma-infra_FOUND )

  add_executable( EcatVolReverse EcatVolReverse.c )
  target_link_libraries( EcatVolReverse ecat+ m )
  SOMA_INSTALL( TARGETS EcatVolReverse
                DESTINATION bin
                COMPONENT ecat+ )

else( soma-infra_FOUND )

  ADD_EXECUTABLE(EcatVolReverse
  EcatVolReverse.c
  )
  
  TARGET_LINK_LIBRARIES(EcatVolReverse
  ecat+
  m
  )
  
  INSTALL(TARGETS EcatVolReverse DESTINATION bin/)

endif( soma-infra_FOUND )
