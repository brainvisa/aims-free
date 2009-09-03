BRAINVISA_GET_FILE_LIST_FROM_PRO( "${CMAKE_CURRENT_SOURCE_DIR}/rfdf.pro"
                            TARGET _target
                            SOURCES _sources )
add_executable( ${_target} ${_sources} )
target_link_libraries( ${_target} aimsfdf )
BRAINVISA_INSTALL( TARGETS ${_target}
              DESTINATION bin
              COMPONENT ${PROJECT_NAME} )

