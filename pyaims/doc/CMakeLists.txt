if( soma-infra_FOUND )

  SOMA_COPY_DIRECTORY( "${CMAKE_CURRENT_SOURCE_DIR}"
                       "share/doc/pyaims-${pyaims_VERSION_MAJOR}.${pyaims_VERSION_MINOR}"
                       pyaims-doc aims )

else( soma-infra_FOUND )

  SHFJ_COPY_CURRENT_DIRECTORY(share/doc/pyaims-${BrainVISA_VERSION})

endif( soma-infra_FOUND )
