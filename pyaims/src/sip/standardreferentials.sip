namespace aims
{

  class StandardReferentials
  {
%TypeHeaderCode
#include <aims/resampling/standardreferentials.h>
%End

  public:
    static std::string mniTemplateReferential();
%Docstring
  aims.StandardReferentials.mniTemplateReferential()

  identifer (str) for the MNI/ICBM space
%End

    static std::string mniTemplateReferentialID();
%Docstring
  aims.StandardReferentials.mniTemplateReferentialID()

  identifer (UUID str) for the MNI/ICBM space
%End

    static std::string acPcReferential();
%Docstring
  aims.StandardReferentials.acPcReferential()

  identifer (str) for the Taialairach/AIMS space
%End
    static std::string acPcReferentialID();
%Docstring
  aims.StandardReferentials.acPcReferentialID()

  identifer (UUID str) for the Talairach/AIMS space
%End
    static std::string talairachReferential();
%Docstring
  aims.StandardReferentials.talairachReferential()

  identifer (str) for the Talairach/CIFTI space
%End
    static std::string commonScannerBasedReferential();
    static std::string commonScannerBasedReferentialID();

    static const AffineTransformation3d & talairachToICBM();
    static const AffineTransformation3d & talairachToICBM2009cTemplate();
%Docstring
    trans = aims.StandardReferentials.talairachToICBM2009cTemplate()

    Transformation between the ICBM152 standard space and the ICBM152
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
%End

    static const carto::Object icbm2009cTemplateHeader();
%Docstring
    hdr = aims.StandardReferentials.icbm2009cTemplateHeader()
    Return a header with size and characteristics of the ICBM2009c MNI
    template volume.
%End
  };

};

