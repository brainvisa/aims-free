BRAINVISA_ADD_EXECUTABLE(AimsT2BasedCorrectionEpi
contextEPI.cc
epiDistortion.cc
g2dSmoother.cc
g2dSmootherD.cc
gCoefficients.cc
gColumns.cc
gLines.cc
gRecursive.cc
interpolation.cc
main.cc
mutualInfo.cc
pdf.cc
)

TARGET_LINK_LIBRARIES(AimsT2BasedCorrectionEpi
aimsalgo
)

INSTALL(TARGETS AimsT2BasedCorrectionEpi DESTINATION bin/)
