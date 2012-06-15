
BRAINVISA_ADD_EXECUTABLE(AimsCorrectionEpi
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


TARGET_LINK_LIBRARIES(AimsCorrectionEpi
aimsalgo
)

INSTALL(TARGETS AimsCorrectionEpi DESTINATION bin/)

