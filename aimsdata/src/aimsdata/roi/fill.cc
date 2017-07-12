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

#include <aims/roi/fill_d.h>

namespace aims
{

  template
  void floodFill( carto::Volume<int8_t> & vol, const Point3d & pos,
                  int8_t value, Connectivity::Type connectivity,
                  int8_t tolerence, bool verbose );
  template
  void floodFill( carto::Volume<uint8_t> & vol, const Point3d & pos,
                  uint8_t value, Connectivity::Type connectivity,
                  uint8_t tolerence, bool verbose );
  template
  void floodFill( carto::Volume<uint16_t> & vol, const Point3d & pos,
                  uint16_t value, Connectivity::Type connectivity,
                  uint16_t tolerence, bool verbose );
  template
  void floodFill( carto::Volume<int16_t> & vol, const Point3d & pos,
                  int16_t value, Connectivity::Type connectivity,
                  int16_t tolerence, bool verbose );
  template
  void floodFill( carto::Volume<int32_t> & vol, const Point3d & pos,
                  int32_t value, Connectivity::Type connectivity,
                  int32_t tolerence, bool verbose );
  template
  void floodFill( carto::Volume<uint32_t> & vol, const Point3d & pos,
                  uint32_t value, Connectivity::Type connectivity,
                  uint32_t tolerence, bool verbose );
  template
  void floodFill( carto::Volume<float> & vol, const Point3d & pos,
                  float value, Connectivity::Type connectivity,
                  float tolerence, bool verbose );
  template
  void floodFill( carto::Volume<double> & vol, const Point3d & pos,
                  double value, Connectivity::Type connectivity,
                  double tolerence, bool verbose );

}

