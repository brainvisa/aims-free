
SET(AimsAttributedViewer_SRCS
attributedViewer.cc
)

kde3_add_moc_files(AimsAttributedViewer_SRCS attributedViewer.h)


ADD_EXECUTABLE(AimsAttributedViewer
${AimsAttributedViewer_SRCS}
)


TARGET_LINK_LIBRARIES(AimsAttributedViewer
aimsgui
)

INSTALL(TARGETS AimsAttributedViewer DESTINATION bin/)

