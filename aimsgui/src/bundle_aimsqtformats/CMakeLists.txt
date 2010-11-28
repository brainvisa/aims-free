add_library( bundle_aimsqtformats MODULE bundle/qtbundle.cc )
if( NOT APPLE )
  # Setting VERSION or SOVERSION on a MODULE cause a link error on MacOS 10.5 with cmake 2.6:
  #  i686-apple-darwin9-g++-4.0.1: -compatibility_version only allowed with -dynamiclib
  set_property( TARGET bundle_aimsqtformats PROPERTY VERSION ${aimsdata_VERSION} )
endif()
set_property( TARGET bundle_aimsqtformats PROPERTY OUTPUT_NAME aimsqtformats )
target_link_libraries( bundle_aimsqtformats aimsqtformats )
BRAINVISA_INSTALL( TARGETS bundle_aimsqtformats
              DESTINATION lib
              COMPONENT aims-gpl )
