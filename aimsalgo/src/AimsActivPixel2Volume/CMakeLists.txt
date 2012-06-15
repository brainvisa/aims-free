
BRAINVISA_ADD_EXECUTABLE(AimsActivPixel2Volume
main.cc
pix2vol.cc
)

TARGET_LINK_LIBRARIES(AimsActivPixel2Volume
aimsalgo
)

INSTALL(TARGETS AimsActivPixel2Volume DESTINATION bin/)

