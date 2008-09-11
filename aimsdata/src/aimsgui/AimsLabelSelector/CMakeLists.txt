
getFromPro(labelselector.pro HEADERS SOURCES)

ADD_EXECUTABLE( AimsLabelSelector 
${SOURCES}
)

TARGET_LINK_LIBRARIES(AimsLabelSelector
aimsgui
)

INSTALL(TARGETS AimsLabelSelector DESTINATION bin/)
