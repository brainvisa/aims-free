ADD_SUBDIRECTORY( library )
ADD_SUBDIRECTORY( tests )
ADD_SUBDIRECTORY( cartogis )
ADD_SUBDIRECTORY( cartonifti )
ADD_SUBDIRECTORY( cartomif )
IF( OPENSLIDE_FOUND )
  ADD_SUBDIRECTORY( cartoopenslide )
ENDIF()
if( DCMTK_FOUND AND TIFF_FOUND AND PNG12_FOUND )
  ADD_SUBDIRECTORY( cartodicom )
ENDIF()
if( TIFF_FOUND )
  ADD_SUBDIRECTORY( cartotiff )
ENDIF()
if( MINC_FOUND )
  ADD_SUBDIRECTORY( cartominc )
ENDIF()

