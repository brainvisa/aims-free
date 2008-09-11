if( soma-infra_FOUND )

  SOMA_COPY_PYTHON_DIRECTORY( ${CMAKE_CURRENT_SOURCE_DIR} pyaimsalgo-python aims )

else( soma-infra_FOUND )

  INCLUDE( SomaMacros )
  SOMA_COPY_PYTHON_DIRECTORY( ${CMAKE_CURRENT_SOURCE_DIR} pyaimsalgo_python )

endif( soma-infra_FOUND )
