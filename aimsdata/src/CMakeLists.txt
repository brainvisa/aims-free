if( soma-infra_FOUND )

  add_subdirectory(aimsdata)
  add_subdirectory(aimsecat)
  add_subdirectory(aimsply)
  add_subdirectory(aimsvida)
  
  IF(APPLE)
    add_subdirectory(bundle_aimsecat)
    add_subdirectory(bundle_aimsply)
    add_subdirectory(bundle_aimsvida)
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
  
  
  add_subdirectory(aimsnifti)
  IF(APPLE)
    add_subdirectory(bundle_aimsnifti)
  ENDIF(APPLE)
  
  
  IF(QT_FOUND)
    add_subdirectory(aimsgui)
    add_subdirectory(aimsqtformats)
    IF(APPLE)
      add_subdirectory(bundle_aimsqtformats)
    ENDIF(APPLE)
  ENDIF(QT_FOUND)
  
  
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
      SOMA_GET_FILE_LIST_FROM_PRO( "${_pro}"
                                  TARGET _target
                                  SOURCES _proSources )
      set( _sources )
      foreach( _f ${_proSources} )
        set( _sources ${_sources} ${_command}/${_f} )
      endforeach( _f ${_proSources} )
      add_executable( ${_target} ${_sources} )
      target_link_libraries( ${_target} aimsdata )
      SOMA_INSTALL( TARGETS ${_target}
                    DESTINATION bin
                    COMPONENT aims )
    endif( IS_DIRECTORY "${_command}" )
  endforeach( _command )

else( soma-infra_FOUND )

  add_subdirectory(aimsdata)
  add_subdirectory(aimsecat)
  add_subdirectory(aimsply)
  add_subdirectory(aimsvida)
  
  IF(APPLE)
    add_subdirectory(bundle_aimsecat)
    add_subdirectory(bundle_aimsply)
    add_subdirectory(bundle_aimsvida)
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
  
  
  IF(NIFTI_FOUND)
    add_subdirectory(aimsnifti)
    IF(APPLE)
      add_subdirectory(bundle_aimsnifti)
    ENDIF(APPLE)
  ENDIF(NIFTI_FOUND)
  
  
  IF(QT_FOUND)
    add_subdirectory(aimsgui)
    add_subdirectory(aimsqtformats)
    IF(APPLE)
      add_subdirectory(bundle_aimsqtformats)
    ENDIF(APPLE)
  ENDIF(QT_FOUND)
  
  
  
  
  # List now the binaries depending on aimsdata
  
  add_subdirectory(AimsAverage)
  add_subdirectory(AimsComposeTransformation)
  add_subdirectory(AimsConnectedComp)
  add_subdirectory(AimsFileConvert)
  add_subdirectory(AimsFileInfo)
  add_subdirectory(AimsFlip)
  add_subdirectory(AimsGraphComplete)
  add_subdirectory(AimsGraphConvert)
  add_subdirectory(AimsGraphExtractTransformation)
  add_subdirectory(AimsGraphMerge)
  add_subdirectory(AimsInvertTransformation)
  add_subdirectory(AimsMassCenter)
  add_subdirectory(AimsMerge2Rgb)
  add_subdirectory(AimsMergeLabel)
  add_subdirectory(AimsMeshArea)
  add_subdirectory(AimsMeshCut)
  add_subdirectory(AimsMeshGenerate)
  add_subdirectory(AimsMeshTransform)
  add_subdirectory(AimsMeshes2Graph)
  add_subdirectory(AimsOverVolume)
  add_subdirectory(AimsRemoveNaN)
  add_subdirectory(AimsRoiFeatures)
  add_subdirectory(AimsSelectLabel)
  add_subdirectory(AimsSetMinf)
  add_subdirectory(AimsSplitRgb)
  add_subdirectory(AimsSubVolume)
  add_subdirectory(AimsTCat)
  add_subdirectory(AimsTex2Graph)
  add_subdirectory(AimsThreshold)
  add_subdirectory(AimsVectorFieldToMesh)
  
  add_subdirectory(examples)

endif( soma-infra_FOUND )

