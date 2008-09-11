
ADD_EXECUTABLE(AimsTex2Graph
tex2graph.cc
)

TARGET_LINK_LIBRARIES(AimsTex2Graph
aimsdata
)

INSTALL(TARGETS AimsTex2Graph DESTINATION bin/)
