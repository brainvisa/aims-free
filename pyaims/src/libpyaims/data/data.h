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

#ifndef PYAIMS_DATA_DATA_H
#define PYAIMS_DATA_DATA_H

#include <cartodata/volume/volume.h>
#include <aims/rgb/rgb.h>
#include <aims/vector/vector.h>

typedef carto::Volume<int8_t> Volume_S8;
typedef carto::Volume<uint8_t> Volume_U8;
typedef carto::Volume<int16_t> Volume_S16;
typedef carto::Volume<uint16_t> Volume_U16;
typedef carto::Volume<int32_t> Volume_S32;
typedef carto::Volume<uint32_t> Volume_U32;
typedef carto::Volume<float> Volume_FLOAT;
typedef carto::Volume<double> Volume_DOUBLE;
typedef carto::Volume<AimsRGB> Volume_RGB;
typedef carto::Volume<AimsRGBA> Volume_RGBA;
typedef carto::Volume<Point3df> Volume_POINT3DF;

typedef carto::rc_ptr<carto::Volume<int8_t> > rc_ptr_Volume_S8;
typedef carto::rc_ptr<carto::Volume<uint8_t> > rc_ptr_Volume_U8;
typedef carto::rc_ptr<carto::Volume<int16_t> > rc_ptr_Volume_S16;
typedef carto::rc_ptr<carto::Volume<uint16_t> > rc_ptr_Volume_U16;
typedef carto::rc_ptr<carto::Volume<int32_t> > rc_ptr_Volume_S32;
typedef carto::rc_ptr<carto::Volume<uint32_t> > rc_ptr_Volume_U32;
typedef carto::rc_ptr<carto::Volume<float> > rc_ptr_Volume_FLOAT;
typedef carto::rc_ptr<carto::Volume<double> > rc_ptr_Volume_DOUBLE;
typedef carto::rc_ptr<carto::Volume<AimsRGB> > rc_ptr_Volume_RGB;
typedef carto::rc_ptr<carto::Volume<AimsRGBA> > rc_ptr_Volume_RGBA;
typedef carto::rc_ptr<carto::Volume<Point3df> > rc_ptr_Volume_POINT3DF;

#endif

