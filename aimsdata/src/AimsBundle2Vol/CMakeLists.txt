
BRAINVISA_ADD_EXECUTABLE( AimsBundle2Vol
    bundle2vol.cc
)

TARGET_LINK_LIBRARIES( AimsBundle2Vol
    aimsdata
)

INSTALL( TARGETS AimsBundle2Vol DESTINATION bin/ )
