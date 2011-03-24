
SET(AimsAttributedViewer_SRCS
attributedViewer.cc
)

kde3_add_moc_files(AimsAttributedViewer_SRCS attributedViewer.h)

if( APPLE )
  set( _guiExecutable MACOSX_BUNDLE )
endif( APPLE )

ADD_EXECUTABLE(AimsAttributedViewer ${_guiExecutable}
${AimsAttributedViewer_SRCS}
)

TARGET_LINK_LIBRARIES(AimsAttributedViewer
aimsgui
)

if( APPLE )
  add_custom_command( TARGET AimsAttributedViewer POST_BUILD
    COMMAND "ln"
    ARGS "-sf"
    "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/AimsAttributedViewer.app/Contents/MacOS/AimsAttributedViewer"
    "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/AimsAttributedViewer"
  )
  BRAINVISA_INSTALL( FILES "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/AimsAttributedViewer"
              DESTINATION bin
              COMPONENT ${PROJECT_NAME} )
endif()

BRAINVISA_INSTALL( TARGETS AimsAttributedViewer
              DESTINATION bin
              COMPONENT ${PROJECT_NAME} )
