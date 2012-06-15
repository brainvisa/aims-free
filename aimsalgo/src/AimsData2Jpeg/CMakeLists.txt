
BRAINVISA_ADD_EXECUTABLE(AimsData2Jpeg
main.cc
postscript.cc
)

TARGET_LINK_LIBRARIES(AimsData2Jpeg
aimsalgo
aimsjpeg
)

INSTALL(TARGETS AimsData2Jpeg DESTINATION bin/)
