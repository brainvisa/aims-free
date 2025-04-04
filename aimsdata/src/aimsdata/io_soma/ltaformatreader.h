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

#ifndef AIMS_IO_SOMA_LTAFORMATREADER_H
#define AIMS_IO_SOMA_LTAFORMATREADER_H

#include <soma-io/reader/formatreader.h>


namespace aims
{
  class AffineTransformation3d;
}

namespace soma
{
  class Transformation3d;

  /** .trm format for a transformation file

      Reader options:

      - inv (bool 0/1):
          if true, invert the transformation
      - apply_vs (bool 0/1), default: 1
          if true, apply source and dest volumes voxels size to be in mm
      - apply_lpi (bool 0/1), default: 1
          if true, apply AIMS LPI transformations
   */
  class LtaFormatReader : public FormatReader<aims::AffineTransformation3d>
  {
  public:
    //========================================================================
    //   N E W   M E T H O D S
    //========================================================================
    virtual aims::AffineTransformation3d*
    createAndRead( carto::rc_ptr<DataSourceInfo> dsi,
                   const AllocatorContext & context,
                   carto::Object options );
    virtual void read( aims::AffineTransformation3d & obj,
                       carto::rc_ptr<DataSourceInfo> dsi,
                       const AllocatorContext & context,
                       carto::Object options );
    virtual FormatReader<aims::AffineTransformation3d>* clone() const;
    virtual std::string formatID() const { return "XFM"; }
  };


  class LtaT3DFormatReader : public FormatReader<Transformation3d>
  {
  public:
    virtual Transformation3d*
    createAndRead( carto::rc_ptr<DataSourceInfo> dsi,
                   const AllocatorContext & context,
                   carto::Object options );
    Transformation3d* create( carto::Object header,
                              const AllocatorContext & context,
                              carto::Object options );
    virtual void read( Transformation3d & obj,
                       carto::rc_ptr<DataSourceInfo> dsi,
                       const AllocatorContext & context,
                       carto::Object options );
    virtual FormatReader<Transformation3d>* clone() const;
    virtual std::string formatID() const { return "XFM3D"; }
  };

}

#endif
