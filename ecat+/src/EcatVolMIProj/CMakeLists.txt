if( soma-infra_FOUND )

  add_executable( EcatVolMIProj
                  EcatVolMIProj.c forward_proj3d_view.c )
  target_link_libraries( EcatVolMIProj ecat+ m )
  SOMA_INSTALL( TARGETS EcatVolMIProj
                DESTINATION bin
                COMPONENT ecat+ )

else( soma-infra_FOUND )

  ADD_EXECUTABLE(EcatVolMIProj
  EcatVolMIProj.c
  forward_proj3d_view.c
  )
  
  TARGET_LINK_LIBRARIES(EcatVolMIProj
  ecat+
  m
  )
  
INSTALL(TARGETS EcatVolMIProj DESTINATION bin/)

endif( soma-infra_FOUND )
