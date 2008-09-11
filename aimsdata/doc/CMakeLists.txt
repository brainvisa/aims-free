if( soma-infra_FOUND )

  SOMA_COPY_DIRECTORY( "${CMAKE_CURRENT_SOURCE_DIR}"
                       "share/doc/aimsdata-${aimsdata_VERSION_MAJOR}.${aimsdata_VERSION_MINOR}"
                       aimsdata-html aimsdata-html )
  

else( soma-infra_FOUND )

  SHFJ_COPY_CURRENT_DIRECTORY(share/doc/aimsdata-${BrainVISA_VERSION})

endif( soma-infra_FOUND )
