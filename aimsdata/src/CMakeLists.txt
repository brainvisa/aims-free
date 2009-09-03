add_subdirectory(aimsdata)
if( NOT WIN32 )
  add_subdirectory(aimsecat)
  add_subdirectory(aimsvida)
endif( NOT WIN32 )
add_subdirectory(aimsply)
add_subdirectory(aimsnifti)
add_subdirectory(aimsfdf)

IF(APPLE)
  add_subdirectory(bundle_aimsecat)
  add_subdirectory(bundle_aimsply)
  add_subdirectory(bundle_aimsvida)
  add_subdirectory(bundle_aimsnifti)
ENDIF(APPLE)


IF(JPEG_FOUND)
  add_subdirectory(aimsjpeg)
  IF(APPLE)
    add_subdirectory(bundle_aimsjpeg)
  ENDIF(APPLE)
ENDIF(JPEG_FOUND)


IF(DCMTK_FOUND)
  add_subdirectory(aimsdicom)
  IF(APPLE)
    add_subdirectory(bundle_aimsdicom)
  ENDIF(APPLE)
ENDIF(DCMTK_FOUND)


IF(MINC_FOUND)
  add_subdirectory(aimsminc)
  IF(APPLE)
    add_subdirectory(bundle_aimsminc)
  ENDIF(APPLE)
ENDIF(MINC_FOUND)


IF(TIFF_FOUND)
  add_subdirectory(aimstiff)
  IF(APPLE)
    add_subdirectory(bundle_aimstiff)
  ENDIF(APPLE)
ENDIF(TIFF_FOUND)

IF(EXPAT_FOUND)
  add_subdirectory(aimsgifti)
ENDIF(EXPAT_FOUND)


#------------------------------------------------------------------------------
# Build targets for commands
#------------------------------------------------------------------------------
foreach( _command 
          AimsMeshTransform
          AimsGraphMerge
          AimsThreshold
          AimsSelectLabel
          AimsAverage
          AimsSubVolume
          AimsInvertTransformation
          AimsMerge2Rgb
          AimsVectorFieldToMesh
          AimsConnectedComp
          AimsMeshGenerate
          AimsRemoveNaN
          AimsMassCenter
          AimsGraphComplete
          AimsGraphExtractTransformation
          AimsMergeLabel
          AimsRoiFeatures
          AimsFlip
          AimsTCat
          AimsMeshCut
          AimsComposeTransformation
          AimsOverVolume
          AimsSetMinf
          AimsMeshes2Graph
          AimsFileConvert
          AimsFileInfo
          AimsMeshArea
          AimsGraphConvert
          AimsTex2Graph
          AimsSplitRgb )
  if( IS_DIRECTORY "${_command}" )
    file( GLOB _pro "${_command}/*.pro" )
    BRAINVISA_GET_FILE_LIST_FROM_PRO( "${_pro}"
                                TARGET _target
                                SOURCES _proSources )
    set( _sources )
    foreach( _f ${_proSources} )
      set( _sources ${_sources} ${_command}/${_f} )
    endforeach( _f ${_proSources} )
    add_executable( ${_target} ${_sources} )
    target_link_libraries( ${_target} aimsdata )
    BRAINVISA_INSTALL( TARGETS ${_target}
                  DESTINATION bin
                  COMPONENT ${PROJECT_NAME} )
  endif( IS_DIRECTORY "${_command}" )
endforeach( _command )
