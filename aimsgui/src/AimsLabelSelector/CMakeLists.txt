
getFromPro(labelselector.pro HEADERS SOURCES)

if( APPLE )
  set( _guiExecutable MACOSX_BUNDLE )
endif( APPLE )

ADD_EXECUTABLE( AimsLabelSelector ${_guiExecutable}
${SOURCES}
)

TARGET_LINK_LIBRARIES(AimsLabelSelector
aimsgui
)

if( APPLE )
  add_custom_command( TARGET AimsLabelSelector POST_BUILD
    COMMAND "ln"
    ARGS "-sf"
    "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/AimsLabelSelector.app/Contents/MacOS/AimsLabelSelector"
    "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/AimsLabelSelector"
  )
  BRAINVISA_INSTALL( FILES "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/AimsLabelSelector"
              DESTINATION bin
              COMPONENT ${PROJECT_NAME} )
endif()

BRAINVISA_INSTALL( TARGETS AimsLabelSelector
              DESTINATION bin
              COMPONENT ${PROJECT_NAME} )
