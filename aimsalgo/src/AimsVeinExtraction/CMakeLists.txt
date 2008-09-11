ADD_EXECUTABLE(AimsVeinExtraction
volume/volume.cc
geodesicminimalpath/geodesicminimalpath.cc
main.cc
)

TARGET_LINK_LIBRARIES(AimsVeinExtraction
aimsalgo
)

INSTALL(TARGETS AimsVeinExtraction DESTINATION bin/)
