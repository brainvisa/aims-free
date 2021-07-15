BRAINVISA_ADD_EXECUTABLE(AimsMeshCleaner
meshCleaner.cc
)

TARGET_LINK_LIBRARIES(AimsMeshCleaner
aimsalgo
)

INSTALL(TARGETS AimsMeshCleaner DESTINATION bin/)
