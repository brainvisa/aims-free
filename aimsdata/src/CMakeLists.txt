
add_subdirectory(aimsdata)
set(HEADERS ${AIMSDATA_HEADERS})

add_subdirectory(aimsgis)
if( BUILD_ECAT )
  add_subdirectory(aimsecat)
  set(HEADERS ${HEADERS} ${AIMSECAT_HEADERS})
endif( BUILD_ECAT )
if( NOT WIN32 )
  add_subdirectory(aimsvida)
  set(HEADERS ${HEADERS} ${AIMSVIDA_HEADERS})
endif( NOT WIN32 )
add_subdirectory(aimsply)
set(HEADERS ${HEADERS} ${AIMSPLY_HEADERS})
add_subdirectory(aimsfdf)
#add_subdirectory(aimsndp)
set(HEADERS ${HEADERS} ${AIMSFDF_HEADERS})
IF(APPLE)
  if( BUILD_ECAT )
    add_subdirectory(bundle_aimsecat)
  endif( BUILD_ECAT )
  add_subdirectory(bundle_aimsply)
  add_subdirectory(bundle_aimsvida)
  add_subdirectory(bundle_aimsfdf)
ENDIF(APPLE)


IF(JPEG_FOUND)
  add_subdirectory(aimsjpeg)
  set(HEADERS ${HEADERS} ${AIMSJPEG_HEADERS})
  IF(APPLE)
    add_subdirectory(bundle_aimsjpeg)
  ENDIF(APPLE)
ENDIF(JPEG_FOUND)


IF(MINC_FOUND)
  add_subdirectory(aimsminc)
  set(HEADERS ${HEADERS} ${AIMSMINC_HEADERS})
  IF(APPLE)
    add_subdirectory(bundle_aimsminc)
  ENDIF(APPLE)
ENDIF(MINC_FOUND)


IF(TIFF_FOUND AND ZLIB_FOUND)
  add_subdirectory(aimstiff)
  set(HEADERS ${HEADERS} ${AIMSTIFF_HEADERS})
  IF(APPLE)
    add_subdirectory(bundle_aimstiff)
  ENDIF(APPLE)
ENDIF()

IF(EXPAT_FOUND)
  add_subdirectory(aimsgifti)
  set(HEADERS ${HEADERS} ${AIMSGIFTI_HEADERS})
ENDIF(EXPAT_FOUND)

# list of header files for doxygen
set(HEADERS ${HEADERS} PARENT_SCOPE)
#------------------------------------------------------------------------------
# Build targets for commands
#------------------------------------------------------------------------------
foreach( _command 
          AimsAverage
          AimsChannelExtract
          AimsComposeTransformation
          AimsConnectedComp
          AimsFiberOversampler
          AimsFileConvert
          AimsFileInfo
          AimsFlip
          AimsGraphComplete
          AimsGraphConvert
          AimsGraphExtractTransformation
          AimsGraphMerge
          AimsInvertTransformation
          AimsLabels2Tex
          AimsMassCenter
          AimsMerge2Rgb
          AimsMergeLabel
          AimsMeshArea
          AimsMeshCut
          AimsMeshes2Graph
          AimsMeshGenerate
          AimsMeshTransform
          AimsOverVolume
          AimsRemoveNaN
          AimsRoiFeatures
          AimsSelectLabel
          AimsSetMinf
          AimsSplitRgb
          AimsSubVolume
          AimsSumSparseMatrix
          AimsTCat
          AimsTex2Graph
          AimsTex2Vol
          AimsThreshold
          AimsVectorFieldToMesh
          AimsVolumeExtract
)
  if( IS_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/${_command}" )
    file( GLOB _pro "${CMAKE_CURRENT_SOURCE_DIR}/${_command}/*.pro" )
    BRAINVISA_GET_FILE_LIST_FROM_PRO( "${_pro}"
                                TARGET _target
                                SOURCES _proSources )
    set( _sources )
    foreach( _f ${_proSources} )
      set( _sources ${_sources} ${_command}/${_f} )
    endforeach( _f ${_proSources} )
    BRAINVISA_ADD_EXECUTABLE( ${_target} ${_sources} )
    target_link_libraries( ${_target} aimsdata )
    BRAINVISA_INSTALL( TARGETS ${_target}
                  DESTINATION bin
                  COMPONENT ${PROJECT_NAME} )
  endif( IS_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/${_command}" )
endforeach( _command )

add_subdirectory( AimsBundle2Vol )
add_subdirectory( AimsBundleAnalysis )

add_subdirectory( tests )
