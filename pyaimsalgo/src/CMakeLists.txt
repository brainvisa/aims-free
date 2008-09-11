if( soma-infra_FOUND )

#   add_subdirectory( libpyaimsalgo )
  add_subdirectory( sip )

else( soma-infra_FOUND )

  ADD_SUBDIRECTORY(libpyaimsalgo)
  ADD_SUBDIRECTORY(sip)

endif( soma-infra_FOUND )
