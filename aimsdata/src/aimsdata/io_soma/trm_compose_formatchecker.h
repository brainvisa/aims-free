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
#ifndef AIMS_IO_SOMA_TRM_COMPOSE_FORMATCHECKER_H
#define AIMS_IO_SOMA_TRM_COMPOSE_FORMATCHECKER_H

#include <soma-io/checker/formatchecker.h>
//----------------------------------------------------------------------------

namespace soma
{

  /** Read a transformation build from compising several transformation files

      An artificial ".trmc" extension is used to recognize this format.

      Files are separated with a "*" character. If some of them have options
      ("?option=value"), then the final composition should also have an option
      separator ("toto.trm?inv=1*tutu.nii.trmhdr.trmc?") in order to avoid the
      URL parser to parse everything after the last "?" character as options.
   */
  class TrmComposeFormatChecker : public FormatChecker
  {
  public:

    virtual DataSourceInfo check( DataSourceInfo dsi,
                                  DataSourceInfoLoader & f,
                                  carto::Object options = carto::none() )
                                  const;
    virtual ~TrmComposeFormatChecker() {}
  };


  /** Read a transformation build from compising several transformation files

      An artificial ".trmc" extension is used to recognize this format.

      Files are separated with a "*" character. If some of them have options
      ("?option=value"), then the final composition should also have an option
      separator ("toto.trm?inv=1*tutu.nii.trmhdr.trmc?") in order to avoid the
      URL parser to parse everything after the last "?" character as options.

      The difference with TrmComposeFormatChecker is that TrmChainFormatChecker will allow non-affine 3D transforms, and avertize the resulting object type as a Transformation3d (probably a TransformationChain3d)
   */
  class TrmChainFormatChecker : public FormatChecker
  {
  public:

    virtual DataSourceInfo check( DataSourceInfo dsi,
                                  DataSourceInfoLoader & f,
                                  carto::Object options = carto::none() )
                                  const;
    virtual ~TrmChainFormatChecker() {}
  };


}

#endif
