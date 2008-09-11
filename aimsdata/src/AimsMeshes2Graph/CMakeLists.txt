
ADD_EXECUTABLE(AimsMeshes2Graph
meshes2graph.cc
)

TARGET_LINK_LIBRARIES(AimsMeshes2Graph
aimsdata
)

INSTALL(TARGETS AimsMeshes2Graph DESTINATION bin/)
