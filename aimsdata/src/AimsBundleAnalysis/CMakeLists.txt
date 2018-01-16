
BRAINVISA_ADD_EXECUTABLE( AimsBundleAnalysis
    bundleAnalysis.cc
)

TARGET_LINK_LIBRARIES( AimsBundleAnalysis
    aimsdata
)

INSTALL( TARGETS AimsBundleAnalysis DESTINATION bin/ )
