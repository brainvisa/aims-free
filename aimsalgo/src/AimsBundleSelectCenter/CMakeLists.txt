
BRAINVISA_ADD_EXECUTABLE( AimsBundleSelectCenter
    bundleSelectCenter.cc
)

TARGET_LINK_LIBRARIES( AimsBundleSelectCenter
    aimsalgo
)

BRAINVISA_INSTALL( TARGETS AimsBundleSelectCenter DESTINATION bin
                   COMPONENT ${PROJECT_NAME} )
