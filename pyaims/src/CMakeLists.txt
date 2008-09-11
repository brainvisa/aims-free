if( soma-infra_FOUND )

  add_subdirectory( libpyaims )
  add_subdirectory( sip )
  if( QT_FOUND )
    add_subdirectory( sip_gui )
  endif( QT_FOUND )

else( soma-infra_FOUND )

  ADD_SUBDIRECTORY(libpyaims)
  ADD_SUBDIRECTORY(sip)
  IF(QT_FOUND)
    ADD_SUBDIRECTORY(sip_gui)
  ENDIF(QT_FOUND)

endif( soma-infra_FOUND )
