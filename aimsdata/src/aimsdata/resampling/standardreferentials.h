/* This software and supporting documentation are distributed by
 *     Institut Federatif de Recherche 49
 *     CEA/NeuroSpin, Batiment 145,
 *     91191 Gif-sur-Yvette cedex
 *     France
 *
 * This software is governed by the CeCILL-B license under
 * French law and abiding by the rules of distribution of free software.
 * You can  use, modify and/or redistribute the software under the
 * terms of the CeCILL-B license as circulated by CEA, CNRS
 * and INRIA at the following URL "http://www.cecill.info".
 *
 * As a counterpart to the access to the source code and  rights to copy,
 * modify and redistribute granted by the license, users are provided only
 * with a limited warranty  and the software's author,  the holder of the
 * economic rights,  and the successive licensors  have only  limited
 * liability.
 *
 * In this respect, the user's attention is drawn to the risks associated
 * with loading,  using,  modifying and/or developing or reproducing the
 * software by the user in light of its specific status of free software,
 * that may mean  that it is complicated to manipulate,  and  that  also
 * therefore means  that it is reserved for developers  and  experienced
 * professionals having in-depth computer knowledge. Users are therefore
 * encouraged to load and test the software's suitability as regards their
 * requirements in conditions enabling the security of their systems and/or
 * data to be ensured and,  more generally, to use and operate it in the
 * same conditions as regards security.
 *
 * The fact that you are presently reading this means that you have had
 * knowledge of the CeCILL-B license and that you accept its terms.
 */

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
  };

}

#endif

