if( soma-infra_FOUND )

  SOMA_COPY_FILES( pyaimsalgo-bin aims 
                   AimsFoldsGraphThickness.py AimsTMSCone.py 
                   DESTINATION bin )

else( soma-infra_FOUND )
  
  ADD_CUSTOM_TARGET(pyaimsalgo-bin ALL)
  
  PYTHON_INSTALL(AimsFoldsGraphThickness.py pyaimsalgo-bin ${CMAKE_INSTALL_PREFIX}/bin)
  PYTHON_INSTALL(AimsTMSCone.py             pyaimsalgo-bin ${CMAKE_INSTALL_PREFIX}/bin)

endif( soma-infra_FOUND )
