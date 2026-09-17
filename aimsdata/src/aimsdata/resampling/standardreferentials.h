#ifndef AIMS_TRANSFORMATION_STANDARDREFERENTIALS_H
#define AIMS_TRANSFORMATION_STANDARDREFERENTIALS_H

#include <aims/transformation/affinetransformation3d.h>

namespace aims
{

  class StandardReferentials
  {
  public:
    static std::string mniTemplateReferential();
    static std::string acPcReferential();
    static std::string talairachReferential();
    static std::string commonScannerBasedReferential();
    static std::string mniTemplateReferentialID();
    static std::string acPcReferentialID();
    static std::string commonScannerBasedReferentialID();
    static std::string icbm2009cTemplateReferentialID();

    static const AffineTransformation3d & talairachToICBM();
    /** Transformation between the ICBM152 standard space and the ICBM152
        template space.

        includes shifts and axes inversions.

        This is the "most standard" field of view of the template image, the
        one from the ICBM2009c_nlin_asym template from the MNI. Actually we
        find various fields of view for the templates. The one we use here is
        (193, 229, 193).
        The transform to the "real" ICBM space (0 roughly at AC) is provided
        in the output transformation header.
        This space has a fixed transformation with our Aims Talairach, along
        with the template volume size and voxel size.
        This space has a fixed transformation with our Aims Talairach.
    */
    static const AffineTransformation3d & talairachToICBM2009cTemplate();
    /** Return a header with size and characteristics of the ICBM2009c MNI
        template volume.
    */
    static const carto::Object icbm2009cTemplateHeader();
    /** try to identify referential ID from its string name or description.

        for instance "Talairach-MNI template-SPM" will be translated to its
        AIMS uuid, "803552a6-ac4d-491d-99f5-b938392b674b"

        If commonScannerBased is true, then "Scanner-based anatomical
        coordinates" will be translated to the value of
        commonScannerBasedReferentialID(), so that other ref names
        of the same kind will match. If commonScannerBased is false, then
        a scanner-based ref name will be either translated to a new UUID, or
        returned unchanged or with a suffix, depending on the values of
        genNewIds (see below) and commonSuffix. If commonSuffix has a value, it
        will be appended after the name "Scanner-based anatomical coordinates".

        if genNewIds is true, then:
        if the refName is an unkwnown referential name (and not an UUID), then
        a new UUID will be generated for it, each time this function is called:
        so calling it twice with the same name will result in two distinct
        IDs.
        if genNewIds is false, and the refName is not recognized, then it
        will be returned unchanged.
    */
    static std::string referentialID( const std::string & refName,
                                      bool commonScannerBased = false,
                                      bool genNewIds = true,
                                      const std::string & commonSuffix = "" );
    static bool isUUID( const std::string & refName );
  };

}

#endif

