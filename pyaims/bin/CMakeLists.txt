if( soma-infra_FOUND )

  SOMA_COPY_FILES( pyaims-bin aims 
                   cartoGraphCat.py cartoGraphTransform.py cartoLinearComb.py cartoSetMinf.py DESTINATION bin )

else( soma-infra_FOUND )

  ADD_CUSTOM_TARGET(pyaims_bin ALL)
  
  PYTHON_INSTALL(cartoGraphCat.py        pyaims_bin ${CMAKE_INSTALL_PREFIX}/bin)
  PYTHON_INSTALL(cartoGraphTransform.py  pyaims_bin ${CMAKE_INSTALL_PREFIX}/bin)
  PYTHON_INSTALL(cartoLinearComb.py      pyaims_bin ${CMAKE_INSTALL_PREFIX}/bin)
  PYTHON_INSTALL(cartoSetMinf.py         pyaims_bin ${CMAKE_INSTALL_PREFIX}/bin)

endif( soma-infra_FOUND )
